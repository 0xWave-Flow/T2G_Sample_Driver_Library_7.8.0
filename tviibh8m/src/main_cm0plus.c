/***************************************************************************//**
* \file main_cm0plus.c
*
* \brief
* Main file for CM0+
*
********************************************************************************
* \copyright
* Copyright 2016-2019, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"

#if (CY_USE_PSVP == 1)
    #define USER_LED_PORT           CY_AB_LED_PORT
    #define USER_LED_PIN            CY_AB_LED_PIN
    #define USER_LED_PIN_MUX        CY_AB_LED_PIN_MUX
#else
    #define USER_LED_PORT           CY_CB_USER_LED1_PORT
    #define USER_LED_PIN            CY_CB_USER_LED1_PIN
    #define USER_LED_PIN_MUX        CY_CB_USER_LED1_PIN_MUX 
#endif


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
    __enable_irq();
	
    SystemInit();

    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);

    for(;;)
    {
        // Wait 0.05 [s]
        Cy_SysTick_DelayInUs(50000ul);

        Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
    }
}



/* [] END OF FILE */
