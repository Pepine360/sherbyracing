#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"

// #include "HAL/MAX11605_HAL.hpp"

// #include "f_util.h"
// #include "ff.h"
#include "hw_config.h"

#ifdef __cplusplus
extern "C"
{
    #include "can2040.h"
}
#endif

#define I2C_PORT i2c1
#define I2C_SDA 26
#define I2C_SCL 27

#define ADC_ADDR 0x65

#define UINT32_T_COUNT 1
#define INT_COUNT 0
#define FLOAT_COUNT 6

// MAX11605 max(I2C_PORT, SINGLE, channel_0, VDD, true, false, false, false, ADC_ADDR, I2C_SCL, I2C_SDA, 400 * 1000, 10);

bool new_can_msg = false;
uint8_t can_msg_arr[8];
uint8_t can_dlc;

// static struct can2040 bus;
static struct can2040_msg can_msg;
static struct can2040_stats stats;

// static void can2040_cb(struct can2040* cd, uint32_t notify, struct can2040_msg* msg)
// {
//     printf("cb called!\n");
//     // if (notify == CAN2040_NOTIFY_RX)
//     // {
//     //     new_can_msg = true;
//     //     memcpy(can_msg_arr, msg->data, msg->dlc);
//     //     can_dlc = msg->dlc;
//     // }

//     // if (notify == CAN2040_NOTIFY_TX)
//     // {
//     //     printf("Message sent!\n");
//     // }
// }

static struct can2040 cbus;

static void
can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    // Add message processing code here...
    printf("callback called!\n");
}

static void
PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void)
{
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 500000;
    uint32_t gpio_rx = 4, gpio_tx = 5;

    // Setup canbus
    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0_IRQn, PIOx_IRQHandler);
    NVIC_SetPriority(PIO0_IRQ_0_IRQn, 1);
    NVIC_EnableIRQ(PIO0_IRQ_0_IRQn);

    // Start canbus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

// int8_t SetupSDCard(size_t sdCardNum, FATFS *fs, const char *fsPath, BYTE opt, const char *filePath, BYTE fileOpts, FIL *file, sd_card_t* card)
// {
//     card = sd_get_by_num(0);
//     FRESULT fr = f_mount(fs, fsPath, opt);
//     if (fr != FR_OK)
//     {
//         panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
//     }

//     fr = f_open(file, filePath, fileOpts);
//     if (FR_OK != fr && FR_EXIST != fr)
//         panic("f_open(%s) error: %s (%d)\n", filePath, FRESULT_str(fr), fr);

//     return PICO_ERROR_NONE;
// }

// int8_t WriteToFile(FIL* path, const char* data)
// {
//     if (path == nullptr || data == nullptr)
//         return PICO_ERROR_GENERIC;

//     if (f_printf(path, "%s", data) < 0)
//     {
//         panic("write failed");
//     }

//     return PICO_ERROR_NONE;
// }

int main()
{
    stdio_init_all();

    // SetupCAN();
    canbus_setup();

    uint16_t tension = 0;
    uint32_t temps = 0;
    can_msg.id = 0xAA;
    
    // char buffer[(UINT32_T_COUNT*sizeof(uint32_t)) + (INT_COUNT * sizeof(int)) + (FLOAT_COUNT * sizeof(float)) + (3 *sizeof(char))];
    char buffer[16];
    // uint8_t work[FF_MAX_SS];

    // sd_card_t*card;
    // FATFS fs;

    // const char* filename = "test1.csv";
    // FIL file;

    // SetupSDCard(0, &fs, "", 1, filename, FA_OPEN_APPEND | FA_WRITE, &file, card);

    while (temps < 100)
    {
        // if (max.GetNewData())
        // {
            // tension = max.buff[0] / 255.f * 3.2 * 4.2;
            temps = time_us_32()/1000;

            can_msg.data[0] = temps;
            can_msg.data[1] = tension;

            can_msg.dlc = 2;

            sprintf(buffer, "%d, %d\n", temps, tension);

            // WriteToFile(&file, buffer);

            if (can2040_check_transmit(&cbus))
                if (can2040_transmit(&cbus, &can_msg))
                    printf("Error sending can msg!\n");

            printf("%s", buffer);
            // max.SetNewData(false);
        // }

            sleep_ms(100);

        //     if (new_can_msg)
        //     {
        //         for (int i = 0; i < can_dlc; ++i)
        //         {
        //             printf("Byte %d : %d\n", i, can_msg_arr[i]);
        //         }
        //         new_can_msg = false;
        // }
    }

    // uint32_t rx_total, tx_total;
    // uint32_t tx_attempt;
    // uint32_t parse_error;

    can2040_get_statistics(&cbus, &stats);

    printf("rx_total : %d\ntx_total : %d\ntx_attempt : %d\nparse_error : %d\n", stats.rx_total, stats.tx_total, stats.tx_attempt, stats.parse_error);

    // FRESULT fr = f_close(&file);
    // f_unmount("");
    // if (FR_OK != fr)
    // {
    //     printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    // }

    for (;;){}
}