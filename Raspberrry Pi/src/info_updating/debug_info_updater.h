/**
 * @file debug_info_updater.h
 * @brief Debug information updater class header file, responsible for outputting water quality data to the debug interface (such as the console)
 * @details This class inherits from the base class of the information updater and implements the logic for displaying water quality data in a debugging scenario,
 *          Typically used for real-time data observation and problem troubleshooting during the development phase.
 */

#ifndef DEBUG_INFO_UPDATER_H
#define DEBUG_INFO_UPDATER_H

#include "../common/com.h"              // Public components and type definitions
#include "../common/water_quality.h"    // Water quality data single instance class, used to obtain real-time monitoring data
#include "info_updater.h"               // Information updater base class, defining a unified update interface

/**
 * @class DebugInfoUpdater
 * @brief Debugging information updater, used to output real-time water quality data to the debugging terminal (such as a console)
 * @details Inherited from the InfoUpdater base class, overrides the update method to implement the debug information refresh logic,
 *          Typically, key parameters such as current turbidity, temperature, and pH value are printed in text form for convenient development and debugging.
 */
class DebugInfoUpdater : public InfoUpdater {
public:
    /**
     * @brief Rewrite the base class method to update debugging information
     * @details Obtain the latest water quality data from the WaterQuality singleton, format it, and output it to the standard output stream (console),
     *          Generally used in conjunction with a timer for periodic calls (e.g., once per second) to achieve real-time data monitoring.
     */
    void update() override;
};

#endif // DEBUG_INFO_UPDATER_H
