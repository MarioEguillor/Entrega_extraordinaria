#include "hal_data.h"
#include "r_adc_api.h"
#include "r_iic_master.h"
#include "I2C.h"
#include "ADC.h"
#include <stdio.h>



fsp_err_t icu_init(void);
fsp_err_t icu_enable(void);
void icu_deinit(void);

//VARIABLES GLOBALES
bool flanco2segs=false;
volatile bool sistema = false;
uint8_t fila1[0x02] = {0x00, 0x80};
uint8_t fila2[0x02] = {0x00, 0xC0};

uint8_t mensaje1[0x08] = {0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
uint8_t mensaje2[0x08] = {0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};

#define BUZZER_PINBSP_IO_PORT_01_PIN_13
bool bocinaon = false;


#define Filtrogeneral 5
uint16_t filtro[Filtrogeneral];
uint8_t indiceFiltro = 0;

//FUNCIONES

void init_led(void);
void blink_led(void);
void init_bocina(void);
void bocina(void);
void init_pulsador(void);
void actualizarFiltro(uint16_t nuevaLectura);
uint16_t calcularMediana();
void mostrarSystemOn();
void DisplayLCD(uint16_t value, uint8_t m1[]);
void DisplayLCDPeligro(int nivel, uint8_t m1[]);
int nivel_velocidad(uint16_t mediana);
void R_BSP_WarmStart(bsp_warm_start_event_t event);


//INICIALIZACIONES

void init_led(void) {

    fsp_err_t err = R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
        assert(FSP_SUCCESS == err);

        err = R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_06, IOPORT_CFG_PORT_DIRECTION_OUTPUT);
        assert(FSP_SUCCESS == err);
}

void init_bocina(void) {

    fsp_err_t err = R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
        assert(FSP_SUCCESS == err);

        err = R_IOPORT_PinCfg(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_13, IOPORT_CFG_PORT_DIRECTION_OUTPUT);
        assert(FSP_SUCCESS == err);
}

void init_pulsador(void) {
    fsp_err_t err;

    err = R_ICU_ExternalIrqOpen(&g_external_irq0_ctrl, &g_external_irq0_cfg);
    assert(FSP_SUCCESS == err);

    err = R_ICU_ExternalIrqCallbackSet(&g_external_irq0_ctrl, btn_callback, NULL, NULL);
    assert(FSP_SUCCESS == err);
}

//FUNCIONES PRINCIPALES

//ACTUALIZAR FILTRO
void actualizarFiltro(uint16_t nuevaLectura) {
    filtro[indiceFiltro] = nuevaLectura;
    indiceFiltro = (indiceFiltro + 1) % Filtrogeneral;
}


//CALCULAR MEDIANA
uint16_t calcularMediana() {
    uint16_t valoresOrdenados[Filtrogeneral];
    memcpy(valoresOrdenados, filtro, sizeof(filtro));

     for (int i = 0; i < Filtrogeneral - 1; i++) {
        for (int j = i + 1; j < Filtrogeneral; j++) {
            if (valoresOrdenados[j] < valoresOrdenados[i]) {
                uint16_t temp = valoresOrdenados[i];
                valoresOrdenados[i] = valoresOrdenados[j];
                valoresOrdenados[j] = temp;
            }
        }
    }
    return valoresOrdenados[Filtrogeneral / 2];
}


//LED
void blink_led(void) {

        fsp_err_t err;
        //LED ON
        err = R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_06, BSP_IO_LEVEL_HIGH);
        assert(FSP_SUCCESS == err);

        // Delay para mantener el LED encendido
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

        //LED OFF
        err = R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_06, BSP_IO_LEVEL_LOW);
        assert(FSP_SUCCESS == err);

        // Delay antes de volver a llamar a blink_led
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);

}

//BOCINA
void bocina(void){

    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_13, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_13, BSP_IO_LEVEL_LOW);
    bocinaon = true;

}

//BOTON
void btn_callback(external_irq_callback_args_t *p_args) {
    if (p_args->channel == g_external_irq0_cfg.channel) {
        sistema = true;  // Activar el sistema

    }
}


// SYSTEM ON
void mostrarSystemOn() {
    bocinaon=false;
    mensaje1[0x00] = 0x40;
    mensaje1[0x01] = 0x53;
    mensaje1[0x02] = 0x59;
    mensaje1[0x03] = 0x53;
    mensaje1[0x04] = 0x54;
    mensaje1[0x05] = 0x45;
    mensaje1[0x06] = 0x4D;
    mensaje1[0x07] = 0x80;

    mensaje2[0x00] = 0x40;
    mensaje2[0x01] = 0x80;
    mensaje2[0x02] = 0x80;
    mensaje2[0x03] = 0x80;
    mensaje2[0x04] = 0x4F;
    mensaje2[0x05] = 0x4E;
    mensaje2[0x06] = 0x80;
    mensaje2[0x07] = 0x80;
    write_LCD(fila1, mensaje1);
    write_LCD(fila2, mensaje2);
}


