/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/Tree_Argon_01/src/Tree_Argon_01.ino"
/*
 * Project ForestNet
 * Description: A plant network communication system
 * Author: Adrian Pijoan
 * Date: 10-APR-2023
 */

#include <credentials.h> // Radio address, network, name, and password stored here

void setup();
void loop();
float plantImpRead(float _hz, int _PULSEPIN, int _PULSEREADPIN, int _PLANTREADPIN);
void sendData(String name, float maxPlantReading, float slope);
void reyaxSetup(String password);
#line 10 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/Tree_Argon_01/src/Tree_Argon_01.ino"
int SENDADDRESS;   // address of radio to be sent to. Set depending on where you want data to go.

const int PULSEPIN = A1;
const int PULSEREADPIN = A2;
const int PLANTREADPIN = A3;
const int sleepDuration = 60000;
int i = 0;
int hz, startTime;
float firstMax, plantReading, maxPlantReading, slope;
int plantReadArray [20];

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

  Serial.begin(9600);
  //waitFor(Serial.isConnected,10000);
  delay(3000);

  //LoRa setup
  Serial1.begin(115200);
  delay(1000);
  reyaxSetup(password);
  SENDADDRESS = 0xA2;

  //PinModes
  pinMode(PULSEPIN, OUTPUT);
  pinMode(PULSEREADPIN, INPUT);
  pinMode(PLANTREADPIN, INPUT);

  //initialize values
  hz = 500;
  startTime = millis();

}

void loop() {

  //set i back to 0 and start taking samples every minute
  if(millis() - startTime >= 60000){
    i = 0;
    
    hz = 500;

    startTime = millis();
  }

  if(i < 20){
    plantReading = plantImpRead(hz, PULSEPIN, PULSEREADPIN, PLANTREADPIN);
    plantReadArray[i] = plantReading;
    
    i = i + 1;
    hz = hz + 500.0;
  }

  if(i == 20){
    maxPlantReading = plantReadArray[0];
    //calculate slope
    slope = ((plantReadArray[19] - plantReadArray[0])/(20.0));

    //Serial.printf("max: %f\nslope: %f\n", maxPlantReading, slope);

    //send argon name, slope, and Max value
    sendData(myName, maxPlantReading, slope);

    //lock out of both if statements until next time it's time to get data
    i = 21;

  }

}

//Read impedence values from the plant at current frequency for 1 second
float plantImpRead(float _hz, int _PULSEPIN, int _PULSEREADPIN, int _PLANTREADPIN){
  int _startTime;
  float _plantReading, _pulseReading, _min, _max;
  
  _startTime = millis();
  _min = 4096;
  _max = 0;

  while(millis() - _startTime <= 1000){
    analogWrite(_PULSEPIN, 127, _hz);
    _pulseReading = analogRead(_PULSEREADPIN);
    _plantReading = analogRead(_PLANTREADPIN);

    if(_plantReading < _min){
      _min = _plantReading;
    }

    if(_plantReading > _max){
      _max = _plantReading;
    }
    
  }

  Serial.printf("max: %f\n", _max);

  return _max;

}

//Send data with LoRa module
void sendData(String name, float maxPlantReading, float slope) {
  char buffer[60];
  sprintf(buffer, "AT+SEND=%i, 60, %.1f, %.2f, %s\r\n", SENDADDRESS, maxPlantReading, slope, name.c_str());
  Serial1.printf("%s",buffer);
  Serial.printf("%s",buffer);
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

// If you want to add a sleep function, this will sleep in ulta low power mode
// void sleepULP(){
//   SystemSleepConfiguration config;
//   config.mode(SystemSleepMode::ULTRA_LOW_POWER).duration(sleepDuration);
//   SystemSleepResult result = System.sleep(config);
//}