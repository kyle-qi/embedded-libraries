#ifndef UTILS_H
#define UTILS_H

#ifndef RAD_TO_DEG
#define RAD_TO_DEG 57.295779513082320876798154814105
#endif

/**
 * @brief Initializes the display
 */
void initDisplay();

/**
 * @brief Initializes the magnetometer
 */
void initMagnetometer();

/**
 * @brief Initializes the accelerometer
 */
void initAcclerometer();

struct Vec3{
  float x, y, z;
};

#endif