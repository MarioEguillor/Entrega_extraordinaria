#include "ADC.h"


void ADCInit(void){
    fsp_err_t err = FSP_SUCCESS;

    /* Inicializar ADC */
   err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
   assert(FSP_SUCCESS == err);
   err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
   assert(FSP_SUCCESS == err);
}

void ADCStartScan (void){
   /* In software trigger mode, start a scan by calling R_ADC_ScanStart(). In other modes, enable external
    * triggers by calling R_ADC_ScanStart(). */
    R_ADC_ScanStart(&g_adc_ctrl);
}

void ADCWaitConversion (void){
   /* Wait for conversion to complete. */
    adc_status_t status;
    R_ADC_StatusGet(&g_adc_ctrl, &status);

    if (status.state == ADC_STATE_SCAN_IN_PROGRESS) {
        return(0);
    }
    return(1);
}



uint16_t ReadADC (adc_channel_t Achan){
   fsp_err_t err = FSP_SUCCESS;
   uint16_t channel4_conversion_result;
   /*Lectura del valor del potenciometro, conectado en el canal 4*/
   err = R_ADC_Read(&g_adc_ctrl, Achan, &channel4_conversion_result);
   assert(FSP_SUCCESS == err);
   return (channel4_conversion_result);
   }







