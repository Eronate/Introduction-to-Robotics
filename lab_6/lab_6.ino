#include "LedControl.h" // need the library

#define destroyableWallsTimer 2500
#define bombBlinkingTimer 100
#define playerTimer 250
#define wallPercentage 70
#define moveInterval 500
#define bombDetonationTimer 3500

unsigned long destroyableWallsStart=0, bombBlinkingStart=0, playerTimerStart=0,
              moveStart = 0, bombInputStart=0, bombTimerDetonateStart = 0;
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
const byte horizontalJoystickPin = A1;
const byte verticalJoystickPin = A0;
const byte buttonBombPin = 4;
int horizontalInput, verticalInput, bombInput;
int bombPosRow=-1, bombPosCol=-1; //marked as -1 to mark it doesn't exist yet
bool hasHandledButtonClick = 0;
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER
byte matrixBrightness = 2;

int playerPosRow = 7, playerPosCol = 0;
bool hasMoved = 0;
byte matrix[matrixSize][matrixSize] = {
  {1, 1, 1, 1, 1, 1, 0, 1},
  {0, 0, 0, 0, 0, 0, 0, 1},
  {0, 1, 0, 1, 0, 1, 0, 1},
  {0, 0, 0, 0, 0, 0, 0, 1},
  {0, 1, 0, 1, 0, 1, 0, 1},
  {0, 0, 0, 0, 0, 0, 0, 1},
  {0, 1, 0, 1, 0, 1, 0, 1},
  {0, 0, 0, 0, 0, 0, 0, 1}
};

byte matrixForBlinking[matrixSize][matrixSize] = {
  {1, 1, 1, 1, 1, 1, 0, 1},
  {0, 0, 0, 0, 0, 0, 0, 1},
  {0, 1, 0, 1, 0, 1, 0, 1},
  {0, 0, 0, 0, 0, 0, 0, 1},
  {0, 1, 0, 1, 0, 1, 0, 1},
  {0, 0, 0, 0, 0, 0, 0, 1},
  {0, 1, 0, 1, 0, 1, 0, 1},
  {0, 0, 0, 0, 0, 0, 0, 1}
};



//Care not to set percentToFill too high, the do{...} while(...) can block the whole program. 
//TO EVENTUALLY IMPLEMENT a timer to break out of the whileafter X seconds maybe
void GenerateTerrain(int rowSize, int colSize, int percentToFill)
{
  int wallsCount = 24;
  float wallsDesired = rowSize * colSize * percentToFill / 100;
  //Pack 1 of destroyable wall combinations
  int freeRows[] = {1, 3, 5, 7},
      freeRowsAssociatedColmumns[] = {1, 3, 5, 7};

  //Pack 2 of destroyable wall combinations
  int freeColumnsAssociatedRows[] = {0, 2, 4, 6},
      freeColumns[] = {0, 2, 4, 6};

  int flickWhichPackToChoose, randomColumn, randomRow, valueRandom;

  do
  {
    flickWhichPackToChoose = random(0, 2);
    //free rows pack
    if(flickWhichPackToChoose == 0)
    {
      valueRandom = random(0,4); //3 is the size of freeRows, currently generating a random index 
      randomRow = freeRows[valueRandom];
      valueRandom = random(0,4); //3 is the size of freeRowsAssociatedColmumns, currently generating a random index 
      randomColumn = freeRowsAssociatedColmumns[valueRandom];
    }
    //free columns pack
    else 
    {
      valueRandom = random(0,4); //3 is the size of freeColumnsAssociatedRows, currently generating a random index 
      randomRow = freeColumnsAssociatedRows[valueRandom];
      valueRandom = int(random(0,4)); //3 is the size of freeColumns, currently generating a random index 
      randomColumn = freeColumns[valueRandom];
    }
    //The following lines are made to prevent "boxing in" the player at the start, leaving him no choice but to bomb himself
    if(matrix[randomRow][randomColumn] == 0)
    {
      if(randomRow == 7 && randomColumn == 1)
      {
        if(matrix[6][0] != 2)
        {
          matrix[randomRow][randomColumn] = 2;
          wallsCount++;
        }
      }
      else if(randomRow == 6 && randomColumn == 0)
      {
        if(matrix[7][1] != 2)
        {
          matrix[randomRow][randomColumn] = 2;
          wallsCount++;
        }
      }
      else
      {
        matrix[randomRow][randomColumn] = 2;
        wallsCount++;
      }
      //Serial.println(wallsCount);
    }
  }
  while(wallsCount < wallsDesired); //while the count of ones is less than % of the matrix, generate pieces of terrain

  //Copy the final matrix configuration into the aux matrix for blinking handling
  for (int row = 0; row < matrixSize; row++) 
    {
      for (int col = 0; col < matrixSize; col++) 
        matrixForBlinking[row][col] = matrix[row][col];
    }
  return;
}

