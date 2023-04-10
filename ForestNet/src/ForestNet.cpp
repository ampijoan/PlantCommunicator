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

#include <credentials.h>

// Define LoRa User and Credentials
void setup();
void loop();
void sendData(String name, float plantReading);
void reyaxSetup(String password);
#line 11 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/ForestNet/src/ForestNet.ino"
String password = "setPassword"; // AES128 password
String myName = "setUsername";
const int RADIOADDRESS = 0xA1; // User defined value between 0xA1 - 0xAF
const int TIMEZONE = -6;

// Define Constants
const int RADIONETWORK = 7;    // range of 0-16
int SENDADDRESS;   // address of radio to be sent to. Set depending on where you want data to go.
// Argon1 = 0xA1, Argon2 = 0xA2, Boron 0xA3

const int PULSEPIN = A1;
const int PLANTREADPIN = A2;
int i = 0;
int hz, startTime;
float plantReading;
float plantReadingArray [39][2];

void setup() {

  Serial1.begin(115200);
  reyaxSetup(password);

  pinMode(PULSEPIN, OUTPUT);
  pinMode(PLANTREADPIN, INPUT);

  hz = 500; //initialize wave frequency at 500hz

  SENDADDRESS = 0xA1;

}

void loop() {

  if (Serial1.available())  { // full incoming buffer: +RCV=203,50,35.08,9,-36,41
    String parse0 = Serial1.readStringUntil('=');  //+RCV
    String parse1 = Serial1.readStringUntil(',');  // address received from
    String parse2 = Serial1.readStringUntil(',');  // buffer length
    String parse3 = Serial1.readStringUntil(',');  // fuseSound
    String parse4 = Serial1.readStringUntil(',');  // fuseDust
    String parse5 = Serial1.readStringUntil(',');  // rssi
    String parse6 = Serial1.readStringUntil('\n'); // snr
    String parse7 = Serial1.readString();          // extra
  } 

   sendData(myName, plantReading);

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

//Send data with LoRa module
void sendData(String name, float plantReading) {
  char buffer[60];
  sprintf(buffer, "AT+SEND=%i,60,%f,%f,%i,%s\r\n", SENDADDRESS, plantReading, name.c_str());
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