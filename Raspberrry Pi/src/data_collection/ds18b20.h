#ifndef DS18B20_H
#define DS18B20_H

class DS18B20 {
    public:
        // Read temperature value from DS18B20 temperature sensor
        virtual float readTemperature();
    };

#endif