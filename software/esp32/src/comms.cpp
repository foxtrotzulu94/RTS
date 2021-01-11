#include "constants.h"
#include "comms.h"
#include "motors.h"

void RouteMessage(const char* cmd){
    std::string action = std::string(cmd, 4);
    if (action == "MOVD"){
        // Simple mapping, nothing fancy
        // Note that doing something like `RTSMove dir = (RTSMove)cmd[4]`
        // Results in undefined behaviour if cmd[4] contains an unmapped value.
        switch (cmd[4])
        {
        case 'F':
            MoveDirection(RTSMove::FORWARD);
            break;
        case 'B':
            MoveDirection(RTSMove::BACKWARD);
            break;
        case 'L':
            MoveDirection(RTSMove::LEFT);
            break;
        case 'R':
            MoveDirection(RTSMove::RIGHT);
            break;
        
        default:
            SerialBT.printf("Unrecognized direction %s for 'RTSMOVD'\n",cmd[4]);
            break;
        }
    }
    else if (action == "STOP"){
        StopMotors();
    }
    else if (action == "HOLA"){
        SerialBT.println("ACK");
    }
}

void HandleMessage(const char* cmd)
{
    if(cmd[0] == 'R' && cmd[1] == 'T' && cmd[2] == 'S'){
        // Remove the header and send it down
        return RouteMessage(&cmd[3]);
    }

    // Single char message testing
    switch (cmd[0])
    {
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

    // auto start = millis();
    int size = std::min((int)param->data_ind.len, BUFF_SIZE);
    // TODO: could replace with strncpy?
    for (int i = 0; i < size; i++){
        buff[i] = *((char*)(param->data_ind.data + i));
    }
    // auto end = millis();
    if(BUFF_SIZE - size > 0){
        buff[size] = '\0';
    }

    // Take the first byte and handle the message
    HandleMessage(buff);
    // SerialBT.printf("Msg: %s - time: %lu ms\n", buff, end-start);

    // Flush all comms so we don't fill the RTOS buffer
    SerialBT.flush();
}