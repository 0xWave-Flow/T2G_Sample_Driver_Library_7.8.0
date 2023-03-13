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

#define WDT_CPU_IRQ_INDEX CPUIntIdx2_IRQn

#define USER_LED_PORT     CY_CB_USER_LED1_PORT
#define USER_LED_PIN      CY_CB_USER_LED1_PIN
#define USER_LED_PIN_MUX  CY_CB_USER_LED1_PIN_MUX

cy_stc_gpio_pin_config_t user_led_port_pin_cfg =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = USER_LED_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};


cy_stc_sysint_irq_t stc_sysint_irq_cfg_wdt =
{
    .sysIntSrc = srss_interrupt_wdt_IRQn,
    .intIdx    = WDT_CPU_IRQ_INDEX,
    .isEnabled = true,
};


void Wdt_Warn_IntrISR(void)
{
    /* Clear Counter when AUTO_SERVICE=disable */
    Cy_WDT_ClearWatchdog();
    Cy_WDT_ClearInterrupt();
    Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
}


int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */
    
    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);

    /*-----------------------*/
    /*  Initialize LED port  */
    /*-----------------------*/
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);

    /*-----------------------*/
    /* Configuration for WDT */
    /*-----------------------*/
    Cy_WDT_Disable();

    /* Interrupt Test (Warn Limit) */
    Cy_WDT_Unlock();
    Cy_WDT_SetLowerLimit(0ul);
    Cy_WDT_SetUpperLimit(40000ul);     /* just an arbitrary value > warn limit */
    Cy_WDT_SetWarnLimit (32000ul);     /* 1sec @clk_lf = 32kHz */
    Cy_WDT_SetLowerAction(CY_WDT_LOW_UPP_ACTION_NONE);
    Cy_WDT_SetUpperAction(CY_WDT_LOW_UPP_ACTION_NONE);
    Cy_WDT_SetWarnAction (CY_WDT_WARN_ACTION_INT);
    Cy_WDT_SetAutoService(CY_WDT_DISABLE);
    Cy_WDT_SetDebugRun(CY_WDT_ENABLE);  /* It needs when using debugger */
    Cy_WDT_Lock();

    Cy_WDT_MaskInterrupt();
    Cy_WDT_Enable();

    /*----------------------------------*/
    /* Interrupt Configuration for WDT  */
    /*----------------------------------*/
    Cy_SysInt_InitIRQ(&stc_sysint_irq_cfg_wdt);
    Cy_SysInt_SetSystemIrqVector(srss_interrupt_wdt_IRQn, Wdt_Warn_IntrISR);
    NVIC_ClearPendingIRQ(WDT_CPU_IRQ_INDEX);
    NVIC_EnableIRQ(WDT_CPU_IRQ_INDEX);

    for(;;);
}


/* [] END OF FILE */