//Actualizar el LCD(potenciometro)
void DisplayLCD(uint16_t value, uint8_t men1[]) {
    char str[100];
    char numero_char;
    unsigned char ValorAsci1, ValorAsci2, ValorAsci3, ValorAsci4, ValorAsci5;
    if (!flanco2segs){
        // Convertir el valor numérico a una cadena de caracteres
        sprintf(str, "%i", value);
        uart_write(str);
        flanco2segs=true;
        }



    numero_char = str[0];
    ValorAsci1 = (unsigned char)numero_char;

    numero_char = str[1];
    ValorAsci2 = (unsigned char)numero_char;

    numero_char = str[2];
    ValorAsci3 = (unsigned char)numero_char;

    numero_char = str[3];
    ValorAsci4 = (unsigned char)numero_char;

    numero_char = str[4];
    ValorAsci5 = (unsigned char)numero_char;

    men1[0x00] = 0x40;
    men1[0x01] = ValorAsci1;
    men1[0x02] = ValorAsci2;
    men1[0x03] = ValorAsci3;
    men1[0x04] = ValorAsci4;
    men1[0x05] = ValorAsci5;
    men1[0x06] = 0x20;
    men1[0x07] = 0x20;
}

//PELIGRO
void DisplayLCDPeligro(int nivel,uint8_t men1[]) {
    if (nivel == 4) {
        blink_led();
        if (bocinaon==0){
        bocina();
        }

        mensaje2[0x01] = 0x50; // P
        mensaje2[0x02] = 0x45; // E
        mensaje2[0x03] = 0x4C; // L
        mensaje2[0x04] = 0x49; // I
        mensaje2[0x05] = 0x47; // G
        mensaje2[0x06] = 0x52; // R
        mensaje2[0x07] = 0x4F; // O

    } else {
        men1[0x00] = 0x20;
        men1[0x01] = 0x20;
        men1[0x02] = 0x20;
        men1[0x03] = 0x20;
        men1[0x04] = 0x20;
        men1[0x05] = 0x20;
        men1[0x06] = 0x80;
        men1[0x07] = 0x80;
    }
}

//UMBRALES DE SEGURIDAD
int nivel_velocidad(uint16_t mediana) {
    const uint16_t menor1 = 1000;
    const uint16_t menor2 = 2600;
    const uint16_t menor3 = 8000;

    if (mediana < menor1) {
        return 1; // Nivel de velocidad menor que 1000
    } else if (mediana < menor2) {
        return 2; // Nivel de velocidad menor que 2600
    } else if (mediana < menor3) {
        return 3; // Nivel de velocidad menor que 8000
    } else {
        return 4; // Nivel de velocidad 4 mayor que 8000
    }
}

// Temporizador
void timer_callback(timer_callback_args_t *p_args) {
    if (TIMER_EVENT_CYCLE_END == p_args->event) {

        ADCStartScan();
        ADCWaitConversion();


        uint16_t resultado = ReadADC(ADC_CHANNEL_4);

        actualizarFiltro(resultado);
        uint16_t mediana = calcularMediana();
        int nivel = nivel_velocidad(resultado);
            if (nivel == 4) {
                flanco2segs=false;
                DisplayLCD(mediana, mensaje1);
                DisplayLCDPeligro(nivel, mensaje2);
            } else {
                mostrarSystemOn();
            }
    }
}
// FUNCIÓN PRINCIPAL
void hal_entry(void) {

    /* INICIZIALIZACION DEL TIMER*/
    fsp_err_t err = FSP_SUCCESS;
    err = R_GPT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    assert(FSP_SUCCESS == err);
    (void) R_GPT_Start(&g_timer0_ctrl);

    ADCInit();
    fsp_err_t i2c = init_i2c();
    clear_i2c();
    init_uart();
    initialice_LCD();

    /* 1. Inicializar el driver IRQ*/
       err = icu_init();
       if (FSP_SUCCESS != err)
       {
           printf("Error en la inicialización del driver");
       }

       // Habilitar el IRQ driver
       err = icu_enable();
       if (FSP_SUCCESS != err)
       {
           // Si habilitar falla, desinicializar el controlador de IRQ para no dejar recursos colgados.
           icu_deinit();
           // Manejar el error
           while(1);
       }

       init_led();
       init_bocina();
       init_pulsador();

       while (!sistema) {

       }

       mostrarSystemOn();
       while (1) {

           write_LCD(fila1, mensaje1);
           write_LCD(fila2, mensaje2);
       }

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif


}
fsp_err_t icu_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ICU module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq0_ctrl, &g_external_irq0_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Open failure message */
        //APP_ERR_PRINT ("\r\n*R_ICU_ExternalIrqOpen API FAILED*\r\n");
    }
    return err;
}

fsp_err_t icu_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Enable ICU module */
    err = R_ICU_ExternalIrqEnable(&g_external_irq0_ctrl);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Enable failure message */
       // APP_ERR_PRINT ("\r\n*R_ICU_ExternalIrqEnable API FAILED*\r\n");
    }
    return err;
}

void icu_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close ICU module */
    err = R_ICU_ExternalIrqClose(&g_external_irq0_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* ICU Close failure message */
        //APP_ERR_PRINT("\r\n*R_ICU_ExternalIrqClose API FAILED*\r\n");
    }
}



/*************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **************/

void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif

