
#ifndef __RTS_COMMS__
#define __RTS_COMMS__

#include "BluetoothSerial.h"

// Classic Bluetooth (not BLE)
static BluetoothSerial SerialBT;

static char LAST_COMMAND[5] = "NONE";
static unsigned long LAST_COMMAND_TIME = 0;

void HandleMessage(const char cmd);
// static void BluetoothCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

#endif