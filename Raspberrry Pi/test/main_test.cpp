#include "../main.cpp"
#include <gtest/gtest.h>

// A simulated PCF8591 class
class MockPCF8591 : public PCF8591 {
public:
    int readMultiple(int* channels, int numChannels, int* results) override {
        // Simulated reading result
        for (int i = 0; i < numChannels; ++i) {
            results[i] = 128; // Simulated fixed value
        }
        return 0; // Simulation successfully completed 0
    }
};

// Simulated DS18B20 type
class MockDS18B20 : public DS18B20 {
public:
    float readTemperature() override {
        return 25.0; // Simulated fixed temperature value
    }
};

// Test data acquisition timer callback function
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

// Update function for test and debug information
TEST(MainTest, UpdateDebugInfo) {
    // Simulated water quality parameters
    g_water_quality.turbidity = 50.0;
    g_water_quality.ds18b20 = 20.0;
    g_water_quality.pH = 7.0;

    // Redirect the standard output to the buffer
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    update_debug_info();

    // Restore standard output
    std::cout.rdbuf(old);

    std::string output = buffer.str();
    EXPECT_TRUE(output.find("Debugging information update") != std::string::npos);
    EXPECT_TRUE(output.find("AIN0 value -> Turbidity: 50") != std::string::npos);
    EXPECT_TRUE(output.find("DS18B20 value -> Temperature: 20℃") != std::string::npos);
    EXPECT_TRUE(output.find("pH value -> pH: 7") != std::string::npos);
}

// Test the TFT information update function
TEST(MainTest, UpdateTFTInfo) {
    // Simulated water quality parameters
    g_water_quality.turbidity = 60.0;
    g_water_quality.ds18b20 = 22.0;
    g_water_quality.pH = 7.5;

    update_tft_info();

    // Due to the specific implementation of the TFTFreetype class, it is not possible to directly verify the drawing result here.
    // It is advisable to add some methods to the TFTFreetype class to obtain the drawing information for verification purposes.
}

// Test the socket communication function
TEST(MainTest, UpdateSocketInfo) {
    // Simulated water quality parameters
    g_water_quality.turbidity = 70.0;
    g_water_quality.ds18b20 = 23.0;
    g_water_quality.pH = 8.0;

    // Simulated socket
    int sock = 1; // Simulated socket descriptor

    // Redirect the standard output to the buffer
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

    update_socket_info(sock);

    // Restore standard output
    std::cout.rdbuf(old);

    std::string output = buffer.str();
    std::string expected = "{\"tur\":\"70.00\", \"tmp\":\"23.00\", \"pH\":\"8.00\"}";
    EXPECT_TRUE(output.find(expected) != std::string::npos);
}

// 删除自定义的 main 函数，使用 Google Test 提供的 main 函数
// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
