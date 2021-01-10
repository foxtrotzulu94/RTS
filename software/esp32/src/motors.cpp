#include "constants.h"
#include "motors.h"

// TODO: better state handling
struct MotorState{
    // From 0 to MotorMaxDuty, +/- indicates direction
    int LeftMotor, RightMotor;
};

void MotorUpdate(){
    // TODO: Update state
}

void StopMotors(){
    ledcWrite(RIGHT_MOTOR_CH1, 0);
    ledcWrite(RIGHT_MOTOR_CH2, 0);
    ledcWrite(LEFT_MOTOR_CH1, 0);
    ledcWrite(LEFT_MOTOR_CH2, 0);
}

void MoveDirection(RTSMove direction){
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