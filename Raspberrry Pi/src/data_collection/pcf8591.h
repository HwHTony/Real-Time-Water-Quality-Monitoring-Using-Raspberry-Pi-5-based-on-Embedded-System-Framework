#ifndef PCF8591_H
#define PCF8591_H

#include "../common/com.h"
#include "../common/constants.h"
/**
 * PCF8591 I2C bus interface 4-channel ADC (Analog-to-Digital Converter) driver class
 * Supports reading data from up to 4 analog input channels
 */
class PCF8591 {
    public:
        /**
         * Constructor: Initialize I2C bus communication
         * - Open I2C device file
         * - Set the PCF8591 device address
         */
        PCF8591();
        /**
         * Destructor: Close the I2C device connection
         */
        ~PCF8591();
        /**
         * Read analog data from multiple channels
         * @param channels Array of channel numbers to read (0-3)
         * @param numChannels The number of channels to read
         * @param results Array to store read results (return integer values 0-255)
         * @return Returns 0 on success and a non-zero error code on failure
         */
        virtual int readMultiple(int channels[], int numChannels, int results[]);
    private:
        int file; // I2C Device File Descriptor
        char buf[2]; // Communication buffer for I2C data transmission
    };

#endif 
