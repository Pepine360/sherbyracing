#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID uart0
#define BAUDRATE 115200
#define UART_TX 0
#define UART_RX 1

#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

typedef struct {
    char ch;
    bool newChar;
} IO_inst_t;

uint8_t IO_Init();

char IO_new_char();
int8_t IO_Write(const uint8_t *data, size_t len);
int8_t IO_Write_Char(char c);
int8_t IO_Write_String(const char *str);