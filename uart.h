/******************************************************************************
 Библиотека для автоматизированной работы с UART
******************************************************************************/
#ifndef UART_H
#define UART_H

// CONFIGURATION FILE
//-----------------------------------------------------------------------------
#include "../../include/config.h"
//-----------------------------------------------------------------------------

// CONFIGURATION PARAMETERS
/******************************************************************************
  UART_RX_BUFFER_SIZE - размер кольцевого FIFO буфера приема в байтах

  UART_TX_BUFFER_SIZE - размер кольцевого FIFO буфера передачи в байтах

   Расскомментируйте следующую строку, если код UART должен размещаться в
   секции загрузчика (для возможности обновления прошивки по UART)
  #define UART_SECTION __attribute__ ((section (".bootloader.text")))
******************************************************************************/

// DEPENDENCIES
//-----------------------------------------------------------------------------
#include "../avr-misc/avr-misc.h"
#ifdef DEBUG
#include "../avr-debug/debug.h"
#endif
//-----------------------------------------------------------------------------
#ifndef UART_RX_BUFFER_SIZE
#error Определите параметр UART_RX_BUFFER_SIZE в файле конфигурации
#endif

#ifndef UART_TX_BUFFER_SIZE
#error Определите параметр UART_TX_BUFFER_SIZE в файле конфигурации
#endif

#ifndef UART_SECTION
#define UART_SECTION
#endif
//------------------------------------------------------------------------------------
enum baud_rate_id
{
    BAUD_9600,
    BAUD_19200,
    BAUD_38400,
    BAUD_57600,
    BAUD_115200,
    BAUD_230400,
    BAUD_460800
};
//-----------------------------------------------------------------------------
void uart_init(const BYTE baud);
BYTE uart_available();
BYTE uart_peek();
BYTE uart_read(BYTE *buf, const BYTE count);
BYTE uart_write(const BYTE *buf, BYTE count);
BYTE uart_write(const char *str);
BYTE uart_pwrite(const char *str);
void uart_flush_rx();
void uart_flush_tx();
//-----------------------------------------------------------------------------
#endif
