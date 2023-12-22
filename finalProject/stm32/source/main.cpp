/* Sockets Example
 * Copyright (c) 2016-2020 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "mbed-trace/mbed_trace.h"
#include "socket.h"
#include <cstdio>
#include <cstring>
#include <tof_gestures.h>
#include <tof_gestures_DIRSWIPE_1.h>
#undef TRACE_LEVEL_INFO
#undef TRACE_LEVEL_DEBUG
#include "VL53L0X.h"
#include <string>

/* Interface definition */
static DevI2C devI2c(PB_11,PB_10);
static DevI2C devI2c_2(D14,D15);

/* Range sensor - B-L475E-IOT01A2 only */
static DigitalOut shutdown_pin(PC_6);
static DigitalOut shutdown_pin_2(PB_2);
static class VL53L0X VL53L0X_1(&devI2c, &shutdown_pin, PC_7);
static class VL53L0X VL53L0X_2(&devI2c_2, &shutdown_pin_2, PA_4);

SocketDemo *socket;

/* multithreading */
EventQueue queue(32 * EVENTS_EVENT_SIZE); 
Thread sensorThread, socketThread;
// std::queue<std::string> shared_queue;
const int POOL_SIZE = 16;
MemoryPool<string, POOL_SIZE> mem_pool;
Queue<string, POOL_SIZE> shared_queue;

void httpRequestTask() {
    while (true) {
        osEvent evt = shared_queue.get();
        if (evt.status == osEventMessage) {
            
            string* message = (string*) evt.value.p;

            socket->send_http_request(message->c_str());
            // socket->receive_http_response
            mem_pool.free(message);
        }
    }
}

void sensorTask() {
    // Continuously measure and detect gestures
    Gesture_DIRSWIPE_1_Data_t gestureSwipeData;

    VL53L0X_RangingMeasurementData_t measurement_data_1;
    VL53L0X_RangingMeasurementData_t measurement_data_2;

    VL53L0X_1.init_sensor(VL53L0X_DEFAULT_ADDRESS);
    VL53L0X_2.init_sensor(VL53L0X_DEFAULT_ADDRESS);
    tof_gestures_initDIRSWIPE_1(200,0,1000,&gestureSwipeData);

    VL53L0X_1.range_start_continuous_mode(); 
    VL53L0X_2.range_start_continuous_mode(); 
    int status, gesture_code;
    bool TAP_flag = 0;
    DirSwipe_Info gestureInfo;
    // If a meaningful gesture is detected, add the HTTP request task to the queue

    while (true) {
        status = VL53L0X_1.get_measurement(range_continuous_polling, &measurement_data_1);

        // if (status == VL53L0X_ERROR_NONE) {
        //     if(measurement_data_1.RangeStatus == 0){
        //         printf("VL53L0X_1 [mm]: %6ld\r\n", measurement_data_1.RangeMilliMeter);
        //     }else{
        //         printf("VL53L0X_1 [mm]: %6ld\r\n", 3000);
        //     }
        // }

        status = VL53L0X_2.get_measurement(range_continuous_polling, &measurement_data_2);
        // if (status == VL53L0X_ERROR_NONE) {
        //     if(measurement_data_2.RangeStatus == 0){
        //         printf("VL53L0X_2 [mm]: %6ld\r\n", measurement_data_2.RangeMilliMeter);
        //     }else{
        //         printf("VL53L0X_2 [mm]: %6ld\r\n", 3000);
        //     }
        // }

        gestureInfo = tof_gestures_detectDIRSWIPE_1(measurement_data_1.RangeMilliMeter,measurement_data_2.RangeMilliMeter,&gestureSwipeData);
        gesture_code = gestureInfo.returnCode;
        // printf("swipe duration [ms]: %6ld\r\n", gestureInfo.duration);
        switch(gesture_code)
        {
            case GESTURES_SWIPE_LEFT_RIGHT: {
                string* message = mem_pool.try_alloc();
                char buff[20];
                snprintf(buff, sizeof(buff), "%ld", gestureInfo.duration);
                message->assign(buff);
                shared_queue.try_put(message);
 
                break;
            }
            case GESTURES_SWIPE_RIGHT_LEFT: {
                string* message = mem_pool.try_alloc();
                char buff[20];
                snprintf(buff, sizeof(buff), "-%ld", gestureInfo.duration);
                message->assign(buff);
                shared_queue.try_put(message);

                break;

            }
            case GESTURES_DISCARDED_TOO_FAST: {
                printf(" too fast\r\n");
                break;
            }
            case GESTURES_DISCARDED_TOO_SLOW: {
                printf(" too slow\r\n");
                break;
            }
            case GESTURES_SINGLE_TAP: {
                string* message = mem_pool.try_alloc();
                message->assign("0");
                shared_queue.try_put(message);
                if (TAP_flag){
                    printf(" !!!!!!!!\r\n");
                }else {
                    printf(" ????????\r\n");
                }
                TAP_flag = !TAP_flag;
                break;
            }
            default:
                break;
        }
        // printf("\033[4A");
        // ThisThread::sleep_for(30ms);
    }
}



int main() {
    printf("\r\n program start \r\n\r\n");

    socket = new SocketDemo();
    MBED_ASSERT(socket);
    socket->run();
    // socket->send_http_request("424");
    // socket->is_socket_closed();
    // socket->send_http_request("211");
    sensorThread.start(sensorTask);

    httpRequestTask();
    return 0;
}
