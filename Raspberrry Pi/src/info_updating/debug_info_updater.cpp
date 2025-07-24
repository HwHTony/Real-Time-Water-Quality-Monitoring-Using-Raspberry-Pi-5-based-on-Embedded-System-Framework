// debug_info_updater.cpp
#include "debug_info_updater.h"
#include <iostream>

void DebugInfoUpdater::update() {
    std::cout << "调试信息更新" << std::endl;
    std::cout << "AIN0值 -> 浊度: " << WaterQuality::getInstance().getTurbidity() << std::endl;
    std::cout << "DS18B20值 -> 温度: " << WaterQuality::getInstance().getDS18B20() << "℃" << std::endl;
    std::cout << "pH值 -> pH: " << WaterQuality::getInstance().getpH() << std::endl;
}