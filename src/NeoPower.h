/*********************************************************************/
/**                                                                 **/
/**   Arduino library for NeoPower boards by Martin's Bastelstube   **/
/**    with any number of DC motor outputs in a daisy chain         **/
/**                                                                 **/
/***************************************************************************/
/**                                                                       **/
/**  https://bastelstube.rocci.net/projects/MBS39_NeoPower/NeoPower.html  **/
/**                                                                       **/
/***************************************************************************/

#ifndef NEOPOWER_H
#define NEOPOWER_H

#include <inttypes.h>

// Predefined values for motor driver type
#define NEOPOWER_DRIVER_TYPE_NONE    0
#define NEOPOWER_DRIVER_TYPE_HALF    1  // Green = IN,             Blue = EN
#define NEOPOWER_DRIVER_TYPE_2PIN    2  // Green = IN1, Red = IN2
#define NEOPOWER_DRIVER_TYPE_3PIN    3  // Green = IN1, Red = IN2, Blue = EN

#define NEOPOWER_DRIVER_TYPE_TWOHALF 3  // Combination of two half-bridges with a common EN signal
#define NEOPOWER_DRIVER_TYPE_L6205   3  // Two full H-bridges with Enable
#define NEOPOWER_DRIVER_TYPE_DRV8251 2  // One full H-bridge
#define NEOPOWER_DRIVER_TYPE_LB1641  2  // One full H-bridge

// Predefined values for motor speed
#define NEOPOWER_MOTOR_SPEED_STOP  0x00  // Motor is stopped
#define NEOPOWER_MOTOR_SPEED_IDLE  0x00  // Motor has no acceleration and no braking
#define NEOPOWER_MOTOR_SPEED_WEAK  0x10  // Motor has little acceleration
#define NEOPOWER_MOTOR_SPEED_SLOW  0x20
#define NEOPOWER_MOTOR_SPEED_QUART 0x40  // ... slightly increasing ...
#define NEOPOWER_MOTOR_SPEED_HALF  0x80
#define NEOPOWER_MOTOR_SPEED_FULL  0xFF  // Motor is running at full speed

// Predefined values for motor state and direction
#define NEOPOWER_MOTOR_STATE_OFF   0  // Motor is switched off
#define NEOPOWER_MOTOR_STATE_IDLE  0  // Motor has no acceleration and no braking
#define NEOPOWER_MOTOR_STATE_FWD   1  // Motor is running forwards
#define NEOPOWER_MOTOR_STATE_REV   2  // Motor is running backwards
#define NEOPOWER_MOTOR_STATE_BRAKE 3  // Motor is braking actively
#define NEOPOWER_MOTOR_STATE_MASK  3  // Bitmask for filtering the motor direction

// GPIO pin where the NeoPower daisy-chain is attached
#define NEOPOWER_GPIO_PIN          5

// Maximum number of supported motors depends on your microcontroller's memory space
#define NEOPOWER_MOTOR_NUM_MAX    32
// Magic number to apply specific actions to all motors at once
#define NEOPOWER_MOTOR_NUM_ALL   199

class NeoPower {
public:
  // Constructor
  NeoPower(void);
  // Initialize only the internal variables
  void init(void);
  // Initialize internal variables and set up the Neopixel driver for internal use
  void begin(void);

  // Specify the motor driver type
  void     setDriverType(uint8_t, uint8_t);
  uint8_t  getDriverType(uint8_t);

  // Convert motor settings to Neopixel colours
  uint32_t getNeoValue (uint8_t, uint8_t, uint8_t);
  uint32_t getNeoValue (uint8_t);

  // Control the motor outputs
  void     setMotorNumber(uint8_t);
  uint8_t  getMotorNumber(void);
  void     setMotorState(uint8_t, uint8_t);
  uint8_t  getMotorState(uint8_t);
  void     setMotorSpeed(uint8_t, uint8_t);
  uint8_t  getMotorSpeed(uint8_t);
  void     setMotor(uint8_t, int);
  int      getMotor(uint8_t);
  void     stopMotor(uint8_t);
  void     stopAllMotors(void);

private:

  // internal variables keeping the driver type, status + speed for each motor
  uint8_t _driv_type_mirror[NEOPOWER_MOTOR_NUM_MAX];
  uint8_t _mot_state_mirror[NEOPOWER_MOTOR_NUM_MAX];
  uint8_t _mot_speed_mirror[NEOPOWER_MOTOR_NUM_MAX];
  uint8_t _mot_number;
  uint8_t _neopix_used;

  // Update all attached NeoPower boards with the most recent motor control data
  void     neoRefresh(void);
};

#endif   /* NEOPOWER_H */
