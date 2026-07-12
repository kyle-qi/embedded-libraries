#pragma once

#include "serial.h"
#include <Arduino.h>

/**
 * @file serial_arduino.h
 * @brief Arduino HardwareSerial-backed implementation of ISerial.
 *
 * Wraps any Arduino HardwareSerial instance (Serial, Serial1, Serial2, …),
 * covering both UART-backed and USB-CDC-backed ports transparently.
 * Instantiate once in your sketch and pass it by reference to any library
 * that requires an ISerial. Libraries never include this header — they only
 * see ISerial.
 *
 * @code
 * #include "serial_arduino.h"
 *
 * ArduinoSerial serial(Serial);
 *
 * void setup() {
 *     serial.begin(115200);
 * }
 * @endcode
 */
class ArduinoSerial : public ISerial {
public:
    /**
     * @brief Construct an ArduinoSerial wrapping the given HardwareSerial port.
     *
     * @param serial Reference to the Arduino HardwareSerial instance to wrap
     *               (e.g. Serial, Serial1, Serial2).
     */
    explicit ArduinoSerial(HardwareSerial& serial) : serial(serial) {}

    /**
     * @brief Initialize the serial port.
     *
     * @param baud Baud rate (e.g. 9600, 115200).
     */
    void begin(uint32_t baud) {
        serial.begin(baud);
    }

    // -------------------------------------------------------------------------
    // Transmit
    // -------------------------------------------------------------------------

    bool write(uint8_t byte) override {
        return serial.write(byte) == 1;
    }

    uint8_t writeBytes(const uint8_t* buf, uint8_t len) override {
        return static_cast<uint8_t>(serial.write(buf, len));
    }

    bool writeString(const char* str) override {
        uint8_t len = 0;
        while (str[len] != '\0') ++len;
        return static_cast<uint8_t>(serial.print(str)) == len;
    }

    // -------------------------------------------------------------------------
    // Receive
    // -------------------------------------------------------------------------

    uint8_t available() override {
        return static_cast<uint8_t>(serial.available());
    }

    uint8_t read() override {
        return static_cast<uint8_t>(serial.read());
    }

    uint8_t readBytes(uint8_t* buf, uint8_t len) override {
        return static_cast<uint8_t>(serial.readBytes(reinterpret_cast<char*>(buf), len));
    }

    int16_t peek() override {
        return static_cast<int16_t>(serial.peek());
    }

    // -------------------------------------------------------------------------
    // Control
    // -------------------------------------------------------------------------

    void flush() override {
        serial.flush();
    }

private:
    HardwareSerial& serial;
};
