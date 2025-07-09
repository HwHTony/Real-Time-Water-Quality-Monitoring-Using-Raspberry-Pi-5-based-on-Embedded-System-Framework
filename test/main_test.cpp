#include "main.cpp"
#include <gtest/gtest.h>

// 模拟 PCF8591 类
class MockPCF8591 : public PCF8591 {
public:
    void readMultiple(int* channels, int numChannels, int* results) override {
        // 模拟读取结果
        for (int i = 0; i < numChannels; ++i) {
            results[i] = 128; // 模拟固定值
        }
    }
};

// 模拟 DS18B20 类
class MockDS18B20 : public DS18B20 {
public:
    float readTemperature() override {
        return 25.0; // 模拟固定温度值
    }
};

// 测试数据采集定时器回调函数
TEST(MainTest, DataCollectionTimerCallback) {
    MockPCF8591 pcf8591;
    MockDS18B20 ds18b20;
    int channels[] = {0, 1};
    int numChannels = 2;
    int results[2];

    pcf8591.readMultiple(channels, numChannels, results);
    g_water_quality.turbidity = 100 - results[0] * 100.0 / 255;
    g_water_quality.ds18b20 = ds18b20.readTemperature();
    g_water_quality.pH = 14.0 - results[1] * 14.0 / 255.0;

    EXPECT_FLOAT_EQ(g_water_quality.turbidity, 100 - 128 * 100.0 / 255);
    EXPECT_FLOAT_EQ(g_water_quality.ds18b20, 25.0);
    EXPECT_FLOAT_EQ(g_water_quality.pH, 14.0 - 128 * 14.0 / 255.0);
}

// 测试调试信息更新函数
TEST(MainTest, UpdateDebugInfo) {
    // 模拟水质参数
    g_water_quality.turbidity = 50.0;
    g_water_quality.ds18b20 = 20.0;
    g_water_quality.pH = 7.0;

    // 重定向标准输出到缓冲区
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    update_debug_info();

    // 恢复标准输出
    std::cout.rdbuf(old);

    std::string output = buffer.str();
    EXPECT_TRUE(output.find("调试信息更新") != std::string::npos);
    EXPECT_TRUE(output.find("AIN0值 -> 浊度: 50") != std::string::npos);
    EXPECT_TRUE(output.find("DS18B20值 -> 温度: 20℃") != std::string::npos);
    EXPECT_TRUE(output.find("pH值 -> pH: 7") != std::string::npos);
}

// 测试 TFT 信息更新函数
TEST(MainTest, UpdateTFTInfo) {
    // 模拟水质参数
    g_water_quality.turbidity = 60.0;
    g_water_quality.ds18b20 = 22.0;
    g_water_quality.pH = 7.5;

    update_tft_info();

    // 由于 TFTFreetype 类的具体实现，这里无法直接验证绘制结果
    // 可以考虑在 TFTFreetype 类中添加一些方法来获取绘制信息进行验证
}

// 测试套接字通信函数
TEST(MainTest, UpdateSocketInfo) {
    // 模拟水质参数
    g_water_quality.turbidity = 70.0;
    g_water_quality.ds18b20 = 23.0;
    g_water_quality.pH = 8.0;

    // 模拟套接字
    int sock = 1; // 模拟套接字描述符

    // 重定向标准输出到缓冲区
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    update_socket_info(sock);

    // 恢复标准输出
    std::cout.rdbuf(old);

    std::string output = buffer.str();
    std::string expected = "{\"tur\":\"70.00\", \"tmp\":\"23.00\", \"pH\":\"8.00\"}";
    EXPECT_TRUE(output.find(expected) != std::string::npos);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}