#pragma once

#include "clock.h"
#include <Arduino.h>

/**
 * @file clock_arduino.h
 * @brief Arduino-backed implementation of IClock.
 *
 * Wraps the Arduino runtime functions millis(), micros(), delay(), and
 * delayMicroseconds(). Instantiate once and inject into any driver that
 * needs timing. Drivers never include this header — they only see IClock.
 *
 * @code
 * #include "clock_arduino.h"
 * #include "mpu6500.h"
 *
 * ArduinoClock clock;
 * MPU6500 imu(bus, clock);
 * @endcode
 */
class ArduinoClock : public IClock {
public:
    uint32_t millis() override {
        return ::millis();
    }

    uint32_t micros() override {
        return ::micros();
    }

    void delayMs(uint32_t ms) override {
        ::delay(ms);
    }

    void delayUs(uint32_t us) override {
        ::delayMicroseconds(us);
    }

    bool elapsed(uint32_t lastMs, uint32_t intervalMs) override {
        // Subtraction handles 32-bit rollover correctly without branching
        return (::millis() - lastMs) >= intervalMs;
    }
};
