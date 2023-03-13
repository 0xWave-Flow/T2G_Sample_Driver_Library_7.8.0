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

#define USER_LED1_PORT                  CY_LED1_PORT
#define USER_LED1_PIN                   CY_LED1_PIN
#define USER_LED1_PIN_MUX               CY_LED1_PIN_MUX

#define USER_LED2_PORT                  CY_CB_USER_LED1_PORT
#define USER_LED2_PIN                   CY_CB_USER_LED1_PIN
#define USER_LED2_PIN_MUX               CY_CB_USER_LED1_PIN_MUX

#define HIB_WAKE_BUTTON_PORT            CY_CB_HIB_WAKE_BUTTON_PORT
#define HIB_WAKE_BUTTON_PIN             CY_CB_HIB_WAKE_BUTTON_PIN
#define HIB_WAKE_BUTTON_PIN_MUX         CY_CB_HIB_WAKE_BUTTON_PIN_MUX

#define HIB_WAKE_BUTTON_IRQ             CY_CB_HIB_WAKE_BUTTON_IRQN


cy_stc_gpio_pin_config_t user_led1_port_pin_cfg =
{
    .outVal    = 0u,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = USER_LED1_PIN_MUX,
    .intEdge   = 0u,
    .intMask   = 0u,
    .vtrip     = 0u,
    .slewRate  = 0u,
    .driveSel  = 0u,
};

cy_stc_gpio_pin_config_t user_led2_port_pin_cfg =
{
    .outVal    = 0u,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = USER_LED2_PIN_MUX,
    .intEdge   = 0u,
    .intMask   = 0u,
    .vtrip     = 0u,
    .slewRate  = 0u,
    .driveSel  = 0u,
};


cy_stc_gpio_pin_config_t hib_wake_button_port_pin_cfg = 
{
    .outVal    = 0u,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom     = HIB_WAKE_BUTTON_PIN_MUX,
    .intEdge   = CY_GPIO_INTR_FALLING,
    .intMask   = 1u,
    .vtrip     = 0u,
    .slewRate  = 0u,
    .driveSel  = 0u,
};

/******   Clock Output Utilities   ******/
cy_stc_gpio_pin_config_t clkOutPortConfig =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = CY_HF3_CLK_OUT_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

/*
 * This is an example for GPIO Hibernate Wake up from hibernate button
 * This example uses 2 LED port and 1 button port on the CPU Board(hiberante button)
 * Settings are:
 *  - SW3 : Detects falling edge. When falling edge is detected, CPU wakes up hibernate.
 */

int main(void)
{
    bool resetCause = false;

    SystemInit();

   /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR); 

    __enable_irq(); /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(USER_LED1_PORT, USER_LED1_PIN, &user_led1_port_pin_cfg);
    Cy_GPIO_Pin_Init(USER_LED2_PORT, USER_LED2_PIN, &user_led2_port_pin_cfg);
    Cy_GPIO_Pin_Init(HIB_WAKE_BUTTON_PORT, HIB_WAKE_BUTTON_PIN, &hib_wake_button_port_pin_cfg);
    /* Start output internal clock */
    Cy_GPIO_Pin_Init(CY_HF3_CLK_OUT_PORT, CY_HF3_CLK_OUT_PIN, &clkOutPortConfig);

    resetCause = Cy_SysReset_IsResetDueToHibWakeup();
    
    if(resetCause == true)
    {
        // LED1 is used to indicate that the wake up is from the hibernate
        Cy_GPIO_Inv(USER_LED1_PORT, USER_LED1_PIN);
    }
   
    /* Check the IO status. If current status is frozen, unfreeze the system. */
    if(Cy_SysPm_GetIoFreezeStatus())
    {
        /* Unfreeze the system */
        Cy_SysPm_IoUnfreeze();
    }
    
    // Toggle an LED for sometime
    for(uint16_t i = 0u; i < 200u; i++)
    {
        /* Turn on LED for to some time to indicate the hibernate mode. */
        Cy_SysTick_DelayInUs(50000ul);

        // LED2 on the cpu board blinks for sometime
        Cy_GPIO_Inv(USER_LED2_PORT, USER_LED2_PIN);
    }
    
    /* Enable wake up source */
    Cy_SysPm_SetHibWakeupSource(CY_SYSPM_HIBPIN0_LOW);

    /* Jump to Hibernate Mode */
    Cy_SysPm_Hibernate();

    for(;;)
    {
    }
}


/* [] END OF FILE */
