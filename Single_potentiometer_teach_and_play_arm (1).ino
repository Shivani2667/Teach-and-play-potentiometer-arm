/*
 * Single Potentiometer Robotic Arm Teach and Play
 * CONTROLS:
 * - Knob (A0): Controls the selected motor.
 * - Button 1 (Pin 2): SELECTOR. Cycles through Base -> Shoulder -> Elbow -> Gripper.
 * - Button 2 (Pin 4): RECORD. Click to save pose. Hold 2s to wipe memory.
 * - Button 3 (Pin 7): PLAY. Replays the saved sequence.
 * * FEEDBACK:
 * - LEDs on Pins 10, 11, 12, 13 show which motor is active.
 */

#include <Servo.h>

Servo base, shoulder, elbow, gripper;

//PIN DEFINITIONS
const int PIN_KNOB = A0;
const int PIN_BTN_SELECT = 2;
const int PIN_BTN_REC = 4;
const int PIN_BTN_PLAY = 7;

// LEDs
const int LED_BASE = 10;
const int LED_SHLDR = 11;
const int LED_ELBOW = 12;
const int LED_GRIP = 13;

// SYSTEM VARIABLES
int currentMotor = 0;       // 0=Base, 1=Shoulder, 2=Elbow, 3=Gripper
int currentAngles[4];       // Stores where the robot IS right now
int locked[4] = {0, 0, 0, 0}; // 0 = Unlocked (Safe), 1 = Locked (Moving)

// MEMORY VARIABLES 
const int MAX_STEPS = 50;
int savedPos[MAX_STEPS][4]; 
int totalSteps = 0;
bool isPlaying = false;

// Debounce timer
unsigned long lastPress = 0;

void setup() {
  Serial.begin(9600);

  // 1. Attach Motors
  base.attach(3);
  shoulder.attach(5);
  elbow.attach(6);
  gripper.attach(9);

  // 2. Set Start Position (Home)
  // We start everything at 90 so it doesn't flop around
  currentAngles[0] = 90; 
  currentAngles[1] = 90; 
  currentAngles[2] = 90; 
  currentAngles[3] = 90;

  base.write(90); 
  shoulder.write(90); 
  elbow.write(90); 
  gripper.write(90);

  // 3. Setup Buttons (Internal Pullups - No resistors needed!)
  pinMode(PIN_BTN_SELECT, INPUT_PULLUP);
  pinMode(PIN_BTN_REC, INPUT_PULLUP);
  pinMode(PIN_BTN_PLAY, INPUT_PULLUP);

  // 4. Setup LEDs
  pinMode(LED_BASE, OUTPUT);
  pinMode(LED_SHLDR, OUTPUT);
  pinMode(LED_ELBOW, OUTPUT);
  pinMode(LED_GRIP, OUTPUT);

  // Light up the first LED
  updateLEDs();
  
  Serial.println("=== ROBOT READY ===");
  Serial.println("Turn Knob to catch the motor position!");
}

void loop() {
  if (isPlaying) return; // Freeze controls while playing

  // 
  // 1. SELECTOR BUTTON (Change Motor)
  //
  if (digitalRead(PIN_BTN_SELECT) == LOW) {
    if (millis() - lastPress > 300) { // Anti-glitch wait
      currentMotor++;
      if (currentMotor > 3) currentMotor = 0; // Loop back to Base
      
      // When we switch, we turn OFF control until knob catches up
      locked[currentMotor] = 0; 
      
      updateLEDs();
      Serial.print("Switched to Motor: "); Serial.println(currentMotor);
      lastPress = millis();
    }
  }

  //
  // 2. KNOB CONTROL (The Logic Brain)
  //
  int knobVal = analogRead(PIN_KNOB);
  int targetAngle = map(knobVal, 0, 1023, 0, 180);

  // SAFETY CHECK: Soft Takeover
  // If we are "Unlocked", we check if the knob matches the motor
  if (locked[currentMotor] == 0) {
    int diff = abs(targetAngle - currentAngles[currentMotor]);
    if (diff < 10) { // If knob is within 10 degrees of motor
      locked[currentMotor] = 1; // LOCK IT! You have control now.
      Serial.println("-> Control CAUGHT! Moving...");
    }
  }

  // MOVE IT: If locked, update the servo
  if (locked[currentMotor] == 1) {
    currentAngles[currentMotor] = targetAngle; // Save to current state variables
    
    switch(currentMotor) {
      case 0: base.write(targetAngle); break;
      case 1: shoulder.write(targetAngle); break;
      case 2: elbow.write(targetAngle); break;
      case 3: gripper.write(targetAngle); break;
    }
    delay(15); // Smoothness delay
  }

  //
  // 3. RECORD BUTTON (Save / Wipe)
  //
  if (digitalRead(PIN_BTN_REC) == LOW) {
    unsigned long pressStart = millis();
    bool wiped = false;

    // Check if user is holding button (WIPE)
    while (digitalRead(PIN_BTN_REC) == LOW) {
      if (millis() - pressStart > 2000) {
        totalSteps = 0;
        Serial.println("MEMORY WIPED!");
        blinkAllLEDs(); // Visual confirmation
        wiped = true;
        break; 
      }
    }
    
    // If it was just a short click (SAVE)
    if (!wiped && millis() - lastPress > 300) {
      if (totalSteps < MAX_STEPS) {
        // Save all 4 current angles into memory
        for(int k=0; k<4; k++) savedPos[totalSteps][k] = currentAngles[k];
        totalSteps++;
        Serial.print("Position Saved. Step count: "); Serial.println(totalSteps);
        delay(200); 
      }
      lastPress = millis();
    }
  }


  // 4. PLAY BUTTON

  if (digitalRead(PIN_BTN_PLAY) == LOW) {
    if (millis() - lastPress > 300) {
      Serial.println("Playing Sequence...");
      playSequence();
      lastPress = millis();
    }
  }
}



void updateLEDs() {
  // Turn all OFF
  digitalWrite(LED_BASE, LOW);
  digitalWrite(LED_SHLDR, LOW);
  digitalWrite(LED_ELBOW, LOW);
  digitalWrite(LED_GRIP, LOW);

  // Turn active one ON
  switch (currentMotor) {
    case 0: digitalWrite(LED_BASE, HIGH); break;
    case 1: digitalWrite(LED_SHLDR, HIGH); break;
    case 2: digitalWrite(LED_ELBOW, HIGH); break;
    case 3: digitalWrite(LED_GRIP, HIGH); break;
  }
}

void blinkAllLEDs() {
  for(int k=0; k<3; k++) {
    digitalWrite(LED_BASE, HIGH); digitalWrite(LED_SHLDR, HIGH);
    digitalWrite(LED_ELBOW, HIGH); digitalWrite(LED_GRIP, HIGH);
    delay(100);
    digitalWrite(LED_BASE, LOW); digitalWrite(LED_SHLDR, LOW);
    digitalWrite(LED_ELBOW, LOW); digitalWrite(LED_GRIP, LOW);
    delay(100);
  }
  updateLEDs(); // Restore correct light
}

void playSequence() {
  isPlaying = true;
  
  // Move smoothly through every saved step
  for (int i = 0; i < totalSteps; i++) {
    smoothMove(savedPos[i][0], savedPos[i][1], savedPos[i][2], savedPos[i][3]);
    delay(500); // Wait 0.5s at each point
  }
  
  // Update the system so it knows where the arm ended up

  currentAngles[0] = base.read();
  currentAngles[1] = shoulder.read();
  currentAngles[2] = elbow.read();
  currentAngles[3] = gripper.read();
  

  locked[0]=0; locked[1]=0; locked[2]=0; locked[3]=0;
  
  isPlaying = false;
  Serial.println("Playback Done.");
}

void smoothMove(int t1, int t2, int t3, int t4) {
  int start1 = base.read();
  int start2 = shoulder.read();
  int start3 = elbow.read();
  int start4 = gripper.read();
  
  int steps = 50; 
  for (int j = 0; j <= steps; j++) {
    base.write(start1 + (t1 - start1) * j / steps);
    shoulder.write(start2 + (t2 - start2) * j / steps);
    elbow.write(start3 + (t3 - start3) * j / steps);
    gripper.write(start4 + (t4 - start4) * j / steps);
    delay(20); 
  }
}
