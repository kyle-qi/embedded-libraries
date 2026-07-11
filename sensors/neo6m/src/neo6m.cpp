#include "neo6m.h"
#include <string.h>
#include <stdlib.h>

Neo6M::Neo6M(HardwareSerial &serial, uint16_t baud, uint8_t rxPin, uint8_t txPin)
: serial(serial), baudRate(baud), rx(rxPin), tx(txPin) {
    lastSentence[0] = '\0'; // initialize empty
}

void Neo6M::begin(){
    this->serial.begin(this->baudRate, SERIAL_8N1, this->rx, this->tx);
    delay(1000);
}

bool Neo6M::isDRDY(){
    return (this->serial.available() > 0);
}

bool Neo6M::read() {
    static char buffer[NEO6M_BUFFER_SIZE];
    static uint8_t index = 0;

    while (this->serial.available()) {
        char c = this->serial.read();

        if (c == '\n') {
            buffer[index] = '\0'; // terminate string

            // Save full raw sentence (for debug)
            strncpy(this->lastSentence, buffer, NEO6M_BUFFER_SIZE);
            this->lastSentence[NEO6M_BUFFER_SIZE - 1] = '\0';

            index = 0;

            if (strncmp(buffer, "$GPGGA", 6) == 0) {
                char *token;
                uint8_t fieldIndex = 0;

                token = strtok(buffer, ",");
                while (token != NULL) {
                    switch (fieldIndex) {
                        case 1:
                            if (strlen(token) >= 6) {
                                this->hour   = (token[0]-'0')*10 + (token[1]-'0');
                                this->minute = (token[2]-'0')*10 + (token[3]-'0');
                                this->second = (token[4]-'0')*10 + (token[5]-'0');
                            }
                            break;
                        case 2: {
                            float rawLat = atof(token);
                            int deg = int(rawLat / 100);
                            float min = rawLat - deg*100;
                            this->latitude = deg + min/60.0;
                            break;
                        }
                        case 3:
                            if (token[0] == 'S') this->latitude = -this->latitude;
                            break;
                        case 4: {
                            float rawLon = atof(token);
                            int deg = int(rawLon / 100);
                            float min = rawLon - deg*100;
                            this->longitude = deg + min/60.0;
                            break;
                        }
                        case 5:
                            if (token[0] == 'W') this->longitude = -this->longitude;
                            break;
                        case 7: {
                            int n = atoi(token);
                            this->numSatelites = (n < 0) ? 0 : (n > 255) ? 255 : static_cast<uint8_t>(n);
                            break;
                        }
                        case 8:
                            this->horizontalDilution = atof(token);
                            break;
                        case 9:
                            this->altitude = atof(token);
                            break;
                        case 11:
                            this->geoidHeight = atof(token);
                            break;
                    }
                    token = strtok(NULL, ",");
                    fieldIndex++;
                }
                return true;
            }

        } else {
            if (index < NEO6M_BUFFER_SIZE - 1) {
                buffer[index++] = c;
            } else {
                index = 0;
            }
        }
    }
    return false;
}

