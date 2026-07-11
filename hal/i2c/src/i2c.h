#pragma once

#include <stdint.h>

/**
 * @file i2c.h
 * @brief Platform-agnostic abstract I2C interface.
 *
 * Defines the minimum set of operations a hardware I2C implementation
 * must provide. Sensor drivers depend only on this header and i2c_utils.h.
 * Concrete implementations (e.g. ArduinoI2C) are injected at construction
 * time, keeping driver code free of any HAL or platform dependencies.
 */
class II2C {
public:
    virtual ~II2C() = default;

    /**
     * @brief Write a byte to a device register.
     *
     * @param addr 7-bit I2C device address.
     * @param reg  Target register address.
     * @param data Byte to write.
     * @return true on success, false otherwise.
     */
    virtual bool write(uint8_t addr, uint8_t reg, uint8_t data) = 0;

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
    virtual bool writeBytes(uint8_t addr, uint8_t reg, const uint8_t* buf, uint8_t len) = 0;

    /**
     * @brief Read a byte from a device register.
     *
     * @param addr 7-bit I2C device address.
     * @param reg  Target register address.
     * @return The byte read from the register.
     */
    virtual uint8_t read(uint8_t addr, uint8_t reg) = 0;

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
    virtual bool readBytes(uint8_t addr, uint8_t reg, uint8_t* buf, uint8_t len) = 0;
};

