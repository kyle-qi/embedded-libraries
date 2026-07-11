#include "qmc5883l.h"
#include "i2c_utils.h"

QMC5883L::QMC5883L(II2C& bus, uint8_t myAddress) : bus(bus), address(myAddress) {}

bool QMC5883L::setMode(Mode mode){
    uint8_t config = bus.read(this->address, QMC5883L_CTRLA_REG);

    // Early return if mode is already set properly
    if ((config & 0b11) == static_cast<uint8_t>(mode)) {
        return true;
    }

    // Mode must be set to suspend between different modes
    if ((config & 0b11) != 0b00) {
        config = static_cast<uint8_t>((config & ~0b11u) | 0b00u);
        if (!bus.write(this->address, QMC5883L_CTRLA_REG, config)){
            return false;
        }
        delay(100);
    }

    // Write to registers
    config = static_cast<uint8_t>((config & ~0b11u) | (static_cast<uint8_t>(mode) & 0b11u));
    return bus.write(this->address, QMC5883L_CTRLA_REG, config);
}
    
bool QMC5883L::setOutputRate(uint8_t odr){
    // Set bits based on mode selected
    uint8_t bits;
    switch(odr){
        case QMC5883L_OUTPUT_ODR_10_HZ:  bits = 0b00; break;
        case QMC5883L_OUTPUT_ODR_50_HZ:  bits = 0b01; break;
        case QMC5883L_OUTPUT_ODR_100_HZ: bits = 0b10; break;
        case QMC5883L_OUTPUT_ODR_200_HZ: bits = 0b11; break;
        default: return false;
    }
    bits <<= 2;

    // Set output rate
    return i2c::writeMasked(bus, this->address, QMC5883L_CTRLA_REG, bits, 0b00001100);
}

bool QMC5883L::setOverSampleRate(uint8_t osr1){
    // Set bits based on mode selected
    uint8_t bits;
    switch(osr1){
        case QMC5883L_OVERSAMPLE_RATE_8: bits = 0b00; break;
        case QMC5883L_OVERSAMPLE_RATE_4: bits = 0b01; break;
        case QMC5883L_OVERSAMPLE_RATE_2: bits = 0b10; break;
        case QMC5883L_OVERSAMPLE_RATE_1: bits = 0b11; break;
        default: return false;
    }
    bits <<= 4;

    // Set over sample rate
    return i2c::writeMasked(bus, this->address, QMC5883L_CTRLA_REG, bits, 0b00110000);
}

bool QMC5883L::setDownSampleRate(uint8_t osr2){
    // Set bits based on mode selected
    uint8_t bits;
    switch(osr2){
        case QMC5883L_DOWNSAMPLE_RATE_1: bits = 0b00; break;
        case QMC5883L_DOWNSAMPLE_RATE_2: bits = 0b01; break;
        case QMC5883L_DOWNSAMPLE_RATE_4: bits = 0b10; break;
        case QMC5883L_DOWNSAMPLE_RATE_8: bits = 0b11; break;
        default: return false;
    }
    bits <<= 6;

    // Set down sample rate
    return i2c::writeMasked(bus, this->address, QMC5883L_CTRLA_REG, bits, 0b11000000);
}

bool QMC5883L::setRange(uint8_t range){
    // Set bits based on mode selected
    uint8_t bits;
    switch(range){
        case QMC5883L_RANGE_30_GAUSS: bits = 0b00; break;
        case QMC5883L_RANGE_12_GAUSS: bits = 0b01; break;
        case QMC5883L_RANGE_8_GAUSS:  bits = 0b10; break;
        case QMC5883L_RANGE_2_GAUSS:  bits = 0b11; break;
        default: return false;
    }
    bits <<= 2;
    this->lsbRes = float(range)/32768.0f;

    // Set range
    return i2c::writeMasked(bus, this->address, QMC5883L_CTRLB_REG, bits, 0b00001100);
}

bool QMC5883L::setSetResetMode(SetResetMode mode){
    return i2c::writeMasked(bus, this->address, QMC5883L_CTRLB_REG, static_cast<uint8_t>(mode), 0b11);
}

bool QMC5883L::resetRegisters(){
    if(!i2c::writeBit(bus, this->address, QMC5883L_CTRLB_REG, 1, 7)){
        return false;
    }
    delay(10);
    if(!i2c::writeBit(bus, this->address, QMC5883L_CTRLB_REG, 0, 7)){
        return false;
    }
    return true;
}

bool QMC5883L::configureDefaults(){
    bool ok = true;
    ok &= resetRegisters();
    ok &= setMode(MODE_CONTINUOUS);
    ok &= setOutputRate(QMC5883L_OUTPUT_ODR_10_HZ);
    ok &= setOverSampleRate(QMC5883L_OVERSAMPLE_RATE_8);
    ok &= setDownSampleRate(QMC5883L_DOWNSAMPLE_RATE_8);
    ok &= setSetResetMode(SET_ON);
    ok &= setRange(QMC5883L_RANGE_8_GAUSS);
    return ok;
}

