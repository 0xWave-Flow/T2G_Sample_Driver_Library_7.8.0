/***************************************************************************//**
* \file main_cm7_0.c
*
* \version 1.0
*
* \brief Main example file for CM7_0
*
********************************************************************************
* \copyright
* Copyright 2016-2020, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"

#define USED_IPC_CHANNEL 7

// #define IPC_RELEASE_INT_NUMBER  6   /* Release interrupt number. This interrupt is handled by notifier core (CM7) */ /* In this example polling is used instead of release IRQ */
#define IPC_NOTIFY_INT_NUMBER   7   /* Notify interrupt number. This interrupt is handled by notified core (CM0+) */


//########################################################
// DO NOT RUN THE SAME EXAMPLE CODE ON THE OTHER CM7 CORE!
//########################################################


int main(void)
{
    SystemInit();

    // Example had been originally tested with "cache off", so ensure that caches are turned off (may have been enabled by new startup.c module)
    SCB_DisableICache(); // Disables and invalidates instruction cache
    SCB_DisableDCache(); // Disables, cleans and invalidates data cache

    uint32_t    u32Led= 0;
    __enable_irq(); /* Enable global interrupts. */

    /* At first force release the lock state. */
    (void)Cy_IPC_Drv_LockRelease(Cy_IPC_Drv_GetIpcBaseAddress(USED_IPC_CHANNEL), CY_IPC_NO_NOTIFICATION);

    /* Wait until the CM0+ IPC server is started */
    /* Note:
     * After the CM0+ IPC server is started, the corresponding number of the INTR_MASK is set.
     * So in this case CM7 can recognize whether the server has started or not by the INTR_MASK status.
     */
    while (((1uL << (USED_IPC_CHANNEL)) !=  
        Cy_IPC_Drv_ExtractAcquireMask(Cy_IPC_Drv_GetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(USED_IPC_CHANNEL)))))
    {
    }

    for(;;)
    {
        // Wait 0.5 [s]
        Cy_SysTick_DelayInUs(500000);

        /* Send the message to the M0+ through IPC */
        Cy_IPC_Drv_SendMsgWord
        (
            Cy_IPC_Drv_GetIpcBaseAddress(USED_IPC_CHANNEL),
            (1u << IPC_NOTIFY_INT_NUMBER),
            u32Led
        );

        /* Wait until the CM0+ get the message and release the lock. */
        bool status = true;
        do
        {
            status = Cy_IPC_Drv_IsLockAcquired(Cy_IPC_Drv_GetIpcBaseAddress(USED_IPC_CHANNEL));
        } while(status);

        u32Led = (u32Led + 1u) % 3u;
    }
}



/* [] END OF FILE */
