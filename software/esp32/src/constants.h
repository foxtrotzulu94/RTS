// RTS Device Constants
const int RTS_BAUD_RATE = 115200;
const char* RTS_BT_NAME = "ShermanToyTank";
const int RTS_PWM_FREQ  = 5000; // 5KHz
const int RTS_PWM_RES   = 8;    // 8-bit resolution, 0 to 255

// PWM Channel Assignments
const int TEST_PWM = 0;
const int RIGHT_MOTOR_CH = 1;
const int LEFT_MOTOR_CH = 2;
const int TURRET_CH = 2;

// GPIO Assignments
const int TEST_PWM_PIN = 13;
const int LEFT_MOTOR_PWN_PIN = 27;
const int RIGHT_MOTOR_PWN_PIN = 26;
const int TURRET_PWN_PIN = 25;
