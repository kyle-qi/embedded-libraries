#include "mpu6500.h"
#include "i2c_utils.h"

MPU6500::MPU6500(II2C& bus, uint8_t myAddress) : bus(bus), address(myAddress) {}

bool MPU6500::setGyroOffset(int16_t offsetX, int16_t offsetY, int16_t offsetZ){
    uint8_t lsbMask = 0b11111100; // bits [1:0] are reserved in offset LSB registers
    uint8_t xMSB = offsetX >> 8;
    uint8_t yMSB = offsetY >> 8;
    uint8_t zMSB = offsetZ >> 8;
    uint8_t xLSB = offsetX & 0xFF;
    uint8_t yLSB = offsetY & 0xFF;
    uint8_t zLSB = offsetZ & 0xFF;
    bool success = true;
    success &= bus.write(this->address, XG_OFFSET_H, xMSB);
    success &= bus.write(this->address, YG_OFFSET_H, yMSB);
    success &= bus.write(this->address, ZG_OFFSET_H, zMSB);
    success &= i2c::writeMasked(bus, this->address, XG_OFFSET_L, xLSB, lsbMask);
    success &= i2c::writeMasked(bus, this->address, YG_OFFSET_L, yLSB, lsbMask);
    success &= i2c::writeMasked(bus, this->address, ZG_OFFSET_L, zLSB, lsbMask);
    return success;
}

bool MPU6500::setSampleRateDivider(uint8_t divider){
    return bus.write(this->address, SMPLRT_DIV, divider);
}

bool MPU6500::setFIFOMode(bool stopWhenFull){
    return i2c::writeBit(bus, this->address, CONFIG, stopWhenFull, 6);
}

bool MPU6500::setFSync(ExtSyncSource source){
    uint8_t bits = static_cast<uint8_t>(source) << 3;
    return i2c::writeMasked(bus, this->address, CONFIG, bits, 0b00111000);
}


