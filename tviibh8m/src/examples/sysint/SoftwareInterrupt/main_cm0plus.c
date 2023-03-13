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

const cy_stc_gpio_pin_config_t user_led_port_pin_cfg =
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

const cy_stc_sysint_t sw_irq0_cfg = 
{
    .intrSrc      = Internal0_IRQn,
    .intrPriority = 3ul,
};

const cy_stc_sysint_t sw_irq7_cfg = 
{
    .intrSrc      = Internal7_IRQn,
    .intrPriority = 3ul,
};

void SoftwareInterupt0Handler(void)
{
    // Button being Pushed
    Cy_GPIO_Write(USER_LED_PORT, USER_LED_PIN, 1ul);
}

void SoftwareInterupt7Handler(void)
{
    // Button being Pushed
    Cy_GPIO_Write(USER_LED_PORT, USER_LED_PIN, 0ul);
}

int main(void)
{
    SystemInit();
   
    __enable_irq(); /* Enable global interrupts. */

    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);

    /* Software Interrupt 0 */
    {
        /* At first change software interrupt 0 handler */
        Cy_SysInt_Init(&sw_irq0_cfg, SoftwareInterupt0Handler);

        /* Enable software interrupt IRQ */
        NVIC_EnableIRQ(sw_irq0_cfg.intrSrc); 
    }

    /* Software Interrupt 7 */
    {
        /* At first change software interrupt 7 handler */
        Cy_SysInt_Init(&sw_irq7_cfg, SoftwareInterupt7Handler);

        /* Enable software interrupt IRQ */
        NVIC_EnableIRQ(sw_irq7_cfg.intrSrc);
    }

    for(;;)
    {
        // Delay
        Cy_SysTick_DelayInUs(50000ul);

        /* Force set pending status in the NVIC */
        NVIC_SetPendingIRQ(sw_irq0_cfg.intrSrc); // this will cause SoftwareInterupt0Handler invoked

        // Delay
        Cy_SysTick_DelayInUs(50000ul);

        /* Force set pending status in the NVIC */
        NVIC_SetPendingIRQ(sw_irq7_cfg.intrSrc); // this will cause SoftwareInterupt7Handler invoked

    }
}



/* [] END OF FILE */
