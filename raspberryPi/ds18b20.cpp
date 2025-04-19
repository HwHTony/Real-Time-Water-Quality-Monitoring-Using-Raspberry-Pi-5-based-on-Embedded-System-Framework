#include "ds18b20.h"
#include <iostream>
#include <fstream>
#include <sstream>

constexpr const char* DS18B20_DEVICE_PATH = "/sys/bus/w1/devices/28-000000579aa1/w1_slave";

float DS18B20::readTemperature() {
    std::ifstream file(DS18B20_DEVICE_PATH);
    if (!file.is_open()) {
        std::cerr << "Failed to open DS18B20 device" << std::endl;
        return -1;
    }
    std::string line;
    std::string content;
    while (std::getline(file, line)) {
        content += line;
    }
    file.close();
    size_t pos = content.find("t=");
    if (pos != std::string::npos) {
        std::string tempStr = content.substr(pos + 2);
        try {
            float temp = std::stof(tempStr) / 1000.0;
            return temp;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid temperature data" << std::endl;
        }
    }
    return -1;
}