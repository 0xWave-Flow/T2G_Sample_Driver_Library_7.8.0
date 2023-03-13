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

#if (CY_USE_PSVP == 1)
    #define USER_LED_PORT           CY_LED0_PORT
    #define USER_LED_PIN            CY_LED0_PIN
    #define USER_LED_PIN_MUX        CY_LED0_PIN_MUX
#else
    #define USER_LED_PORT           CY_CB_USER_LED2_PORT
    #define USER_LED_PIN            CY_CB_USER_LED2_PIN
    #define USER_LED_PIN_MUX        CY_CB_USER_LED2_PIN_MUX
#endif

const cy_stc_gpio_pin_config_t user_led0_port_pin_cfg =
{
    .outVal    = 0x00,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = USER_LED_PIN_MUX,
    .intEdge   = 0,
    .intMask   = 0,
    .vtrip     = 0,
    .slewRate  = 0,
    .driveSel  = 0,
};

const cy_stc_sysint_irq_t irq_cfg_evtgen_dpslp =
{
    .sysIntSrc  = evtgen_0_interrupt_dpslp_IRQn,
    .intIdx     = CPUIntIdx3_IRQn,
    .isEnabled  = true,
};

const cy_stc_sysint_irq_t irq_cfg_evtgen_active =
{
    .sysIntSrc  = evtgen_0_interrupt_IRQn,
    .intIdx     = CPUIntIdx3_IRQn,
    .isEnabled  = true,
};

const cy_stc_evtgen_config_t evtgenTestConfig =
{
  #if (CY_USE_PSVP == 1)
    .frequencyRef          = 24000000,  // clk_ref = clk_hf1 = CLK_PATH0 -> 24,000,000 for PSVP test.
    .frequencyLf           = 32000,     // clk_lf = 32,000 for PSVP test
  #else
    .frequencyRef          = 8000000,   // clk_ref = clk_hf1 = CLK_PATH2 (IMO) -> 8,000,000 for silicon
    .frequencyLf           = 32000,     // clk_lf = 32,000 for silicon
  #endif
    
    .frequencyTick         = 1000000,   // Setting 1,000,000 Hz for event generator clock (clk_ref_div)
    .ratioControlMode      = CY_EVTGEN_RATIO_CONTROL_SW,
    .ratioValueDynamicMode = CY_EVTGEN_RATIO_DYNAMIC_MODE0,
};

const cy_stc_evtgen_struct_config_t evtgenTestStructureConfig = 
{
    .functionalitySelection   = CY_EVTGEN_DEEPSLEEP_FUNCTIONALITY,
    .triggerOutEdge           = CY_EVTGEN_EDGE_SENSITIVE,
    .valueDeepSleepComparator = 2000000, // In active functionality, this value is used for making period of interrupts/triggers
                                         // 2,000,000 / 1,000,000 (clk_ref_div) = 2[s]
    .valueActiveComparator    = 4000000, // In active functionality, this value is used for making period of interrupts/triggers
                                         // 4,000,000 / 1,000,000 (clk_ref_div) = 4[s]
};

cy_stc_evtgen_struct_context_t evtgenStruct0Context;

uint8_t g_flag = 0;

void irqEVTGEN_sleep(void)
{
    Cy_Evtgen_ClearStructInterruptDeepSleep(EVTGEN0,0);
}

void irqEVTGEN_active(void)
{
    Cy_Evtgen_ClearStructInterrupt(EVTGEN0,0);
    Cy_Evtgen_DeinitializeCompStruct(EVTGEN0,0);
    Cy_Evtgen_InitializeCompStruct(EVTGEN0,0, &evtgenTestStructureConfig, &evtgenStruct0Context);
    g_flag = 1;
}

int main(void)
{
    SystemInit();

    // This is temoprary code, since this should be set by default.
    CPUSS->unSLOW_CLOCK_CTL.stcField.u8INT_DIV = 0u;

    __enable_irq(); /* Enable global interrupts. */

    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    //Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);  

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led0_port_pin_cfg);
    Cy_GPIO_Write(USER_LED_PORT, USER_LED_PIN, 1);

    /*******************************************/
    /*        Deinitialize peripherals         */
    /*******************************************/
    Cy_Evtgen_DeinitializeCompStruct(EVTGEN0,0);
    Cy_Evtgen_Deinitialize(EVTGEN0);
    NVIC_ClearPendingIRQ(CPUIntIdx3_IRQn);

    /*******************************************/
    /*  Interrupt setting for Event Generator  */
    /*******************************************/
    Cy_SysInt_InitIRQ(&irq_cfg_evtgen_dpslp);
    Cy_SysInt_SetSystemIrqVector(irq_cfg_evtgen_dpslp.sysIntSrc, irqEVTGEN_sleep);
    Cy_SysInt_InitIRQ(&irq_cfg_evtgen_active);
    Cy_SysInt_SetSystemIrqVector(irq_cfg_evtgen_active.sysIntSrc, irqEVTGEN_active);
    NVIC_EnableIRQ(CPUIntIdx3_IRQn);

    /*********************/
    /*  Enable clk_hf3   */
    /*********************/
  #if (CY_USE_PSVP == 1)
    Cy_SysClk_HfClkEnable(CY_SYSCLK_HFCLK_3);
    Cy_SysClk_HfClockSetSource(CY_SYSCLK_HFCLK_3, CY_SYSCLK_HFCLK_IN_CLKPATH0);
  #else
    Cy_SysClk_ClkPathSetSource(2u, CY_SYSCLK_CLKPATH_IN_IMO);
    Cy_SysClk_HfClkEnable(CY_SYSCLK_HFCLK_3);
    Cy_SysClk_HfClockSetSource(CY_SYSCLK_HFCLK_3, CY_SYSCLK_HFCLK_IN_CLKPATH2);
  #endif

    /*******************************************/
    /*  Initialize and start Event generator   */
    /*******************************************/
    Cy_Evtgen_Initialize(EVTGEN0,&evtgenTestConfig);

    /*******************************************/
    /*  Initialize comparator structure 0      */
    /*******************************************/
    Cy_Evtgen_InitializeCompStruct(EVTGEN0,0, &evtgenTestStructureConfig, &evtgenStruct0Context);

    g_flag = 1;
    while(1)
    {
        while(g_flag == 0);
        g_flag = 0;
        Cy_GPIO_Write(USER_LED_PORT, USER_LED_PIN, 0);
        /* Put the system to DeeSleep */
        Cy_SysPm_DeepSleep((cy_en_syspm_waitfor_t)CY_SYSPM_WAIT_FOR_INTERRUPT);
        Cy_GPIO_Write(USER_LED_PORT, USER_LED_PIN, 1);
    }
}



/* [] END OF FILE */
