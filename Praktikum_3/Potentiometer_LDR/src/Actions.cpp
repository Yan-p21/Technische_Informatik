#include "Actions.hpp"
#include <iostream> // for std::cout
#include <Arduino.h> // for Arduino functions

int light = 0;
int greenTime = 5000; // Default green time in milliseconds
const int thresholdNight = 1200;

bool requestGreen = false;


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

void updateLedMode(MyTraffic* sm) {
    if (sm->state_id == MyTraffic_StateId_YELLOWON || sm->state_id == MyTraffic_StateId_YELLOWOFF) { 
       //requestGreen ignored in night blinking mode
       requestGreen = false;
        // Night mode indication
        digitalWrite(LedDay, LOW); 
        digitalWrite(LedNight, HIGH);     
    } else { 
        digitalWrite(LedNight, LOW); 
        digitalWrite(LedDay, HIGH); 
    }

    digitalWrite(LedRequest, requestGreen ? HIGH : LOW);

    digitalWrite(LedPot, (greenTime > 9000) ? HIGH : LOW); 
}

void initStatusLeds() {
    pinMode(LedDay, OUTPUT);
    pinMode(LedNight, OUTPUT);
    pinMode(LedRequest, OUTPUT);
    pinMode(LedPot, OUTPUT);

    digitalWrite(LedDay, LOW);
    digitalWrite(LedNight, LOW);
    digitalWrite(LedRequest, LOW);
    digitalWrite(LedPot, LOW);
}

/*
void updateLedMode(int lightLevel) {
    if (lightLevel < thresholdNight) {
       digitalWrite(LedDay, LOW);
       digitalWrite(LedNight, HIGH);
    } else {
         digitalWrite(LedNight, LOW);
         digitalWrite(LedDay, HIGH);
    }
     
    if (digitalRead(switchPin) == LOW) {
        digitalWrite(LedRequest, HIGH);
    } else {
        digitalWrite(LedRequest, LOW);
    }

    if (greenTime > 9000) {
        digitalWrite(LedPot, HIGH);
    } else {
        digitalWrite(LedPot, LOW);
    }

}

*/


