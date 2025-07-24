#ifndef PCF8591_H
#define PCF8591_H

#include "../common/com.h"
#include "../common/constants.h"
/**
 * PCF8591 I2C总线接口的4通道ADC(模数转换器)驱动类
 * 支持读取多达4个模拟输入通道的数据
 */
class PCF8591 {
    public:
        /**
         * 构造函数：初始化I2C总线通信
         * - 打开I2C设备文件
         * - 设置PCF8591设备地址
         */
        PCF8591();
        /**
         * 析构函数：关闭I2C设备连接
         */
        ~PCF8591();
        /**
         * 读取多个通道的模拟量数据
         * @param channels 要读取的通道号数组（0-3）
         * @param numChannels 要读取的通道数量
         * @param results 存储读取结果的数组（返回0-255的整数值）
         * @return 成功返回0，失败返回非零错误码
         */
        virtual int readMultiple(int channels[], int numChannels, int results[]);
    private:
        int file; // I2C设备文件描述符
        char buf[2]; // 通信缓冲区，用于I2C数据传输
    };

#endif 