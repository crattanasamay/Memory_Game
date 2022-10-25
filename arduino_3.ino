#include <SevSeg.h>

#include "SevSeg.h"
#include <Wire.h>
SevSeg sevseg; //Instantiate a seven segment object

#define TIMESIZE 2

float displayTimeSecs = 1; //how long do you want each number on display to show (in secs)
float displayTime = (displayTimeSecs * 5000);
long startNumber = 0; //countdown starts with this number
long endNumber = 0; //countdown ends with this number
bool gameplay = false;

void setup() {
  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = false;  // Default 'false' is Recommended
  bool leadingZeros = true; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = true; // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  Wire.onRequest(requestEvent);
  Wire.setTimeout(100);
  Serial.setTimeout(100);
}

void loop() {
  Wire.beginTransmission(4);
  Wire.write(200);
  Wire.endTransmission(4);
  while(gameplay){
    // keeps the timer counting down
    if(startNumber >= endNumber){
      for (long i = 0; i <= displayTime; i++){
        sevseg.setNumber(startNumber, 0); // set the start timer to 30
        sevseg.refreshDisplay();
      } 
      startNumber--; // count down
    }
    if(startNumber == 59){
      Wire.beginTransmission(4);
      Wire.write(200);
      Wire.endTransmission(4);
    }
  }
  sevseg.setNumber(0000,0); //after countdown shows endNumber, show this number.
  sevseg.refreshDisplay();
}

void receiveEvent(int howMany){
  // reads in Wire communication
  while(Wire.available() > 0){
    int c = Wire.read(); // set the timer to an int
    Serial.println(c);
    delay(50);
    // set the initial timer to 30
    if(c == 60){
      startNumber = 60;
      gameplay = true;
    }
    // get out of the loop
    if(c == 100){
      startNumber = 100;
      gameplay = false;
    }
    // game over
    if(c == 101){
      exit(0);
    }
  }
}

void requestEvent(){
  Serial.println(startNumber);
  Wire.write(startNumber);
}
