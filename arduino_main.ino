/* Team 12
 *  Aakash Kotak, Patricia Guera, Christopher Rattanasamay
 *  akotak4, pguera3, cratta3
 *  LED Memory Game
 *  For our Arduino project, we are making a game called the “Memory LED Game” 
 *  and the purpose of this game is for the user to memorize and copy the order 
 *  that the LEDs that light up in a certain amount of time. The user will have 
 *  three lives, and as you progress into the game, the harder the game becomes.
 */

#include <QueueArray.h>
#include <Wire.h>

#define TIMESIZE 2

QueueArray <int> queue;
QueueArray <int> playerInput;

int level = 1; // game starts at level 1, increments when a sequence is correct
int lives = 3; // player starts with 3 lives, decrements when a player gets pattern incorrect or time runs out
int score = 0; // player starts at 0 points, increments when a player guesses a sequence correctly

bool nextLevel = false; // set to false, becomes true when passes a level
bool gamePlay = true;

const int led1 = 7; // LED 1 - set to pin 7
const int led2 = 6; // LED 2 - set to pin 6
const int led3 = 5; // LED 3 - set to pin 5
const int led4 = 4; // LED 4 - set to pin 4
const int led5 = 3; // LED 5 - set to pin 3

const int button1 = 8; // button 1 - set to pin 8
const int button2 = 9; // button 2 - set to pin 9
const int button3 = 10; // button 3 - set to pin 10
const int button4 = 11; // button 4 - set to pin 11
const int button5 = 12; // button 5 - set to pin 12

int buttonState1 = 0; // reads button 1 status
int buttonState2 = 0; // reads button 2 status
int buttonState3 = 0; // reads button 3 status
int buttonState4 = 0; // reads button 4 status
int buttonState5 = 0; // reads button 5 status

bool hint = false; // for our hint 
int num_hints = 3; // num of hints

byte Time = 60;
int Timer;

void setup() {
  pinMode(led1, OUTPUT); // set pin to led 1
  pinMode(led2, OUTPUT); // set pin to led 2
  pinMode(led3, OUTPUT); // set pin to led 3
  pinMode(led4, OUTPUT); // set pin to led 4
  pinMode(led5, OUTPUT); // set pin to led 5
  pinMode(button1, INPUT); // set pin to button 1
  pinMode(button2, INPUT); // set pin to button 2
  pinMode(button3, INPUT); // set pin to button 3
  pinMode(button4, INPUT); // set pin to button 4
  pinMode(button5, INPUT); // set pin to button 5
  Serial.begin(9600);
  Serial.setTimeout(100);
  Wire.begin();
  Wire.setTimeout(100);
}

void loop() {
  // Wait to move onto the next level
  if(!nextLevel){
    if(!queue.isEmpty()){
      while(!queue.isEmpty()){
        queue.pop();
      }
    }
    populateQueue(queue, level); // generates a new level
    nextLevel = true; // set to true when player goes to next level
  }
  while(!queue.isEmpty() && lives > 0 && gamePlay == true) // Add a time constraint as well
  {
     Wire.beginTransmission(4);
     Wire.write(Time); // adds to timer to arduino 1
     Wire.endTransmission(4);
     user_input(queue, playerInput);
     Wire.beginTransmission(4);
     Wire.write(100);
     Wire.endTransmission(4);
     level++; // increment level
     turnOnAllLights(); // turn on LEDs
     nextLevel = false;
  }
}

