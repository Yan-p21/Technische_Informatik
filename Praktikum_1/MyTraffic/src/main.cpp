#include <Arduino.h>
#include "MyTraffic.hpp"
#include "Actions.hpp"


MyTraffic sm; // State machine instance

int switchPin = 18; // Pin for the button
int lastButtonState = HIGH; // Previous state of the button
unsigned long lastButtonTime = 0; // Last time the button was pressed
const unsigned long debounceDelay = 50; // 50 ms debounce

void setup() {
    Serial.begin(9600, SERIAL_8N1); 
    
    MyTraffic_ctor(&sm);
    MyTraffic_start(&sm);

    // LEDs Initialization
    initRedLight();
    initYellowLight();
    initGreenLight();

    pinMode(switchPin, INPUT_PULLUP); // Enable internal pull-up resistor

    Serial.println("Traffic Light State Machine Initialized.");
}

void loop() {

    // Tick event automatically every millisecond
    static unsigned long lastTickTime = 0;
    unsigned long currentTime = millis();
    
    if (currentTime - lastTickTime >= 1) { // 1 ms interval
        MyTraffic_dispatch_event(&sm, MyTraffic_EventId_TICK);
        lastTickTime = currentTime;
    }

    // button = requestGreen event
    int buttonState = digitalRead(switchPin); // Read button state

    if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastButtonTime) > debounceDelay) { // Button pressed
        MyTraffic_dispatch_event(&sm, MyTraffic_EventId_REQUESTGREEN);
        
        Serial.println("REQUESTGREEN sent.");
        lastButtonTime = millis(); // Update last button time
    }
    lastButtonState = buttonState;
    
}
 