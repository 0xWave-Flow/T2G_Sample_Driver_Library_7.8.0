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
#include "tvii_series_smif_ex_adopter.h"

/***** User Modifiable definitions *****/
#if (CY_SMIF_QUAD_MEMORY == CY_SMIF_S25FXXXS)
  #define SMIF_CLK_FREQ_SETUP      (32000000UL) // 32MHz
  #define SMIF_CLK_FREQ_SDR_RD     (32000000UL) // 32MHz
#elif (CY_SMIF_QUAD_MEMORY == CY_SMIF_S25FXXXL)
  #define SMIF_CLK_FREQ_SETUP      (90000000UL) // 90MHz
  #define SMIF_CLK_FREQ_SDR_RD     (90000000UL) // 90MHz
#endif
/**************************************/

#define DEVICE0_XIP_ADDRESS           (0x60000000UL)
#define DEVICE1_XIP_ADDRESS           (0x62000000UL)

#define S25FLXXXS_CMD_4QOR            (0x6C) // Read Quad Out (4-byte address)
#define S25FLXXXS_CMD_4QPP            (0x34) // Page Program (4-byte address) 
#define S25FLXXXS_CMD_WREN            (0x06) // Write Enable 
#define S25FLXXXS_CMD_WRDI            (0x04) // Write Disable 
#define S25FLXXXS_CMD_4SE             (0xDC) // Erase 64 kB or 256 kB (4-byte address) 
#define S25FLXXXS_CMD_BE              (0x60) // Bulk Erase 
#define S25FLXXXS_CMD_RDSR1           (0x05) // Read Status Register-1 
#define S25FLXXXS_CMD_RDCR            (0x35) // Read Configuration Register-1 
#define S25FLXXXS_CMD_WRR             (0x01) // Write Register (Status-1, Configuration-1) 
#define ProgramSizeS25FLXXXS_4PP      (512)
#define StsRegBusyMaskS25FLXXXS       (0x00000001UL)
#define StsRegQuadEnableMaskS25FLXXXS (0x00000200UL)
#define StsRegProtectionMaskS25FLXXXS (0x0000001CUL)

#define S25FLXXXL_CMD_4QOR            (0x6C) // Read Quad Out (4-byte address)
#define S25FLXXXL_CMD_4QPP            (0x34) // Page Program (4-byte address) 
#define S25FLXXXL_CMD_WREN            (0x06) // Write Enable 
#define S25FLXXXL_CMD_WRDI            (0x04) // Write Disable 
#define S25FLXXXL_CMD_4BE             (0xDC) // Erase 64 kB or 256 kB (4-byte address) 
#define S25FLXXXL_CMD_CE              (0x60) // chip Erase 
#define S25FLXXXL_CMD_RDSR1           (0x05) // Read Status Register-1 
#define S25FLXXXL_CMD_RDCR            (0x35) // Read Configuration Register-1 
#define S25FLXXXL_CMD_WRR             (0x01) // Write Register (Status-1, Configuration-1) 
#define ProgramSizeS25FLXXXL_4PP      (512)
#define StsRegBusyMaskS25FLXXXL       (0x00000001UL)
#define StsRegQuadEnableMaskS25FLXXXL (0x00000200UL)
#define StsRegProtectionMaskS25FLXXXL (0x00003C00UL)

