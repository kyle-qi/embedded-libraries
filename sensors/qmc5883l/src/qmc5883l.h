#pragma once

#include "qmc5883l_defs.h"
#include "i2c.h"
#include "clock.h"

class QMC5883L{
    public:

    /**
     * @brief Class constructor.
     *
     * @param bus       Reference to the platform I2C implementation.
     * @param clock     Reference to the platform clock implementation.
     * @param myAddress The I2C address of the device (default @ref QMC5883L_I2C_ADDR_PIN_LOW).
     */
    explicit QMC5883L(II2C& bus, IClock& clock, uint8_t myAddress = QMC5883L_I2C_ADDR_PIN_LOW);

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
     * @return true if the magnetometer has new data ready, false otherwise.
     */
    bool isDRDY();

    /**
     * @brief Indicates if the reading exceeds -30,000 to 30,000 LSBs. Register resets when read.
     * 
     * @return true if an output exceeds the range, false otherwise.
     */
    bool isOVFL();

    /**
     * @brief Reads all three magnetometer axes and updates internal state.
     *
     * @return true if the I2C read succeeded for all axes, false otherwise.
     *         On failure the previously stored values are unchanged.
     */
    bool read();

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

    /**
     * @brief Calculates the azimuth/heading from normalized X and Y readings.
     * 
     * @param xNorm The normalized X reading of the magnetometer [-1, 1].
     * @param yNorm The normalized Y reading of the magnetometer [-1, 1].
     * @return The azimuth/heading in degrees [0, 360).
     */
    float azimuth(float xNorm, float yNorm) const;

    /**
     * @brief Obtain the magnetometer's maximum X axis reading.
     * 
     * @return The magnetometer's maximum X axis reading.
     */
    int16_t getXMax() const { return this->xMax; }

    /**
     * @brief Obtain the magnetometer's maximum Y axis reading.
     * 
     * @return The magnetometer's maximum Y axis reading.
     */ 
    int16_t getYMax() const { return this->yMax; }

    /**
     * @brief Obtain the magnetometer's maximum Z axis reading.
     * 
     * @return The magnetometer's maximum Z axis reading.
     */    
    int16_t getZMax() const { return this->zMax; }

    /**
     * @brief Obtain the magnetometer's minimum X axis reading.
     * 
     * @return The magnetometer's minimum X axis reading.
     */
    int16_t getXMin() const { return this->xMin; }

    /**
     * @brief Obtain the magnetometer's minimum Y axis reading.
     * 
     * @return The magnetometer's minimum Y axis reading.
     */
    int16_t getYMin() const { return this->yMin; }

    /**
     * @brief Obtain the magnetometer's minimum Z axis reading.
     * 
     * @return The magnetometer's minimum Z axis reading.
     */
    int16_t getZMin() const { return this->zMin; }

    private:

    /** 
     * @brief Reference to the platform I2C implementation.
     */
    II2C& bus;

    /**
     * @brief Reference to the platform clock implementation.
     */
    IClock& clock;

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
     * @brief Reads raw magnetometer data from a starting register (little-endian pair),
     *        scales it, and updates internal state.
     *
     * @param reg          Base register address (LSB register).
     * @param normStorage  Where to store the normalized [-1, 1] reading.
     * @param gaussStorage Where to store the Gauss reading.
     * @param maxVal       Calibration maximum for this axis.
     * @param minVal       Calibration minimum for this axis.
     * @return true if the I2C read succeeded, false otherwise.
     */
    bool readAxis(uint8_t reg, float& normStorage, float& gaussStorage, int16_t maxVal, int16_t minVal);

