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
#include <tof_gestures_SWIPE_1.h>
#include "VL53L0X.h"

/* Retrieve the composing elements of the expansion board */

/* Interface definition */
static DevI2C devI2c(PB_11,PB_10);

/* Range sensor - B-L475E-IOT01A2 only */
static DigitalOut shutdown_pin(PC_6);
static VL53L0X VL53L0X(&devI2c, &shutdown_pin, PC_7);

/* Simple main function */
int main() {
    // Gesture structure.
    Gesture_SWIPE_1_Data_t gestureSwipeData;
    // Range value
    uint32_t distance;
    uint16_t detected_num = 0;
  
    VL53L0X.init_sensor(VL53L0X_DEFAULT_ADDRESS);
    tof_gestures_initSWIPE_1(&gestureSwipeData);
  
    printf("\n\r--- Reading sensor values ---\n\r"); ;
 
    while(1) {
        int gesture_code;
        int status;

        status = VL53L0X.get_distance(&distance);
        if (status != VL53L0X_ERROR_NONE) {
            distance = 1200;   
        }
        gesture_code = tof_gestures_detectSWIPE_1(distance, &gestureSwipeData);
        switch(gesture_code)
        {
            case GESTURES_SINGLE_SWIPE:
                detected_num ++;
                break;
            default:
            break;
        }
        printf("VL53L0X [mm]:            %6ld, detected: %d \r\n", distance, detected_num);

        printf("\033[8A");
        
        ThisThread::sleep_for(10);
    }
}