void BlinkMatrixOnTimers()
{
/*
Timers:
  destroyableWallsTimer 1000
  bombTimer 300
  playerTimer 500
*/
  if(millis() - destroyableWallsStart > destroyableWallsTimer)
  {
    //2 means destroyable wall
    for (int row = 0; row < matrixSize; row++) 
      for (int col = 0; col < matrixSize; col++) 
        //Keep in mind, when bombs destroy walls, matrix will have to be updated too, it serves as a reference point for matrixForBlinking to reset its values to
        {
          if(matrix[row][col] == 2)
          //blinking based on curent on/off state
            {
              if(matrixForBlinking[row][col] == 2)
                matrixForBlinking[row][col] = 0;
            }
        }
    //destroyableWallsStart = millis();
  }
  if(millis() - destroyableWallsStart > destroyableWallsTimer + 225)
  {
    //2 means destroyable wall
    for (int row = 0; row < matrixSize; row++) 
      for (int col = 0; col < matrixSize; col++) 
        //Keep in mind, when bombs destroy walls, matrix will have to be updated too, it serves as a reference point for matrixForBlinking to reset its values to
        {
          if(matrix[row][col] == 2)
          //blinking based on curent on/off state
            {
              if(matrixForBlinking[row][col] == 0)
                matrixForBlinking[row][col] = 2;
            }
        }
    destroyableWallsStart = millis();
  }
  if(millis() - playerTimerStart > playerTimer)
  {
    playerTimerStart = millis();
    matrixForBlinking[playerPosRow][playerPosCol] = matrixForBlinking[playerPosRow][playerPosCol] == 0 ? 3 : 0; 
  }
  if(bombPosRow >= 0 && bombPosCol >= 0)
    {
      if(millis() - bombBlinkingStart > bombBlinkingTimer)
        {
          bombBlinkingStart = millis();
          matrixForBlinking[bombPosRow][bombPosCol] = matrixForBlinking[bombPosRow][bombPosCol] == 0 ? 4 : 0;
        }
    }
}

bool CheckIfOutsideBoundsOrWall(int rowVal, int colVal)
{
  if(rowVal < 0 || colVal < 0 || colVal >= matrixSize || rowVal >= matrixSize)
    return 1;
  if(matrix[rowVal][colVal] != 0)
    return 1;
  return 0;
}

void setup() {
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen
  GenerateTerrain(8, 8, wallPercentage);
  pinMode(horizontalJoystickPin, INPUT);
  pinMode(verticalJoystickPin, INPUT);
  //Serial.begin(9600);
  pinMode(buttonBombPin, INPUT_PULLUP);
}

