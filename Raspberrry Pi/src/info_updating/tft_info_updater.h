/**
 * @file tft_info_updater.h
 * @brief TFT screen information updater interface, responsible for displaying water quality data on the TFT screen
 * @details This file defines the specific implementation class used for TFT screen display updates, which inherits from the InfoUpdater abstract base class,
 *          Real-time screen refresh of water quality data (such as turbidity, temperature, pH value) through the TFT Freetype driver,
 *          It is the core module for visualising data in the system.
 */

#ifndef TFT_INFO_UPDATER_H
#define TFT_INFO_UPDATER_H

#include "../common/com.h"              // Public types and utility function definitions
#include "../common/water_quality.h"    // Water quality data single instance class, used to obtain data to be displayed
#include "../display/tft_freetype.h"    // TFT screen and font driver, providing display control interface
#include "info_updater.h"               // Information updater base class, defining a unified update interface

/**
 * @class TFTInfoUpdater
 * @brief TFT screen information updater, responsible for displaying real-time water quality data on the TFT screen
 * @details Inherited from the InfoUpdater abstract base class, it controls screen display through the TFTFreetype object,
 *          Rewrite the update method to format water quality data (turbidity, temperature, pH value, etc.) and refresh the screen,
 *          Typically, it works in conjunction with a timer to periodically (e.g., once per second) update the display content to ensure data real-time.
 */
class TFTInfoUpdater : public InfoUpdater{
private:
    TFTFreetype tft;                     ///< TFT screen and font controller for performing actual display operations
    static const int BUFFER_SIZE = 20;   ///< String buffer size, used to format text to be displayed
    wchar_t turb[BUFFER_SIZE] = {0};     ///< A wide character buffer that stores turbidity information for display on the screen

public:
    /**
     * @brief Override the pure virtual method of the base class to perform TFT screen display updates
     * @details Retrieve the latest water quality data (turbidity, temperature, pH value) from the WaterQuality singleton,
     *          Format as a human-readable wide string (e.g., ‘Turbidity: XX%’) and draw it to the specified position on the screen using the tft object,
     *          Real-time data display on TFT screens.
     */
    void update() override;
};

#endif // TFT_INFO_UPDATER_H
