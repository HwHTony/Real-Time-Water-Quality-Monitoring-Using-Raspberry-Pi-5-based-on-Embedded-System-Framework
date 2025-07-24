/**
 * @file socket_info_updater.h
 * @brief 网络套接字信息更新器类接口，负责通过网络发送水质数据
 * @details 定义了通过已连接的套接字将实时水质监测数据发送到服务器的功能，
 *          继承自InfoUpdater抽象基类，实现了网络数据发送的具体逻辑，
 *          是系统中"数据输出到网络"的核心模块。
 */

#ifndef SOCKET_INFO_UPDATER_H
#define SOCKET_INFO_UPDATER_H

#include "../common/com.h"              // 公共类型及工具函数定义
#include "../common/water_quality.h"    // 水质数据单例类，用于获取待发送数据
#include "info_updater.h"               // 信息更新器基类，提供统一更新接口

/**
 * @class SocketInfoUpdater
 * @brief 基于套接字的信息更新器，负责将实时水质数据通过网络发送到服务器
 * @details 继承自InfoUpdater抽象基类，通过构造函数接收已连接的套接字描述符，
 *          在重写的update方法中实现数据的获取、格式化和发送，通常与定时器配合
 *          周期性（如1秒/次）向服务器推送最新数据。
 */
class SocketInfoUpdater: public InfoUpdater {
private:
    int sock;  ///< 已建立连接的套接字描述符，用于与服务器通信，由构造函数传入

public:
    /**
     * @brief 构造函数，初始化套接字成员
     * @param s 已连接的套接字描述符（需提前通过Socket::connectToServer创建）
     * @note 套接字必须处于已连接状态，否则后续update方法可能发送失败
     */
    SocketInfoUpdater(int s);

    /**
     * @brief 重写基类纯虚方法，执行网络数据发送
     * @details 从WaterQuality单例中获取最新的水质数据（温度、pH值、浊度等），
     *          按预设格式（如字符串、JSON等）格式化后，通过sock套接字发送到服务器，
     *          若发送失败会输出错误信息（具体错误处理逻辑由实现决定）。
     */
    void update() override;
};

#endif // SOCKET_INFO_UPDATER_H