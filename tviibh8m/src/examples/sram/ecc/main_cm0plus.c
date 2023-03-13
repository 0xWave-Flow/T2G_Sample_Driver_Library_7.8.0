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
    #define USER_LED_PORT           CY_AB_LED_PORT
    #define USER_LED_PIN            CY_AB_LED_PIN
    #define USER_LED_PIN_MUX        CY_AB_LED_PIN_MUX
#else
    #define USER_LED_PORT           CY_CB_USER_LED1_PORT
    #define USER_LED_PIN            CY_CB_USER_LED1_PIN
    #define USER_LED_PIN_MUX        CY_CB_USER_LED1_PIN_MUX 
#endif

#define RAM_0_ADDRESS                  (0x08000500)
#define RAM_0_DATA                     (0x5A5A5A5A)
#define RAM_0_CORRECT_PARITY           (0x18)
#define RAM_0_ONEBIT_PARITY            (0x5D)
#define RAM_0_TWOBIT_PARITY            (0x14)

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

uint8_t isr_triggered = 0;
uint32_t faultData0, faultData1, faultData2, faultData3;
cy_en_sysflt_source_t fault_source;
    
void irqFaultReport0Handler(void)
{
    isr_triggered = 1;
    
    /* Clear Interrupt flag */
    Cy_SysFlt_ClearInterrupt(FAULT_STRUCT0);
    
    fault_source = Cy_SysFlt_GetErrorSource(FAULT_STRUCT0);

    if(fault_source != CY_SYSFLT_NO_FAULT)
    {
        if(fault_source == CY_SYSFLT_RAMC0_NC_ECC) // System SRAM 0 non-correctable ECC error
        {
            faultData0 = Cy_SysFlt_GetData0(FAULT_STRUCT0);
            faultData1 = Cy_SysFlt_GetData1(FAULT_STRUCT0);
            faultData2 = Cy_SysFlt_GetData2(FAULT_STRUCT0);
            faultData3 = Cy_SysFlt_GetData3(FAULT_STRUCT0);
        }

        if(fault_source == CY_SYSFLT_RAMC0_C_ECC) // System SRAM 0 correctable ECC error
        {
            faultData0 = Cy_SysFlt_GetData0(FAULT_STRUCT0);
            faultData1 = Cy_SysFlt_GetData1(FAULT_STRUCT0);
            faultData2 = Cy_SysFlt_GetData2(FAULT_STRUCT0);
            faultData3 = Cy_SysFlt_GetData3(FAULT_STRUCT0);
        } 
    }

    Cy_SysFlt_ClearStatus(FAULT_STRUCT0);
}

uint32_t pf_source = 0;
uint32_t fail = 0;

