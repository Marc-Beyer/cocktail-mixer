#include "LedControl.h"

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#define PIN 23
#define N_LEDS 72

#define CHASE_DISTANCE 4

#define ArrayLength(x) (sizeof(x) / sizeof(x[0]))

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// 5 villeicht nicht
uint32_t colors[6];  //= {16711680, 65280, 255, 16776960, 16711935};

long nextEffectTime = 0;
int effectDelay = 1025;

void initColors() {
    colors[0] = strip.Color(255, 0, 0);
    colors[1] = strip.Color(0, 55, 0);
    colors[2] = strip.Color(0, 0, 55);
    colors[3] = strip.Color(0, 55, 55);
    colors[4] = strip.Color(55, 55, 0);
    colors[5] = strip.Color(55, 0, 55);
}

void chase() {
    static uint16_t pixelId = 0;
    static uint32_t color = colors[random(ArrayLength(colors))];

    pixelId = (++pixelId) % (strip.numPixels() + CHASE_DISTANCE);
    if (pixelId == 0) {
        color = colors[random(ArrayLength(colors))];
    }

    strip.setPixelColor(pixelId, color);
    strip.setPixelColor(pixelId - CHASE_DISTANCE, 0);
    strip.show();
}

void fill() {
    static uint16_t pixelId = 0;
    static uint16_t length = strip.numPixels();
    static uint32_t color = colors[random(ArrayLength(colors))];

    pixelId = (++pixelId) % (length + CHASE_DISTANCE);
    if (pixelId == 0) {
        // color = colors[random(ArrayLength(colors))];
        length -= CHASE_DISTANCE;
        if (length <= 0) {
            length = strip.numPixels();
            color = colors[random(ArrayLength(colors))];
        }
    }

    strip.setPixelColor(pixelId, color);
    strip.setPixelColor(pixelId - CHASE_DISTANCE, 0);
    strip.show();
}

void fourRegions() {
    uint16_t regionSize = strip.numPixels() / 4;

    uint16_t offset = random(4);
    uint32_t color = colors[random(ArrayLength(colors))];

    for (uint16_t pixelId = 0; pixelId < regionSize; pixelId++) {
        strip.setPixelColor(pixelId + regionSize * offset, color);
    }
    strip.show();
}

void runTColor() {
    static uint8_t index = 0;

    for (uint16_t pixelId = 0; pixelId < strip.numPixels(); pixelId++) {
        strip.setPixelColor(pixelId, colors[index]);
    }
    strip.show();
    index = (++index) % 6;
}

void smoothColors() {
    static uint16_t offset = 0;
    uint32_t color = strip.ColorHSV(offset, 255, 55);

    for (uint16_t pixelId = 0; pixelId < strip.numPixels(); pixelId++) {
        strip.setPixelColor(pixelId, color);
    }
    strip.show();
    offset += 500;
    if (offset > 65535) {
        offset = 0;
    }
}

void highlight(long spot) {
    for (uint16_t pixelId = 0; pixelId < strip.numPixels(); pixelId++) {
        strip.setPixelColor(pixelId, 0);
    }
}

void setupLeds() {
    randomSeed(analogRead(0));

    strip.begin();

    initColors();

    Serial.println(strip.Color(255, 255, 0));
    Serial.println(strip.Color(0, 255, 255));
    Serial.println(strip.Color(255, 0, 255));
}

uint8_t programNr = 0;
long nextProgramTime = 0;

void loopLeds() {
    if (nextProgramTime < millis()) {
        programNr = random(5);
        // Reset all pixel
        for (uint16_t pixelId = 0; pixelId < strip.numPixels(); pixelId++) {
            strip.setPixelColor(pixelId, 0);
        }
        strip.show();
        nextProgramTime = millis() + 30000;
    }
    if (nextEffectTime < millis()) {
        switch (programNr) {
            case 0:
                runTColor();
                nextEffectTime = millis() + 1500;
                break;
            case 1:
                fourRegions();
                nextEffectTime = millis() + 1500;
                break;
            case 2:
                chase();
                nextEffectTime = millis() + 25;
                break;
            case 3:
                fill();
                nextEffectTime = millis() + 25;
                break;
            case 4:
                smoothColors();
                nextEffectTime = millis() + 25;
                break;
        }
    }
}
