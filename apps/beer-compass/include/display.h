#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

/**
 * @brief Update's the display's needle direction.
 * 
 * @param azimuth The user's current heading in degrees, using standard compass convention.
 * @param target The azimuth of the target in degrees, using standard compass convention.
 * @param display Reference to the Adafruit_SSD1306 display object to draw on.
 */
void drawNeedle(float azimuth, float target, Adafruit_SSD1306 &display);

/**
 * @brief Updates the display's distance to target.
 * 
 * @param distance The distance to the target in meters.
 * @param display Reference to the Adafruit_SSD1306 display object to draw on.
 */
void drawDistance(float distance, Adafruit_SSD1306 &display);

/**
 * @brief Updates the display's needle direction and distance to target.
 * 
 * @param azimuth The user's current heading in degrees, using standard compass convention.
 * @param target The azimuth of the target in degrees, using standard compass convention.
 * @param distance The distance to the target in meters.
 * @param display Reference to the Adafruit_SSD1306 display object to draw on.
 */
void updateCompass(float azimuth, float target, float distance, Adafruit_SSD1306 &display);

#endif