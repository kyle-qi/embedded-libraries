// Basic MPU6500 example: configure the IMU and print accelerometer readings.
#include <Arduino.h>
#include "i2c_handler.h"
#include "mpu6500.h"

// I2C pin / clock configuration
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_FREQ 100000

MPU6500 mpu6500;

void setup() {
    Serial.begin(115200);
    delay(1000);

    if (!i2cInit(SDA_PIN, SCL_PIN, I2C_FREQ)) {
        Serial.println("I2C failed to initialize!");
    }

    // Reset registers and apply the accel-only default profile
    if (!mpu6500.configureDefaults()) {
        Serial.println("MPU6500 failed to initialize!");
    }
}

void loop() {
    if (mpu6500.isDRDY()) {
        mpu6500.readAccel();
        Serial.print("Ax: ");
        Serial.print(mpu6500.getAccelX());
        Serial.print(" Ay: ");
        Serial.print(mpu6500.getAccelY());
        Serial.print(" Az: ");
        Serial.println(mpu6500.getAccelZ());
    }
    delay(200);
}
