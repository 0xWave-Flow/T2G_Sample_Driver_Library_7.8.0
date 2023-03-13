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

#define MASTER_CONTEXT_NO      (6ul)                             // must be greater than "0"
#define MASTER_CONTEXT_MASK    (1ul<<(MASTER_CONTEXT_NO-1ul))

#define TP_PERMITTED_SMPU_STC  (PROT_SMPU_SMPU_STRUCT3)
#define TP_PERMITTED_ADDR      (0x28019000ul)
#define TP_PERMITTED_CTX_NO    (MASTER_CONTEXT_NO)               // same as MASTER_CONTEXT_NO
#define TP_PERMITTED_CTX_MASK  (MASTER_CONTEXT_MASK)             // same as MASTER_CONTEXT_MASK

#define TP_PROHIBITED_SMPU_STC  (PROT_SMPU_SMPU_STRUCT2)
#define TP_PROHIBITED_ADDR     (0x28018000ul)
#define TP_PROHIBITED_CTX_NO   (5ul)
#define TP_PROHIBITED_CTX_MASK (1ul<<(TP_PROHIBITED_CTX_NO-1ul))

cy_stc_smpu_cfg_t g_smpuStructTemp =
{
    .address        = NULL,
    .regionSize     = CY_PROT_SIZE_4KB, // 4KB: 0x1000 Byte
    .subregions     = 0x00u,
    .userPermission = CY_PROT_PERM_RWX,
    .privPermission = CY_PROT_PERM_RWX,
    .secure         = false,            // Non secure
    .pcMatch        = false,
    .pcMask         = 0ul,              // enable context "TP_PROHIBITED_CTX_NO"
};

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */
    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);

    /* 1. Setting for MSx_CTL */
    //                                      busMaster,  privileged, secure,               pcMask
    CY_ASSERT(Cy_Prot_ConfigBusMaster(CPUSS_MS_ID_CM0,        true,  false,    MASTER_CONTEXT_MASK) == CY_PROT_SUCCESS);

    /* 2. Setting for MPU PC = "TEST_CONTEXT_NO" */
    /* This time MPU_PC can be set to only "MASTER_CONTEXT_NO" because only SMPU_MS_CTL_PC_MASK[TEST_CONTEXT_NO] = "1"*/
    CY_ASSERT(Cy_Prot_SetActivePC(CPUSS_MS_ID_CM0, MASTER_CONTEXT_NO) == CY_PROT_SUCCESS);

    /* 3. Setting for SMPU_STRUCT */
    /* 4KB from TP_PROHIBITED_ADDR: Only masters which have "TP_PROHIBITED_CTX_NO" as PC value can access this area */
    g_smpuStructTemp.address = (uint32_t*)(TP_PROHIBITED_ADDR);
    g_smpuStructTemp.pcMask  = TP_PROHIBITED_CTX_MASK;          // enable context "TP_PROHIBITED_CTX_NO"
    CY_ASSERT(Cy_Prot_ConfigSmpuSlaveStruct(TP_PROHIBITED_SMPU_STC, &g_smpuStructTemp) == CY_PROT_SUCCESS);

    /* 4KB from TP_PERMITTED_ADDR: Only masters which have "TP_PERMITTED_CTX_NO" as PC value can access this area */
    g_smpuStructTemp.address = (uint32_t*)(TP_PERMITTED_ADDR);
    g_smpuStructTemp.pcMask  = TP_PERMITTED_CTX_MASK;          // enable context "TP_PERMITTED_CTX_NO"
    CY_ASSERT(Cy_Prot_ConfigSmpuSlaveStruct(TP_PERMITTED_SMPU_STC, &g_smpuStructTemp) == CY_PROT_SUCCESS);

    /* 4. Enable SMPU_STRUCT */
    CY_ASSERT(Cy_Prot_EnableSmpuSlaveStruct(TP_PROHIBITED_SMPU_STC) == CY_PROT_SUCCESS);
    CY_ASSERT(Cy_Prot_EnableSmpuSlaveStruct(TP_PERMITTED_SMPU_STC) == CY_PROT_SUCCESS);

    /* 5. Memory access test1. This area can be accessed by masters which have "TP_PERMITTED_CTX_NO" as PC value */
    /* CM0+ has "TP_PERMITTED_CTX_NO" (= "MASTER_CONTEXT_NO") as PC value, then these accesses are expected to be allowed */
    volatile uint32_t testRead0 = 0ul;
    *(uint32_t*)(TP_PERMITTED_ADDR) = 0xA5A5A5A5ul;
    testRead0 = *(uint32_t*)(TP_PERMITTED_ADDR);

    /* 6. Memory access test2. This area can be accessed by masters which have "TP_PROHIBITED_CTX_NO" as PC value */
    /* CM0+ has "TP_PERMITTED_CTX_NO" as PC value, then these accesses are expected to be restricted and cause HardFault */
    volatile uint32_t testRead1 = 0ul;
    *(uint32_t*)(TP_PROHIBITED_ADDR) = 0xA5A5A5A5ul;
    testRead1 = *(uint32_t*)(TP_PROHIBITED_ADDR);

    for(;;);
}

/* [] END OF FILE */
