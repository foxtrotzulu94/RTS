
#ifndef __RTS_COMMS__
#define __RTS_COMMS__

#include "BluetoothSerial.h"

// Classic Bluetooth (not BLE)
static BluetoothSerial SerialBT;

void HandleMessage(const char* cmd);
void BluetoothCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

#endif