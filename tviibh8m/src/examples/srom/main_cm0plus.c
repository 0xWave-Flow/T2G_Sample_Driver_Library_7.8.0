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

struct
{
    uint32_t FamilyIdHigh;
    uint32_t FamilyIdLow;
    uint32_t MajorRevisionId;
    uint32_t MinorRevisionId;
    cy_en_life_cycle_state_t LifeCycleState;
    cy_en_protection_state_t ProtectionState;
    uint32_t SiliconIdHigh;
    uint32_t SiliconIdLow;
    uint32_t FlashBootMajorVersion;
    uint32_t FlashBootMinorVersion;
    uint32_t SROM_FW_MajorVersion;
    uint32_t SROM_FW_MinorVersion;
} SiIdReadValues = {0};

cy_stc_efuse_byte_map_t readEfuseByteMap = {0};

struct
{
    uint32_t DieId0;
    uint32_t DieId1;
} UnIdReadValues = {0};

int main(void)
{
    __enable_irq();
    
    SystemInit();

    un_srom_api_args_t  params = {0};
    un_srom_api_resps_t resp   = {0};

    // Reading Silicon ID TYPE 0
    // Note the silicon ID type 0 can be read only when debugging mode.
    // Otherwise, please read CPUSS_PRODUCT_ID register instead.
    {
        memset(&params, 0, sizeof(params));
        params.SiId.arg0.Opcode = CY_SROM_OP_SILICON_ID;
        params.SiId.arg0.IdType = 0;

        CY_ASSERT(Cy_Srom_CallApi(&params, &resp) == CY_SROM_DR_SUCCEEDED);

        SiIdReadValues.FamilyIdHigh = resp.SiId.resp0.IdType0.FamilyIdHigh;
        SiIdReadValues.FamilyIdLow  = resp.SiId.resp0.IdType0.FamilyIdLow;
        SiIdReadValues.MajorRevisionId = resp.SiId.resp0.IdType0.MajorRevisionId;
        SiIdReadValues.MinorRevisionId = resp.SiId.resp0.IdType0.MinorRevisionID;
    }

    // Reading Silicon ID TYPE 1
    {
        memset(&params, 0, sizeof(params));
        params.SiId.arg0.Opcode = CY_SROM_OP_SILICON_ID;
        params.SiId.arg0.IdType = 1;

        CY_ASSERT(Cy_Srom_CallApi(&params, &resp) == CY_SROM_DR_SUCCEEDED);

        SiIdReadValues.LifeCycleState  = (cy_en_life_cycle_state_t)resp.SiId.resp0.IdType1.LifeCycleState;
        SiIdReadValues.ProtectionState = (cy_en_protection_state_t)resp.SiId.resp0.IdType1.ProtectionState;
        SiIdReadValues.SiliconIdHigh   = resp.SiId.resp0.IdType1.SiliconIdHigh;
        SiIdReadValues.SiliconIdLow    = resp.SiId.resp0.IdType1.SiliconIdLow;
    }

    // Reading Silicon ID TYPE 2
    {
        memset(&params, 0, sizeof(params));
        params.SiId.arg0.Opcode = CY_SROM_OP_SILICON_ID;
        params.SiId.arg0.IdType = 2;

        CY_ASSERT(Cy_Srom_CallApi(&params, &resp) == CY_SROM_DR_SUCCEEDED);

        SiIdReadValues.FlashBootMajorVersion = resp.SiId.resp0.IdType2.FlashBootMajorVersion;
        SiIdReadValues.FlashBootMinorVersion = resp.SiId.resp0.IdType2.FlashBootMinorVersion;
        SiIdReadValues.SROM_FW_MajorVersion  = resp.SiId.resp0.IdType2.SROM_FW_MajorVersion;
        SiIdReadValues.SROM_FW_MinorVersion  = resp.SiId.resp0.IdType2.SROM_FW_MinorVersion;
    }

    // Reading Fuse Byte
    {
        for(uint32_t i = CY_SROM_EFUSE_RESERVED_NUM; i < sizeof(cy_stc_efuse_byte_map_t); i++)
        {
            memset(&params, 0, sizeof(params));
            params.RdFuse.arg0.Opcode = CY_SROM_OP_READ_FUSE_BYTE;
            params.RdFuse.arg0.eFuseAddr = i;
            CY_ASSERT(Cy_Srom_CallApi(&params, &resp) == CY_SROM_DR_SUCCEEDED);
            ((uint8_t*)&readEfuseByteMap)[i] = (uint8_t)resp.RdFuse.resp0.ReadByte;
        }
    }

    // Reading Unique ID (Zero was read)
    {
        params.RdUnId.arg0.Opcode = CY_SROM_OP_READ_UNIQUE_ID;
        CY_ASSERT(Cy_Srom_CallApi(&params, &resp) == CY_SROM_DR_SUCCEEDED);
        UnIdReadValues.DieId0 = resp.RdUnId.resp1.DIE_ID0;
        UnIdReadValues.DieId1 = resp.RdUnId.resp2.DIE_ID1;
    }

    for(;;);
}


/* [] END OF FILE */
