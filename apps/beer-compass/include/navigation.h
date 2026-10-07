#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <Arduino.h>
#include <cstddef>

#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.017453292519943295769236907684886
#endif

#ifndef EARTH_RADIUS
#define EARTH_RADIUS 6371000
#endif

/**
 * @brief Struct to hold the longitutde and latitude of a location.
 */
struct Location {
    float lat;
    float lon;
};

/**
 * @brief Calculates the closest location from a list of locations.
 * 
 * @param myLocation The user's current location.
 * @param locations An array of locations.
 * @param nLocations The number of locations in the locations array.
 * @return the Location struct containing the closest latitutde/longitutde coordinates.
 */
Location closestTarget(const Location myLocation, const Location locations[], size_t nLocations);

/**
 * @brief Calculates the relative heading of the user to a target location.
 * 
 * @param azimuth The user's heading with respect to magnetic north.
 * @param myLocation The user's location.
 * @param target The target location.
 * @return The relative heading towards the target location.
 */
float targetHeading(const float azimuth, const Location myLocation, const Location target);

/**
 * @brief Calculates the distance between the user's location and a target location.
 * 
 * @param myLocation The user's location.
 * @param target The target location.
 * @return The distance to the target location.
 */
float targetDistance(const Location myLocation, const Location target);

#endif