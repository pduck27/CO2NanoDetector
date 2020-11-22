// TODO:
// Animation in own function without general loop
// triggerButton shows current value as number in Matrix

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LedControl.h>

// Led configuration
#define MAX_DEVICES 1
#define CS_PIN 10
#define DIN_PIN 11
#define CLK_PIN 13

// Tresholds
#define HIGH_Q 800
#define MEDIUM_Q 1200
#define NO_INTVL 5

// Optional trigger
int tasterPin = 4;

// General variables
int co2value = 0;   
int i = 0;   
int measureInterval = 2000;
int secondsUntilAnim = 5;        
bool animationModus = false;

enum level {UNDEFINED_L, LOW_L, MEDIUM_L, HIGH_L};
level currLevel = UNDEFINED_L;

SoftwareSerial co2Serial(2, 3);                                             // RX from MH-Z19B, TX from MH-Z19B Pin
LedControl display = LedControl(DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);          // LED Pins

// Led pattern
// See https://xantorohara.github.io/led-matrix-editor/#3c42a98585a9423c|3c42a58585a5423c|3c42a58989a5423c
// LEDs are turned 90 degree to right

// Facial Expression (Smile, Neutral, Sad)
const uint64_t SMILEY_IMAGES[] = {
  0x3c42a98585a9423c,
  0x3c42a58585a5423c,
  0x3c42a58989a5423c
};
const int SMILEY_IMAGES_LEN = sizeof(SMILEY_IMAGES)/8;
const int SMILE_IMAGE = 0;
const int MEDIUM_IMAGE = 1;
const int SAD_IMAGE = 2;

// Window Animation
const uint64_t WINDOW_ANIMATION[] = {
  0xff8191ffff9181ff,
  0xffc3c3bdbdc3c3ff,
  0xffc3bd8181bdc3ff,
  0xffff81818181ffff,
  0xffc3bd8181bdc3ff,
  0xffc3c3bdbdc3c3ff,
  0xff8191ffff9181ff,
  0xffc3c3bdbdc3c3ff,
  0xffc3bd8181bdc3ff,
  0xffff81818181ffff,
  0xffc3bd8181bdc3ff,
  0xffc3c3bdbdc3c3ff,
  0xff8191ffff9181ff
};
const int WINDOW_ANIMATION_LEN = sizeof(WINDOW_ANIMATION)/8;
 

// Read MH-Z19 
int readCO2()  
{
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  char response[9];
  co2Serial.write(cmd, 9);
  co2Serial.readBytes(response, 9);
  if (response[0] != char(0xFF)) return -1;
  if (response[1] != char(0x86)) return -1;
  int antwortHigh = (int) response[2];
  int antwortLow = (int) response[3]; 
  int ppm = (256 * antwortHigh) + antwortLow;
  return ppm;                         
}

// Led print pattern
void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      display.setLed(0, i, j, bitRead(row, j));
    }
  }
}

void setup()
{            
  // Start CO2 detection
  co2Serial.begin(9600);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Start CO2 detection");

  // Initialize Led
  display.shutdown(0,false);       
  display.setIntensity(0,0);
  display.clearDisplay(0); 

  // Inititalize trigger button
  pinMode(tasterPin,INPUT);  
}

void loop()
{ 

  if (digitalRead(tasterPin)==HIGH){
    Serial.println("Button high");
  }

  // Read CO2 value
  co2value = readCO2();                
  Serial.println(co2value);  

  // Show "open window" animation or smiley
  if (animationModus){    
    displayImage(WINDOW_ANIMATION[i]);
    if (++i >= WINDOW_ANIMATION_LEN ) {
        animationModus = false;
        secondsUntilAnim = 5;
      }
    delay(500);

  } else
  { 
    // Show smiley
    if (co2value <= HIGH_Q) {
      // Good quality
      if (currLevel != HIGH_L){
        display.clearDisplay(0);
        displayImage(SMILEY_IMAGES[SMILE_IMAGE]);
        currLevel = HIGH_L;  
        Serial.println("Switch to HIGH");
        secondsUntilAnim = 5;
      }

    } else if (co2value <= MEDIUM_Q) {
      // Medium quality
      if (currLevel != MEDIUM_L){
        display.clearDisplay(0);
        displayImage(SMILEY_IMAGES[MEDIUM_IMAGE]);
        currLevel = MEDIUM_L;  
        Serial.println("Switch to MIDDLE");
        secondsUntilAnim = 5;
      }

    } else if (currLevel != LOW_L) {
      // Bad quality 
      display.clearDisplay(0);
      displayImage(SMILEY_IMAGES[SAD_IMAGE]);
      currLevel = LOW_L;
      Serial.println("Switch to LOW");

    } else if (currLevel == LOW_L) {
      // Counter to show window animation when bad quality
      secondsUntilAnim -= 1;
      if (secondsUntilAnim <= 0){
        Serial.println("Show window animation");
        animationModus = true;
        i = 0;
      }
    }
      
    delay(measureInterval); 
  }  
        
}
