// debug_info_updater.cpp
#include "debug_info_updater.h"
#include <iostream>

void DebugInfoUpdater::update() {
    std::cout << "Debugging information update" << std::endl;
    std::cout << "AIN0 value -> turbidity: " << WaterQuality::getInstance().getTurbidity() << std::endl;
    std::cout << "DS18B20 value -> temperature: " << WaterQuality::getInstance().getDS18B20() << "℃" << std::endl;
    std::cout << "pH value -> pH: " << WaterQuality::getInstance().getpH() << std::endl;
}
