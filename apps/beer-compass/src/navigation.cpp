#include "navigation.h"
#include "utils.h"

Location closestTarget(const Location myLocation, const Location locations[], size_t nLocations){
    // Edge case handling
    if(nLocations == 1){
        return locations[0];
    }
    if(nLocations == 0){
        return myLocation;
    }

    size_t nearestIndex = 0;
    float minDistanceSq = MAXFLOAT;

    for (size_t i = 0; i < nLocations; ++i){
        float dx = myLocation.lon - locations[i].lon;
        float dy = myLocation.lat - locations[i].lat;
        float distanceSq = dx * dx + dy * dy;
        if(distanceSq < minDistanceSq){
            minDistanceSq = distanceSq;
            nearestIndex = i;
        }
    }
    Location closest = {locations[nearestIndex].lon, locations[nearestIndex].lat};
    return closest;
}

float targetHeading(const float azimuth, const Location myLocation, const Location target){
    // Convert to radians
    float myLat = myLocation.lat * DEG_TO_RAD;
    float targetLat = target.lat * DEG_TO_RAD;
    float dLon = (target.lon - myLocation.lon) * DEG_TO_RAD;

    // Great circle formula
    float y = sinf(dLon) * cosf(targetLat);
    float x = cosf(myLat) * sinf(targetLat) - sinf(myLat) * cosf(targetLat) * cosf(dLon);
    float heading = atan2f(y, x);

    // Convert back to compass convention
    heading *= RAD_TO_DEG;
    if (heading < 0 ) {
        heading += 360.0f;
    }
    heading -= azimuth;
    if (heading < 0) {
        heading += 360.0f;
    }
    if (heading >= 360.0f) {
        heading -= 360.0f;
    }
    return heading;
}

float targetDistance(const Location myLocation, const Location target){
    float myLat = myLocation.lat * DEG_TO_RAD;
    float targetLat = target.lat * DEG_TO_RAD;
    float dLat = targetLat - myLat;
    float dLon = (target.lon - myLocation.lon) * DEG_TO_RAD;

    float a = sinf(dLat/2) * sinf(dLat/2) + cosf(myLat) * cosf(targetLat) *
              sinf(dLon/2) * sinf(dLon/2);
    
    float c = 2.0f * atan2f(sqrtf(a), sqrtf(1.0f - a));
    return c * EARTH_RADIUS; // Convert to metres
}