int main(void)
{    
    SystemInit();
    
    __enable_irq(); /* Enable global interrupts. */

    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);
    
    /*********************************************************************/
    /*****                   Interrupt setting                       *****/
    /*********************************************************************/
    {
        cy_stc_sysint_irq_t irq_cfg;
        irq_cfg = (cy_stc_sysint_irq_t){
            .sysIntSrc  = cpuss_interrupts_fault_0_IRQn,
            .intIdx     = CPUIntIdx2_IRQn,
            .isEnabled  = true,
        };
        Cy_SysInt_InitIRQ(&irq_cfg);
        Cy_SysInt_SetSystemIrqVector(cpuss_interrupts_fault_0_IRQn, irqFaultReport0Handler);
    }

    NVIC_SetPriority(CPUIntIdx2_IRQn, 0);
    NVIC_ClearPendingIRQ(CPUIntIdx2_IRQn);
    NVIC_EnableIRQ(CPUIntIdx2_IRQn);

    /***************************************************************************
    ###########################     SRAM 0     # ###############################
    ***************************************************************************/
    
    // VALIDATING FAULT SOURCE, RAM_0_C_IDX
    // SRAM 0 FAULT MASK AND SOURCE CONFIGURATION FOR ONE BIT STARTED
    // FAULT_STRUCT0->unMASK1.stcField.u32SOURCE = 0x04000000;    
    {
        Cy_SysFlt_ClearStatus(FAULT_STRUCT0); // clear status (typically this process is done by boot code)
        Cy_SysFlt_SetMaskByIdx(FAULT_STRUCT0, CY_SYSFLT_RAMC0_C_ECC); // enable Fault System SRAM 0 non-correctable ECC error
        Cy_SysFlt_SetInterruptMask(FAULT_STRUCT0);
    }
    
    CPUSS->unRAM0_CTL0.stcField.u1ECC_EN = 1;
    CPUSS->unRAM0_CTL0.stcField.u1ECC_AUTO_CORRECT = 0;
    CPUSS->unRAM0_CTL0.stcField.u1ECC_INJ_EN = 1;
    CPUSS->unECC_CTL.stcField.u24WORD_ADDR = (0x000500 >> 2);
    CPUSS->unECC_CTL.stcField.u8PARITY = RAM_0_ONEBIT_PARITY;
    // SRAM 0 FAULT MASK AND SOURCE CONFIGURATION FOR ONE BIT DONE
    
    // SRAM 0 DATA WRITE AT FIRST TIME STARTED 
    CY_SET_REG32(RAM_0_ADDRESS, RAM_0_DATA);    
    // SRAM 0 DATA WRITE AT FIRST TIME DONE
    
    // SRAM 0 DATA READ TO CAUSE THE ECC FAULT STARTED
    CY_GET_REG32(RAM_0_ADDRESS);
    
    Cy_SysLib_Delay(10);
    
    while(!isr_triggered);
    
    // SRAM 0 DATA READ TO CAUSE THE ECC FAULT DONE
    
    // SRAM 0 FAULT MASK AND SOURCE DE-INT FOR ONE BIT STARTED
    // FAULT_STRUCT0->unMASK1.stcField.u32SOURCE = 0x00000000;
    Cy_SysFlt_ClearMaskByIdx(FAULT_STRUCT0, CY_SYSFLT_NO_FAULT);
    
    CPUSS->unRAM0_CTL0.stcField.u1ECC_EN = 1;
    CPUSS->unRAM0_CTL0.stcField.u1ECC_AUTO_CORRECT = 1;
    CPUSS->unRAM0_CTL0.stcField.u1ECC_INJ_EN = 0;
    // SRAM 0 FAULT MASK AND SOURCE DE-INT FOR ONE BIT DONE       
    
    if( (isr_triggered != 0) && (faultData0 != RAM_0_ADDRESS) && (fault_source != CY_SYSFLT_RAMC0_C_ECC))
    {
        fail ++;
    }
    else
    {
        isr_triggered = 0;
    }
   
    // VALIDATING FAULT SOURCE, RAM_0_NC_IDX
    // SRAM 0 FAULT MASK AND SOURCE CONFIGURATION FOR TWO BIT STARTED
    // FAULT_STRUCT0->unMASK1.stcField.u32SOURCE = 0x08000000;    
    {
        Cy_SysFlt_ClearStatus(FAULT_STRUCT0); // clear status (typically this process is done by boot code)
        Cy_SysFlt_SetMaskByIdx(FAULT_STRUCT0, CY_SYSFLT_RAMC0_NC_ECC); // enable Fault System SRAM 0 non-correctable ECC error
        Cy_SysFlt_SetInterruptMask(FAULT_STRUCT0);
    }
    
    CPUSS->unRAM0_CTL0.stcField.u1ECC_EN = 1;
    CPUSS->unRAM0_CTL0.stcField.u1ECC_AUTO_CORRECT = 0;
    CPUSS->unRAM0_CTL0.stcField.u1ECC_INJ_EN = 1;
    CPUSS->unECC_CTL.stcField.u24WORD_ADDR = (0x000500 >> 2);
    CPUSS->unECC_CTL.stcField.u8PARITY = RAM_0_TWOBIT_PARITY;
    // SRAM 0 FAULT MASK AND SOURCE CONFIGURATION FOR TWO BIT DONE
    
    // SRAM 0 DATA WRITE AT FIRST TIME STARTED
    CY_SET_REG32(RAM_0_ADDRESS, RAM_0_DATA);    
    // SRAM 0 DATA WRITE AT FIRST TIME DONE
    
    // SRAM 0 DATA READ TO CAUSE THE ECC FAULT STARTED
    CY_GET_REG32(RAM_0_ADDRESS);
    Cy_SysLib_Delay(10);
    while(!isr_triggered);
    
    // SRAM 0 DATA READ TO CAUSE THE ECC FAULT DONE
    
    // SRAM 0 FAULT MASK AND SOURCE DE-INT FOR TWO BIT STARTED
    // FAULT_STRUCT0->unMASK1.stcField.u32SOURCE = 0x00000000;
    Cy_SysFlt_ClearMaskByIdx(FAULT_STRUCT0, CY_SYSFLT_NO_FAULT);
    
    CPUSS->unRAM0_CTL0.stcField.u1ECC_EN = 1;
    CPUSS->unRAM0_CTL0.stcField.u1ECC_AUTO_CORRECT = 1;
    CPUSS->unRAM0_CTL0.stcField.u1ECC_INJ_EN = 0;
    // SRAM 0 FAULT MASK AND SOURCE DE-INT FOR TWO BIT DONE 
    
    if((isr_triggered != 0) && (faultData0 != RAM_0_ADDRESS) && (fault_source != CY_SYSFLT_RAMC0_NC_ECC))
    {
        fail ++;
    }
    else
    {
        isr_triggered = 0;
    }
    
    for(;;)
    {
        Cy_SysTick_DelayInUs(50000);    
        
        Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
    }
}



/* [] END OF FILE */
