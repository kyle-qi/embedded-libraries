// Basic QMC5883L example: configure the magnetometer and print readings.
#include <Arduino.h>
#include "i2c_handler.h"
#include "qmc5883l.h"

// I2C pin / clock configuration
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_FREQ 100000

QMC5883L qmc5883l;

void setup() {
    Serial.begin(115200);
    delay(1000);

    if (!i2cInit(SDA_PIN, SCL_PIN, I2C_FREQ)) {
        Serial.println("I2C failed to initialize!");
    }

    // Reset registers and apply the continuous-heading default profile
    if (!qmc5883l.configureDefaults()) {
        Serial.println("QMC5883L failed to initialize!");
    }
}

void loop() {
    if (qmc5883l.isDRDY()) {
        qmc5883l.read();
        Serial.print("X: ");
        Serial.print(qmc5883l.getX());
        Serial.print(" Y: ");
        Serial.print(qmc5883l.getY());
        Serial.print(" Z: ");
        Serial.println(qmc5883l.getZ());
    }
    delay(200);
}
