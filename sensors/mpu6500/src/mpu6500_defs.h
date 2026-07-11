#pragma once

/**
 * @file mpu6500_defs.h
 * @brief Complete register map, bit-field constants, and configuration macros
 *        for the MPU6500 IMU driver.
 *
 * Register addresses are taken from the MPU-6500 Product Specification
 * Rev 1.1 and Register Map Rev 2.0.
 */

// ============================================================================
// I2C addresses
// ============================================================================

#ifndef MPU6500_I2C_ADDR_AD0_LOW
#define MPU6500_I2C_ADDR_AD0_LOW  0x68  ///< AD0 pin low (default)
#endif
#ifndef MPU6500_I2C_ADDR_AD0_HIGH
#define MPU6500_I2C_ADDR_AD0_HIGH 0x69  ///< AD0 pin high
#endif

// ============================================================================
// WHO_AM_I expected value
// ============================================================================

#define MPU6500_WHO_AM_I_VAL 0x70  ///< Expected response from WHO_AM_I register

// ============================================================================
// Register map
// ============================================================================

/** @name Self-test registers */
/// @{
#define MPU6500_SELF_TEST_X_GYRO  0x00
#define MPU6500_SELF_TEST_Y_GYRO  0x01
#define MPU6500_SELF_TEST_Z_GYRO  0x02
#define MPU6500_SELF_TEST_X_ACCEL 0x0D
#define MPU6500_SELF_TEST_Y_ACCEL 0x0E
#define MPU6500_SELF_TEST_Z_ACCEL 0x0F
/// @}

/** @name Gyroscope offset registers (factory trim) */
/// @{
#define MPU6500_XG_OFFSET_H 0x13
#define MPU6500_XG_OFFSET_L 0x14
#define MPU6500_YG_OFFSET_H 0x15
#define MPU6500_YG_OFFSET_L 0x16
#define MPU6500_ZG_OFFSET_H 0x17
#define MPU6500_ZG_OFFSET_L 0x18
/// @}

/** @name Sample rate and digital filter configuration */
/// @{
#define MPU6500_SMPLRT_DIV    0x19  ///< Sample rate divider (gyro output rate / (1 + SMPLRT_DIV))
#define MPU6500_CONFIG        0x1A  ///< FIFO mode, ext sync, DLPF config
#define MPU6500_GYRO_CONFIG   0x1B  ///< Gyro self-test, FCHOICE_B, full-scale range
#define MPU6500_ACCEL_CONFIG  0x1C  ///< Accel self-test, full-scale range
#define MPU6500_ACCEL_CONFIG2 0x1D  ///< Accel FCHOICE_B, DLPF config
/// @}

/** @name Low-power accelerometer ODR control */
/// @{
#define MPU6500_LP_ACCEL_ODR  0x1E
/// @}

/** @name Wake-on-motion threshold */
/// @{
#define MPU6500_WOM_THR       0x1F
/// @}

/** @name FIFO control */
/// @{
#define MPU6500_FIFO_EN       0x23  ///< Selects which sensor data is loaded into FIFO
/// @}

/** @name I2C master / auxiliary sensor control */
/// @{
#define MPU6500_I2C_MST_CTRL    0x24
#define MPU6500_I2C_SLV0_ADDR   0x25
#define MPU6500_I2C_SLV0_REG    0x26
#define MPU6500_I2C_SLV0_CTRL   0x27
#define MPU6500_I2C_SLV1_ADDR   0x28
#define MPU6500_I2C_SLV1_REG    0x29
#define MPU6500_I2C_SLV1_CTRL   0x2A
#define MPU6500_I2C_SLV2_ADDR   0x2B
#define MPU6500_I2C_SLV2_REG    0x2C
#define MPU6500_I2C_SLV2_CTRL   0x2D
#define MPU6500_I2C_SLV3_ADDR   0x2E
#define MPU6500_I2C_SLV3_REG    0x2F
#define MPU6500_I2C_SLV3_CTRL   0x30
#define MPU6500_I2C_SLV4_ADDR   0x31
#define MPU6500_I2C_SLV4_REG    0x32
#define MPU6500_I2C_SLV4_DO     0x33
#define MPU6500_I2C_SLV4_CTRL   0x34
#define MPU6500_I2C_SLV4_DI     0x35
#define MPU6500_I2C_MST_STATUS  0x36
/// @}

/** @name Interrupt configuration */
/// @{
#define MPU6500_INT_PIN_CFG   0x37  ///< INT pin / bypass enable configuration
#define MPU6500_INT_ENABLE    0x38  ///< Interrupt enable
#define MPU6500_INT_STATUS    0x3A  ///< Interrupt status (clears on read)
/// @}

