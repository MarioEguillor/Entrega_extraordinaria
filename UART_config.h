#include "hal_data.h"


#ifndef UART_CONFIG_H_
#define UART_CONFIG_H_

#define DATA_LENGTH               (4u)      /* Expected Input Data length */
#define UART_ERROR_EVENTS         (UART_EVENT_BREAK_DETECT | UART_EVENT_ERR_OVERFLOW | UART_EVENT_ERR_FRAMING | \
                                    UART_EVENT_ERR_PARITY)    /* UART Error event bits mapped in registers */
#define RESET_VALUE        0x00


fsp_err_t init_uart(void);
fsp_err_t uart_write(uint8_t *p_msg);
void deinit_uart(void);

#endif /* UART_CONFIG_H_ */
