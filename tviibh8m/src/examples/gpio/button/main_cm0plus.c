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


////////////////////////////////////////////////////////////////////////////////
#define USER_LED_PORT           CY_CB_USER_LED1_PORT
#define USER_LED_PIN            CY_CB_USER_LED1_PIN
#define USER_LED_PIN_MUX        CY_CB_USER_LED1_PIN_MUX

#define USER_BUTTON_PORT        CY_CB_USER_BUTTON1_PORT
#define USER_BUTTON_PIN         CY_CB_USER_BUTTON1_PIN
#define USER_BUTTON_PIN_MUX     CY_CB_USER_BUTTON1_PIN_MUX

////////////////////////////////////////////////////////////////////////////////
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

cy_stc_gpio_pin_config_t user_button_port_pin_cfg = 
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom     = USER_BUTTON_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
}; 

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(USER_BUTTON_PORT, USER_BUTTON_PIN, &user_button_port_pin_cfg);
    Cy_GPIO_Pin_Init(USER_LED_PORT,    USER_LED_PIN,    &user_led_port_pin_cfg);

    // Detect falling edge of the button GPIO, then change the LED level
    // Assumed that chattering is taken care by board HW
    uint32_t prevLevel     = 0ul;
    uint32_t curLevel      = 0ul;
    bool     isFallingEdge = false;
    bool     ledOnOff      = true;
    for(;;)
    {
        // Get the current button level
        curLevel = Cy_GPIO_Read(USER_BUTTON_PORT, USER_BUTTON_PIN);

        // Detect falling edge if current level low in condition previous level was high
        isFallingEdge = (curLevel == 0ul) & (prevLevel == 1ul);

        // Change the LED level if falling edge detected
        if(isFallingEdge)
        {
            Cy_GPIO_Write(USER_LED_PORT, USER_LED_PIN, ledOnOff);

            ledOnOff = !ledOnOff;
        }

        // Save the previous button level
        prevLevel = curLevel;
    }
}

/* [] END OF FILE */
