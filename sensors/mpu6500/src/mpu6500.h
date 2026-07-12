#pragma once

#include "mpu6500_defs.h"
#include "i2c.h"
#include "result.h"

/**
 * @file mpu6500.h
 * @brief Driver for the MPU6500 6-axis IMU (accelerometer + gyroscope).
 *
 * Inject an II2C implementation at construction time. The driver is
 * platform-agnostic and has no dependency on Arduino or any specific HAL.
 */
class MPU6500 {
public:

    /**
     * @brief Construct an MPU6500 driver.
     *
     * LSB resolutions are initialised to power-on reset defaults:
     * ±250 dps for the gyroscope and ±2 g for the accelerometer.
     * Call setGyroRange() / setAccelRange() to change them.
     *
     * @param bus       Reference to the platform I2C implementation.
     * @param address   7-bit I2C address (default MPU6500_I2C_ADDR_AD0_LOW).
     */
    explicit MPU6500(II2C& bus, uint8_t address = MPU6500_I2C_ADDR_AD0_LOW);

    // -------------------------------------------------------------------------
    // Configuration
    // -------------------------------------------------------------------------

    /**
     * @brief Set factory-trim gyroscope offsets.
     *
     * @param offsetX X-axis offset (raw LSB).
     * @param offsetY Y-axis offset (raw LSB).
     * @param offsetZ Z-axis offset (raw LSB).
     * @return true on success.
     */
    bool setGyroOffset(int16_t offsetX, int16_t offsetY, int16_t offsetZ);

    /**
     * @brief Set the sample rate divider.
     *
     * Output rate = internal gyro rate / (1 + divider).
     * Use MPU6500_SMPLRT_DIV_* macros for common rates.
     *
     * @param divider Value written to SMPLRT_DIV [0–255].
     * @return true on success.
     */
    bool setSampleRateDivider(uint8_t divider);

    /**
     * @brief Set the FIFO overflow behaviour.
     *
     * @param stopWhenFull MPU6500_FIFO_STOP_WHEN_FULL or MPU6500_FIFO_OVERWRITE_OLDEST.
     * @return true on success.
     */
    bool setFIFOMode(bool stopWhenFull = MPU6500_FIFO_STOP_WHEN_FULL);

    /**
     * @brief External frame synchronisation source.
     */
    enum ExtSyncSource : uint8_t {
        EXT_SOURCE_DISABLE = 0,
        EXT_SOURCE_TEMP    = 1,
        EXT_SOURCE_GYRO_X  = 2,
        EXT_SOURCE_GYRO_Y  = 3,
        EXT_SOURCE_GYRO_Z  = 4,
        EXT_SOURCE_ACCEL_X = 5,
        EXT_SOURCE_ACCEL_Y = 6,
        EXT_SOURCE_ACCEL_Z = 7
    };

    /**
     * @brief Configure the FSYNC pin input source.
     *
     * @param source One of the ExtSyncSource values.
     * @return true on success.
     */
    bool setFSync(ExtSyncSource source = EXT_SOURCE_DISABLE);

    /**
     * @brief Configure and optionally enable the gyroscope DLPF.
     *
     * When @p isOn is false the LPF is bypassed (FCHOICE_B = 0b01).
     * When true, @p bandwidth selects the cut-off frequency.
     *
     * @param isOn      MPU6500_SENSOR_ENABLE or MPU6500_SENSOR_DISABLE.
     * @param bandwidth MPU6500_GYRO_DLPF_BW_* macro (Hz).
     * @return true on success.
     */
    bool setGyroLPF(bool isOn = MPU6500_SENSOR_ENABLE,
                    uint16_t bandwidth = MPU6500_GYRO_DLPF_BW_250_HZ);

    /**
     * @brief Enable or disable the gyroscope DLPF without changing bandwidth.
     *
     * @param isOn true to enable, false to bypass.
     * @return true on success.
     */
    bool gyroLPFEnable(bool isOn = MPU6500_SENSOR_ENABLE);

    /**
     * @brief Set the gyroscope full-scale range.
     *
     * Also updates the internal LSB-to-rad/s scale factor.
     *
     * @param range MPU6500_GYRO_FS_*DPS macro.
     * @return true on success.
     */
    bool setGyroRange(uint16_t range = MPU6500_GYRO_FS_250DPS);

    /**
     * @brief Configure and optionally enable the accelerometer DLPF.
     *
     * @param isOn      MPU6500_SENSOR_ENABLE or MPU6500_SENSOR_DISABLE.
     * @param bandwidth MPU6500_ACCEL_DLPF_BW_* macro (Hz).
     * @return true on success.
     */
    bool setAccelLPF(bool isOn,
                     uint16_t bandwidth = MPU6500_ACCEL_DLPF_BW_460_HZ);

    /**
     * @brief Enable or disable the accelerometer DLPF without changing bandwidth.
     *
     * @param isOn true to enable, false to bypass.
     * @return true on success.
     */
    bool accelLPFEnable(bool isOn = MPU6500_SENSOR_ENABLE);

    /**
     * @brief Set the accelerometer full-scale range.
     *
     * Also updates the internal LSB-to-m/s² scale factor.
     *
     * @param range MPU6500_ACCEL_FS_*G macro.
     * @return true on success.
     */
    bool setAccelRange(uint8_t range = MPU6500_ACCEL_FS_2G);

    // -------------------------------------------------------------------------
    // Power management
    // -------------------------------------------------------------------------

    /**
     * @brief Reset all registers to power-on defaults.
     *
     * The device enters sleep mode after reset. Call wake() before
     * issuing further configuration commands.
     *
     * @return true on success.
     */
    bool resetRegisters();

