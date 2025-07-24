#include "pcf8591.h"
#include <iostream>

/**
 * PCF8591 I2C Analog-to-digital converter driver
 * Used to communicate with the PCF8591 chip and read data from multiple analog input channels
 */
PCF8591::PCF8591() {
    // Open the I2C device file
    char filename[20] = {0};
    snprintf(filename, 19, I2C_DEV);  // Formatting I2C device paths
    if ((file = open(filename, O_RDWR)) < 0) {
        std::cerr << "Failed to open the i2c bus" << std::endl;
        exit(1);  // Terminate the program if opening fails
    }
    
    // Set the I2C slave address (the default address for PCF8591 is 0x48)
    if (ioctl(file, I2C_SLAVE, PCF8591_ADDRESS) < 0) {
        std::cerr << "Failed to acquire bus access and/or talk to slave" << std::endl;
        exit(1);  // Terminate the program if setting fails
    }
}

/**
 * Destructor: Close the I2C device file
 */
PCF8591::~PCF8591() {
    close(file);  // Release file resources
}

/**
 * Read the values of multiple analog input channels
 * @param channels An array containing the channel numbers to read
 * @param numChannels The number of channels to read
 * @param results Array to store the read results
 * @return Returns 0 on success, non-zero on failure
 */
int PCF8591::readMultiple(int channels[], int numChannels, int results[]) {
    for (int i = 0; i < numChannels; ++i) {
        // 1. Select the channel to read
        buf[0] = channels[i];  // Set the control byte and specify the channel number
        if (::write(file, buf, 1) != 1) {
            std::cerr << "Failed to write to the i2c bus" << std::endl;
            return 1;  // Write failed and returned error
        }
        
        // 2. First read (discarded) - Due to the nature of the PCF8591, the first read is the last conversion result
        if (::read(file, buf, 1) != 1) {
            std::cerr << "Failed to read from the i2c bus" << std::endl;
            return 1;  // Read failed and returned error
        }
        
        // 3. Second read - Get the actual conversion result of the current channel
        if (::read(file, buf, 1) != 1) {
            std::cerr << "Failed to read from the i2c bus" << std::endl;
            return 1;  // Read failed and returned error
        }
        
        // Save the read result (8-bit value, range 0-255)
        results[i] = buf[0];
    }
    return 0;  // Success returns 0
}
