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

#define USER_LED_PORT    CY_CB_USER_LED2_PORT
#define USER_LED_PIN     CY_CB_USER_LED2_PIN
#define USER_LED_PIN_MUX CY_CB_USER_LED2_PIN_MUX

typedef union
{
    struct
    {
        uint32_t user_read          : 1;
        uint32_t user_write         : 1;
        uint32_t user_execute       : 1;
        uint32_t privileged_read    : 1;
        uint32_t privileged_write   : 1;
        uint32_t privileged_execute : 1;
        uint32_t non_secure         : 1;
        uint32_t                    : 1;
        uint32_t master             : 4;
        uint32_t protection_context : 4;
        uint32_t                    : 15;
        uint32_t mpu_smpu           : 1;
    } bf;
    uint32_t u32;
} un_mpuViolationInfo_t;

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

volatile uint32_t              g_violatingAddr = 0ul;
volatile un_mpuViolationInfo_t g_violatingInfo = {0ul};

void irqFaultReport0Handler(void)
{
    cy_en_sysflt_source_t status;
    
    /* Clear Interrupt flag */
    Cy_SysFlt_ClearInterrupt(FAULT_STRUCT0);

    status = Cy_SysFlt_GetErrorSource(FAULT_STRUCT0);

    if(status != CY_SYSFLT_NO_FAULT)
    {
        if(status == CY_SYSFLT_MPU_0) // Master 0 (CM0+) PPU fault
        {
            g_violatingAddr = Cy_SysFlt_GetData0(FAULT_STRUCT0);
            g_violatingInfo.u32 = Cy_SysFlt_GetData1(FAULT_STRUCT0);
            for(;;)
            {
                // delay 0.5 [s]
                Cy_SysTick_DelayInUs(500000ul);
                Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
            }
        }
    }
    
    Cy_SysFlt_ClearStatus(FAULT_STRUCT0);

    while(1);
}

cy_stc_sysflt_t g_fltConfig = 
{
    .ResetEnable   = false,
    .OutputEnable  = true,
    .TriggerEnable = false,
};

cy_stc_sysint_irq_t g_irq_cfg = 
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
    /*****                     Port settings                         *****/
    /*********************************************************************/
    user_led_port_pin_cfg.hsiom  = USER_LED_PIN_MUX;
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);

    /*********************************************************************/
    /*****                 Fault report settings                     *****/
    /*********************************************************************/
    Cy_SysFlt_ClearStatus(FAULT_STRUCT0); // clear status (typically this process is done by boot code)
    Cy_SysFlt_SetMaskByIdx(FAULT_STRUCT0, CY_SYSFLT_MPU_0); // enable Fault Master 0 (CM0+) MPU
    Cy_SysFlt_SetInterruptMask(FAULT_STRUCT0);
    Cy_SysFlt_Init(FAULT_STRUCT0, &g_fltConfig);

    /*********************************************************************/
    /*****                   Interrupt settings                      *****/
    /*********************************************************************/
    Cy_SysInt_InitIRQ(&g_irq_cfg);
    Cy_SysInt_SetSystemIrqVector(g_irq_cfg.sysIntSrc, irqFaultReport0Handler);
    NVIC_SetPriority(g_irq_cfg.intIdx, 0ul);
    NVIC_EnableIRQ(g_irq_cfg.intIdx);

    for(;;);
}



/* [] END OF FILE */
