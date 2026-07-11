#ifndef I2C_UTILS_H
#define I2C_UTILS_H

#include "i2c.h"

/**
 * @file i2c_utils.h
 * @brief Free utility functions built on top of II2C.
 *
 * These are convenience helpers that compose the core II2C interface.
 * They do not belong on the interface itself — no concrete implementation
 * needs to provide them, and no mock needs to implement them.
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
 * @param bus  I2C bus implementation.
 * @param addr 7-bit I2C device address.
 * @param reg  Target register address.
 * @param data New bit values (only bits within @p mask are used).
 * @param mask Bitmask selecting which bits to modify.
 * @return true on success, false otherwise.
 */
inline bool writeMasked(II2C& bus, uint8_t addr, uint8_t reg, uint8_t data, uint8_t mask) {
    uint8_t current = bus.read(addr, reg);
    uint8_t updated = static_cast<uint8_t>((current & ~mask) | (data & mask));
    return bus.write(addr, reg, updated);
}

/**
 * @brief Write a single bit in a register.
 *
 * @param bus    I2C bus implementation.
 * @param addr   7-bit I2C device address.
 * @param reg    Target register address.
 * @param bit    Value to write (true = 1, false = 0).
 * @param bitPos Bit position [0-7].
 * @return true on success, false otherwise.
 */
inline bool writeBit(II2C& bus, uint8_t addr, uint8_t reg, bool bit, uint8_t bitPos) {
    return writeMasked(bus, addr, reg,
        static_cast<uint8_t>(bit) << bitPos,
        static_cast<uint8_t>(1u << bitPos));
}

} // namespace i2c

#endif
