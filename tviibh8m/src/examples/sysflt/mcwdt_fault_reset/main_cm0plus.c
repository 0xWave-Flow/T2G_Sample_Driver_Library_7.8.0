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

#define TEST_NO_FAULT_NO_RESET    (0) // Expected Behavior: Only LED0 will turn on
#define TEST_FAULT_THEN_WDT_CLEAR (1) // Expected Behavior: LED0 will turn on, LED1 will brink
#define TEST_FAULT_THEN_RESET     (2) // Expected Behavior: (LED0, LED1): (off, off)
                                      //                               -> ( on, off)
                                      //                               -> ( on,  on)(very short term)
                                      //                               -> (off, off)

#define TEST_MODE (TEST_FAULT_THEN_RESET)

#define MCWDT_TICKS_PER_SECOND     (32000u)    /* 1 sec when clk_lf = 32 KHz */

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

void irqFaultReport0Handler(void)
{
    cy_en_sysflt_source_t status;
    uint32_t faultData;

    /* Clear Interrupt flag */
    Cy_SysFlt_ClearInterrupt(FAULT_STRUCT0);
    
    status = Cy_SysFlt_GetErrorSource(FAULT_STRUCT0);

    if(status != CY_SYSFLT_NO_FAULT)
    {
        if(status == CY_SYSFLT_SRSS_MCWDT0) // MCWDT0 fault 
        {
            faultData = Cy_SysFlt_GetData0(FAULT_STRUCT0);
            if(faultData & 0x00000001ul)
            {
                 // subcounter 0 lower limit fault
            }
            else if(faultData & 0x00000002ul)
            {
                // subcounter 0 lower limit fault
            }
            else if(faultData & 0x00000004ul)
            {
                 // subcounter 1 lower limit fault
            }
            else if(faultData & 0x00000008ul)
            {
                Cy_GPIO_Inv(CY_LED1_PORT, CY_LED1_PIN); // subcounter 1 upper limit fault
#if (TEST_MODE == TEST_FAULT_THEN_WDT_CLEAR)
                // This need to be done in 6 clk_lf after WDT upper limit
                // Fault generation take up to 3 clk_lf. SW is given at least 3 clk_lf to clear WDT in fault handler.
                Cy_MCWDT_ClearWatchdog(MCWDT0, CY_MCWDT_COUNTER1);
#endif
            }
        }
    }

    Cy_SysFlt_ClearStatus(FAULT_STRUCT0);
}

cy_stc_sysint_irq_t irq_cfg =
{
    .sysIntSrc  = cpuss_interrupts_fault_0_IRQn,
    .intIdx     = CPUIntIdx2_IRQn,
    .isEnabled  = true,
};

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /*********************************************************************/
    /*****                 Fault report settings                     *****/
    /*********************************************************************/
    Cy_SysFlt_ClearInterrupt(FAULT_STRUCT0);
    Cy_SysFlt_ClearStatus(FAULT_STRUCT0); // clear status (typically this process is done by boot code)
    Cy_SysFlt_SetMaskByIdx(FAULT_STRUCT0, CY_SYSFLT_SRSS_MCWDT0); // enable Fault MCWDT #0
    Cy_SysFlt_SetInterruptMask(FAULT_STRUCT0);

    /*********************************************************************/
    /*****                     Port settings                         *****/
    /*********************************************************************/
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    user_led_port_pin_cfg.hsiom  = CY_LED0_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED0_PORT, CY_LED0_PIN, &user_led_port_pin_cfg);

    user_led_port_pin_cfg.hsiom  = CY_LED1_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_LED1_PORT, CY_LED1_PIN, &user_led_port_pin_cfg);

    // wait 2.4 [s]
    Cy_SysTick_DelayInUs(2400000ul);

    Cy_GPIO_Inv(CY_LED0_PORT, CY_LED0_PIN);

    /*********************************************************************/
    /*****                   Interrupt setting                       *****/
    /*********************************************************************/
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, irqFaultReport0Handler);
    NVIC_SetPriority(irq_cfg.intIdx, 0ul);
    NVIC_EnableIRQ(irq_cfg.intIdx);

    /*********************************************************************/
    /*****                        Set actions                        *****/
    /*********************************************************************/
    Cy_MCWDT_SetLowerAction(MCWDT0, CY_MCWDT_COUNTER0, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetUpperAction(MCWDT0, CY_MCWDT_COUNTER0, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetWarnAction(MCWDT0, CY_MCWDT_COUNTER0, CY_MCWDT_WARN_ACTION_NONE);

    Cy_MCWDT_SetLowerAction(MCWDT0, CY_MCWDT_COUNTER1, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetUpperAction(MCWDT0, CY_MCWDT_COUNTER1, CY_MCWDT_ACTION_FAULT_THEN_RESET); // sub counter 1 upper limit causes to reset
    Cy_MCWDT_SetWarnAction(MCWDT0, CY_MCWDT_COUNTER1, CY_MCWDT_WARN_ACTION_NONE);

    Cy_MCWDT_SetSubCounter2Action(MCWDT0, CY_MCWDT_CNT2_ACTION_NONE);

    /*********************************************************************/
    /*****                      Set limit values                     *****/
    /*********************************************************************/
    Cy_MCWDT_SetLowerLimit(MCWDT0, CY_MCWDT_COUNTER1, 0u, 0u);
    Cy_MCWDT_SetWarnLimit(MCWDT0, CY_MCWDT_COUNTER1, 0u, 0u);
    Cy_MCWDT_SetUpperLimit(MCWDT0, CY_MCWDT_COUNTER1, MCWDT_TICKS_PER_SECOND, 0u);

    /*********************************************************************/
    /*****                        Set options                        *****/
    /*********************************************************************/
    Cy_MCWDT_SetAutoService(MCWDT0, CY_MCWDT_COUNTER1, 0u); // disable
    Cy_MCWDT_SetSleepDeepPause(MCWDT0, CY_MCWDT_COUNTER1, 1u); // enable
    Cy_MCWDT_SetDebugRun(MCWDT0, CY_MCWDT_COUNTER1, 1u); // enable
    Cy_MCWDT_Enable(MCWDT0, 
                    CY_MCWDT_CTR1,  // enable COUNTER1
                    0u);
    while(Cy_MCWDT_GetEnabledStatus(MCWDT0, CY_MCWDT_COUNTER1) != 1ul);
    
    for(;;)
    {
#if (TEST_MODE == TEST_NO_FAULT_NO_RESET)
        uint16_t lowerCounter = Cy_MCWDT_GetLowerLimit(MCWDT0, CY_MCWDT_COUNTER1);
        uint16_t countValue = (uint16_t)Cy_MCWDT_GetCount(MCWDT0, CY_MCWDT_COUNTER1);

        if(countValue > lowerCounter)
        {
            Cy_MCWDT_ClearWatchdog(MCWDT0, CY_MCWDT_COUNTER1);
            Cy_MCWDT_WaitForCounterReset(MCWDT0, CY_MCWDT_COUNTER1);
        }
#endif
    }
}



/* [] END OF FILE */
