/*
 * hal_entry.h
 *
 *  Created on: 9 dic 2023
 *      Author: Hezitzaile
 */

#ifndef HAL_ENTRY_H_
#define HAL_ENTRY_H_

#include "hal_data.h"
#include "r_adc_api.h"
#include "r_iic_master.h"

// Declaración de la función de inicio del hardware abstraction layer (HAL)
void hal_entry(void);

// Declaración de la función de callback del temporizador
void timer_callback(timer_callback_args_t *p_args);

// Declaración de la función de inicio del sistema
void R_BSP_WarmStart(bsp_warm_start_event_t event);

#ifdef BSP_TZ_SECURE_BUILD
// Declaración de una función no segura llamable
void template_nonsecure_callable(void);
#endif /* HAL_ENTRY_H_ */