bool MPU6500::setGyroLPF(bool isOn, uint16_t bandwidth){
    // Enable/Disable the LPF
    if(!(this->gyroLPFEnable(isOn))){
        return false;
    }
    // Configure LPF if on
    if (isOn) {
        // Set bits based on mode selected
        uint8_t bits;
        switch(bandwidth){
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

        // Write bits to register
        return i2c::writeMasked(bus, this->address, CONFIG, bits, 0b00000111);
    }
    return true;
}

bool MPU6500::gyroLPFEnable(bool isOn){
    // Configure bits based on isOn
    uint8_t bits = 0b01;
    if (isOn){
        bits = 0b00;
    }

    // Write bits to register
    return i2c::writeMasked(bus, this->address, GYRO_CONFIG, bits, 0b00000011);
}

bool MPU6500::setGyroRange(uint16_t range){
    // Set bits based on mode selected
    uint8_t bits;
    switch(range){
        case MPU6500_GYRO_FS_250DPS:  bits = 0b00; break;
        case MPU6500_GYRO_FS_500DPS:  bits = 0b01; break;
        case MPU6500_GYRO_FS_1000DPS: bits = 0b10; break;
        case MPU6500_GYRO_FS_2000DPS: bits = 0b11; break;
        default: return false;
    }
    bits <<= 3;
    this->lsbResGyro = float(range)/32768.0f;

    // Set gyro range
    return i2c::writeMasked(bus, this->address, GYRO_CONFIG, bits, 0b00011000);
}

bool MPU6500::setAccelLPF(bool isOn, uint16_t bandwidth){
    // Enable/Disable the LPF
    if(!(this->accelLPFEnable(isOn))){
        return false;
    }
    // Configure LPF if on
    if (isOn) {
        // Set bits based on mode selected
        uint8_t bits;
        switch(bandwidth){
            case MPU6500_ACCEL_DLPF_BW_460_HZ: bits = 0; break;
            case MPU6500_ACCEL_DLPF_BW_184_HZ: bits = 1; break;
            case MPU6500_ACCEL_DLPF_BW_92_HZ:  bits = 2; break;
            case MPU6500_ACCEL_DLPF_BW_44_HZ:  bits = 3; break;
            case MPU6500_ACCEL_DLPF_BW_20_HZ:  bits = 4; break;
            case MPU6500_ACCEL_DLPF_BW_10_HZ:  bits = 5; break;
            case MPU6500_ACCEL_DLPF_BW_5_HZ:   bits = 6; break;
            default: return false;
        }

        // Write bits to register
        return i2c::writeMasked(bus, this->address, ACCEL_CONFIG_2, bits, 0b00000111);
    }
    return true;
}

bool MPU6500::accelLPFEnable(bool isOn){
    // Configure bits based on isOn
    uint8_t bits = 1;
    if (isOn){
        bits = 0;
    }
    bits <<= 3;

    // Write bits to register
    return i2c::writeMasked(bus, this->address, ACCEL_CONFIG_2, bits, 0b00001000);
}

bool MPU6500::setAccelRange(uint8_t range){
    // Set bits based on mode selected
    uint8_t bits;
    switch(range){
        case MPU6500_ACCEL_FS_2G:  bits = 0b00; break;
        case MPU6500_ACCEL_FS_4G:  bits = 0b01; break;
        case MPU6500_ACCEL_FS_8G:  bits = 0b10; break;
        case MPU6500_ACCEL_FS_16G: bits = 0b11; break;
        default: return false;
    }
    bits <<= 3;
    this->lsbResAccel = ((float)range * GRAVITY)/32768.0f;

    // Write bits to register
    return i2c::writeMasked(bus, this->address, ACCEL_CONFIG, bits, 0b00011000);
}

bool MPU6500::isDRDY(){
    return (bus.read(this->address, INT_STATUS) & 1) != 0;
}

int16_t MPU6500::readGyroX(){
    return readSensor(GYRO_XOUT_H, this->xGyroRaw, this->xGyro, SensorChannel::Gyro);
}

int16_t MPU6500::readGyroY(){
    return readSensor(GYRO_YOUT_H, this->yGyroRaw, this->yGyro, SensorChannel::Gyro);
}

int16_t MPU6500::readGyroZ(){
    return readSensor(GYRO_ZOUT_H, this->zGyroRaw, this->zGyro, SensorChannel::Gyro);
}

void MPU6500::readGyro(){
    this->readGyroX();
    this->readGyroY();
    this->readGyroZ();
}

int16_t MPU6500::readAccelX(){
    return readSensor(ACCEL_XOUT_H, this->xAccelRaw, this->xAccel, SensorChannel::Accel);
}

int16_t MPU6500::readAccelY(){
    return readSensor(ACCEL_YOUT_H, this->yAccelRaw, this->yAccel, SensorChannel::Accel);
}

int16_t MPU6500::readAccelZ(){
    return readSensor(ACCEL_ZOUT_H, this->zAccelRaw, this->zAccel, SensorChannel::Accel);
}

void MPU6500::readAccel(){
    this->readAccelX();
    this->readAccelY();
    this->readAccelZ();
}

int16_t MPU6500::readTemp(){
    return readSensor(TEMP_OUT_H, this->tempRaw, this->temp, SensorChannel::Temp);
    // Convert the reading into degrees Celsius. Reference the data sheet for equation
    // TODO: Define tempOffset and tempSensitivity
    // return (reading - tempOffset)/tempSensitivity + 21;
    // ALSO TODO: Configure readSensor function properly to handle temp reading
}

bool MPU6500::resetRegisters(){
    return i2c::writeMasked(bus, this->address, PWR_MGMT_1, 1, 0b10000000);
}

bool MPU6500::configureDefaults(){
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

bool MPU6500::sleep(){
    return setSleepMode(1);
}

bool MPU6500::wake(){
    return setSleepMode(0);
}

bool MPU6500::setSleepMode(bool isAsleep){
    return i2c::writeBit(bus, this->address, PWR_MGMT_1, isAsleep, 6);
}

bool MPU6500::enableTempSense(bool isEnable){
    return i2c::writeBit(bus, this->address, PWR_MGMT_1, !isEnable, 3);
}

bool MPU6500::enableXAccel(bool isEnable){
    return i2c::writeBit(bus, this->address, PWR_MGMT_2, !isEnable, 5);
}

bool MPU6500::enableYAccel(bool isEnable){
    return i2c::writeBit(bus, this->address, PWR_MGMT_2, !isEnable, 4);
}

bool MPU6500::enableZAccel(bool isEnable){
    return i2c::writeBit(bus, this->address, PWR_MGMT_2, !isEnable, 3);
}

bool MPU6500::enableAccel(bool isEnable){
    bool success = enableXAccel(isEnable);
    success &= enableYAccel(isEnable);
    success &= enableZAccel(isEnable);
    return success;
}


bool MPU6500::enableXGyro(bool isEnable){
    return i2c::writeBit(bus, this->address, PWR_MGMT_2, !isEnable, 2);
}

bool MPU6500::enableYGyro(bool isEnable){
    return i2c::writeBit(bus, this->address, PWR_MGMT_2, !isEnable, 1);
}

bool MPU6500::enableZGyro(bool isEnable){
    return i2c::writeBit(bus, this->address, PWR_MGMT_2, !isEnable, 0);
}

bool MPU6500::enableGyro(bool isEnable){
    bool success = enableXGyro(isEnable);
    success &= enableYGyro(isEnable);
    success &= enableZGyro(isEnable);
    return success;
}

uint8_t MPU6500::whoAmI(){
    return bus.read(this->address, WHO_AM_I);
}

int16_t MPU6500::readSensor(uint8_t reg, int16_t& rawStorage, float& storage, SensorChannel channel){
    uint8_t buf[2];
    if (!bus.readBytes(this->address, reg, buf, 2)) {
        return rawStorage; // return last known value on failure
    }
    int16_t val = static_cast<int16_t>((static_cast<uint16_t>(buf[0]) << 8) | buf[1]);
    rawStorage = val;
    storage = static_cast<float>(val);
    switch (channel) {
        case SensorChannel::Gyro:
            storage *= lsbResGyro;
            break;
        case SensorChannel::Accel:
            storage *= lsbResAccel;
            break;
        case SensorChannel::Temp:
            break;
    }
    return val;
}
