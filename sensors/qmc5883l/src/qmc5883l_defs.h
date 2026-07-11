#pragma once

/**
 * @file qmc5883l_defs.h
 * @brief Register map and configuration constants for the QMC5883L driver.
 */

// I2C address — can be overridden before including this header
#ifndef QMC5883L_I2C_ADDR_PIN_LOW
#define QMC5883L_I2C_ADDR_PIN_LOW 0x2C
#endif

#ifndef QMC5883L_CALIBRATION_IDLE_MS_DEFAULT
#define QMC5883L_CALIBRATION_IDLE_MS_DEFAULT 5000
#endif

/** @name Register map */
/// @{
#define QMC5883L_XLSB_REG   0x01
#define QMC5883L_XMSB_REG   0x02
#define QMC5883L_YLSB_REG   0x03
#define QMC5883L_YMSB_REG   0x04
#define QMC5883L_ZLSB_REG   0x05
#define QMC5883L_ZMSB_REG   0x06
#define QMC5883L_STATUS_REG 0x09
#define QMC5883L_CTRLA_REG  0x0A
#define QMC5883L_CTRLB_REG  0x0B
/// @}

#ifndef RAD_TO_DEG
#define RAD_TO_DEG 57.295779513082320876798154814105
#endif
