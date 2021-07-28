/**
  ******************************************************************************
  * @file    Templates/Src/Potenciometro.c
  * @author  MCD Application Team
  * @brief   Fichero de inicialización y cofiguración del potenciometro 1 de la 					 
	*					 tarjeta de aplicaciones. Se utiliza el ADC 1 canal 3 para obtener 
	*					 el valor del potenciometro. El ADC se configura con una resolución 
	*					 de 12 bits y el pin al que se conecta el potenciometro es:
	*
	*					 - Pin POT1: PA3 
	*						
	*					 La lectura del potenciometro obtiene los valores de entre 0 y 3.3 
	*					 voltios.
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * 
  ******************************************************************************
  */

#include "Potenciometro.h"

ADC_HandleTypeDef hadc1;

/**
  * @brief Función de inicialización del canal 3 del ADC1 con una resolucion de 12 bits
  * @param None
  * @retval None
  */
int ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

   /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    return -1;
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    return -1;
  }

}

/**
  * @brief Función de lectura del valor del ADC y convierte el valor en la tensión equivalente a la salida del potenciometro 
  * @param None
	* @retval voltaje: Tensión a la salida del potenciometro qu regula la tensión de 3.3 V
  */
float lectura(void)
	{
		uint32_t conversion = 0;
		float voltaje = 0;
		
		/*Se inicia la conversión a traves del ADC*/
		HAL_ADC_Start(&hadc1);
		/*Se obtiene el valor a la salida del potenciometro*/
		conversion = HAL_ADC_GetValue(&hadc1);
		/*Se convierte el valor a la tensión equivalente*/
		voltaje = conversion*3.3/4096; //3.3V de vref y 4096 por los 12 bits de resolución en el ADC
		
		return voltaje;

}