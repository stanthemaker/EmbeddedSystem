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
// #include "HTS221Sensor.h"
// #include "LPS22HBSensor.h"
// #include "LSM6DSLSensor.h"

#ifdef TARGET_DISCO_L475VG_IOT01A
#include "VL53L0X.h"

#else // Nucleo-XXX + X-Nucleo-IKS01A2 or SensorTile

#ifdef TARGET_NUCLEO_L476RG
#define TARGET_SENSOR_TILE   // comment out to use actual NUCLEO-L476RG instead of SensorTile
#endif

#include "LSM303AGRMagSensor.h"
#include "LSM303AGRAccSensor.h"

#endif


/* Retrieve the composing elements of the expansion board */

/* Interface definition */
#ifdef TARGET_DISCO_L475VG_IOT01A
static DevI2C devI2c(PB_11,PB_10);
#else // X-Nucleo-IKS01A2 or SensorTile
#ifdef TARGET_SENSOR_TILE
#define SPI_TYPE_LPS22HB   LPS22HBSensor::SPI3W
#define SPI_TYPE_LSM6DSL   LSM6DSLSensor::SPI3W
SPI devSPI(PB_15, NC, PB_13);  // 3-wires SPI on SensorTile  
static Serial ser(PC_12,PD_2); // Serial with SensorTile Cradle Exp. Board + Nucleo   
#define printf(...) ser.printf(__VA_ARGS__)     
#else  // Nucleo-XXX + X-Nucleo-IKS01A2 
static DevI2C devI2c(D14,D15);
#endif
#endif

// /* Environmental sensors */
// #ifdef TARGET_SENSOR_TILE
// static LPS22HBSensor press_temp(&devSPI, PA_3, NC, SPI_TYPE_LPS22HB); 
// #else  // Nucleo-XXX + X-Nucleo-IKS01A2 or B-L475E-IOT01A2
// static LPS22HBSensor press_temp(&devI2c);
// static HTS221Sensor hum_temp(&devI2c);
// #endif

/* Motion sensors */
// #ifdef TARGET_DISCO_L475VG_IOT01A
// // static LSM6DSLSensor acc_gyro(&devI2c,LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW,PD_11); // low address
// // static LIS3MDL magnetometer(&devI2c);
// #else // X-NUCLEO-IKS01A2 or SensorTile
// #if defined (TARGET_SENSOR_TILE)
// static LSM6DSLSensor acc_gyro(&devSPI,PB_12, NC, PA_2, SPI_TYPE_LSM6DSL); 
// static LSM303AGRMagSensor magnetometer(&devSPI, PB_1);
// static LSM303AGRAccSensor accelerometer(&devSPI, PC_4);
// #else
// static LSM6DSLSensor acc_gyro(&devI2c,LSM6DSL_ACC_GYRO_I2C_ADDRESS_HIGH,D4,D5); // high address
// static LSM303AGRMagSensor magnetometer(&devI2c);
// static LSM303AGRAccSensor accelerometer(&devI2c);
// #endif
// #endif

/* Range sensor - B-L475E-IOT01A2 only */
#ifdef TARGET_DISCO_L475VG_IOT01A
static DigitalOut shutdown_pin(PC_6);
static VL53L0X range(&devI2c, &shutdown_pin, PC_7);
#endif

/* Simple main function */
int main() {
  uint8_t id;
  float value1, value2;
//  char buffer1[32], buffer2[32];
  int32_t axes[3];
  
  
// #ifdef TARGET_DISCO_L475VG_IOT01A
  range.init_sensor(VL53L0X_DEFAULT_ADDRESS);
// #else // X-NUCLEO-IKS01A2 or SensorTile
//   accelerometer.init(NULL);
// #endif/

  /* Enable all sensors */
// #ifndef TARGET_SENSOR_TILE  
//   hum_temp.enable();
// #endif  
//   press_temp.enable();
// #ifndef TARGET_DISCO_L475VG_IOT01A // X-NUCLEO-IKS01A2
//   magnetometer.enable();
//   accelerometer.enable();
// #endif
//   acc_gyro.enable_x();
//   acc_gyro.enable_g();
  
  printf("\033[2J\033[20A");
  printf ("\r\n--- Starting new run ---\r\n\r\n");

// #ifndef TARGET_SENSOR_TILE
//   hum_temp.read_id(&id);
//   printf("HTS221  humidity & temperature    = 0x%X\r\n", id);
// #endif  
//   press_temp.read_id(&id);
//   printf("LPS22HB pressure & temperature    = 0x%X\r\n", id);
//   magnetometer.read_id(&id);
// #ifdef TARGET_DISCO_L475VG_IOT01A
//   printf("LIS3MDL magnetometer              = 0x%X\r\n", id);
// #else // X-NUCLEO-IKS01A2 or SensorTile
//   printf("LSM303AGR magnetometer            = 0x%X\r\n", id);
//   accelerometer.read_id(&id);
//   printf("LSM303AGR accelerometer           = 0x%X\r\n", id); 
// #endif
//   acc_gyro.read_id(&id);
//   printf("LSM6DSL accelerometer & gyroscope = 0x%X\r\n", id);
  
  printf("\n\r--- Reading sensor values ---\n\r"); ;
 
  while(1) {
    printf("\r\n");

// #ifndef TARGET_SENSOR_TILE    
//     hum_temp.get_temperature(&value1);
//     hum_temp.get_humidity(&value2);
//     printf("HTS221:  [temp] %.2f C, [hum]   %.2f%%\r\n", value1, value2);
// #endif    
//     value1=value2=0;    
//     press_temp.get_temperature(&value1);
//     press_temp.get_pressure(&value2);
//     printf("LPS22HB: [temp] %.2f C, [press] %.2f mbar\r\n", value1, value2);

//     printf("---\r\n");

//     magnetometer.get_m_axes(axes);
// #ifdef TARGET_DISCO_L475VG_IOT01A
//     printf("LIS3MDL [mag/mgauss]:    %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
// #else // X-NUCLEO-IKS01A2 or SensorTile
//     printf("LSM303AGR [mag/mgauss]:  %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);
//     accelerometer.get_x_axes(axes);
//     printf("LSM303AGR [acc/mg]:      %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);    
// #endif    

//     acc_gyro.get_x_axes(axes);
//     printf("LSM6DSL [acc/mg]:        %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);

//     acc_gyro.get_g_axes(axes);
//     printf("LSM6DSL [gyro/mdps]:     %6ld, %6ld, %6ld\r\n", axes[0], axes[1], axes[2]);

#ifdef TARGET_DISCO_L475VG_IOT01A
    uint32_t distance;
    int status = range.get_distance(&distance);
    if (status == VL53L0X_ERROR_NONE) {
        printf("VL53L0X [mm]:            %6ld\r\n", distance);
    } else {
        printf("VL53L0X [mm]:                --\r\n");
    }
#endif

#if defined (TARGET_SENSOR_TILE)  
    printf("\033[7A");
#else    
    printf("\033[8A");
#endif
    ThisThread::sleep_for(5s);
  }
}
