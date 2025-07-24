#include "pcf8591.h"
#include <iostream>

/**
 * PCF8591 I2C 模数转换器驱动类
 * 用于与 PCF8591 芯片通信，读取多个模拟输入通道的数据
 */
PCF8591::PCF8591() {
    // 打开 I2C 设备文件
    char filename[20] = {0};
    snprintf(filename, 19, I2C_DEV);  // 格式化 I2C 设备路径
    if ((file = open(filename, O_RDWR)) < 0) {
        std::cerr << "Failed to open the i2c bus" << std::endl;
        exit(1);  // 打开失败时终止程序
    }
    
    // 设置 I2C 从设备地址（PCF8591 的默认地址为 0x48）
    if (ioctl(file, I2C_SLAVE, PCF8591_ADDRESS) < 0) {
        std::cerr << "Failed to acquire bus access and/or talk to slave" << std::endl;
        exit(1);  // 设置失败时终止程序
    }
}

/**
 * 析构函数：关闭 I2C 设备文件
 */
PCF8591::~PCF8591() {
    close(file);  // 释放文件资源
}

/**
 * 读取多个模拟输入通道的值
 * @param channels 包含要读取的通道号的数组
 * @param numChannels 要读取的通道数量
 * @param results 存储读取结果的数组
 * @return 成功返回 0，失败返回非零值
 */
int PCF8591::readMultiple(int channels[], int numChannels, int results[]) {
    for (int i = 0; i < numChannels; ++i) {
        // 1. 选择要读取的通道
        buf[0] = channels[i];  // 设置控制字节，指定通道号
        if (::write(file, buf, 1) != 1) {
            std::cerr << "Failed to write to the i2c bus" << std::endl;
            return 1;  // 写入失败返回错误
        }
        
        // 2. 第一次读取（丢弃） - 由于 PCF8591 的特性，第一次读取的是上次转换结果
        if (::read(file, buf, 1) != 1) {
            std::cerr << "Failed to read from the i2c bus" << std::endl;
            return 1;  // 读取失败返回错误
        }
        
        // 3. 第二次读取 - 获取当前通道的实际转换结果
        if (::read(file, buf, 1) != 1) {
            std::cerr << "Failed to read from the i2c bus" << std::endl;
            return 1;  // 读取失败返回错误
        }
        
        // 保存读取结果（8位值，范围0-255）
        results[i] = buf[0];
    }
    return 0;  // 成功返回 0
}