#if (CY_SMIF_QUAD_MEMORY == CY_SMIF_S25FXXXS)
  #define S25FLXXX_CMD_4QOR            S25FLXXXS_CMD_4QOR
  #define S25FLXXX_CMD_4QPP            S25FLXXXS_CMD_4QPP
  #define S25FLXXX_CMD_WREN            S25FLXXXS_CMD_WREN
  #define S25FLXXX_CMD_WRDI            S25FLXXXS_CMD_WRDI
  #define S25FLXXX_CMD_SECTERASE       S25FLXXXS_CMD_4SE
  #define S25FLXXX_CMD_CHIPERASE       S25FLXXXS_CMD_BE
  #define S25FLXXX_CMD_RDSR1           S25FLXXXS_CMD_RDSR1
  #define S25FLXXX_CMD_RDCR            S25FLXXXS_CMD_RDCR
  #define S25FLXXX_CMD_WRR             S25FLXXXS_CMD_WRR
  #define ProgramSizeS25FLXXX_4PP      ProgramSizeS25FLXXXS_4PP
  #define StsRegBusyMaskS25FLXXX       StsRegBusyMaskS25FLXXXS
  #define StsRegQuadEnableMaskS25FLXXX StsRegQuadEnableMaskS25FLXXXS
  #define StsRegProtectionMaskS25FLXXX StsRegProtectionMaskS25FLXXXS
#elif (CY_SMIF_QUAD_MEMORY == CY_SMIF_S25FXXXL)
  #define S25FLXXX_CMD_4QOR            S25FLXXXL_CMD_4QOR
  #define S25FLXXX_CMD_4QPP            S25FLXXXL_CMD_4QPP
  #define S25FLXXX_CMD_WREN            S25FLXXXL_CMD_WREN
  #define S25FLXXX_CMD_WRDI            S25FLXXXL_CMD_WRDI
  #define S25FLXXX_CMD_SECTERASE       S25FLXXXL_CMD_4BE
  #define S25FLXXX_CMD_CHIPERASE       S25FLXXXL_CMD_CE
  #define S25FLXXX_CMD_RDSR1           S25FLXXXL_CMD_RDSR1
  #define S25FLXXX_CMD_RDCR            S25FLXXXL_CMD_RDCR
  #define S25FLXXX_CMD_WRR             S25FLXXXL_CMD_WRR
  #define ProgramSizeS25FLXXX_4PP      ProgramSizeS25FLXXXL_4PP
  #define StsRegBusyMaskS25FLXXX       StsRegBusyMaskS25FLXXXL
  #define StsRegQuadEnableMaskS25FLXXX StsRegQuadEnableMaskS25FLXXXL
  #define StsRegProtectionMaskS25FLXXX StsRegProtectionMaskS25FLXXXL
#endif

// #define DLP_SETTING_UPDATE_AUTO

#ifdef DLP_SETTING_UPDATE_AUTO
  #define SETTING_DELAY_SEL CY_SMIF_1_NEW_SEL_PER_TAP
  #define SETTING_DLP_AUTO  CY_SMIF_DLP_UPDATE_AUTO
#else
  #define SETTING_DELAY_SEL CY_SMIF_NO_DELAY_SEL
  #define SETTING_DLP_AUTO  CY_SMIF_DLP_UPDATE_MANUAL
#endif

cy_stc_smif_config_t const smifConfig =
{
    .txClk         = CY_SMIF_INV_FOR_DDR,
    .rxClk         = CY_SMIF_INV_OUTPUT_CLK, // Note
    .delaySel      = SETTING_DELAY_SEL,
    .dlpAuto       = SETTING_DLP_AUTO,
    .capDelay      = CY_SMIF_CAPTURE_DELAY_0_CYCLE,
    .deselectDelay = CY_SMIF_MIN_DESELECT_1_CLK,
    .setupDelay    = CY_SMIF_SETUP_0_CLK_PULUS_MIN,
    .holdDelay     = CY_SMIF_HOLD_0_CLK_PULUS_MIN,
    .mode          = CY_SMIF_NORMAL, // MMIO mode
    .blockEvent    = CY_SMIF_BUS_ERROR,
};

