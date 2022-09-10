/*
 * hih_5031_sensor.h
 *
 *  Created on:  21 jun. 2022
 *      Author: esalgado
 */

#ifndef HIH_5031_SENSOR_H_
#define HIH_5031_SENSOR_H_

/* --------------------------------- Module -------------------------------- */
#ifndef HIH_5031_SENSOR_H_INCLUDED
#define HIH_5031_SENSOR_H_INCLUDED
/* ----------------------------- Include files ----------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

/* ---------------------- External C language linkage ---------------------- */
/* ----------------------------- Local macros ------------------------------ */
/* ------------------------------- Constants ------------------------------- */
/* ------------------------------- Data types ------------------------------ */
/* -------------------------- External functions --------------------------- */
bool hih_5031_sensor_readHumidity(ADC_HandleTypeDef *pHandle, uint8_t *humidity);

/* -------------------- External C language linkage end -------------------- */
/* ------------------------------ Module end ------------------------------- */
#endif
/* ------------------------------ File footer ------------------------------ */


#endif /* HIH_5031_SENSOR_H_ */
