#ifndef QMC5883L_DEFS_H
#define QMC5883L_DEFS_H

/**
 * @file qmc5883l_defs.h
 * @brief Register map and configuration constants for the QMC5883L driver.
 *
 * Override any @c #ifndef-wrapped value before including this header.
 */

#ifndef QMC5883L_I2C_ADDR_PIN_LOW
#define QMC5883L_I2C_ADDR_PIN_LOW 0x2C
#endif

/** @name setOutputRate — valid ODR (Hz) */
/// @{
#define QMC5883L_OUTPUT_ODR_10_HZ  10
#define QMC5883L_OUTPUT_ODR_50_HZ  50
#define QMC5883L_OUTPUT_ODR_100_HZ 100
#define QMC5883L_OUTPUT_ODR_200_HZ 200
/// @}

/** @name setOverSampleRate — valid ratios */
/// @{
#define QMC5883L_OVERSAMPLE_RATE_1 1
#define QMC5883L_OVERSAMPLE_RATE_2 2
#define QMC5883L_OVERSAMPLE_RATE_4 4
#define QMC5883L_OVERSAMPLE_RATE_8 8
/// @}

/** @name setDownSampleRate — valid ratios */
/// @{
#define QMC5883L_DOWNSAMPLE_RATE_1 1
#define QMC5883L_DOWNSAMPLE_RATE_2 2
#define QMC5883L_DOWNSAMPLE_RATE_4 4
#define QMC5883L_DOWNSAMPLE_RATE_8 8
/// @}

/** @name setRange — full scale (Gauss) */
/// @{
#define QMC5883L_RANGE_2_GAUSS  2
#define QMC5883L_RANGE_8_GAUSS  8
#define QMC5883L_RANGE_12_GAUSS 12
#define QMC5883L_RANGE_30_GAUSS 30
/// @}

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

#endif
