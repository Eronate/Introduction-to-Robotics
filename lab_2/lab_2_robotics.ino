
//Pins for INPUT_PULLUP
const int floor1InputPin = 7,
          floor2InputPin = 4,
          floor3InputPin = 2;
//Pins for writing to LEDs
const int ledF1OutputPin = 9,
          ledF2OutputPin = 10,
          ledF3OutputPin = 11,
          flickingLedOutputPin = 5;
//Memory that holds what the red LED values should be for each floor
int ledStatefloor1 = HIGH, 
    ledStatefloor2 = LOW, 
    ledStatefloor3 = LOW;

int buttonStatefloor1 = 0, 
    buttonStatefloor2 = 0, 
    buttonStatefloor3 = 0;

int debounceDelay = 50, lastDebounceTime1 = 0, lastDebounceTime2= 0, lastDebounceTime3 = 0;
int currentFloor = 1;

//Auxiliary variables to help determine if a button was pressed or not
//------------------------------------------------------------
int currFloor1PressedState = 0, prevFloor1PressedState = 0,
    currFloor2PressedState = 0, prevFloor2PressedState = 0,
    currFloor3PressedState = 0, prevFloor3PressedState = 0;
int isBeingPulledUp1 = 0,
    isBeingPulledUp2 = 0,
    isBeingPulledUp3 = 0;
//------------------------------------------------------------
void setup() {
    Serial.begin(9600);
    //Setting the pins from which I will read the pressed state of the buttons as INPUT_PULLUP.
    pinMode(floor1InputPin, INPUT_PULLUP);
    pinMode(floor2InputPin, INPUT_PULLUP);
    pinMode(floor3InputPin, INPUT_PULLUP);
    //Pins that will feed current to the red leds
    pinMode(ledF1OutputPin, OUTPUT);
    pinMode(ledF2OutputPin, OUTPUT);
    pinMode(ledF2OutputPin, OUTPUT);
    pinMode(flickingLedOutputPin, OUTPUT);
}

int currentlyMoving = 0, destinationFloor= -1, timeBetweenFloors = 3000, isInBetweenFloors = 0, intervalForInbetweenFloors = 1500;
unsigned long flickingInterval = 500, startingTimeFlickingLed;
unsigned long startingTime;

void buttonPressHandler(int &currFloorPressedState, int &prevFloorPressedState, 
                        int &lastDebounceTime, int &buttonStatefloor, 
                        int &currentlyMoving, int &destinationFloor,
                        int floorToMoveTo)
{
   if(currFloorPressedState != prevFloorPressedState)
    {
    lastDebounceTime = millis();
    prevFloorPressedState = currFloorPressedState;
    }

  if((millis() - lastDebounceTime) > debounceDelay)
  {
    if(currFloorPressedState != buttonStatefloor)
      buttonStatefloor = currFloorPressedState;
    
    if(buttonStatefloor == HIGH && !currentlyMoving)
    {
      startingTimeFlickingLed = millis();
      startingTime = millis();
      currentlyMoving = 1;
      destinationFloor = floorToMoveTo;
    }
  }
}

void loop() {
  //Lighting up the LED's to match the current values in memory
  digitalWrite(ledF1OutputPin, ledStatefloor1);
  digitalWrite(ledF2OutputPin, ledStatefloor2);
  digitalWrite(ledF3OutputPin, ledStatefloor3);

  //Block to read which buttons were pressed last, negated because of how INPUT_PULLUP works internally
  currFloor1PressedState = !digitalRead(floor1InputPin);
  currFloor2PressedState = !digitalRead(floor2InputPin);
  currFloor3PressedState = !digitalRead(floor3InputPin);

  buttonPressHandler(currFloor1PressedState, prevFloor1PressedState, 
                    lastDebounceTime1, buttonStatefloor1, 
                    currentlyMoving, destinationFloor,
                    1);

  buttonPressHandler(currFloor2PressedState, prevFloor2PressedState, 
                    lastDebounceTime2, buttonStatefloor2, 
                    currentlyMoving, destinationFloor,
                    2);

  buttonPressHandler(currFloor3PressedState, prevFloor3PressedState, 
                    lastDebounceTime3, buttonStatefloor3, 
                    currentlyMoving, destinationFloor,
                    3);
  if(currentlyMoving && currentFloor != destinationFloor)
  {
    if(millis() - startingTime > intervalForInbetweenFloors)
      isInBetweenFloors = 1;
    if(millis() - startingTime > timeBetweenFloors)
      {
        isInBetweenFloors = 0;
        // if(currentFloor < destinationFloor)
        //     currentFloor = currentFloor + 1;
        // else if(currentFloor > destinationFloor)
        //   currentFloor = currentFloor - 1;
        currentFloor = currentFloor < destinationFloor ? (currentFloor + 1) : (currentFloor - 1);
        startingTime = millis();
      }
      
  }
  else if(currentFloor == destinationFloor)
    currentlyMoving = 0;
  if(isInBetweenFloors)
    ledStatefloor1 = ledStatefloor2 = ledStatefloor3 = LOW;
  else
  {
  ledStatefloor1 = currentFloor == 1 ? HIGH : LOW;
  ledStatefloor2 = currentFloor == 2 ? HIGH : LOW;
  ledStatefloor3 = currentFloor == 3 ? HIGH : LOW;
  }
  if(currentlyMoving)
  {
    if(millis() - startingTimeFlickingLed > flickingInterval)
      {
        startingTimeFlickingLed = millis();
        digitalWrite(flickingLedOutputPin, HIGH);
      }
    else
        digitalWrite(flickingLedOutputPin, LOW);
  }
  else
    digitalWrite(flickingLedOutputPin, HIGH);
  //Serial.println("floor:" +  String(currentFloor));
  //Serial.println(currentFloor);
  //Serial.println("destination:" + String(destinationFloor));
  //Serial.println(destinationFloor);
  // Serial.println("floor1" + String(ledStatefloor1));
  // Serial.println("floor2" + String(ledStatefloor2));
  // Serial.println("floor3" + String(ledStatefloor3));
  //Serial.println("currentFloor2pressedstate:" + String(currFloor2PressedState));
  //Serial.println("currentFloor2pressedstate:" + String(currFloor2PressedState));
  Serial.println("currentFloor:" + String(currentFloor));
  //Serial.println("starting time:" + String(startingTime));
}

















