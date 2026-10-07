#pragma once

#include <stdint.h>
#include <Arduino.h>
#include "result.h"

/**
 * @file serial.h
 * @brief Arduino HardwareSerial-backed serial helper.
 *
 * Wraps any Arduino HardwareSerial instance (Serial, Serial1, Serial2, ...),
 * covering both UART-backed and USB-CDC-backed ports. Instantiate once in
 * your sketch and pass it by reference to any library that needs serial I/O.
 *
 * @code
 * #include "serial.h"
 *
 * ArduinoSerial serial(Serial);
 *
 * void setup() {
 *     serial.begin(115200);
 * }
 * @endcode
 */
class ArduinoSerial {
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

    /**
     * @brief Write a single byte.
     *
     * @param byte The byte to transmit.
     * @return true on success, false if the transmit buffer is full or an
     *         error occurred.
     */
    bool write(uint8_t byte) {
        return serial.write(byte) == 1;
    }

    /**
     * @brief Write @p len bytes from @p buf.
     *
     * @param buf Source buffer (must be at least @p len bytes).
     * @param len Number of bytes to write.
     * @return Number of bytes successfully written. A value less than @p len
     *         indicates a partial write or error.
     */
    uint8_t writeBytes(const uint8_t* buf, uint8_t len) {
        return static_cast<uint8_t>(serial.write(buf, len));
    }

    /**
     * @brief Write a null-terminated string.
     *
     * Does not transmit the terminating null byte.
     *
     * @param str Pointer to a null-terminated C string.
     * @return true if all bytes were written successfully.
     */
    bool writeString(const char* str) {
        uint8_t len = 0;
        while (str[len] != '\0') ++len;
        return static_cast<uint8_t>(serial.print(str)) == len;
    }

    // -------------------------------------------------------------------------
    // Receive
    // -------------------------------------------------------------------------

    /**
     * @brief Return the number of bytes available to read.
     *
     * @return Number of bytes waiting in the receive buffer.
     */
    uint8_t available() {
        return static_cast<uint8_t>(serial.available());
    }

    /**
     * @brief Read a single byte from the receive buffer.
     *
     * @return Result carrying the byte and a Status. Status::Error means no
     *         data was available or a transport error occurred.
     */
    Result<uint8_t, Status> read() {
        int value = serial.read();
        if (value < 0) {
            return {0, Status::Error};
        }
        return {static_cast<uint8_t>(value), Status::Ok};
    }

    /**
     * @brief Read up to @p len bytes into @p buf without blocking.
     *
     * Reads however many bytes are currently available, up to @p len.
     *
     * @param buf  Destination buffer (must be at least @p len bytes).
     * @param len  Maximum number of bytes to read.
     * @return Number of bytes actually read.
     */
    uint8_t readBytes(uint8_t* buf, uint8_t len) {
        return static_cast<uint8_t>(serial.readBytes(reinterpret_cast<char*>(buf), len));
    }

    /**
     * @brief Return the next byte in the receive buffer without consuming it.
     *
     * @return Result carrying the peeked byte and a Status. Status::Error
     *         means no data is available.
     */
    Result<uint8_t, Status> peek() {
        int value = serial.peek();
        if (value < 0) {
            return {0, Status::Error};
        }
        return {static_cast<uint8_t>(value), Status::Ok};
    }

    // -------------------------------------------------------------------------
    // Control
    // -------------------------------------------------------------------------

    /**
     * @brief Flush the transmit buffer.
     *
     * Blocks until all pending transmit bytes have been sent.
     */
    void flush() {
        serial.flush();
    }

private:
    HardwareSerial& serial;
};
