#ifndef COM_H
#define COM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <gpiod.h>
#include <ft2build.h>
#include <wchar.h>
#include FT_FREETYPE_H

// --- i2c ---
#include <linux/i2c-dev.h>

// --- pcf8591 ---
#include "pcf8591.h"
#define PCF8591_ADDRESS 0x48
#define PCF8591_AIN0 0x40
#define PCF8591_AIN1 0x41

// --- tft ---
#include "tft_freetype.h"

// --- ds18b20 ---
#include "ds18b20.h"

// ---
#include "thread.h"

// ---
#include "sock.h"

// --- GPIO 设定 ---
#define RST_PIN 26
#define DC_PIN  22
#define BLK_PIN 17
#define CS_PIN  27

// --- SPI 设备路径 ---
#define SPI_DEV "/dev/spidev0.0"
// --- I2C 设备路径 ---
#define I2C_DEV "/dev/i2c-1"

// typedef struct Param
// {
//     float turbidity;
//     float pH;
//     float ds18b20;
// } WaterQuality;

class WaterQuality {
    public:
        float turbidity;
        float pH;
        float ds18b20;
    };


#endif