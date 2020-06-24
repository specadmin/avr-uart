//-----------------------------------------------------------------------------
// uart.cpp
/* код UART считается стабильным, используется для обновления прошивки
** и поэтому живет в области загрузчика */
//-----------------------------------------------------------------------------
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "uart_registers.h"
//-----------------------------------------------------------------------------
#define UART_AVAILABLE ((UART_RX_BUFFER_SIZE+rx_head-rx_tail)%UART_RX_BUFFER_SIZE)
#define uart_tx_buf_is_empty  (UART::tx_head==UART::tx_tail)
#define uart_rx_buf_is_empty  (UART::rx_head==UART::rx_tail)

#ifdef DEBUG
#define HALT(text) { DSTR("ERROR: "text); DSTR("Device halted. Shit happens, sorry"); while(1); }
#else
#define HALT(text) while(1);
#endif

#if (UART_TX_BUFFER_SIZE>256)
typedef WORD tx_buf_index_t;
#else
typedef BYTE tx_buf_index_t;
#endif

#if (UART_RX_BUFFER_SIZE>256)
typedef WORD rx_buf_index_t;
#else
typedef BYTE rx_buf_index_t;
#endif
//-----------------------------------------------------------------------------
namespace UART
{
    extern BYTE inactive;
    // кольцевые буферы приема и передачи
    extern BYTE rx_buf[UART_RX_BUFFER_SIZE];
    extern volatile rx_buf_index_t rx_head, rx_tail;
    extern BYTE tx_buf[UART_TX_BUFFER_SIZE];
    extern volatile tx_buf_index_t tx_head, tx_tail;
};
//-----------------------------------------------------------------------------
__inline void uart_rx_interrupt()
{
    // вкладыш в обработчик прерывания RXC
    // ! отладочные комманды в этом обработчике приводят к пропуску байт при чтении !
#ifdef DEBUG
    if(test_bit(_UCSRA,_DOR))
    {
        HALT("RX Data overrun detected");
    }
#endif
    UART::inactive=0;     // сбрасываем счетчик следящего таймера
    register rx_buf_index_t index=(UART::rx_head+1)%UART_RX_BUFFER_SIZE;
    UART::rx_buf[UART::rx_head]=_UDR;
    UART::rx_head=index;
}
//-----------------------------------------------------------------------------
__inline void uart_tx_interrupt()
{
    // вкладыш в обработчик прерывания UDRE
    BYTE val=UART::tx_buf[UART::tx_tail];
    UART::tx_tail=(UART::tx_tail+1)%UART_TX_BUFFER_SIZE;
    _UDR=val;
    set_bit(_UCSRA,_TXC);
    if(uart_tx_buf_is_empty)
    {
        // буфер пуст - выключаем прерывание
        clr_bit(_UCSRB,_UDRIE);
    }
}
//-----------------------------------------------------------------------------
namespace UART
{
    BYTE inactive;
    // кольцевые буферы приема и передачи
    BYTE rx_buf[UART_RX_BUFFER_SIZE];
    volatile rx_buf_index_t rx_head, rx_tail;
    BYTE tx_buf[UART_TX_BUFFER_SIZE];
    volatile tx_buf_index_t tx_head, tx_tail;
}
//-----------------------------------------------------------------------------
using namespace UART;
//-----------------------------------------------------------------------------
void UART_SECTION uart_init(const BYTE baud, enum UART_mode mode)
{
    WORD ubrr=0;
    _UCSRA=0;  // "одинарная" скорость менее требовательна к делителю частоты и точности
    switch(baud)
    {
    case BAUD_9600:
        ubrr=(F_CPU/8/9600-1)/2;    // такой подход реализует округление
        break;
    case BAUD_19200:
        ubrr=(F_CPU/8/19200-1)/2;
        break;
    case BAUD_38400:
        ubrr=(F_CPU/8/38400-1)/2;
        break;
    case BAUD_57600:
        ubrr=(F_CPU/8/57600-1)/2;
        break;
    case BAUD_115200:
        ubrr=(F_CPU/8/115200-1)/2;
        break;
    case BAUD_230400:
        ubrr=(F_CPU/8/230400-1)/2;
        break;
    case BAUD_460800:
        ubrr=(F_CPU/8/460800-1)/2;
        break;
    default:
        HALT("Invalid UART baud rate");
    }
    _UBRRH=ubrr>>8;
    _UBRRL=(BYTE)ubrr;
    uart_flush_rx();
    uart_flush_tx();
    _UCSRB = 0;
    if(mode & 0x01)
    {
        // enable receiption
        _UCSRB = _UCSRB | _bit(_RXEN) | _bit(_RXCIE);
    }
    if(mode & 0x02)
    {
        // enable transmission
        _UCSRB = _UCSRB | _bit(_TXEN);
    }
    _UCSRC=_bit(_UCSZ1) | _bit(_UCSZ0);  // 8 bit data
}
//-----------------------------------------------------------------------------
void UART_SECTION uart_flush_rx()
{
    rx_head=0;
    rx_tail=0;
}
//-----------------------------------------------------------------------------
void UART_SECTION uart_flush_tx()
{
    tx_head=0;
    tx_tail=0;
    clr_bit(_UCSRB,_UDRIE);
}
//-----------------------------------------------------------------------------
BYTE UART_SECTION uart_available()
{
    disable_interrupts();
    BYTE sz=UART_AVAILABLE;
    resume_interrupts();
    return sz;
}
//-----------------------------------------------------------------------------
// BYTE uart_peek() возвращает первый байт из очереди, не изменяя самой очереди
BYTE UART_SECTION uart_peek()
{
    if(uart_rx_buf_is_empty)
    {
        return 0;  // буфер пуст
    }
    return rx_buf[rx_tail];
}
//-----------------------------------------------------------------------------
BYTE UART_SECTION uart_read(BYTE *buf, const BYTE count)
{
    if(uart_rx_buf_is_empty)
    {
        // буфер пуст
        HALT("UART: nothing to read");
        return 0;
    }
    BYTE num=count;
    if(count>UART_AVAILABLE)
    {
        num=UART_AVAILABLE;
    }
    BYTE i;
    for(i=0;i<count;i++)
    {
        buf[i]=rx_buf[rx_tail];
        rx_tail=(rx_tail+1)%UART_RX_BUFFER_SIZE;
    }
    return num;
}
//-----------------------------------------------------------------------------
__inline BYTE uart_write(const BYTE ch)
{
    if(uart_tx_buf_is_empty && test_bit(_UCSRA,_UDRE))
    {
        // выходной буфер пуст - напрямую отправляем 1 байт
        // к этому моменту прерывание UDRIE было отключено
        // его обработчиком или функцией flush_tx()
        _UDR=ch;
        set_bit(_UCSRA,_TXC);
        return 1;
    }
    set_bit(_UCSRB,_UDRIE);     // включаем прерывание
    tx_buf_index_t i=(tx_head+1)%UART_TX_BUFFER_SIZE;
    while (i==tx_tail)
    {
        // выходной буфер заполнен, ждем пока освободится.
        // Нехорошая ситуация, которая заставляет нас ждать
        // возможно, следует увеличить размер tx_buf
        if (!test_bit(SREG,SREG_I) && test_bit(_UCSRA,_UDRE))
        {
            // если прерывания отключены, а UART готов принять
            // от нас для передачи следующий байт, вызываем
            // передающее прерывание "вручную"
            uart_tx_interrupt();
        }
    }
    tx_buf[tx_head]=ch;
    tx_head=i;
    if(test_bit(_UCSRA,_UDRE))
    {
        uart_tx_interrupt();    // на случай, если UDR освободился до включения прерывания
    }
    return 1;
}
//-----------------------------------------------------------------------------
BYTE UART_SECTION uart_write(const char *str)
{
    BYTE n=0;
    while(*str)
    {
        uart_write(*str);
        str++;
        n++;
    }
    return n;
}
//-----------------------------------------------------------------------------
BYTE UART_SECTION uart_write(const BYTE *buffer, BYTE count)
{
    BYTE n=count;
    while(count--)
    {
        uart_write(*buffer++);
    }
    return n;
}
//-----------------------------------------------------------------------------
BYTE UART_SECTION uart_pwrite(const char *str)      // from PROGMEM
{
    BYTE n=0;
    char ch;
    while(ch=pgm_read_byte(str++))
    {
        uart_write(ch);
        n++;
    }
    return n;
}
//-----------------------------------------------------------------------------
ISR(UART_RX_VECTOR)
{
    uart_rx_interrupt();
}
//-----------------------------------------------------------------------------
ISR(UART_TX_VECTOR)
{
    uart_tx_interrupt();
}
//-----------------------------------------------------------------------------




