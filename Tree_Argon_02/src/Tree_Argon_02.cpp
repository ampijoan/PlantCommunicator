/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/Tree_Argon_02/src/Tree_Argon_02.ino"
/*
 * Project ForestNet
 * Description: A plant network communication system
 * Author: Adrian Pijoan
 * Date: 10-APR-2023
 */

#include <credentials.h> // Radio address, network, name, and password stored here

void setup();
void loop();
void plantImpRead(float _hz, int _PULSEPIN, int _PULSEREADPIN, int _PLANTREADPIN, float *_maxPlantReading);
void sendData(String name, float plant01Max, float plant01Slope, float maxPlantReading, float slope);
void reyaxSetup(String password);
#line 10 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/Tree_Argon_02/src/Tree_Argon_02.ino"
int SENDADDRESS;   // address of radio to be sent to. Set depending on where you want data to go.

const int PULSEPIN = A1;
const int PULSEREADPIN = A2;
const int PLANTREADPIN = A3;
int i;
int hz, startTime;
float firstMax, maxPlantReading, slope, plant01Slope, plant01Max;
float plantReadArray [20][2];

void plantImpRead(int _hz, int _PULSEPIN, int _PLANTREADPIN, float *_maxPlantReading);

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

  Serial.begin(9600);
  //waitFor(Serial.isConnected, 10000);

  Serial1.begin(115200);
  reyaxSetup(password);

  pinMode(PULSEPIN, OUTPUT);
  pinMode(PULSEREADPIN, INPUT);
  pinMode(PLANTREADPIN, INPUT);

  hz = 500; //initialize wave frequency at 500hz

  SENDADDRESS = 0xA3;

  startTime = millis();
  i = 21; //wait until we receive data from other microcontrollers to start doing things

}

void loop() {


  //when there's incoming LoRa data, parse the data and then get ready to take data
  if (Serial1.available())  { // full incoming buffer: +RCV=203,50,35.08,9,-36,41
    String parse0 = Serial1.readStringUntil('=');  //+RCV
    String parse1 = Serial1.readStringUntil(',');  // address received from
    String parse2 = Serial1.readStringUntil(',');  // buffer length
    String parse3 = Serial1.readStringUntil(',');  // max value from plant 01
    String parse4 = Serial1.readStringUntil(','); // slope from plant 01
    String parse5 = Serial1.readStringUntil(',');  // rssi
    String parse6 = Serial1.readStringUntil('\n'); // snr
    String parse7 = Serial1.readString();          // extra

    Serial.printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", parse0.c_str(), parse1.c_str(), parse2.c_str(), parse3.c_str(), parse4.c_str(), parse5.c_str(), parse6.c_str(), parse7.c_str());
    plant01Max = strtof(parse3.c_str(),NULL);
    plant01Slope = strtof(parse4.c_str(),NULL);
    Serial.printf("Plant 01 Max: %f\n", plant01Max);
    Serial.printf("Plant 01 slope: %f\n", plant01Slope);

    //set counter to zero to start taking data after receiving data
    i = 0;

  }


  if(i < 20){

    plantImpRead(hz, PULSEPIN, PULSEREADPIN, PLANTREADPIN, &maxPlantReading);

    if(hz == 500.0){
      firstMax = maxPlantReading;
    }
    //store raw values in 0 and normalized values in 1
    plantReadArray[i][0] = maxPlantReading;
    plantReadArray[i][1] = (maxPlantReading/firstMax);

    i = i + 1;
    hz = hz + 500.0;

  }

  if(i == 20){
    //calculate slope
    slope = (10000-500)/(plantReadArray[0][0] - plantReadArray[19][0]);
    Serial.printf("max: %f\nslope: %f\nreceived max: %f\nreceived slope: %f\n", maxPlantReading, slope, plant01Max, plant01Slope);
    //send previous plant data and new plant data
    sendData(myName, plant01Max, plant01Slope, maxPlantReading, slope);

    //lock out of both if statements until next time it's time to get data
    i = 21;

  }

}