/** @name Sensor output registers (read burst from ACCEL_XOUT_H for all 14 bytes) */
/// @{
#define MPU6500_ACCEL_XOUT_H  0x3B
#define MPU6500_ACCEL_XOUT_L  0x3C
#define MPU6500_ACCEL_YOUT_H  0x3D
#define MPU6500_ACCEL_YOUT_L  0x3E
#define MPU6500_ACCEL_ZOUT_H  0x3F
#define MPU6500_ACCEL_ZOUT_L  0x40
#define MPU6500_TEMP_OUT_H    0x41
#define MPU6500_TEMP_OUT_L    0x42
#define MPU6500_GYRO_XOUT_H   0x43
#define MPU6500_GYRO_XOUT_L   0x44
#define MPU6500_GYRO_YOUT_H   0x45
#define MPU6500_GYRO_YOUT_L   0x46
#define MPU6500_GYRO_ZOUT_H   0x47
#define MPU6500_GYRO_ZOUT_L   0x48
/// @}

/** @name External sensor data (from auxiliary I2C slaves) */
/// @{
#define MPU6500_EXT_SENS_DATA_00 0x49
#define MPU6500_EXT_SENS_DATA_01 0x4A
#define MPU6500_EXT_SENS_DATA_02 0x4B
#define MPU6500_EXT_SENS_DATA_03 0x4C
#define MPU6500_EXT_SENS_DATA_04 0x4D
#define MPU6500_EXT_SENS_DATA_05 0x4E
#define MPU6500_EXT_SENS_DATA_06 0x4F
#define MPU6500_EXT_SENS_DATA_07 0x50
#define MPU6500_EXT_SENS_DATA_08 0x51
#define MPU6500_EXT_SENS_DATA_09 0x52
#define MPU6500_EXT_SENS_DATA_10 0x53
#define MPU6500_EXT_SENS_DATA_11 0x54
#define MPU6500_EXT_SENS_DATA_12 0x55
#define MPU6500_EXT_SENS_DATA_13 0x56
#define MPU6500_EXT_SENS_DATA_14 0x57
#define MPU6500_EXT_SENS_DATA_15 0x58
#define MPU6500_EXT_SENS_DATA_16 0x59
#define MPU6500_EXT_SENS_DATA_17 0x5A
#define MPU6500_EXT_SENS_DATA_18 0x5B
#define MPU6500_EXT_SENS_DATA_19 0x5C
#define MPU6500_EXT_SENS_DATA_20 0x5D
#define MPU6500_EXT_SENS_DATA_21 0x5E
#define MPU6500_EXT_SENS_DATA_22 0x5F
#define MPU6500_EXT_SENS_DATA_23 0x60
/// @}

/** @name I2C slave data-out registers */
/// @{
#define MPU6500_I2C_SLV0_DO   0x63
#define MPU6500_I2C_SLV1_DO   0x64
#define MPU6500_I2C_SLV2_DO   0x65
#define MPU6500_I2C_SLV3_DO   0x66
/// @}

/** @name I2C master delay control */
/// @{
#define MPU6500_I2C_MST_DELAY_CTRL 0x67
/// @}

/** @name Signal path reset */
/// @{
#define MPU6500_SIGNAL_PATH_RESET  0x68  ///< Resets gyro, accel, and temp sensor signal paths
/// @}

/** @name Motion detection control */
/// @{
#define MPU6500_MOT_DETECT_CTRL    0x69
/// @}

/** @name User control */
/// @{
#define MPU6500_USER_CTRL          0x6A  ///< FIFO enable, I2C master enable, sensor reset
/// @}

/** @name Power management */
/// @{
#define MPU6500_PWR_MGMT_1         0x6B  ///< Device reset, sleep, cycle, temp disable, clk select
#define MPU6500_PWR_MGMT_2         0x6C  ///< Per-axis enable/disable for accel and gyro
/// @}

/** @name FIFO count and data registers */
/// @{
#define MPU6500_FIFO_COUNTH        0x72
#define MPU6500_FIFO_COUNTL        0x73
#define MPU6500_FIFO_R_W           0x74  ///< Read/write FIFO data
/// @}

/** @name Device identity */
/// @{
#define MPU6500_WHO_AM_I           0x75
/// @}

/** @name Accelerometer offset registers */
/// @{
#define MPU6500_XA_OFFSET_H        0x77
#define MPU6500_XA_OFFSET_L        0x78
#define MPU6500_YA_OFFSET_H        0x7A
#define MPU6500_YA_OFFSET_L        0x7B
#define MPU6500_ZA_OFFSET_H        0x7D
#define MPU6500_ZA_OFFSET_L        0x7E
/// @}

