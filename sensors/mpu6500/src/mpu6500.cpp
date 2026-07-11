#include "mpu6500.h"
#include "i2c_utils.h"

MPU6500::MPU6500(II2C& bus, uint8_t myAddress) : bus(bus), address(myAddress) {}

bool MPU6500::setGyroOffset(int16_t offsetX, int16_t offsetY, int16_t offsetZ) {
    // bits [1:0] are reserved in the offset LSB registers — preserve them
    const uint8_t lsbMask = 0b11111100;
    bool ok = true;
    ok &= bus.write(this->address, MPU6500_XG_OFFSET_H, static_cast<uint8_t>(offsetX >> 8));
    ok &= bus.write(this->address, MPU6500_YG_OFFSET_H, static_cast<uint8_t>(offsetY >> 8));
    ok &= bus.write(this->address, MPU6500_ZG_OFFSET_H, static_cast<uint8_t>(offsetZ >> 8));
    ok &= i2c::writeMasked(bus, this->address, MPU6500_XG_OFFSET_L, static_cast<uint8_t>(offsetX), lsbMask);
    ok &= i2c::writeMasked(bus, this->address, MPU6500_YG_OFFSET_L, static_cast<uint8_t>(offsetY), lsbMask);
    ok &= i2c::writeMasked(bus, this->address, MPU6500_ZG_OFFSET_L, static_cast<uint8_t>(offsetZ), lsbMask);
    return ok;
}

bool MPU6500::setSampleRateDivider(uint8_t divider) {
    return bus.write(this->address, MPU6500_SMPLRT_DIV, divider);
}

bool MPU6500::setFIFOMode(bool stopWhenFull) {
    return i2c::writeBit(bus, this->address, MPU6500_CONFIG, stopWhenFull, MPU6500_CONFIG_FIFO_MODE_BIT);
}

bool MPU6500::setFSync(ExtSyncSource source) {
    uint8_t bits = static_cast<uint8_t>(source) << MPU6500_CONFIG_EXT_SYNC_SET_SHIFT;
    return i2c::writeMasked(bus, this->address, MPU6500_CONFIG, bits, MPU6500_CONFIG_EXT_SYNC_SET_MASK);
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
    return i2c::writeMasked(bus, this->address, MPU6500_CONFIG, bits, MPU6500_CONFIG_DLPF_CFG_MASK);
}

bool MPU6500::gyroLPFEnable(bool isOn) {
    // FCHOICE_B = 0b01 disables LPF, 0b00 enables it
    uint8_t bits = isOn ? 0b00 : 0b01;
    return i2c::writeMasked(bus, this->address, MPU6500_GYRO_CONFIG, bits, MPU6500_GYRO_CONFIG_FCHOICE_B_MASK);
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
    this->lsbResGyro = static_cast<float>(range) / 32768.0f;
    return i2c::writeMasked(bus, this->address, MPU6500_GYRO_CONFIG, bits, MPU6500_GYRO_CONFIG_FS_SEL_MASK);
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
    return i2c::writeMasked(bus, this->address, MPU6500_ACCEL_CONFIG2, bits, MPU6500_ACCEL_CONFIG2_DLPF_CFG_MASK);
}

bool MPU6500::accelLPFEnable(bool isOn) {
    // FCHOICE_B bit: 1 = bypass LPF, 0 = use LPF
    return i2c::writeBit(bus, this->address, MPU6500_ACCEL_CONFIG2, !isOn, MPU6500_ACCEL_CONFIG2_FCHOICE_B_BIT);
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
    this->lsbResAccel = (static_cast<float>(range) * GRAVITY) / 32768.0f;
    return i2c::writeMasked(bus, this->address, MPU6500_ACCEL_CONFIG, bits, MPU6500_ACCEL_CONFIG_FS_SEL_MASK);
}

bool MPU6500::isDRDY() {
    return (bus.read(this->address, MPU6500_INT_STATUS) & (1 << MPU6500_INT_DATA_RDY_BIT)) != 0;
}

int16_t MPU6500::readGyroX() {
    return readSensor(MPU6500_GYRO_XOUT_H, this->xGyroRaw, this->xGyro, SensorChannel::Gyro);
}

int16_t MPU6500::readGyroY() {
    return readSensor(MPU6500_GYRO_YOUT_H, this->yGyroRaw, this->yGyro, SensorChannel::Gyro);
}

int16_t MPU6500::readGyroZ() {
    return readSensor(MPU6500_GYRO_ZOUT_H, this->zGyroRaw, this->zGyro, SensorChannel::Gyro);
}

void MPU6500::readGyro() {
    readGyroX();
    readGyroY();
    readGyroZ();
}

