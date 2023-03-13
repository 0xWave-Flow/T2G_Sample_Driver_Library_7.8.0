/***************************************************************************//**
* \file main_cm7_0.c
*
* \brief
* Main file for CM7 #0
*
********************************************************************************
* \copyright
* Copyright 2016-2021, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

/*****************************************************************************
* Include files
*****************************************************************************/
#include "cy_project.h"
#include "cy_device_headers.h"

/* FreeRTOS & FreeRTOS+TCP includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_sockets.h"

#include <stdio.h>

/*****************************************************************************
* Local pre-processor symbols/macros ('define')
*****************************************************************************/
/* LEDs used for toggling (define names are different and not yet harmonized across all devices) */
#if   defined(CY_CB_USER_LED2_PORT)
    #define USER_LED_PORT           CY_CB_USER_LED2_PORT
    #define USER_LED_PIN            CY_CB_USER_LED2_PIN
    #define USER_LED_PIN_MUX        CY_CB_USER_LED2_PIN_MUX
#elif defined(CY_USER_LED2_PORT)
    #define USER_LED_PORT           CY_USER_LED2_PORT
    #define USER_LED_PIN            CY_USER_LED2_PIN
    #define USER_LED_PIN_MUX        CY_USER_LED2_PIN_MUX
#endif

/* Priorities at which the tasks are created. */
#define PRIO_TASK_LED               (tskIDLE_PRIORITY + 1u)
#define PRIO_TASK_PING              (tskIDLE_PRIORITY + 2u)

/* A ping request will be sent by the ping task to this IP address */
#define IP_ADDRESS_TO_PING          "192.168.111.10"

/* UPD port number which is used to receive data that will be echoed via printf (UART) */
#define UDP_RX_PORT                 55555

/* Number of characters to echo from the received UDP payload */
#define UDP_RX_PRINTF_ECHO_LEN      16  

/*****************************************************************************
* Global variable definitions (declared in header file with 'extern')
*****************************************************************************/
TaskHandle_t xPingTaskHandle;
TaskHandle_t xUdpReceiveTaskHandle;
TaskHandle_t xLedTaskHandle;

/*****************************************************************************
* Local type definitions ('typedef')
*****************************************************************************/

/*****************************************************************************
* Local variable definitions ('static')
*****************************************************************************/

/* Use a locally administered MAC address (Bit 1 of first byte is set) */
static const uint8_t ucMACAddress[6] = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* Define the network addressing.  These parameters will be used if either
ipconfigUDE_DHCP is 0 or if ipconfigUSE_DHCP is 1 but DHCP auto configuration
failed. */
static const uint8_t ucIPAddress[4]      = { 192, 168, 111, 20 };
static const uint8_t ucNetMask[4]        = { 255, 255, 255, 0 };
static const uint8_t ucGatewayAddress[4] = { 192, 168, 111, 1 };

/* The following is the address of the Google DNS server. */
static const uint8_t ucDNSServerAddress[4] = { 8, 8, 8, 8 };

static char udpRxPrintfBuf[UDP_RX_PRINTF_ECHO_LEN + 1] = { 0 };

/*****************************************************************************
* Local function prototypes ('static')                                                                            
*****************************************************************************/
static void prvPingTask(void *pvParameters);
static void prvUdpReceiveTask(void *pvParameters);
static void prvLedTask(void *pvParameters);
static void SetupOsExceptions(void);
static void SetupAppInterrupts(void);


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
    __enable_irq();
    
    SystemInit();

    Cy_Semihosting_InitAll(CY_USB_SCB_UART_TYPE, 115200, NULL, false);

    Cy_GPIO_Pin_FastInit(USER_LED_PORT, USER_LED_PIN, CY_GPIO_DM_STRONG, 0, USER_LED_PIN_MUX);

    /* Initialise the RTOS’s TCP/IP stack.  The tasks that use the network
    are created in the vApplicationIPNetworkEventHook() hook function
    below.  The hook function is called when the network connects. */
    FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);
    
    SetupOsExceptions();
    SetupAppInterrupts();    

    xTaskCreate(prvLedTask,  "LED",  configMINIMAL_STACK_SIZE, NULL, PRIO_TASK_LED,  &xLedTaskHandle);

    /* Ping task will be created on the "network up" event in vApplicationIPNetworkEventHook */

    /* Start the OS */
    vTaskStartScheduler();

    /* Should never reach this code. */
    taskDISABLE_INTERRUPTS();

    for(;;)
    {
    }
}


/**
 *****************************************************************************
 ** Task that will send periodic ping requests
 **
 ** \param pvParameters   Parameters for the task
 **
 ** \return none
 *****************************************************************************/
