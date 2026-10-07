#pragma once

#include <stdint.h>
#include <Wire.h>
#include "result.h"

/**
 * @file i2c.h
 * @brief Arduino Wire-backed I2C helper for register-based devices.
 *
 * Instantiate this once in your sketch and pass it by reference to every
 * sensor driver constructor.
 *
 * @code
 * #include "i2c.h"
 * #include "mpu6500.h"
 *
 * ArduinoI2C bus;
 * MPU6500    imu(bus);
 * @endcode
 */
class ArduinoI2C {
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

    /**
     * @brief Write a byte to a device register.
     *
     * @param addr 7-bit I2C device address.
     * @param reg  Target register address.
     * @param data Byte to write.
     * @return true on success, false otherwise.
     */
    bool write(uint8_t addr, uint8_t reg, uint8_t data) {
        Wire.beginTransmission(addr);
        Wire.write(reg);
        Wire.write(data);
        return Wire.endTransmission() == 0;
    }

    /**
     * @brief Write @p len consecutive bytes from @p buf starting at @p reg.
     *
     * Sends the register address followed by all bytes in a single I2C
     * transaction. Relies on the device auto-incrementing its internal
     * register pointer after each byte.
     *
     * @param addr 7-bit I2C device address.
     * @param reg  Starting register address.
     * @param buf  Source buffer (must be at least @p len bytes).
     * @param len  Number of bytes to write.
     * @return true on success, false otherwise.
     */
    bool writeBytes(uint8_t addr, uint8_t reg, const uint8_t* buf, uint8_t len) {
        Wire.beginTransmission(addr);
        Wire.write(reg);
        for (uint8_t i = 0; i < len; ++i) {
            Wire.write(buf[i]);
        }
        return Wire.endTransmission() == 0;
    }

    /**
     * @brief Read a byte from a device register.
     *
     * @param addr 7-bit I2C device address.
     * @param reg  Target register address.
     * @return Result carrying the byte read and a Status. On failure
     *         the value is unspecified and the status is Status::Error.
     */
    Result<uint8_t, Status> read(uint8_t addr, uint8_t reg) {
        Wire.beginTransmission(addr);
        Wire.write(reg);
        if (Wire.endTransmission(false) != 0) {
            return {0, Status::Error};
        }
        if (Wire.requestFrom(addr, static_cast<uint8_t>(1)) != 1) {
            return {0, Status::Error};
        }
        int value = Wire.read();
        if (value < 0) {
            return {0, Status::Error};
        }
        return {static_cast<uint8_t>(value), Status::Ok};
    }

    /**
     * @brief Read @p len consecutive bytes starting at @p reg into @p buf.
     *
     * Most I2C devices auto-increment their internal register pointer after
     * each byte, so this performs a single transaction and fills the buffer
     * in register order. The caller is responsible for byte assembly.
     *
     * @param addr 7-bit I2C device address.
     * @param reg  Starting register address.
     * @param buf  Destination buffer (must be at least @p len bytes).
     * @param len  Number of bytes to read.
     * @return true on success, false otherwise.
     */
    bool readBytes(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len) {
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
