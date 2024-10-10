#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

#include "MAX11605_HAL.h"


#define mainQUEUE_MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define mainLFS_WRITE_TASK_PRIORITY (tskIDLE_PRIORITY + 2)

#define mainQUEUE_SEND_TASK_FREQUENCY (100 / portTICK_PERIOD_MS)
#define mainQUEUE_LENGTH 1
#define mainTASK_LED PICO_DEFAULT_LED_PIN

#define appSTART_TIMER 1

void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

void startFreeRTOS(max11605_t *adc);