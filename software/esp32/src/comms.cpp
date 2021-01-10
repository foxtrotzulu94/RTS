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
        ledcWrite(LEFT_MOTOR_CH1, 128);
        ledcWrite(LEFT_MOTOR_CH2, 0);
        break;

    case 'R':
        ledcWrite(RIGHT_MOTOR_CH1, 128);
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

const int BUFF_SIZE = 600;
static char buff[BUFF_SIZE];
static bool buff_init = false;
void BluetoothCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
    if(event != ESP_SPP_DATA_IND_EVT){ return; }

    if (buff_init == false){
        buff_init = true;
        for(int i=0; i<BUFF_SIZE; ++i){
            buff[i] = '\0';
        }
    }

    // By this point, SerialBT should have the bytes stored in the RTOS queue
    auto start = millis();
    int size = std::min((int)param->data_ind.len, BUFF_SIZE);
    // TODO: could replace with strncpy?
    for (int i = 0; i < size; i++){
        buff[i] = *((char*)(param->data_ind.data + i));
    }
    auto end = millis();
    if(BUFF_SIZE - size > 0){
        buff[size] = '\0';
    }

    // char cmd[5];
    // cmd[4] = '\0';
    // SerialBT.readBytes(cmd, 4);
    // Take the first byte and handle the message
    // HandleMessage(buff[0]);
    SerialBT.printf("Msg: %s - time: %lu ms\n", buff, end-start);

    // Flush all comms down the toilet
    SerialBT.flush();
}