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

#define USER_LED_PORT    CY_CB_USER_LED1_PORT
#define USER_LED_PIN     CY_CB_USER_LED1_PIN
#define USER_LED_PIN_MUX CY_CB_USER_LED1_PIN_MUX

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

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */
    
    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);

    /* If the reset cause is WDT, it goes to infinite loop blinking a LED */
    {
        uint32_t resetReason;
        resetReason = Cy_SysReset_GetResetReason();
        if((resetReason & CY_SYSRESET_WDT) != 0ul)
        {
            /* Clear  Reset reason */
            Cy_SysReset_ClearAllResetReasons();
            Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);
            while(1)
            {
                Cy_SysTick_DelayInUs(2000000);
                Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
            }
        }
    }

    /* Clear the PORVDDD Reset reason flag */
    /* This is necessary because PORVDDD flag blocks the WDT flag to rise */
    Cy_SysReset_ClearAllResetReasons();

    /*-----------------------*/
    /* Configuration for WDT */
    /*-----------------------*/
    Cy_WDT_Init();                      /* Upper Limit: 1sec and reset */
    Cy_WDT_Unlock();
    Cy_WDT_SetUpperLimit(64000ul);      /* Upper Limit: 2sec (override) */
    Cy_WDT_SetDebugRun(CY_WDT_ENABLE);  /* This is necessary when using debugger */
    Cy_WDT_Lock();
    Cy_WDT_Enable();

    // turn off the LED
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);

    for(;;)
    {
#if 1
        /* When testing WDT reset, this code needs to be commented out. */
        Cy_WDT_ClearWatchdog();
#endif
    }
}

/* [] END OF FILE */
