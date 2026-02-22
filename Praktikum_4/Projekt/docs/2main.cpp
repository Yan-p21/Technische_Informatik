#include <Arduino.h>

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


/*
void setup() {
  Serial.begin(9600);
  
  Wire.begin(21, 22); // I2C starten (ESP32: SDA=21, SCL=22)

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {  // Display initialisieren
    Serial.println("OLED konnte nicht gestartet werden!");
    while (true); 
  }
  
  /*
  // 2 und 3
  display.clearDisplay();               // Bildschirm löschen
  display.setTextSize(2);               // Charaktergröße, 1 = 6x8 Pixel, 2 = 12x16 Pixel
  display.setTextColor(SSD1306_WHITE);  // Weiß auf Schwarz
  display.setCursor(0, 0);  // Startposition oben links
  
  // Text auf dem Display ausgeben
  display.println("Hallo THM!");
  display.println("TI2");

  // Fließkommazahl auf dem Display ausgeben
  float wert = 23.0123;
  display.println(wert, 2);  // 2 Dezimalstellen

  display.display();       // Anzeigen
  */

/*
// 4
 if (!mpu.begin()) {  // MPU6050 initialisieren
    Serial.println("MPU6050 nicht gefunden!");
    while (true);
  }

  display.clearDisplay(); 
  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE);
}
}

void loop() {
  sensors_event_t a, g, temp; 
  mpu.getEvent(&a, &g, &temp); // Sensorwerte auslesen, a = Beschleunigung (m/s2), g = Gyroskop rad/s, temp = Temperatur (°C)

  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("Beschleunigung:");
  display.print("X: ");
  display.println(a.acceleration.x);

  display.print("Y: ");
  display.println(a.acceleration.y);

  display.print("Z: ");
  display.println(a.acceleration.z);

  display.display();

  delay(200);

 }
*/

/*
 // 5 
void setup() {
  Serial.begin(9600);
  
  myServo.attach(27);
}

void loop() {
  myServo.write(0);    // 0 Grad
  delay(1000);

  myServo.write(90);   // 90 Grad
  delay(1000);

  myServo.write(180);  // 180 Grad
  delay(1000);
}
*/

// 6

void setup() { 
  Serial.begin(9600); 
  Wire.begin(21, 22); 

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { 
    Serial.println("OLED konnte nicht gestartet werden!"); 
    while(true); 
  }
   
  display.clearDisplay(); 
  display.setTextSize(2); 
  display.setTextColor(SSD1306_WHITE); 
  
  if(!mpu.begin()) { 
    Serial.println("MPU6050 nicht gefunden!"); 
    while(true); 
  }
  
  myServo.attach(27); 
}

void loop() { 
  sensors_event_t a, g, temp; 
  mpu.getEvent(&a, &g, &temp); 

  float angle = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI; // Neigungswinkel berechnen

  int servoAngle = map(angle, -90, 90, 0, 180); // Winkel auf 0-180 Grad mappen

  servoAngle = constrain(servoAngle, 0, 180); // Winkel begrenzen

  myServo.write(servoAngle); // Servo auf den berechneten Winkel setzen

  // OLED Anzeige aktualisieren
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Angle: ");
  display.println(angle, 1); // Winkel mit 2 Dezimalstellen
  display.print("Servo: ");
  display.println(servoAngle);

  display.display();
  delay(100);
}


  



