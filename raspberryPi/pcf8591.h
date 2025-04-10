#ifndef PCF8591_H
#define PCF8591_H

#include "com.h"

void pcf8591_init();
void pcf8591_clear();
int pcf8591_read(int channel);

#endif 