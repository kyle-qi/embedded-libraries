#ifndef MPU6500_DEFS_H
#define MPU6500_DEFS_H

/**
 * @file mpu6500_defs.h
 * @brief Register map, scaling constant, and configuration macros for the MPU6500 driver.
 */

/** @name Register map */
/// @{
#define SELF_TEST_X_GYRO 0x00
#define SELF_TEST_Y_GYRO 0x01
#define SELF_TEST_Z_GYRO 0x02

#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E
#define SELF_TEST_Z_ACCEL 0x0F

#define XG_OFFSET_H 0x13
#define XG_OFFSET_L 0x14
#define YG_OFFSET_H 0x15
#define YG_OFFSET_L 0x16
#define ZG_OFFSET_H 0x17
#define ZG_OFFSET_L 0x18

#define SMPLRT_DIV 0x19
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_CONFIG_2 0x1D

#define INT_STATUS 0x3A

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42

#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C

#define WHO_AM_I 0x75
/// @}

#ifndef GRAVITY
#define GRAVITY 9.10665
#endif

#ifndef MPU6500_I2C_ADDR_AD0_LOW
#define MPU6500_I2C_ADDR_AD0_LOW 0x68
#endif

/** @name setSampleRateDivider — SMPLRT_DIV (1 kHz internal → ~100 Hz with 9) */
/// @{
#define MPU6500_SMPLRT_DIV_9 9
/// @}

/** @name setFIFOMode — @c stopWhenFull */
/// @{
#define MPU6500_FIFO_OVERWRITE_OLDEST false
#define MPU6500_FIFO_STOP_WHEN_FULL   true
/// @}

/** @name setAccelLPF — bandwidth (Hz) when LPF on */
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

/** @name setGyroRange — full scale (dps) */
/// @{
#define MPU6500_GYRO_FS_250DPS  250
#define MPU6500_GYRO_FS_500DPS  500
#define MPU6500_GYRO_FS_1000DPS 1000
#define MPU6500_GYRO_FS_2000DPS 2000
/// @}

/** @name setGyroLPF — bandwidth (Hz) when LPF on */
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

/** @name Sensor / LPF enable arguments */
/// @{
#define MPU6500_SENSOR_ENABLE  true
#define MPU6500_SENSOR_DISABLE false
/// @}

#endif
