/**
 * @file my_location.h
 * @brief This file contains the user's desired list of target locations.
 * 
 * @note Longitude and latitude must be expressed in decimal degrees.
 * @note The coordinates must follow N/E positive convention.
 */

#ifndef MY_LOCATIONS_H
#define MY_LOCATIONS_H

#include "navigation.h"

// TODO: Figure out a way to get LCBO locations
/**
 * @brief Location struct array used to hold target location data.
 */
const Location LOCATIONS[] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
};

/**
 * @brief The number of location objects in the LOCATIONS array.
 */
constexpr size_t nLocations = sizeof(LOCATIONS) / sizeof(LOCATIONS[0]);

#endif