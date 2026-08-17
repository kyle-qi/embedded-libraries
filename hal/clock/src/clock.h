#pragma once

#include <stdint.h>
#include <Arduino.h>

/**
 * @file clock.h
 * @brief Arduino-backed timing and delay helper.
 *
 * Wraps the Arduino runtime functions millis(), micros(), delay(), and
 * delayMicroseconds(). Instantiate once and inject into any driver that
 * needs timing.
 *
 * @code
 * #include "clock.h"
 * #include "qmc5883l.h"
 *
 * ArduinoClock clock;
 * QMC5883L mag(bus, clock);
 * @endcode
 */
class ArduinoClock {
public:
    /**
     * @brief Return the number of milliseconds since the system started.
     *
     * Wraps to zero after approximately 49.7 days (2^32 ms).
     *
     * @return Elapsed milliseconds as an unsigned 32-bit value.
     */
    uint32_t millis() {
        return ::millis();
    }

    /**
     * @brief Return the number of microseconds since the system started.
     *
     * Wraps to zero after approximately 71.6 minutes (2^32 us).
     *
     * @return Elapsed microseconds as an unsigned 32-bit value.
     */
    uint32_t micros() {
        return ::micros();
    }

    /**
     * @brief Block execution for at least @p ms milliseconds.
     *
     * @note This is a blocking call. Avoid in interrupt context or
     *       time-critical code paths. For non-blocking timing use
     *       millis() and track elapsed time manually.
     *
     * @param ms Number of milliseconds to delay.
     */
    void delayMs(uint32_t ms) {
        ::delay(ms);
    }

    /**
     * @brief Block execution for at least @p us microseconds.
     *
     * @note Accuracy below ~10 us is platform-dependent.
     *
     * @param us Number of microseconds to delay.
     */
    void delayUs(uint32_t us) {
        ::delayMicroseconds(us);
    }

    /**
     * @brief Return true once @p intervalMs has elapsed since @p lastMs.
     *
     * Designed for non-blocking periodic execution:
     * @code
     * uint32_t last = 0;
     * if (clock.elapsed(last, 100)) {
     *     last = clock.millis();
     *     // runs every 100 ms
     * }
     * @endcode
     *
     * Handles the 32-bit millisecond rollover correctly.
     *
     * @param lastMs     Timestamp of the last event (milliseconds).
     * @param intervalMs Desired interval in milliseconds.
     * @return true if the interval has elapsed, false otherwise.
     */
    bool elapsed(uint32_t lastMs, uint32_t intervalMs) {
        // Subtraction handles 32-bit rollover correctly without branching
        return (::millis() - lastMs) >= intervalMs;
    }
};