bool QMC5883L::isDRDY(){
    return (bus.read(this->address, QMC5883L_STATUS_REG) & 0b01) != 0;
}

bool QMC5883L::isOVFL(){
    return (bus.read(this->address, QMC5883L_STATUS_REG) & 0b10) != 0;
}

void QMC5883L::calibrate(uint32_t calibrationTime){
    // Stores the max and min magnetometer reading
    int16_t minMaxReadings[3][2] = {
        {INT16_MAX, INT16_MIN},
        {INT16_MAX, INT16_MIN},
        {INT16_MAX, INT16_MIN},
    };

    // Tracks timestamp of last array update
    uint32_t timeStamp = millis();

    // If no new max values have been written for
    // calibrationTime seconds, then the compass is calibrated
    bool isCalibrated = false;
    while (!isCalibrated) {
        Serial.println("Keep moving compass...");
        int16_t readings[3] = {readX(), readY(), readZ()};

        for (uint8_t i = 0; i < 3; ++i) {
            if (readings[i] < minMaxReadings[i][0]) {
                minMaxReadings[i][0] = readings[i];
                timeStamp = millis();
            }
            if (readings[i] > minMaxReadings[i][1]) {
                minMaxReadings[i][1] = readings[i];
                timeStamp = millis();
            }
        }

        if (millis() - timeStamp > calibrationTime) {
            isCalibrated = true;
        }
        delay(10);
    } 

    // Write the values to the object
    this->xMax = minMaxReadings[0][1];
    this->yMax = minMaxReadings[1][1];
    this->zMax = minMaxReadings[2][1];
    this->xMin = minMaxReadings[0][0];
    this->yMin = minMaxReadings[1][0];
    this->zMin = minMaxReadings[2][0];

    Serial.println("If you want to hardcode debug values, add this to main:");
    Serial.print("<yourCompassName>.setCalibrationData(");
    Serial.print(this->xMax);
    Serial.print(", ");
    Serial.print(this->yMax);
    Serial.print(", ");
    Serial.print(this->zMax);
    Serial.print(", ");
    Serial.print(this->xMin);
    Serial.print(", ");
    Serial.print(this->yMin);
    Serial.print(", ");
    Serial.print(this->zMin);
    Serial.println(");");
}

void QMC5883L::setCalibrationData(int16_t xMax, int16_t yMax, int16_t zMax, int16_t xMin, int16_t yMin, int16_t zMin){
    this->xMax = xMax;
    this->yMax = yMax;
    this->zMax = zMax;
    this->xMin = xMin;
    this->yMin = yMin;
    this->zMin = zMin;
}

int16_t QMC5883L::readX() {
    return readAxis(QMC5883L_XLSB_REG, this->xRaw, this->x, this->xGauss, this->xMax, this->xMin);
}

int16_t QMC5883L::readY() {
    return readAxis(QMC5883L_YLSB_REG, this->yRaw, this->y, this->yGauss, this->yMax, this->yMin);
}

int16_t QMC5883L::readZ() {
    return readAxis(QMC5883L_ZLSB_REG, this->zRaw, this->z, this->zGauss, this->zMax, this->zMin);
}

void QMC5883L::read(){
    this->readX();
    this->readY();
    this->readZ();
}

float QMC5883L::azimuth(float xNorm, float yNorm){
    float angle = atan2f(yNorm, xNorm) * RAD_TO_DEG; // Obtain angle in radians
    // Convert angle to compass sign convention
    return fmod(450-angle, 360);
}

int16_t QMC5883L::readAxis(uint8_t reg, int16_t& rawStorage, float& normStorage, float& gaussStorage, int16_t maxVal, int16_t minVal){
    uint8_t buf[2];
    if (!bus.readBytes(this->address, reg, buf, 2)) {
        return rawStorage; // return last known value on failure
    }
    // QMC5883L output registers are little-endian: LSB at reg, MSB at reg+1
    int16_t val = static_cast<int16_t>((static_cast<uint16_t>(buf[1]) << 8) | buf[0]);
    rawStorage = val;
    normStorage = normalize(val, maxVal, minVal);
    gaussStorage = static_cast<float>(val) * lsbRes;
    return val;
}

float QMC5883L::normalize(int16_t val, int16_t maxVal, int16_t minVal) {
    // Edge case handling
    if(maxVal == minVal){
        return 0.0f;
    }
    float center = (maxVal + minVal)/2.0f;
    float halfRange = (maxVal - minVal)/2.0f;
    return (val - center)/ halfRange;
}
