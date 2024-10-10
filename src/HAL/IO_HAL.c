#include "IO_HAL.h"

static IO_inst_t io;

static void on_uart_rx()
{
    // IO_Write_String("Interrup!\n");
    if (uart_is_readable(UART_ID))
    {
        io.ch = uart_getc(UART_ID);
        io.newChar = true;
        IO_Write_Char(io.ch);
    }
}

uint8_t IO_Init()
{
    uart_init(uart0, 115200);

    gpio_set_function(0, UART_FUNCSEL_NUM(uart0, 0));
    gpio_set_function(1, UART_FUNCSEL_NUM(uart0, 1));

    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    uart_set_fifo_enabled(UART_ID, false);

    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);
}

char IO_new_char()
{
    if (io.newChar)
    {
        io.newChar = false;
        return io.ch;
    }
    
    return 0;
}

int8_t IO_Write(const uint8_t *data, size_t len)
{
    if (UART_ID == NULL || data == NULL)
        return PICO_ERROR_GENERIC;

    if (uart_is_writable(UART_ID))
    {
        uart_write_blocking(UART_ID, data, len);
    }

    uart_tx_wait_blocking(UART_ID);

    return 1;
}

int8_t IO_Write_Char(char c)
{
    if (UART_ID == NULL)
        return PICO_ERROR_GENERIC;

    if (uart_is_writable(UART_ID))
    {
        uart_putc_raw(UART_ID, c);
    }

    return 1;
}

int8_t IO_Write_String(const char* str)
{
    if (UART_ID == NULL)
        return PICO_ERROR_GENERIC;

    uart_puts(UART_ID, str);

    uart_tx_wait_blocking(UART_ID);

    return 1;
}