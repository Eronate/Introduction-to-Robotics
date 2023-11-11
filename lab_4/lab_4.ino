const int buttonStartPausePin = 13, buttonResetPin = 8, buttonLapPin = 2;

//Holds the value of whether the buttons are clicked or not
int stateButtonStart=0, stateButtonReset=0, stateButtonLap=0, currentNumber,
    lastStateButtonStart =0, lastStateButtonReset=0, lastStateButtonLap=0;

const int latchPin = 11, // Pin for latching data into the register
          clockPin = 10, // Pin for the shift register clock
          dataPin = 12; // Pin for the serial data input to the shift register

unsigned long currentMillis, //holds millis() program value
              interval = 100, 
              previousMillis = 0, //needed for updating the integer value that represents the current timer.
              debounceInterval = 50, 
              intervalForFlag = 1500, //interval for cycling between laps when the button is being held down.
              lastClickedTime =0,
              startClickStartButton, startClickResetButton, startClickFlagButton; //the exact moment the buttons get pressed, for debouncing logic

//Booleans for click handling logic
bool hasHandledClickStart=0, hasHandledClickReset =0, hasHandledClickFlag = 0, 
     hasHandledClickHoldDownFlag = 0; //this boolean determines when an action should be dispatched when the stopwatch is in the Lap viewing state.
const byte regSize = 8;
const int encodingsNumber = 10, 
          displayCount = 4,
          segD1 = 4, // Control pin for the first digit
          segD2 = 5, // Control pin for the second digit
          segD3 = 6, // Control pin for the third digit
          segD4 = 7; // Control pin for the fourth digit

int displayDigits[] = {
  segD1, segD2, segD3, segD4
};

//Stopwatch states: counting, paused and lap viewing
#define COUNTING 0
#define PAUSED 1
#define LAP_VIEWING 2

byte encodingArray[encodingsNumber][regSize] = {
// A  B  C  D  E  F  G  DP
  {1, 1, 1, 1, 1, 1, 0, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1, 0}, // 2
  {1, 1, 1, 1, 0, 0, 1, 0}, // 3
  {0, 1, 1, 0, 0, 1, 1, 0}, // 4
  {1, 0, 1, 1, 0, 1, 1, 0}, // 5
  {1, 0, 1, 1, 1, 1, 1, 0}, // 6
  {1, 1, 1, 0, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1, 0}, // 8
  {1, 1, 1, 1, 0, 1, 1, 0}, // 9
};

const int lapCount =4;
int lapsInMemory = -1, 
    lapsIndex = -1, //index for when the lap needs to be saved in memory
    lapViewingIndex = -1;  //index for displaying a lap on the stopwatch
int laps[lapCount]; 

//the game starts in paused state because it needs to be started to 'start counting'
int currentCircuitState = PAUSED;

//Needed to reduce ghosting for the digit display
void TurnOffAllDisplays()
{
  for (int i = 0; i < displayCount; i++)  
    digitalWrite(displayDigits[i], HIGH);
}

//Function that gets a number and displays the number on the screen.
void DisplayNumber(int number)
{
  int displayIndex = 3;
  while(displayIndex >= 0)
  {
    TurnOffAllDisplays();
    //Feed the digits to the individual display function
    WriteDigitToDisplayWithIndex(number % 10, displayIndex);
    number /= 10;
    displayIndex--;
  }
}

void WriteDigitToDisplayWithIndex(int digit, int displayIndex)
{
  //Send the digit's encoded value to be written on the shift register
  if(displayIndex == 2)
    encodingArray[digit][7] = 1;
  WriteReg(encodingArray[digit]);
  //Only light up the display that we're trying to write the digit to
  for (int i = 0; i < displayCount; i++) 
    displayIndex == i ? digitalWrite(displayDigits[i], LOW) : digitalWrite(displayDigits[i], HIGH);
  encodingArray[digit][7] = 0;
}

//Function that writes the 8 bit encoding onto the shift register
void WriteReg(byte encoding[]) {
// Send each bit of the encoding to the shift register
  digitalWrite(latchPin, LOW); // Prepare to send data
  for (int i = 0; i < regSize; i++) 
  {
    digitalWrite(clockPin, LOW); // Set the clock pin low before sending data
    digitalWrite(dataPin, encoding[i]); // Send the data bit
    digitalWrite(clockPin, HIGH); // Clock the data bit into the register
  }
  digitalWrite(latchPin, HIGH); 
}

//Deletes the laps in memory
void ResetLaps()
{
  lapsInMemory = -1;
  lapsIndex = -1;
  for(int i=0 ;i < lapCount; i++)
    laps[i] = 0;
  return;
}

//function to detect when a button press actually occurred
bool Debouncer(int buttonState, int lastButtonState, unsigned long &startClickButton, bool &hasHandledClick)
{
  if(buttonState == 1 && lastButtonState == 0)
    {
      startClickButton = millis();
      hasHandledClick = 0;
    }
  else if (buttonState == 1 && millis() - startClickButton > debounceInterval && hasHandledClick == 0)
   return 1;
  else return 0;
}

