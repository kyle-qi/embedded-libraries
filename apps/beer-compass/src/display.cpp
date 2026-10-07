#include "display.h"
#include <Adafruit_GFX.h>
#include <math.h>

void drawNeedle(float azimuth, float target, Adafruit_SSD1306 &display){
    int16_t cx = display.width() / 2;
    int16_t cy = display.height() / 2;
    int16_t r = 20;

    float theta = radians(target - azimuth);
    int16_t x1 = cx + static_cast<int16_t>(r * cosf(theta));
    int16_t y1 = cy - static_cast<int16_t>(r * sinf(theta));

    display.drawCircle(cx, cy, r, SSD1306_WHITE);
    display.drawLine(cx, cy, x1, y1, SSD1306_WHITE);
}

void drawDistance(float distance, Adafruit_SSD1306 &display){
    int16_t cx = display.width() / 2;
    int16_t cy = display.height() / 2;
    int16_t r = 20;

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);

    int roundedDist = static_cast<int>(lroundf(distance));
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d m", roundedDist);

    int16_t x, y;
    uint16_t w, h;
    display.getTextBounds(buffer, 0, 0, &x, &y, &w, &h);

    int16_t textX = cx - static_cast<int16_t>(w / 2);
    int16_t textY = cy + r + 4;

    display.setCursor(textX, textY);
    display.print(buffer);
}

void updateCompass(float azimuth, float target, float distance, Adafruit_SSD1306 &display){
    display.clearDisplay();
    
    drawNeedle(azimuth, target, display);
    drawDistance(distance, display);

    display.display();
}