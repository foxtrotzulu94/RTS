#include <Arduino.h>

#include "constants.h"
#include "comms.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

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
}