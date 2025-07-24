/**
 * @file debug_info_updater.h
 * @brief 调试信息更新器类的头文件，负责将水质数据输出到调试接口（如控制台）
 * @details 该类继承自信息更新器基类，实现调试场景下的水质数据展示逻辑，
 *          通常用于开发阶段的实时数据观测和问题排查。
 */

#ifndef DEBUG_INFO_UPDATER_H
#define DEBUG_INFO_UPDATER_H

#include "../common/com.h"              // 公共组件及类型定义
#include "../common/water_quality.h"    // 水质数据单例类，用于获取实时监测数据
#include "info_updater.h"               // 信息更新器基类，定义统一的更新接口

/**
 * @class DebugInfoUpdater
 * @brief 调试信息更新器，用于将实时水质数据输出到调试终端（如控制台）
 * @details 继承自InfoUpdater基类，重写update方法实现调试信息的刷新逻辑，
 *          通常以文本形式打印当前浊度、温度、pH值等关键参数，方便开发调试。
 */
class DebugInfoUpdater : public InfoUpdater {
public:
    /**
     * @brief 重写基类方法，执行调试信息更新
     * @details 从WaterQuality单例中获取最新水质数据，格式化后输出到标准输出流（控制台），
     *          一般与定时器配合周期性调用（如1秒/次），实现实时数据监控。
     */
    void update() override;
};

#endif // DEBUG_INFO_UPDATER_H