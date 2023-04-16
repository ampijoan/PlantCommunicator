/*
 * Project ForestNet
 * Description: A plant network communication system
 * Author: Adrian Pijoan
 * Date: 10-APR-2023
 */

#include <credentials.h> // Radio address, network, name, and password stored here

int SENDADDRESS;   // address of radio to be sent to. Set depending on where you want data to go.

const int PULSEPIN = A1;
const int PULSEREADPIN = A2;
const int PLANTREADPIN = A3;
const int sleepDuration = 60000;
int i = 0;
int hz, startTime;
float firstMax, maxPlantReading, slope;
int plantReadArray [20][2];

void plantImpRead(float _hz, int _PULSEPIN, int _PULSEREADPIN, int _PLANTREADPIN, float *maxPlantReading);

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

  Serial.begin(9600);
  //waitFor(Serial.isConnected,10000);
  //delay(3000);

  //LoRa setup
  Serial1.begin(115200);
  delay(1000);
  reyaxSetup(password);
  SENDADDRESS = 0xA2;

  //PinModes
  pinMode(PULSEPIN, OUTPUT);
  pinMode(PLANTREADPIN, INPUT);

  //initialize values
  hz = 500;
  startTime = millis();

}

void loop() {

  //set i back to 0 and start taking samples every minute
  if(millis() - startTime >= 60000){
    i = 0;

    startTime = millis();
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
    slope = (10000-500)/(plantReadArray[0][0]-plantReadArray[19][0]);

    //send argon name, slope, and Max value
    sendData(myName, maxPlantReading, slope);

    //lock out of both if statements until next time it's time to get data
    i = 21;

  }

}

//Read impedence values from the plant at current frequency for 1 second
void plantImpRead(float _hz, int _PULSEPIN, int _PULSEREADPIN, int _PLANTREADPIN, float *_maxPlantReading){
  int _startTime;
  float _plantReading, _pulseReading, _plantImp, _min, _max;
  
  _startTime = millis();
  _min = 4096;
  _max = 0;

  while(millis() - _startTime <= 1000){
    analogWrite(_PULSEPIN, 127, _hz);
    _pulseReading = analogRead(_PULSEREADPIN);
    _plantReading = analogRead(_PLANTREADPIN);

    //Serial.printf("pulse read: %f\nplant read: %f\n", _pulseReading, _plantReading);
    if(_pulseReading != 0.0){
      _plantImp = (_plantReading / _pulseReading);

      if(_plantImp < _min){
        _min = _plantImp;
      }

      if(_plantImp > _max){
        _max = _plantImp;
      }
    }
    }

  Serial.printf("max: %f\n", _max);
  *_maxPlantReading = _max;
  //not currently returning min, but could if it becomes interesting

}

//Send data with LoRa module
void sendData(String name, float maxPlantReading, float slope) {
  char buffer[60];
  sprintf(buffer, "AT+SEND=%i, 60, %f, %f, %s\r\n", SENDADDRESS, maxPlantReading, slope, name.c_str());
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