# Introduction-to-Robotics 2023-2024
*Repo home to the homework/projects assigned as part of the IR course taken in the final year at FMI, UB. Each homework should specify the corresponding tasks, implementation, code and image files that went into the process of solving it.*

# Homework 6
<details>
<summary>Develop a small game on the 8x8 matrix. The game must have at least 3 types of elements: player (blinks slowly), bombs/bullets (blinks fast), wall (doesn’t blink). The basic idea is that you generate walls on the map (50% - 75% of the map) and then you move around with the player and destroy them. You can do it Bomberman style or terminator-tanks style (or another way, be creative). </summary>
<img src="https://github.com/Eronate/Introduction-to-Robotics/assets/99949441/e58eeafa-4c25-4b1d-83ac-b1447196748f" width="500" height="400">
  
Functionality showcase: https://youtu.be/b-_ReiODskc
</details>

## Homework 5

<details>
<summary>Develop a ”Smart Environment Monitor and Logger” using Arduino. This system will utilize various sensors to gather environmental data, log this data into EEPROM, and provide both visual feedback via an RGB LED and user interaction through a Serial Menu.</summary>
<img src="https://github.com/Eronate/Introduction-to-Robotics/assets/99949441/7f98f412-7b50-4f4a-895e-83a88166bae8" width="500" height="500">
  
Functionality showcase: https://youtu.be/cpWhU1-xfDY
</details>

## Homework 4 

<details>
<summary>Task summary: Using the 4 digit 7 segment display and 3 buttons, implement a stopwatch timer that counts in 10ths of a second and has a save lap functionality (similar to most basic stopwatch functionson most phones). Holding the lap button should cycle between laps, and saving more than 4 laps should overwrite the ones saved first. 
The starting value of the 4 digit 7 segment display should be ”000.0”. The buttons should have the following functionalities: <br>
  – Button 1: Start / pause. <br>
  – Button 2: Reset (if in pause mode). Reset saved laps (if  in  lapviewing mode). <br>
  – Button 3: Save lap (if in counting mode), cycle through last saved laps (up to 4 laps).</summary>
<br>
<img src="https://github.com/Eronate/Introduction-to-Robotics/assets/99949441/313885bb-ea2e-4446-b86b-705888deb67f" width="500" height="500">

Functionality showcase: https://www.youtube.com/shorts/n1kGXVphehw
</details>

## Homework 3
<details>
<summary> Task summary: "Use the joystick to control the position of the segment and ”draw” on the display. The movement between segments should be natural, meaning they should jump from the current positiononly to neighbors, but without passing through ”walls”. The initial position should be on the DP. The current position always blinks (irrespective of the fact that the segment is on or off). Use the joystick to move from one position to neighbors (see table for corresponding movement). Short pressing the button toggles the segment state from ON to OFF or from OFF to ON. Long pressing the button resets the entire display by turning all the segments OFF and moving the current position to the decimal point." </summary>

<img src="https://github.com/Eronate/Introduction-to-Robotics/assets/99949441/211e6255-5d09-49f7-ae75-dc0811ae08b2" width="500" height="500">

Functionality showcase: https://youtube.com/shorts/h5qEf3AfeyA
</details>

## Homework 2
<details>
<summary> Task summary: "Design a control system that simulates a 3-floor elevator using the Arduino platform. LED Indicators: Each of the 3 LEDs should represent one of the 3 floors. The LED corresponding to the current floor should light up.  Additionally, another LED should represent the elevator’s operational state.  It should blink when the elevator is moving and remain static when stationary. Implement 3 buttons that represent the call buttons from the 3 floors. When pressed, the elevator should simulate movement towardsthe floor after a short interval (2-3 seconds)." </summary>

<img src="https://github.com/Eronate/Introduction-to-Robotics/assets/99949441/75fddbcf-cd0b-497e-815b-9e33f9422eca" width="500" height="500">

Functionality showcase: https://youtube.com/shorts/96-ROu_GeIg?feature=share
</details>

## Homework 1
<details>
<summary> Task summary: "Use a separate potentiometer for controlling each color of the RGB LED: Red, Green and Blue. This control must leverage digital electronics. Specifically, you need to read the potentiometer’s value with Arduino and then  write a mapped value to the LED pins."</summary>

<img src="https://github.com/Eronate/Introduction-to-Robotics/assets/99949441/bbe62097-65e5-4d6f-bc76-840cc9cc204f" width="500" height="500">

Functionality showcase: https://youtube.com/shorts/SaiOIygApBE?feature=share
</details>
