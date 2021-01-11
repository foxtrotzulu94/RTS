#include "constants.h"
#include "comms.h"
#include "motors.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

hw_timer_t * timer = NULL;

void setup() {
  Serial.begin(RTS_BAUD_RATE);
  SerialBT.begin(RTS_BT_NAME);
  SerialBT.register_callback(&BluetoothCallback);
  Serial.println("Device serial started");

  MotorSetup();
  Serial.println("PWM Enabled");

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &MotorUpdate, true);
  timerAlarmWrite(timer, 50000, true); // call every 50 ms, constantly
  timerAlarmEnable(timer);

  Serial.println("Timer Enabled");
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
  // if (SerialBT.available()) {
  //   // Do the thing
  //   HandleMessage(SerialBT.read());
  // }
}