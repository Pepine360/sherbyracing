#include <stdio.h>

#include "FreeRTOS_CLI.h"

#include "SR_FreeRTOS.h"
#include "hardware/gpio.h"
#include "lfs_HAL.h"
#include "IO_HAL.h"

#include "commands.h"

#include <limits.h>

#define MAX_INPUT_LENGTH 50
#define MAX_OUTPUT_LENGTH 100

QueueHandle_t xQueue = NULL;
TimerHandle_t xADC_Timer;

static char *path = "test.txt";
static const uint8_t *const cliWelcomeMsg = "Sherbyracing Datalogger v0.1\r\n";

void vApplicationMallocFailedHook(void)
{
    configASSERT((volatile void *)NULL);
}

void vApplicationIdleHook(void)
{
    volatile size_t xFreeHeapSpace;
    xFreeHeapSpace = xPortGetFreeHeapSize();

    (void)xFreeHeapSpace;
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)pxTask;

    configASSERT((volatile void *)NULL);
}

void vApplicationTickHook(void)
{
}

// void prvWriteToLFSTask(void *pvParameters)
// {
//     for (;;)
//     {
//         xTaskNotifyWait(0x00, ULONG_MAX, NULL, pdMS_TO_TICKS(10));
// 
//         lfs_file_t file;
//         max11605_t *max;
//         xQueuePeek(xQueue, &max, 0);
// 
//         lfs_file_open(&lfs, &file, path, LFS_O_APPEND | LFS_O_RDWR | LFS_O_CREAT);
//         lfs_file_write(&lfs, &file, max->buff, 12);
//         lfs_file_close(&lfs, &file);
// 
//         printf("Wrote to LFS!\n");
//     }
// }

void vTimerCallback(TimerHandle_t xTimer)
{
    max11605_t *adc;
    xQueuePeek(xQueue, &adc, 0);
    float tension = 0;
    MAX11605_Read(adc);
    tension = adc->buff[0] / 255.f;
    tension *= 4.6;
    tension *= 3.2;

    IO_Write_String("ADC Read!\n");
    // xTaskNotify(xTaskGetHandle("LFS_Write"), 1, eNoAction);
}

// void prvMainTask(void* pvParameters)
// {
//     max11605_t *adc = (max11605_t*)pvParameters;
//     bool timerStarted = false;
//     for (;;)
//     {
//         if (appSTART_TIMER && xADC_Timer != NULL && !timerStarted)
//         {
//             xQueueSend(xQueue, (void*)&adc, 0);
//             xTimerStart(xADC_Timer, 0);
//             timerStarted = true;
//         }
//         // vTaskGetRunTimeStats(runtime_stats);
//     }
// }

void vCLITask(void* pvParameters)
{
    uint8_t rxChar, inputIndex = 0;
    BaseType_t xMoreDataToFollow;
    static uint8_t outputString[MAX_OUTPUT_LENGTH], inputString[MAX_INPUT_LENGTH];

    IO_Write_String(cliWelcomeMsg);

    for (;;)
    {
        rxChar = IO_new_char();
        if (rxChar != 0)
        {
            if (rxChar == 0x0d)
            {
                IO_Write_String("\r\n");
                do
                {
                    xMoreDataToFollow = FreeRTOS_CLIProcessCommand(inputString, outputString, MAX_OUTPUT_LENGTH);
                    IO_Write_String(outputString);

                } while (xMoreDataToFollow != pdFALSE);

                
                inputIndex = 0;
                memset(inputString, 0x00, MAX_INPUT_LENGTH);
            }
            else
            {
                if (rxChar == '\r')
                {

                }
                else if (rxChar == 0x7f)
                {
                    if (inputIndex > 0)
                    {
                        --inputIndex;
                        inputString[inputIndex] = '\0';
                    }
                }
                else
                {
                    if (inputIndex < MAX_INPUT_LENGTH)
                    {
                        inputString[inputIndex] = rxChar;
                        ++inputIndex;
                    }
                }
            }
        }
    }
}



static void InitIO(max11605_t* max, uart_inst_t* uart)
{
    stdio_init_all();
    MAX11605_init(max);

    IO_Init();
}

static void InitCore(max11605_t *adc)
{
    xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(struct max11605_t *));
    xQueueSend(xQueue, (void *)adc, 0);

    xADC_Timer = xTimerCreate("ADC_TIMER", 10, pdTRUE, (void *)0, vTimerCallback);
    

    vRegisterSampleCLICommands();

    xTaskCreate(vCLITask, "CLI", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_MAIN_TASK_PRIORITY, NULL);
}

void startFreeRTOS(max11605_t* adc_ex)
{

    // xTaskCreate(prvMainTask, "main", configMINIMAL_STACK_SIZE, (void *)adc_ex, mainQUEUE_MAIN_TASK_PRIORITY, NULL);
    // xTaskCreate(prvWriteToLFSTask, "LFS_Write", 2 * configMINIMAL_STACK_SIZE, NULL, mainLFS_WRITE_TASK_PRIORITY, NULL);
    InitIO(adc_ex, UART_ID);
    InitCore(adc_ex);
    vTaskStartScheduler();
}