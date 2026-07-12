#include "mpu6500.h"
#include "i2c_utils.h"
#include <math.h>

// Power-on reset defaults: ±250 dps, ±2 g
static constexpr float kDefaultLsbResGyro  = (250.0f  * (float)M_PI / 180.0f) / 32768.0f;
static constexpr float kDefaultLsbResAccel = (2.0f    * GRAVITY)              / 32768.0f;

MPU6500::MPU6500(II2C& bus, uint8_t address)
    : bus(bus)
    , address(address)
    , xGyro(0.0f),  yGyro(0.0f),  zGyro(0.0f)
    , xAccel(0.0f), yAccel(0.0f), zAccel(0.0f)
    , temp(0.0f)
    , lsbResGyro(kDefaultLsbResGyro)
    , lsbResAccel(kDefaultLsbResAccel)
{}

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------

bool MPU6500::setGyroOffset(int16_t offsetX, int16_t offsetY, int16_t offsetZ) {
    // Bits [1:0] of each LSB register are reserved — preserve them
    const uint8_t lsbMask = 0b11111100;
    bool ok = true;
    ok &= bus.write(address, MPU6500_XG_OFFSET_H, static_cast<uint8_t>(offsetX >> 8));
    ok &= bus.write(address, MPU6500_YG_OFFSET_H, static_cast<uint8_t>(offsetY >> 8));
    ok &= bus.write(address, MPU6500_ZG_OFFSET_H, static_cast<uint8_t>(offsetZ >> 8));
    ok &= i2c::writeMasked(bus, address, MPU6500_XG_OFFSET_L, static_cast<uint8_t>(offsetX), lsbMask);
    ok &= i2c::writeMasked(bus, address, MPU6500_YG_OFFSET_L, static_cast<uint8_t>(offsetY), lsbMask);
    ok &= i2c::writeMasked(bus, address, MPU6500_ZG_OFFSET_L, static_cast<uint8_t>(offsetZ), lsbMask);
    return ok;
}

bool MPU6500::setSampleRateDivider(uint8_t divider) {
    return bus.write(address, MPU6500_SMPLRT_DIV, divider);
}

bool MPU6500::setFIFOMode(bool stopWhenFull) {
    return i2c::writeBit(bus, address, MPU6500_CONFIG, stopWhenFull,
                         MPU6500_CONFIG_FIFO_MODE_BIT);
}

bool MPU6500::setFSync(ExtSyncSource source) {
    uint8_t bits = static_cast<uint8_t>(source) << MPU6500_CONFIG_EXT_SYNC_SET_SHIFT;
    return i2c::writeMasked(bus, address, MPU6500_CONFIG, bits,
                            MPU6500_CONFIG_EXT_SYNC_SET_MASK);
}

bool MPU6500::setGyroLPF(bool isOn, uint16_t bandwidth) {
    if (!gyroLPFEnable(isOn)) return false;
    if (!isOn) return true;

    uint8_t bits;
    switch (bandwidth) {
        case MPU6500_GYRO_DLPF_BW_250_HZ:  bits = 0; break;
        case MPU6500_GYRO_DLPF_BW_184_HZ:  bits = 1; break;
        case MPU6500_GYRO_DLPF_BW_92_HZ:   bits = 2; break;
        case MPU6500_GYRO_DLPF_BW_41_HZ:   bits = 3; break;
        case MPU6500_GYRO_DLPF_BW_20_HZ:   bits = 4; break;
        case MPU6500_GYRO_DLPF_BW_10_HZ:   bits = 5; break;
        case MPU6500_GYRO_DLPF_BW_5_HZ:    bits = 6; break;
        case MPU6500_GYRO_DLPF_BW_3600_HZ: bits = 7; break;
        default: return false;
    }
    return i2c::writeMasked(bus, address, MPU6500_CONFIG, bits,
                            MPU6500_CONFIG_DLPF_CFG_MASK);
}

bool MPU6500::gyroLPFEnable(bool isOn) {
    // FCHOICE_B: 0b00 = LPF active, 0b01 = LPF bypassed
    uint8_t bits = isOn ? 0b00u : 0b01u;
    return i2c::writeMasked(bus, address, MPU6500_GYRO_CONFIG, bits,
                            MPU6500_GYRO_CONFIG_FCHOICE_B_MASK);
}

bool MPU6500::setGyroRange(uint16_t range) {
    uint8_t bits;
    switch (range) {
        case MPU6500_GYRO_FS_250DPS:  bits = 0b00; break;
        case MPU6500_GYRO_FS_500DPS:  bits = 0b01; break;
        case MPU6500_GYRO_FS_1000DPS: bits = 0b10; break;
        case MPU6500_GYRO_FS_2000DPS: bits = 0b11; break;
        default: return false;
    }
    bits <<= MPU6500_GYRO_CONFIG_FS_SEL_SHIFT;
    // Scale factor: (full_scale_dps * π/180) / 32768 → rad/s per LSB
    lsbResGyro = (static_cast<float>(range) * (float)M_PI / 180.0f) / 32768.0f;
    return i2c::writeMasked(bus, address, MPU6500_GYRO_CONFIG, bits,
                            MPU6500_GYRO_CONFIG_FS_SEL_MASK);
}

