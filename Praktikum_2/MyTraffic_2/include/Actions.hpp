#pragma once
#include <stdint.h>
#include <Arduino.h>

// Pins Light Definitions
const int RED_PIN = 13;
const int YELLOW_PIN = 12;
const int GREEN_PIN = 14;


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

extern bool requestGreen; // Declaration of external variable to simulate request green event