//Needs separate logic because it can be held down and have a different functionality for both clicking and being held down
bool DebouncerFlag(int buttonState, int lastButtonState, unsigned long &startClickButton, unsigned long &lastClickedTime, bool &hasHandledClick)
{
  //lastClickedTime gets initialized here because when it actually gets into the 'if'
  //that debounces, it will have the right value
  if(buttonState == 1 && lastButtonState == 0)
    {
      startClickButton = millis();
      hasHandledClick = 0;
      lastClickedTime = millis();
    }
  else if (buttonState == 1 && millis() - startClickButton > debounceInterval)
   {
    //if intervalForFlag seconds pass, the click returns to the state of being 'not handled'.
    //this is for the functionality of being held down.
    if(millis() - lastClickedTime > intervalForFlag)
    {
      hasHandledClick = 0;
      lastClickedTime = millis();
      return 1;
    }
    //same logic as the other debouncers, only return true if the click hasn't been handled yet.
    if(hasHandledClick == 0)
      return 1;
    else return 0;
   }
  else return 0;
}

void HandleStart(int buttonState, int lastButtonState)
{
  if(Debouncer(buttonState, lastButtonState, startClickStartButton, hasHandledClickStart) == 1)
    {
    hasHandledClickStart = 1;
    switch(currentCircuitState){
      case LAP_VIEWING:
        //Laps in memory is -1 when no lap has been saved in memory. This is supposed to not block the circuit into 'lap viewing' when you press lap and the 
        //circuit has no laps in memory, so you see '0' and can't press start.
        if(lapsInMemory == -1)
        {
          currentNumber = 0;
          currentCircuitState = COUNTING;
        }
        break;
      case COUNTING:
        currentCircuitState = PAUSED;
        break;
      case PAUSED:
        currentCircuitState = COUNTING;
        break;
      default:
        break;
      }
    }
}

void HandleReset(int buttonState, int lastButtonState)
{
  if(Debouncer(buttonState, lastButtonState, startClickResetButton, hasHandledClickReset) == 1)
    {
      hasHandledClickReset = 1;
      switch(currentCircuitState){
        case LAP_VIEWING:
          {
            ResetLaps(); //Empty lap array
            lapViewingIndex = -1; //Reset variables used for lap logic to their initial state
            currentNumber = 0;
            currentCircuitState = PAUSED;
            break;
          }
        case COUNTING:
            break;
        case PAUSED:
          {
            //currentNumber = 0;
            lapViewingIndex = -1;
            currentCircuitState = LAP_VIEWING;
            break;
          }
        default:
          break;
      }
    }
}

void HandleLap(int buttonState, int lastButtonState)
{
    switch(currentCircuitState)
      {
        //separate button press handling for what state the circuit is in when flag button is pressed 
        case LAP_VIEWING:
          {
            if(DebouncerFlag(buttonState, lastButtonState, startClickFlagButton, lastClickedTime, hasHandledClickHoldDownFlag) == 1)
            {
              hasHandledClickHoldDownFlag = 1;
              //cycling through lap indexes by pressing the flag button. Some logic to make sure it doesn't go out of bounds.
              lapViewingIndex = lapViewingIndex == lapsInMemory ? 0 : min(lapViewingIndex + 1, lapsInMemory);
            }
            break;
          }
        case COUNTING:
          {
            if(Debouncer(buttonState, lastButtonState, startClickFlagButton, hasHandledClickFlag) == 1)
            {
              hasHandledClickFlag = 1;
              lapsInMemory = min(3, lapsInMemory + 1);
              //due to only having access to 4 saved laps.
              lapsIndex = lapsIndex == 3 ? 0 : lapsIndex + 1;
              laps[lapsIndex] = currentNumber;
            }
            break;
          }
        case PAUSED:
          break;
        default:
          break;
      }
}

void HandleCurrentCircuitState()
{
  switch(currentCircuitState)
  {
    case LAP_VIEWING:
    {
      //Display 0 at first when in lap viewing mode, as an intermediary 'Has Just Reset' state
      if(lapViewingIndex == -1)
        currentNumber = 0;
      else
        //Otherise, display the current lap
        currentNumber = laps[lapViewingIndex];
      break;
    }
    case COUNTING:
    {
      //Updates currentNumber at 100ms intervals
      currentMillis = millis();
      if (currentMillis - previousMillis >= interval) 
      {
        previousMillis = currentMillis;
        currentNumber += 1; // Increment the tenths of a second
      }
     break;
    }  
    case PAUSED:     
      break;
    default:
      break;
  }
}

void setup() { 
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(buttonStartPausePin, INPUT_PULLUP);
  pinMode(buttonResetPin, INPUT_PULLUP);
  pinMode(buttonLapPin, INPUT_PULLUP);

  // Initialize digit control pins and set them to ON, they are active low
  for (int i = 0; i < displayCount; i++) 
  {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  //Starts counting from 0
  currentNumber = 0;
}

void loop() {
  //Reading button states, negated because of how input pullup works internally
  stateButtonStart = !digitalRead(buttonStartPausePin);
  stateButtonReset = !digitalRead(buttonResetPin);
  stateButtonLap = !digitalRead(buttonLapPin);

  HandleStart(stateButtonStart, lastStateButtonStart); //Handles start button state
  HandleReset(stateButtonReset, lastStateButtonReset); //Handles reset button state
  HandleLap(stateButtonLap, lastStateButtonLap); //Handles lap button state

  lastStateButtonStart = stateButtonStart;
  lastStateButtonReset = stateButtonReset;
  lastStateButtonLap = stateButtonLap;

  HandleCurrentCircuitState();
  DisplayNumber(currentNumber);
}
