// Code was mainly copied and overworked from other projects.
// Thanks to: https://esp8266-server.de/CO2Ampel.html
// and: https://xantorohara.github.io/led-matrix-editor


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
#define AVERAGE_Q 1200
#define NO_INTVL 5

// Optional trigger button
int buttonPin = 4;

// General variables
int co2value;   
int secondsUntilAnim;        
int currImage;
int measureInterval = 2000;
bool animationModus = false;

enum level {UNDEFINED_L, LOW_L, AVERAGE_L, HIGH_L};
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
const int HIGH_IMAGE = 0;
const int AVERAGE_IMAGE = 1;
const int LOW_IMAGE = 2;

// Numbers
// https://xantorohara.github.io/led-matrix-editor/#003e7f51497f3e00|0001017f7f110100|0031794945672300|00367f4949632200|00047f7f24140c00|004e5f5151737200|00266f49497f3e00|0060785f47606000|00367f49497f3600|003e7f49497b3200
const uint64_t NUMBER_IMAGES[] = {
  0x003e7f51497f3e00,
  0x0001017f7f110100,
  0x0031794945672300,
  0x00367f4949632200,
  0x00047f7f24140c00,
  0x004e5f5151737200,
  0x00266f49497f3e00,
  0x0060785f47606000,
  0x00367f49497f3600,
  0x003e7f49497b3200
};
const int NUMBER_IMAGES_LEN = sizeof(NUMBER_IMAGES)/8;

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

// Switch to new Level
void switchToLevel(int newSmileyImageNo, level newLevel){
  display.clearDisplay(0);
  currLevel = newLevel;        
  currImage = newSmileyImageNo;
  displayImage(SMILEY_IMAGES[currImage]);  
  secondsUntilAnim = 5;
}

// Reset to last state
void resetLevel(){
  switchToLevel(currImage, currLevel);
}

// Display Window Animation
void displayWindowAnimation(){
  display.clearDisplay(0);
  for (int i = 0; i < WINDOW_ANIMATION_LEN; i++){  
    displayImage(WINDOW_ANIMATION[i]);
    delay(500);
  }
  resetLevel();
}

// Display Digit number with delays
void displayNumber(int numberToDisplay)
{
  displayImage(NUMBER_IMAGES[numberToDisplay]);
  delay(800);
  display.clearDisplay(0);
  delay(200);
}

// Show CO2 value as digits
void showValue(int value){
int ones = (value%10);
    int tens = (value/10) %10;
    int hundreds = (value/100) %10;
    int thousands = (value/1000);
    bool showZero = false;
    
    display.clearDisplay(0);
    delay(200);

    if (thousands > 0){      
      displayNumber(thousands);
      Serial.println(thousands);      
      showZero = true;
    }

    if (hundreds > 0 || showZero){      
      displayNumber(hundreds);
      Serial.println(hundreds);      
      showZero = true;
    }

    if (tens > 0 || showZero){      
      displayNumber(tens);
      Serial.println(tens);      
      showZero = true;
    }

    if (ones > 0 || showZero){      
      displayNumber(ones);
      Serial.println(ones);      
    }   
    
    resetLevel();
}

// Setup
void setup()
{            
  Serial.begin(115200);
  
  // Start CO2 detection
  co2Serial.begin(9600);  
  Serial.println("");
  Serial.println("Start CO2 detection");

  // Initialize Led
  display.shutdown(0,false);       
  display.setIntensity(0,0);
  display.clearDisplay(0); 

  // Inititalize trigger button
  pinMode(buttonPin, INPUT);   
}


// Loop
void loop()
{ 

  if (digitalRead(buttonPin)==HIGH){
    Serial.println("Button high, show CO2 value");    
    showValue(co2value);
  }

  // Read CO2 value
  co2value = readCO2();                
  Serial.println(co2value);    
 
  // Show smiley
  if (co2value <= HIGH_Q) {
    // Good quality
    if (currLevel != HIGH_L){
      switchToLevel(HIGH_IMAGE, HIGH_L);      
      Serial.println("Switch to HIGH");      
    }

  } else if (co2value <= AVERAGE_Q) {
    // Medium quality
    if (currLevel != AVERAGE_L){
      switchToLevel(AVERAGE_IMAGE, AVERAGE_L);   
      Serial.println("Switch to AVERAGE");      
    }

  } else if (currLevel != LOW_L) {
    // Bad quality 
    switchToLevel(LOW_IMAGE, LOW_L);  
    Serial.println("Switch to LOW");    

  } else if (currLevel == LOW_L) {
    // Counter to show window animation when bad quality
    secondsUntilAnim -= 1;
    if (secondsUntilAnim <= 0){
      Serial.println("Show window animation");
      displayWindowAnimation();      
    }
  }
    
  delay(measureInterval); 
 
        
}
