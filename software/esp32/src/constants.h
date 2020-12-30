// RTS Device Constants
const int RTS_BAUD_RATE = 115200;
const char* RTS_BT_NAME = "ShermanToyTank";
const int RTS_PWM_FREQ  = 5000; // 5KHz
const int RTS_PWM_RES   = 8;    // 8-bit resolution, 0 to 255
const int RTS_TIME_DIV  = 80;

const int RTS_WATCHDOG_TIMER = 0;
const int RTS_WATCHDOG_PERIOD= 33000; // 33ms

// PWM Channel Assignments
const uint8_t TEST_PWM = 0;
const uint8_t RIGHT_MOTOR_CH1 = 1;
const uint8_t RIGHT_MOTOR_CH2 = 2;
const uint8_t LEFT_MOTOR_CH1= 3;
const uint8_t LEFT_MOTOR_CH2= 4;
const uint8_t TURRET_CH = 5;

// GPIO Assignments
// NOTE: On our dev board, pins 35, 34, 39, 36 seem busted.
const uint8_t BOOT_BUTTON = 0;
const uint8_t TEST_PWM_PIN = 33;
const uint8_t DRV_FAULT_IN_PIN = 26;
const uint8_t RIGHT_MOTOR_PWN_PIN1 = 27; //AIN1
const uint8_t RIGHT_MOTOR_PWN_PIN2 = 14; //AIN2
const uint8_t LEFT_MOTOR_PWN_PIN1 = 12;  //BIN1
const uint8_t LEFT_MOTOR_PWN_PIN2 = 13;  //BIN2
const uint8_t TURRET_PWN_PIN = 25; // Currently unused...