cy_stc_smif_mem_rd_cmd_t readCmd_S25FL256S_Quad =
{
    .cmdCtl =
    {
        .cmd      = S25FLXXX_CMD_4QOR,
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
        .presence = CY_SMIF_PRESENT_1BYTE,
    },
    .addrCtl =
    {
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
    },
    .modeCtl =
    {
        .cmd      = 0,
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
        .presence = CY_SMIF_NOT_PRESENT,
    },
    .memDummyCtl =
    {
        .dummyCtl =
        {
            .numOfDummy = 8u,
            .presence   = CY_SMIF_PRESENT_1BYTE,
        },
        .doubleDm = false,
        .rwdsInDm = false,
        .dlpEn    = false,
        .capEn    = false,
    },
    .dataCtl =
    {
        .width    = CY_SMIF_WIDTH_QUAD,
        .dataRate = CY_SMIF_SDR,
    },

    /* for future use */
//    .crcCtl =
//    .boundCtl =
};


cy_stc_smif_mem_wr_cmd_t programCmd_S25FL256S_Quad =
{
    .cmdCtl =
    {
        .cmd      = S25FLXXX_CMD_4QPP,
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
        .presence = CY_SMIF_PRESENT_1BYTE,
    },
    .addrCtl =
    {
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
    },
    .modeCtl =
    {
        .cmd      = 0,
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
        .presence = CY_SMIF_NOT_PRESENT,
    },
    .memDummyCtl =
    {
        .dummyCtl =
        {
            .numOfDummy = 0,
            .presence   = CY_SMIF_NOT_PRESENT,
        },
        .doubleDm = false,
        .rwdsInDm = false,
        .dlpEn    = false,
        .capEn    = false,
    },
    .dataCtl = 
    {
        .width    = CY_SMIF_WIDTH_QUAD,
        .dataRate = CY_SMIF_SDR,
    },
    /* for future use */
//    .crcCtl =
};

cy_stc_smif_mem_cmd_t writeEnCmd_S25FL256S =
{
    .cmdCtl =
    {
        .cmd      = S25FLXXX_CMD_WREN,
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
        .presence = CY_SMIF_PRESENT_1BYTE,
    },
};

cy_stc_smif_mem_cmd_t writeDisCmd_S25FL256S =
{
    .cmdCtl =
    {
        .cmd      = S25FLXXX_CMD_WRDI,
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
        .presence = CY_SMIF_PRESENT_1BYTE,
    },
};

cy_stc_smif_mem_cmd_t eraseCmd_S25FL256S =
{
    .cmdCtl =
    {
        .cmd      = S25FLXXX_CMD_SECTERASE,
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
        .presence = CY_SMIF_PRESENT_1BYTE,
    },
};

cy_stc_smif_mem_cmd_t chipEraseCmd_S25FL256S =
{
    .cmdCtl =
    {
        .cmd      = S25FLXXX_CMD_CHIPERASE,
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
        .presence = CY_SMIF_PRESENT_1BYTE,
    },
};

cy_stc_smif_mem_cmd_t readStsRegWipCmd_S25FL256S =
{
    .cmdCtl =
    {
        .cmd      = S25FLXXX_CMD_RDSR1,
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
        .presence = CY_SMIF_PRESENT_1BYTE,
    },
};

cy_stc_smif_mem_cmd_t readStsRegQeCmd_S25FL256S =
{
    .cmdCtl =
    {
        .cmd      = S25FLXXX_CMD_RDCR,
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
        .presence = CY_SMIF_PRESENT_1BYTE,
    },
};

cy_stc_smif_mem_cmd_t writeStsRegQeCmd_S25FL256S =
{
    .cmdCtl =
    {
        .cmd      = S25FLXXX_CMD_WRR,
        .width    = CY_SMIF_WIDTH_SINGLE,
        .dataRate = CY_SMIF_SDR,
        .presence = CY_SMIF_PRESENT_1BYTE,
    },
};

