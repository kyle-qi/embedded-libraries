// Basic Neo6M example: read GPGGA sentences and print latitude / longitude.
#include <Arduino.h>
#include "serial_arduino.h"
#include "neo6m.h"

// Debug output port
ArduinoSerial debugSerial(Serial);

// GPS module serial port — initialize with baud rate and pins before use
ArduinoSerial gpsSerial(Serial1);

#define NEO6M_BAUD_RATE 9600

Neo6M neo6m(gpsSerial);

void setup() {
    debugSerial.begin(115200);
    gpsSerial.begin(NEO6M_BAUD_RATE);
    debugSerial.writeString("Neo6M example started\n");
}

void loop() {
    if (neo6m.read()) {
        // A complete GPGGA sentence was parsed — print the result
        debugSerial.writeString("Latitude: ");
        // writeString only takes const char* — convert floats in the application
        // as needed (e.g. using dtostrf on AVR or snprintf on 32-bit platforms)
    }

    // isDRDY() can be used to poll before calling read()
    // if (neo6m.isDRDY()) { neo6m.read(); }
}
