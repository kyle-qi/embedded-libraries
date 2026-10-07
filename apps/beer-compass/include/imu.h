/**
 * @file imu.h
 * @brief Provides functions to compute pitch, roll, yaw, and tilt-compensated azimuth
 *        from accelerometer and magnetometer readings.
 * 
 * @note All accelerometer readings should be in consistent units (ex: m/s^2).
 *       The exact units don't matter as long as it is consistent across axes.
 * @note All magnetometer readings used for yaw must be normalized to [-1,1] and
 *       and compensated for any offsets.
 * @note The function input parameters must match the chips' physical coordinate frame.
 * @note The accelerometer and magnetometer must share the same coordinate frame.
 * @note Accuracy is best when the board isn't experiencing high jerk.
 */

#ifndef IMU_H
#define IMU_H

#include <Arduino.h>

/**
 * @brief Calculates the pitch using accelerometer readings.
 * 
 * @param ax The x reading of the accelerometer.
 * @param ay The y reading of the accelerometer.
 * @param az The z reading of the accelerometer.
 * @return The pitch in radians.
 */
float pitch_accel(float ax, float ay, float az);

/**
 * @brief Calculates the roll using accelerometer readings.
 * 
 * @param ay The y reading of the accelerometer.
 * @param az The z reading of the accelerometer.
 * @return The roll in radians.
 */
float roll_accel(float ay, float az);

/**
 * @brief Calculates the yaw using accelerometer and magnetometer readings.
 * 
 * @param ax The x reading of the accelerometer.
 * @param ay The y reading of the accelerometer.
 * @param az The z reading of the accelerometer.
 * @param mx The x reading of the magnetometer normalized to [-1,1].
 * @param my The y reading of the magnetometer normalized to [-1,1].
 * @param mz The z reading of the magnetometer normalized to [-1,1].
 * @return The yaw in radians.
 */
float yaw_accel_mag(float ax, float ay, float az, float mx, float my, float mz);

/**
 * @brief Calculates the board's tilt-corrected azimuth.
 * @param yaw The yaw of the board.
 * @return The azimuth in degrees, following compass convention.
 */
float true_azimuth(float yaw);

/**
 * @brief Calculates the board's tilt-corrected azimuth.
 * 
 * @param ax The x reading of the accelerometer.
 * @param ay The y reading of the accelerometer.
 * @param az The z reading of the accelerometer.
 * @param mx The x reading of the magnetometer normalized to [-1,1].
 * @param my The y reading of the magnetometer normalized to [-1,1].
 * @param mz The z reading of the magnetometer normalized to [-1,1].
 * @return The azimuth in degrees, following compass convention.
 */
float true_azimuth(float ax, float ay, float az, float mx, float my, float mz);

#endif