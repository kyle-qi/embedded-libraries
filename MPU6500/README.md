# MPU6500 PlatformIO Driver

## Overview

Driver for the MPU6500 6-axis accelerometer + gyroscope. Provides register-level
configuration (sample rate, FIFO mode, FSYNC, digital low-pass filters, full
scale ranges, power management, gyro offsets) and raw / scaled readings for the
accelerometer (m/s^2), gyroscope (rad/s), and temperature.

## Hardware

- **Device address:** `0x68` by default (`MPU6500_I2C_ADDR_AD0_LOW`)
- **Communication protocol:** I2C
- **Required pins:** SDA, SCL

## Dependencies

- [`i2c_handler`](../../common/i2c_handler) — I2C register access helpers.

## Usage Example

```cpp
#include <Arduino.h>
#include "i2c_handler.h"
#include "mpu6500.h"

MPU6500 mpu6500;

void setup() {
    Serial.begin(115200);
    i2cInit(21, 22, 100000);
    mpu6500.configureDefaults();
}

void loop() {
    if (mpu6500.isDRDY()) {
        mpu6500.readAccel();
        Serial.println(mpu6500.getAccelX());
    }
    delay(200);
}
```

## Supported Platforms

- ESP32
- STM32
- Arduino AVR
- Any Arduino-framework platform with I2C support
