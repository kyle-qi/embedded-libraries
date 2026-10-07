#pragma once

#include "qmc5883l_defs.h"
#include "i2c.h"
#include "clock.h"
#include "result.h"

class QMC5883L{
    public:

    /**
     * @brief Class constructor.
     *
     * @param bus       Reference to the I2C bus.
     * @param clock     Reference to the clock used for mode-switch delays.
     * @param myAddress The I2C address of the device (default @ref QMC5883L_I2C_ADDR_PIN_LOW).
     */
    explicit QMC5883L(ArduinoI2C& bus, ArduinoClock& clock, uint8_t myAddress = QMC5883L_I2C_ADDR_PIN_LOW);

    /**
     * @brief Operating mode of the magnetometer.
     */
    enum class Mode : uint8_t {
        Suspend    = 0,
        Normal     = 1,
        Single     = 2,
        Continuous = 3,
    };

    /**
     * @brief Sets the magnetometer's operating mode.
     * 
     * @param mode The desired mode.
     * @return true if the configuration is successful, false otherwise.
     */
    bool setMode(Mode mode);

    /**
     * @brief Output data rate of the magnetometer.
     */
    enum class OutputRate : uint8_t {
        Hz10  = 0,
        Hz50  = 1,
        Hz100 = 2,
        Hz200 = 3,
    };

    /**
     * @brief Sets the magnetometer's data output frequency.
     * 
     * @param odr Desired output data rate. Default: @ref OutputRate::Hz10.
     * @return true if the configuration is successful, false otherwise.
     */
    bool setOutputRate(OutputRate odr = OutputRate::Hz10);

    /**
     * @brief Over-sample rate ratio of the magnetometer.
     */
    enum class OverSampleRate : uint8_t {
        x8 = 0,
        x4 = 1,
        x2 = 2,
        x1 = 3,
    };

    /**
     * @brief Sets the magnetometer's over-sample rate ratio.
     * 
     * @param osr Desired over-sample ratio. Default: @ref OverSampleRate::x2.
     * @return true if the configuration is successful, false otherwise.
     */
    bool setOverSampleRate(OverSampleRate osr = OverSampleRate::x2);

    /**
     * @brief Down-sample rate ratio of the magnetometer.
     */
    enum class DownSampleRate : uint8_t {
        x1 = 0,
        x2 = 1,
        x4 = 2,
        x8 = 3,
    };

    /**
     * @brief Sets the magnetometer's down-sample rate ratio.
     * 
     * @param osr Desired down-sample ratio. Default: @ref DownSampleRate::x4.
     * @return true if the configuration is successful, false otherwise.
     */
    bool setDownSampleRate(DownSampleRate osr = DownSampleRate::x4);

    /**
     * @brief Full-scale magnetic range of the magnetometer.
     */
    enum class Range : uint8_t {
        Gauss30 = 0,
        Gauss12 = 1,
        Gauss8  = 2,
        Gauss2  = 3,
    };

    /**
     * @brief Sets the magnetometer's full-scale magnetic range.
     * 
     * Also updates the internal LSB-to-Gauss scale factor.
     *
     * @param range Desired range. Default: @ref Range::Gauss2.
     * @return true if the configuration is successful, false otherwise.
     */
    bool setRange(Range range = Range::Gauss2);

    /**
     * @brief Set/reset mode of the magnetometer.
     */
    enum class SetResetMode : uint8_t {
        SetAndReset = 0,
        SetOnly     = 1,
        Off         = 2,
    };

    /**
     * @brief Sets the magnetometer's set/reset mode.
     * 
     * @param mode The desired mode.
     * @return true if the configuration is successful, false otherwise.
     */
    bool setSetResetMode(SetResetMode mode);

    /**
     * @brief Resets the magnetometer's registers to its default values.
     * 
     * @return true if the operation is successful, false otherwise.
     */
    bool resetRegisters();

    /**
     * @brief Continuous heading profile: reset, continuous mode, 10 Hz ODR,
     *        oversample x8, downsample x8, 8 Gauss range, set-only mode.
     *
     * @return true if every step succeeded, false on the first failure.
     */
    bool configureDefaults();

    /** 
     * @brief Tells you if the magnetometer has data ready.
     *
     * @return Result where `value` is the DRDY flag and `status` indicates
     *         whether the I2C read itself succeeded. Check `!r` for bus
     *         errors before using `r.value`.
     */
    Result<bool, Status> isDRDY();

    /**
     * @brief Indicates if the reading exceeds -30,000 to 30,000 LSBs. Register resets when read.
     *
     * @return Result where `value` is the overflow flag and `status`
     *         indicates whether the I2C read succeeded.
     */
    Result<bool, Status> isOVFL();

    /**
     * @brief Read all three magnetometer axes in a single 6-byte burst.
     *
     * Guarantees X, Y, and Z come from the same sample. On failure the
     * previously stored values are left unchanged.
     *
     * @return true if the I2C read succeeded, false otherwise.
     */
    bool read();

    // TODO: This is a stub for now.
    /**
     * @brief Normalizes the raw magnetometer reading to the range [-1, 1].
     *
     * @param rawX The raw X reading.
     * @param rawY The raw Y reading.
     * @param rawZ The raw Z reading.
     * @return The normalized reading in the range [-1, 1].
     */
    float normalize(int16_t rawX, int16_t rawY, int16_t rawZ);

    /**
     * @brief Obtains the most recent magnetometer normalized x reading.
     * 
     * @return The most recent normalized x reading in [-1, 1].
     */
    float getX() const { return this->x; }

    /**
     * @brief Obtains the most recent magnetometer normalized y reading.
     * 
     * @return The most recent normalized y reading in [-1, 1].
     */
    float getY() const { return this->y; }

    /**
     * @brief Obtains the most recent magnetometer normalized z reading.
     * 
     * @return The most recent normalized z reading in [-1, 1].
     */
    float getZ() const { return this->z; }

    /**
     * @brief Obtains the most recent magnetometer x reading in Gauss.
     */
    float getXGauss() const { return this->xGauss; }

    /**
     * @brief Obtains the most recent magnetometer y reading in Gauss.
     */
    float getYGauss() const { return this->yGauss; }

    /**
     * @brief Obtains the most recent magnetometer z reading in Gauss.
     */
    float getZGauss() const { return this->zGauss; }

    private:

    /**
     * @brief Reference to the I2C bus.
     */
    ArduinoI2C& bus;

    /**
     * @brief Reference to the clock used for mode-switch delays.
     */
    ArduinoClock& clock;

    /** 
     * @brief I2C address of the device.
     */
    uint8_t address;

    /**
     * @brief Maximum magnetometer reading in the indicated axis.
     */
    int16_t xMax, yMax, zMax;

     /**
     * @brief Minimum magnetometer reading in the indicated axis.
     */
    int16_t xMin, yMin, zMin;

    /**
     * @brief The most recent magnetometer reading in the indicated axis, normalized to [-1, 1].
     */
    float x, y, z;

    /**
     * @brief The most recent magnetometer reading in the indicated axis in Gauss.
     */
    float xGauss, yGauss, zGauss;

    /**
     * @brief The LSB-to-Gauss scale factor, set by setRange().
     */
    float lsbRes;

    /**
     * @brief Assemble a little-endian int16 from two raw bytes.
     */
    static int16_t toInt16LE(uint8_t lsb, uint8_t msb) {
        return static_cast<int16_t>((static_cast<uint16_t>(msb) << 8) | lsb);
    }
};
