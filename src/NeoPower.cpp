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

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "NeoPower.h"

Adafruit_NeoPixel NeoPow (NEOPOWER_MOTOR_NUM_MAX, NEOPOWER_GPIO_PIN, NEO_GRB + NEO_KHZ800);


// Constructor
NeoPower::NeoPower(void)
{
  init ();
}

// The init function initializes only the internal variables
void NeoPower::init(void)
{
  uint8_t neo_loop;

  for (neo_loop = 0; neo_loop < NEOPOWER_MOTOR_NUM_MAX; neo_loop ++)
  {
    _driv_type_mirror[neo_loop] = NEOPOWER_DRIVER_TYPE_2PIN;
    _mot_state_mirror[neo_loop] = NEOPOWER_MOTOR_STATE_OFF;
    _mot_speed_mirror[neo_loop] = NEOPOWER_MOTOR_SPEED_STOP;
  }
  _mot_number  = NEOPOWER_MOTOR_NUM_MAX;
  _neopix_used = 0;
}

// The begin function initializes the internal variables
//  and sets up the Neopixel driver for internal use.
void NeoPower::begin(void)
{
  init ();
  
  NeoPow.begin ();
  NeoPow.clear ();
  NeoPow.setBrightness (255);
  NeoPow.fill (NeoPow.Color (NEOPOWER_MOTOR_SPEED_IDLE, NEOPOWER_MOTOR_SPEED_IDLE, NEOPOWER_MOTOR_STATE_OFF));
  NeoPow.show ();
  
  _neopix_used = 1;
}


// ##########################################################
// ##   Access funtions for NeoPower motor driver boards   ##
// ##########################################################

void NeoPower::setDriverType(uint8_t mot_num, uint8_t drv_type)
{
  if (drv_type <= NEOPOWER_DRIVER_TYPE_3PIN)
  {
    if (mot_num < NEOPOWER_MOTOR_NUM_MAX)
    {
      _driv_type_mirror[mot_num] = drv_type;
    }
    else if (mot_num == NEOPOWER_MOTOR_NUM_ALL)
    {
      uint8_t neo_loop;

      for (neo_loop = 0; neo_loop < NEOPOWER_MOTOR_NUM_MAX; neo_loop ++)
      {
        _driv_type_mirror[neo_loop] = drv_type;
      }
    }
  }
}

uint8_t NeoPower::getDriverType(uint8_t mot_num)
{
  uint8_t drv_type = NEOPOWER_DRIVER_TYPE_NONE;

  if (mot_num < NEOPOWER_MOTOR_NUM_MAX)
  {
    drv_type = _driv_type_mirror[mot_num];
  }

  return drv_type;
}

// Set the number of actively used NeoPower nodes in the daisy chain.
// The number is limited to NEOPOWER_MOTOR_NUM_MAX
void NeoPower::setMotorNumber(uint8_t mot_num)
{
  if (mot_num <= NEOPOWER_MOTOR_NUM_MAX)
  {
    _mot_number = mot_num;
  }
  else
  {
    _mot_number = NEOPOWER_MOTOR_NUM_MAX;
  }
}

uint8_t NeoPower::getMotorNumber(void)
{
  return _mot_number;
}

