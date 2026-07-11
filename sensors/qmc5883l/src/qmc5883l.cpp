#include "qmc5883l.h"
#include "i2c_utils.h"
#include <math.h>

// LSB-to-Gauss scale factors keyed to the Range enum encoding
static constexpr float kLsbResTable[4] = {
    30.0f  / 32768.0f,  // Range::Gauss30
    12.0f  / 32768.0f,  // Range::Gauss12
     8.0f  / 32768.0f,  // Range::Gauss8
     2.0f  / 32768.0f,  // Range::Gauss2
};

// Power-on reset default: 8 Gauss range (register default is 0b00 → Gauss30,
// but we initialise to match configureDefaults()).
static constexpr float kDefaultLsbRes = kLsbResTable[2]; // Gauss8

QMC5883L::QMC5883L(II2C& bus, IClock& clock, uint8_t myAddress)
    : bus(bus)
    , clock(clock)
    , address(myAddress)
    , xMax(0), yMax(0), zMax(0)
    , xMin(0), yMin(0), zMin(0)
    , xRaw(0), yRaw(0), zRaw(0)
    , x(0.0f), y(0.0f), z(0.0f)
    , xGauss(0.0f), yGauss(0.0f), zGauss(0.0f)
    , lsbRes(kDefaultLsbRes)
{}

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
        clock.delayMs(100);
    }

    // Write to registers
    config = static_cast<uint8_t>((config & ~0b11u) | (static_cast<uint8_t>(mode) & 0b11u));
    return bus.write(this->address, QMC5883L_CTRLA_REG, config);
}

bool QMC5883L::setOutputRate(OutputRate odr){
    uint8_t bits = static_cast<uint8_t>(odr) << 2;
    return i2c::writeMasked(bus, this->address, QMC5883L_CTRLA_REG, bits, 0b00001100);
}

bool QMC5883L::setOverSampleRate(OverSampleRate osr){
    uint8_t bits = static_cast<uint8_t>(osr) << 4;
    return i2c::writeMasked(bus, this->address, QMC5883L_CTRLA_REG, bits, 0b00110000);
}

bool QMC5883L::setDownSampleRate(DownSampleRate osr){
    uint8_t bits = static_cast<uint8_t>(osr) << 6;
    return i2c::writeMasked(bus, this->address, QMC5883L_CTRLA_REG, bits, 0b11000000);
}

bool QMC5883L::setRange(Range range){
    uint8_t idx = static_cast<uint8_t>(range);
    uint8_t bits = idx << 2;
    this->lsbRes = kLsbResTable[idx];
    return i2c::writeMasked(bus, this->address, QMC5883L_CTRLB_REG, bits, 0b00001100);
}

bool QMC5883L::setSetResetMode(SetResetMode mode){
    return i2c::writeMasked(bus, this->address, QMC5883L_CTRLB_REG, static_cast<uint8_t>(mode), 0b11);
}

bool QMC5883L::resetRegisters(){
    if(!i2c::writeBit(bus, this->address, QMC5883L_CTRLB_REG, 1, 7)){
        return false;
    }
    clock.delayMs(10);
    return i2c::writeBit(bus, this->address, QMC5883L_CTRLB_REG, 0, 7);
}

bool QMC5883L::configureDefaults(){
    if (!resetRegisters())                              return false;
    if (!setMode(Mode::Continuous))                     return false;
    if (!setOutputRate(OutputRate::Hz10))               return false;
    if (!setOverSampleRate(OverSampleRate::x8))         return false;
    if (!setDownSampleRate(DownSampleRate::x8))         return false;
    if (!setSetResetMode(SetResetMode::SetOnly))        return false;
    if (!setRange(Range::Gauss8))                       return false;
    return true;
}

bool QMC5883L::isDRDY(){
    return (bus.read(this->address, QMC5883L_STATUS_REG) & 0b01) != 0;
}

bool QMC5883L::isOVFL(){
    return (bus.read(this->address, QMC5883L_STATUS_REG) & 0b10) != 0;
}

void QMC5883L::calibrate(uint32_t calibrationTime){
    // Stores the max and min magnetometer reading per axis: [axis][0=min, 1=max]
    int16_t minMaxReadings[3][2] = {
        {INT16_MAX, INT16_MIN},
        {INT16_MAX, INT16_MIN},
        {INT16_MAX, INT16_MIN},
    };

    // Tracks timestamp of last array update
    uint32_t timeStamp = clock.millis();

    bool isCalibrated = false;
    while (!isCalibrated) {
        Serial.println("Keep moving compass...");

        // Read all three axes individually so each updates internal state
        int16_t readings[3];
        bool ok = true;
        ok &= readAxis(QMC5883L_XLSB_REG, xRaw, x, xGauss, xMin, xMax);
        ok &= readAxis(QMC5883L_YLSB_REG, yRaw, y, yGauss, yMin, yMax);
        ok &= readAxis(QMC5883L_ZLSB_REG, zRaw, z, zGauss, zMin, zMax);
        readings[0] = xRaw;
        readings[1] = yRaw;
        readings[2] = zRaw;

        if (ok) {
            for (uint8_t i = 0; i < 3; ++i) {
                if (readings[i] < minMaxReadings[i][0]) {
                    minMaxReadings[i][0] = readings[i];
                    timeStamp = clock.millis();
                }
                if (readings[i] > minMaxReadings[i][1]) {
                    minMaxReadings[i][1] = readings[i];
                    timeStamp = clock.millis();
                }
            }
        }

        if (clock.millis() - timeStamp > calibrationTime) {
            isCalibrated = true;
        }
        clock.delayMs(10);
    }

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

bool QMC5883L::read(){
    bool ok = true;
    ok &= readAxis(QMC5883L_XLSB_REG, this->xRaw, this->x, this->xGauss, this->xMin, this->xMax);
    ok &= readAxis(QMC5883L_YLSB_REG, this->yRaw, this->y, this->yGauss, this->yMin, this->yMax);
    ok &= readAxis(QMC5883L_ZLSB_REG, this->zRaw, this->z, this->zGauss, this->zMin, this->zMax);
    return ok;
}

float QMC5883L::azimuth(float xNorm, float yNorm) const {
    float angle = atan2f(yNorm, xNorm) * RAD_TO_DEG;
    // Convert to compass bearing [0, 360)
    return fmodf(450.0f - angle, 360.0f);
}

// -----------------------------------------------------------------------------
// Private helpers
// -----------------------------------------------------------------------------

bool QMC5883L::readAxis(uint8_t reg, int16_t& rawStorage, float& normStorage, float& gaussStorage, int16_t maxVal, int16_t minVal){
    uint8_t buf[2];
    if (!bus.readBytes(this->address, reg, buf, 2)) {
        return false;
    }
    // QMC5883L output registers are little-endian: LSB at reg, MSB at reg+1
    int16_t val = static_cast<int16_t>((static_cast<uint16_t>(buf[1]) << 8) | buf[0]);
    rawStorage   = val;
    normStorage  = normalize(val, maxVal, minVal);
    gaussStorage = static_cast<float>(val) * lsbRes;
    return true;
}

float QMC5883L::normalize(int16_t val, int16_t maxVal, int16_t minVal) {
    if (maxVal == minVal) {
        return 0.0f;
    }
    float center    = (maxVal + minVal) / 2.0f;
    float halfRange = (maxVal - minVal) / 2.0f;
    return (val - center) / halfRange;
}
