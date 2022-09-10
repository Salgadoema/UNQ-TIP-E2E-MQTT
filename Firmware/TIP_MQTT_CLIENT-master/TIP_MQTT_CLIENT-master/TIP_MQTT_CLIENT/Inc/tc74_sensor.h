/*
 * tc74_sensor.h
 *
 *  Created on: 21 jun. 2022
 *      Author: esalgado
 */

#ifndef TC74_SENSOR_H_
#define TC74_SENSOR_H_

#ifndef TC74_SENSOR_H_INCLUDED
#define TC74_SENSOR_H_INCLUDED
/* ----------------------------- Include files ----------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

/* ---------------------- External C language linkage ---------------------- */
/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External functions --------------------------- */
bool tc74_sensor_readTemperature(I2C_HandleTypeDef *pHandle, uint8_t *temperature);

/* -------------------- External C language linkage end -------------------- */
/* ------------------------------ Module end ------------------------------- */
#endif
/* ------------------------------ File footer ------------------------------ */

#endif /* TC74_SENSOR_H_ */
