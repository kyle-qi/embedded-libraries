#pragma once

#include <stdint.h>
#include "result.h"

/**
 * @file serial.h
 * @brief Platform-agnostic abstract serial interface.
 *
 * Defines the minimum set of operations a serial implementation must
 * provide. This interface covers byte-oriented, full-duplex communication
 * and is intentionally transport-agnostic — it can be backed by a hardware
 * UART peripheral, a USB-CDC virtual COM port, or any other byte stream.
 *
 * Code that needs serial I/O depends only on this header. Concrete
 * implementations (e.g. ArduinoSerial) are injected at construction time,
 * keeping library code free of any platform dependencies.
 */
class ISerial {
public:
    virtual ~ISerial() = default;

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
    virtual bool write(uint8_t byte) = 0;

    /**
     * @brief Write @p len bytes from @p buf.
     *
     * @param buf Source buffer (must be at least @p len bytes).
     * @param len Number of bytes to write.
     * @return Number of bytes successfully written. A value less than @p len
     *         indicates a partial write or error.
     */
    virtual uint8_t writeBytes(const uint8_t* buf, uint8_t len) = 0;

    /**
     * @brief Write a null-terminated string.
     *
     * Does not transmit the terminating null byte.
     *
     * @param str Pointer to a null-terminated C string.
     * @return true if all bytes were written successfully.
     */
    virtual bool writeString(const char* str) = 0;

    // -------------------------------------------------------------------------
    // Receive
    // -------------------------------------------------------------------------

    /**
     * @brief Return the number of bytes available to read.
     *
     * @return Number of bytes waiting in the receive buffer.
     */
    virtual uint8_t available() = 0;

    /**
     * @brief Read a single byte from the receive buffer.
     *
     * @return Result carrying the byte read and a success status. If no data
     *         is available the status is false and the value is unspecified.
     */
    virtual Result<uint8_t, bool> read() = 0;

    /**
     * @brief Read up to @p len bytes into @p buf without blocking.
     *
     * Reads however many bytes are currently available, up to @p len.
     *
     * @param buf  Destination buffer (must be at least @p len bytes).
     * @param len  Maximum number of bytes to read.
     * @return Number of bytes actually read.
     */
    virtual uint8_t readBytes(uint8_t* buf, uint8_t len) = 0;

    /**
     * @brief Return the next byte in the receive buffer without consuming it.
     *
     * @return Result carrying the peeked byte and a success status. If no data
     *         is available the status is false and the value is unspecified.
     */
    virtual Result<uint8_t, bool> peek() = 0;

    // -------------------------------------------------------------------------
    // Control
    // -------------------------------------------------------------------------

    /**
     * @brief Flush the transmit buffer.
     *
     * Blocks until all pending transmit bytes have been sent.
     */
    virtual void flush() = 0;
};
