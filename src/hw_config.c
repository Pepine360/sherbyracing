
/* hw_config.c
Copyright 2021 Carl John Kugler III

Licensed under the Apache License, Version 2.0 (the License); you may not use
this file except in compliance with the License. You may obtain a copy of the
License at

   http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
*/
/*
This file should be tailored to match the hardware design.

See
https://github.com/carlk3/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/tree/main#customizing-for-the-hardware-configuration
*/

#include "hw_config.h"

#ifdef __cplusplus
extern "C"
{
#endif
    /* Configuration of RP2040 hardware SPI object */
    static spi_t spi = {
        .hw_inst = spi1, // RP2040 SPI component
        .sck_gpio = 14,   // GPIO number (not Pico pin number)
        .mosi_gpio = 15,
        .miso_gpio = 12,
        .baud_rate = 12 * 1000 * 1000, // Actual frequency: 10416666.
        .no_miso_gpio_pull_up = true
    };

    /* SPI Interface */
    static sd_spi_if_t spi_if = {
        .spi = &spi, // Pointer to the SPI driving this card
        .ss_gpio = 9 // The SPI slave select GPIO for this SD card
    };

    /* Configuration of the SD Card socket object */
    static sd_card_t sd_card = {
        .type = SD_IF_SPI,
        .spi_if_p = &spi_if, // Pointer to the SPI interface driving this card
        .use_card_detect = false
    };

    /* ********************************************************************** */

    size_t sd_get_num() { return 1; }

    sd_card_t *sd_get_by_num(size_t num)
    {
        if (0 == num)
        {
            return &sd_card;
        }
        else
        {
            return NULL;
        }
    }
#ifdef __cplusplus
}
#endif
/* [] END OF FILE */