#include <Arduino.h>

#include "BluetoothSerial.h"
#include "constants.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Classic Bluetooth (not BLE)
BluetoothSerial SerialBT;

hw_timer_t * timer = NULL;

void playWithLED(){
  const int min = 0, max = 255;
  static int step = +1;
  static int dutyCycle = 1;

  if(dutyCycle == 255 || dutyCycle == 0){
    step *= -1;
  }

  dutyCycle += step;
  ledcWrite(TEST_PWM, dutyCycle);
}

void setup() {
  Serial.begin(RTS_BAUD_RATE);
  SerialBT.begin(RTS_BT_NAME);
  Serial.println("Device serial started");

  // Setup PWM
  // configure LED PWM functionalitites
  ledcSetup(TEST_PWM, RTS_PWM_FREQ, RTS_PWM_RES);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(TEST_PWM_PIN, TEST_PWM);

  Serial.println("PWM Enabled");

  timer = timerBegin(0, 80000, true);
  timerAttachInterrupt(timer, &playWithLED, true);
  timerAlarmWrite(timer, 15, true); // 15 ms
  timerAlarmEnable(timer);

  Serial.println("Timer Enabled");
}

void loop() {
  // // increase the LED brightness
  // for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
  //   // changing the LED brightness with PWM
  //   ledcWrite(TEST_PWM, dutyCycle);
  //   delay(15);
  // }

  // // decrease the LED brightness
  // for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
  //   // changing the LED brightness with PWM
  //   ledcWrite(TEST_PWM, dutyCycle);   
  //   delay(15);
  // }
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }

  delay(20);
}