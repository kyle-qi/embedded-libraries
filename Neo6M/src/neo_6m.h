#ifndef NEO_6M_H
#define NEO_6M_H

#include <Arduino.h>

// Bufer to store Neo6M reading
#define NEO6M_BUFFER_SIZE 128

class Neo6M {
    public:

    /**
     * @brief Class constructor.
     * 
     * @param serial
     * @param baud
     * @param rxPin
     * @param txPin
     */
    Neo6M(HardwareSerial &serial, uint16_t baud, uint8_t rxPin, uint8_t txPin);

    /**
     * @brief Initializes the Neo6M's serial port.
     */
    void begin();

    /**
     * @brief Reads an NMEA sentence and stores it in the object's member variables.
     * 
     * @return true if the operation succeeded, false otherwise.
     */
    bool read();

    /**
     * @brief Tells you if information is available in the serial register.
     * 
     * @return true if any information is available in the register, false otherwise.
     */
    bool isDRDY();

    /**
     * @brief Returns the most recent hour reading.
     * 
     * @return The most recent hour reading.
     */
    float getHour() {return this->hour;}

    /**
     * @brief Returns the most recent minute reading.
     * 
     * @return The most recent minute reading.
     */
    float getMinute() {return this->minute;}

    /**
     * @brief Returns the most recent second reading.
     * 
     * @return The most recent second reading.
     */
    float getSecond() {return this->second;}

    /**
     * @brief Returns the most recent latitude reading in decimal degrees.
     * 
     * @return The most recent latitutde reading in decimal degrees.
     */
    float getLatitude() {return this->latitude;}

    /**
     * @brief Returns the most recent longitutde reading in decimal degrees.
     * 
     * @return The most recent longitutde reading in decimal degrees.
     */
    float getLongitude() {return this->longitude;}

    /**
     * @brief Returns the most recent horizontal dilution reading.
     * 
     * @return The most recent horizontal dilution reading.
     */
    float getHorizontalDilution() {return this->horizontalDilution;}

    /**
     * @brief Returns the most recent altitude reading in metres.
     * 
     * @return The most recent altitude reading in metres.
     */
    float getAltitude() {return this->altitude;}

    /**
     * @brief Returns the most recent geoid height in metres.
     * 
     * @return The most recent geoid height in metres.
     */
    float getGeoidHeight() {return this->geoidHeight;}

    /**
     * @brief Returns the most recent number of satelites detected/used.
     * 
     * @return The most recent number of satelites detected/used.
     */
    uint8_t getNumSatelites() { return this->numSatelites; }

    // TODO: Create Doxygen
    /**
     * @brief TBD
     * 
     * @return TBD
     */
    char* getLastSentence() {return this->lastSentence;}
    
    private:

    /**
     * @brief The instance's serial port.
     */
    HardwareSerial &serial;

    /**
     * @brief The Neo6M's baud rate.
     */
    uint16_t baudRate;

    /**
     * @brief The RX, and TX pins used for UART communication.
     */
    uint8_t rx, tx;

    /**
     * @brief The objects's time variables.
     */
    uint8_t hour, minute, second;

    /**
     * @brief The object's latitutde and longitude.
     */
    float latitude, longitude;

    /**
     * @brief The object's horizontal dilution.
     */
    float horizontalDilution;

    /**
     * @brief The object's altitutde.
     */
    float altitude;

    /**
     * @brief The object's geoid height.
     */
    float geoidHeight;

    /**
     * @brief The object's number of satelites detected/used.
     */
    uint8_t numSatelites;

    // TODO: Create doxygen
    /**
     * @brief TBD
     */
    char lastSentence[NEO6M_BUFFER_SIZE];
};

#endif
