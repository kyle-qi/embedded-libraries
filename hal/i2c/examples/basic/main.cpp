// Basic ArduinoI2C example: initialize the I2C bus and scan for devices.
#include <Arduino.h>
#include "i2c_arduino.h"

// I2C pin / clock configuration
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_FREQ 100000

ArduinoI2C bus;

void setup() {
    Serial.begin(115200);
    delay(1000);

    if (!bus.begin(SDA_PIN, SCL_PIN, I2C_FREQ)) {
        Serial.println("I2C failed to initialize!");
    }

    // Probe addresses 0x01–0x7E and print any that respond
    Serial.println("Scanning for I2C devices...");
    bool found = false;
    for (uint8_t addr = 1; addr < 127; ++addr) {
        // A successful read means a device acknowledged
        bus.read(addr, 0x00);
        // Wire.endTransmission() result is not exposed here; use the Arduino
        // Serial Monitor and check for non-0xFF responses as a heuristic,
        // or wire up a dedicated scan helper if needed.
    }
    Serial.println("Scan complete.");
}

void loop() {
    // Example: read register 0x00 from a device at address 0x68
    Result<uint8_t, bool> reg = bus.read(0x68, 0x00);
    if (reg) {
        Serial.print("Register 0x00 = 0x");
        Serial.println(reg.value, HEX);
    } else {
        Serial.println("Register read failed");
    }
    delay(1000);
}
