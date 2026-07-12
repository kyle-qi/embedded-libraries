#pragma once

#include <stdint.h>
#include "serial.h"

// Buffer to store Neo6M reading
#define NEO6M_BUFFER_SIZE 128

/**
 * @file neo6m.h
 * @brief Driver for the u-blox NEO-6M GPS module.
 *
 * Inject an ISerial implementation at construction time. The driver is
 * platform-agnostic and has no dependency on Arduino or any specific HAL.
 * Serial port initialization (baud rate, pins) is the caller's responsibility
 * before passing the port to this driver.
 */
class Neo6M {
public:

    /**
     * @brief Construct a Neo6M driver.
     *
     * The serial port must already be initialized (begin() called) before
     * any read operations are performed.
     *
     * @param serial Reference to the platform serial implementation.
     */
    explicit Neo6M(ISerial& serial);

    /**
     * @brief Reads available NMEA sentences and updates internal state.
     *
     * Consumes all bytes currently in the receive buffer. Returns true if
     * a complete, recognized GPGGA sentence was parsed in this call.
     *
     * @return true if a GPGGA sentence was successfully parsed.
     */
    bool read();

    /**
     * @brief Returns true if bytes are available in the serial receive buffer.
     *
     * @return true if at least one byte is waiting to be read.
     */
    bool isDRDY();

    /**
     * @brief Returns the most recent hour reading (UTC).
     */
    uint8_t getHour() const { return this->hour; }

    /**
     * @brief Returns the most recent minute reading (UTC).
     */
    uint8_t getMinute() const { return this->minute; }

    /**
     * @brief Returns the most recent second reading (UTC).
     */
    uint8_t getSecond() const { return this->second; }

    /**
     * @brief Returns the most recent latitude in decimal degrees.
     *
     * Negative values indicate South.
     */
    float getLatitude() const { return this->latitude; }

    /**
     * @brief Returns the most recent longitude in decimal degrees.
     *
     * Negative values indicate West.
     */
    float getLongitude() const { return this->longitude; }

    /**
     * @brief Returns the most recent horizontal dilution of precision.
     */
    float getHorizontalDilution() const { return this->horizontalDilution; }

    /**
     * @brief Returns the most recent altitude above mean sea level in metres.
     */
    float getAltitude() const { return this->altitude; }

    /**
     * @brief Returns the most recent geoid height in metres.
     */
    float getGeoidHeight() const { return this->geoidHeight; }

    /**
     * @brief Returns the most recent satellite count.
     */
    uint8_t getNumSatelites() const { return this->numSatelites; }

    /**
     * @brief Returns the raw text of the last received NMEA sentence.
     *
     * Useful for debugging. The returned pointer is valid until the next
     * call to read().
     *
     * @return Null-terminated C string containing the last sentence.
     */
    const char* getLastSentence() const { return this->lastSentence; }

private:

    /**
     * @brief Reference to the platform serial implementation.
     */
    ISerial& serial;

    // Time (UTC)
    uint8_t hour, minute, second;

    // Position
    float latitude, longitude;
    float horizontalDilution;
    float altitude;
    float geoidHeight;
    uint8_t numSatelites;

    // Raw last sentence (for debug)
    char lastSentence[NEO6M_BUFFER_SIZE];

    // Persistent receive buffer across read() calls
    char    rxBuf[NEO6M_BUFFER_SIZE];
    uint8_t rxIdx;
};
