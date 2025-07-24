// data_collector.cpp
#include "data_collector.h"

/**
 * @brief 执行一次完整的水质数据采集与处理
 * @details 从PCF8591 ADC模块读取模拟传感器数据（浊度、pH值），
 *          从DS18B20读取温度数据，经过转换后更新到全局水质数据单例中。
 */
void DataCollector::collectData() {
    // 定义需要读取的ADC通道（0：浊度传感器，1：pH传感器）
    int channels[] = {0, 1};
    // 计算通道数量（数组元素个数）
    int numChannels = sizeof(channels) / sizeof(int);
    // 存储ADC读取结果的数组（与通道顺序对应）
    int results[numChannels];

    // 从PCF8591 ADC模块读取指定通道的模拟数据
    pcf8591.readMultiple(channels, numChannels, results);

    // 处理浊度数据：将ADC原始值（0-255）转换为浊度百分比（0-100%）
    // 公式说明：ADC值越小（透光率越高），浊度越低；值越大（透光率越低），浊度越高
    WaterQuality::getInstance().setTurbidity(100 - results[0] * 100.0 / 255);

    // 读取DS18B20温度传感器数据，并更新到水质数据中
    WaterQuality::getInstance().setDS18B20(ds18b20.readTemperature());

    // 处理pH值数据：将ADC原始值（0-255）转换为pH值（0-14）
    // 公式说明：假设传感器输出与pH值成反比（ADC值越大，pH值越小），满量程对应pH 0-14
    WaterQuality::getInstance().setpH(14.0 - results[1] * 14.0 / 255.0);
}