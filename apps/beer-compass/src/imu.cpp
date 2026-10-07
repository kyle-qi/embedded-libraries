#include "imu.h"
#include "utils.h"

float pitch_accel(float ax, float ay, float az){
    return atan2f(-ax, sqrt(ay * ay + az * az));
}

float roll_accel(float ay, float az){
    return atan2f(ay, az);
}

float yaw_accel_mag(float ax, float ay, float az, float mx, float my, float mz){
    float pitch = pitch_accel(ax, ay, az);
    float roll = roll_accel(ay, az);
    float mxh = mx * cosf(pitch) + mz * sinf(pitch);
    float myh = mx * sinf(roll) * sinf(pitch) + my * cosf(roll) - mz * sinf(roll) * cosf(pitch);
    return atan2f(-myh, mxh);
}

float true_azimuth(float yaw){
    yaw *= RAD_TO_DEG;
    if(yaw < 0){
        yaw += 360.0f;
    }
    return yaw;
}

float true_azimuth(float ax, float ay, float az, float mx, float my, float mz){
    float yaw = yaw_accel_mag(ax, ay, az, mx, my, mz);
    return true_azimuth(yaw);
}