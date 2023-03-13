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

#include "mcal_project.h"


int main(void)
{
    __enable_irq();
	
    // SystemInit(); // MCAL Mcu driver will be used for initialization of clocks, etc.
    
    Cy_WDT_Disable();

    Cy_Power_SwitchToPmic(CY_SYSREGHC_VADJ_1V100, CY_POWER_PMIC_ENABLE_HIGH, CY_POWER_PMIC_STATUS_ABNORMAL_LOW, 0u);
    
    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);

    for(;;)
    {
    }
}


/* [] END OF FILE */
