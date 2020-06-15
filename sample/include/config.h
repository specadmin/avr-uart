#ifndef CONFIG_H
#define CONFIG_H
//----------------------------------------------------------------------------
#include <avr/io.h>
//----------------------------------------------------------------------------

#define UART_RX_BUFFER_SIZE 64
#define UART_TX_BUFFER_SIZE 64

#define DEBUG_PORT          PORTB
#define DEBUG_DDR           DDRB
#define DEBUG_BIT           4
#define DEBUG_BAUD_RATE     1152000
// #define DEBUG_EXTENDED_DUMP
//#define RAM_GUARD_ENABLED

//----------------------------------------------------------------------------
#endif

