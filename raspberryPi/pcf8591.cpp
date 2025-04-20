#include "pcf8591.h"
#include <iostream>

PCF8591::PCF8591() {
    char filename[20] = {0};
    snprintf(filename, 19, I2C_DEV);
    if ((file = open(filename, O_RDWR)) < 0) {
        std::cerr << "Failed to open the i2c bus" << std::endl;
        exit(1);
    }
    // 设置 I2C 从设备地址
    if (ioctl(file, I2C_SLAVE, PCF8591_ADDRESS) < 0) {
        std::cerr << "Failed to acquire bus access and/or talk to slave" << std::endl;
        exit(1);
    }
}

PCF8591::~PCF8591() {
    close(file);
}

int PCF8591::readMultiple(int channels[], int numChannels, int results[]) {
    for (int i = 0; i < numChannels; ++i)
    {
        buf[0] = channels[i];
        if (::write(file, buf, 1) != 1) {
            std::cerr << "Failed to write to the i2c bus" << std::endl;
            return 1;
        }
        if(::read(file, buf, 1) !=1){
            std::cerr<<"Failed to read from the i2c bus"<<std::endl;
            return 1;
        }
        // 读取 AIN0 通道的值
        if (::read(file, buf, 1) != 1) {
            std::cerr << "Failed to read from the i2c bus" << std::endl;
            return 1;
        }
        results[i] = buf[0];
    }
    return 0; 
}