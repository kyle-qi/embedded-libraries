// Basic Neo6M example: read GPGGA sentences and print latitude / longitude.
#include <Arduino.h>
#include <HardwareSerial.h>
#include "neo_6m.h"

// Neo6M UART configuration
#define NEO6M_SERIAL_PORT Serial1
#define NEO6M_BAUD_RATE 9600
#define NEO6M_RX 16
#define NEO6M_TX 17

Neo6M neo6m(NEO6M_SERIAL_PORT, NEO6M_BAUD_RATE, NEO6M_RX, NEO6M_TX);

void setup() {
    Serial.begin(115200);
    delay(1000);

    neo6m.begin();
}

void loop() {
    while (neo6m.isDRDY()) {
        neo6m.read();
    }

    Serial.print("Latitude: ");
    Serial.println(neo6m.getLatitude());
    Serial.print("Longitude: ");
    Serial.println(neo6m.getLongitude());

    delay(200);
}
