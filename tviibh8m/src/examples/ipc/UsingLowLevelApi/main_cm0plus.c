/***************************************************************************//**
* \file main_cm0plus.c
*
* \version 1.0
*
* \brief Main example file for CM0+
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

#define IPC_NOTIFY_CPU_IRQ_INDEX    CPUIntIdx2_IRQn


cy_stc_gpio_pin_config_t user_led0_port_pin_cfg =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = CY_LED0_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

cy_stc_gpio_pin_config_t user_led1_port_pin_cfg =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = CY_LED1_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

cy_stc_gpio_pin_config_t user_led2_port_pin_cfg =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = CY_LED2_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

#define IPC_RELEASE_INT_NUMBER  6   /* Release interrupt number. This interrupt is handled by notifier core (CM7) */
#define IPC_NOTIFY_INT_NUMBER   7   /* Notify interrupt number. This interrupt is handled by notified core (CM0+) */

static cy_stc_sysint_irq_t stcSysIntIpcNotifyInt = 
{
    .sysIntSrc = (cy_en_intr_t)(cpuss_interrupts_ipc_0_IRQn + USED_IPC_CHANNEL),
    .intIdx    = IPC_NOTIFY_CPU_IRQ_INDEX,
    .isEnabled = true
};

/*****************************************************************************
 ** \brief IpcNotifyInt_ISR
 *****************************************************************************/
void IpcNotifyInt_ISR(void)
{
    uint32_t interruptMasked = 
            Cy_IPC_Drv_ExtractAcquireMask
            (
                Cy_IPC_Drv_GetInterruptStatusMasked
                (
                    Cy_IPC_Drv_GetIntrBaseAddr(IPC_NOTIFY_INT_NUMBER)
                )
            );
    /* Check if the interrupt is caused by the notifier channel */
    if (interruptMasked == (1uL << USED_IPC_CHANNEL))
    {
        /* Clear the interrupt */
        Cy_IPC_Drv_ClearInterrupt
        (
            Cy_IPC_Drv_GetIntrBaseAddr(IPC_NOTIFY_INT_NUMBER), 
            CY_IPC_NO_NOTIFICATION,
            interruptMasked
        );

        uint32_t u32Led;
        if(CY_IPC_DRV_SUCCESS == Cy_IPC_Drv_ReadMsgWord(Cy_IPC_Drv_GetIpcBaseAddress(USED_IPC_CHANNEL), &u32Led))
        {
            switch(u32Led)
            {
                case 0:
                    Cy_GPIO_Write(CY_LED0_PORT,   CY_LED0_PIN,    1); //Led 0 On 
                    Cy_GPIO_Write(CY_LED1_PORT,   CY_LED1_PIN,    0); //Led 1 Off
                    Cy_GPIO_Write(CY_LED2_PORT,   CY_LED2_PIN,    0); //Led 2 Off
                    break;
                case 1:
                    Cy_GPIO_Write(CY_LED0_PORT,   CY_LED0_PIN,    0); //Led 0 Off 
                    Cy_GPIO_Write(CY_LED1_PORT,   CY_LED1_PIN,    1); //Led 1 On
                    Cy_GPIO_Write(CY_LED2_PORT,   CY_LED2_PIN,    0); //Led 2 Off
                    break;
                case 2:
                    Cy_GPIO_Write(CY_LED0_PORT,   CY_LED0_PIN,    0); //Led 0 Off 
                    Cy_GPIO_Write(CY_LED1_PORT,   CY_LED1_PIN,    0); //Led 1 Off
                    Cy_GPIO_Write(CY_LED2_PORT,   CY_LED2_PIN,    1); //Led 2 On
                    break;
                default:
                    break;
            }
        }
        /* Finally relase the lock */
        Cy_IPC_Drv_ReleaseNotify(Cy_IPC_Drv_GetIpcBaseAddress(USED_IPC_CHANNEL), (1u << IPC_RELEASE_INT_NUMBER));
    }
}

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);

    /* Setup IPC interrupt line */
    Cy_SysInt_InitIRQ(&stcSysIntIpcNotifyInt);
    
    /* Setup IPC interrupt system handler */
    Cy_SysInt_SetSystemIrqVector((cy_en_intr_t)(cpuss_interrupts_ipc_0_IRQn + USED_IPC_CHANNEL), IpcNotifyInt_ISR);
    
    NVIC_ClearPendingIRQ(IPC_NOTIFY_CPU_IRQ_INDEX);
    NVIC_EnableIRQ(IPC_NOTIFY_CPU_IRQ_INDEX);
    
    
    
    /* Don't set the release interrupt. */
    /* If user needs to use the release interrupt on the client side (in this example CM4)
     * user must set it after CM0+ server works.
     */
    Cy_IPC_Drv_SetInterruptMask
    (
        Cy_IPC_Drv_GetIntrBaseAddr(IPC_NOTIFY_INT_NUMBER),
        CY_IPC_NO_NOTIFICATION,
        (1uL << USED_IPC_CHANNEL)
    );


    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(CY_LED0_PORT, CY_LED0_PIN, &user_led0_port_pin_cfg);
    Cy_GPIO_Pin_Init(CY_LED1_PORT, CY_LED1_PIN, &user_led1_port_pin_cfg);
    Cy_GPIO_Pin_Init(CY_LED2_PORT, CY_LED2_PIN, &user_led2_port_pin_cfg);

    for(;;)
    {
        
    }
}



/* [] END OF FILE */
