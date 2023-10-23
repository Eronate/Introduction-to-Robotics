int analogInput1 = A0,
    analogInput2 = A1,
    analogInput3 = A2;
    
int pinOutputRed = 9,
    pinOutputGreen = 10,
    pinOutputBlue = 11;
int minPotenVal = 0, 
    maxPotenVal = 1023, 
    minValInt = 0, 
    maxValInt = 255;
int valPoten1, valMapped1,
    valPoten2, valMapped2,
    valPoten3, valMapped3;

void setup() {
    pinMode(pinOutputRed, OUTPUT);
    pinMode(pinOutputGreen, OUTPUT);
    pinMode(pinOutputBlue, OUTPUT);
}

void loop() {
  valPoten1 = analogRead(analogInput1);
  valPoten2 = analogRead(analogInput2);
  valPoten3 = analogRead(analogInput3);

  valMapped1 = map(valPoten1, minPotenVal, maxPotenVal, minValInt, maxValInt);
  valMapped2 = map(valPoten2, minPotenVal, maxPotenVal, minValInt, maxValInt);
  valMapped3 = map(valPoten3, minPotenVal, maxPotenVal, minValInt, maxValInt);

  analogWrite(pinOutputRed, valMapped1);
  analogWrite(pinOutputGreen, valMapped2);
  analogWrite(pinOutputBlue, valMapped3);
}
