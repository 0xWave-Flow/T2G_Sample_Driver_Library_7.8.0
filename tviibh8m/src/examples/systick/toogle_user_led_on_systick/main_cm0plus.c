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

#if CY_USE_PSVP == 1
  #define USER_LED_PORT           CY_LED0_PORT
  #define USER_LED_PIN            CY_LED0_PIN
  #define USER_LED_PIN_MUX        CY_LED0_PIN_MUX
#else
  #define USER_LED_PORT           CY_CB_USER_LED1_PORT
  #define USER_LED_PIN            CY_CB_USER_LED1_PIN
  #define USER_LED_PIN_MUX        CY_CB_USER_LED1_PIN_MUX
#endif

#if (CY_USE_PSVP == 1)  
    #define SYSTICK_RELOAD_VAL   (10000000UL)
#else  
    #define SYSTICK_RELOAD_VAL   (10000000UL * 5)
#endif 

cy_stc_gpio_pin_config_t user_led0_port_pin_cfg = 
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

uint32_t counterTick = 0;


void toggle_gpio_on_systick_handler(void)
{
    
    counterTick++;
    if(counterTick > 10)
    {
        counterTick = 0;
        Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
    }
}


int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */
    
	/* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR); 

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led0_port_pin_cfg);
    Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_CPU, SYSTICK_RELOAD_VAL);
    Cy_SysTick_SetCallback(0, toggle_gpio_on_systick_handler);
    Cy_SysTick_Enable();
    
    for(;;)
    {

    }      

}

/* [] END OF FILE */
