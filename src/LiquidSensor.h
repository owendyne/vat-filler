#ifndef LIQUID_SENSOR_H
#define LIQUID_SENSOR_H

#include <Arduino.h>

class LiquidSensor
{
private:
    int pin;
public:
    LiquidSensor(int pin) : pin(pin)
    {};

    double read();
};


#endif // LIQUID_SENSOR_H