static void prvPingTask(void *pvParameters)
{
    /* Call every 5s. */
    const TickType_t xFrequency = 5000 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    uint32_t ulIpAddr = FreeRTOS_inet_addr(IP_ADDRESS_TO_PING);
    
    xLastWakeTime = xTaskGetTickCount();

    for(;;)
    {
        FreeRTOS_SendPingRequest(ulIpAddr, 8, 100 / portTICK_PERIOD_MS);
        vTaskDelayUntil(&xLastWakeTime, xFrequency)
    }
}


/**
 *****************************************************************************
 ** Task that will create a socket and echo incoming packets via printf (UART)
 **
 ** For testing using 'scapy' the following command can be used
 ** (assuming the example's default IP and port address):
 **
 ** send(IP(dst="192.168.111.20")/UDP(sport=55554, dport=55555)/("abcdefgh"))
 **
 ** 
 ** \param pvParameters   Parameters for the task
 **
 ** \return none
 *****************************************************************************/
static void prvUdpReceiveTask(void *pvParameters)
{
    Socket_t                 sock;
    struct freertos_sockaddr local;
    struct freertos_sockaddr from;
    int                      fromLen;
    int                      result;
    uint8_t *                pPacket;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    sock = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP);

    if (sock != FREERTOS_INVALID_SOCKET)
    {
        /* prepare local address */
        memset(&local, 0, sizeof(local));
        local.sin_port = FreeRTOS_htons(UDP_RX_PORT);

        /* bind to local address */
        if (FreeRTOS_bind(sock, (struct freertos_sockaddr *)&local, sizeof(local)) == 0)
        {
            /* receive RTP packets */
            while(1)
            {
                fromLen = sizeof(from);
                result  = FreeRTOS_recvfrom( sock,
                                             &pPacket,
                                             0,
                                             FREERTOS_ZERO_COPY,
                                             (struct freertos_sockaddr *)&from,
                                             (socklen_t *)&fromLen);
                if(result > 0)
                {
                    // Convert the sender's IP address to the well known human readable format
                    char fromIpAddrHumanReadable[4*3 + 3 + 1]; // 4*3 digits, 3* point separator, 1* \0 termination
                    FreeRTOS_inet_ntop4(&from.sin_addr, fromIpAddrHumanReadable, sizeof(fromIpAddrHumanReadable));

                    // Copy the payload to a separate and length restricted buffer where we can ensure the \0 terminator for printing
                    memcpy(udpRxPrintfBuf, pPacket, CY_MIN(result, UDP_RX_PRINTF_ECHO_LEN));
                    udpRxPrintfBuf[CY_MIN(result, UDP_RX_PRINTF_ECHO_LEN)] = '\0';

                    // Print information about the received packet
                    printf("UDP-RX (from %s:%u) -> %s\n", fromIpAddrHumanReadable, FreeRTOS_ntohs(from.sin_port), udpRxPrintfBuf);

                    // Release the packet buffer now that we do no longer need to access the payload
                    FreeRTOS_ReleaseUDPPayloadBuffer((void *) pPacket);
                }
                else if (result != FREERTOS_EWOULDBLOCK)
                {                    
                }
            }
        }
        FreeRTOS_closesocket(sock);
    }
    vTaskDelete(NULL);
}


/**
 *****************************************************************************
 ** Task that controls a LED
 **
 ** \param pvParameters   Parameters for the task
 **
 ** \return none
 *****************************************************************************/
static void prvLedTask(void *pvParameters)
{
    /* Call every 200 ms. */
    const TickType_t xFrequency = 200 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;

    /* Remove compiler warning about unused parameter. */
    (void) pvParameters;

    xLastWakeTime = xTaskGetTickCount();

    for(;;)
    {
        Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
        vTaskDelayUntil(&xLastWakeTime, xFrequency)
    }
}


/**
 *****************************************************************************
 ** Network event hook
 **
 ** \param eNetworkEvent   Network event
 **
 ** \return none
 *****************************************************************************/
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    static BaseType_t xTasksAlreadyCreated = pdFALSE;

    /* Both eNetworkUp and eNetworkDown events can be processed here. */
    if( eNetworkEvent == eNetworkUp )
    {
        /* Create the tasks that use the TCP/IP stack if they have not already
        been created. */
        if( xTasksAlreadyCreated == pdFALSE )
        {
            /*
             * For convenience, tasks that use FreeRTOS+TCP can be created here
             * to ensure they are not created before the network is usable.
             */
            
            xTaskCreate(prvPingTask,       "Ping",   configMINIMAL_STACK_SIZE,       NULL, PRIO_TASK_PING,                 &xPingTaskHandle);
            xTaskCreate(prvUdpReceiveTask, "UDP-RX", (configMINIMAL_STACK_SIZE * 2), NULL, (ipconfigIP_TASK_PRIORITY - 1), &xUdpReceiveTaskHandle);

            xTasksAlreadyCreated = pdTRUE;
        }
    }
}


