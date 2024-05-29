#ifndef CONFIG_H
#define CONFIG_H

namespace Config
{
    constexpr int StepsPerRevolution = 200;
    constexpr float MaxSpeed = 20.0;
    constexpr float Acceleration = 2.0;
    constexpr float Speed = 4.0;
    constexpr long StepDelta = 400;
    constexpr float LiquidThreshold = 30.0; // 0 - 100
    constexpr unsigned long LiquidInterval = 2500; // ms
}

#endif // CONFIG_H