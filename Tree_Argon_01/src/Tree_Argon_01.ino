/*
 * Project ForestNet
 * Description: A plant network communication system
 * Author: Adrian Pijoan
 * Date: 10-APR-2023
 */

#include <credentials.h> // Radio address, network, name, and password stored here

int SENDADDRESS;   // address of radio to be sent to. Set depending on where you want data to go.

const int PULSEPIN = A1;
const int PLANTREADPIN = A2;
int i = 0;
int hz, startTime;
int plantReading;
int plantReadingArray [20][2];

int plantImpRead(int _hz, int _i, int _PULSEPIN);

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

  Serial.begin(9600);
 
  waitFor(Serial.isConnected,10000);
  delay(3000);

  Serial1.begin(115200);
  delay(1000);
  reyaxSetup(password);

  pinMode(PULSEPIN, OUTPUT);
  pinMode(PLANTREADPIN, INPUT);

  hz = 500; //initialize wave frequency at 500hz

  SENDADDRESS = 0xA2;

  startTime = millis();

}

void loop() {

  //if statement to set i back to 0, for example on wake up.

  if(i < 20){
    
    plantReadingArray[0][i] = hz;
    plantReadingArray[1][i] = plantImpRead(hz, i, PULSEPIN);

    i = i + 1;
    hz = hz + 500;
  }

  //parse out array here and decide what to send.



  if (Serial1.available())  { // full incoming buffer: +RCV=203,50,35.08,9,-36,41
    String parse0 = Serial1.readStringUntil('=');  //+RCV
    String parse1 = Serial1.readStringUntil(',');  // address received from
    String parse2 = Serial1.readStringUntil(',');  // buffer length
    String parse3 = Serial1.readStringUntil(',');  // plantReading
    String parse5 = Serial1.readStringUntil(',');  // rssi
    String parse6 = Serial1.readStringUntil('\n'); // snr
    String parse7 = Serial1.readString();          // extra
  } 

}


//Send data with LoRa module
void sendData(String name, int plantReading) {
  char buffer[60];
  sprintf(buffer, "AT+SEND=%i,60,%i,%s\r\n", SENDADDRESS, plantReading, name.c_str());
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

//Read impedence values from the plant at current frequency for 1 second
int plantImpRead(int _hz, int _i, int _PULSEPIN){
  int i = 0;
  int _plantReading, _avgPlantReading, _totalPlantReading, _startTime;

  _startTime = millis();
  _totalPlantReading = 0;

  while(millis() - _startTime <= 1000){
    analogWrite(_PULSEPIN, 127, _hz);
    _plantReading = random(0, 10000);     //analogRead(PLANTREADPIN);
    i = i+1;

    _totalPlantReading = _totalPlantReading + _plantReading;

  }

  _avgPlantReading = _totalPlantReading / i;

  return _avgPlantReading;

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