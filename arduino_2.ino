#include <LiquidCrystal.h>
#include "pitches.h"

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Life Indicator
const int led1 = 13; // LED 1 - set to pin 13
const int led2 = 10; // LED 2 - set to pin 10
const int led3 = 9; // LED 3 - set to pin 9
const int buzzer = 7; // buzzer - set to pin 7

int score = 0; // display our scoreboard
int ButtonState = 0; // reads hint button status
int lives = 3; // player starts with 3 lives, an LED turns off if player gets pattern incorrect

void setup() {
  // put your setup code here, to run once:
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  pinMode(led1, OUTPUT); // set pin to led 1
  pinMode(led2, OUTPUT); // set pin to led 2
  pinMode(led3, OUTPUT); // set pin to led 3
  pinMode(buzzer, OUTPUT); // set pin to buzzer
  Serial.begin(9600);
  Serial.setTimeout(100);
}

void loop(){
  lcd.clear(); // clear the LCD screen
  lcd.setCursor(0,0);
  lcd.print("Score: ");
  lcd.print(score); // print our current score
  lcd.setCursor(0,1);
  lcd.print("Lives: ");
  lcd.print(lives); // display number of lives 

  // Three LEDs are on to indicate 3 lives left
  if(lives == 3){
    digitalWrite(led1, HIGH); // on 
    digitalWrite(led2, HIGH); // on 
    digitalWrite(led3, HIGH); // on
  }

  // allows us to communiate from Arduino 1 to Arduino 2
  if(Serial.available() > 0){
    String serialInput = Serial.readString();
    checkInput(serialInput);
  }
}

// helper function - plays a noise when an LED or button is used
void playTune(String num){
  // button 1 is pressed
  if(num == "1"){
    tone(buzzer, NOTE_C4);
    delay(150);
    noTone(buzzer);
  }
  // button 2 is pressed
  if(num == "2"){
    tone(buzzer, NOTE_G3);
    delay(150);
    noTone(buzzer);
  }
  // button 3 is pressed
  if(num == "3"){
    tone(buzzer, NOTE_A3);
    delay(150);
    noTone(buzzer);
  }
  // button 4 is pressed
  if(num == "4"){
    tone(buzzer, NOTE_B3);
    delay(150);
    noTone(buzzer);  
  }
  // button 5 is pressed
  if(num == "5"){
    tone(buzzer, NOTE_G2);
    delay(150);
    noTone(buzzer);
  }
}

// checks the type of input that was sent from Arduino 1 to Arduino 2
void checkInput(String input){
  String type = input.substring(0,4); // 4 letter string is checked and what was inputted
  String num = input.substring(5,6); // which button number that was inputted

  // if the LED is used, buzzer plays a noise
  if(type == "fake"){
    playTune(num); // buzzer is used
  }
  
  // if a button is pressed, add a point to score and buzzer plays a noise
  if(type == "note"){
    score++; // increment the score
    playTune(num); // buzzer is used
  }

  // player presses an incorrect button
  if(type == "scor"){
    // sets the score back to 0 if the player gets pattern wrong
    if(num == "0"){
      lives--; // decrement the lives you have
      score = 0; // set score back to 0

      // two LEDs are on if player has 2 lives
      if(lives == 2){
        digitalWrite(led1, HIGH); // on
        digitalWrite(led2, HIGH); // on
        digitalWrite(led3, LOW); // off
      }
      // one LED is on if player has 1 life
      if(lives == 1){
        digitalWrite(led1, HIGH); // on
        digitalWrite(led2, LOW); // off
        digitalWrite(led3, LOW); // off
      }
      // no LEDs are on and it's game over if 0 lives
      if(lives == 0){
        digitalWrite(led1, LOW); // off
        digitalWrite(led2, LOW); // off
        digitalWrite(led3, LOW); // off
        lcd.clear(); // clear the LCD screen
        lcd.setCursor(0,0);
        lcd.print("Game Over!!!!!"); // display game over on the screen
        delay(5000);
        exit(0); // exit the code
      }
    }
  }
}
