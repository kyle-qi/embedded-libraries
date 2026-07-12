#pragma once

#include "i2c.h"
#include <Wire.h>

/**
 * @file i2c_arduino.h
 * @brief Arduino Wire-backed implementation of II2C.
 *
 * Instantiate this once in your sketch and pass it by reference to every
 * sensor driver constructor. Sensor drivers never include this header —
 * they only see II2C.
 *
 * @code
 * #include "i2c_arduino.h"
 * #include "mpu6500.h"
 *
 * ArduinoI2C bus;
 * MPU6500    imu(bus);
 * @endcode
 */
class ArduinoI2C : public II2C {
public:
    /**
     * @brief Initializes the I2C bus.
     *
     * @param sdaPin    SDA pin number.
     * @param sclPin    SCL pin number.
     * @param frequency Bus clock frequency in Hz.
     * @return true if initialization succeeded, false otherwise.
     */
    bool begin(int sdaPin, int sclPin, uint32_t frequency) {
        return Wire.begin(sdaPin, sclPin, frequency);
    }

    bool write(uint8_t addr, uint8_t reg, uint8_t data) override {
        Wire.beginTransmission(addr);
        Wire.write(reg);
        Wire.write(data);
        return Wire.endTransmission() == 0;
    }

    bool writeBytes(uint8_t addr, uint8_t reg, const uint8_t* buf, uint8_t len) override {
        Wire.beginTransmission(addr);
        Wire.write(reg);
        for (uint8_t i = 0; i < len; ++i) {
            Wire.write(buf[i]);
        }
        return Wire.endTransmission() == 0;
    }

    Result<uint8_t, bool> read(uint8_t addr, uint8_t reg) override {
        Wire.beginTransmission(addr);
        Wire.write(reg);
        if (Wire.endTransmission(false) != 0) { // repeated start
            return {0, false};
        }
        if (Wire.requestFrom(addr, static_cast<uint8_t>(1)) != 1) {
            return {0, false};
        }
        int value = Wire.read();
        if (value < 0) {
            return {0, false};
        }
        return {static_cast<uint8_t>(value), true};
    }

    bool readBytes(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len) override {
        Wire.beginTransmission(addr);
        Wire.write(reg);
        if (Wire.endTransmission(false) != 0) return false;

        Wire.requestFrom(addr, len);
        if (Wire.available() < len) return false;

        for (uint8_t i = 0; i < len; ++i) {
            buf[i] = Wire.read();
        }
        return true;
    }
};

