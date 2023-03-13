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

int main(void)
{
    SystemInit();
  
    __enable_irq();

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    user_led_port_pin_cfg.hsiom = CY_LED0_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED0_PORT, CY_LED0_PIN, &user_led_port_pin_cfg);
    user_led_port_pin_cfg.hsiom = CY_LED1_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED1_PORT, CY_LED1_PIN, &user_led_port_pin_cfg);
    user_led_port_pin_cfg.hsiom = CY_LED2_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED2_PORT, CY_LED2_PIN, &user_led_port_pin_cfg);
    user_led_port_pin_cfg.hsiom = CY_LED3_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED3_PORT, CY_LED3_PIN, &user_led_port_pin_cfg);
    user_led_port_pin_cfg.hsiom = CY_LED4_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED4_PORT, CY_LED4_PIN, &user_led_port_pin_cfg);
    user_led_port_pin_cfg.hsiom = CY_LED5_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED5_PORT, CY_LED5_PIN, &user_led_port_pin_cfg);
    user_led_port_pin_cfg.hsiom = CY_LED6_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED6_PORT, CY_LED6_PIN, &user_led_port_pin_cfg);
    user_led_port_pin_cfg.hsiom = CY_LED7_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED7_PORT, CY_LED7_PIN, &user_led_port_pin_cfg);
    user_led_port_pin_cfg.hsiom = CY_LED8_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED8_PORT, CY_LED8_PIN, &user_led_port_pin_cfg);
    user_led_port_pin_cfg.hsiom = CY_LED9_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED9_PORT, CY_LED9_PIN, &user_led_port_pin_cfg);

    for(;;)
    {
        // Wait 0.05 [s]
        Cy_SysTick_DelayInUs(50000ul);

        Cy_GPIO_Inv(CY_LED0_PORT, CY_LED0_PIN);
        Cy_GPIO_Inv(CY_LED1_PORT, CY_LED1_PIN);
        Cy_GPIO_Inv(CY_LED2_PORT, CY_LED2_PIN);
        Cy_GPIO_Inv(CY_LED3_PORT, CY_LED3_PIN);
        Cy_GPIO_Inv(CY_LED4_PORT, CY_LED4_PIN);
        Cy_GPIO_Inv(CY_LED5_PORT, CY_LED5_PIN);
        Cy_GPIO_Inv(CY_LED6_PORT, CY_LED6_PIN);
        Cy_GPIO_Inv(CY_LED7_PORT, CY_LED7_PIN);
        Cy_GPIO_Inv(CY_LED8_PORT, CY_LED8_PIN);
        Cy_GPIO_Inv(CY_LED9_PORT, CY_LED9_PIN);
    }
}



/* [] END OF FILE */