cy_stc_smif_mem_device_cfg_t deviceCfg =
{
    .isCommand2Byte       = false,
    .numOfAddrBytes       = CY_SMIF_XIP_ADDRESS_4_BYTE,
    .readCmd              = &readCmd_S25FL256S_Quad,
    .writeEnCmd           = &writeEnCmd_S25FL256S,
    .writeDisCmd          = &writeDisCmd_S25FL256S,
    .eraseCmd             = &eraseCmd_S25FL256S,
    .chipEraseCmd         = &chipEraseCmd_S25FL256S,
    .programCmd           = &programCmd_S25FL256S_Quad,
    .programSize          = ProgramSizeS25FLXXX_4PP,
    .readStsRegWipCmd     = &readStsRegWipCmd_S25FL256S,
    .readStsRegQeCmd      = &readStsRegQeCmd_S25FL256S,
    .writeStsRegQeCmd     = &writeStsRegQeCmd_S25FL256S,
    .stsRegBusyMask       = StsRegBusyMaskS25FLXXX,
    .stsRegQuadEnableMask = StsRegQuadEnableMaskS25FLXXX,
    .stsRegProtectionMask = StsRegProtectionMaskS25FLXXX,
};


cy_stc_smif_mem_config_t memConfig_Device0 = 
{
    .slaveSelect   = CY_SMIF_SLAVE_SELECT_0,
    .writeEn       = true,
    .crypteEn      = false,
    .XIPSettingEn  = true,
    .dataSelect    = CY_SMIF_DATA_SEL0,
    .baseAddress   = 0,
    .memMappedSize = CY_SMIF_DEVICE_32M_BYTE,
    .dualQuadSlots = 0,
    .deviceCfg     = &deviceCfg,
};

cy_stc_smif_mem_config_t memConfig_Device1 = 
{
    .slaveSelect   = CY_SMIF_SLAVE_SELECT_1,
    .writeEn       = true,
    .crypteEn      = false,
    .XIPSettingEn  = true,
    .dataSelect    = CY_SMIF_DATA_SEL2,
    .baseAddress   = DEVICE1_XIP_ADDRESS,
    .memMappedSize = CY_SMIF_DEVICE_32M_BYTE,
    .dualQuadSlots = 0,
    .deviceCfg     = &deviceCfg,
};

cy_stc_smif_mem_config_t* memConfig[2] = { &memConfig_Device0, &memConfig_Device1, };

cy_stc_smif_block_config_t const blockConfig =
{
    .memCount = 2,          /**< The number of SMIF memory defined  */
    .memConfig = memConfig, /**< The pointer to the array of the memory configuration structures of size Memory_count */
    .majorVersion = 0,      /**< The version of the SMIF driver */
    .minorVersion = 1,      /**< The version of the SMIF driver */
};

cy_stc_sysint_irq_t smif_irq_cfg =
{
    .sysIntSrc  = SMIF_IRQ_NO,
    .intIdx     = CPUIntIdx3_IRQn,
    .isEnabled  = true,
};

typedef struct
{
    volatile stc_GPIO_PRT_t*  port;
    uint8_t                   pin;
    en_hsiom_sel_t            hsiom;
    uint32_t                  driveMode;
} cy_stc_smif_port_t;

