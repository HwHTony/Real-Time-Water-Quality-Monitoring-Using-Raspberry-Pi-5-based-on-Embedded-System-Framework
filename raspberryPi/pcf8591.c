#include "pcf8591.h"

static int file;
static char buf[2];

void pcf8591_init()
{
    char filename[20] = {0};
    snprintf(filename, 19, I2C_DEV);
    if ((file = open(filename, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        exit(1);
    }
    // set i2c slace address
    if (ioctl(file, I2C_SLAVE, PCF8591_ADDRESS) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        exit(1);
    }
}

void pcf8591_clear()
{
    close(file);
}

int pcf8591_read(int channel)
{
    buf[0] = PCF8591_AIN0;
    if (write(file, buf, 1) != 1) {
        perror("Failed to write to the i2c bus");
        return 1;
    }
    // read ain channel value
    if (read(file, buf, 1) != 1) {
        perror("Failed to read from the i2c bus");
        return 1;
    }
    return buf[0]; 
}
