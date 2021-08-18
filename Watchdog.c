/**
  ******************************************************************************
  * @file    Templates/Src/Watchdog.c
  * @author  MCD Application Team
	* @brief   Fichero que contiene las funciones de inicialización y configuración
	*					 del IWDG.
	*  
	*					 Se configura con una frecuencia de 1 Hz al configurar el prescaler a 
	*					 32 y el reload a 750, ya que la frecuencia del reloj LSI es de 32 kHz.
	*
	*					 fiwdg = flsi / ((2^PR) * RL) = 32000 /(32*750) = 1.3 Hz
	*					
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * 
  ******************************************************************************
  */
	#include "Watchdog.h"
	
IWDG_HandleTypeDef IwdgHandle;
	

/**
	* @brief Función de inicialización y configuración del IWDG donde se establece
	*				 un prescaler de 32 y un valor de reload de 750 para tener un IWDG  
	* 			 TimeOut de 750 ms.
	*				
	*				 tiwdg = tlsi * (2^PR) * RL = (1/32000) * 32 * 750 = 750 ms
	*
  * @param None
  * @retval valor que se devuelve para comprobar si se ha realizado correctamente 
	*					la inicialización (0).
  */
int init_Watchdog (){
	
  IwdgHandle.Instance = IWDG;
  IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
  IwdgHandle.Init.Reload    = 749;
	
  if (HAL_IWDG_Init(&IwdgHandle) != HAL_OK)
  {
    /* Initialization Error */
		return -1;
  }
	return 0;
}

/**
	* @brief Función que realiza el reseteo de IWDG para que vuelva a comenzar la cuenta
	*				 y no se resetee el sistema
	*
  * @param None
  * @retval None
  */
void reset_Watchdog (){
	HAL_IWDG_Refresh(&IwdgHandle);
}