/**
 *****************************************************************************
 ** Hook Function for Ping Reply
 **
 ** \param eStatus       Ping status
 ** \param usIdentifier  Identifier
 **
 ** \return none
 *****************************************************************************/
void vApplicationPingReplyHook( ePingReplyStatus_t eStatus, uint16_t usIdentifier )
{
    switch( eStatus )
    {
        case eSuccess :
        case eInvalidChecksum :
        case eInvalidData :
            break;
    }
}


/**
 *****************************************************************************
 ** Provides a sequence number
 **
 ** \param ulSourceAddress       Source IP
 ** \param usSourcePort          Source port
 ** \param ulDestinationAddress  Destination IP
 ** \param usDestinationPort     Destination port
 **
 ** \return Sequence number
 *****************************************************************************/
uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                             uint16_t usSourcePort,
                                             uint32_t ulDestinationAddress,
                                             uint16_t usDestinationPort )
{
    uint32_t seqNum = 0;
    xApplicationGetRandomNumber(&seqNum);
    return seqNum;
}


/**
 *****************************************************************************
 ** Used to provide a random number
 **
 ** \param pulNumber  Pointer where random number will be stored
 **
 ** \return pdTRUE if random number ok, otherwise pdFALSE
 *****************************************************************************/
BaseType_t xApplicationGetRandomNumber( uint32_t * pulNumber )
{
#ifdef __ghs__
    static uint32_t seedState = 0;
    *pulNumber = rand_r(&seedState);
#else
    taskENTER_CRITICAL();
    // Not reentrant
    *pulNumber = rand();
    taskEXIT_CRITICAL();
#endif
    return pdTRUE;
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


/**
 *****************************************************************************
 ** Setup the interrupt handlers of the application
 **
 **
 ** \return none
 *****************************************************************************/
static void SetupAppInterrupts(void)
{
    const cy_stc_sysint_irq_t irq_cfg_ethx_q0 = { .sysIntSrc = CY_GIG_ETH_IRQN0, .intIdx = CPUIntIdx0_IRQn, .isEnabled = true };
    const cy_stc_sysint_irq_t irq_cfg_ethx_q1 = { .sysIntSrc = CY_GIG_ETH_IRQN1, .intIdx = CPUIntIdx0_IRQn, .isEnabled = true };
    const cy_stc_sysint_irq_t irq_cfg_ethx_q2 = { .sysIntSrc = CY_GIG_ETH_IRQN2, .intIdx = CPUIntIdx0_IRQn, .isEnabled = true };

    if(CY_GIG_ETH_TYPE == ETH0)
    {
        Cy_SysInt_SetSystemIrqVector(irq_cfg_ethx_q0.sysIntSrc, Cy_EthIf00_DecodeEvent);
        Cy_SysInt_SetSystemIrqVector(irq_cfg_ethx_q1.sysIntSrc, Cy_EthIf00_DecodeEvent);
        Cy_SysInt_SetSystemIrqVector(irq_cfg_ethx_q2.sysIntSrc, Cy_EthIf00_DecodeEvent);
    }
#ifdef ETH1
    else // currently only up to 2 ETH IP instances
    {
        Cy_SysInt_SetSystemIrqVector(irq_cfg_ethx_q0.sysIntSrc, Cy_EthIf01_DecodeEvent);
        Cy_SysInt_SetSystemIrqVector(irq_cfg_ethx_q1.sysIntSrc, Cy_EthIf01_DecodeEvent);
        Cy_SysInt_SetSystemIrqVector(irq_cfg_ethx_q2.sysIntSrc, Cy_EthIf01_DecodeEvent);
    }
#endif

    Cy_SysInt_InitIRQ(&irq_cfg_ethx_q0);
    Cy_SysInt_InitIRQ(&irq_cfg_ethx_q1);
    Cy_SysInt_InitIRQ(&irq_cfg_ethx_q2);
    
    uint32_t irq_prio = configMAX_API_CALL_INTERRUPT_PRIORITY >> (8U - __NVIC_PRIO_BITS);
    NVIC_SetPriority(CPUIntIdx0_IRQn, irq_prio);
    NVIC_ClearPendingIRQ(CPUIntIdx0_IRQn);
    NVIC_EnableIRQ(CPUIntIdx0_IRQn);
}

/* [] END OF FILE */
