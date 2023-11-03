#include "mbed.h"
#include "stm32l4xx_hal.h"
#include <cstdint>

TIM_HandleTypeDef htim1;
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma1;


static events::EventQueue event_queue(32 * EVENTS_EVENT_SIZE);

#define SAMPLE_BUFFER_SIZE  256
uint16_t sample_buffer[SAMPLE_BUFFER_SIZE];


void Error_Handler()
{
    __disable_irq();
    while (1)
    {

    }
}




void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)
    {
        /* Peripheral clock enable */
        __HAL_RCC_ADC_CLK_ENABLE();

        /* ADC1 interrupt Init */
        /*NVIC_SetVector(ADC1_2_IRQn, (uint32_t)&ADC1_2_IRQHandler);
        HAL_NVIC_SetPriority(ADC1_2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(ADC1_2_IRQn);*/
    }

}


void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance==ADC1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_ADC_CLK_DISABLE();
        /* ADC1 interrupt DeInit */
        HAL_NVIC_DisableIRQ(ADC1_2_IRQn);
    }

}

void ADC1_Init(void)
{
    ADC_MultiModeTypeDef multimode = {0};
    ADC_ChannelConfTypeDef sConfig = {0};

  /** Common config
  */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;//ADC_CLOCK_ASYNC_DIV1;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc1.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }

  /** Configure the ADC multi-mode
  */
    multimode.Mode = ADC_MODE_INDEPENDENT;
    if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
    {
        Error_Handler();
    }   

  /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;//ADC_SAMPLETIME_247CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

}



void print_top_half_callback()
{
    for(int i = 0; i < SAMPLE_BUFFER_SIZE / 2; i++)
    {
        printf("%d ", sample_buffer[i]);
    }
   
}
void print_bot_half_callback(){
    for(int i = SAMPLE_BUFFER_SIZE / 2; i < SAMPLE_BUFFER_SIZE; i++)
    {
        printf("%d ", sample_buffer[i]);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	//to do ..........................
    event_queue.call(print_bot_half_callback);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    event_queue.call(print_top_half_callback);
}

void DMA1_Channel1_IRQHandler(void)
{
    SET_BIT(hadc1.Instance->CFGR, ADC_CFGR_DMACFG);
    HAL_DMA_IRQHandler(&hdma1);
}

void ADC1_DMA1CH1_init()
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    hdma1.Instance = DMA1_Channel1;
    hdma1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma1.Init.MemInc = DMA_MINC_ENABLE;
    hdma1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma1.Init.Mode = DMA_CIRCULAR;
    hdma1.Init.Priority = DMA_PRIORITY_HIGH;

    if (HAL_DMA_Init(&hdma1) != HAL_OK)
    {
        Error_Handler();
    }

    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma1);

    ADC1_Init();

    NVIC_SetVector(DMA1_Channel1_IRQn, (uint32_t)&DMA1_Channel1_IRQHandler);
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}



void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
    if(htim_base->Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_ENABLE();
        /*NVIC_SetVector(TIM1_UP_TIM16_IRQn, (uint32_t)&TIM1_UP_IRQHandler);
        HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);*/
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
    if(htim_base->Instance == TIM1)
    {
        __HAL_RCC_TIM1_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
    }
}


static void TIM1_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 4000 - 1;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 1000 - 1;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

// main() runs in its own thread in the OS
int main()
{
    //HAL_Init();
    //SystemClock_Config();

    TIM1_Init();
    ADC1_DMA1CH1_init();
    
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)sample_buffer, SAMPLE_BUFFER_SIZE);
    HAL_TIM_Base_Start_IT(&htim1);
    event_queue.dispatch_forever();

    return 0;
}

