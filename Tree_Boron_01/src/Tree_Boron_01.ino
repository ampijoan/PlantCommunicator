/*
 * Project ForestNet
 * Description: A plant network communication system
 * Author: Adrian Pijoan
 * Date: 10-APR-2023
 */

#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"

#include <credentials.h> // Radio address, network, name, and password stored here

TCPClient TheClient;

Adafruit_MQTT_SPARK mqtt(&TheClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish plant01M = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/plant01Max");
Adafruit_MQTT_Publish plant01S = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/plant01Slope");
Adafruit_MQTT_Publish plant02M = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/plant02Max");
Adafruit_MQTT_Publish plant02S = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/plant02Slope");
// Adafruit_MQTT_Publish plant03M = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/plant03Max");
// Adafruit_MQTT_Publish plant03S = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/plant03Slope");

int SENDADDRESS;   // address of radio to be sent to. Set depending on where you want data to go.

float plant01Max, plant01Slope, plant02Max, plant02Slope;
int i;


SYSTEM_MODE(AUTOMATIC);

void setup() {

  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  Serial1.begin(115200);
  reyaxSetup(password);

  SENDADDRESS = 0;
  i = 0;

}

void loop() {

  if (Serial1.available())  { // full incoming buffer: +RCV=203,50,35.08,9,-36,41
    String parse0 = Serial1.readStringUntil('=');  //+RCV
    String parse1 = Serial1.readStringUntil(',');  // address received from
    String parse2 = Serial1.readStringUntil(',');  // buffer length
    String parse3 = Serial1.readStringUntil(',');  // Plant01Max
    String parse4 = Serial1.readStringUntil(',');  // Plant01Slope
    String parse5 = Serial1.readStringUntil(',');  // Plant02Max
    String parse6 = Serial1.readStringUntil(',');  // Plant02Slope
    String parse7 = Serial1.readStringUntil(',');  // rssi
    String parse8 = Serial1.readStringUntil('\n'); // snr
    //String parse9 = Serial1.readString();          // extra

    Serial.printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", parse0.c_str(), parse1.c_str(), parse2.c_str(), parse3.c_str(), parse4.c_str(), parse5.c_str(), parse6.c_str(), parse7.c_str(), parse8.c_str());
    plant01Max = strtof(parse3.c_str(),NULL);
    plant01Slope = strtof(parse4.c_str(),NULL);
    plant02Max = strtof(parse5.c_str(),NULL);
    plant02Slope = strtof(parse6.c_str(),NULL);

    Serial.printf("plant 01 max: %f\nplant 01 slope: %f\nplant 02 max: %f\nplant 02 slope: %f\n", plant01Max, plant01Slope, plant02Max, plant02Slope);

    i = 1; 

  }

  MQTT_connect();
  MQTT_ping();

  if(i == 1){

    //send argon #, slope, and Max value
    plant01M.publish(plant01Max);
    delay(50);
    plant01S.publish(plant01Slope);
    delay(50);
    plant02M.publish(plant02Max);
    delay(50);
    plant02S.publish(plant02Slope);
    delay(50);

    i = 0;

  }


}


// Function to connect and reconnect as necessary to the MQTT server.
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

//Send data with LoRa module
void sendData(String name, int plantReading) {
  char buffer[60];
  sprintf(buffer, "AT+SEND=%i,60,%.2f,%.2f,%s\r\n", SENDADDRESS, plantReading, name.c_str());
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

//Sleep until Boron receives data over Serial1 (LoRa)
// void sleepULP(){
//   SystemSleepConfiguration config;
//   config.mode(SystemSleepMode::ULTRA_LOW_POWER).usart(Serial1);
//   SystemSleepResult result = System.sleep(config);

// }