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
#include "flexray_port_config.h"

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
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = USER_LED_PIN_MUX,                           
    .intEdge = 0,                                  
    .intMask = 0,                                  
    .vtrip = 0,                                    
    .slewRate = 0,                                 
    .driveSel = 0,                                 
    .vregEn = 0,                                   
    .ibufMode = 0,                                 
    .vtripSel = 0,                                 
    .vrefSel = 0,                                  
    .vohSel = 0,                                   
};

int main(void)
{
    SystemInit();
  
    __enable_irq();
    
    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    // Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    // Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);
   
    /** ============= Port Setup ====================== */
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);
    
    /* Initialize all FlexRay port pins */
    for (uint8_t i = 0; i < (sizeof(flexray_pin_cfg) / sizeof(flexray_pin_cfg[0])); i++)
    {
        Cy_GPIO_Pin_Init(flexray_pin_cfg[i].portReg, flexray_pin_cfg[i].pinNum, &flexray_pin_cfg[i].cfg);
    }
    
    /** ============= Clock Setup ====================== */
    
    /* FlexRay peripheral clock is derived from CLK_HF2, so it needs to be enabled */
    Cy_SysClk_HfClkEnable(CY_SYSCLK_HFCLK_2);
    
  #if (CY_USE_PSVP == 1)
    // For PSVP we need to use the fractional divider to get to one of the standard bitrates e.g. supported by Vector CANoe (i.e. 1.25, 2.5, 5, 10 MBit/s)
    // If no such analyzer shall be used and user can control the frequency of the other network nodes, also the integer divider (e.g. 24 MHz) can be used
    // For 2.5MBit/s operation we need 20 MHz, there are 2 fractional dividers possible which result in a frequency slightly lower and higher.
    // The higher frequency turned out to be working if the other nodes run at nominal 20 MHz. But maybe for different setups/tolerances this may change
    
    Cy_SysClk_PeriphAssignDivider(PCLK_FLEXRAY0_CLK_FLEXRAY, CY_SYSCLK_DIV_24_5_BIT, 0u);    
    // Cy_SysClk_PeriphSetFracDivider(Cy_SysClk_GetClockGroup(PCLK_FLEXRAY0_CLK_FLEXRAY), CY_SYSCLK_DIV_24_5_BIT, 0u, 0u, 7u);  // divider 1+(7/32) = 1.21875 --> 24 MHz / 1.21875 = 19.69MHz -> ~2.500 MBit/s at 8 times oversampling
    Cy_SysClk_PeriphSetFracDivider(Cy_SysClk_GetClockGroup(PCLK_FLEXRAY0_CLK_FLEXRAY), CY_SYSCLK_DIV_24_5_BIT, 0u, 0u, 6u);  // divider 1+(6/32) = 1.1875 --> 24 MHz / 1.1875 = 20.21MHz -> ~2.500 MBit/s at 8 times oversampling
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(PCLK_FLEXRAY0_CLK_FLEXRAY), CY_SYSCLK_DIV_24_5_BIT, 0u);
  #else
    
    // CLK_HF2 is configured to 80 MHz by SystemInit

    Cy_SysClk_PeriphAssignDivider(PCLK_FLEXRAY0_CLK_FLEXRAY, CY_SYSCLK_DIV_8_BIT, 0u);
    Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(PCLK_FLEXRAY0_CLK_FLEXRAY), CY_SYSCLK_DIV_8_BIT, 0u, 0u); // divide-by-1 --> 80 MHz
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(PCLK_FLEXRAY0_CLK_FLEXRAY), CY_SYSCLK_DIV_8_BIT, 0u);
  #endif
    
    
    /** ============= FlexRay Controller Setup ====================== */
    
    // Will set global enable bit of FlexRay IP
    cy_fr_api_enable_controller(FLEXRAY0);
    
    
    /** ============= FlexRay I/O Test Mode ====================== */
    
    // Step through with debugger and measure the output signals on scope or provide GND/VCCIO on the input signals and observe variable
    
    // Setting Write Enable Bit in TEST1 register requires unlock sequence first:
    FLEXRAY0->unLCK.u32Register = 0x00007500;
    FLEXRAY0->unLCK.u32Register = 0x00008A00;
    FLEXRAY0->unTEST1.stcField.u1WRTEN = 1; // Set Write Enable
    FLEXRAY0->unTEST1.stcField.u2TMC   = 2; // Enter I/O Test Mode
    
    FLEXRAY0->unTEST1.stcField.u1TXA   = 1;  // TP67 (Adapter Board)
    FLEXRAY0->unTEST1.stcField.u1TXA   = 0;  
    FLEXRAY0->unTEST1.stcField.u1TXB   = 1;  // TP46 (Adapter Board)
    FLEXRAY0->unTEST1.stcField.u1TXB   = 0;  
    FLEXRAY0->unTEST1.stcField.u1TXENA = 1;  // TP49 (Adapter Board)
    FLEXRAY0->unTEST1.stcField.u1TXENA = 0;  
    FLEXRAY0->unTEST1.stcField.u1TXENB = 1;  // TP63 (Adapter Board)
    FLEXRAY0->unTEST1.stcField.u1TXENB = 0;
    
    volatile uint32_t read_val;
    read_val = FLEXRAY0->unTEST1.stcField.u1RXA; // RXA  // TP50 (Adapter Board)
    read_val = FLEXRAY0->unTEST1.stcField.u1RXA; // RXA
    read_val = FLEXRAY0->unTEST1.stcField.u1RXB; // RXB  // TP64 (Adapter Board)
    read_val = FLEXRAY0->unTEST1.stcField.u1RXB; // RXB

    for(;;)
    {
        Cy_SysTick_DelayInUs(50000);

        Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
    }
}



/* [] END OF FILE */
