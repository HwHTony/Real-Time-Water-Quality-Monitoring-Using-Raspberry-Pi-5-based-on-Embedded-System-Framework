/**
 * @file tft_info_updater.h
 * @brief TFT屏幕信息更新器类接口，负责将水质数据显示在TFT屏幕上
 * @details 该文件定义了用于TFT屏幕显示更新的具体实现类，继承自InfoUpdater抽象基类，
 *          通过TFTFreetype驱动实现水质数据（如浊度、温度、pH值）的屏幕实时刷新，
 *          是系统中可视化展示数据的核心模块。
 */

#ifndef TFT_INFO_UPDATER_H
#define TFT_INFO_UPDATER_H

#include "../common/com.h"              // 公共类型及工具函数定义
#include "../common/water_quality.h"    // 水质数据单例类，用于获取待显示数据
#include "../display/tft_freetype.h"    // TFT屏幕及字体驱动，提供显示控制接口
#include "info_updater.h"               // 信息更新器基类，定义统一更新接口

/**
 * @class TFTInfoUpdater
 * @brief TFT屏幕信息更新器，负责将实时水质数据显示在TFT屏幕上
 * @details 继承自InfoUpdater抽象基类，通过TFTFreetype对象控制屏幕显示，
 *          重写update方法实现水质数据（浊度、温度、pH值等）的格式化与屏幕刷新，
 *          通常与定时器配合周期性（如1秒/次）更新显示内容，确保数据实时性。
 */
class TFTInfoUpdater : public InfoUpdater{
private:
    TFTFreetype tft;                     ///< TFT屏幕及字体控制器，用于执行实际显示操作
    static const int BUFFER_SIZE = 20;   ///< 字符串缓冲区大小，用于格式化待显示文本
    wchar_t turb[BUFFER_SIZE] = {0};     ///< 存储浊度信息的宽字符缓冲区，用于屏幕显示

public:
    /**
     * @brief 重写基类纯虚方法，执行TFT屏幕显示更新
     * @details 从WaterQuality单例中获取最新水质数据（浊度、温度、pH值），
     *          格式化为人可读的宽字符串（如"浊度：XX%"），通过tft对象绘制到屏幕指定位置，
     *          实现TFT屏幕的实时数据展示。
     */
    void update() override;
};

#endif // TFT_INFO_UPDATER_H