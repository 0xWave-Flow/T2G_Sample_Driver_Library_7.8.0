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

#define BACKGROUND_REGION_ADDR       (0x00000000ul) // Back Ground Region Start Address
#define CM7_SELF_ITCM_REGION_ADDR    (0x00000000ul) // Self Instruction TCM RAM Region Start Address
#define CODE_FLASH_REGION_ADDR       (0x10000000ul) // Code Flash Region Start Address
#define WORK_FLASH_REGION_ADDR       (0x14000000ul) // Work Flash Region Start Address
#define CM7_SELF_DTCM_REGION_ADDR    (0x20000000ul) // CM7 Self Data TCM RAM Region Start Address
#define SRAM_REGION_ADDR             (0x28000000ul) // System RAM Region Start Address
#define PERI_REGISTER_REGION_ADDR    (0x40000000ul) // Peripheral Register Region Start Address
#define SMIF0_XIP_REGION_ADDR        (0x60000000ul) // SMIF Channel 0 XIP Region Start Address
#define SMIF1_XIP_REGION_ADDR        (0x80000000ul) // SMIF Channel 1 XIP Region Start Address (Only For Cluster MCU)
#define CM7_0_ITCM_REGION_ADDR       (0xA0000000ul) // CM7_0 Instruction TCM RAM Region Start Address
#define CM7_0_DTCM_REGION_ADDR       (0xA0010000ul) // CM7_0 Data TCM RAM Region Start Address
#define CM7_1_ITCM_REGION_ADDR       (0xA0100000ul) // CM7_1 Instruction TCM RAM Region Start Address
#define CM7_1_DTCM_REGION_ADDR       (0xA0110000ul) // CM7_1 Data TCM RAM Region Start Address
#define ARM_SYS_REGISTER_REGION_ADDR (0xE0000000ul) // ARM System Registers Region Start Address

#define BACKGROUND_MPU_NO       (0)
#define CM7_SELF_ITCM_MPU_NO    (1)
#define CODE_FLASH_MPU_NO       (2)
#define WORK_FLASH_MPU_NO       (3)
#define CM7_SELF_DTCM_MPU_NO    (4)
#define SRAM_MPU_NO             (5)
#define PERI_REGISTER_MPU_NO    (6)
#define SMIF0_XIP_MPU_NO        (7)
#define SMIF1_XIP_MPU_NO        (8)
#define CM7_0_ITCM_MPU_NO       (9)
#define CM7_0_DTCM_MPU_NO       (10)
#define CM7_1_ITCM_MPU_NO       (11)
#define CM7_1_DTCM_MPU_NO       (12)
#define ARM_SYS_REGISTER_MPU_NO (13)

