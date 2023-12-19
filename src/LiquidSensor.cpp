#include "LiquidSensor.h"

double LiquidSensor::read()
{
  int value = 1023 - analogRead(pin);
  return map(static_cast<double>(value), 0.0, 1023.0, 0.0, 100.0);
}
