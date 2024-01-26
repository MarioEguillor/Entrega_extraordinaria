#ifndef I2C_LCD_CONTROL_H
#define I2C_LCD_CONTROL_H

#include "hal_data.h"

// Declaración de la función para inicializar el módulo I2C
fsp_err_t init_i2c(void);

// Declaración de la función para inicializar el LCD
void initialice_LCD(void);

// Declaración de la función para limpiar el LCD
void clear_i2c(void);

// Declaración de la función para escribir en el LCD
fsp_err_t write_LCD(uint8_t line[], uint8_t msg[]);

void g_i2c_master_callback(i2c_master_callback_args_t *p_args);

#endif // I2C_LCD_CONTROL_H
