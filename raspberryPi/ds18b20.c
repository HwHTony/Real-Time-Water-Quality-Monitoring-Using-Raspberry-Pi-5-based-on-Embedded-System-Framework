#include "ds18b20.h"

#define DS18B20_DEVICE_PATH "/sys/bus/w1/devices/28-000000579aa1/w1_slave" // change actual device id

// read tempeartere
float read_temperature() {
    FILE *fp;
    char buf[256];
    float temp = -1;

    // open device file
    fp = fopen(DS18B20_DEVICE_PATH, "r");
    if (fp == NULL) {
        perror("Failed to open DS18B20 device");
        return -1;
    }

    // read content
    if (fread(buf, sizeof(char), sizeof(buf) - 1, fp) > 0) {
        buf[sizeof(buf) - 1] = '\0'; // end with \0
        char *ptr = strstr(buf, "t="); // search data
        if (ptr) {
            temp = atof(ptr + 2) / 1000.0; // change it to temperature
        }
    }

    fclose(fp);
    return temp;
}