// Convert motor settings to Neopixel colours:
// Calculate the PWM colour pattern for a NeoPower unit, depending on the driver type
uint32_t NeoPower::getNeoValue (uint8_t driv_type, uint8_t mot_speed, uint8_t mot_state)
{
  uint32_t neo_val;
  uint32_t neo_grn = NEOPOWER_MOTOR_SPEED_STOP;
  uint32_t neo_red = NEOPOWER_MOTOR_SPEED_STOP;
  uint32_t neo_blu = NEOPOWER_MOTOR_STATE_OFF;

  switch (driv_type)
  {
    case NEOPOWER_DRIVER_TYPE_HALF:
      // Motor driver consists of several separated half-bridges
      // Green = IN,  Blue = EN
      if (mot_state == NEOPOWER_MOTOR_STATE_FWD)
      {
        neo_grn = NEOPOWER_MOTOR_SPEED_FULL;
      }
      neo_blu = mot_speed;
      break;

    case NEOPOWER_DRIVER_TYPE_2PIN:
      // Motor driver has two input pins (e.g. DRV8251)
      // Green = IN1, Red = IN2
      switch (mot_state)
      {
        case NEOPOWER_MOTOR_STATE_OFF:
          neo_grn = NEOPOWER_MOTOR_SPEED_FULL;
          neo_red = NEOPOWER_MOTOR_SPEED_FULL;
          break;

        case NEOPOWER_MOTOR_STATE_FWD:
          neo_grn = mot_speed;
          break;

        case NEOPOWER_MOTOR_STATE_REV:
          neo_red = mot_speed;
          break;

        case NEOPOWER_MOTOR_STATE_BRAKE:
        default:
          /* Nothing to do */
          break;
      }
      break;

    case NEOPOWER_DRIVER_TYPE_3PIN:
      // Motor driver has three input pins (e.g. L6205)
      // Green = IN1, Red = IN2, Blue = EN
      switch (mot_state)
      {
        case NEOPOWER_MOTOR_STATE_FWD:
          neo_grn = NEOPOWER_MOTOR_SPEED_FULL;
          break;

        case NEOPOWER_MOTOR_STATE_REV:
          neo_red = NEOPOWER_MOTOR_SPEED_FULL;
          break;

        case NEOPOWER_MOTOR_STATE_BRAKE:
          neo_grn = NEOPOWER_MOTOR_SPEED_FULL;
          neo_red = NEOPOWER_MOTOR_SPEED_FULL;
          break;

        case NEOPOWER_MOTOR_STATE_OFF:
        default:
          /* Nothing to do */
          break;
      }
      neo_blu = mot_speed;
      break;

    case NEOPOWER_DRIVER_TYPE_NONE:
    default:
      /* Nothing to do */
      break;
  }

  // Combine the 3 individual colours into one 32-bit value according to NEO_GRB pattern
  neo_val = (neo_grn << 16) | (neo_red << 8) | neo_blu;

  return neo_val;
}

// Calculate the PWM pattern for a NeoPower unit, depending on the stored internal variables
uint32_t NeoPower::getNeoValue (uint8_t mot_num)
{
  uint32_t neo_val = 0x00000000;

  if (mot_num < NEOPOWER_MOTOR_NUM_MAX)
  {
    neo_val = getNeoValue (_driv_type_mirror[mot_num], _mot_speed_mirror[mot_num], _mot_state_mirror[mot_num]);
  }

  return neo_val;
}

// Set the state + direction of one motor without changing its speed
void NeoPower::setMotorState(uint8_t mot_num, uint8_t mot_state)
{
  mot_state = mot_state & NEOPOWER_MOTOR_STATE_MASK;

  if (mot_num < NEOPOWER_MOTOR_NUM_MAX)
  {
    _mot_state_mirror[mot_num] = mot_state;

    neoRefresh ();
  }
  else if (mot_num == NEOPOWER_MOTOR_NUM_ALL)
  {
    uint8_t neo_loop;

    for (neo_loop = 0; neo_loop < NEOPOWER_MOTOR_NUM_MAX; neo_loop ++)
    {
      _mot_state_mirror[neo_loop] = mot_state;
    }

    neoRefresh ();
  }
}

// Get the state + direction of one motor
uint8_t NeoPower::getMotorState(uint8_t mot_num)
{
  uint8_t mot_state = NEOPOWER_MOTOR_STATE_IDLE;

  if (mot_num < NEOPOWER_MOTOR_NUM_MAX)
  {
    mot_state = _mot_state_mirror[mot_num];
  }

  return mot_state;
}

// Set the speed of one (or several) motor(s) without changing the direction
void NeoPower::setMotorSpeed(uint8_t mot_num, uint8_t mot_speed)
{
  if (mot_num < NEOPOWER_MOTOR_NUM_MAX)
  {
    _mot_speed_mirror[mot_num] = mot_speed;

    neoRefresh ();
  }
  else if (mot_num == NEOPOWER_MOTOR_NUM_ALL)
  {
    uint8_t neo_loop;

    for (neo_loop = 0; neo_loop < NEOPOWER_MOTOR_NUM_MAX; neo_loop ++)
    {
      _mot_speed_mirror[neo_loop] = mot_speed;
    }

    neoRefresh ();
  }
}

