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

#if (CY_USE_PSVP == 1)  
    #define SYSTICK_RELOAD_VAL   (10000000UL)
#else  
    #define SYSTICK_RELOAD_VAL   (10000000UL * 5)
#endif 

cy_stc_gpio_pin_config_t user_led1_port_pin_cfg = 
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = CY_LED1_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};


void toggle_gpio_on_systick_handler(void)
{
    Cy_GPIO_Inv(CY_LED1_PORT, CY_LED1_PIN);
}


int main(void)
{
    SystemInit();

    // Example had been originally tested with "cache off", so ensure that caches are turned off (may have been enabled by new startup.c module)
    SCB_DisableICache(); // Disables and invalidates instruction cache
    SCB_DisableDCache(); // Disables, cleans and invalidates data cache

      uint32_t value;
      
    __enable_irq(); /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(CY_LED1_PORT, CY_LED1_PIN, &user_led1_port_pin_cfg);
    Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_CPU, SYSTICK_RELOAD_VAL);
    
#if (CY_USE_PSVP == 1) 
    /* Load the TENMS field value such that the clock cycles that make up 10msec
       Here, IMO = 24MHz. That means, it requires 24MHz*10mses cycles = 240000*/
    Cy_SysTick_Set10msCalibration(240000);
#else
	/* Load the TENMS field value such that the clock cycles that make up 10msec
       Here, PLL_OUT = 160 MHz. That means, it requires (160 MHz * 10 ms) cycles = 1600000 */
    Cy_SysTick_Set10msCalibration(1600000);
#endif
    
    Cy_SysTick_SetCallback(0, toggle_gpio_on_systick_handler);
    Cy_SysTick_Enable();
    
    value = Cy_SysTick_Get10msCalibration(); //CountValue_10ms();
    Cy_SysTick_Clear();
    Cy_SysTick_SetReload(value-1);
    
    for(;;)
    {

    }      

}

/* [] END OF FILE */
