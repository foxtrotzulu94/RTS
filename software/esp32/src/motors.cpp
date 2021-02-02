#include "constants.h"
#include "motors.h"

// TODO: better state handling
struct{
    // From 0 to MotorMaxDuty, +/- indicates direction
    int Left, Right;
    bool isLeftForward() { return Left >=0; }
    bool isRightForward() { return Right >=0; }
} MotorState;

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

static RTSMove lastDirection = RTSMove::NONE;

int lerp(int v0, int v1, float t) {
    return (1 - t) * v0 + t * v1;
}

void MotorDriveUpdate(){
    // Increase speed but keep direction
    // TODO: Full piecewise -255 to 0 then 0 to 255. Lerp does not have continuity
    const bool isLeftForward = MotorState.isLeftForward();
    const int leftMax = isLeftForward? MotorMaxDuty : -MotorMaxDuty;
    MotorState.Left = lerp(MotorState.Left, leftMax, MotorDutyStepPercent);

    const bool isRightForward = MotorState.isRightForward();
    const int rightMax = isRightForward? MotorMaxDuty : -MotorMaxDuty;
    MotorState.Right = lerp(MotorState.Right, rightMax, MotorDutyStepPercent);

    // Execute engine update
    // CH1 channel is backwards, and CH2 is forward
    ledcWrite(MotorState.Left < 0? LEFT_MOTOR_CH1 : LEFT_MOTOR_CH2, 
        std::min(MotorMaxDuty, std::abs(MotorState.Left)));

    ledcWrite(MotorState.Right < 0? RIGHT_MOTOR_CH1 : RIGHT_MOTOR_CH2, 
        std::min(MotorMaxDuty, std::abs(MotorState.Right)));
}

void IRAM_ATTR MotorUpdate(){
    // Watchdog check!
    // if 250ms has passed without a move command, stop imediately
    if(micros() - LastMoveTime > WatchdogTimeout){
        StopMotors();
        return;
    }
}

void StopMotors(){
    ledcWrite(RIGHT_MOTOR_CH1, 0);
    ledcWrite(RIGHT_MOTOR_CH2, 0);
    ledcWrite(LEFT_MOTOR_CH1, 0);
    ledcWrite(LEFT_MOTOR_CH2, 0);
    lastDirection = RTSMove::NONE;
}

void ChangeDirection(RTSMove direction){
    switch (direction)
    {
    case RTSMove::FORWARD:
        ledcWrite(RIGHT_MOTOR_CH1, 0);
        ledcWrite(RIGHT_MOTOR_CH2, MotorMinDuty);
        ledcWrite(LEFT_MOTOR_CH1, 0);
        ledcWrite(LEFT_MOTOR_CH2, MotorMinDuty);
        MotorState.Left = MotorState.Right = MotorMinDuty;
        break;

    case RTSMove::BACKWARD:
        ledcWrite(RIGHT_MOTOR_CH1, MotorMinDuty);
        ledcWrite(RIGHT_MOTOR_CH2, 0);
        ledcWrite(LEFT_MOTOR_CH1, MotorMinDuty);
        ledcWrite(LEFT_MOTOR_CH2, 0);
        MotorState.Left = MotorState.Right = -MotorMinDuty;
        break;

    case RTSMove::LEFT:
        ledcWrite(RIGHT_MOTOR_CH1, 0);
        ledcWrite(RIGHT_MOTOR_CH2, MotorMinDuty);
        ledcWrite(LEFT_MOTOR_CH1, MotorMinDuty);
        ledcWrite(LEFT_MOTOR_CH2, 0);
        MotorState.Right = MotorMinDuty;
        MotorState.Left = -MotorMinDuty;
        break;

    case RTSMove::RIGHT:
        ledcWrite(RIGHT_MOTOR_CH1, MotorMinDuty);
        ledcWrite(RIGHT_MOTOR_CH2, 0);
        ledcWrite(LEFT_MOTOR_CH1, 0);
        ledcWrite(LEFT_MOTOR_CH2, MotorMinDuty);
        MotorState.Right = -MotorMinDuty;
        MotorState.Left = MotorMinDuty;
        break;

    default:
        // this direction isn't recognize? stop immediately
        StopMotors();
        break;
    }
}

void MoveDirection(RTSMove direction){
    // Update to last moment

    LastMoveTime = micros();
    
    if (lastDirection != direction){
        ChangeDirection(direction);
        lastDirection = direction;
    }else{    
        // Otherwise, update the motor state
        MotorDriveUpdate();
    }
}