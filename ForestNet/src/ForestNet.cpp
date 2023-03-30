/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/ForestNet/src/ForestNet.ino"
/*
 * Project ForestNet
 * Description: A plant network communication system
 * Author: Adrian Pijoan
 * Date: 29-MAR-2023
 */

void setup();
void loop();
#line 8 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/ForestNet/src/ForestNet.ino"
const int PULSEPIN = A1;
const int PLANTREADPIN = A2;
int hz, startTime;
float plantReading;

void setup() {

  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  pinMode(PULSEPIN, OUTPUT);
  pinMode(PLANTREADPIN, INPUT);

  hz = 500; //initialize wave frequency at 500hz

}

void loop() {

  //test code to measure plant impedence and see the range of values
  if(startTime - millis() >= 1000){
    if(hz <= 10000){
      analogWrite(PULSEPIN, 127, hz);   //syntax for setting frequency of PWM pin -- analogWrite(pin, value, frequency);
      hz = hz + 250; //increase by 250 hz

      plantReading = analogRead(PLANTREADPIN);
      Serial.printf("Reading from plant: %.4f at %i hz\n\n", plantReading, hz);

    }

    else{
      printf("Done\n");
    }

  }

}