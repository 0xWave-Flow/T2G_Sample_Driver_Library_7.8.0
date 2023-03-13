/***************************************************************************//**
* \file main_cm7_0.c
*
* \brief
* Main file for CM7 #0
*
********************************************************************************
* \copyright
* Copyright 2016-2020, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

/*****************************************************************************
* Include files
*****************************************************************************/
#include "cy_project.h"
#include "cy_device_headers.h"

/* RTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


/*****************************************************************************
* Local pre-processor symbols/macros ('define')
*****************************************************************************/
/* Map board specific LEDs to task LEDs used for toggling */
#define TASK1_LED_PORT          CY_LED0_PORT
#define TASK1_LED_PIN           CY_LED0_PIN
#define TASK1_LED_PIN_MUX       CY_LED0_PIN_MUX
#define TASK2_LED_PORT          CY_LED1_PORT
#define TASK2_LED_PIN           CY_LED1_PIN
#define TASK2_LED_PIN_MUX       CY_LED1_PIN_MUX
#define TASK3_LED_PORT          CY_LED2_PORT
#define TASK3_LED_PIN           CY_LED2_PIN
#define TASK3_LED_PIN_MUX       CY_LED2_PIN_MUX

/* Priorities at which the tasks are created. */
#define TASK1_PRIORITY          (tskIDLE_PRIORITY + 3u)
#define TASK2_PRIORITY          (tskIDLE_PRIORITY + 2u)
#define TASK3_PRIORITY          (tskIDLE_PRIORITY + 1u)

/*****************************************************************************
* Global variable definitions (declared in header file with 'extern')
*****************************************************************************/

/*****************************************************************************
* Local type definitions ('typedef')
*****************************************************************************/

/*****************************************************************************
* Local variable definitions ('static')
*****************************************************************************/
static SemaphoreHandle_t xSemTrigger1;
static SemaphoreHandle_t xSemTrigger2;

/*****************************************************************************
* Local function prototypes ('static')                                                                            
*****************************************************************************/
static void prvLedTask1(void *pvParameters);
static void prvLedTask2(void *pvParameters);
static void prvLedTask3(void *pvParameters);
static void SetupOsExceptions(void);

/*****************************************************************************
* Function implementation - global ('extern') and local ('static')
*****************************************************************************/


/**
 *****************************************************************************
 ** Main function configures the HW and tasks and starts the OS
 **
 ** \return none
 *****************************************************************************/
int main(void)
{
    SystemInit();
    
    __enable_irq(); /* Enable global interrupts. */

    Cy_GPIO_Pin_FastInit(TASK1_LED_PORT, TASK1_LED_PIN, CY_GPIO_DM_STRONG, 0, TASK1_LED_PIN_MUX);
    Cy_GPIO_Pin_FastInit(TASK2_LED_PORT, TASK2_LED_PIN, CY_GPIO_DM_STRONG, 0, TASK2_LED_PIN_MUX);
    Cy_GPIO_Pin_FastInit(TASK3_LED_PORT, TASK3_LED_PIN, CY_GPIO_DM_STRONG, 0, TASK3_LED_PIN_MUX);
    
    SetupOsExceptions();
    
    xSemTrigger1 = xSemaphoreCreateBinary();
    xSemTrigger2 = xSemaphoreCreateBinary();
	
    xTaskCreate(prvLedTask1, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK1_PRIORITY, NULL);
    xTaskCreate(prvLedTask2, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK2_PRIORITY, NULL);
    xTaskCreate(prvLedTask3, "Task3", configMINIMAL_STACK_SIZE, NULL, TASK3_PRIORITY, NULL);

    /* Start the OS */
    vTaskStartScheduler();

    for(;;)
    {
        /* Should never reach this code. */
        taskDISABLE_INTERRUPTS();
        while(1)
        {        
        }
    }
}


/**
 *****************************************************************************
 ** Task that controls LED1
 **
 ** \param pvParameters   Parameters for the task
 **
 ** \return none
 *****************************************************************************/
