#include <EEPROM.h>

#define  MAIN_MENU 0

#define  SENSOR_SETTINGS 1
#define  SENSOR_SAMPLING_INTERVAL 2
#define  ULTRASONIC_ALERT_THRESHOLD 3
#define  LDR_ALERT_THRESHOLD 4

#define  RESET_LOGGER_DATA 5

#define  SYSTEM_STATUS 6
#define  CURRENT_SENSOR_READINGS 7
#define  CURRENT_SENSOR_SETTINGS 8
#define  DISPLAY_LOGGED_DATA 9

#define  RGB_LED_CONTROL 10
#define  MANUAL_COLOR_CONTROL 11
#define  LED_TOGGLE 12

#define  UNDEFINED -100
#define  SENSOR_LOG_SIZE 10
#define  ultrasonicMinThreshold 0
#define  ultrasonicMaxThreshold 300
#define  photocellMinThreshold 0
#define  photocellMaxThreshold 500
#define  initialSSRU 3 
#define  initialSSRP 3
//Pin initialization
const int echoPin = 10, trigPin = 9, photocellVoltagePin = 11, photocellInputPin = A1, redLedPin = 6, greenLedPin = 5, blueLedPin = 3;
//Variables to handle sensor reading 
int photocellValue, ultrasonicDuration, ultrasonicDistance;
unsigned long readingTimePhoto=0, readingTimeUltrasonic=0;

int menuState = MAIN_MENU, hasDisplayed = 0, choice;
int sensorSamplingRateUltrasonic = 3, sensorSamplingRatePhoto = 3;
unsigned long lastPrintedTimeUltrasonic = 0, lastPrintedTimePhoto = 0;

int ultrasonicAlertThreshold = 40, 
    photocellAlertThreshold = 200;

int *ultrasonicLogs = new int[SENSOR_LOG_SIZE], *photocellLogs = new int[SENSOR_LOG_SIZE], 
    ultrasonicLogsSize = 0, photocellLogsSize = 0, ultrasonicLogsIndex=-1, photocellLogsIndex=-1;

int redSavedVal =0, greenSavedVal=0, blueSavedVal=255;

bool ifTrueThenAutomatic = 1;

