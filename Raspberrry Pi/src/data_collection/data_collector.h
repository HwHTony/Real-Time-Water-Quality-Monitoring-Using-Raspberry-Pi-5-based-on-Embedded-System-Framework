/**
 * @file data_collector.h
 * @brief Core interface of water quality data acquisition module
 * @details Defines the functional abstraction of collecting environmental data from various sensors.
 *          Including the acquisition of water quality parameters such as temperature, pH value, dissolved oxygen, etc.
 */

#ifndef DATA_COLLECTOR_H
#define DATA_COLLECTOR_H

#include "../common/com.h"          // Communication protocol and basic type definition
#include "../data_collection/pcf8591.h"  // ADC Converter Driver
#include "../data_collection/ds18b20.h"  // Temperature sensor driver
#include "../common/water_quality.h"  // Water quality data structure definition

/**
 * @class DataCollector
 * @brief Water quality data collector, responsible for coordinating the work of multiple sensors
 * @details By combining different types of sensor drivers, the periodic collection of water quality parameters can be achieved.
 *          and convert the raw data into standard water quality indicators.
 */
class DataCollector {
private:
    PCF8591 pcf8591;  // Analog signal acquisition (ADC) for sensors such as pH and turbidity
    DS18B20 ds18b20;  // Digital temperature sensor, providing high-precision water temperature measurement

public:
    /**
     * @brief Perform a complete data collection cycle
     * @details Read data from all sensors in a preset order, perform unit conversion and calibration,
     *          and store the results into a global data structure.
     * @note The calling frequency should be adjusted according to the sensor characteristics and system requirements.
     *       It is usually recommended to go no lower than 1Hz to capture changes in water quality.
     * @see WaterQuality
     */
    void collectData();
};

#endif // DATA_COLLECTOR_H
