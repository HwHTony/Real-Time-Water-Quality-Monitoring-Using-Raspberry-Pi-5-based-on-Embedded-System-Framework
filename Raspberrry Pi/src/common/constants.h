// constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

// --- GPIO 设定 ---
constexpr int RST_PIN = 26;
constexpr int DC_PIN  = 22;
constexpr int BLK_PIN = 17;
constexpr int CS_PIN  = 27;

// --- SPI 设备路径 ---
constexpr const char* SPI_DEV = "/dev/spidev0.0";
// --- I2C 设备路径 ---
constexpr const char* I2C_DEV = "/dev/i2c-1";

// --- pcf8591 ---
constexpr uint8_t PCF8591_ADDRESS = 0x48;
constexpr uint8_t PCF8591_AIN0 = 0x40;
constexpr uint8_t PCF8591_AIN1 = 0x41;

#endif