/**
 * @file data_collector.h
 * @brief 水质数据采集模块的核心接口
 * @details 定义了从各类传感器采集环境数据的功能抽象，
 *          包括温度、pH值、溶解氧等水质参数的获取。
 */

#ifndef DATA_COLLECTOR_H
#define DATA_COLLECTOR_H

#include "../common/com.h"          // 通信协议与基础类型定义
#include "../data_collection/pcf8591.h"  // ADC转换器驱动
#include "../data_collection/ds18b20.h"  // 温度传感器驱动
#include "../common/water_quality.h"  // 水质数据结构定义

/**
 * @class DataCollector
 * @brief 水质数据采集器，负责协调多个传感器工作
 * @details 通过组合不同类型的传感器驱动，实现对水质参数的周期性采集，
 *          并将原始数据转换为标准的水质指标。
 */
class DataCollector {
private:
    PCF8591 pcf8591;  // 模拟信号采集器（ADC），用于pH值、浊度等传感器
    DS18B20 ds18b20;  // 数字温度传感器，提供高精度水温测量

public:
    /**
     * @brief 执行一次完整的数据采集周期
     * @details 按预设顺序从所有传感器读取数据，进行单位转换和校准，
     *          并将结果存储到全局数据结构中。
     * @note 调用频率应根据传感器特性和系统需求调整，
     *       通常建议不低于1Hz以捕获水质变化。
     * @see WaterQuality
     */
    void collectData();
};

#endif // DATA_COLLECTOR_H