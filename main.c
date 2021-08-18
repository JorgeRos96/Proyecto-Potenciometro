/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   Proyecto para el manejo del potenciometro de la tarjeta de aplicaiones. 
	*					 A traves del ADC 1 del microcontrolador se obtiene el valor de la tensión
	*					 a la salida del potenciometro, el cual se envía a traves de la USART
	*					 al terminal. Variando el potenciometro se varía el valor de la tensión.
	*	
	*					 En este caso se esta obteniendo el valor a la salida del potenciometro 1
	*					 que se encuentra conectao al ADC 1 canal 3. Por lo que el pin al que se 
	*					 encuentra conectado es:
	*
	*					 - Pot 1: Pin PA3
	*			
	*					 Se configura el ADC con una resolución de 12 bits y se realiza la lectura 
	*					 del valor del potenciometro cada medio segundo. El valor del potenciometro
	*					 se encuentra entre 0 y 3.3 voltios.
	*
	*				 	 La frecuencia del sistema se ha configurado a 180 MHz utilizando el PLL 
	*					 con el HSI como fuente de reloj.
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Potenciometro.h"
#include "Delay.h"
#include "USART.h"
#include "Watchdog.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif



/* Private variables ---------------------------------------------------------*/
float v = 0;
char buf[100];
int size = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(int fallo);


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	/*Inicialización del IWDG*/
	if (init_Watchdog() != 0)
			Error_Handler(5);
	
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  if (HAL_Init() != HAL_OK)
		Error_Handler(0);

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();

	/*Inicializaciñon del Delay*/
	Init_Delay(180,4);

	/*Inicializaciñon del ADC1 para el POT1*/
  if (ADC1_Init() != HAL_OK)
		Error_Handler(4);
	
	/* Inicialización de la USART a traves de la función init_USART de la libreria USART
	*	 y habilitación de la transmisión
	*							- Baudrate = 9600 baud
	*							- Word length = 8 bits
	*							- Un bit de stop
	*							- Sin bit de paridad
	*							- Sin control de flujo
	*/
	if (init_USART() != 0)
		Error_Handler(2);
	

  /* Infinite loop */
  while (1)
  {
		v=lectura();
		/* Texto que se desea enviar*/
		size = sprintf(buf,"\rLa tension a la salida del potenciometro es: %f \n", v);
		/* Envío del array al terminal a traves de la función tx_USART de la librería USART*/
		if (tx_USART(buf, size) != 0)
			Error_Handler(3);
		Delay_ms(500);
		reset_Watchdog();
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 8
  *            PLL_N                          = 180
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	
  /** Se configura el HSI como fuente de reloj del PLL y se configuran
	* 	los parametros del PLL para ajusta la frecuencia a 180 MHz con una
	* 	frecuencia del HSI de 16 MHZ (por defecto).
	* 	SYSCLK =[(16MHz(frecuencia HSI)/8(PLLM))*180 (PLLN)]/2 (PLLP) = 180 MHz
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler(1);
  }
  /** Se activa el modo de Over Drive para poder alcanzar los 180 MHz
	* 	como frecuencia del sistema
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler(1);
  }
  /** Se selecciona el PLL como fuente de reloj del sistema y se configuran los parametros
	*		para configurar el HCLK, PCLK1 y PCLK2. La frecuencia máxima del HCLK es 180 MHZ, la 
	*		frecuencia máxima del PCLK1 es de 45 MHZ y la frecuencia máxima del PCLK2 es de 90 MHz
	*		HCLK = SYSCK/AHB = 180 MHz / 1 = 180 MHz
	*		PCLK1 = HCLK/APB1 = 180 MHz / 4 = 45 MHZ
	*		PCLK2 = HCLK/APB2 = 180 MHz / 2 = 90 MHZ
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler(1);
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(int fallo)
{
  if(fallo == 0)
		/* Mensaje si se ha producido un error en la inicializacón de la librería HAL*/
		printf(buf,"\r Se ha producido un error al inicializar la librería HAL\n");
	else if (fallo == 1)
		/* Mensaje si se ha producido un error en la inicializacón del reloj del sistema*/
		printf(buf,"\r Se ha producido un error al inicializar el reloj del sistema\n");
	else if(fallo == 2)
		/* Mensaje si se ha producido un error en la inicializacón de la USART*/
		printf(buf,"\r Se ha producido un error al inicializar la USART\n");
	else if (fallo == 3)
		/* Mensaje si se ha producido un error en el envío de datos de la USART*/
		printf(buf,"\r Se ha producido un error al enviar datos por la USART\n"); 
	else if (fallo == 4)
		/* Mensaje si se ha producido un error en la inicialización del ADC 1*/
		printf(buf,"\r Se ha producido un error al inicializar el potenciometro 1\n");
	else if (fallo == 5)
		/* Mensaje si se ha producido un error en la inicialización del Watchdog*/
		printf(buf,"\r Se ha producido un error al inicializar el Watchdog\n");
 
	while(1)
  {
  }
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
