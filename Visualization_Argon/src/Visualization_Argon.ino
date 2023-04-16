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
const int plant01MaxTag = 9911;
const int plant01SlopeTag = 9912;
const int plant02MaxTag = 9921;
const int plant02SlopeTag = 9922;

float plant01MaxRead, plant01SlopeRead, plant02MaxRead, plant02SlopeRead, plant03MaxRead, plant03SlopeRead;

TCPClient TheClient;

Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);

//Adafruit_MQTT_Subscribe plantFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plantData");
Adafruit_MQTT_Subscribe plant01M = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plant01Max");
Adafruit_MQTT_Subscribe plant01S = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plant01Slope");
Adafruit_MQTT_Subscribe plant02M = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plant02Max");
Adafruit_MQTT_Subscribe plant02S = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plant02Slope");
Adafruit_MQTT_Subscribe plant03M = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plant03Max");
Adafruit_MQTT_Subscribe plant03S = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plant03Slope");


SYSTEM_MODE(AUTOMATIC);

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);


  mqtt.subscribe(&plant01M);
  mqtt.subscribe(&plant01S);
  mqtt.subscribe(&plant02M);
  mqtt.subscribe(&plant02S);
  mqtt.subscribe(&plant03M);
  mqtt.subscribe(&plant01S);
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

    if (subscription == &plant03M) {
      plant03MaxRead = atof((char *)plant03M.lastread);
    }

    if (subscription == &plant03S) {
      plant03SlopeRead = atof((char *)plant03S.lastread);
    }

  }

  //Maybe instead of delay, have it continuously print each set for a second, so if processing misses it the first time it's ok?
  //could send a STOP code at the end??? OR just make Processing break out after second parse?
  Serial.printf("%i\n%f\n", plant01Tag, plant01MaxRead);
  Serial.printf("%i\n%f\n", plant01Tag, plant01SlopeRead);
  Serial.printf("%i\n%f\n", plant02Tag, plant02MaxRead);
  Serial.printf("%i\n%f\n", plant02Tag, plant02SlopeRead);
  Serial.printf("%i\n%f\n", plant03Tag, plant03MaxRead);
  Serial.printf("%i\n%f\n", plant03Tag, plant03SlopeRead);




}

void MQTT_connect() {
  int8_t ret;
 
  // Return if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       //Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       //Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
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
