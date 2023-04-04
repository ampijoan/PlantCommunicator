/*
 * Project ForestNet
 * Description: A plant network communication system
 * Author: Adrian Pijoan
 * Date: 29-MAR-2023
 */

const int PULSEPIN = A1;
const int PLANTREADPIN = A2;
int i = 0;
int hz, startTime;
float plantReading;
float plantReadingArray [39];

void setup() {

  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  pinMode(PULSEPIN, OUTPUT);
  pinMode(PLANTREADPIN, INPUT);

  hz = 500; //initialize wave frequency at 500hz

}

void loop() {

  /* Test code to measure plant impedence and see the range of values.
  Measures from 500-10000hz in intervals of 250 hz once a second */
  if(startTime - millis() >= 1000){

    if(hz <= 10000){
      analogWrite(PULSEPIN, 127, hz);   //syntax for setting frequency of PWM pin -- analogWrite(pin, value, frequency);

      plantReading = analogRead(PLANTREADPIN);

      plantReadingArray[i] = plantReading;
      
      Serial.printf("Reading from plant: %.4f at %i hz\n\n", plantReading, hz);

      hz = hz + 250; //increase by 250 hz

      i++;

    }

    else{
      printf("Done\n");
    }

  }

}