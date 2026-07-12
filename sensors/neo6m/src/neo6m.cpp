#include "neo6m.h"
#include <string.h>
#include <stdlib.h>

Neo6M::Neo6M(ISerial& serial)
    : serial(serial)
    , hour(0), minute(0), second(0)
    , latitude(0.0f), longitude(0.0f)
    , horizontalDilution(0.0f)
    , altitude(0.0f)
    , geoidHeight(0.0f)
    , numSatelites(0)
    , rxIdx(0)
{
    lastSentence[0] = '\0';
    rxBuf[0]        = '\0';
}

bool Neo6M::isDRDY() {
    return serial.available() > 0;
}

bool Neo6M::read() {
    bool parsed = false;

    while (serial.available()) {
        Result<uint8_t, bool> r = serial.read();
        if (!r) {
            break;
        }
        char c = static_cast<char>(r.value);

        if (c == '\n') {
            rxBuf[rxIdx] = '\0';

            // Save raw sentence for debug
            strncpy(lastSentence, rxBuf, NEO6M_BUFFER_SIZE);
            lastSentence[NEO6M_BUFFER_SIZE - 1] = '\0';

            rxIdx = 0;

            if (strncmp(rxBuf, "$GPGGA", 6) == 0) {
                // Work on a mutable copy — strtok modifies the string in place
                char work[NEO6M_BUFFER_SIZE];
                strncpy(work, rxBuf, NEO6M_BUFFER_SIZE);
                work[NEO6M_BUFFER_SIZE - 1] = '\0';

                char*   token      = strtok(work, ",");
                uint8_t fieldIndex = 0;

                while (token != NULL) {
                    switch (fieldIndex) {
                        case 1:
                            if (strlen(token) >= 6) {
                                hour   = static_cast<uint8_t>((token[0]-'0')*10 + (token[1]-'0'));
                                minute = static_cast<uint8_t>((token[2]-'0')*10 + (token[3]-'0'));
                                second = static_cast<uint8_t>((token[4]-'0')*10 + (token[5]-'0'));
                            }
                            break;
                        case 2: {
                            float rawLat = static_cast<float>(atof(token));
                            int   deg    = static_cast<int>(rawLat / 100);
                            float min    = rawLat - deg * 100;
                            latitude     = deg + min / 60.0f;
                            break;
                        }
                        case 3:
                            if (token[0] == 'S') latitude = -latitude;
                            break;
                        case 4: {
                            float rawLon = static_cast<float>(atof(token));
                            int   deg    = static_cast<int>(rawLon / 100);
                            float min    = rawLon - deg * 100;
                            longitude    = deg + min / 60.0f;
                            break;
                        }
                        case 5:
                            if (token[0] == 'W') longitude = -longitude;
                            break;
                        case 7: {
                            int n        = atoi(token);
                            numSatelites = (n < 0) ? 0u : (n > 255) ? 255u : static_cast<uint8_t>(n);
                            break;
                        }
                        case 8:
                            horizontalDilution = static_cast<float>(atof(token));
                            break;
                        case 9:
                            altitude = static_cast<float>(atof(token));
                            break;
                        case 11:
                            geoidHeight = static_cast<float>(atof(token));
                            break;
                    }
                    token = strtok(NULL, ",");
                    fieldIndex++;
                }
                parsed = true;
            }

        } else {
            if (rxIdx < NEO6M_BUFFER_SIZE - 1) {
                rxBuf[rxIdx++] = c;
            } else {
                // Line too long — discard and restart
                rxIdx = 0;
            }
        }
    }

    return parsed;
}
