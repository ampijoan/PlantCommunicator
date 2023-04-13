/*
 * Project Imp_Measure_Serial
 * Description: Final plant impedence measurement test code
 * Author: Adrian Pijoan
 * Date: 12-APR-2023
 */

const int PULSEPIN = A1;
const int PULSEREADPIN = A2;
const int PLANTREADPIN = A3;
int i, j, startTime;
float hz;
float plantReading, firstMax;
float plantReadArray [20][2];

bool logStart;
const int STARTPIN = D7;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 10000);

  pinMode(STARTPIN, INPUT);
  pinMode(PULSEPIN, OUTPUT);
  pinMode(PLANTREADPIN, INPUT);

  hz = 500.0;                                                     //initialize square wave frequency at 500hz

  startTime = millis();
}

void loop() {

  if(i < 20){
    plantReadArray[i][0] = hz;
    plantReadArray[i][1] = plantImpRead(hz, i, PULSEPIN, PULSEREADPIN, PLANTREADPIN);
    
    i = i + 1;
    hz = hz + 500.0;
  }

  if(i == 20){
    j = 0;
    while(j < 20){
      hz = plantReadArray[j][0];
      plantReading = plantReadArray[j][1];

      Serial.printf("%f\n", plantReading);
      j = j + 1;
    }

    i = 0;
    hz = 500.0;
  }

  delay(2000);

}

//Read impedence values from the plant at current frequency for 1 second
float plantImpRead(float _hz, int _i, int _PULSEPIN, int _PULSEREADPIN, int _PLANTREADPIN){
  float j;
  int _startTime;
  float _plantReading, _pulseReading, _plantImp, _avgPlantImp, _totalPlantImp, _min, _max;
  static float _maxAt500, _minAt500;
  
  _startTime = millis();
  _totalPlantImp = 0.0;
  j = 0.0;
  _min = 4096;
  _max = 0;

  while(millis() - _startTime <= 1000){
    analogWrite(_PULSEPIN, 127, _hz);
    _pulseReading = analogRead(_PULSEREADPIN);
    _plantReading = analogRead(_PLANTREADPIN);

    _plantImp = _plantReading / _pulseReading;

    if(_plantReading < _min){
      _min = plantReading;
    }

    if(_plantReading > _max){
      _max = _plantReading;
    }

    j = j + 1.0;
    //_totalPlantImp = (_totalPlantImp + _plantImp);
    
  }

  //this statement lets us normalize the data for EZ plotting in excel
  if(_hz == 500.0){
    _maxAt500 = _max; 
    _minAt500 = _min;
  }

  Serial.printf("first max: %f\nnew max: %f\n", _maxAt500, _max);

  _max = (_max / _maxAt500);
  //_min = (_min / _minAt500 );
  //_avgPlantImp = _max - _min;

  //_avgPlantImp = (_totalPlantImp / j);
  Serial.printf("hz: %f\n", _hz);
  Serial.printf("max: %f\n\n", _max);
  Serial.printf("min: %f\n", _min);
  

  return _max;

}