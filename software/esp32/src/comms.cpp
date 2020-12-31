#include "constants.h"
#include "comms.h"

void HandleMessage(const char cmd)
{
    // Single char message (for now)
    switch (cmd)
    {
    case 'B':
        ledcWrite(RIGHT_MOTOR_CH1, 128);
        ledcWrite(RIGHT_MOTOR_CH2, 0);
        ledcWrite(LEFT_MOTOR_CH1, 128);
        ledcWrite(LEFT_MOTOR_CH2, 0);
        break;

    case 'F': // Forward
        ledcWrite(RIGHT_MOTOR_CH1, 0);
        ledcWrite(RIGHT_MOTOR_CH2, 255);
        ledcWrite(LEFT_MOTOR_CH1, 0);
        ledcWrite(LEFT_MOTOR_CH2, 255);
        break;

    case 'L':
        ledcWrite(RIGHT_MOTOR_CH1, 0);
        ledcWrite(RIGHT_MOTOR_CH2, 128);
        ledcWrite(LEFT_MOTOR_CH1, 64);
        ledcWrite(LEFT_MOTOR_CH2, 0);
        break;

    case 'R':
        ledcWrite(RIGHT_MOTOR_CH1, 64);
        ledcWrite(RIGHT_MOTOR_CH2, 0);
        ledcWrite(LEFT_MOTOR_CH1, 0);
        ledcWrite(LEFT_MOTOR_CH2, 128);
        break;

    case 'S':
        ledcWrite(RIGHT_MOTOR_CH1, 0);
        ledcWrite(RIGHT_MOTOR_CH2, 0);
        ledcWrite(LEFT_MOTOR_CH1, 0);
        ledcWrite(LEFT_MOTOR_CH2, 0);
        break;

    case 'O':
        ledcWrite(RIGHT_MOTOR_CH1, 255);
        ledcWrite(RIGHT_MOTOR_CH2, 255);
        ledcWrite(LEFT_MOTOR_CH1, 255);
        ledcWrite(LEFT_MOTOR_CH2, 255);
        break;

    case 'T':
        SerialBT.println("Testing underway");
        ledcWrite(RIGHT_MOTOR_CH1, 0);
        ledcWrite(RIGHT_MOTOR_CH2, 0);
        ledcWrite(LEFT_MOTOR_CH1, 0);
        ledcWrite(LEFT_MOTOR_CH2, 0);

        for (int i = 0; i <= 255; ++i)
        {
            SerialBT.printf("Duty = %i\n", (100 * i) / 255);
            ledcWrite(RIGHT_MOTOR_CH2, i);
            ledcWrite(LEFT_MOTOR_CH2, i);
            delay(50);
        }
        SerialBT.println("TEST complete");
        ledcWrite(LEFT_MOTOR_CH2, 0);
        ledcWrite(RIGHT_MOTOR_CH2, 0);

        break;

    case 'W':
        SerialBT.printf("Time: %lu\n", millis());
        break;

    default:
        break;
    }

    // Copy last command
    LAST_COMMAND[0] = cmd;
    strncpy(&LAST_COMMAND[1], "", 3);
}