// data_collector.cpp
#include "data_collector.h"

/**
 * @brief Perform a complete water quality data collection and processing
 * @details Read analog sensor data (turbidity, pH) from the PCF8591 ADC module,
 *          The temperature data is read from DS18B20 and updated to the global water quality data singleton after conversion.
 */
void DataCollector::collectData() {
    // Define the ADC channel to be read (0: turbidity sensor, 1: pH sensor)
    int channels[] = {0, 1};
    // Calculate the number of channels (number of array elements)
    int numChannels = sizeof(channels) / sizeof(int);
    // Array storing ADC reading results (corresponding to the channel order)
    int results[numChannels];

    // Read analog data of a specified channel from the PCF8591 ADC module
    pcf8591.readMultiple(channels, numChannels, results);

    // Process turbidity data: Convert ADC raw value (0-255) to turbidity percentage (0-100%)
    // Formula description: The smaller the ADC value (the higher the transmittance), the lower the turbidity; the larger the value (the lower the transmittance), the higher the turbidity
    WaterQuality::getInstance().setTurbidity(100 - results[0] * 100.0 / 255);

    // Read DS18B20 temperature sensor data and update it to water quality data
    WaterQuality::getInstance().setDS18B20(ds18b20.readTemperature());

    // Processing pH data: Convert ADC raw value (0-255) to pH value (0-14)
    // Formula description: Assuming that the sensor output is inversely proportional to the pH value (the larger the ADC value, the smaller the pH value), the full scale corresponds to pH 0-14
    WaterQuality::getInstance().setpH(14.0 - results[1] * 14.0 / 255.0);
}