// Get the current speed of one motor
uint8_t NeoPower::getMotorSpeed(uint8_t mot_num)
{
  uint8_t mot_speed = NEOPOWER_MOTOR_SPEED_IDLE;

  if (mot_num < NEOPOWER_MOTOR_NUM_MAX)
  {
    mot_speed = _mot_speed_mirror[mot_num];
  }

  return mot_speed;
}

// Set the speed + state + direction of one motor
void NeoPower::setMotor(uint8_t mot_num, int mot_speed)
{
  uint8_t mot_state;

  if (mot_speed > 0)
  {
    mot_state = NEOPOWER_MOTOR_STATE_FWD;

    if (mot_speed > 255)
	  {
	    mot_speed = 255;
	  }
  }
  else if (mot_speed < 0)
  {
    mot_state = NEOPOWER_MOTOR_STATE_REV;
    mot_speed = -mot_speed;
	  if (mot_speed > 255)
	  {
	    mot_speed = 255;
	  }
  }
  else  /* mot_speed == 0 */
  {
	  mot_state = NEOPOWER_MOTOR_STATE_OFF;
  }

  if (mot_num < NEOPOWER_MOTOR_NUM_MAX)
  {
    _mot_state_mirror[mot_num] = mot_state;
    _mot_speed_mirror[mot_num] = mot_speed;

    neoRefresh ();
  }
  else if (mot_num == NEOPOWER_MOTOR_NUM_ALL)
  {
    uint8_t neo_loop;

    for (neo_loop = 0; neo_loop < NEOPOWER_MOTOR_NUM_MAX; neo_loop ++)
    {
      _mot_state_mirror[neo_loop] = mot_state;
      _mot_speed_mirror[neo_loop] = mot_speed;
    }

    neoRefresh ();
  }
}

// Get the speed + direction of a motor combined in one value
int NeoPower::getMotor(uint8_t mot_num)
{
  int     mot_speed;
  uint8_t mot_state;
  
  mot_state = getMotorState (mot_num);
  mot_speed = getMotorSpeed (mot_num);

  if (mot_state == NEOPOWER_MOTOR_STATE_REV)
  {
    mot_speed = -mot_speed;
  }

  return mot_speed;
}

void NeoPower::stopMotor(uint8_t mot_num)
{
  if (mot_num < NEOPOWER_MOTOR_NUM_MAX)
  {
    _mot_state_mirror[mot_num] = NEOPOWER_MOTOR_STATE_OFF;
    _mot_speed_mirror[mot_num] = NEOPOWER_MOTOR_SPEED_STOP;
  }
  else if (mot_num == NEOPOWER_MOTOR_NUM_ALL)
  {
    uint8_t neo_loop;

    for (neo_loop = 0; neo_loop < NEOPOWER_MOTOR_NUM_MAX; neo_loop ++)
    {
      _mot_state_mirror[neo_loop] = NEOPOWER_MOTOR_STATE_OFF;
      _mot_speed_mirror[neo_loop] = NEOPOWER_MOTOR_SPEED_STOP;
    }
  }

  neoRefresh ();
}

void NeoPower::stopAllMotors(void)
{
  stopMotor (NEOPOWER_MOTOR_NUM_ALL);
}

void NeoPower::neoRefresh(void)
{
  uint8_t  neo_loop;
  uint32_t neo_val;

  // only if internal NeoPixel usage has been initialized before:
  if (_neopix_used > 0)
  {
    for (neo_loop = 0; neo_loop < _mot_number; neo_loop ++)
    {
      neo_val = getNeoValue (_driv_type_mirror[neo_loop], _mot_speed_mirror[neo_loop], _mot_state_mirror[neo_loop]);
      NeoPow.setPixelColor (neo_loop, neo_val);
    }
    NeoPow.show();
  }
}
