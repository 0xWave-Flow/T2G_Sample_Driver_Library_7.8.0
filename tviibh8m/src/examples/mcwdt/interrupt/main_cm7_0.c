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

cy_stc_gpio_pin_config_t user_led_port_pin_cfg =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = CY_LED8_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

#define MCWDT_TICKS_PER_SECOND     (32000u)    /* 1 sec when clk_lf = 32 KHz */

#define MCWDT_CPU_IRQ_INDEX     CPUIntIdx2_IRQn

cy_stc_sysint_irq_t irq_cfg =
{
    .sysIntSrc  = srss_interrupt_mcwdt_1_IRQn,
    .intIdx     = MCWDT_CPU_IRQ_INDEX,
    .isEnabled  = true,
};

cy_stc_mcwdt_config_t mcwdtConfig = 
{
    .coreSelect       = CY_MCWDT_PAUSED_BY_DPSLP_CM4_CM7_0,
    .c0LowerLimit     = 0u,
    .c0UpperLimit     = 0xFFFFu,
    .c0WarnLimit      = MCWDT_TICKS_PER_SECOND,  /* 1 sec */
    .c0LowerAction    = CY_MCWDT_ACTION_NONE,
    .c0UpperAction    = CY_MCWDT_ACTION_NONE,
    .c0WarnAction     = CY_MCWDT_WARN_ACTION_INT,
    .c0AutoService    = CY_MCWDT_ENABLE,
    .c0SleepDeepPause = CY_MCWDT_ENABLE,
    .c0DebugRun       = CY_MCWDT_ENABLE,
    .c1LowerLimit     = 0u,
    .c1UpperLimit     = 0xFFFFu,
    .c1WarnLimit      = MCWDT_TICKS_PER_SECOND * 2u, /* 2 sec */
    .c1LowerAction    = CY_MCWDT_ACTION_NONE,
    .c1UpperAction    = CY_MCWDT_ACTION_NONE,
    .c1WarnAction     = CY_MCWDT_WARN_ACTION_INT,
    .c1AutoService    = CY_MCWDT_ENABLE,
    .c1SleepDeepPause = CY_MCWDT_ENABLE,
    .c1DebugRun       = CY_MCWDT_ENABLE,
    .c2ToggleBit      = CY_MCWDT_CNT2_MONITORED_BIT15,
    .c2Action         = CY_MCWDT_CNT2_ACTION_INT,
    .c2SleepDeepPause = CY_MCWDT_ENABLE,
    .c2DebugRun       = CY_MCWDT_ENABLE,
};

void irqMCWDT1Handler(void)
{
    uint32_t masked;
    masked = Cy_MCWDT_GetInterruptStatusMasked(MCWDT1);

    if(MCWDT_INTR_MASKED_CTR0_INT_Msk & masked)
    {
        Cy_GPIO_Inv(CY_LED8_PORT, CY_LED8_PIN);
    }
    if(MCWDT_INTR_MASKED_CTR1_INT_Msk & masked)
    {
        Cy_GPIO_Inv(CY_LED9_PORT, CY_LED9_PIN);
    }
    if(MCWDT_INTR_MASKED_CTR2_INT_Msk & masked)
    {
        Cy_GPIO_Inv(CY_LED4_PORT, CY_LED4_PIN);
    }

    Cy_MCWDT_ClearInterrupt(MCWDT1, masked);
}

int main(void)
{
    SystemInit();

    // Example had been originally tested with "cache off", so ensure that caches are turned off (may have been enabled by new startup.c module)
    SCB_DisableICache(); // Disables and invalidates instruction cache
    SCB_DisableDCache(); // Disables, cleans and invalidates data cache

    __enable_irq(); /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    user_led_port_pin_cfg.hsiom = CY_LED8_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED8_PORT, CY_LED8_PIN, &user_led_port_pin_cfg);

    user_led_port_pin_cfg.hsiom = CY_LED9_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED9_PORT, CY_LED9_PIN, &user_led_port_pin_cfg);

    user_led_port_pin_cfg.hsiom = CY_LED4_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED4_PORT, CY_LED4_PIN, &user_led_port_pin_cfg);

    /*********************************************************************/
    /*****                   Interrupt setting                       *****/
    /*********************************************************************/
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(srss_interrupt_mcwdt_1_IRQn, irqMCWDT1Handler);
    NVIC_SetPriority(MCWDT_CPU_IRQ_INDEX, 0ul);
    NVIC_ClearPendingIRQ(MCWDT_CPU_IRQ_INDEX);
    NVIC_EnableIRQ(MCWDT_CPU_IRQ_INDEX);

    Cy_MCWDT_DeInit(MCWDT1);
    Cy_MCWDT_Init(MCWDT1, &mcwdtConfig);

    Cy_MCWDT_Unlock(MCWDT1);
    Cy_MCWDT_SetInterruptMask(MCWDT1, CY_MCWDT_CTR_Msk);
    Cy_MCWDT_Enable(MCWDT1, 
                    CY_MCWDT_CTR_Msk,  // enable all counter
                    0u);
    Cy_MCWDT_Lock(MCWDT1);

    for(;;)
    {
    }
}



/* [] END OF FILE */