int16_t MPU6500::readAccelX() {
    return readSensor(MPU6500_ACCEL_XOUT_H, this->xAccelRaw, this->xAccel, SensorChannel::Accel);
}

int16_t MPU6500::readAccelY() {
    return readSensor(MPU6500_ACCEL_YOUT_H, this->yAccelRaw, this->yAccel, SensorChannel::Accel);
}

int16_t MPU6500::readAccelZ() {
    return readSensor(MPU6500_ACCEL_ZOUT_H, this->zAccelRaw, this->zAccel, SensorChannel::Accel);
}

void MPU6500::readAccel() {
    readAccelX();
    readAccelY();
    readAccelZ();
}

int16_t MPU6500::readTemp() {
    return readSensor(MPU6500_TEMP_OUT_H, this->tempRaw, this->temp, SensorChannel::Temp);
    // TODO: apply temperature conversion formula:
    // temp_C = (raw - MPU6500_TEMP_OFFSET) / MPU6500_TEMP_SENSITIVITY + MPU6500_TEMP_ROOM_OFFSET
}

bool MPU6500::resetRegisters() {
    return i2c::writeBit(bus, this->address, MPU6500_PWR_MGMT_1, true, MPU6500_PWR1_DEVICE_RESET_BIT);
}

bool MPU6500::configureDefaults() {
    bool ok = true;
    ok &= resetRegisters();
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
    return setSleepMode(true);
}

bool MPU6500::wake() {
    return setSleepMode(false);
}

bool MPU6500::setSleepMode(bool isAsleep) {
    return i2c::writeBit(bus, this->address, MPU6500_PWR_MGMT_1, isAsleep, MPU6500_PWR1_SLEEP_BIT);
}

bool MPU6500::enableTempSense(bool isEnable) {
    // PD_PTAT bit: 1 = sensor powered down, so invert isEnable
    return i2c::writeBit(bus, this->address, MPU6500_PWR_MGMT_1, !isEnable, MPU6500_PWR1_PD_PTAT_BIT);
}

bool MPU6500::enableXAccel(bool isEnable) {
    return i2c::writeBit(bus, this->address, MPU6500_PWR_MGMT_2, !isEnable, MPU6500_PWR2_DISABLE_XA_BIT);
}

bool MPU6500::enableYAccel(bool isEnable) {
    return i2c::writeBit(bus, this->address, MPU6500_PWR_MGMT_2, !isEnable, MPU6500_PWR2_DISABLE_YA_BIT);
}

bool MPU6500::enableZAccel(bool isEnable) {
    return i2c::writeBit(bus, this->address, MPU6500_PWR_MGMT_2, !isEnable, MPU6500_PWR2_DISABLE_ZA_BIT);
}

bool MPU6500::enableAccel(bool isEnable) {
    bool ok = enableXAccel(isEnable);
    ok &= enableYAccel(isEnable);
    ok &= enableZAccel(isEnable);
    return ok;
}

bool MPU6500::enableXGyro(bool isEnable) {
    return i2c::writeBit(bus, this->address, MPU6500_PWR_MGMT_2, !isEnable, MPU6500_PWR2_DISABLE_XG_BIT);
}

bool MPU6500::enableYGyro(bool isEnable) {
    return i2c::writeBit(bus, this->address, MPU6500_PWR_MGMT_2, !isEnable, MPU6500_PWR2_DISABLE_YG_BIT);
}

bool MPU6500::enableZGyro(bool isEnable) {
    return i2c::writeBit(bus, this->address, MPU6500_PWR_MGMT_2, !isEnable, MPU6500_PWR2_DISABLE_ZG_BIT);
}

bool MPU6500::enableGyro(bool isEnable) {
    bool ok = enableXGyro(isEnable);
    ok &= enableYGyro(isEnable);
    ok &= enableZGyro(isEnable);
    return ok;
}

uint8_t MPU6500::whoAmI() {
    return bus.read(this->address, MPU6500_WHO_AM_I);
}

int16_t MPU6500::readSensor(uint8_t reg, int16_t& rawStorage, float& storage, SensorChannel channel) {
    uint8_t buf[2];
    if (!bus.readBytes(this->address, reg, buf, 2)) {
        return rawStorage; // return last known value on bus failure
    }
    // MPU6500 output registers are big-endian: MSB at reg, LSB at reg+1
    int16_t val = static_cast<int16_t>((static_cast<uint16_t>(buf[0]) << 8) | buf[1]);
    rawStorage = val;
    storage = static_cast<float>(val);
    switch (channel) {
        case SensorChannel::Gyro:  storage *= lsbResGyro;  break;
        case SensorChannel::Accel: storage *= lsbResAccel; break;
        case SensorChannel::Temp:                          break;
    }
    return val;
}
