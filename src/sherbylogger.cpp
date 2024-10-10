
// // Data will be copied from src to dst
// const char src[] = "Hello, world! (from DMA)";
// char dst[count_of(src)];


//     // SPI initialisation. This example will use SPI at 1MHz.
//     spi_init(SPI_PORT, 1000*1000);
//     gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
//     gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
//     gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
//     gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

//     // Get a free channel, panic() if there are none
//     int chan = dma_claim_unused_channel(true);
    
//     // 8 bit transfers. Both read and write address increment after each
//     // transfer (each pointing to a location in src or dst respectively).
//     // No DREQ is selected, so the DMA transfers as fast as it can.
    
//     dma_channel_config c = dma_channel_get_default_config(chan);
//     channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
//     channel_config_set_read_increment(&c, true);
//     channel_config_set_write_increment(&c, true);
    
//     dma_channel_configure(
//         chan,          // Channel to be configured
//         &c,            // The configuration we just created
//         dst,           // The initial write address
//         src,           // The initial read address
//         count_of(src), // Number of transfers; in this case each is 1 byte.
//         true           // Start immediately.
//     );
    
//     // We could choose to go and do something else whilst the DMA is doing its
//     // thing. In this case the processor has nothing else to do, so we just
//     // wait for the DMA to finish.
//     dma_channel_wait_for_finish_blocking(chan);
    
//     // The DMA has now copied our text from the transmit buffer (src) to the
//     // receive buffer (dst), so we can print it out from there.
//     puts(dst);

//     // Timer example code - This example fires off the callback after 2000ms
//     add_alarm_in_ms(2000, alarm_callback, NULL, false);
//     // For more examples of timer use see https://github.com/raspberrypi/pico-examples/tree/master/timer

//     while (true) {
//         printf("Hello, world!\n");
//         sleep_ms(1000);
//     }
// }
#include <stdio.h>

#include "pico/stdlib.h"

#include "hardware/timer.h"
// #include "hardware/dma.h"
// #include "hardware/spi.h"

#include "HAL/MAX11605_HAL.hpp"
#include "../pico-mcp2515-main/include/mcp2515/mcp2515.h"

#include "f_util.h"
#include "ff.h"
#include "hw_config.h"

// #define SPI_PORT spi0
// #define PIN_MISO 16
// #define PIN_CS   17
// #define PIN_SCK  18
// #define PIN_MOSI 19

#define I2C_PORT i2c1
#define I2C_SDA 26
#define I2C_SCL 27

#define ADC_ADDR 0x65

#define UINT32_T_COUNT 1
#define INT_COUNT 0
#define FLOAT_COUNT 1

MAX11605 max(I2C_PORT, SINGLE, channel_0, VDD, true, false, false, false, ADC_ADDR, I2C_SCL, I2C_SDA, 400 * 1000, 10);

// MCP2515 can(spi0, PICO_DEFAULT_SPI_CSN_PIN, PICO_DEFAULT_SPI_TX_PIN, 20, PICO_DEFAULT_SPI_SCK_PIN, 10000000);
MCP2515 can;
can_frame tx;
can_frame rx;

// sd_card_t *card;
// FATFS fs;


int8_t WriteToFile(FIL* path, const char* data)
{
    if (path == nullptr || data == nullptr)
        return PICO_ERROR_GENERIC;

    if (f_printf(path, "%s", data) < 0)
    {
        panic("write failed");
    }

    return 1;
}

int main()
{
    stdio_init_all();
    
    can.reset();
    can.setBitrate(CAN_500KBPS);
    can.setLoopbackMode();    

    uint16_t mv = 0;
    uint8_t counter = 0;
    uint32_t temps = 0;
    tx.can_id = 0x026 | (1 << 30);
    memset(tx.data, 0, sizeof(tx.data) * sizeof(__u8));
    // // char buffer[(UINT32_T_COUNT*sizeof(uint32_t)) + (INT_COUNT * sizeof(int)) + (FLOAT_COUNT * sizeof(float)) + (3 *sizeof(char))];
    char buffer[16];
    // uint8_t work[FF_MAX_SS];

    // card = sd_get_by_num(0);
    // // f_mkfs("", 0, work, sizeof(work));
    // FRESULT fr = f_mount(&fs, "", 1);
    // if (fr != FR_OK)
    // {
    //     panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    // }

    // FIL file;
    // const char *filename = "test1.csv";

    // fr = f_open(&file, filename, FA_OPEN_APPEND | FA_WRITE);
    // if (FR_OK != fr && FR_EXIST != fr)
    //     panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);

    while (true)
    {
        if (max.GetNewData())
        {
            mv = (uint32_t)((max.buff[0] / 255.f) * 3.2 * 4.2 * 1000);
            temps = time_us_32()/1000;
            
            sprintf(buffer, "%d, %d\n", temps, mv);
            tx.data[0] = (mv >> 24) & 0xFF;
            tx.data[1] = (mv >> 16) & 0xFF;
            tx.data[2] = (mv >> 8) & 0xFF;
            tx.data[3] = (mv >> 0) & 0xFF;
            tx.data[4] = (temps >> 24) & 0xFF;
            tx.data[5] = (temps >> 16) & 0xFF;
            tx.data[6] = (temps >> 8) & 0xFF;
            tx.data[7] = (temps >> 0) & 0xFF;

            tx.can_dlc = sizeof(mv) + sizeof(temps);

            can.sendMessage(&tx);

            if (can.sendMessage(&tx) == MCP2515::ERROR::ERROR_OK)
            {
                printf("ADDR : 0x%x, Buffer : %s", tx.can_id, buffer);
            }

            sleep_us(250);

            if (can.readMessage(&rx) == MCP2515::ERROR::ERROR_OK)
            {
                printf("Message received. ADDR : 0x%x, Data[0] : %d\n", rx.can_id, rx.data[0]);
            }

            // WriteToFile(&file, buffer);

            printf("%s", buffer);
            max.SetNewData(false);
        }

    }

    // fr = f_close(&file);
    // f_unmount("");
    // if (FR_OK != fr)
    // {
    //     printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    // }

    for (;;){}
}