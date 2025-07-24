#ifndef WATER_QUALITY_H
#define WATER_QUALITY_H

/**
 * @brief Single instance class for water quality monitoring data (Hungry Man implementation)
 * 
 * This class is designed using the starving singleton pattern, and an instance is created immediately when the program starts，
 * Ensure thread safety and eliminate concerns about memory leaks. Used to store and manage various parameters in water quality monitoring systems,
 * Including turbidity, pH value, and temperature. Any module in the system can obtain and update the current water quality data through this class.
 */
class WaterQuality {
private:
    float turbidity;  ///< Turbidity value, unit depends on sensor
    float pH;         ///< pH value, reflecting the acidity or alkalinity of water
    float ds18b20;    ///< Temperature values measured by the DS18B20 temperature sensor, in degrees Celsius
    /**
     * @brief Private constructor
     * 
     * Initialise all water quality parameters to 0 and ensure that this class cannot be instantiated externally.
     */
    WaterQuality() : turbidity(0), pH(0), ds18b20(0) {}

    /**
     * @brief Disable copy constructors
     * 
     * Prevent singleton instances from being copied
     */
    WaterQuality(const WaterQuality&) = delete;

    /**
     * @brief Disable assignment operators
     * 
     * Prevent singleton instances from being assigned values
     */
    WaterQuality& operator=(const WaterQuality&) = delete;

    /**
     * @brief Static private instance
     * 
     * Initialise immediately when the program starts, ensuring thread safety
     */
    static  WaterQuality instance;

public:
    /**
     * @brief Obtain a singleton instance
     * 
     * Directly return the created singleton instance without performing thread synchronisation checks
     * 
     * @return WaterQuality& Reference to a singleton instance
     */
    static WaterQuality& getInstance() {
        return instance;
    }

    /**
     * @brief Set turbidity value
     * @param value New turbidity values
     */
    void setTurbidity(float value) { turbidity = value; }

    /**
     * @brief Set pH value
     * @param value New pH value
     */
    void setpH(float value) { pH = value; }

    /**
     * @brief Set temperature value
     * @param value New temperature value (from DS18B20 sensor)
     */
    void setDS18B20(float value) { ds18b20 = value; }

    /**
     * @brief Obtain the current turbidity value
     * @return float Current turbidity value
     */
    float getTurbidity() const { return turbidity; }

    /**
     * @brief Obtain the current pH value
     * @return float Current pH value
     */
    float getpH() const { return pH; }

    /**
     * @brief Get the current temperature value
     * @return float Current temperature value
     */
    float getDS18B20() const { return ds18b20; }
};

#endif // WATER_QUALITY_H
