#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "hal_data.h"

void ADCInit(void);
void ADCStartScan (void);
void ADCWaitConversion (void);
uint16_t ReadADC (adc_channel_t Achan);
static fsp_err_t ADC_Initialize(void);


#endif // LCD_I2C_H