bool MPU6500::setAccelLPF(bool isOn, uint16_t bandwidth) {
    if (!accelLPFEnable(isOn)) return false;
    if (!isOn) return true;

    uint8_t bits;
    switch (bandwidth) {
        case MPU6500_ACCEL_DLPF_BW_460_HZ: bits = 0; break;
        case MPU6500_ACCEL_DLPF_BW_184_HZ: bits = 1; break;
        case MPU6500_ACCEL_DLPF_BW_92_HZ:  bits = 2; break;
        case MPU6500_ACCEL_DLPF_BW_44_HZ:  bits = 3; break;
        case MPU6500_ACCEL_DLPF_BW_20_HZ:  bits = 4; break;
        case MPU6500_ACCEL_DLPF_BW_10_HZ:  bits = 5; break;
        case MPU6500_ACCEL_DLPF_BW_5_HZ:   bits = 6; break;
        default: return false;
    }
    return i2c::writeMasked(bus, address, MPU6500_ACCEL_CONFIG2, bits,
                            MPU6500_ACCEL_CONFIG2_DLPF_CFG_MASK);
}

bool MPU6500::accelLPFEnable(bool isOn) {
    // FCHOICE_B: 1 = bypass LPF, 0 = use LPF
    return i2c::writeBit(bus, address, MPU6500_ACCEL_CONFIG2, !isOn,
                         MPU6500_ACCEL_CONFIG2_FCHOICE_B_BIT);
}

bool MPU6500::setAccelRange(uint8_t range) {
    uint8_t bits;
    switch (range) {
        case MPU6500_ACCEL_FS_2G:  bits = 0b00; break;
        case MPU6500_ACCEL_FS_4G:  bits = 0b01; break;
        case MPU6500_ACCEL_FS_8G:  bits = 0b10; break;
        case MPU6500_ACCEL_FS_16G: bits = 0b11; break;
        default: return false;
    }
    bits <<= MPU6500_ACCEL_CONFIG_FS_SEL_SHIFT;
    // Scale factor: (full_scale_g * standard_gravity) / 32768 → m/s² per LSB
    lsbResAccel = (static_cast<float>(range) * GRAVITY) / 32768.0f;
    return i2c::writeMasked(bus, address, MPU6500_ACCEL_CONFIG, bits,
                            MPU6500_ACCEL_CONFIG_FS_SEL_MASK);
}

// -----------------------------------------------------------------------------
// Power management
// -----------------------------------------------------------------------------

bool MPU6500::resetRegisters() {
    return i2c::writeBit(bus, address, MPU6500_PWR_MGMT_1, true,
                         MPU6500_PWR1_DEVICE_RESET_BIT);
}

bool MPU6500::configureDefaults() {
    bool ok = true;
    ok &= resetRegisters();
    ok &= wake();  // device enters sleep after reset — wake before configuring
    ok &= setSampleRateDivider(MPU6500_SMPLRT_DIV_9);
    ok &= setFIFOMode(MPU6500_FIFO_OVERWRITE_OLDEST);
    ok &= setFSync(EXT_SOURCE_DISABLE);
    ok &= setAccelLPF(MPU6500_SENSOR_ENABLE, MPU6500_ACCEL_DLPF_BW_44_HZ);
    ok &= setAccelRange(MPU6500_ACCEL_FS_2G);
    ok &= enableGyro(MPU6500_SENSOR_DISABLE);
    ok &= enableTempSense(MPU6500_SENSOR_DISABLE);
    return ok;
}

bool MPU6500::sleep() {
    return i2c::writeBit(bus, address, MPU6500_PWR_MGMT_1, true,
                         MPU6500_PWR1_SLEEP_BIT);
}

bool MPU6500::wake() {
    return i2c::writeBit(bus, address, MPU6500_PWR_MGMT_1, false,
                         MPU6500_PWR1_SLEEP_BIT);
}

bool MPU6500::enableTempSense(bool isEnable) {
    // PD_PTAT: 1 = powered down, so invert
    return i2c::writeBit(bus, address, MPU6500_PWR_MGMT_1, !isEnable,
                         MPU6500_PWR1_PD_PTAT_BIT);
}

bool MPU6500::enableAccel(bool isEnable) {
    // DISABLE_XA/YA/ZA bits: 1 = off, so invert isEnable for all three at once
    const uint8_t mask = (1u << MPU6500_PWR2_DISABLE_XA_BIT) |
                         (1u << MPU6500_PWR2_DISABLE_YA_BIT) |
                         (1u << MPU6500_PWR2_DISABLE_ZA_BIT);
    const uint8_t bits = isEnable ? 0x00u : mask;
    return i2c::writeMasked(bus, address, MPU6500_PWR_MGMT_2, bits, mask);
}