void CheckForMoves()
{
  horizontalInput = analogRead(horizontalJoystickPin);
  verticalInput   = analogRead(verticalJoystickPin);

  if (verticalInput > 700 && hasMoved == 0)
    {
      if (!CheckIfOutsideBoundsOrWall(playerPosRow - 1, playerPosCol))
      {
          if (horizontalInput > 700)
          {
              if (!CheckIfOutsideBoundsOrWall(playerPosRow - 1, playerPosCol + 1))
              {
                  matrixForBlinking[playerPosRow][playerPosCol] = 0;
                  playerPosRow -= 1;
                  playerPosCol += 1;
                  hasMoved = 1;
              }
          }
          else if (horizontalInput < 300)
          {
              if (!CheckIfOutsideBoundsOrWall(playerPosRow - 1, playerPosCol - 1))
              {
                  matrixForBlinking[playerPosRow][playerPosCol] = 0;
                  playerPosRow -= 1;
                  playerPosCol -= 1;
                  hasMoved = 1;
              }
          }
          else
          {
            matrixForBlinking[playerPosRow][playerPosCol] = 0;
            playerPosRow -= 1;
            hasMoved = 1;
          }
      }
  }
  else if (verticalInput < 400 && hasMoved == 0)
  {
      if (!CheckIfOutsideBoundsOrWall(playerPosRow + 1, playerPosCol))
      {
          if (horizontalInput > 600)
          {
              if (!CheckIfOutsideBoundsOrWall(playerPosRow + 1, playerPosCol + 1))
              {
                  matrixForBlinking[playerPosRow][playerPosCol] = 0;
                  playerPosRow += 1;
                  playerPosCol += 1;
                  hasMoved = 1;
              }
          }
          else if (horizontalInput < 300)
          {
              if (!CheckIfOutsideBoundsOrWall(playerPosRow + 1, playerPosCol - 1))
              {
                  matrixForBlinking[playerPosRow][playerPosCol] = 0;
                  playerPosRow += 1;
                  playerPosCol -= 1;
                  hasMoved = 1;
              }
          }
          else
          {
              matrixForBlinking[playerPosRow][playerPosCol] = 0;
              playerPosRow += 1;
              hasMoved = 1;
          }
      }
  }

  if(horizontalInput > 700 && hasMoved == 0)
  {
    if(!CheckIfOutsideBoundsOrWall(playerPosRow, playerPosCol + 1))
      {
        if(verticalInput > 700)
        {
          if(!CheckIfOutsideBoundsOrWall(playerPosRow - 1, playerPosCol + 1))
            {
              matrixForBlinking[playerPosRow][playerPosCol] = 0;
              playerPosRow -= 1;
              playerPosCol += 1;
              hasMoved = 1;
            }
        }
        else if(verticalInput < 300)
        {
            if(!CheckIfOutsideBoundsOrWall(playerPosRow + 1, playerPosCol + 1))
            {
              matrixForBlinking[playerPosRow][playerPosCol] = 0;
              playerPosRow += 1;
              playerPosCol += 1;
              hasMoved = 1;
            }
        }
        else
        {
            matrixForBlinking[playerPosRow][playerPosCol] = 0;
            playerPosCol += 1;
            hasMoved = 1;
        }
      }
  }
  else if(horizontalInput < 400 && hasMoved == 0)
  {
    if(!CheckIfOutsideBoundsOrWall(playerPosRow, playerPosCol - 1))
      {
        if(verticalInput > 600)
        {
          if(!CheckIfOutsideBoundsOrWall(playerPosRow - 1, playerPosCol - 1))
            { 
              matrixForBlinking[playerPosRow][playerPosCol] = 0;
              playerPosRow -= 1;
              playerPosCol -= 1;
              hasMoved = 1;
            }
        }
        else if(verticalInput < 300)
        {
            if(!CheckIfOutsideBoundsOrWall(playerPosRow + 1, playerPosCol - 1))
            {
              matrixForBlinking[playerPosRow][playerPosCol] = 0;
              playerPosRow += 1;
              playerPosCol -= 1;
              hasMoved = 1;
            }
        }
        else 
        {
          matrixForBlinking[playerPosRow][playerPosCol] = 0;
          playerPosCol -= 1;
          hasMoved = 1;
        }
      }
  }
  if(millis() - moveStart > moveInterval)
  {
    moveStart = millis();
    hasMoved = 0;
  }
}

void CheckForBomb()
{
  bombInput = !digitalRead(buttonBombPin);

  if(bombInput && 
    millis() - bombInputStart > 50 &&
    bombPosRow < 0 && bombPosCol < 0 && 
    hasHandledButtonClick == 0)
  {
    bombPosRow = playerPosRow;
    bombPosCol = playerPosCol;
    hasHandledButtonClick = 1;
    bombTimerDetonateStart = millis();
    bombInputStart = millis();
  }

  else if(bombInput == 0)
    hasHandledButtonClick = 0;

  if(bombPosRow >=0 && bombPosCol >= 0)
  {
    if(millis() - bombTimerDetonateStart > bombDetonationTimer)
    {
      // Check if moving up is within the matrix border
      if (bombPosRow - 1 >= 0) 
      {
          if (matrix[bombPosRow - 1][bombPosCol] == 2) 
              {
                matrix[bombPosRow - 1][bombPosCol] = 0;
                matrixForBlinking[bombPosRow - 1][bombPosCol] = 0;
              }
      }

      // Check if moving left is within the matrix border
      if (bombPosCol - 1 >= 0) 
      {
          if (matrix[bombPosRow][bombPosCol - 1] == 2) 
              {
                matrix[bombPosRow][bombPosCol - 1] = 0;
                matrixForBlinking[bombPosRow][bombPosCol - 1] = 0;
              }
      }

      // Check if moving down is within the matrix border
      if (bombPosRow + 1 < 8) 
      {
          if (matrix[bombPosRow + 1][bombPosCol] == 2) 
              {
                matrix[bombPosRow + 1][bombPosCol] = 0;
                matrixForBlinking[bombPosRow + 1][bombPosCol] = 0;
              }
      }

      // Check if moving right is within the matrix border
      if (bombPosCol + 1 < 8) 
      {
          if (matrix[bombPosRow][bombPosCol + 1] == 2) 
              {
                matrix[bombPosRow][bombPosCol + 1] = 0;
                matrixForBlinking[bombPosRow][bombPosCol + 1] = 0;
              }
      }
      matrix[bombPosRow][bombPosCol] = 0;
      matrixForBlinking[bombPosRow][bombPosCol] = 0;
      bombPosRow = -1;
      bombPosCol = -1;
    }
  }
}
void loop() 
{
  for (int row = 0; row < matrixSize; row++) 
  {
    for (int col = 0; col < matrixSize; col++) 
    {
      lc.setLed(0, row, col, (matrixForBlinking[row][col] > 0));
    }
  }
  BlinkMatrixOnTimers();
  CheckForMoves();
  CheckForBomb();
}
