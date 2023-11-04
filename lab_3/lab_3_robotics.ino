const int dpPin = 4, aPin = 12 , bPin = 10, cPin=9, dPin = 8, ePin = 7, fPin = 6, gPin = 5, clickPin = 13;
int xJoyPin = A0, yJoyPin= A1;
const int pins[] = {aPin, bPin, cPin, dPin, ePin, fPin, gPin, dpPin};
bool currConfig[] = {  0,     0,   0,     0,    0,    0,    0,    0};
bool currPressedState[] = {0,0,0,0,0,0,0,0};
int currentPosition = 0, lastPosition =0;
const int lengthPins = sizeof(pins) / sizeof(pins[0]);

const int minThreshold = 350, maxThreshold = 700; 
unsigned long startTimeFlicker=0, flickerInterval = 200, clickDebouncer = 50, startTimeClick = 0, clickResetTimer = 3000;
int xValue, yValue;
int row, initialRow;
int hasHandledMove, hasHandledClick, movedInDirectionBool, directionThatNeedsToReset, hasHandledReset;

//For handling clicks, whatToDoWithClicking: 0 - do nothing, 1 - light up led, 2 - reset
int lastClickState, whatToDoWithClicking, clickState = 0;
int ledMap[][4] = 
{// UP, DOWN, LEFT, RIGHT
  //0
  {-1,   6,   5,     1}, //a
  //1
  {0,    6,   5,    -1}, //b
  //2
  {6,    3,   4,     7}, //c
  //3
  {6,   -1,   4,     2}, //d
  //4
  {6,    3,   -1,    2}, //e
  //5  
  {0,    6,   -1,    1}, //f
  //6
  {0,    3,   -1,   -1}, //g
  //7
  {-1,  -1,   2,    -1} //dp
};

int checkIfOutOfBounds(int row, int col)
{
  return ledMap[row][col];
}

void moveHandler(int axisValue, int direction)
{
  //If the value exceeds the threshold where we can consider it to 'have moved',
  //we mantain the moved state and make sure it only triggers an action once to give the toggle effect
  if(axisValue > maxThreshold || axisValue < minThreshold)
    movedInDirectionBool = 1;

  if(axisValue > maxThreshold)
    {
      //if it's moved in a direction, we mantain state until joystick returns to original position
      if(movedInDirectionBool == 1)
      //if the move hasn't been handled, we handle it once and set the operation as handled to give toggle effect
        if(hasHandledMove == 0)
          {
            //Handling matrix navigation through joystick input.
            if(direction == 0)
              {
                if(checkIfOutOfBounds(row, 3) != -1)
                  {
                    currentPosition = ledMap[row][3];
                    row = currentPosition;
                  }
              }
            else 
              {
                if(checkIfOutOfBounds(row, 0) != -1)
                  {
                    currentPosition = ledMap[row][0];
                    row = currentPosition;
                  }
              }
            hasHandledMove = 1;
            directionThatNeedsToReset = direction;
          }
    }

  else if(axisValue < minThreshold)
    {
      if(movedInDirectionBool == 1)
        if(hasHandledMove == 0)
          {
            if(direction == 0)
              {
                if(checkIfOutOfBounds(row, 2) != -1)
                  {
                    currentPosition = ledMap[row][2];
                    row = currentPosition;
                  }
              }
            else 
              {
                if(checkIfOutOfBounds(row, 1) != -1)
                  {
                    currentPosition = ledMap[row][1];
                    row = currentPosition;
                  }
              }
            hasHandledMove = 1;
            directionThatNeedsToReset = direction;
          }
    }
  //Only the current direction that triggered the action can reset the states by going back to original position
  else if(directionThatNeedsToReset == direction)
  {
    movedInDirectionBool = 0;
    hasHandledMove = 0;
  }
}

void clickHandler(){
  if(lastClickState == 0 && clickState == 1)
    {
      startTimeClick = millis();
      hasHandledReset = 0;
      hasHandledClick = 0;
    }
  if(clickState == 1)
    {
    if(millis() - startTimeClick > clickDebouncer && millis() - startTimeClick < clickResetTimer && hasHandledClick == 0)
        whatToDoWithClicking = 1;
    else if(millis() - startTimeClick> clickResetTimer && hasHandledReset == 0)
        whatToDoWithClicking = 2;
    }
  lastClickState = clickState;
}

void setup() {
  Serial.begin(9600);
  initialRow = 7;
  row = 7;
  whatToDoWithClicking = 0;
  for(int i = 0; i < lengthPins; i++)
    pinMode(pins[i], OUTPUT);
  pinMode(xJoyPin, INPUT);
  pinMode(yJoyPin, INPUT);
  pinMode(clickPin, INPUT);

  hasHandledMove=0;
  lastClickState = 0;
  clickState = 0;
  hasHandledClick=0;
  hasHandledReset = 0;
  movedInDirectionBool=0;
  directionThatNeedsToReset = -1;
  currentPosition = 7;
}

void loop() {
  //Read x and y from joystick input
  xValue = analogRead(xJoyPin);
  yValue = analogRead(yJoyPin);
  clickState = !digitalRead(clickPin); 
  //Move handling
  moveHandler(xValue, 1);
  moveHandler(yValue, 0);
  //Click handling
  clickHandler();

  // //Before re-loading the 'Pressed' configuration into the, we make sure that the lastPosition isn't lit by default anymore
  // //order is important because otherwise it wouldn't get lit anymore
  if(lastPosition != currentPosition)
    currConfig[lastPosition] = 0;

  if(whatToDoWithClicking == 1 && hasHandledClick == 0)
    {
      currPressedState[currentPosition] = !currPressedState[currentPosition];
      hasHandledClick = 1;
      whatToDoWithClicking =0;
    }

  else if(whatToDoWithClicking == 2 && hasHandledReset == 0)
    {
      for(int i = 0; i<lengthPins; i++)
        currPressedState[i] = 0;
      currentPosition = 7;
      lastPosition = currentPosition;
      currConfig[currentPosition] = 1;
      row = 7;

      hasHandledReset = 1;
      whatToDoWithClicking=0;
    }
    //Loading the 'Pressed' config into the currentConfig. current position mantains a flickering state regardless of pressed state
  for(int i = 0; i<lengthPins; i++)
    currConfig[i] = i == currentPosition ? currConfig[i] : currPressedState[i];

  //Flickering the current position
  if(millis() - startTimeFlicker > flickerInterval)
  {
    currConfig[currentPosition] = !currConfig[currentPosition];
    startTimeFlicker = millis();
  }
  //Lighting up the display according to the current configuration
  for(int i=0; i < lengthPins; i++)
  {
    if (currConfig[i] == 1) 
      digitalWrite(pins[i], HIGH);
    else 
      digitalWrite(pins[i], LOW);
  }

  //Serial.println("row: " +String(row) + "column: " + String(column));
  Serial.println(clickState);
  // Serial.print("curr config:");
  // for(int i=0; i < lengthPins; i++)
  //   Serial.print(String(currConfig[i]) + " ");
  // Serial.println();
  lastPosition = currentPosition;
}