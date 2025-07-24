/**
 * @file info_updater.h
 * @brief 信息更新器抽象基类接口，定义统一的信息更新规范
 * @details 该文件声明了所有信息更新器的基类，通过纯虚函数强制子类实现具体的更新逻辑，
 *          实现了"接口与实现分离"，便于扩展不同类型的信息更新功能（如调试输出、TFT显示、网络发送等）。
 */

#ifndef INFO_UPDATER_H
#define INFO_UPDATER_H

/**
 * @class InfoUpdater
 * @brief 信息更新器抽象基类，定义信息更新的统一接口
 * @details 作为所有具体信息更新器的父类，提供纯虚的`update`方法，
 *          子类需根据自身功能实现该方法（如刷新TFT屏幕、打印调试信息、发送网络数据等）。
 *          采用接口设计模式，确保系统中不同更新模块的行为一致性。
 */
class InfoUpdater {
public:
    /**
     * @brief 纯虚方法，执行具体的信息更新操作
     * @details 子类必须实现此方法，用于完成自身职责的信息更新，
     *          例如：调试信息更新器输出到控制台，TFT更新器刷新屏幕显示，
     *          网络更新器发送数据到服务器等。
     */
    virtual void update() = 0;

    /**
     * @brief 虚析构函数，确保子类析构时正确释放资源
     * @details 为基类定义虚析构函数，避免删除子类对象时因析构函数未被正确调用而导致内存泄漏。
     */
    virtual ~InfoUpdater() {}
};

#endif // INFO_UPDATER_H