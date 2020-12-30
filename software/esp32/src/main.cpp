#include <Arduino.h>

#include "BluetoothSerial.h"
#include "constants.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Classic Bluetooth (not BLE)
BluetoothSerial SerialBT;

hw_timer_t * timer = NULL;

void Log(const char* msg){
  Serial.println(msg);
  SerialBT.println(msg);
}

void playWithLED(){
  const int min = 0, max = 255;
  static int step = +1;
  static int dutyCycle = 1;

  if(dutyCycle == max || dutyCycle == min){
    step *= -1;
  }

  dutyCycle += step;
  ledcWrite(TEST_PWM, dutyCycle);
}

void setup() {
  Serial.begin(RTS_BAUD_RATE);
  SerialBT.begin(RTS_BT_NAME);
  Serial.println("Device serial started");

  // Motor controller pin in
  pinMode(DRV_FAULT_IN_PIN, INPUT);

  // Setup PWM
  ledcSetup(TEST_PWM, RTS_PWM_FREQ, RTS_PWM_RES);
  ledcSetup(RIGHT_MOTOR_CH1, RTS_PWM_FREQ, RTS_PWM_RES);
  ledcSetup(RIGHT_MOTOR_CH2, RTS_PWM_FREQ, RTS_PWM_RES);
  ledcSetup(LEFT_MOTOR_CH1, RTS_PWM_FREQ, RTS_PWM_RES);
  ledcSetup(LEFT_MOTOR_CH2, RTS_PWM_FREQ, RTS_PWM_RES);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(TEST_PWM_PIN, TEST_PWM);
  ledcAttachPin(RIGHT_MOTOR_PWN_PIN1, RIGHT_MOTOR_CH1);
  ledcAttachPin(RIGHT_MOTOR_PWN_PIN2, RIGHT_MOTOR_CH2);
  ledcAttachPin(LEFT_MOTOR_PWN_PIN1, LEFT_MOTOR_CH1);
  ledcAttachPin(LEFT_MOTOR_PWN_PIN2, LEFT_MOTOR_CH2);

  Serial.println("PWM Enabled");

  // timer = timerBegin(0, 80, true);
  // timerAttachInterrupt(timer, &playWithLED, true);
  // timerAlarmWrite(timer, 15000, true); // 15 ms
  // timerAlarmEnable(timer);

  // Serial.println("Timer Enabled");
  Serial.println("Device Ready");
}

void HandleMessage(const char* msg){
  // Single char message (hopefully)
  char cmd = msg[0];
  switch (cmd)
  {
  case 'F':
    Log("Forward");
    ledcWrite(RIGHT_MOTOR_CH1, 255);
    ledcWrite(RIGHT_MOTOR_CH2, 0);
    ledcWrite(LEFT_MOTOR_CH1, 255);
    ledcWrite(LEFT_MOTOR_CH2, 0);
    break;
  case 'B':
    Log("Back");
    ledcWrite(RIGHT_MOTOR_CH1, 0);
    ledcWrite(RIGHT_MOTOR_CH2, 255);
    ledcWrite(LEFT_MOTOR_CH1, 0);
    ledcWrite(LEFT_MOTOR_CH2, 255);
    break;
  case 'L':
    Log("LEFT");
    ledcWrite(RIGHT_MOTOR_CH1, 0);
    ledcWrite(RIGHT_MOTOR_CH2, 0);
    ledcWrite(LEFT_MOTOR_CH1, 0);
    ledcWrite(LEFT_MOTOR_CH2, 0);
    break;
  case 'R':
    Log("RIGHT");
    ledcWrite(RIGHT_MOTOR_CH1, 0);
    ledcWrite(RIGHT_MOTOR_CH2, 0);
    ledcWrite(LEFT_MOTOR_CH1, 0);
    ledcWrite(LEFT_MOTOR_CH2, 0);
    break;

  case 'S':
    Log("STOP");
    ledcWrite(RIGHT_MOTOR_CH1, 0);
    ledcWrite(RIGHT_MOTOR_CH2, 0);
    ledcWrite(LEFT_MOTOR_CH1, 0);
    ledcWrite(LEFT_MOTOR_CH2, 0);

  default:
    break;
  }
}

// Our DRV8833 has a fault pin. It'll be driven low in the event of an actual fault
static int DRV_FAULT = -1;

void loop() {
  if (SerialBT.available()) {
    // Do the thing
    String msg = SerialBT.readString();
    Serial.println(msg);
    HandleMessage(msg.c_str());
    
    // Check DRV State
    int curr_state = digitalRead(DRV_FAULT_IN_PIN);
    if(curr_state != DRV_FAULT){
      Serial.printf("DRV STATE: %s", curr_state?"No issues":"A Fault has ocurred!!!");
      DRV_FAULT = curr_state;
    }
  }
  
  // delay(20);
}