    /**
     * @brief Apply the accel-only default profile and wake the device.
     *
     * Sequence: reset → wake → sample rate → FIFO mode → FSync off →
     * accel LPF (44 Hz) → accel range (±2 g) → gyros off → temp off.
     *
     * @return true if every step succeeded.
     */
    bool configureDefaults();

    /**
     * @brief Put the device into sleep mode.
     * @return true on success.
     */
    bool sleep();

    /**
     * @brief Wake the device from sleep mode.
     * @return true on success.
     */
    bool wake();

    /**
     * @brief Enable or disable the internal temperature sensor.
     * @param isEnable true to enable, false to disable.
     * @return true on success.
     */
    bool enableTempSense(bool isEnable);

    /**
     * @brief Enable or disable all three accelerometer axes together.
     * @param isEnable true to enable, false to disable.
     * @return true on success.
     */
    bool enableAccel(bool isEnable);

    /**
     * @brief Enable or disable all three gyroscope axes together.
     * @param isEnable true to enable, false to disable.
     * @return true on success.
     */
    bool enableGyro(bool isEnable);

    // -------------------------------------------------------------------------
    // Data ready
    // -------------------------------------------------------------------------

    /**
     * @brief Check whether the sensor has a new sample ready.
     * @return true if the DATA_RDY interrupt flag is set. A failed read
     *         returns false.
     */
    bool isDRDY();

    // -------------------------------------------------------------------------
    // Sensor reads — individual axes
    // -------------------------------------------------------------------------

    /** @return Raw 16-bit gyroscope X reading (and updates getGyroX()). */
    Result<int16_t, bool> readGyroX();
    /** @return Raw 16-bit gyroscope Y reading (and updates getGyroY()). */
    Result<int16_t, bool> readGyroY();
    /** @return Raw 16-bit gyroscope Z reading (and updates getGyroZ()). */
    Result<int16_t, bool> readGyroZ();

    /** @return Raw 16-bit accelerometer X reading (and updates getAccelX()). */
    Result<int16_t, bool> readAccelX();
    /** @return Raw 16-bit accelerometer Y reading (and updates getAccelY()). */
    Result<int16_t, bool> readAccelY();
    /** @return Raw 16-bit accelerometer Z reading (and updates getAccelZ()). */
    Result<int16_t, bool> readAccelZ();

    /** @return Raw 16-bit temperature reading (and updates getTemp()). */
    Result<int16_t, bool> readTemp();

    // -------------------------------------------------------------------------
    // Sensor reads — burst (all axes in one I2C transaction)
    // -------------------------------------------------------------------------

    /**
     * @brief Read all three gyroscope axes in a single 6-byte burst.
     *
     * Guarantees all three values come from the same sample. Prefer this
     * over calling readGyroX/Y/Z individually.
     *
     * @return true if the burst read succeeded. On failure the stored values
     *         are left unchanged.
     */
    bool readGyro();

    /**
     * @brief Read all three accelerometer axes in a single 6-byte burst.
     *
     * Guarantees all three values come from the same sample. Prefer this
     * over calling readAccelX/Y/Z individually.
     *
     * @return true if the burst read succeeded. On failure the stored values
     *         are left unchanged.
     */
    bool readAccel();

    // -------------------------------------------------------------------------
    // Getters — scaled values
    // -------------------------------------------------------------------------

    /** @return Most recent gyroscope X reading in rad/s. */
    float getGyroX() const { return xGyro; }
    /** @return Most recent gyroscope Y reading in rad/s. */
    float getGyroY() const { return yGyro; }
    /** @return Most recent gyroscope Z reading in rad/s. */
    float getGyroZ() const { return zGyro; }

    /** @return Most recent accelerometer X reading in m/s². */
    float getAccelX() const { return xAccel; }
    /** @return Most recent accelerometer Y reading in m/s². */
    float getAccelY() const { return yAccel; }
    /** @return Most recent accelerometer Z reading in m/s². */
    float getAccelZ() const { return zAccel; }

    /** @return Most recent temperature reading in °C. */
    float getTemp() const { return temp; }

    // -------------------------------------------------------------------------
    // Identity
    // -------------------------------------------------------------------------

    /**
     * @brief Read the WHO_AM_I register.
     *
     * Expected value: MPU6500_WHO_AM_I_VAL (0x70).
     *
     * @return Result carrying the register contents and a success status.
     */
    Result<uint8_t, bool> whoAmI();

private:
    
    // I2C information
    II2C&   bus;
    uint8_t address;

    // Scaled outputs
    float xGyro,  yGyro,  zGyro;   ///< rad/s
    float xAccel, yAccel, zAccel;  ///< m/s²
    float temp;                    ///< °C

    // LSB scale factors — kept in sync with setGyroRange / setAccelRange
    float lsbResGyro;   ///< rad/s per LSB
    float lsbResAccel;  ///< m/s² per LSB

    enum class SensorChannel : uint8_t { Gyro, Accel, Temp };

    /**
     * @brief Read two consecutive big-endian bytes from @p reg, scale, and store.
     *
     * @return Result carrying the raw value and a success status. On failure
     *         the stored scaled value is left unchanged.
     */
    Result<int16_t, bool> readSensor(uint8_t reg, float& scaledOut, SensorChannel channel);

    /**
     * @brief Assemble a big-endian int16 from two raw bytes.
     */
    static int16_t toInt16BE(uint8_t msb, uint8_t lsb) {
        return static_cast<int16_t>((static_cast<uint16_t>(msb) << 8) | lsb);
    }
};
