// Basic ArduinoClock example: demonstrates blocking delay and non-blocking
// elapsed timing.
#include <Arduino.h>
#include "clock_arduino.h"

ArduinoClock clk;

void setup() {
    Serial.begin(115200);
    clk.delayMs(1000);
    Serial.println("Clock example started.");
}

// Non-blocking 500 ms heartbeat using elapsed()
uint32_t lastHeartbeat = 0;

// Non-blocking 2 s sensor poll using elapsed()
uint32_t lastPoll = 0;

void loop() {
    if (clk.elapsed(lastHeartbeat, 500)) {
        lastHeartbeat = clk.millis();
        Serial.print("Uptime (ms): ");
        Serial.println(lastHeartbeat);
    }

    if (clk.elapsed(lastPoll, 2000)) {
        lastPoll = clk.millis();
        Serial.print("Uptime (us): ");
        Serial.println(clk.micros());
    }
}
