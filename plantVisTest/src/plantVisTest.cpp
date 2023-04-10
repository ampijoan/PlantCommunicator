/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/plantVisTest/src/plantVisTest.ino"
/*
 * Project PlantVisTest
 * Description: testing sending a changing variable to processing from argon
 * Author: Adrian Pijoan
 * Date: 05-APR-2023
 */

void setup();
void loop();
#line 8 "/Users/adrianpijoan/Documents/IoT/PlantCommunicator/plantVisTest/src/plantVisTest.ino"
int height;
int length;
int startX;
int startY;
int color;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);


}

void loop() {
  startX = random(0,640);
  startY = random(0,640);
  height = random(-100,100);
  length = random(-100,100);
  color = random(0,3);

  Serial.printf("%i\n%i\n%i\n%i\n%i\n", height, length, startX, startY, color);

}