cy_stc_mpu_region_cfg_t g_mpuCfg[] =
{
    /*** Back Ground Region ***/
    {
        .addr       = BACKGROUND_REGION_ADDR,
        .size       = CY_MPU_SIZE_4GB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_NORM_MEM_WT,
        .execute    = CY_MPU_INST_ACCESS_EN,
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** CM7 Self-Instruction TCM RAM Region ***/
    {
        .addr       = CM7_SELF_ITCM_REGION_ADDR,
        .size       = CY_MPU_SIZE_16KB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_NORM_MEM_WT,
        .execute    = CY_MPU_INST_ACCESS_EN, 
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** Code Flash Region ***/
    {
        .addr       = CODE_FLASH_REGION_ADDR,
        .size       = CY_MPU_SIZE_8MB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_NORM_MEM_WT,
        .execute    = CY_MPU_INST_ACCESS_EN,
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** Work Flash Region ***/
    {
        .addr       = WORK_FLASH_REGION_ADDR,
        .size       = CY_MPU_SIZE_256KB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_NORM_MEM_WT,
        .execute    = CY_MPU_INST_ACCESS_DIS,
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** CM7 Self Data TCM RAM Region ***/
    {
        .addr       = CM7_SELF_DTCM_REGION_ADDR,
        .size       = CY_MPU_SIZE_16KB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_NORM_MEM_WT,
        .execute    = CY_MPU_INST_ACCESS_DIS,
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** System RAM Region ***/
    {
        .addr       = SRAM_REGION_ADDR,
        .size       = CY_MPU_SIZE_1MB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_NORM_MEM_WT,
        .execute    = CY_MPU_INST_ACCESS_EN,
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** Peripheral Register Region ***/
    {
        .addr       = PERI_REGISTER_REGION_ADDR,
        .size       = CY_MPU_SIZE_64MB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_SHR_DEV,
        .execute    = CY_MPU_INST_ACCESS_DIS,
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** SMIF Channel 0 XIP Region ***/
    {
        .addr       = SMIF0_XIP_REGION_ADDR,
        .size       = CY_MPU_SIZE_128MB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_STR_ORD_DEV,
        .execute    = CY_MPU_INST_ACCESS_EN, 
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** SMIF Channel 1 XIP Region ***/
    {
        .addr       = SMIF1_XIP_REGION_ADDR,
        .size       = CY_MPU_SIZE_128MB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_STR_ORD_DEV,
        .execute    = CY_MPU_INST_ACCESS_EN, 
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** CM7_0 Instruction TCM RAM Region ***/
    {
        .addr       = CM7_0_ITCM_REGION_ADDR,
        .size       = CY_MPU_SIZE_16KB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_NORM_SHR_MEM_WT,
        .execute    = CY_MPU_INST_ACCESS_EN, 
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** CM7_0 Data TCM RAM Region ***/
    {
        .addr       = CM7_0_DTCM_REGION_ADDR,
        .size       = CY_MPU_SIZE_16KB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_SHR_DEV,    
        .execute    = CY_MPU_INST_ACCESS_EN, 
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** CM7_1 Instruction TCM RAM Region ***/
    {
        .addr       = CM7_1_ITCM_REGION_ADDR,
        .size       = CY_MPU_SIZE_16KB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_SHR_DEV,
        .execute    = CY_MPU_INST_ACCESS_EN,
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** CM7_1 Data TCM RAM Region ***/
    {
        .addr       = CM7_1_DTCM_REGION_ADDR,
        .size       = CY_MPU_SIZE_16KB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_SHR_DEV,
        .execute    = CY_MPU_INST_ACCESS_EN,
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },

    /*** ARM System Registers Region ***/
    {
        .addr       = ARM_SYS_REGISTER_REGION_ADDR,
        .size       = CY_MPU_SIZE_512MB,
        .permission = CY_MPU_ACCESS_P_FULL_ACCESS,
        .attribute  = CY_MPU_ATTR_STR_ORD_DEV,
        .execute    = CY_MPU_INST_ACCESS_DIS,
        .srd        = 0x00u,
        .enable     = CY_MPU_ENABLE
    },
};

#define MPU_CFG_ARRAY_SIZE(array) (sizeof(array)/sizeof(cy_stc_mpu_region_cfg_t))

static void UnAlignedAccessTest(void);

int main(void)
{
    SystemInit();
    
    __enable_irq();

    /********* Core MPU setting **************/
    CY_ASSERT(Cy_MPU_Setup(g_mpuCfg, MPU_CFG_ARRAY_SIZE(g_mpuCfg), CY_MPU_DISABLE_USE_DEFAULT_MAP, CY_MPU_DISABLED_DURING_FAULT_NMI) == CY_MPU_SUCCESS);

    // Read from peripheral register region which is not restricted.
    *(volatile uint32_t*)(PERI_REGISTER_REGION_ADDR);

    /********* Try un-aligned access with All attribute for SRAM Region **************/
    UnAlignedAccessTest();

    /********* Core MPU setting **************/
    g_mpuCfg[PERI_REGISTER_MPU_NO].permission = CY_MPU_ACCESS_P_NO_ACCESS;
    CY_ASSERT(Cy_MPU_Setup(g_mpuCfg, MPU_CFG_ARRAY_SIZE(g_mpuCfg), CY_MPU_DISABLE_USE_DEFAULT_MAP, CY_MPU_DISABLED_DURING_FAULT_NMI) == CY_MPU_SUCCESS);

    // Read from peripheral register region which is restricted. this will cause hard-fault.
    *(volatile uint32_t*)(PERI_REGISTER_REGION_ADDR);

    for(;;);
}

static void UnAlignedRamAccess(cy_en_mpu_attr_t attr)
{
    static uint32_t    gRamAccessTest[2] = {0ul, 0ul}; // supposed to be located on System RAM region.
    volatile uint32_t* gpUnaligneRamAccessTest = (volatile uint32_t*)((uint32_t)&gRamAccessTest[0] + 2ul);

    g_mpuCfg[SRAM_MPU_NO].attribute = attr;
    CY_ASSERT(Cy_MPU_SetRegion(&g_mpuCfg[SRAM_MPU_NO], SRAM_MPU_NO) == CY_MPU_SUCCESS);
    *gpUnaligneRamAccessTest = 0xAAAAAAAAul;
    *gpUnaligneRamAccessTest;
}

static void UnAlignedAccessTest(void)
{
#if 0 // un-aligned access with strongly ordered memory will cause fault.
    UnAlignedRamAccess(CY_MPU_ATTR_STR_ORD_DEV);
#endif
#if 0 // un-aligned access with device attribute memory will cause fault.
    UnAlignedRamAccess(CY_MPU_ATTR_SHR_DEV);
#endif
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_WT);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_WT);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_WB);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_WB);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_NC);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_NC);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_WA);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_WA);
#if 0 // un-aligned access with device attribute memory will cause fault.
    UnAlignedRamAccess(CY_MPU_ATTR_DEV);
#endif
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_NC_OUT_NC);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_WA_OUT_NC);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_WT_OUT_NC);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_WB_OUT_NC);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_NC_OUT_NC);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_WA_OUT_NC);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_WT_OUT_NC);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_WB_OUT_NC);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_NC_OUT_WA);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_WA_OUT_WA);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_WT_OUT_WA);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_WB_OUT_WA);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_NC_OUT_WA);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_WA_OUT_WA);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_WT_OUT_WA);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_WB_OUT_WA);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_NC_OUT_WT);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_WA_OUT_WT);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_WT_OUT_WT);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_WB_OUT_WT);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_NC_OUT_WT);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_WA_OUT_WT);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_WT_OUT_WT);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_WB_OUT_WT);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_NC_OUT_WB);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_WA_OUT_WB);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_WT_OUT_WB);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_MEM_IN_WB_OUT_WB);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_NC_OUT_WB);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_WA_OUT_WB);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_WT_OUT_WB);
    UnAlignedRamAccess(CY_MPU_ATTR_NORM_SHR_MEM_IN_WB_OUT_WB);

    // Retrieve the original attribute setting.
    g_mpuCfg[SRAM_MPU_NO].attribute = CY_MPU_ATTR_NORM_MEM_WT;
    CY_ASSERT(Cy_MPU_SetRegion(&g_mpuCfg[SRAM_MPU_NO], SRAM_MPU_NO) == CY_MPU_SUCCESS);

}


/* [] END OF FILE */
