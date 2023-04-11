/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/ForestNet/src/ForestNet.ino"
/*
 * Project ForestNet
 * Description: This code is now the playground for writing just the plant reading aspect of the project
 * Author: Adrian Pijoan
 * Date: 29-MAR-2023
 */


void setup();
void loop();
#line 9 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/ForestNet/src/ForestNet.ino"
const int PULSEPIN = A1;
const int PLANTREADPIN = A2;
int i = 0;
int j = 0;
int k = 0;
int hz, startTime, sampleTime;
int plantReading, avgPlantReading, totalPlantReading;
int plantReadingArray [20][2];

int plantImpRead(int _hz, int _j);

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  pinMode(PULSEPIN, OUTPUT);
  pinMode(PLANTREADPIN, INPUT);

  hz = 500; //initialize wave frequency at 500hz

  startTime = millis();
  sampleTime = millis();

}

void loop() {

  //write a statement to set j to 0 if a condition is met (whatever condition nec to start gathering data)

  if(j < 20){
    //analogWrite(PULSEPIN, 127, hz);

    plantReadingArray[j][0] = hz;
    plantReadingArray[j][1] = plantImpRead(hz, j);
    j = j + 1;
    hz = hz + 500;

    // i = 0;

    // while(millis() - startTime <= 1000){
    //   //if(millis() - sampleTime <= 100){ ADD THIS SAMPLE DELAY BACK IN IF NUMBERS ARE GETTING TOO LARGE
    //     plantReading = random(0, 10000);     //analogRead(PLANTREADPIN);
    //     i = i+1;

    //   totalPlantReading = totalPlantReading + plantReading;
    //   //sampleTime = millis();
    // //}
    // }

    // avgPlantReading = (totalPlantReading / i);
    
    // Serial.printf("%i\n", avgPlantReading);

    // plantReadingArray[j][0] = hz;
    //  avgPlantReading;
    // avgPlantReading = 0;
    // totalPlantReading = 0;

    // startTime = millis();
  }

  if(j == 20){
    for(k=0;k<20;k++){
      hz = plantReadingArray[k][0];
      plantReading = plantReadingArray[k][1];
      Serial.printf("frequency: %i      avg plant Reading: %i\n", hz, plantReading);
    }

    if(k == 20){
      j=0;
      hz = 500;
    }
  }



  /* Code to measure plant impedence and see the range of values.
  commented out while working on LoRa comm
  Measures from 500-10000hz in intervals of 250 hz once a second */
  // if(startTime - millis() >= 1000){

  //   if(hz <= 10000){
  //     analogWrite(PULSEPIN, 127, hz);   //syntax for setting frequency of PWM pin -- analogWrite(pin, value, frequency);

  //     plantReading = analogRead(PLANTREADPIN);

  //     plantReadingArray[i] = plantReading;
      
  //     Serial.printf("Reading from plant: %.4f at %i hz\n\n", plantReading, hz);

  //     hz = hz + 250; //increase by 250 hz

  //     i++;

  //   }

  //   else{
  //     printf("Done\n");
  //   }

  // }


}

int plantImpRead(int _hz, int _j){
  int i = 0;
  int _plantReading, _avgPlantReading, _totalPlantReading, _startTime;

  _startTime = millis();
  _totalPlantReading = 0;

  while(millis() - _startTime <= 1000){
    _plantReading = random(0, 10000);     //analogRead(PLANTREADPIN);
    i = i+1;

    _totalPlantReading = _totalPlantReading + _plantReading;

  }

  _avgPlantReading = _totalPlantReading / i;


  return _avgPlantReading;


}

