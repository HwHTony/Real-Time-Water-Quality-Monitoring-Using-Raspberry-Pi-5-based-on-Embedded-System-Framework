#ifndef WATER_QUALITY_H
#define WATER_QUALITY_H

/**
 * @brief 水质监测数据的单例类（饿汉式实现）
 * 
 * 该类采用饿汉式单例模式设计，在程序启动时立即创建实例，
 * 确保线程安全且无需考虑内存泄漏问题。用于存储和管理水质监测系统中的各项参数，
 * 包括浊度、pH值和温度。系统中任何模块都可以通过该类获取和更新当前的水质数据。
 */
class WaterQuality {
private:
    float turbidity;  ///< 浊度值，单位根据传感器而定
    float pH;         ///< pH值，反映水体酸碱度
    float ds18b20;    ///< DS18B20温度传感器测得的温度值，单位为摄氏度

    /**
     * @brief 私有构造函数
     * 
     * 初始化所有水质参数为0，并确保该类不能被外部实例化。
     */
    WaterQuality() : turbidity(0), pH(0), ds18b20(0) {}

    /**
     * @brief 禁用拷贝构造函数
     * 
     * 防止单例实例被复制
     */
    WaterQuality(const WaterQuality&) = delete;

    /**
     * @brief 禁用赋值运算符
     * 
     * 防止单例实例被赋值
     */
    WaterQuality& operator=(const WaterQuality&) = delete;

    /**
     * @brief 静态私有实例
     * 
     * 在程序启动时立即初始化，保证线程安全
     */
    static  WaterQuality instance;

public:
    /**
     * @brief 获取单例实例
     * 
     * 直接返回已创建的单例实例，无需进行线程同步检查
     * 
     * @return WaterQuality& 单例实例的引用
     */
    static WaterQuality& getInstance() {
        return instance;
    }

    /**
     * @brief 设置浊度值
     * @param value 新的浊度值
     */
    void setTurbidity(float value) { turbidity = value; }

    /**
     * @brief 设置pH值
     * @param value 新的pH值
     */
    void setpH(float value) { pH = value; }

    /**
     * @brief 设置温度值
     * @param value 新的温度值（来自DS18B20传感器）
     */
    void setDS18B20(float value) { ds18b20 = value; }

    /**
     * @brief 获取当前浊度值
     * @return float 当前浊度值
     */
    float getTurbidity() const { return turbidity; }

    /**
     * @brief 获取当前pH值
     * @return float 当前pH值
     */
    float getpH() const { return pH; }

    /**
     * @brief 获取当前温度值
     * @return float 当前温度值
     */
    float getDS18B20() const { return ds18b20; }
};

#endif // WATER_QUALITY_H
