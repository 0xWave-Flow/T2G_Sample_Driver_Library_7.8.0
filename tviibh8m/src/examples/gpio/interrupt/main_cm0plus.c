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

#define USER_BUTTON_PORT        CY_CB_USER_BUTTON1_PORT
#define USER_BUTTON_PIN         CY_CB_USER_BUTTON1_PIN
#define USER_BUTTON_PIN_MUX     CY_CB_USER_BUTTON1_PIN_MUX
#define USER_BUTTON_IRQ         CY_CB_USER_BUTTON1_IRQN

////////////////////////////////////////////////////////////////////////////////
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

const cy_stc_gpio_pin_config_t user_button_port_pin_cfg = 
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom     = USER_BUTTON_PIN_MUX,
    .intEdge   = CY_GPIO_INTR_FALLING,
    .intMask   = 1ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

/* Setup GPIO for BUTTON1 interrupt */
const cy_stc_sysint_irq_t irq_cfg =
{
    .sysIntSrc  = USER_BUTTON_IRQ,
    .intIdx     = CPUIntIdx3_IRQn,
    .isEnabled  = true,
};

void ButtonIntHandler(void)
{
    uint32_t intStatus;

    /* If falling edge detected */
    intStatus = Cy_GPIO_GetInterruptStatusMasked(USER_BUTTON_PORT, USER_BUTTON_PIN);
    if (intStatus != 0ul)
    {
        Cy_GPIO_ClearInterrupt(USER_BUTTON_PORT, USER_BUTTON_PIN);

        /* Toggle LED */
        Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
    }
}

/*
 * This is an example for GPIO edge detection interrupt.
 * This example uses 1 LED ports and 1 button ports on the base board.
 * Settings are:
 */
////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(USER_LED_PORT,    USER_LED_PIN,    &user_led_port_pin_cfg);
    Cy_GPIO_Pin_Init(USER_BUTTON_PORT, USER_BUTTON_PIN, &user_button_port_pin_cfg);

    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, ButtonIntHandler);

    NVIC_SetPriority(irq_cfg.intIdx, 3ul);
    NVIC_EnableIRQ(irq_cfg.intIdx);

    for(;;);
}
////////////////////////////////////////////////////////////////////////////////

/* [] END OF FILE */
