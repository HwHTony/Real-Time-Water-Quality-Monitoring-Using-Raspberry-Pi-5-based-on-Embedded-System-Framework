#ifndef PCF8591_H
#define PCF8591_H

#include "com.h"

// void pcf8591_init();
// void pcf8591_clear();
// int pcf8591_read(int channel);

class PCF8591 {
    public:
        PCF8591();
        ~PCF8591();
        int read(int channel);
    private:
        int file;
        char buf[2];
    };

#endif 