/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/Impedence_measurement_SD/src/Impedence_measurement_SD.ino"
/*
 * Project Impedence_measurement_SD
 * Description: Code to measure impedence and store data to an SD card
 * Author: Adrian Pijoan
 * Date: 04-APR-2023
 */

#include <SPI.h>
#include <SdFat.h>

void setup();
void loop();
#line 11 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/Impedence_measurement_SD/src/Impedence_measurement_SD.ino"
const int chipSelect = SS;
const char FILE_BASE_NAME[]="impede";

const int PULSEPIN = A1;
const int PLANTREADPIN = A2;
int i, hz, fileNumber, startTime;
float plantReading;
float plantReadingArray [40][2];

bool logStart;
const int STARTPIN = D7;

const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
char fileName[13];

// Create file system object.
SdFat sd;
SdFile file;


SYSTEM_MODE(SEMI_AUTOMATIC);


void setup() {

  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  // Initialize at the highest speed supported by the board that is
  // not over 50 MHz. Try a lower speed if SPI errors occur.cc
  if (!sd.begin(chipSelect, SD_SCK_MHZ(10))) {
    Serial.printf("Error starting SD Module"); 
  }

  if (BASE_NAME_SIZE > 6) {
    Serial.println("FILE_BASE_NAME too long");
    while(1); //stop program
  }

  fileNumber = 0;
  sprintf(fileName,"%s%02i.csv",FILE_BASE_NAME,fileNumber);

  pinMode(PULSEPIN, OUTPUT);
  pinMode(PLANTREADPIN, INPUT);

  hz = 500; //initialize square wave frequency at 500hz

}

void loop() {

  Serial.printf("Press button to log data \n");
  logStart = digitalRead(STARTPIN);
  //wait for button to be pressed
  while(logStart==false) {
    logStart = digitalRead(STARTPIN);
    delay(5);
  }

  Serial.printf("Starting Data Logging \n");
  while (sd.exists(fileName)) {                                 //cycle through files until number not found
    fileNumber++;
    sprintf(fileName,"%s%02i.csv",FILE_BASE_NAME,fileNumber);   //create numbered filename
    Serial.printf("Filename is %s\n",fileName);
  }
  if (!file.open(fileName, O_WRONLY | O_CREAT | O_EXCL)) {     // open file for printing
    Serial.println("File Failed to Open");
  }
  file.printf("Frequency, Plant reading \n");                    // print header row

  /* Measure plant impedence and log to SD.
  Measures from 500-10000hz in intervals of 250 hz once a second */
  if(startTime - millis() >= 1000){

    for(i=0;i<40;i++){
      analogWrite(PULSEPIN, 127, hz);   //syntax for setting frequency of PWM pin -- analogWrite(pin, value, frequency);
      plantReadingArray[i][0] = hz;
      plantReadingArray[i][1] = analogRead(PLANTREADPIN);

      hz = hz + 250; //increase by 250 hz

      startTime = millis();
    }
  }

  file.close();
  Serial.printf("Done \n");

}