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

#define BACKGROUND_REGION_ADDR       (0x00000000ul) // Back Ground Region Start Address
#define CODE_FLASH_REGION_ADDR       (0x10000000ul) // Code Flash Region Start Address
#define WORK_FLASH_REGION_ADDR       (0x14000000ul) // Work Flash Region Start Address
#define SRAM_REGION_ADDR             (0x28000000ul) // System RAM Region Start Address
#define PERI_REGISTER_REGION_ADDR    (0x40000000ul) // Peripheral Register Region Start Address
#define ARM_SYS_REGISTER_REGION_ADDR (0xE0000000ul) // ARM System Registers Region Start Address

#define BACKGROUND_MPU_NO       (0)
#define CODE_FLASH_MPU_NO       (1)
#define WORK_FLASH_MPU_NO       (2)
#define SRAM_MPU_NO             (3)
#define PERI_REGISTER_MPU_NO    (4)
#define ARM_SYS_REGISTER_MPU_NO (5)


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

int main(void)
{
    SystemInit();

    __enable_irq();

    /********* Core MPU setting **************/
    CY_ASSERT(Cy_MPU_Setup(g_mpuCfg, MPU_CFG_ARRAY_SIZE(g_mpuCfg), CY_MPU_DISABLE_USE_DEFAULT_MAP, CY_MPU_DISABLED_DURING_FAULT_NMI) == CY_MPU_SUCCESS);

    // Read from peripheral register region which is not restricted.
    *(volatile uint32_t*)(PERI_REGISTER_REGION_ADDR);

    /********* Core MPU setting **************/
    g_mpuCfg[PERI_REGISTER_MPU_NO].permission = CY_MPU_ACCESS_P_NO_ACCESS;
    CY_ASSERT(Cy_MPU_Setup(g_mpuCfg, MPU_CFG_ARRAY_SIZE(g_mpuCfg), CY_MPU_DISABLE_USE_DEFAULT_MAP, CY_MPU_DISABLED_DURING_FAULT_NMI) == CY_MPU_SUCCESS);

    // Read from peripheral register region which is restricted. this will cause hard-fault.
    *(volatile uint32_t*)(PERI_REGISTER_REGION_ADDR);

    for(;;);
}



/* [] END OF FILE */
