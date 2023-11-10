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
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <string>
#include "stm32l475e_iot01_accelero.h"
#include "arm_math.h"
#include "math_helper.h"
#if defined(SEMIHOSTING)
#include <stdio.h>
#endif
/* ----------------------------------------------------------------------
** Macro Defines
** ------------------------------------------------------------------- */
#define TEST_LENGTH_SAMPLES  320
/*
This SNR is a bit small. Need to understand why
this example is not giving better SNR ...
*/
#define SNR_THRESHOLD_F32    75.0f
#define BLOCK_SIZE            32
#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
/* Must be a multiple of 16 */
#define NUM_TAPS_ARRAY_SIZE              32
#else
#define NUM_TAPS_ARRAY_SIZE              29
#endif
#define NUM_TAPS              29
#define SAMPLE_RATE          48000
/* -------------------------------------------------------------------
 * The input signal and reference output (computed with MATLAB)
 * are defined externally in arm_fir_lpf_data.c.
 * ------------------------------------------------------------------- */
/* -------------------------------------------------------------------
 * Declare Test output buffer
 * ------------------------------------------------------------------- */
static float32_t testOutput[TEST_LENGTH_SAMPLES];
/* -------------------------------------------------------------------
 * Declare State buffer of size (numTaps + blockSize - 1)
 * ------------------------------------------------------------------- */
#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
static float32_t firStateF32[2 * BLOCK_SIZE + NUM_TAPS - 1];
#else
static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1];
#endif 
/* ----------------------------------------------------------------------
** FIR Coefficients buffer generated using fir1() MATLAB function.
** fir1(28, 6/24)
** ------------------------------------------------------------------- */
#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
const float32_t firCoeffs32[NUM_TAPS_ARRAY_SIZE] = {
  -0.0018225230f, -0.0015879294f, +0.0000000000f, +0.0036977508f, +0.0080754303f, +0.0085302217f, -0.0000000000f, -0.0173976984f,
  -0.0341458607f, -0.0333591565f, +0.0000000000f, +0.0676308395f, +0.1522061835f, +0.2229246956f, +0.2504960933f, +0.2229246956f,
  +0.1522061835f, +0.0676308395f, +0.0000000000f, -0.0333591565f, -0.0341458607f, -0.0173976984f, -0.0000000000f, +0.0085302217f,
  +0.0080754303f, +0.0036977508f, +0.0000000000f, -0.0015879294f, -0.0018225230f, 0.0f,0.0f,0.0f
};
#else
const float32_t firCoeffs32[NUM_TAPS_ARRAY_SIZE] = {
  -0.0018225230f, -0.0015879294f, +0.0000000000f, +0.0036977508f, +0.0080754303f, +0.0085302217f, -0.0000000000f, -0.0173976984f,
  -0.0341458607f, -0.0333591565f, +0.0000000000f, +0.0676308395f, +0.1522061835f, +0.2229246956f, +0.2504960933f, +0.2229246956f,
  +0.1522061835f, +0.0676308395f, +0.0000000000f, -0.0333591565f, -0.0341458607f, -0.0173976984f, -0.0000000000f, +0.0085302217f,
  +0.0080754303f, +0.0036977508f, +0.0000000000f, -0.0015879294f, -0.0018225230f
};
#endif

/* ------------------------------------------------------------------
 * Global variables for FIR LPF Example
 * ------------------------------------------------------------------- */
uint32_t blockSize = BLOCK_SIZE;
uint32_t numBlocks = TEST_LENGTH_SAMPLES/BLOCK_SIZE;
float32_t  snr;
// LocalFileSystem local("local")
/* ----------------------------------------------------------------------
 * FIR LPF Example
 * ------------------------------------------------------------------- */

DigitalOut led(LED1);
class DSPDemo {
    public:
        DSPDemo():pDataXYZ{0}{
            memset(output, 0, sizeof(output));
        }

        void gatherData(){
            printf("Start sensor init\n");
            // Concatenate the ACCELERO data
            BSP_ACCELERO_AccGetXYZ(pDataXYZ);
            snprintf(output + strlen(output), sizeof(output) - strlen(output), "ACCELERO_X = %d\n", pDataXYZ[0]);
            snprintf(output + strlen(output), sizeof(output) - strlen(output), "ACCELERO_Y = %d\n", pDataXYZ[1]);
            snprintf(output + strlen(output), sizeof(output) - strlen(output), "ACCELERO_Z = %d\n", pDataXYZ[2]);

            // // Print the final concatenated string
            printf("data gather:\n %s", output);

            // led = 0;

        }
        void generateSingal(float32_t* signal, float32_t freq1, float32_t freq2){
            for (int i = 0; i < TEST_LENGTH_SAMPLES; i++) {
                // float32_t sample1 = arm_sin_f32(2.0 * PI * freq1 * ((float32_t)i / SAMPLE_RATE));
                // float32_t sample2 = arm_sin_f32(2.0 * PI * freq2 * ((float32_t)i / SAMPLE_RATE));
                BSP_ACCELERO_AccGetXYZ(pDataXYZ);
                signal[i] = pDataXYZ[2];
                printf("%.2f ", signal[i]);
                // if (i % 100 == 0){
                //     printf("%d ", i );
                // }
            }
            printf("\nSingal generated\n");

        }
        void run()
        {
            uint32_t i;
            arm_fir_instance_f32 S;
            arm_status status;
            float32_t  *inputF32, *outputF32;
            /* Initialize input and output buffer pointers */
            float32_t testInput_f32_1kHz_15kHz[TEST_LENGTH_SAMPLES];
            generateSingal(testInput_f32_1kHz_15kHz, 1000, 15000);
            inputF32 = &testInput_f32_1kHz_15kHz[0];
            outputF32 = &testOutput[0];
            /* Call FIR init function to initialize the instance structure. */
            arm_fir_init_f32(&S, NUM_TAPS, (float32_t *)&firCoeffs32[0], &firStateF32[0], blockSize);
            /* ----------------------------------------------------------------------
            ** Call the FIR process function for every blockSize samples
            ** ------------------------------------------------------------------- */
            for(i=0; i < numBlocks; i++)
            {
                arm_fir_f32(&S, inputF32 + (i * blockSize), outputF32 + (i * blockSize), blockSize);
            }
            printf("output:\n");
            for (i =0 ;i < TEST_LENGTH_SAMPLES; i++){
                printf("%.4f ", testOutput[i]);
            }
            /* ----------------------------------------------------------------------
            ** Compare the generated output against the reference output computed
            ** in MATLAB.
            ** ------------------------------------------------------------------- */
            // float32_t refOutput[TEST_LENGTH_SAMPLES];
            // snr = arm_snr_f32(&refOutput[0], &testOutput[0], TEST_LENGTH_SAMPLES);
            // printf("snr: %f\n",snr);
            // status = (snr < SNR_THRESHOLD_F32) ? ARM_MATH_TEST_FAILURE : ARM_MATH_SUCCESS;
  
            // if (status != ARM_MATH_SUCCESS){
            //     printf("FAILURE\n");
            // }else{
            //     printf("SUCCESS\n");
            // }
        }

    private:
        char output[500];
        int16_t pDataXYZ[3];
};

int main() {
    printf("Starting DSP demo\n");
    // printf("order %d: ",NUM_TAPS_ARRAY_SIZE);
    BSP_ACCELERO_Init();
    DSPDemo demo;
    // demo.gatherData();
    demo.run();
    return 0;
}