void setup() 
{
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(photocellVoltagePin, OUTPUT);
  pinMode(photocellInputPin, INPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  digitalWrite(photocellVoltagePin, HIGH); //5v fed to photocell circuit
  Serial.begin(9600); // Starts the serial communication
}

void ReadSensors()
{

  if(EEPROM.read(0) != sensorSamplingRateUltrasonic)
    sensorSamplingRateUltrasonic = EEPROM.read(0);

  if(EEPROM.read(sizeof(int)) != sensorSamplingRatePhoto)
    sensorSamplingRatePhoto = EEPROM.read(sizeof(int));

  if(millis() - readingTimeUltrasonic > sensorSamplingRateUltrasonic * 1000)
  { 
    //Signal the ultrasonic sensor to send ultrasonic waves and intercept them, calculating the distance 
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    ultrasonicDuration = pulseIn(echoPin, HIGH);  
    ultrasonicDistance = ultrasonicDuration*0.034/2; //calculation ends here

    readingTimeUltrasonic = millis(); 

    //if the log array is full, move each value 1 index back to make room for the latest one.
    if(ultrasonicLogsIndex == SENSOR_LOG_SIZE)
      {
        for(int i = 0 ; i< ultrasonicLogsSize; i++)
          ultrasonicLogs[i] = ultrasonicLogs[i+1];
        ultrasonicLogs[ultrasonicLogsSize - 1] = ultrasonicDistance;
      }
    else
      ultrasonicLogs[++ultrasonicLogsIndex] = ultrasonicDistance;
    ultrasonicLogsSize = min(ultrasonicLogsSize + 1, SENSOR_LOG_SIZE);
  }
  if(millis() - readingTimePhoto > sensorSamplingRatePhoto * 1000)
    {
      photocellValue= analogRead(photocellInputPin);
      readingTimePhoto = millis();

      if(photocellLogsIndex == SENSOR_LOG_SIZE)
      {
        for(int i = 0 ; i< photocellLogsSize; i++)
          photocellLogs[i] = photocellLogs[i+1];
        photocellLogs[photocellLogsSize - 1] = photocellValue;
      }
      else
        photocellLogs[++photocellLogsIndex] = photocellValue;

      photocellLogsSize = min(photocellLogsSize + 1, SENSOR_LOG_SIZE);
    }
  
}

//handles Main Menu choice
void MainMenuHandler()
{
  //Only display the menu once, loop will come back here until Serial.available() >= 2
  if(hasDisplayed == 0)
  {
    Serial.println("1: Sensor settings ");
    Serial.println("2: Reset logger data ");
    Serial.println("3: System Status ");
    Serial.println("4: RGB Led Control");
    Serial.println("Choose your option: _");
    hasDisplayed = 1;
  }
  //When we finally have input
  if(Serial.available() >= 2)
  {
    choice = Serial.parseInt();
    hasDisplayed = 0;
    switch(choice)
    {
      case 1:
        menuState = SENSOR_SETTINGS;
        break;
      case 2:
        menuState = RESET_LOGGER_DATA;
        break;
      case 3:
        menuState = SYSTEM_STATUS;
        break;
      case 4:
        menuState = RGB_LED_CONTROL;
        break;
      default:
        {
          Serial.println("Something went wrong.");
          menuState = MAIN_MENU;
          break;
        }
    }
  }
}

//handles Sensor Settings submenu choice
void SensorSettingsHandler()
{
  //Only display the menu once, loop will come back here until Serial.available() >= 2
  if(hasDisplayed == 0)
  {
    Serial.println("  1: Sensor sampling interval ");
    Serial.println("  2: Ultrasonic alert threshold ");
    Serial.println("  3: LDR alert Threshold ");
    Serial.println("  4: Back");
    Serial.println("  Choose your option: _");
    hasDisplayed = 1;
  }
  //When we finally have input
  if(Serial.available() >= 2)
  {
    choice = Serial.parseInt();
    hasDisplayed = 0;
    switch(choice)
    {
      hasDisplayed = 0;
      case 1:
        menuState = SENSOR_SAMPLING_INTERVAL;
        break;
      case 2:
        menuState = ULTRASONIC_ALERT_THRESHOLD;
        break;
      case 3:
        menuState = LDR_ALERT_THRESHOLD;       
        break;
      case 4:
        menuState = MAIN_MENU;  //back to main menu      
        break;
      default:
        {
          Serial.println("Something went wrong.");
          menuState = MAIN_MENU;
          break;
        }
    }
  }
}

void SensorSamplingIntervalHandler() 
{
  int valueRead;
  if(hasDisplayed == 0)
  {
    Serial.println("Write individual values inline (Ultrasonic value, then LDR value): _ _");
    hasDisplayed = 1;
  }
  if(Serial.available() >= 4)
  {
    hasDisplayed = 0;
    valueRead = Serial.parseInt();
    EEPROM.put(0, valueRead);
    valueRead = Serial.parseInt();
    EEPROM.put(sizeof(int), valueRead);
    Serial.println("Your sensors have been updated to: " + String(EEPROM.read(0)) + " " + String(EEPROM.read(sizeof(int))));
    menuState = SENSOR_SETTINGS;
  }
}

void UltrasonicAlertThresholdHandler()
{
  if(hasDisplayed == 0)
  {
    Serial.println("Write the value for the ultrasonic alarm threshold, for it to work it has to be between those values: " + String(ultrasonicMinThreshold) + " and " + String(ultrasonicMaxThreshold) + ": _");
    hasDisplayed = 1;
  }
  if(Serial.available() >= 2)
  {
    hasDisplayed = 0;
    ultrasonicAlertThreshold = Serial.parseInt();
    Serial.println("Your ultrasonic sensor alert has been updated to: " + String(ultrasonicAlertThreshold));
    menuState = SENSOR_SETTINGS;
  }
}

void LDRAlertThresholdHandler()
{
  if(hasDisplayed == 0)
  {
    Serial.println("Write the value for the LDR alarm threshold, for it to work it has to be between those values: " + String(photocellMinThreshold) + " and " + String(photocellMaxThreshold) + ": _");
    hasDisplayed = 1;
  }
  if(Serial.available() >= 2)
  {
    hasDisplayed = 0;
    photocellAlertThreshold = Serial.parseInt();
    Serial.println("Your photocell sensor alert has been updated to: " + String(photocellAlertThreshold));
    menuState = SENSOR_SETTINGS;
  }
}
void ResetLoggerDataHandler()
{
  if(hasDisplayed == 0)
  {
    Serial.println("1. Yes!");
    Serial.println("2. No...");
    Serial.println("Are you a billion percent sure you want to do this? _");
    hasDisplayed = 1;
  }
  if(Serial.available() >= 2)
  {
    hasDisplayed = 0;
    choice = Serial.parseInt();
    menuState = MAIN_MENU;
    if(choice == 1)
    {
      delete [] photocellLogs;
      delete [] ultrasonicLogs;
      ultrasonicLogsSize = 0;
      photocellLogsSize = 0;
      ultrasonicLogsIndex = -1;
      photocellLogsIndex = -1;
      ultrasonicLogs = new int[SENSOR_LOG_SIZE];
      photocellLogs = new int[SENSOR_LOG_SIZE];
      Serial.println("Successfully deleted.");
      return;
    }
    else if (choice == 2)
    {
      return;
    }
    
    else if (choice != 1 and choice != 2)
    { 
      Serial.println("Something bad happened.");
      return;
    } 
  }
}

void SystemStatusHandler()
{
  //Only display the menu once, loop will come back here until Serial.available() >= 2
  if(hasDisplayed == 0)
  {
    Serial.println("  1: Current Sensor Readings");
    Serial.println("  2: Current Sensor Settings ");
    Serial.println("  3: Display Logged Data ");
    Serial.println("  4: Back");
    Serial.println("  Choose your option: _");
    hasDisplayed = 1;
  }
  //When we finally have input
  if(Serial.available() >= 2)
  {
    choice = Serial.parseInt();
    hasDisplayed = 0;
    switch(choice)
    {
      case 1:
        menuState = CURRENT_SENSOR_READINGS;
        break;
      case 2:
        menuState = CURRENT_SENSOR_SETTINGS;
        break;
      case 3:
        menuState = DISPLAY_LOGGED_DATA;       
        break;
      case 4:
        menuState = MAIN_MENU;  //back to main menu      
        break;
      default:
        {
          Serial.println("Something went wrong.");
          menuState = MAIN_MENU;
          break;
        }
    }
  }
}

void CurrentSensorReadingsHandler()
{
  if(hasDisplayed == 0)
  {
    Serial.println("Escape this menu by typing capital 'C'");
    hasDisplayed = 1;
  }
  if(Serial.available() > 1)
  {
    char letter; 
    letter = Serial.read();
    if(letter == 'C')
      {
        hasDisplayed = 0;
        menuState = SYSTEM_STATUS;
      }
    return;
  }
  else 
  {
    if(millis() - lastPrintedTimePhoto > sensorSamplingRatePhoto * 1000)
    {
      Serial.println("Luminosity: " + String(photocellValue));
      lastPrintedTimePhoto = millis();
    }
    if(millis() - lastPrintedTimeUltrasonic > sensorSamplingRateUltrasonic * 1000 )
    {
      Serial.println("Distance: " + String(ultrasonicDistance) + " cm");
      lastPrintedTimeUltrasonic = millis();
    }
  }
}

void CurrentSensorSettingsHandler()
{
  Serial.println("The current sensor settings are: ");
  Serial.println("Alert threshold: Photocell " + String(photocellAlertThreshold)+ " Ultrasonic " + String(ultrasonicAlertThreshold));
  Serial.println("Value threshold ultrasonic: max " + String(ultrasonicMaxThreshold)+ " min " + String(ultrasonicMinThreshold));
  Serial.println("Sampling rates in seconds: Photocell " + String(sensorSamplingRatePhoto)+ " Ultrasonic " + String(sensorSamplingRateUltrasonic));
  menuState = SYSTEM_STATUS;
  return;
}

void DisplayLoggedDataHandler()
{
    if(hasDisplayed == 0)
    {
      Serial.println("The last 10 sensor readings for ultrasonic: ");
      for(int i = 0; i < ultrasonicLogsSize; i++)
      {
        Serial.print(String(ultrasonicLogs[i]) + " ");
      }
      Serial.println();

      Serial.println("The last 10 sensor readings for photocell: ");
      for(int i = 0; i < photocellLogsSize; i++)
      {
        Serial.print(String(photocellLogs[i]) + " ");
      }
      Serial.println();
      hasDisplayed = 0;
      menuState = SYSTEM_STATUS;
      return;
    }
}

void RGBLedControlHandler()
{
    //Only display the menu once, loop will come back here until Serial.available() >= 2
  if(hasDisplayed == 0)
  {
    Serial.println("  1: Manual Color Control");
    Serial.println("  2: LED Toggle Automatic ON/OFF");
    Serial.println("  3: Back");
    Serial.println("  Choose your option: _");
    hasDisplayed = 1;
  }
  //When we finally have input
  if(Serial.available() >= 2)
  {
    choice = Serial.parseInt();
    hasDisplayed = 0;
    switch(choice)
    {
      case 1:
        menuState = MANUAL_COLOR_CONTROL;
        break;
      case 2:
        menuState = LED_TOGGLE;
        break;
      case 3:
        menuState = MAIN_MENU;       
        break;
      default:
        {
          Serial.println("Something went wrong.");
          menuState = MAIN_MENU;
          break;
        }
    }
  }
  return;
}

void ManualColorControlHandler()
{
  if(hasDisplayed == 0)
  {
    Serial.println("Type 0-255 values for Red, Green and Blue inline:  _, _, _");
    hasDisplayed = 1;
  }
  if(Serial.available() >= 6)
  {
    int readValue;
    hasDisplayed = 0;

    readValue  = Serial.parseInt();
    if(readValue <= 255 && readValue >= 0)
      redSavedVal = readValue;

    readValue  = Serial.parseInt();
    if(readValue <= 255 && readValue >= 0)
      greenSavedVal = readValue;

    readValue = Serial.parseInt();
    if(readValue <= 255 && readValue >= 0)
      blueSavedVal = readValue;

    hasDisplayed = 0;
    Serial.println("Updated values RED GREEN BLUE: " + String(redSavedVal) + " " + String(greenSavedVal)+ " " + String(blueSavedVal));
    menuState = RGB_LED_CONTROL;
    return;
  }
  return;
}

void LedToggleHandler() 
{
  if(hasDisplayed == 0)
  {
    Serial.print("Current mode: ");
    ifTrueThenAutomatic == 1 ? Serial.println("Automatic") : Serial.println("Manual");
    Serial.print("Do you wish to change to ");
    ifTrueThenAutomatic == 1 ? Serial.println("Manual?") : Serial.println("Automatic?");
    Serial.println("1. Yes");
    Serial.println("2. No");
    hasDisplayed = 1;
  }

  //When we finally have input
  if(Serial.available() >= 2)
  {
    choice = Serial.parseInt();
    hasDisplayed = 0;
    switch(choice)
    {
      case 1:
        {
          ifTrueThenAutomatic = !ifTrueThenAutomatic;
          Serial.println("Display mode changes saved.");
          menuState = RGB_LED_CONTROL;
        }
        break;
      case 2:
        menuState = RGB_LED_CONTROL;
        break;
      default:
        {
          Serial.println("Something went wrong.");
          menuState = MAIN_MENU;
          break;
        }
    }
  }
}

void LightUpRGBLed()
{
  if(ifTrueThenAutomatic == 1)
  {
    if(photocellValue < photocellAlertThreshold || ultrasonicDistance < ultrasonicAlertThreshold)
    {
      digitalWrite(redLedPin, HIGH);
      digitalWrite(greenLedPin, LOW);
      digitalWrite(blueLedPin, LOW);
    }
    else
    {
      digitalWrite(redLedPin, LOW);
      digitalWrite(greenLedPin, HIGH);
      digitalWrite(blueLedPin, LOW);
    }
  }

  else
  {
      analogWrite(redLedPin, redSavedVal);
      analogWrite(greenLedPin, greenSavedVal);
      analogWrite(blueLedPin, blueSavedVal);
  }
}

void MenuStateHandler()
{
  switch(menuState)
  {
    case MAIN_MENU:
      MainMenuHandler();
      break;
    case SENSOR_SETTINGS:
      SensorSettingsHandler();
      break;
    case SENSOR_SAMPLING_INTERVAL:
      SensorSamplingIntervalHandler();
      break;
    case ULTRASONIC_ALERT_THRESHOLD:
      UltrasonicAlertThresholdHandler();
      break;
    case LDR_ALERT_THRESHOLD:
      LDRAlertThresholdHandler();
      break;
    case RESET_LOGGER_DATA:
      ResetLoggerDataHandler();
      break;
    case SYSTEM_STATUS:
      SystemStatusHandler(); //
      break;
    case CURRENT_SENSOR_READINGS:
      CurrentSensorReadingsHandler();
      break;
    case CURRENT_SENSOR_SETTINGS:
      CurrentSensorSettingsHandler();
      break;
    case DISPLAY_LOGGED_DATA:
      DisplayLoggedDataHandler();
      break;
    case RGB_LED_CONTROL:
      RGBLedControlHandler();
      break;
    case MANUAL_COLOR_CONTROL:
      ManualColorControlHandler();
      break;
    case LED_TOGGLE:
      LedToggleHandler();
      break;
    default:  
      Serial.println("Something went wrong.");
      break;
  }
}

void loop() 
{
  ReadSensors();
  MenuStateHandler();
  LightUpRGBLed();
}
