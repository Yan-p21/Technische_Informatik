#pragma once
#include <stdint.h>
#include <Arduino.h>
#include "MyTraffic.hpp"

// Pins Light Definitions
const int RED_PIN = 23;
const int YELLOW_PIN = 22;
const int GREEN_PIN = 21;
const int switchPin = 19; // Pin for the button

const int pinLdr = 35;
const int pinPot = 14; // Pin for potentiometer
extern const int thresholdNight; //night threshold
extern int light; // current light level, updated in updateMode()
extern int greenTime; // green time duration in milliseconds

// Green lEDs for mode indication
const int LedDay = 32; // Day mode LED
const int LedNight = 33; // Night mode LED
const int LedRequest = 25; // Request green LED
const int LedPot = 26; // Potentiometer height level LED

extern bool requestGreen; // Declaration of external variable to simulate request green event

// Declarations of initialization functions
void initRedLight();
void initYellowLight();
void initGreenLight();

// Declarations of action functions
void turnOnRedLight();
void turnOffRedLight();

void turnOnYellowLight();
void turnOffYellowLight();

void turnOnGreenLight();
void turnOffGreenLight();

void updateLedMode(MyTraffic* sm);
void initStatusLeds();