static void prvLedTask1(void *pvParameters)
{
    /* Block for 200ms. */
    const TickType_t xDelay = 200 / portTICK_PERIOD_MS;

    /* Remove compiler warning about unused parameter. */
    (void) pvParameters;

    for(;;)
    {
        Cy_GPIO_Inv(TASK1_LED_PORT, TASK1_LED_PIN);
        xSemaphoreGive(xSemTrigger1);        
        vTaskDelay(xDelay);
    }
}

/**
 *****************************************************************************
 ** Task that controls LED2
 **
 ** \param pvParameters   Parameters for the task
 **
 ** \return none
 *****************************************************************************/
static void prvLedTask2(void *pvParameters)
{
    /* Block for 50ms. */
    const TickType_t xDelay = 50 / portTICK_PERIOD_MS;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    for(;;)
    {
        if(xSemaphoreTake(xSemTrigger1, (TickType_t ) portMAX_DELAY) == pdTRUE)
        {
            vTaskDelay(xDelay);
            Cy_GPIO_Inv(TASK2_LED_PORT, TASK2_LED_PIN);
            xSemaphoreGive(xSemTrigger2);
        }
    }
}

/**
 *****************************************************************************
 ** Task that controls LED3
 **
 ** \param pvParameters   Parameters for the task
 **
 ** \return none
 *****************************************************************************/
static void prvLedTask3(void *pvParameters)
{
    /* Block for 50ms. */
    const TickType_t xDelay = 50 / portTICK_PERIOD_MS;

    /* Remove compiler warning about unused parameter. */
    (void) pvParameters;

    for(;;)
    {
        if(xSemaphoreTake(xSemTrigger2, (TickType_t) portMAX_DELAY) == pdTRUE)
        {
            vTaskDelay(xDelay);
            Cy_GPIO_Inv(TASK3_LED_PORT, TASK3_LED_PIN);
        }
    }
}

/**
 *****************************************************************************
 ** RTOS idle hook
 **
 **
 ** \return none
 *****************************************************************************/
void vApplicationIdleHook(void)
{
    /* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
    FreeRTOSConfig.h. This function is called on each cycle of the idle task. */
}

/**
 *****************************************************************************
 ** RTOS "malloc failed" hook if not enough memory is left on the heap
 **
 **
 ** \return none
 *****************************************************************************/
void vApplicationMallocFailedHook(void)
{
    /* The heap space has been exceeded. */

    taskDISABLE_INTERRUPTS();
    
    while(1)
    {
        /* Do nothing - this is a placeholder for a breakpoint */
    }
}


/**
 *****************************************************************************
 ** RTOS "stack overflow" hook if stack overflow has been detected by the RTOS
 **
 ** \param xTask       Handle of the task for which overflow has been detected
 ** \param pcTaskName  Name of the task
 **
 ** \return none
 *****************************************************************************/
void vApplicationStackOverflowHook(TaskHandle_t xTask, char_t *pcTaskName)
{
    /* The stack space has been exceeded for a task */
    
    (void)xTask;
    (void)pcTaskName;

    taskDISABLE_INTERRUPTS();
    
    while(1)
    {
    }
}


/**
 *****************************************************************************
 ** Setup the exception handlers of the RTOS
 **
 **
 ** \return none
 *****************************************************************************/
static void SetupOsExceptions(void)
{
    /* Handler for Cortex Supervisor Call (SVC, formerly SWI) - address 11 */
    Cy_SysInt_SetVector(SVCall_IRQn, (cy_israddress)vPortSVCHandler);

    /* Handler for Cortex PendSV Call - address 14 */
    Cy_SysInt_SetVector(PendSV_IRQn, (cy_israddress)xPortPendSVHandler);

    /* Handler for Cortex SYSTICK - address 15 */
    Cy_SysInt_SetVector(SysTick_IRQn, (cy_israddress)xPortSysTickHandler);
}


/* [] END OF FILE */
