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
float plantImpRead(float _hz, int _i, int _PULSEPIN, int _PULSEREADPIN, int _PLANTREADPIN);
#line 11 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/Impedence_measurement_SD/src/Impedence_measurement_SD.ino"
const int chipSelect = SS;
const char FILE_BASE_NAME[]="impede";

const int PULSEPIN = A1;
const int PULSEREADPIN = A2;
const int PLANTREADPIN = A3;
int i, j, hz, fileNumber, startTime;
float plantReading;
float plantReadingArray [20][2];

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

  pinMode(STARTPIN, INPUT);
  pinMode(PULSEPIN, OUTPUT);
  pinMode(PLANTREADPIN, INPUT);

  hz = 500;                                                     //initialize square wave frequency at 500hz

  startTime = millis();
  delay(1000);

}

void loop() {

  Serial.printf("Press button to log data \n");
  logStart = digitalRead(STARTPIN);

  while(logStart==false) {                                      //wait for button to be pressed
    logStart = digitalRead(STARTPIN);
    delay(5);
  }

  Serial.printf("Starting Data Logging \n");

  while (sd.exists(fileName)) {                                 //cycle through files until number not found
    fileNumber++;
    sprintf(fileName,"%s%02i.csv",FILE_BASE_NAME,fileNumber);   //create numbered filename
    Serial.printf("Filename is %s\n",fileName);
  }

  if (!file.open(fileName, O_WRONLY | O_CREAT | O_EXCL)) {      // open file for printing
    Serial.println("File Failed to Open");
  }
  file.printf("Frequency, Plant reading \n");                   // print header row

  /* Measure plant impedence and log to SD.
  Measures from 500-10000hz in intervals of 500 hz once a second */

  if(i < 20){
    plantReadingArray[0][i] = hz;
    plantReadingArray[1][i] = plantImpRead(hz, i, PULSEPIN, PULSEREADPIN, PLANTREADPIN);

    i = i + 1;
    hz = hz + 500;

  }
if(i == 20){
  for (j=0;j<=20;j++) {
    Serial.printf("frequency: %i    plant reading: %i\n", plantReadingArray[j][0], plantReadingArray[j][1]);
    //uncomment to write to SD instead of Serial Monitor
    Serial.print("x");
    file.printf("%f , %f\n", plantReadingArray[j][0], plantReadingArray[j][1]);  //print frequency and measurement to file on SD
    delay(50);
  }

  if(j == 20){
    i = 0;
    file.close();
    Serial.printf("\nDone \n");
    hz  = 500;                                                    //reset frequency to 500hz for next round
    delay(500);
  }
}


}

//Read impedence values from the plant at current frequency for 1 second
float plantImpRead(float _hz, int _i, int _PULSEPIN, int _PULSEREADPIN, int _PLANTREADPIN){
  float j;
  int _startTime;
  float _plantReading, _pulseReading, _plantImp, _avgPlantImp, _totalPlantImp, _min, _max;
  static float _maxAt500, _minAt500;
  
  _startTime = millis();
  _totalPlantImp = 0.0;
  j = 0.0;
  _min = 4096;
  _max = 0;

  while(millis() - _startTime <= 1000){
    analogWrite(_PULSEPIN, 127, _hz);
    _pulseReading = analogRead(_PULSEREADPIN);
    _plantReading = analogRead(_PLANTREADPIN);

    _plantImp = _plantReading / _pulseReading;

    if(_plantReading < _min){
      _min = plantReading;
    }

    if(_plantReading > _max){
      _max = _plantReading;
    }

    j = j + 1.0;
    //_totalPlantImp = (_totalPlantImp + _plantImp);
    
  }

  //this statement lets us normalize the data for EZ plotting in excel
  if(_hz == 500.0){
    _maxAt500 = _max; 
    _minAt500 = _min;
  }

  Serial.printf("first max: %f\nnew max: %f\n", _maxAt500, _max);

  _max = (_max / _maxAt500);
  //_min = (_min / _minAt500 );
  //_avgPlantImp = _max - _min;

  //_avgPlantImp = (_totalPlantImp / j);
  Serial.printf("hz: %f\n", _hz);
  Serial.printf("max: %f\n\n", _max);
  Serial.printf("min: %f\n", _min);
  

  return _max;

}
