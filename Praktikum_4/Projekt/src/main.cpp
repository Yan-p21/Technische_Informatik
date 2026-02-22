#include <Arduino.h>
#include "MyTraffic.hpp"
#include "Actions.hpp"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR     0x3C   // Adresse standard des SSD1306 I2C


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Display-Objekt erstellen
Adafruit_MPU6050 mpu; // MPU6050 Objekt erstellen
Servo myServo; // Servo-Objekt erstellen
#define myServoPin 27 // Pin für Servo



MyTraffic sm; // State machine instance

enum EventType { TICK, REQUESTGREEN };

struct Event { EventType type; };

QueueHandle_t eventQueue;


void taskTrafficLight(void *Parameters);
void taskTaster(void *Parameters);
void taskTick(void *Parameters);
void taskMenu(void *Parameters);
void taskAnalog(void *Parameters);
void taskBarrier(void *Parameters);

// 
bool barrierOpen = false; // Barrier state
unsigned long barrierCloseTimestamp = 0; // Timestamp when barrier was closed
bool pendingRequestGreen = false; // pending request green

void openBarrier() {
    myServo.write(0);
    barrierOpen = true;
}

void closeBarrier() {
    myServo.write(180);
    barrierOpen = false;
    barrierCloseTimestamp = millis();
}

//

void setup() {
    //
    Serial.begin(9600); 

    Wire.begin(17, 16); // SDA, SCL pins for ESP32

    if (!mpu.begin()) {
       Serial.println("MPU6050 nicht gefunden!");
       while (true);
    }

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("OLED konnte nicht gestartet werden!");
        while (true);
    }

    myServo.attach(myServoPin); 
    barrierCloseTimestamp = millis();
    //

    // State Machine initialization
    MyTraffic_ctor(&sm);
    MyTraffic_start(&sm);

    // LEDs Initialization
    initRedLight();
    initYellowLight();
    initGreenLight();

    // Status LEDs Initialization
    initStatusLeds();

    pinMode(switchPin, INPUT_PULLUP); // Button pin
    
    Serial.println("Traffic Light State Machine Initialized.");

    // Create event queue
    eventQueue = xQueueCreate(10, sizeof(Event));
    if (eventQueue == NULL) {
        Serial.println("Error creating the event queue.");
        while (1); // Halt if queue creation fails
    }

    // Create tasks
    xTaskCreate(taskTrafficLight, "TrafficLightTask", 2048, NULL, 2, NULL);
    xTaskCreate(taskTaster, "TasterTask", 2048, NULL, 1, NULL);
    xTaskCreate(taskMenu, "MenuTask", 2048, NULL, 1, NULL);
    xTaskCreate(taskTick, "TickTask", 2048, NULL, 3, NULL); // 2048 stack size
    xTaskCreate(taskAnalog, "AnalogTask", 2048, NULL, 2, NULL);
    xTaskCreate(taskBarrier, "BarrierTask", 2048, NULL, 2, NULL);


}
void loop() {
    // Empty. Tasks are running independently.
}

// Task to sent events to the state machine
void taskTrafficLight(void *Parameters) {
    Event e;

    while (true) {
        if (xQueueReceive(eventQueue, &e, portMAX_DELAY) == pdTRUE) {
            switch (e.type) {
                case TICK:
                    MyTraffic_dispatch_event(&sm, MyTraffic_EventId_TICK);
                    break;
                case REQUESTGREEN:
                    //requestGreen = true; // set requestGreen variable

                    MyTraffic_dispatch_event(&sm, MyTraffic_EventId_REQUESTGREEN);
                    break;
            }
        }
    }
}

// auto-generate tick events every 1 ms
void taskTick(void *Parameters) {
    Event e;
    e.type = TICK;

    while (true) {
        xQueueSend(eventQueue, &e, portMAX_DELAY);
        vTaskDelay(1); // 1 ms interval
    }
}

void taskTaster(void *Parameters) {
   while (true) {
     if (digitalRead(switchPin) == LOW) { // Button pressed
        pendingRequestGreen = true;
        requestGreen = true; // set requestGreen variable
        updateLedMode(&sm); // Update mode LEDs

         /*
          Event e;
         e.type = REQUESTGREEN;
         xQueueSend(eventQueue, &e, portMAX_DELAY);
         */
        

         // Wait for button release
         while (digitalRead(switchPin) == LOW) {
             vTaskDelay(10);
         }
     }
        vTaskDelay(10);
   }
}

