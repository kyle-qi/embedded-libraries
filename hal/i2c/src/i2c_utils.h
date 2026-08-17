#pragma once

#include "i2c.h"

/**
 * @file i2c_utils.h
 * @brief Free utility functions built on top of ArduinoI2C.
 *
 * These are convenience helpers that compose the ArduinoI2C register API.
 *
 * Include this header in driver code alongside i2c.h.
 */
namespace i2c {

/**
 * @brief Read-modify-write a register using a bitmask.
 *
 * Reads the current register value, replaces only the bits selected by
 * @p mask with the corresponding bits from @p data, then writes back.
 *
 * @param bus  I2C bus.
 * @param addr 7-bit I2C device address.
 * @param reg  Target register address.
 * @param data New bit values (only bits within @p mask are used).
 * @param mask Bitmask selecting which bits to modify.
 * @return true on success, false otherwise.
 */
inline bool writeMasked(ArduinoI2C& bus, uint8_t addr, uint8_t reg, uint8_t data, uint8_t mask) {
    Result<uint8_t, Status> current = bus.read(addr, reg);
    if (!current) {
        return false;
    }
    uint8_t updated = static_cast<uint8_t>((current.value & ~mask) | (data & mask));
    return bus.write(addr, reg, updated);
}

/**
 * @brief Write a single bit in a register.
 *
 * @param bus    I2C bus.
 * @param addr   7-bit I2C device address.
 * @param reg    Target register address.
 * @param bit    Value to write (true = 1, false = 0).
 * @param bitPos Bit position [0-7].
 * @return true on success, false otherwise.
 */
inline bool writeBit(ArduinoI2C& bus, uint8_t addr, uint8_t reg, bool bit, uint8_t bitPos) {
    return writeMasked(bus, addr, reg,
        static_cast<uint8_t>(bit) << bitPos,
        static_cast<uint8_t>(1u << bitPos));
}

} // namespace i2c
