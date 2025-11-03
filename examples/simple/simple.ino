// Simple demo of NeoPower library with Arduino Uno
// for detailed description of the hardware, please refer to
// https://bastelstube.rocci.net/projects/MBS39_NeoPower/NeoPower.html
//
//  (c) Martin's Bastelstube 2025

#include <Wire.h>
#include <NeoPower.h>

NeoPower  neo;

void setup() {
  // Initialize the NeoPower library and the NeoPixel driver
  neo.begin ();

  // The library supports having different types of NeoPower boards daisy-chained:
  neo.setDriverType(NEOPOWER_MOTOR_NUM_ALL, NEOPOWER_DRIVER_TYPE_2PIN);  // Nearly all motors have a driver chip with two input signals
  neo.setDriverType(3,                      NEOPOWER_DRIVER_TYPE_3PIN);  // only motor 3 has a different type of driver chip
}

void loop() {
  // Assuming we have a platform with four motors:
  delay (500);
  neo.setMotorState(0,  NEOPOWER_MOTOR_STATE_FWD);   // Motor 0 at half speed forward
  neo.setMotorSpeed(0,  NEOPOWER_MOTOR_SPEED_HALF);
  delay (500);
  neo.setMotorState(1,  NEOPOWER_MOTOR_STATE_FWD);   // Motor 1 at full speed forward
  neo.setMotorSpeed(1,  NEOPOWER_MOTOR_SPEED_FULL);
  delay (500);
  neo.setMotor     (2, -NEOPOWER_MOTOR_SPEED_HALF);  // Motor 2 at half speed backward
  delay (500);
  neo.setMotorState(3,  NEOPOWER_MOTOR_STATE_REV);   // Motor 3 at quarter speed backward
  neo.setMotorSpeed(3,  NEOPOWER_MOTOR_SPEED_QUART);
  delay (500);
  neo.setMotorState(0,  NEOPOWER_MOTOR_STATE_OFF);   // Motor 0 stop
  neo.setMotorSpeed(0,  NEOPOWER_MOTOR_SPEED_STOP);
  delay (500);
  neo.stopAllMotors();                              // All motors stop at once
  delay (1000);
}
