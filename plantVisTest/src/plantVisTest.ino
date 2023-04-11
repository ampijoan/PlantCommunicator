/*
 * Project PlantVisTest
 * Description: testing sending a changing variable to processing from argon
 * Author: Adrian Pijoan
 * Date: 05-APR-2023
 */

#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"

#include "credentials.h"

TCPClient TheClient;

Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);

Adafruit_MQTT_Subscribe plantFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/plantData");

int plantNum;
int height;
int length;
int startX;
int startY;
int color;

SYSTEM_MODE(AUTOMATIC);

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);


  mqtt.subscribe(&plantFeed);

}

void loop() {

  MQTT_connect();
  MQTT_ping();

  // this is our 'wait for incoming subscription packets' busy subloop 
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {
    if (subscription == &plantFeed) {
      plantNum = atoi((char *)plantFeed.lastread);
      height = plantNum;
      Serial.printf("%i\n", height);
    }

  }





  // startX = random(0,640);
  // startY = random(0,640);
  // height = random(-100,100);
  // length = random(-100,100);
  // color = random(0,3);

  // Serial.printf("%i\n%i\n%i\n%i\n%i\n", height, length, startX, startY, color);



}

void MQTT_connect() {
  int8_t ret;
 
  // Return if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
}

bool MQTT_ping() {
  static unsigned int last;
  bool pingStatus;

  if ((millis()-last)>120000) {
      Serial.printf("Pinging MQTT \n");
      pingStatus = mqtt.ping();
      if(!pingStatus) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
  return pingStatus;
}