cy_stc_smif_port_t smifPortCfg[] =
{
    {CY_SMIF_CLK_PORT,         CY_SMIF_CLK_PIN,       CY_SMIF_CLK_PIN_MUX,     CY_GPIO_DM_STRONG},
    {CY_SMIF_CLK_INV_PORT,     CY_SMIF_CLK_INV_PIN,   HSIOM_SEL_GPIO,          CY_GPIO_DM_STRONG_IN_OFF},
    {CY_SMIF_RWDS_PORT,        CY_SMIF_RWDS_PIN,      CY_SMIF_RWDS_PIN_MUX,    CY_GPIO_DM_PULLDOWN},
    {CY_SMIF_SELECT0_PORT,     CY_SMIF_SELECT0_PIN,   CY_SMIF_SELECT0_PIN_MUX, CY_GPIO_DM_PULLUP},
    {CY_SMIF_SELECT1_PORT,     CY_SMIF_SELECT1_PIN,   CY_SMIF_SELECT1_PIN_MUX, CY_GPIO_DM_PULLUP},
    {CY_SMIF_DATA0_PORT,       CY_SMIF_DATA0_PIN,     CY_SMIF_DATA0_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA1_PORT,       CY_SMIF_DATA1_PIN,     CY_SMIF_DATA1_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA2_PORT,       CY_SMIF_DATA2_PIN,     CY_SMIF_DATA2_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA3_PORT,       CY_SMIF_DATA3_PIN,     CY_SMIF_DATA3_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA4_PORT,       CY_SMIF_DATA4_PIN,     CY_SMIF_DATA4_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA5_PORT,       CY_SMIF_DATA5_PIN,     CY_SMIF_DATA5_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA6_PORT,       CY_SMIF_DATA6_PIN,     CY_SMIF_DATA6_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA7_PORT,       CY_SMIF_DATA7_PIN,     CY_SMIF_DATA7_PIN_MUX,   CY_GPIO_DM_STRONG},
};
#define SIZE_OF_PORT (sizeof(smifPortCfg)/sizeof(cy_stc_smif_port_t))

static void SmifPortInit(cy_stc_smif_port_t cfg[], uint8_t size)
{
    cy_stc_gpio_pin_config_t pinCfg = {0};
    for(uint32_t i = 0; i < size; i++)
    {
        pinCfg.driveMode = cfg[i].driveMode;
        pinCfg.hsiom     = cfg[i].hsiom;
        Cy_GPIO_Pin_Init(cfg[i].port, cfg[i].pin, &pinCfg);
    }
}

cy_stc_smif_context_t smifContext;

void SMIF_UserInterruptHandler(void)
{
    Cy_SMIF_Interrupt(SMIF_USED, &smifContext);
}

static void MMIO_ERASE_READ_WRITE_TEST_FOR_DEVICE0(void);
static void XIP_READ_WRITE_TEST_FOR_DEVICE1(void);
static void SetModeWithBusyCheck(volatile cy_stc_smif_reg_t *base, cy_en_smif_mode_t mode);

