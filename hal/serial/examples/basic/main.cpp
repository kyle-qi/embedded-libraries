// Basic ArduinoSerial example: echo received bytes back to the sender.
#include <Arduino.h>
#include "serial.h"

ArduinoSerial serial(Serial);

void setup() {
    serial.begin(115200);
    serial.writeString("Serial ready\n");
}

void loop() {
    // Echo any received bytes back to the sender
    while (serial.available() > 0) {
        Result<uint8_t, Status> r = serial.read();
        if (r) {
            serial.write(r.value);
        }
    }
}
