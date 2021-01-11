#include "constants.h"
#include "motors.h"

// TODO: better state handling
struct MotorState{
    // From 0 to MotorMaxDuty, +/- indicates direction
    int LeftMotor, RightMotor;
};

void MotorSetup(){
    // Motor controller pin in
    // Currently unused
    pinMode(DRV_FAULT_IN_PIN, INPUT);

    // Enable the 4 PWM channels
    ledcSetup(RIGHT_MOTOR_CH1, RTS_PWM_FREQ, RTS_PWM_RES);
    ledcSetup(RIGHT_MOTOR_CH2, RTS_PWM_FREQ, RTS_PWM_RES);
    ledcSetup(LEFT_MOTOR_CH1, RTS_PWM_FREQ, RTS_PWM_RES);
    ledcSetup(LEFT_MOTOR_CH2, RTS_PWM_FREQ, RTS_PWM_RES);

    // attach each channel to the GPIO to be controlled
    ledcAttachPin(RIGHT_MOTOR_PWN_PIN1, RIGHT_MOTOR_CH1);
    ledcAttachPin(RIGHT_MOTOR_PWN_PIN2, RIGHT_MOTOR_CH2);
    ledcAttachPin(LEFT_MOTOR_PWN_PIN1, LEFT_MOTOR_CH1);
    ledcAttachPin(LEFT_MOTOR_PWN_PIN2, LEFT_MOTOR_CH2);

    ledcWrite(RIGHT_MOTOR_CH1, 0);
    ledcWrite(RIGHT_MOTOR_CH2, 0);
    ledcWrite(LEFT_MOTOR_CH1, 0);
    ledcWrite(LEFT_MOTOR_CH2, 0);
}

const unsigned long WatchdogTimeout = 150000;
static unsigned long LastMoveTime = 0;

void IRAM_ATTR MotorUpdate(){
    // Watchdog check!
    // if 250ms has passed without a move command, stop imediately
    if(micros() - LastMoveTime > WatchdogTimeout){
        StopMotors();
    }

    // TODO: Update state and lerp
    
}

void StopMotors(){
    ledcWrite(RIGHT_MOTOR_CH1, 0);
    ledcWrite(RIGHT_MOTOR_CH2, 0);
    ledcWrite(LEFT_MOTOR_CH1, 0);
    ledcWrite(LEFT_MOTOR_CH2, 0);
}

void MoveDirection(RTSMove direction){
    // Update to last moment

    LastMoveTime = micros();
    // TODO: update motor state
    switch (direction)
    {
    case RTSMove::FORWARD:
        ledcWrite(RIGHT_MOTOR_CH1, 0);
        ledcWrite(RIGHT_MOTOR_CH2, 255);
        ledcWrite(LEFT_MOTOR_CH1, 0);
        ledcWrite(LEFT_MOTOR_CH2, 255);
        break;

    case RTSMove::BACKWARD:
        ledcWrite(RIGHT_MOTOR_CH1, 128);
        ledcWrite(RIGHT_MOTOR_CH2, 0);
        ledcWrite(LEFT_MOTOR_CH1, 128);
        ledcWrite(LEFT_MOTOR_CH2, 0);
        break;

    case RTSMove::LEFT:
        ledcWrite(RIGHT_MOTOR_CH1, 0);
        ledcWrite(RIGHT_MOTOR_CH2, 200);
        ledcWrite(LEFT_MOTOR_CH1, 200);
        ledcWrite(LEFT_MOTOR_CH2, 0);
        break;

    case RTSMove::RIGHT:
        ledcWrite(RIGHT_MOTOR_CH1, 200);
        ledcWrite(RIGHT_MOTOR_CH2, 0);
        ledcWrite(LEFT_MOTOR_CH1, 0);
        ledcWrite(LEFT_MOTOR_CH2, 200);
        break;

    default:
        // this direction isn't recognize? stop immediately
        StopMotors();
        break;
    }
}