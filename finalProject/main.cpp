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
static class VL53L0X VL53L0X_1(&devI2c, &shutdown_pin, PC_7);
static class VL53L0X VL53L0X_2(&devI2c_2, &shutdown_pin_2, PA_4);

/* Simple main function */
int main() {
    
    Gesture_DIRSWIPE_1_Data_t gestureSwipeData;

    VL53L0X_RangingMeasurementData_t measurement_data_1;
    VL53L0X_RangingMeasurementData_t measurement_data_2;

    VL53L0X_1.init_sensor(VL53L0X_DEFAULT_ADDRESS);
    VL53L0X_2.init_sensor(VL53L0X_DEFAULT_ADDRESS);
    tof_gestures_initDIRSWIPE_1(200,0,1000,&gestureSwipeData);

    VL53L0X_1.range_start_continuous_mode(); 
    VL53L0X_2.range_start_continuous_mode(); 
    int status, gesture_code;
    DirSwipe_Info gestureInfo;

    while (true) {
        status = VL53L0X_1.get_measurement(range_continuous_polling, &measurement_data_1);

        if (status == VL53L0X_ERROR_NONE) {
            if(measurement_data_1.RangeStatus == 0){
                printf("VL53L0X_1 [mm]: %6ld\r\n", measurement_data_1.RangeMilliMeter);
            }else{
                printf("VL53L0X_1 [mm]: %6ld\r\n", 3000);
            }
        }

        status = VL53L0X_2.get_measurement(range_continuous_polling, &measurement_data_2);
        if (status == VL53L0X_ERROR_NONE) {
            if(measurement_data_2.RangeStatus == 0){
                printf("VL53L0X_2 [mm]: %6ld\r\n", measurement_data_2.RangeMilliMeter);
            }else{
                printf("VL53L0X_2 [mm]: %6ld\r\n", 3000);
            }
        }

        gestureInfo = tof_gestures_detectDIRSWIPE_1(measurement_data_1.RangeMilliMeter,measurement_data_2.RangeMilliMeter,&gestureSwipeData);
        gesture_code = gestureInfo.returnCode;
        printf("swipe duration [ms]: %6ld\r\n", gestureInfo.duration);
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
                break;
        }
        printf("\033[4A");
        // ThisThread::sleep_for(30ms);
    }
}
