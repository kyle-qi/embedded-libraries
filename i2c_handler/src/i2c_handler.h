#ifndef I2C_HANDLER_H
#define I2C_HANDLER_H

namespace i2c_field {
  /** Replace @a mask bits in @a reg with corresponding bits from @a value (stray bits in @a value are masked off). */
  inline uint8_t merge_masked(uint8_t reg, uint8_t value, uint8_t mask) {
    return static_cast<uint8_t>((reg & static_cast<uint8_t>(~mask)) | (value & mask));
  }

  /** Extract the field selected by @a mask, shifted so its LSB aligns with bit 0. */
  inline uint8_t read_shifted(uint8_t reg, uint8_t mask) {
    if (mask == 0) {
      return 0;
    }
    uint8_t m = mask;
    uint8_t r = reg;
    while ((m & 1u) == 0) {
      m = static_cast<uint8_t>(m >> 1);
      r = static_cast<uint8_t>(r >> 1);
    }
    return static_cast<uint8_t>(r & m);
  }
}  // namespace i2c_field

/**
 * @brief Scans for devices on the I2C lines.
 * @return true if a device was detected on the I2C lines, false otherwise.
 */
bool i2cScan();

/**
 * @brief Initializes the I2C lines.
 * @param sdaPin MCU serial data pin.
 * @param sclPin MCU serial clock pin.
 * @param frequency I2C line operational frequency.
 * @return true if initialization was successful, false otherwise.
 */
bool i2cInit(int sdaPin, int sclPin, uint32_t frequency);

/**
 * @brief Writes data to an I2C device's register.
 * @param addr The address of the I2C device.
 * @param reg The I2C device's targest register.
 * @param data The data to be written to the register.
 * @return true if writing was successful, false otherwise.
 */
bool i2cWrite(uint8_t addr, uint8_t reg, uint8_t data);

/**
 * @brief Writes data to an I2C device's register at the bits specified.
 * @param addr The address of the I2C device.
 * @param reg The I2C device's target register.
 * @param data The data to be written to the register.
 * @param mask The bits in the register to be modified.
 * @return true if writing was successful, false otherwise.
 */
bool i2cWrite(uint8_t addr, uint8_t reg, uint8_t data, uint8_t mask);

/**
 * @brief Sets a single bit in a desired chip register. Used for set/reset purposes.
 * 
 * @param addr The address of the I2C device.
 * @param reg The desired register to be modifed.
 * @param myBit The bit to write to the register.
 * @param bitPos The target bit position in the desired register.
 * @return true if the operation is successful, false otherwise.
 */
bool i2cWriteBit(uint8_t addr, uint8_t reg, bool myBit, uint8_t bitPos);

/**
 * @brief Reads data from an I2C device's register.
 * 
 * @param addr The address of the I2C device.
 * @param reg The I2C device's target register.
 * @return the data located in the target register.
 */
uint8_t i2cRead(uint8_t addr, uint8_t reg);

/**
 * @brief Reads two bytes of data from an I2C device's register.
 * 
 * @param addr The address of the I2C device.
 * @param lsbReg The I2C device's register that contains the 8 LSBs.
 * @param msbReg The I2C device's register that contains the 8 MSBs.
 */
uint16_t i2cReadTwo(uint8_t addr, uint8_t lsbReg, uint8_t msbReg);

#endif
