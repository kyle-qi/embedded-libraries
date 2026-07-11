#pragma once

#include <stdint.h>

/**
 * @file clock.h
 * @brief Platform-agnostic abstract clock interface.
 *
 * Provides timing and delay services to HAL and sensor drivers without
 * coupling them to any platform (Arduino, STM32 HAL, POSIX, etc.).
 *
 * Inject a concrete implementation (e.g. ArduinoClock) at construction
 * time wherever timing is needed.
 */
class IClock {
public:
    virtual ~IClock() = default;

    /**
     * @brief Return the number of milliseconds since the system started.
     *
     * Wraps to zero after approximately 49.7 days (2^32 ms).
     *
     * @return Elapsed milliseconds as an unsigned 32-bit value.
     */
    virtual uint32_t millis() = 0;

    /**
     * @brief Return the number of microseconds since the system started.
     *
     * Wraps to zero after approximately 71.6 minutes (2^32 µs).
     *
     * @return Elapsed microseconds as an unsigned 32-bit value.
     */
    virtual uint32_t micros() = 0;

    /**
     * @brief Block execution for at least @p ms milliseconds.
     *
     * @note This is a blocking call. Avoid in interrupt context or
     *       time-critical code paths. For non-blocking timing use
     *       millis() and track elapsed time manually.
     *
     * @param ms Number of milliseconds to delay.
     */
    virtual void delayMs(uint32_t ms) = 0;

    /**
     * @brief Block execution for at least @p us microseconds.
     *
     * @note Accuracy below ~10 µs is platform-dependent.
     *
     * @param us Number of microseconds to delay.
     */
    virtual void delayUs(uint32_t us) = 0;

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
    virtual bool elapsed(uint32_t lastMs, uint32_t intervalMs) = 0;
};
