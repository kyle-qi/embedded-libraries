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
    , x(0.0f), y(0.0f), z(0.0f)
    , xGauss(0.0f), yGauss(0.0f), zGauss(0.0f)
    , lsbRes(kDefaultLsbRes)
{}

bool QMC5883L::setMode(Mode mode){
    Result<uint8_t, bool> current = bus.read(this->address, QMC5883L_CTRLA_REG);
    if (!current) {
        return false;
    }
    uint8_t config = current.value;

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
    Result<uint8_t, bool> status = bus.read(this->address, QMC5883L_STATUS_REG);
    if (!status) {
        return false; // treat a failed read as "not ready"
    }
    return (status.value & 0b01) != 0;
}

bool QMC5883L::isOVFL(){
    Result<uint8_t, bool> status = bus.read(this->address, QMC5883L_STATUS_REG);
    if (!status) {
        return false;
    }
    return (status.value & 0b10) != 0;
}

bool QMC5883L::read(){
    bool ok = true;
    ok &= readAxis(QMC5883L_XLSB_REG, this->x, this->xGauss, this->xMin, this->xMax);
    ok &= readAxis(QMC5883L_YLSB_REG, this->y, this->yGauss, this->yMin, this->yMax);
    ok &= readAxis(QMC5883L_ZLSB_REG, this->z, this->zGauss, this->zMin, this->zMax);
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

bool QMC5883L::readAxis(uint8_t reg, float& normStorage, float& gaussStorage, int16_t maxVal, int16_t minVal){
    uint8_t buf[2];
    if (!bus.readBytes(this->address, reg, buf, 2)) {
        return false;
    }
    // QMC5883L output registers are little-endian: LSB at reg, MSB at reg+1
    int16_t val = static_cast<int16_t>((static_cast<uint16_t>(buf[1]) << 8) | buf[0]);
    normStorage  = normalize(val, maxVal, minVal);
    gaussStorage = static_cast<float>(val) * lsbRes;
    return true;
}
