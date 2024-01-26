#include "UART_config.h"


/* Flag for user callback */
static volatile uint8_t g_uart_event = RESET_VALUE;
fsp_err_t uart = FSP_SUCCESS;


/* Inicializar modulo. LLamar desde el hal_entry.c */
fsp_err_t init_uart (void)
{
    uart = R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    return uart;
}



/* Funcion de envio de datos por UART*/
fsp_err_t uart_write(uint8_t *p_msg)
{

    uint32_t local_timeout = (DATA_LENGTH * UINT16_MAX);
    uint8_t msg_len = RESET_VALUE;
    char *p_temp_ptr = (char *)p_msg;

    msg_len = ((uint8_t)(strlen(p_temp_ptr)));


    uart = R_SCI_UART_Write (&g_uart0_ctrl, p_msg, msg_len);
    if (FSP_SUCCESS != uart)
     {
         return uart;
     }

     /* Check for event transfer complete */
     while ((UART_EVENT_TX_COMPLETE != g_uart_event) && (--local_timeout))
     {
         /* Check if any error event occurred */
         if (UART_ERROR_EVENTS == g_uart_event)
         {
             return FSP_ERR_TRANSFER_ABORTED;
         }
     }
     if(RESET_VALUE == local_timeout)
     {
         uart = FSP_ERR_TIMEOUT;
     }
     return uart;
 }



void deinit_uart(void)
{
    /* Close module */
    uart = R_SCI_UART_Close (&g_uart0_ctrl);
    assert(FSP_SUCCESS != uart);
}


void uart_callback(uart_callback_args_t *p_args)
{
    /* Logged the event in global variable */
    g_uart_event = (uint8_t)p_args->event;
}


