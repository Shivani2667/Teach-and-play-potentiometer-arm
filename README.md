# Teach-and-play-potentiometer-arm
# Single Potentiometer 4-DOF Robotic Arm

## Project Overview
This project demonstrates a cost-effective method for controlling a 4-DOF (Degrees of Freedom) robotic arm using a single analog potentiometer[cite: 10]. Instead of using four separate knobs, the system utilizes a State Selection Logic to switch control between the Base, Shoulder, Elbow, and Gripper[cite: 10].

A key feature of this system is the "Soft Takeover" Safety Protocol[cite: 10]. When switching motors, the potentiometer does not immediately move the new motor[cite: 10]. Instead, the user must turn the knob until it matches the current position of the motor ("catching" it)[cite: 10]. This prevents dangerous, sudden jerks often seen in shared-control systems[cite: 10].

## Features
*   **Multiplexed Control:** Control 4 servos with 1 potentiometer and a selection button[cite: 10].
*   **Visual Feedback:** 4 LEDs indicate which joint is currently active[cite: 10].
*   **Soft Takeover (Safety):** Prevents sudden motor jumps when switching channels; the user must align the knob with the motor's actual angle to regain control[cite: 10].
*   **Teach & Play:** Record a sequence of up to 50 moves and replay them with smooth interpolation[cite: 10].
*   **Memory Wipe:** Hold the Record button for 2 seconds to clear the sequence[cite: 10].

## Hardware Requirements
*   1x Arduino Uno (or compatible)[cite: 10]
*   4x Servo Motors (SG90 or MG995)[cite: 10]
*   1x 10k Potentiometer[cite: 10]
*   3x Push Buttons (Select, Record, Play)[cite: 10]
*   4x LEDs (Red/Green/Blue/Yellow recommended)[cite: 10]
*   4x 220Ω Resistors (for LEDs)[cite: 10]
*   Breadboard & Jumper Wires[cite: 10]

## Pin Mapping
| Component | Arduino Pin | Description |
| :--- | :--- | :--- |
| Potentiometer | A0 | Main control knob[cite: 11] |
| Btn: SELECT | D2 | Cycles active motor (Base → Shoulder → Elbow → Gripper)[cite: 11] |
| Btn: RECORD | D4 | Click to save pose; Hold 2s to wipe memory[cite: 11] |
| Btn: PLAY | D7 | Replays saved sequence[cite: 11] |
| Servo: Base | D3 | PWM Control[cite: 11] |
| Servo: Shoulder | D5 | PWM Control[cite: 11] |
| Servo: Elbow | D6 | PWM Control[cite: 11] |
| Servo: Gripper | D9 | PWM Control[cite: 11] |
| LED Indicators | D10, D11, D12, D13 | Corresponds to Base, Shoulder, Elbow, Gripper[cite: 11] |

## How to Use

**1. Manual Control (The "Catch" Method)**
1.  Power on the system[cite: 11]. The Base LED (D10) will light up[cite: 11].
2.  Turn the potentiometer[cite: 11]. You will notice the motor does not move immediately[cite: 11].
3.  Continue turning until the knob position matches the motor's current angle[cite: 11].
4.  The Serial Monitor will print `-> Control CAUGHT!`[cite: 11]. You now have full control of that joint[cite: 11].
5.  Press the SELECT button to switch to the next joint (Shoulder)[cite: 11].
6.  Repeat the "Catch" process to move the shoulder[cite: 11].

**2. Teaching Mode (Recording)**
1.  Move the arm to a desired pose using the method above[cite: 12].
2.  Press the RECORD button once[cite: 12].
3.  The Serial Monitor will confirm: `Position Saved. Step count: X`[cite: 12].
4.  Repeat for up to 50 steps[cite: 12].

**3. Playback Mode**
1.  Press the PLAY button[cite: 12].
2.  The arm will replay all saved steps with smooth interpolation between points[cite: 12].
3.  Controls are locked during playback[cite: 12].

## Future Improvements
*   Implement EEPROM storage to save sequences even after power is disconnected[cite: 12].
*   Add a "Pause" function during playback[cite: 12].
