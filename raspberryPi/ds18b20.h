#ifndef DS18B20_H
#define DS18B20_H

#include "com.h"
// // 读取温度数据的函数
// float read_temperature();

class DS18B20 {
    public:
        float readTemperature();
    };

#endif