// ============================================================================
// CONFIG (0x1A) bit fields
// ============================================================================

#define MPU6500_CONFIG_FIFO_MODE_BIT  6   ///< 1 = stop when full, 0 = overwrite oldest
#define MPU6500_CONFIG_EXT_SYNC_SET_SHIFT 3
#define MPU6500_CONFIG_EXT_SYNC_SET_MASK  0b00111000
#define MPU6500_CONFIG_DLPF_CFG_MASK      0b00000111

// ============================================================================
// GYRO_CONFIG (0x1B) bit fields
// ============================================================================

#define MPU6500_GYRO_CONFIG_XG_ST_BIT     7   ///< X-axis gyro self-test
#define MPU6500_GYRO_CONFIG_YG_ST_BIT     6
#define MPU6500_GYRO_CONFIG_ZG_ST_BIT     5
#define MPU6500_GYRO_CONFIG_FS_SEL_SHIFT  3
#define MPU6500_GYRO_CONFIG_FS_SEL_MASK   0b00011000
#define MPU6500_GYRO_CONFIG_FCHOICE_B_MASK 0b00000011

// ============================================================================
// ACCEL_CONFIG (0x1C) bit fields
// ============================================================================

#define MPU6500_ACCEL_CONFIG_XA_ST_BIT    7
#define MPU6500_ACCEL_CONFIG_YA_ST_BIT    6
#define MPU6500_ACCEL_CONFIG_ZA_ST_BIT    5
#define MPU6500_ACCEL_CONFIG_FS_SEL_SHIFT 3
#define MPU6500_ACCEL_CONFIG_FS_SEL_MASK  0b00011000

// ============================================================================
// ACCEL_CONFIG2 (0x1D) bit fields
// ============================================================================

#define MPU6500_ACCEL_CONFIG2_FCHOICE_B_BIT 3
#define MPU6500_ACCEL_CONFIG2_DLPF_CFG_MASK 0b00000111

// ============================================================================
// INT_PIN_CFG (0x37) bit fields
// ============================================================================

#define MPU6500_INT_PIN_CFG_ACTL_BIT        7   ///< Logic level for INT: 1 = active low
#define MPU6500_INT_PIN_CFG_OPEN_BIT        6   ///< 1 = open drain, 0 = push-pull
#define MPU6500_INT_PIN_CFG_LATCH_INT_EN_BIT 5  ///< 1 = latch until cleared, 0 = 50us pulse
#define MPU6500_INT_PIN_CFG_INT_RD_CLEAR_BIT 4  ///< 1 = clear on any read, 0 = clear on INT_STATUS
#define MPU6500_INT_PIN_CFG_BYPASS_EN_BIT   1   ///< 1 = enables I2C bypass to aux bus

// ============================================================================
// INT_ENABLE (0x38) / INT_STATUS (0x3A) bit fields
// ============================================================================

#define MPU6500_INT_WOM_EN_BIT     6   ///< Wake-on-motion interrupt
#define MPU6500_INT_FIFO_OFLOW_BIT 4   ///< FIFO overflow interrupt
#define MPU6500_INT_I2C_MST_INT_BIT 3  ///< I2C master interrupt
#define MPU6500_INT_DATA_RDY_BIT   0   ///< Data ready interrupt

// ============================================================================
// USER_CTRL (0x6A) bit fields
// ============================================================================

#define MPU6500_USER_CTRL_FIFO_EN_BIT     6
#define MPU6500_USER_CTRL_I2C_MST_EN_BIT  5
#define MPU6500_USER_CTRL_I2C_IF_DIS_BIT  4   ///< Disable I2C, enable SPI
#define MPU6500_USER_CTRL_FIFO_RESET_BIT  2
#define MPU6500_USER_CTRL_I2C_MST_RESET_BIT 1
#define MPU6500_USER_CTRL_SIG_COND_RESET_BIT 0

// ============================================================================
// PWR_MGMT_1 (0x6B) bit fields
// ============================================================================

#define MPU6500_PWR1_DEVICE_RESET_BIT  7
#define MPU6500_PWR1_SLEEP_BIT         6
#define MPU6500_PWR1_CYCLE_BIT         5
#define MPU6500_PWR1_GYRO_STANDBY_BIT  4
#define MPU6500_PWR1_PD_PTAT_BIT       3   ///< Disable internal temp sensor (1 = off)
#define MPU6500_PWR1_CLKSEL_MASK       0b00000111

