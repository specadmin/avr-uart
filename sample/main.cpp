//-----------------------------------------------------------------------------
#include <stdlib.h>
#include "lib/avr-misc/avr-misc.h"
#include "lib/avr-uart/uart.h"
//-----------------------------------------------------------------------------
int main()
{
    uart_init(BAUD_115200);

    enable_interrupts();

    while(1)
    {
        uart_write("ABCD ");
        mdelay(100);
    }

    return 0;
}
//-----------------------------------------------------------------------------
