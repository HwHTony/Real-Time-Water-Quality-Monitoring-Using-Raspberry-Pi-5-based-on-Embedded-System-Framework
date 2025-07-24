#include "ds18b20.h"
#include <iostream>
#include <fstream>
#include <sstream>

constexpr const char* DS18B20_DEVICE_PATH = "/sys/bus/w1/devices/28-000000579aa1/w1_slave";

// Read temperature value from DS18B20 temperature sensor
float DS18B20::readTemperature() {
    // Open the DS18B20 device file
    std::ifstream file(DS18B20_DEVICE_PATH);
    if (!file.is_open()) {
        // Output error message and return -1 if file open fails
        std::cerr << "Failed to open DS18B20 device" << std::endl;
        return -1;
    }
    
    std::string line;
    std::string content;
    // Read all content from the file and concatenate into a single string
    while (std::getline(file, line)) {
        content += line;
    }
    file.close();
    
    // Find the starting position of temperature data (t=)
    size_t pos = content.find("t=");
    if (pos != std::string::npos) {
        // Extract the temperature string
        std::string tempStr = content.substr(pos + 2);
        try {
            // Convert the string to a float and convert unit from millidegrees to degrees Celsius
            float temp = std::stof(tempStr) / 1000.0;
            return temp;
        } catch (const std::invalid_argument& e) {
            // Output error message if conversion fails
            std::cerr << "Invalid temperature data" << std::endl;
        }
    }
    
    // Return -1 if temperature data is not found or conversion fails
    return -1;
}