//Read impedence values from the plant at current frequency for 1 second
void plantImpRead(float _hz, int _PULSEPIN, int _PULSEREADPIN, int _PLANTREADPIN, float *_maxPlantReading){
  int _startTime;
  float _plantReading, _pulseReading, _min, _max;
  
  _startTime = millis();
  _min = 4096;
  _max = 0;

  while(millis() - _startTime <= 1000){
    analogWrite(_PULSEPIN, 127, _hz);
    _pulseReading = analogRead(_PULSEREADPIN);
    _plantReading = analogRead(_PLANTREADPIN);

    //Serial.printf("pulse read: %f\nplant read: %f\n", _pulseReading, _plantReading);

      if(_plantReading < _min){
        _min = _plantReading;
      }

      if(_plantReading > _max){
        _max = _plantReading;
      }
    }

  Serial.printf("max: %f\n", _max);
  *_maxPlantReading = _max;
  //not currently returning min, but could if it becomes interesting

}

//Send data with LoRa module
void sendData(String name, float plant01Max, float plant01Slope, float maxPlantReading, float slope) {
  char buffer[60];
  sprintf(buffer, "AT+SEND=%i,60,%.1f,%.2f,%.1f,%.2f,%s\r\n", SENDADDRESS, plant01Max, plant01Slope, maxPlantReading, slope, name.c_str());
  Serial1.printf("%s",buffer);
  //Serial1.println(buffer); 
  delay(1000);
  if (Serial1.available() > 0)
  {
    Serial.printf("Awaiting Reply from send\n");
    String reply = Serial1.readStringUntil('\n');
    Serial.printf("Send reply: %s\n", reply.c_str());
  }
}

//set up the Reyax LoRa modules
void reyaxSetup(String password) {
  // following four paramaters have most significant effect on range
  // recommended within 3 km: 10,7,1,7
  // recommended more than 3 km: 12,4,1,7
  const int SPREADINGFACTOR = 10;
  const int BANDWIDTH = 7;
  const int CODINGRATE = 1;
  const int PREAMBLE = 7;
  String reply; // string to store replies from module

  Serial1.printf("AT+ADDRESS=%i\r\n", RADIOADDRESS); // set the radio address
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply from address\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("Reply address: %s\n", reply.c_str());
  }

  Serial1.printf("AT+NETWORKID=%i\r\n", RADIONETWORK); // set the radio network
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply from networkid\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("Reply network: %s\n", reply.c_str());
  }

  Serial1.printf("AT+CPIN=%s\r\n", password.c_str());
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply from password\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("Reply: %s\n", reply.c_str());
  }

  Serial1.printf("AT+PARAMETER=%i,%i,%i,%i\r\n", SPREADINGFACTOR, BANDWIDTH, CODINGRATE, PREAMBLE);
  delay(200);
  if (Serial1.available() > 0) {
    reply = Serial1.readStringUntil('\n');
    Serial.printf("reply: %s\n", reply.c_str());
  }

  Serial1.printf("AT+ADDRESS?\r\n");
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("Radio Address: %s\n", reply.c_str());
  }

  Serial1.printf("AT+NETWORKID?\r\n");
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("Radio Network: %s\n", reply.c_str());
  }

  Serial1.printf("AT+PARAMETER?\r\n");
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("RadioParameters: %s\n", reply.c_str());
  }

  Serial1.printf("AT+CPIN?\r\n");
  delay(200);
  if (Serial1.available() > 0) {
    Serial.printf("Awaiting Reply\n");
    reply = Serial1.readStringUntil('\n');
    Serial.printf("Radio Password: %s\n", reply.c_str());
  }
}

//If you want sleep -- sleep until Argon receives data over Serial1 (LoRa)
// void sleepULP(){
//   SystemSleepConfiguration config;
//   config.mode(SystemSleepMode::ULTRA_LOW_POWER).usart(Serial1);
//   SystemSleepResult result = System.sleep(config);

// }