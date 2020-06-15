//-----------------------------------------------------------------------------
// uart_registers.h
//-----------------------------------------------------------------------------
#if defined(__AVR_ATmega32__)
    #define UART_RX_Interrupt() ISR(USART_RXC_vect)
    #define UART_TX_Interrupt() ISR(USART_UDRE_vect)
    #define _UDR    UDR
    #define _UCSRA  UCSRA
    #define _UCSRB  UCSRB
    #define _UCSRC  UCSRC
    #define _URSEL  URSEL
    #define _UCSZ1  UCSZ1
    #define _UCSZ0  UCSZ0
    #define _RXEN   RXEN
    #define _TXEN   TXEN
    #define _TXC    TXC
    #define _UDRE   UDRE
    #define _RXCIE  RXCIE
    #define _UDRIE  UDRIE
    #define _UBRRH  UBRRH
    #define _UBRRL  UBRRL
    #define _PE     PE
    #define _DOR    DOR
    #define _FE     FE
#elif defined(__AVR_ATmega32U4__)
    #define UART_RX_VECTOR USART1_RXC_vect
    #define UART_TX_VECTOR USART1_UDRE_vect
    #define _UDR    UDR1
    #define _UCSRA  UCSR1A
    #define _UCSRB  UCSR1B
    #define _RXEN   RXEN1
    #define _TXEN   TXEN1
    #define _TXC    TXC1
    #define _UDRE   UDRE1
    #define _RXCIE  RXCIE1
    #define _UDRIE  UDRIE1
    #define _UBRRH  UBRR1H
    #define _UBRRL  UBRR1L*
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega88PA__)
    #define UART_RX_VECTOR USART_RX_vect
    #define UART_TX_VECTOR USART_UDRE_vect
    #define _UDR    UDR0
    #define _UCSRA  UCSR0A
    #define _UCSRB  UCSR0B
    #define _UCSRC  UCSR0C
    #define _URSEL  UMSEL00
    #define _UCSZ1  UCSZ01
    #define _UCSZ0  UCSZ00
    #define _RXEN   RXEN0
    #define _TXEN   TXEN0
    #define _TXC    TXC0
    #define _UDRE   UDRE0
    #define _RXCIE  RXCIE0
    #define _UDRIE  UDRIE0
    #define _UBRRH  UBRR0H
    #define _UBRRL  UBRR0L
    #define _PE     UPE0
    #define _DOR    DOR0
    #define _FE     FE0
#elif defined(__AVR_ATmega1284P__)
    #define UART_RX_VECTOR USART0_RX_vect
    #define UART_TX_VECTOR USART0_UDRE_vect
    #define _UDR    UDR0
    #define _UCSRA  UCSR0A
    #define _UCSRB  UCSR0B
    #define _UCSRC  UCSR0C
    #define _URSEL  UMSEL00
    #define _UCSZ1  UCSZ01
    #define _UCSZ0  UCSZ00
    #define _RXEN   RXEN0
    #define _TXEN   TXEN0
    #define _TXC    TXC0
    #define _UDRE   UDRE0
    #define _RXCIE  RXCIE0
    #define _UDRIE  UDRIE0
    #define _UBRRH  UBRR0H
    #define _UBRRL  UBRR0L
    #define _PE     UPE0
    #define _DOR    DOR0
    #define _FE     FE0
#endif
//-----------------------------------------------------------------------------
