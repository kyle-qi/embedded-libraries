# MPU6500

## Overview

Driver for the MPU6500 6-axis IMU (accelerometer + gyroscope). Provides
register-level configuration and scaled readings for all three sensor axes.

**Accelerometer** — scaled to m/s²  
**Gyroscope** — scaled to dps (raw LSB × full-scale range / 32768)  
**Temperature** — raw register value (conversion formula pending)

## Hardware

- **Device address:** `0x68` (default, `MPU6500_I2C_ADDR_AD0_LOW`); `0x69` when AD0 is high
- **Communication protocol:** I2C
- **Required pins:** SDA, SCL

## Dependencies

- [`i2c_handler`](../i2c_handler) — provides `II2C` interface and `ArduinoI2C` implementation.

## Usage Example

```cpp
#include <Arduino.h>
#include "arduino_i2c.h"
#include "mpu6500.h"

ArduinoI2C bus;
MPU6500 mpu(bus);

void setup() {
    Serial.begin(115200);
    bus.begin(21, 22, 100000);

    if (!mpu.configureDefaults()) {
        Serial.println("MPU6500 failed to initialize!");
    }
}

void loop() {
    if (mpu.isDRDY()) {
        mpu.readAccel();
        Serial.print("Ax: "); Serial.print(mpu.getAccelX());
        Serial.print(" Ay: "); Serial.print(mpu.getAccelY());
        Serial.print(" Az: "); Serial.println(mpu.getAccelZ());
    }
    delay(200);
}
```

## Supported Platforms

Any platform with an `II2C`-compatible implementation. The `ArduinoI2C`
implementation supports ESP32, STM32, Arduino AVR, and any other
Arduino-framework target with `Wire`.
