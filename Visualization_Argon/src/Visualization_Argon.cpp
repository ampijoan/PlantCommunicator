/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/Visualization_Argon/src/Visualization_Argon.ino"
/*
 * Project PlantVisTest
 * Description: Pulling plant data from cloud and sending into visualization software over USB
 * Author: Adrian Pijoan
 * Date: 14-APR-2023
 */

#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"

#include "credentials.h"

//tags used to tell vis software which plant data it is receiving
void setup();
void loop();
float plantImpRead(float _hz, int _PULSEPIN, int _PULSEREADPIN, int _PLANTREADPIN);
void MQTT_connect();
bool MQTT_ping();
#line 15 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/Visualization_Argon/src/Visualization_Argon.ino"
const int PULSEPIN = A1;
const int PULSEREADPIN = A2;
const int PLANTREADPIN = A3;
const int plant01MaxTag = 9911;
const int plant01SlopeTag = 9912;
const int plant02MaxTag = 9921;
const int plant02SlopeTag = 9922;
const int plant03MaxTag = 9931;
const int plant03SlopeTag = 9932;

float plant01MaxRead, plant01SlopeRead, plant02MaxRead, plant02SlopeRead, plant03MaxRead, plant03SlopeRead;
int plant01MaxInt, plant01SlopeInt, plant02MaxInt, plant02SlopeInt, plant03MaxInt, plant03SlopeInt;
int i;
int hz, startTime;
float firstMax, plantReading, maxPlantReading, slope, plant01Slope, plant01Max;
float plantReadArray [20];

TCPClient TheClient;

Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);

//Adafruit_MQTT_Subscribe plantFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plantData");
Adafruit_MQTT_Subscribe plant01M = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plant01Max");
Adafruit_MQTT_Subscribe plant01S = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plant01Slope");
Adafruit_MQTT_Subscribe plant02M = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plant02Max");
Adafruit_MQTT_Subscribe plant02S = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plant02Slope");
// Adafruit_MQTT_Subscribe plant03M = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plant03Max");
// Adafruit_MQTT_Subscribe plant03S = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plant03Slope");

SYSTEM_MODE(AUTOMATIC);

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  //PinModes
  pinMode(PULSEPIN, OUTPUT);
  pinMode(PULSEREADPIN, INPUT);
  pinMode(PLANTREADPIN, INPUT);

  //initialize values
  hz = 500;
  startTime = millis();

  //Adafruit subscribe
  mqtt.subscribe(&plant01M);
  mqtt.subscribe(&plant01S);
  mqtt.subscribe(&plant02M);
  mqtt.subscribe(&plant02S);

  i = 21;
}

void loop() {

  MQTT_connect();
  MQTT_ping();

  // this is our 'wait for incoming subscription packets' busy subloop 
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {
    if (subscription == &plant01M) {
      plant01MaxRead = atof((char *)plant01M.lastread);
    }

    if (subscription == &plant01S) {
      plant01SlopeRead = atof((char *)plant01S.lastread);
    }

    if (subscription == &plant02M) {
      plant02MaxRead = atof((char *)plant02M.lastread);
    }

    if (subscription == &plant02S) {
      plant02SlopeRead = atof((char *)plant02S.lastread);
    }

  plant01SlopeInt = plant01SlopeRead *10;
  plant02SlopeInt = plant02SlopeRead *10;
  plant01MaxInt = plant01MaxRead;
  plant02MaxInt = plant02MaxRead;

  i = 0;
  hz = 500;
  }

   if(i < 20){
    plantReading = plantImpRead(hz, PULSEPIN, PULSEREADPIN, PLANTREADPIN);
    plantReadArray[i] = plantReading;
    
    i = i + 1;
    hz = hz + 500.0;
  }

  if(i == 20){
    plant03MaxInt = plantReadArray[0];
    plant03SlopeRead = ((plantReadArray[19] - plantReadArray[0])/(20.0));
    plant03SlopeInt = plant03SlopeRead * 10;
    
    Serial.printf("%i\n%i\n", plant01MaxTag, plant01MaxInt);
    Serial.printf("%i\n%i\n", plant01SlopeTag, plant01SlopeInt);
    Serial.printf("%i\n%i\n", plant02MaxTag, plant02MaxInt);
    Serial.printf("%i\n%i\n", plant02SlopeTag, plant02SlopeInt);
    Serial.printf("%i\n%i\n", plant03MaxTag, plant03MaxInt);
    Serial.printf("%i\n%i\n", plant03SlopeTag, plant03SlopeInt);

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

  //Serial.printf("max: %f\n", _max);

  return _max;

}
void MQTT_connect() {
  int8_t ret;
 
  // Return if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  //Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       //Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       //Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  //Serial.printf("MQTT Connected!\n");
}

bool MQTT_ping() {
  static unsigned int last;
  bool pingStatus;

  if ((millis()-last)>120000) {
      //Serial.printf("Pinging MQTT \n");
      pingStatus = mqtt.ping();
      if(!pingStatus) {
        //Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
  return pingStatus;
}
