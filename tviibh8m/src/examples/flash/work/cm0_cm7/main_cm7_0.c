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
#include "flash_ex_rand_data.h"

/* Select flash address to be tested.                                */
/* By default,top large sector and top small sector will be tested */
#define TEST_W_LS_ADDR    (CY_WFLASH_LG_SBM_TOP)
#define TEST_W_SS_ADDR    (CY_WFLASH_SM_SBM_TOP)

void BasicWorkFlashBlockingTest(uint32_t SectorAddr, uint32_t SectorSizeInByte);

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    SystemInit();

    SCB_DisableDCache(); // Disables. Because cores share SRAM pointed by SROM scratch address

    // Wait 3 [s] to avoid breaking debugger connection.
    Cy_SysTick_DelayInUs(3000000);

    // Initialization 
    Cy_FlashInit(false /*blocking*/);

    // Test for Large Sector
    BasicWorkFlashBlockingTest(TEST_W_LS_ADDR, CY_WORK_LES_SIZE_IN_BYTE);

    // Test for Small Sector
    BasicWorkFlashBlockingTest(TEST_W_SS_ADDR, CY_WORK_SES_SIZE_IN_BYTE);

    while(1); // success erase program test
}


// In this routine, whole one sector erased/written/read/verified
void BasicWorkFlashBlockingTest(uint32_t SectorAddr, uint32_t SectorSizeInByte)
{
    uint32_t* p_TestFlsTop = (uint32_t*)SectorAddr;

    uint32_t SectorSizeInWord = SectorSizeInByte / 4ul;

    /** Erasing **/
    // Erase
    Cy_FlashSectorErase(SectorAddr, CY_FLASH_DRIVER_BLOCKING);

    // Verify
    Cy_WorkFlashBlankCheck(SectorAddr, CY_FLASH_DRIVER_BLOCKING);

    /** Programming **/
    for(uint32_t i_addr = SectorAddr, i_addrOffset = 0; i_addr < SectorAddr + SectorSizeInByte; i_addr+=4, i_addrOffset+=4)
    {
        uint32_t i_dataPos = i_addrOffset % PROGRAM_DATA_SIZE_IN_BYTE;

        // Flash 
        Cy_FlashWriteWork(i_addr, (uint32_t*)&programData[i_dataPos], CY_FLASH_DRIVER_BLOCKING);
    }

    // Verify 
    uint32_t* pProgramData = (uint32_t*)programData;
    for(uint32_t i_wordId = 0; i_wordId < SectorSizeInWord; i_wordId++)
    {
        uint32_t i_dataPos = i_wordId % PROGRAM_DATA_SIZE_IN_WORD;
        CY_ASSERT(p_TestFlsTop[i_wordId] == pProgramData[i_dataPos]);
    }

    /** Erasing Again **/
    // Erase
    Cy_FlashSectorErase(SectorAddr, CY_FLASH_DRIVER_BLOCKING);

    // Verify
    Cy_WorkFlashBlankCheck(SectorAddr, CY_FLASH_DRIVER_BLOCKING);
}


/* [] END OF FILE */