bool MPU6500::enableGyro(bool isEnable) {
    const uint8_t mask = (1u << MPU6500_PWR2_DISABLE_XG_BIT) |
                         (1u << MPU6500_PWR2_DISABLE_YG_BIT) |
                         (1u << MPU6500_PWR2_DISABLE_ZG_BIT);
    const uint8_t bits = isEnable ? 0x00u : mask;
    return i2c::writeMasked(bus, address, MPU6500_PWR_MGMT_2, bits, mask);
}

// -----------------------------------------------------------------------------
// Data ready
// -----------------------------------------------------------------------------

bool MPU6500::isDRDY() {
    Result<uint8_t, bool> status = bus.read(address, MPU6500_INT_STATUS);
    if (!status) {
        return false; // treat a failed read as "not ready"
    }
    return (status.value & (1u << MPU6500_INT_DATA_RDY_BIT)) != 0;
}

// -----------------------------------------------------------------------------
// Sensor reads — individual axes
// -----------------------------------------------------------------------------

Result<int16_t, bool> MPU6500::readGyroX() {
    return readSensor(MPU6500_GYRO_XOUT_H, xGyro, SensorChannel::Gyro);
}
Result<int16_t, bool> MPU6500::readGyroY() {
    return readSensor(MPU6500_GYRO_YOUT_H, yGyro, SensorChannel::Gyro);
}
Result<int16_t, bool> MPU6500::readGyroZ() {
    return readSensor(MPU6500_GYRO_ZOUT_H, zGyro, SensorChannel::Gyro);
}

Result<int16_t, bool> MPU6500::readAccelX() {
    return readSensor(MPU6500_ACCEL_XOUT_H, xAccel, SensorChannel::Accel);
}
Result<int16_t, bool> MPU6500::readAccelY() {
    return readSensor(MPU6500_ACCEL_YOUT_H, yAccel, SensorChannel::Accel);
}
Result<int16_t, bool> MPU6500::readAccelZ() {
    return readSensor(MPU6500_ACCEL_ZOUT_H, zAccel, SensorChannel::Accel);
}

Result<int16_t, bool> MPU6500::readTemp() {
    return readSensor(MPU6500_TEMP_OUT_H, temp, SensorChannel::Temp);
}

// -----------------------------------------------------------------------------
// Sensor reads — burst
// -----------------------------------------------------------------------------

bool MPU6500::readGyro() {
    uint8_t buf[6];
    if (!bus.readBytes(address, MPU6500_GYRO_XOUT_H, buf, 6)) return false;

    const int16_t rawX = toInt16BE(buf[0], buf[1]);
    const int16_t rawY = toInt16BE(buf[2], buf[3]);
    const int16_t rawZ = toInt16BE(buf[4], buf[5]);

    xGyro = static_cast<float>(rawX) * lsbResGyro;
    yGyro = static_cast<float>(rawY) * lsbResGyro;
    zGyro = static_cast<float>(rawZ) * lsbResGyro;
    return true;
}

bool MPU6500::readAccel() {
    uint8_t buf[6];
    if (!bus.readBytes(address, MPU6500_ACCEL_XOUT_H, buf, 6)) return false;

    const int16_t rawX = toInt16BE(buf[0], buf[1]);
    const int16_t rawY = toInt16BE(buf[2], buf[3]);
    const int16_t rawZ = toInt16BE(buf[4], buf[5]);

    xAccel = static_cast<float>(rawX) * lsbResAccel;
    yAccel = static_cast<float>(rawY) * lsbResAccel;
    zAccel = static_cast<float>(rawZ) * lsbResAccel;
    return true;
}

// -----------------------------------------------------------------------------
// Identity
// -----------------------------------------------------------------------------

Result<uint8_t, bool> MPU6500::whoAmI() {
    return bus.read(address, MPU6500_WHO_AM_I);
}

// -----------------------------------------------------------------------------
// Private helpers
// -----------------------------------------------------------------------------

Result<int16_t, bool> MPU6500::readSensor(uint8_t reg, float& scaledOut,
                                          SensorChannel channel) {
    uint8_t buf[2];
    if (!bus.readBytes(address, reg, buf, 2)) {
        return {0, false}; // leave stored scaled value unchanged on bus failure
    }

    const int16_t val = toInt16BE(buf[0], buf[1]);

    switch (channel) {
        case SensorChannel::Gyro:
            scaledOut = static_cast<float>(val) * lsbResGyro;
            break;
        case SensorChannel::Accel:
            scaledOut = static_cast<float>(val) * lsbResAccel;
            break;
        case SensorChannel::Temp:
            // Datasheet: T(°C) = (TEMP_OUT - room_offset) / sensitivity + 21
            scaledOut = (static_cast<float>(val) - MPU6500_TEMP_OFFSET)
                        / MPU6500_TEMP_SENSITIVITY
                        + MPU6500_TEMP_ROOM_OFFSET;
            break;
    }
    return {val, true};
}
