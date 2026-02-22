#pragma once
#include <stdint.h>
#include <Arduino.h>

// Pins Light Definitions
const int RED_PIN = 2;
const int YELLOW_PIN = 4;
const int GREEN_PIN = 5;


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
