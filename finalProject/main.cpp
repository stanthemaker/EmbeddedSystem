/**
 ******************************************************************************
 * @file    main.cpp
 * @author  CLab
 * @version V1.0.0
 * @date    5-September-2017
 * @brief   Simple Example application for using X_NUCLEO_IKS01A2  
 *          MEMS Inertial & Environmental Sensor Nucleo expansion and 
 *          B-L475E-IOT01A2 boards.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
*/ 

/* Includes */
#include "mbed.h"
#include <tof_gestures.h>
// #include <tof_gestures_DIRSWIPE_1.h>
#include <tof_gestures_DIRSWIPE_1.h>
#include "VL53L0X.h"

/* Retrieve the composing elements of the expansion board */

/* Interface definition */
static DevI2C devI2c(PB_11,PB_10);
static DevI2C devI2c_2(D14,D15);

/* Range sensor - B-L475E-IOT01A2 only */
static DigitalOut shutdown_pin(PC_6);
static DigitalOut shutdown_pin_2(PB_2);
static class VL53L0X VL53L0X(&devI2c, &shutdown_pin, PC_7);
static class VL53L0X VL53L0X_2(&devI2c_2, &shutdown_pin_2, PA_4);

/* Simple main function */
int main() {
    // Gesture structure.
    Gesture_DIRSWIPE_1_Data_t gestureSwipeData;
    // Range value
    uint32_t distance, distance_2;
    uint16_t detected_num = 0;
    VL53L0X.init_sensor(VL53L0X_DEFAULT_ADDRESS);
    VL53L0X_2.init_sensor(VL53L0X_DEFAULT_ADDRESS);
    
    tof_gestures_initDIRSWIPE_1(400,1,1500,&gestureSwipeData); //max duration = 1.5s
    // tof_gestures_initSWIPE_1(&gestureSwipeData);
  
    printf("--- Reading sensor values ---\n"); ;
 
    while(1) {
        int gesture_code;
        int status;

        status = VL53L0X.get_distance(&distance);
        if (status != VL53L0X_ERROR_NONE) {
            distance = 3000; 
        }
        printf("VL53L0X_1 [mm]: %6ld \r\n", distance);
        status = VL53L0X_2.get_distance(&distance_2);
        if (status != VL53L0X_ERROR_NONE) {
            distance = 3000; 
        }        
        printf("VL53L0X_2 [mm]: %6ld\r\n", distance_2);
    
        gesture_code = tof_gestures_detectDIRSWIPE_1(distance,distance_2,&gestureSwipeData);
        switch(gesture_code)
        {
            case GESTURES_SWIPE_LEFT_RIGHT:
                printf(" ->->->->\r\n");
                break;

            case GESTURES_SWIPE_RIGHT_LEFT:
                printf(" <-<-<-<-\r\n");
                break;
            case GESTURES_DISCARDED_TOO_FAST:
                printf(" too fast\r\n");
                break;
            case GESTURES_DISCARDED_TOO_SLOW:
                printf(" too slow\r\n");
                break;
            default:
                // printf(" ---------- \r\n");
            break;
        }
        
        printf("\033[3A");
        
        // ThisThread::sleep_for(10);
    }
}
