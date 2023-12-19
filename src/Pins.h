#ifndef PINS_H
#define PINS_H

namespace Pins
{

class L298N
{
public:
  // enable pins
  static const int E1 = 8;
  static const int E2 = 9;

  // current sensing pins
  static const int CS1 = A1;
  static const int CS2 = A2;

  // input pins
  static const int I1 = 4;
  static const int I2 = 5;
  static const int I3 = 6;
  static const int I4 = 7;
};

class OLED
{
public:
  static const int SCK = A4;
  static const int SDA = A5;
};

class RotaryEncoder
{
public:
  static const int A = A2;
  static const int B = A3;
  static const int SW = 2;
};

class LiquidSensor
{
public:
  static const int PIN = A0;
};

} // Pins


#endif // PINS_H
