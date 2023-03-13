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

#define USER_LED_PORT           CY_CB_USER_LED2_PORT
#define USER_LED_PIN            CY_CB_USER_LED2_PIN
#define USER_LED_PIN_MUX        CY_CB_USER_LED2_PIN_MUX 

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

#define MCWDT_TICKS_PER_SECOND     (32000u)    /* 1 sec when clk_lf = 32 KHz */

cy_stc_mcwdt_config_t mcwdtConfig =
{
    .coreSelect       = CY_MCWDT_PAUSED_BY_DPSLP_CM4_CM7_0,
    .c0LowerLimit     = 0u,
    .c0UpperLimit     = MCWDT_TICKS_PER_SECOND,  /* 1 sec */
    .c0WarnLimit      = 0u,
    .c0LowerAction    = CY_MCWDT_ACTION_NONE,
    .c0UpperAction    = CY_MCWDT_ACTION_FAULT_THEN_RESET, /* Note */
    .c0WarnAction     = CY_MCWDT_WARN_ACTION_NONE,
    .c0AutoService    = CY_MCWDT_DISABLE,
    .c0SleepDeepPause = CY_MCWDT_ENABLE,
    .c0DebugRun       = CY_MCWDT_ENABLE,
    .c1LowerLimit     = 100u,
    .c1UpperLimit     = MCWDT_TICKS_PER_SECOND * 2u, /* 2 sec */
    .c1WarnLimit      = 0u,
    .c1LowerAction    = CY_MCWDT_ACTION_NONE,
    .c1UpperAction    = CY_MCWDT_ACTION_NONE,
    .c1WarnAction     = CY_MCWDT_WARN_ACTION_NONE,
    .c1AutoService    = CY_MCWDT_DISABLE,
    .c1SleepDeepPause = CY_MCWDT_ENABLE,
    .c1DebugRun       = CY_MCWDT_ENABLE,
    .c2ToggleBit      = CY_MCWDT_CNT2_MONITORED_BIT15,
    .c2Action         = CY_MCWDT_CNT2_ACTION_INT,
    .c2SleepDeepPause = CY_MCWDT_ENABLE,
    .c2DebugRun       = CY_MCWDT_ENABLE,
};

int main(void)
{
    SystemInit();

    // Example had been originally tested with "cache off", so ensure that caches are turned off (may have been enabled by new startup.c module)
    SCB_DisableICache(); // Disables and invalidates instruction cache
    SCB_DisableDCache(); // Disables, cleans and invalidates data cache

 
    __enable_irq(); /* Enable global interrupts. */

    /* If the reset cause is MCWDT1, it goes to infinite loop blinking a LED */
    {
        uint32_t resetReason;
        resetReason = Cy_SysReset_GetResetReason();
        if((resetReason & CY_SYSRESET_MCWDT1) != 0ul)
        {
            Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);
            while(1)
            {
                Cy_SysTick_DelayInUs(50000);
                Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
            }
        }
    }


    Cy_MCWDT_DeInit(MCWDT1);
    Cy_MCWDT_Init(MCWDT1, &mcwdtConfig);
    Cy_MCWDT_Unlock(MCWDT1);
    Cy_MCWDT_SetInterruptMask(MCWDT1, CY_MCWDT_CTR_Msk);
    Cy_MCWDT_Enable(MCWDT1, 
                    CY_MCWDT_CTR_Msk,  // enable all counter
                    0u);
    Cy_MCWDT_Lock(MCWDT1);

    /*******************************************************************/
    /*****                    Turn the LED off                     *****/
    /*******************************************************************/
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);
    
    for(;;) 
    {
#if 0 // If you want to trigger reset, remove this code.
        uint16_t lowerCounter = Cy_MCWDT_GetLowerLimit(MCWDT1, CY_MCWDT_COUNTER0);
        uint16_t countValue = (uint16_t)Cy_MCWDT_GetCount(MCWDT1, CY_MCWDT_COUNTER0);

        if(countValue > lowerCounter)
        {
            Cy_MCWDT_ClearWatchdog(MCWDT1, CY_MCWDT_COUNTER0);
            Cy_MCWDT_WaitForCounterReset(MCWDT1, CY_MCWDT_COUNTER0);
        }
#endif
    }
}



/* [] END OF FILE */
