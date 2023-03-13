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

#define USER_LED_PORT           CY_CB_USER_LED1_PORT
#define USER_LED_PIN            CY_CB_USER_LED1_PIN
#define USER_LED_PIN_MUX        CY_CB_USER_LED1_PIN_MUX

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

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /**************************************************/
    /*****      Checking the cause of reset       *****/
    /**************************************************/
    /* If the reset cause is MCWDT0, it goes to infinite loop blinking a LED */
    {
        uint32_t resetReason;
        resetReason = Cy_SysReset_GetResetReason();
        if((resetReason & CY_SYSRESET_MCWDT0) != 0ul)
        {
            Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);
            while(1)
            {
                Cy_SysTick_DelayInUs(50000);
                Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
            }
        }
    }

    /*********************************************************************/
    /*****      Set Core DeepSleep of which can pause counter        *****/
    /*********************************************************************/
    Cy_MCWDT_CpuSelectForDpSlpPauseAction(MCWDT0, CY_MCWDT_PAUSED_BY_DPSLP_CM0);

    /*********************************************************************/
    /*****                        Set actions                        *****/
    /*********************************************************************/
    Cy_MCWDT_SetLowerAction(MCWDT0, CY_MCWDT_COUNTER0, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetUpperAction(MCWDT0, CY_MCWDT_COUNTER0, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetWarnAction(MCWDT0, CY_MCWDT_COUNTER0, CY_MCWDT_WARN_ACTION_NONE);

    Cy_MCWDT_SetLowerAction(MCWDT0, CY_MCWDT_COUNTER1, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetUpperAction(MCWDT0, CY_MCWDT_COUNTER1, CY_MCWDT_ACTION_FAULT_THEN_RESET); // sub counter 1 upper limit causes to reset
    Cy_MCWDT_SetWarnAction(MCWDT0, CY_MCWDT_COUNTER1, CY_MCWDT_WARN_ACTION_NONE);

    Cy_MCWDT_SetSubCounter2Action(MCWDT0, CY_MCWDT_CNT2_ACTION_NONE);

    /*********************************************************************/
    /*****                      Set limit values                     *****/
    /*********************************************************************/
    Cy_MCWDT_SetLowerLimit(MCWDT0, CY_MCWDT_COUNTER0, 0u, 0u);
    Cy_MCWDT_SetWarnLimit(MCWDT0, CY_MCWDT_COUNTER0, 0u, 0u);
    Cy_MCWDT_SetUpperLimit(MCWDT0, CY_MCWDT_COUNTER0, 100u, 0u);

    Cy_MCWDT_SetLowerLimit(MCWDT0, CY_MCWDT_COUNTER1, 0u, 0u);
    Cy_MCWDT_SetWarnLimit(MCWDT0, CY_MCWDT_COUNTER1, 0u, 0u);
    Cy_MCWDT_SetUpperLimit(MCWDT0, CY_MCWDT_COUNTER1, MCWDT_TICKS_PER_SECOND * 2u, 0u);  /* 2 sec */

    Cy_MCWDT_SetToggleBit(MCWDT0, CY_MCWDT_CNT2_MONITORED_BIT15); // means 32768 count period


    /*********************************************************************/
    /*****                        Set options                        *****/
    /*********************************************************************/
    Cy_MCWDT_SetAutoService(MCWDT0, CY_MCWDT_COUNTER0, 0ul); // disable
    Cy_MCWDT_SetAutoService(MCWDT0, CY_MCWDT_COUNTER1, 0ul); // disable

    Cy_MCWDT_SetSleepDeepPause(MCWDT0, CY_MCWDT_COUNTER0, 1ul); // enable
    Cy_MCWDT_SetSleepDeepPause(MCWDT0, CY_MCWDT_COUNTER1, 1ul); // enable
    Cy_MCWDT_SetSleepDeepPause(MCWDT0, CY_MCWDT_COUNTER2, 1ul); // enable

    Cy_MCWDT_SetDebugRun(MCWDT0, CY_MCWDT_COUNTER0, 1ul); // enable
    Cy_MCWDT_SetDebugRun(MCWDT0, CY_MCWDT_COUNTER1, 1ul); // enable
    Cy_MCWDT_SetDebugRun(MCWDT0, CY_MCWDT_COUNTER2, 1ul); // enable

    Cy_MCWDT_Enable(MCWDT0, 
                    CY_MCWDT_CTR_Msk,  // enable all counter
                    0u);

    while(Cy_MCWDT_GetEnabledStatus(MCWDT0, CY_MCWDT_COUNTER0) != 1ul);
    while(Cy_MCWDT_GetEnabledStatus(MCWDT0, CY_MCWDT_COUNTER1) != 1ul);
    while(Cy_MCWDT_GetEnabledStatus(MCWDT0, CY_MCWDT_COUNTER2) != 1ul);

    /*******************************************************************/
    /*****                    Turn the LED off                     *****/
    /*******************************************************************/
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);

    for(;;)
    {
#if 0 // If you want to trigger reset, remove this code.
        uint16_t lowerCounter = Cy_MCWDT_GetLowerLimit(MCWDT0, CY_MCWDT_COUNTER1);
        uint16_t countValue = (uint16_t)Cy_MCWDT_GetCount(MCWDT0, CY_MCWDT_COUNTER1);

        if(countValue > lowerCounter)
        {
            Cy_MCWDT_ClearWatchdog(MCWDT0, CY_MCWDT_COUNTER1);
            Cy_MCWDT_WaitForCounterReset(MCWDT0, CY_MCWDT_COUNTER1);
        }
#endif
    }
}



/* [] END OF FILE */
