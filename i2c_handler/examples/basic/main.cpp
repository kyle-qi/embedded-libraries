// Basic i2c_handler example: initialize the I2C bus and scan for devices.
#include <Arduino.h>
#include "i2c_handler.h"

// I2C pin / clock configuration
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_FREQ 100000

void setup() {
    Serial.begin(115200);
    delay(1000);

    if (!i2cInit(SDA_PIN, SCL_PIN, I2C_FREQ)) {
        Serial.println("I2C failed to initialize!");
    }

    // Scan the bus and report any detected devices
    i2cScan();
}

void loop() {
    // Example read of register 0x00 from a device at address 0x68
    uint8_t value = i2cRead(0x68, 0x00);
    Serial.print("Register 0x00 = 0x");
    Serial.println(value, HEX);
    delay(1000);
}
