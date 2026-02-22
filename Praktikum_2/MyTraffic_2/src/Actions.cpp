#include "Actions.hpp"
#include <iostream> // for std::cout
#include <Arduino.h> // for Arduino functions


// Implementations of initialization functions
void initRedLight() { pinMode(RED_PIN, OUTPUT); digitalWrite(RED_PIN, LOW); }
void initYellowLight() { pinMode(YELLOW_PIN, OUTPUT); digitalWrite(YELLOW_PIN, LOW); }
void initGreenLight() { pinMode(GREEN_PIN, OUTPUT); digitalWrite(GREEN_PIN, LOW); }

//  Implementations of action functions
void turnOnRedLight() { digitalWrite(RED_PIN, HIGH); }
void turnOffRedLight() { digitalWrite(RED_PIN, LOW); }

void turnOnYellowLight() { digitalWrite(YELLOW_PIN, HIGH); }
void turnOffYellowLight() { digitalWrite(YELLOW_PIN, LOW); }

void turnOnGreenLight() { digitalWrite(GREEN_PIN, HIGH); }
void turnOffGreenLight() { digitalWrite(GREEN_PIN, LOW); }

//bool requestGreen = false; // Global variable to simulate request green event

