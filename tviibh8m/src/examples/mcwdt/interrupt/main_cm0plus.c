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

cy_stc_gpio_pin_config_t user_led_port_pin_cfg =
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

#define MCWDT_TICKS_PER_SECOND     (32000u)    /* 1 sec when clk_lf = 32 KHz */

#define MCWDT_CPU_IRQ_INDEX     CPUIntIdx2_IRQn

cy_stc_sysint_irq_t irq_cfg =
{
    .sysIntSrc  = srss_interrupt_mcwdt_0_IRQn,
    .intIdx     = MCWDT_CPU_IRQ_INDEX,
    .isEnabled  = true,
};

void irqMCWDT0Handler(void)
{
    uint32_t masked;
    masked = Cy_MCWDT_GetInterruptStatusMasked(MCWDT0);

    if(MCWDT_INTR_MASKED_CTR0_INT_Msk & masked)
    {
        Cy_GPIO_Inv(CY_LED0_PORT, CY_LED0_PIN);
    }
    if(MCWDT_INTR_MASKED_CTR1_INT_Msk & masked)
    {
        Cy_GPIO_Inv(CY_LED1_PORT, CY_LED1_PIN);
    }
    if(MCWDT_INTR_MASKED_CTR2_INT_Msk & masked)
    {
        Cy_GPIO_Inv(CY_LED2_PORT, CY_LED2_PIN);
    }

    Cy_MCWDT_ClearInterrupt(MCWDT0, masked);
}

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */
    
    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);

    /*********************************************************************/
    /*****                     Port settings                         *****/
    /*********************************************************************/
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    user_led_port_pin_cfg.hsiom  = CY_LED0_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED0_PORT, CY_LED0_PIN, &user_led_port_pin_cfg);

    user_led_port_pin_cfg.hsiom  = CY_LED1_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED1_PORT, CY_LED1_PIN, &user_led_port_pin_cfg);

    user_led_port_pin_cfg.hsiom  = CY_LED2_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED2_PORT, CY_LED2_PIN, &user_led_port_pin_cfg);

    /*********************************************************************/
    /*****                   Interrupt setting                       *****/
    /*********************************************************************/
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(srss_interrupt_mcwdt_0_IRQn, irqMCWDT0Handler);
    NVIC_SetPriority(MCWDT_CPU_IRQ_INDEX, 0ul);
    NVIC_ClearPendingIRQ(MCWDT_CPU_IRQ_INDEX);
    NVIC_EnableIRQ(MCWDT_CPU_IRQ_INDEX);

    /*********************************************************************/
    /*****      Set Core DeepSleep of which can pause counter        *****/
    /*********************************************************************/
    Cy_MCWDT_CpuSelectForDpSlpPauseAction(MCWDT0, CY_MCWDT_PAUSED_BY_DPSLP_CM0);

    /*********************************************************************/
    /*****                        Set actions                        *****/
    /*********************************************************************/
    Cy_MCWDT_SetLowerAction(MCWDT0, CY_MCWDT_COUNTER0, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetUpperAction(MCWDT0, CY_MCWDT_COUNTER0, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetWarnAction(MCWDT0, CY_MCWDT_COUNTER0, CY_MCWDT_WARN_ACTION_INT);

    Cy_MCWDT_SetLowerAction(MCWDT0, CY_MCWDT_COUNTER1, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetUpperAction(MCWDT0, CY_MCWDT_COUNTER1, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetWarnAction(MCWDT0, CY_MCWDT_COUNTER1, CY_MCWDT_WARN_ACTION_INT);

    Cy_MCWDT_SetSubCounter2Action(MCWDT0, CY_MCWDT_CNT2_ACTION_INT);

    /*********************************************************************/
    /*****                      Set limit values                     *****/
    /*********************************************************************/
    Cy_MCWDT_SetLowerLimit(MCWDT0, CY_MCWDT_COUNTER0, 0u, 0u);
    Cy_MCWDT_SetWarnLimit(MCWDT0, CY_MCWDT_COUNTER0, MCWDT_TICKS_PER_SECOND, 0u);  /* 1 sec */
    Cy_MCWDT_SetUpperLimit(MCWDT0, CY_MCWDT_COUNTER0, 0xFFFFu, 0u);

    Cy_MCWDT_SetLowerLimit(MCWDT0, CY_MCWDT_COUNTER1, 0u, 0u);
    Cy_MCWDT_SetWarnLimit(MCWDT0, CY_MCWDT_COUNTER1, MCWDT_TICKS_PER_SECOND * 2u, 0u);  /* 2 sec */
    Cy_MCWDT_SetUpperLimit(MCWDT0, CY_MCWDT_COUNTER1, 0xFFFFu, 0u);

    Cy_MCWDT_SetToggleBit(MCWDT0, CY_MCWDT_CNT2_MONITORED_BIT15); // means 32768 count period


    /*********************************************************************/
    /*****                        Set options                        *****/
    /*********************************************************************/
    Cy_MCWDT_SetAutoService(MCWDT0, CY_MCWDT_COUNTER0, 1ul); // enable
    Cy_MCWDT_SetAutoService(MCWDT0, CY_MCWDT_COUNTER1, 1ul); // enable

    Cy_MCWDT_SetSleepDeepPause(MCWDT0, CY_MCWDT_COUNTER0, 1ul); // enable
    Cy_MCWDT_SetSleepDeepPause(MCWDT0, CY_MCWDT_COUNTER1, 1ul); // enable
    Cy_MCWDT_SetSleepDeepPause(MCWDT0, CY_MCWDT_COUNTER2, 1ul); // enable

    Cy_MCWDT_SetDebugRun(MCWDT0, CY_MCWDT_COUNTER0, 1ul); // enable
    Cy_MCWDT_SetDebugRun(MCWDT0, CY_MCWDT_COUNTER1, 1ul); // enable
    Cy_MCWDT_SetDebugRun(MCWDT0, CY_MCWDT_COUNTER2, 1ul); // enable

    Cy_MCWDT_SetInterruptMask(MCWDT0, CY_MCWDT_CTR_Msk); // enable all counter interrupts

    Cy_MCWDT_Enable(MCWDT0,
                    CY_MCWDT_CTR_Msk,  // enable all counter
                    0u);

    while(Cy_MCWDT_GetEnabledStatus(MCWDT0, CY_MCWDT_COUNTER0) != 1ul);
    while(Cy_MCWDT_GetEnabledStatus(MCWDT0, CY_MCWDT_COUNTER1) != 1ul);
    while(Cy_MCWDT_GetEnabledStatus(MCWDT0, CY_MCWDT_COUNTER2) != 1ul);

    for(;;)
    {
    }
}



/* [] END OF FILE */
