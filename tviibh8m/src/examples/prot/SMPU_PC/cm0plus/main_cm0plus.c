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
#include "prot_setting_params.h"

#define MASTER_ID_OF_THIS_CPU CPUSS_MS_ID_CM0

const cy_stc_smpu_cfg_t smpuStruct2Config =
{
    .address        = (uint32_t*)(TP_PERMITTED_ADDR),
    .regionSize     = CY_PROT_SIZE_4KB,                       // 4KB: 0x1000 Byte
    .subregions     = 0x00,
    .userPermission = CY_PROT_PERM_RWX,
    .privPermission = CY_PROT_PERM_RWX,
    .secure         = 0,                                      // Non secure
    .pcMatch        = 0,
    .pcMask         = 1 << (TP_PERMITTED_CONTEXT - 1),        // enable context "TP_PERMITTED_CONTEXT"
};

const cy_stc_smpu_cfg_t smpuStruct3Config =
{
    .address        = (uint32_t*)(TP_PROHIBITED_ADDR),
    .regionSize     = CY_PROT_SIZE_4KB,                        // 4KB: 0x1000 Byte
    .subregions     = 0x00,
    .userPermission = CY_PROT_PERM_RWX,
    .privPermission = CY_PROT_PERM_RWX,
    .secure         = 0,                                       // Non secure
    .pcMatch        = 0,
    .pcMask         = 1 << (TP_PROHIBITED_CONTEXT - 1),        // enable context "TP_PROHIBITED_CONTEXT"
};

int main(void)
{
    /* Enable global interrupts. */
    __enable_irq();
	
    SystemInit();

    cy_en_prot_status_t status;

    /* Enable CM7_0. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);

    /**************************/
    /* 1. Setting for MSx_CTL */
    /**************************/
    /* 1.1 Setting for MSx_CTL (for this CPU) to allow the PC value to become "TP_PROT_CONTEXT" */
    status = Cy_Prot_ConfigBusMaster(MASTER_ID_OF_THIS_CPU, TP_PRIVILEGED, TP_SECURE, 1 << (TP_PROT_CONTEXT-1));
    CY_ASSERT(status == CY_PROT_SUCCESS);

    /**************************/
    /* 2. Setting for MPU PC  */
    /**************************/
    /* 2.1 Setting for MPU so that this CPU's PC value becomes "TP_PROT_CONTEXT" */
    status = Cy_Prot_SetActivePC(MASTER_ID_OF_THIS_CPU, TP_PROT_CONTEXT);
    CY_ASSERT(status == CY_PROT_SUCCESS);

    /********************************/
    /* 3. Setting for SMPU_STRUCT 2 */
    /********************************/
    /* 3.1 Disable SMPU_STRUCT 2 */
    status = Cy_Prot_DisableSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT2);
    CY_ASSERT(status == CY_PROT_SUCCESS);

    /* 3.2 Setting SMPU_STRUCT 2 for PERMITTED area */
    status = Cy_Prot_ConfigSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT2, &smpuStruct2Config);
    CY_ASSERT(status == CY_PROT_SUCCESS);

    /* 3.3 Enable SMPU_STRUCT 2 */
    status = Cy_Prot_EnableSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT2);
    CY_ASSERT(status == CY_PROT_SUCCESS);

    /********************************/
    /* 4. Setting for SMPU_STRUCT 3 */
    /********************************/
    /* 4.1 Disable SMPU_STRUCT 3 */
    status = Cy_Prot_DisableSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT3);
    CY_ASSERT(status == CY_PROT_SUCCESS);

    /* 4.2 Setting SMPU_STRUCT 3 for PROHIBITED area */
    status = Cy_Prot_ConfigSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT3, &smpuStruct3Config);
    CY_ASSERT(status == CY_PROT_SUCCESS);

    /* 4.3 Enable SMPU_STRUCT 3 */
    status = Cy_Prot_EnableSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT3);
    CY_ASSERT(status == CY_PROT_SUCCESS);

    /********************************/
    /* 5.    Memory access test1    */
    /********************************/
    /* 5.1 This area can be accessed by this master */
    *(uint32_t*)(TP_PERMITTED_ADDR) = 0xA5A5A5A5; /* write dummy data */
    *(volatile uint32_t*)(TP_PERMITTED_ADDR);     /* dummy read */

    /********************************/
    /* 6.    Memory access test2    */
    /********************************/
    /* 6.1 This area can NOT be accessed by this master */
    /* These accessing are expected to cause HardFault */
    *(uint32_t*)(TP_PROHIBITED_ADDR) = 0xA5A5A5A5; /* write dummy data <- will be get HardFault here. */
    *(volatile uint32_t*)(TP_PROHIBITED_ADDR);     /* dummy read */

    for(;;);
}

/* [] END OF FILE */
