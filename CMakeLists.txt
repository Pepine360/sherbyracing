# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(src/pico_sdk_import.cmake)
# include(FreeRTOS_Kernel_import.cmake)

project(sherbylogger C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(sherbylogger 
    src/sherbylogger.cpp
    src/HAL/MAX11605_HAL.cpp 
    src/hw_config.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/ff15/source/ff.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/ff15/source/ffsystem.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/ff15/source/ffunicode.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/sd_driver/dma_interrupts.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/sd_driver/sd_card.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/sd_driver/SDIO/rp2040_sdio.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/sd_driver/SDIO/sd_card_sdio.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/sd_driver/SPI/my_spi.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/sd_driver/SPI/sd_card_spi.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/sd_driver/SPI/sd_spi.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/src/crash.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/src/crc.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/src/f_util.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/src/ff_stdio.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/src/file_stream.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/src/glue.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/src/my_debug.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/src/my_rtc.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/src/sd_timeouts.c
    ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/src/util.c
    ${CMAKE_CURRENT_LIST_DIR}/pico-mcp2515-main/include/mcp2515/mcp2515.cpp
) #SR_FreeRTOS.c littlefs/lfs.c littlefs/lfs_util.c lfs_HAL.c IO_HAL.c FreeRTOS_CLI.c commands.c)

pico_set_program_name(sherbylogger "sherbylogger")
pico_set_program_version(sherbylogger "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(sherbylogger 1)
pico_enable_stdio_usb(sherbylogger 1)

# Add the standard library to the build
target_link_libraries(sherbylogger
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(sherbylogger PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. 
  ${CMAKE_CURRENT_LIST_DIR}/src
  ${CMAKE_CURRENT_LIST_DIR}/src/HAL
  ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src
  ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/ff15/source# for our common lwipopts or any other standard includes, if required
  ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/include# for our common lwipopts or any other standard includes, if required
  ${CMAKE_CURRENT_LIST_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico-main/src/sd_driver
  ${CMAKE_CURRENT_LIST_DIR}/pico-mcp2515-main/include/
)

# Add any user requested libraries
target_link_libraries(sherbylogger 
        hardware_spi
        hardware_i2c
        hardware_dma
        # hardware_flash
        # hardware_sync
        hardware_gpio
        hardware_timer
        # hardware_uart
        # hardware_rtc
        hardware_pio
        pico_aon_timer
        # no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
        cmsis_core
        #FreeRTOS-Kernel
        #FreeRTOS-Kernel-Heap1
        )

pico_add_extra_outputs(sherbylogger)

