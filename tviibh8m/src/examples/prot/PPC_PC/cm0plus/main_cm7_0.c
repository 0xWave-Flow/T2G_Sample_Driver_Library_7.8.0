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
#include "prot_setting_params.h"

#define COUNTERPART_PPU_FAULT CY_SYSFLT_PERI_MS_VIO_0 // Counterpart means CM0+
#define COUNTERPART_CPU_ID    CPUSS_MS_ID_CM0

#if CY_USE_PSVP == 1
  #define INDICATOR_LED_PORT    CY_LED8_PORT
  #define INDICATOR_LED_PIN     CY_LED8_PIN
  #define INDICATOR_LED_MUX     CY_LED8_PIN_MUX
#else
  # if defined(CY_CB_USER_LED2_PORT)
    #define INDICATOR_LED_PORT    CY_CB_USER_LED2_PORT
    #define INDICATOR_LED_PIN     CY_CB_USER_LED2_PIN
    #define INDICATOR_LED_MUX     CY_CB_USER_LED2_PIN_MUX
  #elif defined(CY_USER_LED2_PORT)
    #define INDICATOR_LED_PORT    CY_USER_LED2_PORT
    #define INDICATOR_LED_PIN     CY_USER_LED2_PIN
    #define INDICATOR_LED_MUX     CY_USER_LED2_PIN_MUX
  #else
    #error "No LED definitions in BSP."
  #endif
#endif


static const cy_stc_gpio_pin_config_t user_led_port_pin_cfg =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = INDICATOR_LED_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul, 
    .driveSel  = 0ul,
};

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
    } bitfld;
    uint32_t u32;
} un_mpuViolationInfo_t;

cy_stc_sysint_irq_t irq_cfg = 
{
    .sysIntSrc  = cpuss_interrupts_fault_0_IRQn,
    .intIdx     = CPUIntIdx2_IRQn,
    .isEnabled  = true,
};

void irqFaultReport0Handler(void)
{
    cy_en_sysflt_source_t status;
    volatile uint32_t violatingAddr = 0;
    un_mpuViolationInfo_t violatingInfo = {0u};
    
    /* Clear Interrupt flag */
    Cy_SysFlt_ClearInterrupt(FAULT_STRUCT0);

    /* 1. Checking error source */
    status = Cy_SysFlt_GetErrorSource(FAULT_STRUCT0);
    CY_ASSERT(status == COUNTERPART_PPU_FAULT);

    /* 2. Checking violation source */
    violatingAddr = Cy_SysFlt_GetData0(FAULT_STRUCT0);

    /* 3. Checking violation information */
    violatingInfo.u32 = Cy_SysFlt_GetData1(FAULT_STRUCT0);
    if(TP_PRIVILEGED == 0)
    {
        CY_ASSERT(violatingInfo.bitfld.user_read    == 1); // the fault should occur in write process
        CY_ASSERT(violatingInfo.bitfld.user_write   == 0);
        CY_ASSERT(violatingInfo.bitfld.user_execute == 0);
    }
    else
    {
        CY_ASSERT(violatingInfo.bitfld.privileged_read    == 1);  // the fault should occur in write process
        CY_ASSERT(violatingInfo.bitfld.privileged_write   == 0);
        CY_ASSERT(violatingInfo.bitfld.privileged_execute == 0);
    }
    CY_ASSERT(violatingInfo.bitfld.non_secure         != TP_SECURE);
    CY_ASSERT(violatingInfo.bitfld.master             == COUNTERPART_CPU_ID);
    CY_ASSERT(violatingInfo.bitfld.protection_context == TP_PROHIBITED_PC);
    CY_ASSERT(violatingInfo.bitfld.mpu_smpu == 0); /* should NOT be SMPU violation */
    
    Cy_SysFlt_ClearStatus(FAULT_STRUCT0);

    for(;;)
    {
        Cy_SysTick_DelayInUs(50000);
        Cy_GPIO_Inv(INDICATOR_LED_PORT, INDICATOR_LED_PIN);
    }
}

int main(void)
{
    /* Enable global interrupts. */
    __enable_irq();
    
    SystemInit();

    /*********************************************************************/
    /*****                     Port settings                         *****/
    /*********************************************************************/
    Cy_GPIO_Pin_Init(INDICATOR_LED_PORT, INDICATOR_LED_PIN, &user_led_port_pin_cfg);

    /*********************************************************************/
    /*****                 Fault report settings                     *****/
    /*********************************************************************/
    Cy_SysFlt_ClearStatus(FAULT_STRUCT0); // clear status (typically this process is done by boot code)
    Cy_SysFlt_SetMaskByIdx(FAULT_STRUCT0, COUNTERPART_PPU_FAULT); // enable the PPU
    Cy_SysFlt_SetInterruptMask(FAULT_STRUCT0);

    /*********************************************************************/
    /*****                   Interrupt settings                      *****/
    /*********************************************************************/
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, irqFaultReport0Handler);
    NVIC_SetPriority(irq_cfg.intIdx, 0);
    NVIC_EnableIRQ(irq_cfg.intIdx);

    for(;;);
}

/* [] END OF FILE */
