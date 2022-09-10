/*
 * hih_5031_sensor.c
 *
 *  Created on: 21 jun. 2022
 *      Author: esalgado
 */


/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "hih_5031_sensor.h"
/* ---------------------------- Global functions --------------------------- */

uint8_t  vOutRH, trueRH ;

bool hih_5031_sensor_readHumidity(ADC_HandleTypeDef *pHandle, uint8_t *humidity)
{
  uint32_t counts;

  if (HAL_OK == HAL_ADC_Start(pHandle))
  {
    if (HAL_OK == HAL_ADC_PollForConversion(pHandle, 10))
    {
      counts = HAL_ADC_GetValue(pHandle);
      vOutRH=counts/(1.0546-(0.00216*21))*2.0/4095;
      *humidity=  (int)(((vOutRH-0.5f)/0.02f));
    }

      return true;
    }


  return false;
}

/* ------------------------------ End of file ------------------------------ */
