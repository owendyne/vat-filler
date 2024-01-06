#ifndef PINS_H
#define PINS_H

namespace Pins
{

    class L298N
    {
    public:
        // enable pins
        static const int E1 = 7;
        static const int E2 = 10;

        // current sensing pins
        // static const int CS1 = A1;
        // static const int CS2 = A2;

        // input pins
        static const int I1 = 12;
        static const int I2 = 11;
        static const int I3 = 8;
        static const int I4 = 9;
    };

    static const int LiquidSensor = A0;

    static const int STOP = 4;

} // Pins


#endif // PINS_H