int main(void)
{
    SystemInit();

    __enable_irq();

    Cy_SysClk_HfClkEnable(SMIF_HF_CLOCK);

    // Please modify according to your HW condition.
    ChangePLLFrequency(SMIF_CLK_FREQ_SETUP);

    SmifPortInit(smifPortCfg, SIZE_OF_PORT);

    memConfig_Device0.baseAddress = CY_SMIF_GetXIP_Address(SMIF_USED);
    memConfig_Device1.baseAddress = CY_SMIF_GetXIP_Address(SMIF_USED) + 0x02000000;

    /* Interrupt settings */
    Cy_SysInt_InitIRQ(&smif_irq_cfg);
    Cy_SysInt_SetSystemIrqVector(smif_irq_cfg.sysIntSrc, SMIF_UserInterruptHandler);
    NVIC_SetPriority(smif_irq_cfg.intIdx, 0);
    NVIC_EnableIRQ(smif_irq_cfg.intIdx);

    /*************************/
    /* 1. Setting for SMIF 0 */
    /*************************/
    Cy_SMIF_DeInit(SMIF_USED);
    Cy_SMIF_Init(SMIF_USED, &smifConfig, 1000, &smifContext);
    Cy_SMIF_SetTxFifoTriggerLevel(SMIF_USED, 7u);
    Cy_SMIF_SetRxFifoTriggerLevel(SMIF_USED, 0u);

    /**************************/
    /* 2. Setting for Devices */
    /**************************/
    Cy_SMIF_Memslot_Init(SMIF_USED, &blockConfig, &smifContext);

    /********************/
    /* 3. Enable SMIF 0 */
    /********************/
    Cy_SMIF_Enable(SMIF_USED, &smifContext);

    /********************/
    /* 4. Quad Enable  */
    /********************/
    {
        /* Quad Enable for deviec 0 */
        Cy_SMIF_Memslot_QuadEnable(SMIF_USED,
                                   &memConfig_Device0,
                                   &smifContext);
        /* Wait for the device becoming ready */
        while(Cy_SMIF_Memslot_IsBusy(SMIF_USED, &memConfig_Device0, &smifContext));

        /* Quad Enable for deviec 1 */
        Cy_SMIF_Memslot_QuadEnable(SMIF_USED,
                                   &memConfig_Device1,
                                   &smifContext);
        /* Wait for the device becoming ready */
        while(Cy_SMIF_Memslot_IsBusy(SMIF_USED, &memConfig_Device1, &smifContext));
    }

    /**************************/
    /* 5. Release Protection  */
    /**************************/
    {
        /* None protection deviec 0 */
        Cy_SMIF_Memslot_BlockProtNone(SMIF_USED,
                                      &memConfig_Device0,
                                      &smifContext);
        /* Wait for the device becoming ready */
        while(Cy_SMIF_Memslot_IsBusy(SMIF_USED, &memConfig_Device0, &smifContext));

        /* None protection deviec 1 */
        Cy_SMIF_Memslot_BlockProtNone(SMIF_USED,
                                      &memConfig_Device1,
                                      &smifContext);
        /* Wait for the device becoming ready */
        while(Cy_SMIF_Memslot_IsBusy(SMIF_USED, &memConfig_Device1, &smifContext));
    }

    ChangePLLFrequency(SMIF_CLK_FREQ_SDR_RD);

    MMIO_ERASE_READ_WRITE_TEST_FOR_DEVICE0();

    XIP_READ_WRITE_TEST_FOR_DEVICE1();

    for(;;);
}
void MMIO_ERASE_READ_WRITE_TEST_FOR_DEVICE0(void)
{
    /* 1. bulk erase then check if read value is 0xFFFFFFFF */
    {
        /* Issue write enable */
        Cy_SMIF_Memslot_CmdWriteEnable(SMIF_USED,
                                   &memConfig_Device0,
                                   &smifContext);

        /* Issue Bulk erase */
        Cy_SMIF_Memslot_CmdChipErase(SMIF_USED,
                                     &memConfig_Device0,
                                     &smifContext);
        /* Wait for the erasing completes !! It may take several minutes */
        while(Cy_SMIF_Memslot_IsBusy(SMIF_USED, &memConfig_Device0, &smifContext));

        /* Then read data from certain address */
        uint8_t readData[4] = { 0u };
        Cy_SMIF_Memslot_CmdRead(SMIF_USED, 
                                &memConfig_Device0,
                                0x00123400UL,
                                readData,
                                4,
                                NULL,
                                &smifContext);
        // Wait for the recieving completes.
        while(Cy_SMIF_GetTxfrStatus(SMIF_USED, &smifContext) == CY_SMIF_REC_BUSY);
        
        /* Checking the read data has been erased */
        CY_ASSERT(*(uint32_t*)readData == 0xFFFFFFFF);
    }

    /* 2. Write then read test */
    {
        /* Issue write enable */
        Cy_SMIF_Memslot_CmdWriteEnable(SMIF_USED,
                                       &memConfig_Device0,
                                       &smifContext);

        /* Write data into certain address */
        uint8_t writeData[4];
        *(uint32_t*)writeData = 0xA5B6C7D8;
        Cy_SMIF_Memslot_CmdProgram(SMIF_USED,
                                   &memConfig_Device0,
                                   0x00123400UL,
                                   writeData,
                                   4u,
                                   NULL,
                                   &smifContext);

        /* Wait for the sending completes */
        while(Cy_SMIF_GetTxfrStatus(SMIF_USED, &smifContext) == CY_SMIF_SEND_BUSY);

        /* Wait for the device becoming ready */
        while(Cy_SMIF_Memslot_IsBusy(SMIF_USED, &memConfig_Device0, &smifContext));

        /* Read from the address */
        uint8_t readData[4] = { 0u };
        Cy_SMIF_Memslot_CmdRead(SMIF_USED, 
                                &memConfig_Device0,
                                0x00123400UL,
                                readData,
                                4,
                                NULL,
                                &smifContext);
        /* Wait for the recieving completes */
        while(Cy_SMIF_GetTxfrStatus(SMIF_USED, &smifContext) == CY_SMIF_REC_BUSY);

        /* Check if the read value and write value are same */
        CY_ASSERT(*(uint32_t*)writeData == *(uint32_t*)readData);
    }

    /* 3. Sector erase then check if the read data is 0xFFFFFFFF */
    {
        /* Issue write enable */
        Cy_SMIF_Memslot_CmdWriteEnable(SMIF_USED,
                                       &memConfig_Device0,
                                       &smifContext);

        /* Issue sector erase */
        Cy_SMIF_Memslot_CmdSectorErase(SMIF_USED,
                                       &memConfig_Device0,
                                       0x00120000UL,       // from 0x0012_0000 to 0x0013_0000
                                       &smifContext);

        /* Wait for the device becoming ready */
        while(Cy_SMIF_Memslot_IsBusy(SMIF_USED, &memConfig_Device0, &smifContext));

        /* Read from the area which has been erased supposedly */
        uint8_t readData[4] = { 0u };
        Cy_SMIF_Memslot_CmdRead(SMIF_USED, 
                                &memConfig_Device0,
                                0x00123400UL,
                                readData,
                                4,
                                NULL,
                                &smifContext);
        /* Wait for the recieving completes */
        while(Cy_SMIF_GetTxfrStatus(SMIF_USED, &smifContext) == CY_SMIF_REC_BUSY);

        /* Check if the read data is 0xFFFFFFFF */
        CY_ASSERT(*(uint32_t*)readData == 0xFFFFFFFF);
    }
}

