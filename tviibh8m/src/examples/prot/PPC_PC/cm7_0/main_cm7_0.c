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

#define MASTER_ID_TO_BE_CHECKED CPUSS_MS_ID_CM7_0

const cy_stc_ppu_gr_cfg_t  ppuFixedAttr_AllEnable =
{
    .userPermission = CY_PROT_PERM_RWX,
    .privPermission = CY_PROT_PERM_RWX,
    .secure         = TP_SECURE,
};

const cy_stc_ppu_gr_cfg_t  ppuFixedAttr_AllDisable =
{
    .userPermission = CY_PROT_PERM_DISABLED,
    .privPermission = CY_PROT_PERM_DISABLED,
    .secure         = TP_SECURE,
};

const cy_stc_gpio_pin_config_t gpioCfg =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = HSIOM_SEL_GPIO,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

int main(void)
{
    /* Enable global interrupts. */
    __enable_irq();
	
    SystemInit();

    cy_en_prot_status_t status;

    /********************************************/
    /* 1. Setting for GPIO 0 Register attribute */
    /********************************************/
    /* 1_1. Set permissions so that master whose PC is 5 can not access GPIO 0 */
    status = Cy_Prot_ConfigPpuFixedSlaveStruct(PERI_MS_PPU_FX_GPIO_PRT0_PRT, TP_PROHIBITED_PC, &ppuFixedAttr_AllDisable);
    CY_ASSERT(status == CY_PROT_SUCCESS);

    /* 1_2. Set permissions so that master whose PC is 6 can access GPIO 0 */
    status = Cy_Prot_ConfigPpuFixedSlaveStruct(PERI_MS_PPU_FX_GPIO_PRT0_PRT, TP_PERMITTED_PC, &ppuFixedAttr_AllEnable);
    CY_ASSERT(status == CY_PROT_SUCCESS); 

    /*********************************************/
    /* 2. Setting for MPUx (MPU for this Master) */
    /*********************************************/
    /* 2_1. Setting for MSx_CTL to allow the PC value to become "TP_PERMITTED_PC" or "TP_PROHIBITED_PC" */
    status = Cy_Prot_ConfigBusMaster(MASTER_ID_TO_BE_CHECKED, TP_PRIVILEGED, TP_SECURE, TP_PERMITTED_PC_MASK|TP_PROHIBITED_PC_MASK);
    CY_ASSERT(status == CY_PROT_SUCCESS);

    /*******************************/
    /* 3. Access with PC permitted */
    /*******************************/
    /* 3_1. Setting for MPU so that this CPU's PC value becomes "TP_PERMITTED_PC" */
    status = Cy_Prot_SetActivePC(MASTER_ID_TO_BE_CHECKED, TP_PERMITTED_PC);
    CY_ASSERT(status == CY_PROT_SUCCESS);

    /* 3_2. Read from GPIO 0 register */
    GPIO_PRT0->unOUT.u32Register;

    /********************************/
    /* 4. Access with PC prohibited */
    /********************************/
    /* 4_1. Setting for MPU so that this CPU's PC value becomes "TP_PROHIBITED_PC" */
    status = Cy_Prot_SetActivePC(MASTER_ID_TO_BE_CHECKED, TP_PROHIBITED_PC);
    CY_ASSERT(status == CY_PROT_SUCCESS);

    /* 4_2. Read from GPIO 0 register (this will cause hardfault) */
    GPIO_PRT0->unOUT.u32Register;

    for(;;);
}

/* [] END OF FILE */
