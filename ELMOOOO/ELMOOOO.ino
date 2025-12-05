//01 = good, 001 - 015 for all 3 folders
//02 = medium
//03 = evil
#include <Servo.h>
Servo mouthServo;
//Servo handServo;

//was gonna do individual animations for Elmos sound files, but Rick said maybe mapping sound levels to servo positions would work better
//thought that would make it a lot more natural, its hard to get the correct angles and time frame
//+ it applies to any additional sound files I add in the future
int talkAnimationOne[][2] = {
{70, 250},
{90, 250},
{70, 250},
{60, 250},
{90, 1000},
{65, 250},
{70, 250},
{60, 250},
{50, 250},
{90, 1000}
};

int talkAnimationTwo[][2] = {
{80, 250},
{70, 250},
{80, 250},
{70, 250},
{75, 250},
{70, 250},
{80, 250},
{85, 250},
{70, 250},
{90, 1000},
{65, 500},
{60, 250},
{65, 500},
{60, 750},
{90, 1000}
};

#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySerial(10,11); //RX, TX pins
DFRobotDFPlayerMini myDFPlayer;

//sound sensor
int soundPin = A0;


//button variables
int buttonPin = 4;


boolean buttonState = false;

//folder variable to identify which folder to play
int folder1 = 1;
int folder2 = 2;
int folder3 = 3;

//tracks vars
int track1 = random(1,16);
//int track2 = random(1,16);
int track3 = random(1,11);

//i wanted elmos mood to change based on time, so I asked copilot and it suggested millis timer and mood variables
unsigned long lastMood = 0;
const unsigned long moodInterval = 20000;
int currentMood = 1;

unsigned long evilMood = 0;
const unsigned long evilInterval = 30000;
//after asking copilot what I should do to make my logic work, it recommended I add these two variables for evil elmo
int moodCount = 0;
boolean isEvil = false;


void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT);
  mouthServo.attach(7);
  Serial.begin(9600);
  mySerial.begin(9600);

//audio player initialization
 if (!myDFPlayer.begin(mySerial)){
    Serial.println("DFPlayer mini not detected");
    while(true);
 }
 //sound volume
  myDFPlayer.volume(50);
  
}

void loop() {
//sound sensor stuff
//servo motor less reactive in evil mode?
updateMovement();

  // put your main code here, to run repeatedly:
//millis logic
//copilot said moodCount would count how many intervals have passed, and as long as it hadnt gone above the threshold it would keep counting
if (!isEvil && millis() - lastMood >= moodInterval){
  currentMood = 2;  //random(1,3); //so that will be folders 1 and 2
  lastMood = millis();
  moodCount++;
}

//once threshold is reached, evil mode activates
//the && !isEvil smoothed the servo motor for some reason
if(moodCount >= 5 && !isEvil){
  isEvil = true;
  Serial.println("Evil mode activated");
 evilMood = millis(); //that starts his evil intervals
}

if(isEvil && millis() - evilMood >= evilInterval){
  int track = random(1, 11);
  myDFPlayer.playFolder(folder3, track);
  evilMood = millis();
  }


buttonState = digitalRead(buttonPin);
//if statement to press button = play sound
//only if he isnt in evil mode.
  if(!isEvil && buttonState == true){
    int track = random(1,16);
    //int folderr = random(1,3);
    Serial.println("button pressed, sound playing");
    myDFPlayer.playFolder(currentMood, track);
    //delay(500);

    if(currentMood == 2 && track == 1){
      playAnimation(talkAnimationOne, 10);
}
if(currentMood == 2 && track == 2){
  playAnimation(talkAnimationTwo, 15);
}
  }


/*
//my 1st attempt at making an evil mood
  if(millis() - lastMood <= (moodInterval * 5)){
    currentMood = folder3;
    if(millis() - evilMood <= evilInterval){
    myDFPlayer.playFolder(currentMood, track3);
    }

  }
*/



}


void Track1(int one, int file){
  myDFPlayer.playFolder(one, file);
}

void Track2(int two, int file2){
  myDFPlayer.playFolder(two, file2);
}

void Track3(int three, int file3){
  myDFPlayer.playFolder(three, file3);
}


void playAnimation(int animation[][2], int steps){
for(int i =0; i < steps; i++){
  mouthServo.write(animation[i][0]);
  delay(animation[i][1]);
}
}


float getSmoothReading(int aPin){
  int currentRead;
  static int index = 0;
  const int readAmount = 100;
  static int readingArray[readAmount];
  static unsigned long sum = 0;
  int avg = 0;
//read sensor
  currentRead = analogRead(aPin);
//subtract old reading from sum
  sum -= readingArray[index];
//add new reading to sum
  sum += currentRead;
//put new reading inside array
  readingArray[index] = currentRead;
//update index
  index++;
  //or
//index %= readAmount          // -> cuz mod 3 is 0, 1, 2, 0, 1, 2 looping forever
  if (index >= readAmount){
    index = 0;
  }

//update avg
  avg = sum / readAmount;
  currentRead = avg;
  return currentRead;
}

//sound sensor values = mapped to servo motors
//works so much better than the custom animations
void updateMovement(){
int soundLevel = getSmoothReading(soundPin);
Serial.println(soundLevel);
if(soundLevel > 280){
  int mouthPosition = map(getSmoothReading(soundPin), 200, 500, 90, 65);
mouthServo.write(mouthPosition);
//delay(10);
}
}
