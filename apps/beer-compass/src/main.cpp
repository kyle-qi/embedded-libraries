// Include required libraries
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "i2c.h"
#include "clock.h"
#include "serial.h"
#include "utils.h"
#include "qmc5883l.h"
#include "mpu6500.h"
#include "neo6m.h"
#include "imu.h"
#include "navigation.h"
#include "my_locations.h"
#include "display.h"

// OLED global declarations
#define SSD1306_ADDR 0x3c // I2C address of OLED
#define SSD1306_SCREEN_WIDTH 128 // Screen width
#define SSD1306_SCREEN_HEIGHT 64 // Screen height
#define SSD1306_RESET -1 // OLED reset macro

// Create SSD1306 object
Adafruit_SSD1306 ssd1306(SSD1306_SCREEN_WIDTH, SSD1306_SCREEN_HEIGHT, &Wire, SSD1306_RESET);

// I2C global declarations
#define SDA_PIN 21 // I2C SDA pin
#define SCL_PIN 22 // I2C SCL pin
#define I2C_FREQ 100000 // I2C clock speed, standard mode (HZ)

// Shared HAL instances, injected into the drivers below
ArduinoI2C bus;
ArduinoClock clk;

// Create QMC5883L / MPU6500 (defaults: I2C addresses and register options in library headers)
QMC5883L qmc5883l(bus, clk);
MPU6500 mpu6500(bus);

// Neo6M global declarations
#define NEO6M_SERIAL_PORT Serial1
#define NEO6M_BAUD_RATE 9600
#define NEO6M_RX 16 // TODO: Confirm what pin I want to use
#define NEO6M_TX 17 // TODO: Confirm what pin I want to use

// Create Neo6M object. The driver expects the port to be initialized by the
// application, so pins and baud rate are applied in setup().
ArduinoSerial gpsSerial(NEO6M_SERIAL_PORT);
Neo6M neo6m(gpsSerial);

// Struct for storing sensor information
Vec3 accel, mag;

// Keep track of setup errors
bool setupSuccess = true;
bool qmc5883lSuccess = true;
bool mpu6500Success = true;

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  delay(1000);

  // Initialize Neo6M Serial. ArduinoSerial::begin() only takes a baud rate,
  // so the ESP32 pin mapping is applied on the underlying port directly.
  NEO6M_SERIAL_PORT.begin(NEO6M_BAUD_RATE, SERIAL_8N1, NEO6M_RX, NEO6M_TX);

  // Initialize I2C
  if(!bus.begin(SDA_PIN, SCL_PIN, I2C_FREQ)){
    Serial.println("I2C failed to initialize!");
    setupSuccess = false;
  }

  // Initialize ssd1306
  if(!ssd1306.begin(SSD1306_SWITCHCAPVCC, SSD1306_ADDR)){
    Serial.println("SSD1306 failed to initialize!");
    setupSuccess = false;
  }

  // Scan for I2C devices (if necessary)
  // i2cScan();

  // Configure qmc5883l / mpu6500 (macros and steps live in library headers / configureDefaults)
  qmc5883lSuccess &= qmc5883l.configureDefaults();
  setupSuccess &= qmc5883lSuccess;
  if(!qmc5883lSuccess){
    Serial.println("QMC5883L failed to initialize!");
  }

  mpu6500Success &= mpu6500.configureDefaults();
  setupSuccess &= mpu6500Success;
  if(!mpu6500Success){
    Serial.println("MPU6500 failed to initialize!");
  }

  // TODO: Restore magnetometer calibration once the QMC5883L library
  //       provides it again (calibrate() was removed in the library refactor).
}

void loop() {
  // Setup fail condition
  if(!setupSuccess){
    Serial.println("Setup failed!");
    while(true){}
  }

  // Get magnetometer readings
  // isDRDY() returns a Result: !magReady is a bus error, magReady.value is the flag
  Result<bool, Status> magReady = qmc5883l.isDRDY();
  if (!magReady) {
    Serial.println("QMC5883L DRDY read failed!");
  } else if (magReady.value) {
    if (qmc5883l.read()) {
      mag.x = qmc5883l.getX();
      mag.y = qmc5883l.getY();
      mag.z = qmc5883l.getZ();
    } else {
      Serial.println("QMC5883L read failed!");
    }
  }

  // Get accelerometer readings
  Result<bool, Status> accelReady = mpu6500.isDRDY();
  if (!accelReady) {
    Serial.println("MPU6500 DRDY read failed!");
  } else if (accelReady.value) {
    if (mpu6500.readAccel()) {
      accel.x = mpu6500.getAccelX();
      accel.y = mpu6500.getAccelY();
      accel.z = mpu6500.getAccelZ();
    } else {
      Serial.println("MPU6500 read failed!");
    }
  }

  // Get tilt-compensated azimuth
  float azimuth = true_azimuth(accel.x, accel.y, accel.z, mag.x, mag.y, mag.z);

  Serial.println("START READING");

  // Print accelerometer readings
  Serial.print("X accel: ");
  Serial.print(accel.x);
  Serial.print(" Y accel: ");
  Serial.print(accel.y);
  Serial.print(" Z accel: ");
  Serial.println(accel.z);

  // Print magnetometer readings
  Serial.print("X mag: ");
  Serial.print(mag.x);
  Serial.print(" Y mag: ");
  Serial.print(mag.y);
  Serial.print(" Z mag: ");
  Serial.println(mag.z);

  // Print tilt-compensated azimuth
  Serial.print("Corrected azimuth: ");
  Serial.println((int)azimuth);

  Serial.println("END READING");

  // Read Neo6M
  if(neo6m.isDRDY()){
    neo6m.read();
  } else {
    Serial.println("Neo6M information not available!");
  }

  Location myLocation = {neo6m.getLatitude(), neo6m.getLongitude()};

  // Find heading and distance to target
  Location target = closestTarget(myLocation, LOCATIONS, nLocations);
  float heading = targetHeading(azimuth, myLocation, target);
  float distance = targetDistance(myLocation, target);

  // Display on OLED
  updateCompass(azimuth, heading, distance, ssd1306);

  delay(200);
}