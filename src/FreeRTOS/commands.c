/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/******************************************************************************
 *
 * https://www.FreeRTOS.org/cli
 *
 ******************************************************************************/

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

#include "commands.h"

#ifndef configINCLUDE_TRACE_RELATED_CLI_COMMANDS
#define configINCLUDE_TRACE_RELATED_CLI_COMMANDS 0
#endif

#ifndef configINCLUDE_QUERY_HEAP_COMMAND
#define configINCLUDE_QUERY_HEAP_COMMAND 0
#endif

extern QueueHandle_t xQueue;
extern TimerHandle_t xADC_Timer;

/*
 * Implements the task-stats command.
 */
static BaseType_t prvTaskStatsCommand(char *pcWriteBuffer,
                                      size_t xWriteBufferLen,
                                      const char *pcCommandString);

/*
 * Implements the echo-parameters command.
 */
static BaseType_t prvParameterEchoCommand(char *pcWriteBuffer,
                                          size_t xWriteBufferLen,
                                          const char *pcCommandString);

static BaseType_t prvStartADCCommand(char *pcWriteBuffer,
                                     size_t xWriteBufferLen,
                                     const char *pcCommandString);

static BaseType_t prvStopADCCommand(char *pcWriteBuffer,
                                    size_t xWriteBufferLen,
                                    const char *pcCommandString);

/* Structure that defines the "task-stats" command line command.  This generates
 * a table that gives information on each task in the system. */
static const CLI_Command_Definition_t xTaskStats =
    {
        "task-stats", /* The command string to type. */
        "\r\ntask-stats:\r\n Displays a table showing the state of each FreeRTOS task\r\n",
        prvTaskStatsCommand, /* The function to run. */
        0                    /* No parameters are expected. */
};

/* Structure that defines the "echo_parameters" command line command.  This
 * takes a variable number of parameters that the command simply echos back one at
 * a time. */
static const CLI_Command_Definition_t xParameterEcho =
    {
        "echo-parameters",
        "\r\necho-parameters <...>:\r\n Take variable number of parameters, echos each in turn\r\n",
        prvParameterEchoCommand, /* The function to run. */
        -1                       /* The user can enter any number of commands. */
};

static const CLI_Command_Definition_t xStartADC =
    {
        "start-adc",
        "\r\nstart-adc <...>:\r\n Take channel and start reading it\r\n",
        prvStartADCCommand, /* The function to run. */
        -1                  /* The user can enter any number of commands. */
};

static const CLI_Command_Definition_t xStopADC =
    {
        "stop-adc",
        "\r\nstart-adc:\r\n stop reading ADC\r\n",
        prvStopADCCommand, /* The function to run. */
        -1                 /* The user can enter any number of commands. */
};

/*-----------------------------------------------------------*/

void vRegisterSampleCLICommands(void)
{
    /* Register all the command line commands defined immediately above. */
    FreeRTOS_CLIRegisterCommand(&xTaskStats);
    FreeRTOS_CLIRegisterCommand(&xParameterEcho);
    FreeRTOS_CLIRegisterCommand(&xStartADC);
    FreeRTOS_CLIRegisterCommand(&xStopADC);
}
/*-----------------------------------------------------------*/

static BaseType_t prvTaskStatsCommand(char *pcWriteBuffer,
                                      size_t xWriteBufferLen,
                                      const char *pcCommandString)
{
    const char *const pcHeader = "     State   Priority  Stack    #\r\n************************************************\r\n";
    BaseType_t xSpacePadding;

    /* Remove compile time warnings about unused parameters, and check the
     * write buffer is not NULL.  NOTE - for simplicity, this example assumes the
     * write buffer length is adequate, so does not check for buffer overflows. */
    (void)pcCommandString;
    (void)xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Generate a table of task stats. */
    strcpy(pcWriteBuffer, "Task");
    pcWriteBuffer += strlen(pcWriteBuffer);

    /* Minus three for the null terminator and half the number of characters in
     * "Task" so the column lines up with the centre of the heading. */
    configASSERT(configMAX_TASK_NAME_LEN > 3);

    for (xSpacePadding = strlen("Task"); xSpacePadding < (configMAX_TASK_NAME_LEN - 3); xSpacePadding++)
    {
        /* Add a space to align columns after the task's name. */
        *pcWriteBuffer = ' ';
        pcWriteBuffer++;

        /* Ensure always terminated. */
        *pcWriteBuffer = 0x00;
    }

    strcpy(pcWriteBuffer, pcHeader);
    vTaskList(pcWriteBuffer + strlen(pcHeader));

    /* There is no more data to return after this single string, so return
     * pdFALSE. */
    return pdFALSE;
}
/*-----------------------------------------------------------*/

static BaseType_t prvParameterEchoCommand(char *pcWriteBuffer,
                                          size_t xWriteBufferLen,
                                          const char *pcCommandString)
{
    const char *pcParameter;
    BaseType_t xParameterStringLength, xReturn;
    static UBaseType_t uxParameterNumber = 0;

    /* Remove compile time warnings about unused parameters, and check the
     * write buffer is not NULL.  NOTE - for simplicity, this example assumes the
     * write buffer length is adequate, so does not check for buffer overflows. */
    (void)pcCommandString;
    (void)xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    if (uxParameterNumber == 0)
    {
        /* The first time the function is called after the command has been
         * entered just a header string is returned. */
        sprintf(pcWriteBuffer, "The parameters were:\r\n");

        /* Next time the function is called the first parameter will be echoed
         * back. */
        uxParameterNumber = 1U;

        /* There is more data to be returned as no parameters have been echoed
         * back yet. */
        xReturn = pdPASS;
    }
    else
    {
        /* Obtain the parameter string. */
        pcParameter = FreeRTOS_CLIGetParameter(
            pcCommandString,        /* The command string itself. */
            uxParameterNumber,      /* Return the next parameter. */
            &xParameterStringLength /* Store the parameter string length. */
        );

        if (pcParameter != NULL)
        {
            /* Return the parameter string. */
            memset(pcWriteBuffer, 0x00, xWriteBufferLen);
            sprintf(pcWriteBuffer, "%d: ", (int)uxParameterNumber);
            strncat(pcWriteBuffer, (char *)pcParameter, (size_t)xParameterStringLength);
            strncat(pcWriteBuffer, "\r\n", strlen("\r\n"));

            /* There might be more parameters to return after this one. */
            xReturn = pdTRUE;
            uxParameterNumber++;
        }
        else
        {
            /* No more parameters were found.  Make sure the write buffer does
             * not contain a valid string. */
            pcWriteBuffer[0] = 0x00;

            /* No more data to return. */
            xReturn = pdFALSE;

            /* Start over the next time this command is executed. */
            uxParameterNumber = 0;
        }
    }

    return xReturn;
}

static BaseType_t prvStartADCCommand(char *pcWriteBuffer,
                                     size_t xWriteBufferLen,
                                     const char *pcCommandString)
{
    xTimerStart(xADC_Timer, 0);
}

static BaseType_t prvStopADCCommand(char *pcWriteBuffer,
                                    size_t xWriteBufferLen,
                                    const char *pcCommandString)
{
    xTimerStop(xADC_Timer, 0);
}