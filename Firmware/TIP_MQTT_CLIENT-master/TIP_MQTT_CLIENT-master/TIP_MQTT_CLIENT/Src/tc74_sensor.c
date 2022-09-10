/*
 * tc74_sensor.c
 *
 *  Created on: 21 jun. 2022
 *      Author: esalgado
 */


/* --------------------------------- Notes --------------------------------- */
/* ----------------------------- Include files ----------------------------- */
#include "tc74_sensor.h"

/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
#define TC74_ADDRESS 0x9A // TC74	A5 address

/* ---------------------------- Local data types --------------------------- */
/* ---------------------------- Global variables --------------------------- */
/* --------------------------- External variables -------------------------- */
/* ---------------------------- Local variables ---------------------------- */
/* ----------------------- Local function prototypes ----------------------- */
/* -------------------------- Callback functions --------------------------- */
/* ---------------------------- Local functions ---------------------------- */
/* ---------------------------- Global functions --------------------------- */
bool tc74_sensor_readTemperature(I2C_HandleTypeDef *pHandle, uint8_t *temperature)
{
	uint8_t cmd = 0x00;

  if (HAL_OK == HAL_I2C_Master_Transmit(pHandle, TC74_ADDRESS, (uint8_t *) &cmd, sizeof(cmd), 10))
  {
    if (HAL_OK == HAL_I2C_Master_Receive(pHandle, TC74_ADDRESS, (uint8_t *) temperature, sizeof(*temperature), 10))
    {
      return true;
    }
  }

  return false;
}

/* ------------------------------ End of file ------------------------------ */