void XIP_READ_WRITE_TEST_FOR_DEVICE1(void)
{
    /* Issue Erase sector in MMIO mode */
    Cy_SMIF_Memslot_CmdSectorErase(SMIF_USED,
                                   &memConfig_Device1,
                                   0x00000000UL,       // from 0x0000_0000 to 0x0004_0000
                                   &smifContext);
    /* Wait for the device becoming ready */
    while(Cy_SMIF_Memslot_IsBusy(SMIF_USED, &memConfig_Device1, &smifContext));

    /* Issue write enable in MMIO mode */
    Cy_SMIF_Memslot_CmdWriteEnable(SMIF_USED,
                                   &memConfig_Device1,
                                   &smifContext);

    /* Change the SMIF device into XIP mode */
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_MEMORY);

    /* Write in XIP mode */
    uint32_t writeData = 0x12345678ul;
    *(uint32_t*)(DEVICE1_XIP_ADDRESS) = writeData;

    /* Change the SMIF device into MMIO mode */
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);

    /* Wait for the device becoming ready */
    while(Cy_SMIF_Memslot_IsBusy(SMIF_USED, &memConfig_Device1, &smifContext));

    /* Change the SMIF device into XIP mode */
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_MEMORY);

    /* Read in XIP mode */
    uint32_t readData = 0x0ul;
    readData = *(uint32_t*)(DEVICE1_XIP_ADDRESS);

    /* Check if the read and write data are same */
    CY_ASSERT(writeData == readData);
}

static void SetModeWithBusyCheck(volatile cy_stc_smif_reg_t *base, cy_en_smif_mode_t mode)
{
    cy_en_smif_status_t status;
    do
    {
        status = Cy_SMIF_SetMode(base, mode);
    }while(status != CY_SMIF_SUCCESS);
}
/* [] END OF FILE */
