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

#define USER_LED1_PORT          CY_LED8_PORT
#define USER_LED1_PIN           CY_LED8_PIN
#define USER_LED1_PIN_MUX       CY_LED8_PIN_MUX

#define USER_LED2_PORT          CY_LED9_PORT
#define USER_LED2_PIN           CY_LED9_PIN
#define USER_LED2_PIN_MUX       CY_LED9_PIN_MUX

#define MCWDT_TICKS_PER_SECOND     (32000u)    /* 1 sec when clk_lf = 32 KHz */

#define FAULT_CPU_IRQ_INDEX     CPUIntIdx2_IRQn

cy_stc_gpio_pin_config_t user_led_port_pin_cfg =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = USER_LED1_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

cy_stc_sysint_irq_t irq_cfg =
{
    .sysIntSrc  = cpuss_interrupts_fault_1_IRQn,
    .intIdx     = FAULT_CPU_IRQ_INDEX,
    .isEnabled  = true,
};

cy_stc_mcwdt_config_t mcwdtConfig =
{
    .coreSelect       = CY_MCWDT_PAUSED_BY_DPSLP_CM4_CM7_0,
    .c0LowerLimit     = 0u,
    .c0UpperLimit     = MCWDT_TICKS_PER_SECOND + 2u,  /* 1.002 sec, to avoid overlap with CNT1 */
    .c0WarnLimit      = 0u,
    .c0LowerAction    = CY_MCWDT_ACTION_NONE,
    .c0UpperAction    = CY_MCWDT_ACTION_FAULT,
    .c0WarnAction     = CY_MCWDT_WARN_ACTION_NONE,
    .c0AutoService    = CY_MCWDT_DISABLE,
    .c0SleepDeepPause = CY_MCWDT_ENABLE,
    .c0DebugRun       = CY_MCWDT_ENABLE,
    .c1LowerLimit     = 0u,
    .c1UpperLimit     = MCWDT_TICKS_PER_SECOND * 2u, /* 2 sec */
    .c1WarnLimit      = 0u,
    .c1LowerAction    = CY_MCWDT_ACTION_NONE,
    .c1UpperAction    = CY_MCWDT_ACTION_FAULT,
    .c1WarnAction     = CY_MCWDT_WARN_ACTION_NONE,
    .c1AutoService    = CY_MCWDT_DISABLE,
    .c1SleepDeepPause = CY_MCWDT_ENABLE,
    .c1DebugRun       = CY_MCWDT_ENABLE,
    .c2ToggleBit      = CY_MCWDT_CNT2_MONITORED_BIT15,
    .c2Action         = CY_MCWDT_CNT2_ACTION_INT,
    .c2SleepDeepPause = CY_MCWDT_ENABLE,
    .c2DebugRun       = CY_MCWDT_ENABLE,
};

void irqFaultReport1Handler(void)
{
    cy_en_sysflt_source_t status;
    uint32_t faultData;
    
        /* Clear Interrupt flag */
    Cy_SysFlt_ClearInterrupt(FAULT_STRUCT1);

    status = Cy_SysFlt_GetErrorSource(FAULT_STRUCT1);

    if(status != CY_SYSFLT_NO_FAULT)
    {
        if(status == CY_SYSFLT_SRSS_MCWDT1) // MCWDT1 fault
        {
            faultData = Cy_SysFlt_GetData0(FAULT_STRUCT1);
            if(faultData & 0x00000001ul)
            {
                 // subcounter 0 lower limit fault
            }
            else if(faultData & 0x00000002ul)
            {
                Cy_GPIO_Inv(USER_LED1_PORT, USER_LED1_PIN); // subcounter 0 upper limit fault
                Cy_MCWDT_ClearWatchdog(MCWDT1, CY_MCWDT_COUNTER0);
            }
            else if(faultData & 0x00000004ul)
            {
                 // subcounter 1 lower limit fault
            }
            else if(faultData & 0x00000008ul)
            {
                Cy_GPIO_Inv(USER_LED2_PORT, USER_LED2_PIN); // subcounter 1 upper limit fault
                Cy_MCWDT_ClearWatchdog(MCWDT1, CY_MCWDT_COUNTER1);
            }
        }
    }

    Cy_SysFlt_ClearStatus(FAULT_STRUCT1);
}


int main(void)
{
    SystemInit();

    // Example had been originally tested with "cache off", so ensure that caches are turned off (may have been enabled by new startup.c module)
    SCB_DisableICache(); // Disables and invalidates instruction cache
    SCB_DisableDCache(); // Disables, cleans and invalidates data cache

    __enable_irq(); /* Enable global interrupts. */

    /*********************************************************************/
    /*****                 Fault report settings                     *****/
    /*********************************************************************/
    Cy_SysFlt_ClearStatus(FAULT_STRUCT1); // clear status (typically this process is done by boot code)
    Cy_SysFlt_SetMaskByIdx(FAULT_STRUCT1, CY_SYSFLT_SRSS_MCWDT1); // enable Fault MCWDT #1
    Cy_SysFlt_SetInterruptMask(FAULT_STRUCT1);

    /*********************************************************************/
    /*****                     Port settings                         *****/
    /*********************************************************************/
    user_led_port_pin_cfg.hsiom = USER_LED1_PIN_MUX;
    Cy_GPIO_Pin_Init(USER_LED1_PORT, USER_LED1_PIN, &user_led_port_pin_cfg);

    user_led_port_pin_cfg.hsiom = USER_LED2_PIN_MUX;
    Cy_GPIO_Pin_Init(USER_LED2_PORT, USER_LED2_PIN, &user_led_port_pin_cfg);

    /*********************************************************************/
    /*****                   Interrupt setting                       *****/
    /*********************************************************************/
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(cpuss_interrupts_fault_1_IRQn, irqFaultReport1Handler);
    NVIC_SetPriority(FAULT_CPU_IRQ_INDEX, 0ul);
    NVIC_ClearPendingIRQ(FAULT_CPU_IRQ_INDEX);
    NVIC_EnableIRQ(FAULT_CPU_IRQ_INDEX);

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
