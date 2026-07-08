#include "mpu6500.h"
#include "i2c_handler.h"

MPU6500::MPU6500(uint8_t myAddress) : address(myAddress) {}

bool MPU6500::setGyroOffset(int16_t offsetX, int16_t offsetY, int16_t offsetZ){
    uint8_t lsbMask = 0b11111111;
    uint8_t xMSB = offsetX >> 8;
    uint8_t yMSB = offsetY >> 8;
    uint8_t zMSB = offsetZ >> 8;
    uint8_t xLSB = offsetX & lsbMask;
    uint8_t yLSB = offsetY & lsbMask;
    uint8_t zLSB = offsetZ & lsbMask;
    bool success = true;
    success &= i2cWrite(this->address, XG_OFFSET_H, xMSB);
    success &= i2cWrite(this->address, YG_OFFSET_H, yMSB);
    success &= i2cWrite(this->address, ZG_OFFSET_H, zMSB);
    success &= i2cWrite(this->address, XG_OFFSET_L, xLSB);
    success &= i2cWrite(this->address, YG_OFFSET_L, yLSB);
    success &= i2cWrite(this->address, ZG_OFFSET_L, zLSB);
    return success;
}

bool MPU6500::setSampleRateDivider(uint8_t divider){
    return i2cWrite(this->address, SMPLRT_DIV, divider);
}

bool MPU6500::setFIFOMode(bool stopWhenFull){
    return i2cWriteBit(this->address, CONFIG, stopWhenFull, 6);
}

bool MPU6500::setFSync(ExtSyncSource source){
    uint8_t bits = static_cast<uint8_t>(source) << 3;
    return i2cWrite(this->address, CONFIG, bits, 0b00111000);
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
        return i2cWrite(this->address, CONFIG, bits, 0b00000111);
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
    return i2cWrite(this->address, GYRO_CONFIG, bits, 0b00000011);
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
    return i2cWrite(this->address, GYRO_CONFIG, bits, 0b00011000);
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
        return i2cWrite(this->address, ACCEL_CONFIG_2, bits, 0b00000111);
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
    return i2cWrite(this->address, ACCEL_CONFIG_2, bits, 0b00001000);
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
    return i2cWrite(this->address, ACCEL_CONFIG, bits, 0b00011000);
}

bool MPU6500::isDRDY(){
    return (i2cRead(this->address, INT_STATUS) & 1) != 0;
}

int16_t MPU6500::readGyroX(){
    return readSensor(GYRO_XOUT_H, GYRO_XOUT_L, this->xGyroRaw, this->xGyro, SensorChannel::Gyro);
}

int16_t MPU6500::readGyroY(){
    return readSensor(GYRO_YOUT_H, GYRO_YOUT_L, this->yGyroRaw, this->yGyro, SensorChannel::Gyro);
}

int16_t MPU6500::readGyroZ(){
    return readSensor(GYRO_ZOUT_H, GYRO_ZOUT_L, this->zGyroRaw, this->zGyro, SensorChannel::Gyro);
}

void MPU6500::readGyro(){
    this->readGyroX();
    this->readGyroY();
    this->readGyroZ();
}

int16_t MPU6500::readAccelX(){
    return readSensor(ACCEL_XOUT_H, ACCEL_XOUT_L, this->xAccelRaw, this->xAccel, SensorChannel::Accel);
}

int16_t MPU6500::readAccelY(){
    return readSensor(ACCEL_YOUT_H, ACCEL_YOUT_L, this->yAccelRaw, this->yAccel, SensorChannel::Accel);
}

int16_t MPU6500::readAccelZ(){
    return readSensor(ACCEL_ZOUT_H, ACCEL_ZOUT_L, this->zAccelRaw, this->zAccel, SensorChannel::Accel);
}

void MPU6500::readAccel(){
    this->readAccelX();
    this->readAccelY();
    this->readAccelZ();
}

int16_t MPU6500::readTemp(){
    return readSensor(TEMP_OUT_H, TEMP_OUT_L, this->tempRaw, this->temp, SensorChannel::Temp);
    // Convert the reading into degrees Celsius. Reference the data sheet for equation
    // TODO: Define tempOffset and tempSensitivity
    // return (reading - tempOffset)/tempSensitivity + 21;
    // ALSO TODO: Configure readSensor function properly to handle temp reading
}

bool MPU6500::resetRegisters(){
    return i2cWrite(this->address, PWR_MGMT_1, 1, 0b10000000);
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
    return i2cWriteBit(this->address, PWR_MGMT_1, isAsleep, 6);
}

bool MPU6500::enableTempSense(bool isEnable){
    return i2cWriteBit(this->address, PWR_MGMT_1, !isEnable, 3);
}

bool MPU6500::enableXAccel(bool isEnable){
    return i2cWriteBit(this->address, PWR_MGMT_2, !isEnable, 5);
}

bool MPU6500::enableYAccel(bool isEnable){
    return i2cWriteBit(this->address, PWR_MGMT_2, !isEnable, 4);
}

bool MPU6500::enableZAccel(bool isEnable){
    return i2cWriteBit(this->address, PWR_MGMT_2, !isEnable, 3);
}

bool MPU6500::enableAccel(bool isEnable){
    bool success = enableXAccel(isEnable);
    success &= enableYAccel(isEnable);
    success &= enableZAccel(isEnable);
    return success;
}


bool MPU6500::enableXGyro(bool isEnable){
    return i2cWriteBit(this->address, PWR_MGMT_2, !isEnable, 2);
}

bool MPU6500::enableYGyro(bool isEnable){
    return i2cWriteBit(this->address, PWR_MGMT_2, !isEnable, 1);
}

bool MPU6500::enableZGyro(bool isEnable){
    return i2cWriteBit(this->address, PWR_MGMT_2, !isEnable, 0);
}

bool MPU6500::enableGyro(bool isEnable){
    bool success = enableXGyro(isEnable);
    success &= enableYGyro(isEnable);
    success &= enableZGyro(isEnable);
    return success;
}

uint8_t MPU6500::whoAmI(){
    return i2cRead(this->address, WHO_AM_I);
}

int16_t MPU6500::readSensor(uint8_t msbReg, uint8_t lsbReg, int16_t& rawStorage, float& storage, SensorChannel channel){
    int16_t val = (int16_t)i2cReadTwo(this->address, lsbReg, msbReg);
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