void user_input(QueueArray<int>& queue,QueueArray<int>& playerInput){
  while(!queue.isEmpty() && lives > 0 && Timer > -1 && gamePlay == true){
    Wire.requestFrom(4,1); // grabs the time from arduino 3 to arduino 1
    while(Wire.available() > 0){
      Timer = Wire.read(); // reads the timer
      // player loses a life with timer hits 1
      if(Timer == 1){
        level--; // decrease level to stay in same level
        wrong_input(); // player enters wrong button
        nextLevel = false;
        // empty the queue
        while(!queue.isEmpty()){
          queue.pop();
        }
        Wire.beginTransmission(4); // reset the timer
        Wire.write(100); // ends the timer
        Wire.endTransmission(4); // exit
        delay(100);
        nextLevel = false;
        return;
      }
    }

    // reads each button pressed as a button state
    buttonState1 = digitalRead(button1);
    buttonState2 = digitalRead(button2);
    buttonState3 = digitalRead(button3);
    buttonState4 = digitalRead(button4);
    buttonState5 = digitalRead(button5);
    delay(10);

    if(buttonState1 == HIGH){
      // top of the queue is led 1
      if(led1 == queue.peek()){
        Serial.println("note 1"); // button 1 to buzz
        digitalWrite(led1, HIGH); // on
        delay(75);
        queue.pop();
        digitalWrite(led1, LOW); // off
        delay(75);
      }
      // wrong button
      else{
        wrong_input();
      }
    }
    
    if(buttonState2 == HIGH){
      // top of the queue is led 2
      if(led2 == queue.peek()){
        digitalWrite(led2, HIGH); // on
        Serial.println("note 2"); // button 2 to buzz
        delay(75);
        queue.pop();
        digitalWrite(led2, LOW); // off
        delay(75);
      }
      // wrong button
      else{
        wrong_input();
      }
    }
      
    if(buttonState3 == HIGH){
      // top of the queue is led 3
      if(led3 == queue.peek()){
        Serial.println("note 3");
        digitalWrite(led3,HIGH);
        delay(75);
        queue.pop();
        digitalWrite(led3,LOW);
        delay(75);
      }
      // wrong button
      else{
        wrong_input();
      }
    }
    if(buttonState4 == HIGH){
      // top of the queue is led 4
      if(led4 == queue.peek()){
        Serial.println("note 4");
        digitalWrite(led4, HIGH);
        delay(75);
        queue.pop();
        digitalWrite(led4, LOW);
        delay(75);
      }
      // wrong button
      else{
        wrong_input();
      }
    }
    if(buttonState5 == HIGH){
      // top of the queue is led 5
      if(led5 == queue.peek()){
        Serial.println("note 5");
        digitalWrite(led5,HIGH);
        delay(75);
        queue.pop();
        digitalWrite(led5,LOW);
        delay(75);
      }
      // wrong button
      else{
        wrong_input();
      }
    }
  }

  // game over
  if(lives == 0){
    Wire.beginTransmission(4);
    Wire.write(101);
    Wire.endTransmission(4);
    Serial.println("Over");
    delay(500);
    exit(0);
  }
  return;
}

// creates a random generated pattern between our 5 LEDs
void populateQueue(QueueArray<int>& queue,int level){
  // level increments when the player passes a level
  for(int i = 0; i < level; i++){
    int led = random(3,8); // generates a random number between our 3 and 7 pins, sets it to led 
    buttonPrint(led); // buzzer will make a sound when leds light up
    queue.push(led); // pushes the random generated LED into the queue
    Serial.println();
    turnLightOn(led); // turns on the LED
  }
}

// buzzer makes a noise when a led lights up
void buttonPrint(int led){
  // led 5
  if(led == 3){
    Serial.println("fake 5");
  }
  // led 4
  if(led == 4){
    Serial.println("fake 4");
  }
  // led 3
  if(led == 5){
    Serial.println("fake 3");
  }
  // led 2
  if(led == 6){
    Serial.println("fake 2");
  }
  // led 1
  if(led == 7){
    Serial.println("fake 1");
  }
}

// helper function to make LED blink
void turnLightOn(int led){
  digitalWrite(led, HIGH); // turns on LED
  delay(500); // wait half a second
  digitalWrite(led, LOW); // turns off LED
  delay(500); // wait half a second
}

// when the player passes a level, turns on all the lights to indicate it's correct
void turnOnAllLights(){
  digitalWrite(led1,HIGH); // turns on led 1
  digitalWrite(led2,HIGH); // turns on led 2
  digitalWrite(led3,HIGH); // turns on led 3
  digitalWrite(led4,HIGH); // turns on led 4
  digitalWrite(led5,HIGH); // turns on led 5
  delay(2000); // wait 2 seconds
  digitalWrite(led1,LOW); // turns off led 1
  digitalWrite(led2,LOW); // turns off led 2
  digitalWrite(led3,LOW); // turns off led 3
  digitalWrite(led4,LOW); // turns off led 4
  digitalWrite(led5,LOW); // turns off led 5
  delay(3000); // wait 3 seconds before next level generates
}

// when player presses an incorrect button for the pattern, have all the lights blink to indicate player is wrong
void wrong_input(){
  digitalWrite(led1, HIGH); // turns on led 1
  delay(75); // wait .075 seconds
  digitalWrite(led1, LOW); // turns off led 1
  digitalWrite(led2, HIGH); // turns on led 2
  delay(75); // wait .075 seconds
  digitalWrite(led2, LOW); // turns off led 2
  digitalWrite(led3, HIGH); // turns on led 3
  delay(75); // wait .075 seconds 
  digitalWrite(led3, LOW); // turns off led 3
  digitalWrite(led4, HIGH); // turns on led 4
  delay(75); // wait .075 seconds
  digitalWrite(led4, LOW); // turns off led 4
  digitalWrite(led5, HIGH); // turns on led 5
  delay(75); // wait .075 seconds 
  digitalWrite(led5, LOW); // turns off led 5
  lives--; // decrement the amount of lives when player guesses wrong
  Serial.println("scor 0"); // Serial communication for Arduino 3 - set score to 0 when wrong
}
