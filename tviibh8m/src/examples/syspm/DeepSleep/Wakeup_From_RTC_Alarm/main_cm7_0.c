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

/*******************************************************************************
*    Parameters definitions
*******************************************************************************/
const cy_stc_gpio_pin_config_t user_led3_port_pin_cfg =
{
    .outVal    = 0u,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = CY_LED3_PIN_MUX,
    .intEdge   = 0u,
    .intMask   = 0u,
    .vtrip     = 0u,
    .slewRate  = 0u,
    .driveSel  = 0u,
};

// Initialize all SYSTEM_INT_STRUCTS
const cy_stc_sysint_irq_t irq_cfg =
{
    .sysIntSrc = srss_interrupt_backup_IRQn,
    .intIdx    = CPUIntIdx2_IRQn,
    .isEnabled = true,
};

void RTC_Handler(void)
{
    // CM0+ will handle
}

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    Cy_GPIO_Pin_Init(CY_LED3_PORT, CY_LED3_PIN, &user_led3_port_pin_cfg);

    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, RTC_Handler);
    NVIC_SetPriority(irq_cfg.intIdx, 3ul);
    NVIC_EnableIRQ(irq_cfg.intIdx);

    uint8_t count = 0u;
    for(;;)
    {
        // wait 0.05 [s]
        Cy_SysTick_DelayInUs(50000ul);

        Cy_GPIO_Inv(CY_LED3_PORT, CY_LED3_PIN);

        count++;
        if (count > 200u)
        {
            count = 0u;
            Cy_GPIO_Write(CY_LED3_PORT, CY_LED3_PIN, 0ul);
            Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
        }
    }
}

/* [] END OF FILE */
