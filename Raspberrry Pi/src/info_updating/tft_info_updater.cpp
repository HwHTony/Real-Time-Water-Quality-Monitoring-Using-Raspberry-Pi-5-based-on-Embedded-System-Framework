// tft_info_updater.cpp
#include "tft_info_updater.h"

#include <cstring>  // Add the cstring header file
#include <cwchar>

void TFTInfoUpdater::update() {
    memset(turb, ' ', BUFFER_SIZE);
    std::swprintf(turb, sizeof(turb) / sizeof(wchar_t), L"Turbidity: %.2f", WaterQuality::getInstance().getTurbidity());
    tft.drawString(5, 20, turb,  0xFFFF);

    std::swprintf(turb, sizeof(turb) / sizeof(wchar_t), L"Temperature: %.2f℃", WaterQuality::getInstance().getDS18B20());
    tft.drawString(5, 50, turb,  0xFFFF);

    std::swprintf(turb, sizeof(turb) / sizeof(wchar_t), L"pH: %.2f", WaterQuality::getInstance().getpH());
    tft.drawString(5, 80, turb,  0xFFFF);

    tft.fillScreen(0x0000);
}
