/***************************************************************************//**
* \file main_cm7_1.c
*
* \version 1.0
*
* \brief Main example file for CM7_1
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

#define USER_LED_PORT       CY_LED0_PORT
#define USER_LED_PIN        CY_LED0_PIN
#define USER_LED_PIN_MUX    CY_LED0_PIN_MUX

#define USER_BUTTON_PORT    CY_CB_USER_BUTTON1_PORT
#define USER_BUTTON_PIN     CY_CB_USER_BUTTON1_PIN
#define USER_BUTTON_PIN_MUX CY_CB_USER_BUTTON1_PIN_MUX
#define USER_BUTTON_IRQ     CY_CB_USER_BUTTON1_IRQN

const cy_stc_gpio_pin_config_t user_led_port_pin_cfg =
{
    .outVal    = 0u,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = USER_LED_PIN_MUX,
    .intEdge   = 0u,
    .intMask   = 0u,
    .vtrip     = 0u,
    .slewRate  = 0u,
    .driveSel  = 0u,
};


const cy_stc_sysint_irq_t button_irq_cfg =
{
    .sysIntSrc  = USER_BUTTON_IRQ,
    .intIdx     = CPUIntIdx2_IRQn,
    .isEnabled  = true,
};


static bool putIntoDeepSleep = false;

void ButtonIntHandler(void)
{
    uint32_t intStatus;

    /* If button1 falling edge detected */
    intStatus = Cy_GPIO_GetInterruptStatusMasked(USER_BUTTON_PORT, USER_BUTTON_PIN);
    if (intStatus != 0ul)
    {
        putIntoDeepSleep = true;

        // wait 0.05 [s] to make sure all cores in interrupt handler.
        Cy_SysTick_DelayInUs(50000ul);

        Cy_GPIO_ClearInterrupt(USER_BUTTON_PORT, USER_BUTTON_PIN);
    }
}

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);

    Cy_SysInt_InitIRQ(&button_irq_cfg);
    Cy_SysInt_SetSystemIrqVector(button_irq_cfg.sysIntSrc, ButtonIntHandler);
    NVIC_SetPriority(button_irq_cfg.intIdx, 3ul);
    NVIC_EnableIRQ(button_irq_cfg.intIdx);
    
    for(;;)
    {
        // turn off all LEDs to 1
        Cy_GPIO_Write(USER_LED_PORT, USER_LED_PIN, 1ul);

        if(putIntoDeepSleep == true)
        {
            // turn off all LEDs to 0
            Cy_GPIO_Write(USER_LED_PORT, USER_LED_PIN, 0ul);

            Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);

            putIntoDeepSleep = false;
        }
    }
}



/* [] END OF FILE */