// Task to handle user menu via Serial
void taskMenu(void *Parameters) {
    while (true) {
        Serial.println("Wählen Sie einen Eintrag per Eingabe der Nummer:");
        Serial.println("1. Auslesen des Leucht-Zustands einer Ampel");
        Serial.println("2. Setzen des Requests bei einer Ampel");
        Serial.println("");
        Serial.print("Auswahl: ___");

        while (!Serial.available()) { vTaskDelay(10); }
        char choice = Serial.read();

        switch (choice) {
            case '1':
                Serial.print("Aktueller Zustand der Ampel: ");
                Serial.println(MyTraffic_state_id_to_string(sm.state_id));
                Serial.println("");
                break;
            case '2':
                pendingRequestGreen = true;

                requestGreen = true; // set requestGreen variable
                updateLedMode(&sm); // Update mode LEDs

                /*
                Event e;
                e.type = REQUESTGREEN;
                xQueueSend(eventQueue, &e, portMAX_DELAY);
                */
                
                Serial.println("Requestgreen event gesendet.");
                Serial.println("");
               
                break;
            
            default:
                Serial.println("Ungültige Auswahl. Bitte erneut versuchen.");
                break;
        }

        vTaskDelay(10);
    }
}

void taskAnalog(void *Parameters) {
    while (true) {
        light = analogRead(pinLdr);
        greenTime = map(analogRead(pinPot), 0, 4095, 3000, 15000);

        updateLedMode(&sm);
        vTaskDelay(50); // Update light level every 50 ms
    }
}

// Task to handle barrier based on MPU6050 readings
void taskBarrier(void *Parameters) {
    while (true) {

        // read MPU6050 data
        sensors_event_t a, g, temp; // accelerometer, gyroscope, temperature
        mpu.getEvent(&a, &g, &temp);

        float angle = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI; // berechnen des Neigungswinkels in Grad
       // intervalle für lever up/down um falsche auslösungen zu vermeiden
        bool leverUp = angle > 10;
        bool leverDown = angle < -10;

        // display angle and servo position on OLED
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(2); 
        display.setTextColor(SSD1306_WHITE);
        display.print("Angle: ");
        display.println(angle, 1);
        display.print("Schranke: ");
        display.println(leverUp ? "UP" : "DOWN"); 
        display.display();

        // open barrier if lever is up and traffic light is red
        if (leverUp && sm.state_id == MyTraffic_StateId_TRAFFICLIGHTSTATERED) {
            openBarrier();
        }
        // close barrier if lever is down and barrier is open
        if (leverDown && barrierOpen) {
            closeBarrier();
        }

       // allow green only if barrier closed for 4 seconds 
       if (!barrierOpen && 
            millis() - barrierCloseTimestamp >= 4000 
            && pendingRequestGreen == true && 
            sm.state_id == MyTraffic_StateId_TRAFFICLIGHTSTATERED) 
        { 
            Event e; 
            e.type = REQUESTGREEN; 
            xQueueSend(eventQueue, &e, 0); 

            pendingRequestGreen = false; // reset pending request
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}



/*
//Test Poteniometer

const int pinPot = 14;

void setup() {
  Serial.begin(115200);      // start serial communication at 115200bps
}

void loop() {
  int value = analogRead(pinPot);  // read the input on analog pin 0
  int greenTime = map(value, 0, 4095, 3000, 15000);
  
  Serial.print("value : ");
  Serial.println(value);         // print the value to the serial monitor
  Serial.print("greenTime : ");
  Serial.println(greenTime);         // print the value to the serial monitor
 
  delay(500); // wait for greenTime milliseconds 
}
*/


/*
// Test LDR

const int pinLdr = 35;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int light = analogRead(pinLdr);
  Serial.print("value: ");
  Serial.println(light);
  delay(500);
}
*/

/*
//Nachtmodus
enum Mode { DAY, NIGHT };
Mode mode = DAY;

const int pinLdr = 35;
const int thresholdNight = 1500;

void updateMode() {
  int light = analogRead(pinLdr);

  if (light < thresholdNight) {
    mode = NIGHT;
  } else {
    mode = DAY;
  }
}
*/

/*
void taskTaster(void *Parameters) {
    const int switchPin = 18; // Pin for the button
    int lastButtonState = HIGH;
    unsigned long pressTime = 0;
    const unsigned long longPressDuration = 1000; // 1 second for long press

    while (true) {
        int buttonState = digitalRead(switchPin);

        if (buttonState == LOW && lastButtonState == HIGH) { // Button pressed
            pressTime = millis();
        }
        if (buttonState == HIGH && lastButtonState == LOW) { // Button released
            unsigned long duration = millis() - pressTime;
            Event e;
            e.type = (duration >= longPressDuration) ? REQUESTGREEN : TICK;
            xQueueSend(eventQueue, &e, portMAX_DELAY);
        }
        
        lastButtonState = buttonState;
        vTaskDelay(10); // Debounce delay
    }

}
*/







 