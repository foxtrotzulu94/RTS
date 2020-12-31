#include <Arduino.h>

#include "BluetoothSerial.h"
#include "constants.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Classic Bluetooth (not BLE)
BluetoothSerial SerialBT;

hw_timer_t * timer = NULL;

void setupPWM(){
  // Enable the 4 PWM channels
  // ledcSetup(TEST_PWM, RTS_PWM_FREQ, RTS_PWM_RES);
  ledcSetup(RIGHT_MOTOR_CH1, RTS_PWM_FREQ, RTS_PWM_RES);
  ledcSetup(RIGHT_MOTOR_CH2, RTS_PWM_FREQ, RTS_PWM_RES);
  ledcSetup(LEFT_MOTOR_CH1, RTS_PWM_FREQ, RTS_PWM_RES);
  ledcSetup(LEFT_MOTOR_CH2, RTS_PWM_FREQ, RTS_PWM_RES);
  
  // attach each channel to the GPIO to be controlled
  // ledcAttachPin(TEST_PWM_PIN, TEST_PWM);
  ledcAttachPin(RIGHT_MOTOR_PWN_PIN1, RIGHT_MOTOR_CH1);
  ledcAttachPin(RIGHT_MOTOR_PWN_PIN2, RIGHT_MOTOR_CH2);
  ledcAttachPin(LEFT_MOTOR_PWN_PIN1, LEFT_MOTOR_CH1);
  ledcAttachPin(LEFT_MOTOR_PWN_PIN2, LEFT_MOTOR_CH2);

  ledcWrite(RIGHT_MOTOR_CH1, 0);
  ledcWrite(RIGHT_MOTOR_CH2, 0);
  ledcWrite(LEFT_MOTOR_CH1, 0);
  ledcWrite(LEFT_MOTOR_CH2, 0);

  Serial.println("PWM Enabled");
}

void setup() {
  Serial.begin(RTS_BAUD_RATE);
  SerialBT.begin(RTS_BT_NAME);
  Serial.println("Device serial started");

  // Motor controller pin in
  pinMode(DRV_FAULT_IN_PIN, INPUT);

  setupPWM();

  // timer = timerBegin(0, 80, true);
  // timerAttachInterrupt(timer, &playWithLED, true);
  // timerAlarmWrite(timer, 15000, true); // 15 ms
  // timerAlarmEnable(timer);

  // Serial.println("Timer Enabled");
  Serial.println("Device Ready");
}

static char LAST_CMD = 'X';
const int MAX = 255;

static int stepRMCH1 = 0;
static int stepRMCH2 = 0;
static int stepLMCH1 = 0;
static int stepLMCH2 = 0;
static int valRMCH1 = 0;
static int valRMCH2 = 0;
static int valLMCH1 = 0;
static int valLMCH2 = 0;

void HandleMessage(const char cmd){
  // Single char message (for now)
  switch (cmd)
  {
  case 'B':
    ledcWrite(RIGHT_MOTOR_CH1, 128);
    ledcWrite(RIGHT_MOTOR_CH2, 0);
    ledcWrite(LEFT_MOTOR_CH1, 128);
    ledcWrite(LEFT_MOTOR_CH2, 0);
    stepRMCH1 = 1;
    stepRMCH2 = 0;
    stepLMCH1 = 1;
    stepLMCH2 = 0;

    valRMCH1 = 128;
    valRMCH2 = 0;
    valLMCH1 = 128;
    valLMCH2 = 0;
    break;

  case 'F': // Forward
    ledcWrite(RIGHT_MOTOR_CH1, 0);
    ledcWrite(RIGHT_MOTOR_CH2, 255);
    ledcWrite(LEFT_MOTOR_CH1, 0);
    ledcWrite(LEFT_MOTOR_CH2, 255);
    break;

  case 'L':
    ledcWrite(RIGHT_MOTOR_CH1, 255);
    ledcWrite(RIGHT_MOTOR_CH2, 250);
    ledcWrite(LEFT_MOTOR_CH1, 250);
    ledcWrite(LEFT_MOTOR_CH2, 255);
    break;

  case 'R':
    ledcWrite(RIGHT_MOTOR_CH1, 250);
    ledcWrite(RIGHT_MOTOR_CH2, 255);
    ledcWrite(LEFT_MOTOR_CH1, 255);
    ledcWrite(LEFT_MOTOR_CH2, 250);
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

    for (int i=0; i<=255; ++i){
      SerialBT.printf("Duty = %i\n", (100 * i) / 255);
      ledcWrite(RIGHT_MOTOR_CH2, i);
      ledcWrite(LEFT_MOTOR_CH2, i);
      delay(50);
    }
    SerialBT.println("TEST complete");
    ledcWrite(LEFT_MOTOR_CH2, 0);
    ledcWrite(RIGHT_MOTOR_CH2, 0);
    
    break;

  default:
    break;
  }

  LAST_CMD = cmd;
}

static unsigned long nextUpdate = 0;
void updateCMD(){
  if(LAST_CMD != 'B' || nextUpdate > millis()){
    return;
  }

  valRMCH1 = valRMCH1 >= MAX? MAX : valRMCH1 + 1;
  valLMCH1 = valLMCH1 >= MAX? MAX : valLMCH1 + 1;
  ledcWrite(RIGHT_MOTOR_CH1, valRMCH1);
  ledcWrite(RIGHT_MOTOR_CH2, 0);
  ledcWrite(LEFT_MOTOR_CH1, valLMCH1);
  ledcWrite(LEFT_MOTOR_CH2, 0);

  nextUpdate = millis() + 50;
}

// Our DRV8833 has a fault pin. It'll be driven low in the event of an actual fault
static int DRV_FAULT = -1;

// Check DRV State - Unused (we don't have a pull up resistor for this)
void checkDRVFault(){
    int curr_state = digitalRead(DRV_FAULT_IN_PIN);
    if(curr_state != DRV_FAULT){
      Serial.printf("DRV STATE: %s", curr_state==1?"No issues":"A Fault has ocurred!!!");
      DRV_FAULT = curr_state;
    }
}

void loop() {
  if (SerialBT.available()) {
    // Do the thing
    HandleMessage(SerialBT.read());
  }
  // updateCMD();
}