/** @name PWR_MGMT_1 clock source selection (CLKSEL) */
/// @{
#define MPU6500_CLOCK_INTERNAL_20MHZ   0   ///< Internal 20 MHz oscillator
#define MPU6500_CLOCK_AUTO             1   ///< Auto-select best available source (recommended)
#define MPU6500_CLOCK_STOP             7   ///< Stops the clock
/// @}

// ============================================================================
// PWR_MGMT_2 (0x6C) bit fields
// ============================================================================

#define MPU6500_PWR2_DISABLE_XA_BIT  5
#define MPU6500_PWR2_DISABLE_YA_BIT  4
#define MPU6500_PWR2_DISABLE_ZA_BIT  3
#define MPU6500_PWR2_DISABLE_XG_BIT  2
#define MPU6500_PWR2_DISABLE_YG_BIT  1
#define MPU6500_PWR2_DISABLE_ZG_BIT  0

// ============================================================================
// FIFO_EN (0x23) bit fields
// ============================================================================

#define MPU6500_FIFO_EN_TEMP_BIT   7
#define MPU6500_FIFO_EN_XG_BIT     6
#define MPU6500_FIFO_EN_YG_BIT     5
#define MPU6500_FIFO_EN_ZG_BIT     4
#define MPU6500_FIFO_EN_ACCEL_BIT  3
#define MPU6500_FIFO_EN_SLV2_BIT   2
#define MPU6500_FIFO_EN_SLV1_BIT   1
#define MPU6500_FIFO_EN_SLV0_BIT   0

// ============================================================================
// Physical constants and scaling
// ============================================================================

#ifndef GRAVITY
#define GRAVITY 9.80665f   ///< Standard gravity (m/s^2)
#endif

/** Temperature conversion: degrees_C = (raw - 0) / 333.87 + 21.0 */
#define MPU6500_TEMP_SENSITIVITY  333.87f
#define MPU6500_TEMP_OFFSET       0.0f
#define MPU6500_TEMP_ROOM_OFFSET  21.0f

// ============================================================================
// Configuration macro arguments
// ============================================================================

/** @name setSampleRateDivider — common divider values */
/// @{
#define MPU6500_SMPLRT_DIV_1    0    ///< 1 kHz (no division)
#define MPU6500_SMPLRT_DIV_9    9    ///< ~100 Hz
#define MPU6500_SMPLRT_DIV_19   19   ///< 50 Hz
#define MPU6500_SMPLRT_DIV_99   99   ///< 10 Hz
/// @}

/** @name setFIFOMode */
/// @{
#define MPU6500_FIFO_OVERWRITE_OLDEST false
#define MPU6500_FIFO_STOP_WHEN_FULL   true
/// @}

/** @name setGyroLPF — DLPF bandwidth (Hz) */
/// @{
#define MPU6500_GYRO_DLPF_BW_250_HZ  250
#define MPU6500_GYRO_DLPF_BW_184_HZ  184
#define MPU6500_GYRO_DLPF_BW_92_HZ   92
#define MPU6500_GYRO_DLPF_BW_41_HZ   41
#define MPU6500_GYRO_DLPF_BW_20_HZ   20
#define MPU6500_GYRO_DLPF_BW_10_HZ   10
#define MPU6500_GYRO_DLPF_BW_5_HZ    5
#define MPU6500_GYRO_DLPF_BW_3600_HZ 3600
/// @}

/** @name setGyroRange — full scale (dps) */
/// @{
#define MPU6500_GYRO_FS_250DPS  250
#define MPU6500_GYRO_FS_500DPS  500
#define MPU6500_GYRO_FS_1000DPS 1000
#define MPU6500_GYRO_FS_2000DPS 2000
/// @}

/** @name setAccelLPF — DLPF bandwidth (Hz) */
/// @{
#define MPU6500_ACCEL_DLPF_BW_460_HZ 460
#define MPU6500_ACCEL_DLPF_BW_184_HZ 184
#define MPU6500_ACCEL_DLPF_BW_92_HZ  92
#define MPU6500_ACCEL_DLPF_BW_44_HZ  44
#define MPU6500_ACCEL_DLPF_BW_20_HZ  20
#define MPU6500_ACCEL_DLPF_BW_10_HZ  10
#define MPU6500_ACCEL_DLPF_BW_5_HZ   5
/// @}

/** @name setAccelRange — full scale (g) */
/// @{
#define MPU6500_ACCEL_FS_2G  2
#define MPU6500_ACCEL_FS_4G  4
#define MPU6500_ACCEL_FS_8G  8
#define MPU6500_ACCEL_FS_16G 16
/// @}

/** @name Sensor enable/disable arguments */
/// @{
#define MPU6500_SENSOR_ENABLE  true
#define MPU6500_SENSOR_DISABLE false
/// @}
