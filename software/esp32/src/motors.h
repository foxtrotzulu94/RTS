#ifndef __RTS_MOTOR__
#define __RTS_MOTOR__

#include "constants.h"

const int MIN_OPERATING_VOLTAGE = 6; 
const int MAX_OPERATING_VOLTAGE = 11;

static /*volatile*/ int MotorVoltage = 9; // VIN for motor controller, usually 9 volts
static /*volatile*/ int MotorMaxDuty = (1 << RTS_PWM_RES) - 1; // If 8, should be 255
static /*volatile*/ int MotorMinDuty = (MIN_OPERATING_VOLTAGE * MotorMaxDuty) / MotorVoltage;
// static volatile float MotorDutyStepPercent = 0.3f;

enum RTSMove{
    NONE = 'O',
    FORWARD = 'F',
    BACKWARD = 'B',
    LEFT = 'L',
    RIGHT = 'R'
};

void MotorSetup();

// Called on the ESP32 timer for motor update
void MotorUpdate();

// Used to issue commands to move a certain direction
void MoveDirection(RTSMove);

// Immediately brakes, stopping all motors
void StopMotors();

#endif