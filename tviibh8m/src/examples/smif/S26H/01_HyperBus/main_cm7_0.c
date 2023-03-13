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
#include "SMIF_TestData.h"
#include "tvii_series_smif_ex_adopter.h"

/************ Modifiable definitions **********/
#define TEST_SECTOR_NO (0)            // modifiable 0 ~ 255
// !!!!!!! Note the sector defined by below will be erased during delay line calibration !!!!!!!!!
#define SECTOR_NO_USED_BY_CALIB (255) // modifiable 0 ~ 255
#define SMIF_DEVICE_USED SMIF_DEVICE0
#define TARGET_CLK_FREQ_IN_MHZ (200)
/**********************************************/


#define HB_WRITE_BUF_SIZE_WORD (256)      // 256 word = 512 byte
#define HB_SECTOR_SIZE         (0x00040000UL) // 256KB
#define TEST_SECTOR_ADDRESS    (HB_SECTOR_SIZE * TEST_SECTOR_NO)
#define SECTOR_ADDR_USED_BY_CALIB (HB_SECTOR_SIZE * SECTOR_NO_USED_BY_CALIB)

cy_stc_smif_config_t const smifConfig =
{
    .txClk         = CY_SMIF_INV_FOR_DDR,
    .rxClk         = CY_SMIF_INV_RWDS,
    .dlpAuto       = CY_SMIF_DLP_UPDATE_MANUAL,
    .capDelay      = CY_SMIF_CAPTURE_DELAY_0_CYCLE,
    .delaySel      = CY_SMIF_1_SEN_SEL_PER_TAP,
    .deselectDelay = CY_SMIF_MIN_DESELECT_1_CLK,
    .setupDelay    = CY_SMIF_SETUP_3_CLK_PULUS_MIN,
    .holdDelay     = CY_SMIF_HOLD_3_CLK_PULUS_MIN,
    .mode          = CY_SMIF_MEMORY, // XIP mode
    .blockEvent    = CY_SMIF_BUS_ERROR,
};
cy_stc_smif_context_t smifContext;

#define HB_FLASH_TARGET_LC  CY_SMIF_HB_LC20

cy_stc_device_hb_config_t smifDevHBFlashCfg =
{
    .xipReadCmd         = CY_SMIF_HB_READ_CONTINUOUS_BURST,
    .xipWriteCmd        = CY_SMIF_HB_WRITE_CONTINUOUS_BURST,
    .mergeEnable        = false,                  // will be updated in the application
    .mergeTimeout       = CY_SMIF_MER_TIMEOUT_1_CYCLE,
    .totalTimeoutEnable = false,
    .totalTimeout       = 50,
    .addr               = 0,                       // will be updated in the application
    .size               = CY_SMIF_DEVICE_64M_BYTE,
    .lc_hb              = HB_FLASH_TARGET_LC,
    .hbDevType          = CY_SMIF_HB_FLASH,
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
    {CY_SMIF_CLK_PORT, CY_SMIF_CLK_PIN, CY_SMIF_CLK_PIN_MUX,     CY_GPIO_DM_STRONG},
    {CY_SMIF_CLK_INV_PORT,     CY_SMIF_CLK_INV_PIN,   HSIOM_SEL_GPIO,          CY_GPIO_DM_STRONG_IN_OFF},
    {CY_SMIF_RWDS_PORT, CY_SMIF_RWDS_PIN, CY_SMIF_RWDS_PIN_MUX,    CY_GPIO_DM_PULLDOWN},
    {CY_SMIF_SELECT0_PORT, CY_SMIF_SELECT0_PIN, CY_SMIF_SELECT0_PIN_MUX, CY_GPIO_DM_STRONG},
    {CY_SMIF_SELECT1_PORT,  CY_SMIF_SELECT1_PIN, CY_SMIF_SELECT1_PIN_MUX,  CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA0_PORT, CY_SMIF_DATA0_PIN, CY_SMIF_DATA0_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA1_PORT, CY_SMIF_DATA1_PIN, CY_SMIF_DATA1_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA2_PORT, CY_SMIF_DATA2_PIN, CY_SMIF_DATA2_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA3_PORT, CY_SMIF_DATA3_PIN, CY_SMIF_DATA3_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA4_PORT, CY_SMIF_DATA4_PIN, CY_SMIF_DATA4_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA5_PORT, CY_SMIF_DATA5_PIN, CY_SMIF_DATA5_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA6_PORT, CY_SMIF_DATA6_PIN, CY_SMIF_DATA6_PIN_MUX,   CY_GPIO_DM_STRONG},
    {CY_SMIF_DATA7_PORT, CY_SMIF_DATA7_PIN, CY_SMIF_DATA7_PIN_MUX,   CY_GPIO_DM_STRONG},
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

static void SetModeWithBusyCheck(volatile cy_stc_smif_reg_t *base, cy_en_smif_mode_t mode);
uint16_t HyperFlashReadStatusOpInMMIO(cy_en_smif_slave_select_t slave);
void HyperFlashEraseOpInMMIO(cy_en_smif_slave_select_t slave, uint32_t address);
void HyperFlashWordProgramOpInMMIO(cy_en_smif_slave_select_t slave, uint32_t address, uint16_t* writeData, uint16_t halfWordCount);
bool SetSmifFrequencyWithDelayLineTapNumCalibration_HFLASH(uint32_t safeFreq, uint32_t freq, uint32_t usedAddr, cy_en_smif_slave_select_t slave);
void CalibrateDelayLineSelect(uint32_t freq, bool isDDR);

static void EnterHyperBusMode(volatile cy_stc_smif_reg_device_t *device);

/*******************************/
/*** Cortex M7 MPU Utilities ***/
/*******************************/
#define BG_REGION_ADDR      (0x00000000UL)
#define SRAM_REGION_ADDR    (0x28000000UL)
#define FLASH_REGION_ADDR   (0x10000000UL)
#define SMIF0_DEV_XIP_ADDR (0x60000000UL)
#define SMIF1_DEV_XIP_ADDR (0x80000000UL) // Only For Cluster MCU
#define REGISTE_REGION_ADDR (0x40000000UL)
typedef enum
{
    MPU_REG_BG = 0,
    MPU_REG_SRAM,
    MPU_REG_FLASH,
    MPU_REG_SMIF0_DEV,
    MPU_REG_SMIF1_DEV, // Only for cluster MCU
    MPU_REG_REGISTER,
} en_mpu_region_name_t;

cy_stc_mpu_region_cfg_t g_mpuCfg[] =
{
    {.addr = BG_REGION_ADDR,      .size = CY_MPU_SIZE_4GB,   .permission = CY_MPU_ACCESS_P_FULL_ACCESS, .attribute = CY_MPU_ATTR_NORM_MEM_WT, .execute = CY_MPU_INST_ACCESS_EN, .srd = 0x00u, .enable = CY_MPU_ENABLE},
    {.addr = SRAM_REGION_ADDR,    .size = CY_MPU_SIZE_1MB,   .permission = CY_MPU_ACCESS_P_FULL_ACCESS, .attribute = CY_MPU_ATTR_NORM_MEM_WT, .execute = CY_MPU_INST_ACCESS_EN, .srd = 0x00u, .enable = CY_MPU_ENABLE},
    {.addr = FLASH_REGION_ADDR,   .size = CY_MPU_SIZE_8MB,   .permission = CY_MPU_ACCESS_P_FULL_ACCESS, .attribute = CY_MPU_ATTR_NORM_MEM_WT, .execute = CY_MPU_INST_ACCESS_EN, .srd = 0x00u, .enable = CY_MPU_ENABLE},
    {.addr = SMIF0_DEV_XIP_ADDR,  .size = CY_MPU_SIZE_256MB, .permission = CY_MPU_ACCESS_P_FULL_ACCESS, .attribute = CY_MPU_ATTR_STR_ORD_DEV, .execute = CY_MPU_INST_ACCESS_EN, .srd = 0x00u, .enable = CY_MPU_ENABLE},
    {.addr = SMIF1_DEV_XIP_ADDR,  .size = CY_MPU_SIZE_256MB, .permission = CY_MPU_ACCESS_P_FULL_ACCESS, .attribute = CY_MPU_ATTR_STR_ORD_DEV, .execute = CY_MPU_INST_ACCESS_EN, .srd = 0x00u, .enable = CY_MPU_ENABLE},
    {.addr = REGISTE_REGION_ADDR, .size = CY_MPU_SIZE_256MB, .permission = CY_MPU_ACCESS_P_FULL_ACCESS, .attribute = CY_MPU_ATTR_SHR_DEV,     .execute = CY_MPU_INST_ACCESS_EN, .srd = 0x00u, .enable = CY_MPU_ENABLE},
};

#define MPU_CFG_ARRAY_SIZE(array) (sizeof(array)/sizeof(cy_stc_mpu_region_cfg_t))

/*** End of Cortex M7 MPU Utilities ***/

int main(void)
{
    SystemInit();

    __enable_irq();

    /********* Core MPU setting **************/
    // When writing to SMIF XIP address via AXI, memory attribute for the XIP region has to be strongly ordered. 
    CY_ASSERT(Cy_MPU_Setup(g_mpuCfg, MPU_CFG_ARRAY_SIZE(g_mpuCfg), CY_MPU_DISABLE_USE_DEFAULT_MAP, CY_MPU_DISABLED_DURING_FAULT_NMI) == CY_MPU_SUCCESS);

    Cy_SysClk_HfClkEnable(SMIF_HF_CLOCK);   // 32MHz
    ChangePLLFrequency(64000000); // SMIF out clock will be 32,000,000

    SmifPortInit(smifPortCfg ,SIZE_OF_PORT);

    // Please comment out below line if the external flash is in Hyper bus mode by default
    EnterHyperBusMode(SMIF_DEVICE_USED);

    cy_en_smif_slave_select_t slaveSelect = (SMIF_DEVICE_USED == SMIF_DEVICE0)
                                          ? CY_SMIF_SLAVE_SELECT_0
                                          : CY_SMIF_SLAVE_SELECT_1;

    /*************************************/
    /*       SMIF Initialization         */
    /*************************************/
    Cy_SMIF_DeInit(SMIF_USED);
    Cy_SMIF_Init(SMIF_USED, &smifConfig, 1000, &smifContext);
    Cy_SMIF_Set_DelayTapSel(SMIF_USED, (CY_SMIF_GetDelayTapsNumber(SMIF_USED) - 1));
    Cy_SMIF_Enable(SMIF_USED, &smifContext);

    volatile CY_SMIF_FLASHDATA* pHyperFlashBaseAddr = (CY_SMIF_FLASHDATA*)(CY_SMIF_GetXIP_Address(SMIF_USED));

    /*************************************/
    /*     SMIF DEVICE Initialization    */
    /*************************************/
    smifDevHBFlashCfg.addr = (uint32_t)pHyperFlashBaseAddr;
    Cy_SMIF_InitDeviceHyperBus(SMIF_DEVICE_USED, &smifDevHBFlashCfg);

    /*************************************/
    /*      Hyper Bus Initialization     */
    /*************************************/
    /***** Set Configuration register 1 *****/
    // Load Configuration register 1
    cy_un_h_flash_cfg1_reg_t flashVCR1  = { .u16 = CFG_REG1_DEFAULT_S26H };
    flashVCR1.fld.readLatency           = smifDevHBFlashCfg.lc_hb;
    flashVCR1.fld.driveStrength         = CY_SMIF_HB_S26H_DRV_45_OHMS;
    CY_SMIF_HbFlash_LoadVolatileConfigReg(pHyperFlashBaseAddr, flashVCR1.u16);

    // Read Configuration register 1
    uint16_t readConfig1RegVal = CY_SMIF_HbFlash_ReadVolatileConfigReg(pHyperFlashBaseAddr);

    // Verify Configuration register 1
    CY_ASSERT(readConfig1RegVal == flashVCR1.u16);

    /***** Set Configuration register 2 *****/
    // Load Configuration register 2
    cy_un_h_flash_cfg2_reg_t flashVCR2  = { .u16 = CFG_REG2_DEFAULT_S26H };
    flashVCR2.fld.eccErrorSel           = CY_SMIF_HB_1BIT_ECC_ERR_PROCESSED;
    CY_SMIF_HbFlash_LoadVolatileConfigReg2(pHyperFlashBaseAddr, flashVCR2.u16);

    // Read Configuration register 2
    uint16_t readConfig2RegVal = CY_SMIF_HbFlash_ReadVolatileConfigReg2(pHyperFlashBaseAddr);

    // Verify Configuration register 2
    CY_ASSERT(readConfig2RegVal == flashVCR2.u16);

    /*************************************/
    /*  Calibration for Delay RWDS line  */
    /*************************************/
    /*********  Calibrate Delay Line Select   *******/
    // Please modify according to your HW condition.
    uint32_t targetSmifFreqFlash = TARGET_CLK_FREQ_IN_MHZ * 1000000;
    uint32_t safeSmifFreqFlash = 32000000;

    CalibrateDelayLineSelect(targetSmifFreqFlash, true);
    SetSmifFrequencyWithDelayLineTapNumCalibration_HFLASH(safeSmifFreqFlash, targetSmifFreqFlash, SECTOR_ADDR_USED_BY_CALIB, slaveSelect);

    /**********************************************/
    /*       Hyper Flash Test In XIP mode         */
    /**********************************************/
    /*********  Erase Top Sector   *******/
    CY_SMIF_HbFlash_SectorEraseOp(pHyperFlashBaseAddr, TEST_SECTOR_ADDRESS);

    /*********  Write Data to the Top Sector   *******/
    for(uint32_t i_WriteAddr = TEST_SECTOR_ADDRESS; i_WriteAddr < (TEST_PROGRAM_SIZE / 2);i_WriteAddr += HB_WRITE_BUF_SIZE_WORD)
    {
        // Write buffer size = 512 byte = 256 word
        CY_SMIF_HbFlash_WriteBufferProgramOp(pHyperFlashBaseAddr, i_WriteAddr, HB_WRITE_BUF_SIZE_WORD, (CY_SMIF_FLASHDATA*)&programData[i_WriteAddr*2]);
    }

    /*********  Marge mode enable   *******/
    // If more than 1 cycle merge time accepted, there will be long CS# low duration when burst reading.
    // It may cause error because Low/High ratio of CLK should be around 50/50 during reading because of Memory device restriction.
    Cy_SMIF_DeviceTransferMergingEnable(SMIF_DEVICE_USED,CY_SMIF_MER_TIMEOUT_1_CYCLE);
    while(Cy_SMIF_IsBusy(SMIF_USED));

    // From here on, We use XIP region as normal MEMORY to improve performance.
    g_mpuCfg[SMIF_MPU_REG_NO].attribute = CY_MPU_ATTR_NORM_MEM_WT;
    CY_ASSERT(Cy_MPU_Setup(g_mpuCfg, MPU_CFG_ARRAY_SIZE(g_mpuCfg), CY_MPU_DISABLE_USE_DEFAULT_MAP, CY_MPU_DISABLED_DURING_FAULT_NMI) == CY_MPU_SUCCESS);

    /*********  Read back and verify   *******/
    static uint8_t readBuf[TEST_PROGRAM_SIZE];
    memcpy(readBuf, (void*)pHyperFlashBaseAddr, TEST_PROGRAM_SIZE);

    // Verify data
    CY_ASSERT(memcmp(readBuf, programData, TEST_PROGRAM_SIZE) == 0); 

    /**********************************************/
    /*       Hyper Flash Test In MMIO mode        */
    /**********************************************/
    g_mpuCfg[SMIF_MPU_REG_NO].attribute = CY_MPU_ATTR_STR_ORD_DEV;
    CY_ASSERT(Cy_MPU_Setup(g_mpuCfg, MPU_CFG_ARRAY_SIZE(g_mpuCfg), CY_MPU_DISABLE_USE_DEFAULT_MAP, CY_MPU_DISABLED_DURING_FAULT_NMI) == CY_MPU_SUCCESS);
    // MMIO mode does not support inter-page boundary latency.
    // Thus, use double initial latency mode in which no inter-page boundary exists.
    flashVCR2.fld.doubleLatencySel      = CY_SMIF_HB_DOUBLE_LATENCY;
    CY_SMIF_HbFlash_LoadVolatileConfigReg2(pHyperFlashBaseAddr, flashVCR2.u16);
    // Read Configuration register 2
    readConfig2RegVal = CY_SMIF_HbFlash_ReadVolatileConfigReg2(pHyperFlashBaseAddr);
    // Verify Configuration register 2
    CY_ASSERT(readConfig2RegVal == flashVCR2.u16);

    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);

    /*********  Erase Top Sector   *******/
    HyperFlashEraseOpInMMIO(slaveSelect, TEST_SECTOR_ADDRESS);

    /*********  Write Data to the Top Sector   *******/
    for(uint32_t i_WriteAddr = TEST_SECTOR_ADDRESS; i_WriteAddr < (TEST_PROGRAM_SIZE / 2);i_WriteAddr += HB_WRITE_BUF_SIZE_WORD)
    {
        // write buffer size = 512 byte = 256 word
        HyperFlashWordProgramOpInMMIO(slaveSelect, (TEST_SECTOR_ADDRESS + i_WriteAddr), (uint16_t*)&programData[i_WriteAddr*2], HB_WRITE_BUF_SIZE_WORD);
    }

    /*********  Read back and verify   *******/
    #define READ_BUF_SIZE_IN_HWORD (256)
    for(uint32_t i_WriteAddr = TEST_SECTOR_ADDRESS; i_WriteAddr < (TEST_PROGRAM_SIZE / 2);i_WriteAddr += READ_BUF_SIZE_IN_HWORD)
    {
        static uint16_t readBuffer[READ_BUF_SIZE_IN_HWORD];
        Cy_SMIF_HB_MMIO_Read(SMIF_USED,
                             slaveSelect,
                             CY_SMIF_HB_COUTINUOUS_BURST,
                             (i_WriteAddr+TEST_SECTOR_ADDRESS), // address
                             READ_BUF_SIZE_IN_HWORD,            // size in half word
                             readBuffer,
                             smifDevHBFlashCfg.lc_hb,
                             true,                              // Double initial latency
                             true,                              // Blocking mode
                             &smifContext
                             );
        CY_ASSERT(memcmp(readBuffer, &programData[i_WriteAddr*2], READ_BUF_SIZE_IN_HWORD*2) == 0);
    }

    for(;;);
}

static void SetModeWithBusyCheck(volatile cy_stc_smif_reg_t *base, cy_en_smif_mode_t mode)
{
    cy_en_smif_status_t status;
    do
    {
        status = Cy_SMIF_SetMode(base, mode);
    }while(status != CY_SMIF_SUCCESS);
}

uint16_t HyperFlashReadStatusOpInMMIO(cy_en_smif_slave_select_t slave)
{
    uint16_t writeBuf   = 0x0070;
    uint16_t readStatus = 0x0000;

    Cy_SMIF_HB_MMIO_Write(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST,
                         0x555,                   // address
                         1,                       // size in half word
                         &writeBuf,
                         CY_SMIF_HB_FLASH,
                         CY_SMIF_HB_LC5,          // latency code, do not care
                         true,                    // Blocking mode
                         &smifContext
                         );

    Cy_SMIF_HB_MMIO_Read(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST,
                         0x0000,                   // address
                         1,                        // size in half word
                         &readStatus,
                         smifDevHBFlashCfg.lc_hb,
                         false,                    // Single initial latency
                         true,                     // Blocking mode
                         &smifContext
                         );

    return (readStatus);
}


void HyperFlashEraseOpInMMIO(cy_en_smif_slave_select_t slave, uint32_t address)
{
    uint16_t writeBuf = 0x0000;

    writeBuf = 0x00AA;
    Cy_SMIF_HB_MMIO_Write(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST, // not cared
                         0x555,                   // address
                         1,                       // size in half word
                         &writeBuf,
                         CY_SMIF_HB_FLASH,
                         CY_SMIF_HB_LC5,          // latency code, do not care
                         true,                    // Blocking mode
                         &smifContext
                         );

    writeBuf = 0x0055;
    Cy_SMIF_HB_MMIO_Write(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST, // not cared
                         0x2AA,                   // address
                         1,                       // size in half word
                         &writeBuf,
                         CY_SMIF_HB_FLASH,
                         CY_SMIF_HB_LC5,          // latency code, do not care
                         true,                    // Blocking mode
                         &smifContext
                         );

    writeBuf = 0x0080;
    Cy_SMIF_HB_MMIO_Write(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST, // not cared
                         0x555,                   // address
                         1,                       // size in half word
                         &writeBuf,
                         CY_SMIF_HB_FLASH,
                         CY_SMIF_HB_LC5,          // latency code, do not care
                         true,                    // Blocking mode
                         &smifContext
                         );

    writeBuf = 0x00AA;
    Cy_SMIF_HB_MMIO_Write(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST, // not cared
                         0x555,                   // address
                         1,                       // size in half word
                         &writeBuf,
                         CY_SMIF_HB_FLASH,
                         CY_SMIF_HB_LC5,          // latency code, do not care
                         true,                    // Blocking mode
                         &smifContext
                         );

    writeBuf = 0x0055;
    Cy_SMIF_HB_MMIO_Write(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST, // not cared
                         0x2AA,                   // address
                         1,                       // size in half word
                         &writeBuf,
                         CY_SMIF_HB_FLASH,
                         CY_SMIF_HB_LC5,          // latency code, do not care
                         true,                    // Blocking mode
                         &smifContext
                         );

    writeBuf = 0x0030;
    Cy_SMIF_HB_MMIO_Write(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST, // not cared
                         address,                 // address
                         1,                       // size in half word
                         &writeBuf,
                         CY_SMIF_HB_FLASH,
                         CY_SMIF_HB_LC5,          // latency code, do not care
                         true,                    // Blocking mode
                         &smifContext
                         );


    cy_un_h_flash_stat_reg_t flashStatus;
    do
    {
        flashStatus.u16 = HyperFlashReadStatusOpInMMIO(slave);
    } while(flashStatus.fld.DeviceReady == 0); // wait for the device becoming ready

    return;
}


void HyperFlashWordProgramOpInMMIO(cy_en_smif_slave_select_t slave, uint32_t address, uint16_t* writeData, uint16_t halfWordCount)
{
    uint16_t writeBuf = 0x0000;

    writeBuf = 0x00AA;
    Cy_SMIF_HB_MMIO_Write(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST, // not cared
                         0x555,                   // address
                         1,                       // size in half word
                         &writeBuf,
                         CY_SMIF_HB_FLASH,
                         CY_SMIF_HB_LC5,          // latency code, do not care
                         true,                    // Blocking mode
                         &smifContext
                         );

    writeBuf = 0x0055;
    Cy_SMIF_HB_MMIO_Write(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST, // not cared
                         0x2AA,                   // address
                         1,                       // size in half word
                         &writeBuf,
                         CY_SMIF_HB_FLASH,
                         CY_SMIF_HB_LC5,          // latency code, do not care
                         true,                    // Blocking mode
                         &smifContext
                         );

    writeBuf = 0x00A0;
    Cy_SMIF_HB_MMIO_Write(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST, // not cared
                         0x555,                   // address
                         1,                       // size in half word
                         &writeBuf,
                         CY_SMIF_HB_FLASH,
                         CY_SMIF_HB_LC5,          // latency code, do not care
                         true,                    // Blocking mode
                         &smifContext
                         );

    Cy_SMIF_HB_MMIO_Write(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST, // not cared
                         address,                 // address
                         halfWordCount,                       // size in half word
                         writeData,
                         CY_SMIF_HB_FLASH,
                         CY_SMIF_HB_LC5,          // latency code, do not care
                         true,                    // Blocking mode
                         &smifContext
                         );

    cy_un_h_flash_stat_reg_t flashStatus;
    do
    {
        flashStatus.u16 = HyperFlashReadStatusOpInMMIO(slave);
    } while(flashStatus.fld.DeviceReady == 0); // wait for the device becoming ready

    return;
}

bool SetSmifFrequencyWithDelayLineTapNumCalibration_HFLASH(uint32_t safeFreq, uint32_t freq, uint32_t usedAddr, cy_en_smif_slave_select_t slave)
{
    uint8_t readTest[12]      = {0};
    uint8_t testData[12]      = {0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x55, 0xAA};
    bool    isMatch[32]       = {false};

    cy_en_smif_mode_t savedMode = Cy_SMIF_GetMode(SMIF_USED);

    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);

    ChangePLLFrequency(safeFreq*2);

    HyperFlashEraseOpInMMIO(slave, usedAddr);
    HyperFlashWordProgramOpInMMIO(slave, usedAddr, (uint16_t*)testData/*Data*/, 0x6);

    ChangePLLFrequency(freq*2);

    for(uint8_t i_tapNum = 0; i_tapNum < CY_SMIF_GetDelayTapsNumber(SMIF_USED); i_tapNum++)
    {
        while(Cy_SMIF_IsBusy(SMIF_USED));
        Cy_SMIF_Set_DelayTapSel(SMIF_USED, i_tapNum);
        Cy_SMIF_HB_MMIO_Read(SMIF_USED,
                         slave,
                         CY_SMIF_HB_COUTINUOUS_BURST,
                         usedAddr,                 // address
                         0x6,                      // size in half word
                         (uint16_t*)readTest,
                         smifDevHBFlashCfg.lc_hb,
                         false,                    // single initial latency
                         true,                     // Blocking mode
                         &smifContext
                         );
        if(memcmp(&readTest, testData, 12) == 0)
        {
            isMatch[i_tapNum] = true;
        }
        else
        {
            isMatch[i_tapNum] = false;
        }
    }

    uint8_t bestTapNum = 0xFF;
    uint8_t consectiveMatchNum    = 0;
    uint8_t maxConsectiveMatchNum = 0;
    for(uint8_t i_tapNum = 0; i_tapNum < CY_SMIF_GetDelayTapsNumber(SMIF_USED); i_tapNum++)
    {
        if(isMatch[i_tapNum] == true)
        {
            consectiveMatchNum += 1;
        }
        else
        {
            if(maxConsectiveMatchNum < consectiveMatchNum)
            {
                maxConsectiveMatchNum = consectiveMatchNum;
                bestTapNum = i_tapNum - ((maxConsectiveMatchNum + 1) / 2);
                consectiveMatchNum = 0;
            }
        }
    }

    if(maxConsectiveMatchNum < consectiveMatchNum)
    {
        bestTapNum = CY_SMIF_GetDelayTapsNumber(SMIF_USED) - ((consectiveMatchNum + 1) / 2);
    }

    if(bestTapNum != 0xFF)
    {
        Cy_SMIF_Set_DelayTapSel(SMIF_USED, bestTapNum);
    }
    else
    {
        CY_ASSERT(false); // No right tap number
    }

    HyperFlashEraseOpInMMIO(slave, usedAddr);

    SetModeWithBusyCheck(SMIF_USED, savedMode);

    return true;
}


void CalibrateDelayLineSelect(uint32_t freq, bool isDDR)
{
    static const float minDelayPerTapInNs[4] = {0.0559, 0.1197, 0.1935, 0.3973};

    /*********  Calibrate Delay Line Select   *******/
    // disable delay line
    uint32_t tapLineChecked = 0;
    for(; tapLineChecked < 4; tapLineChecked++)
    {
        float maxTapNum;
        if(isDDR == true)
        {
            maxTapNum = (uint32_t)((1000000000.0 / (freq * 4 * minDelayPerTapInNs[tapLineChecked])) + 0.5);
        }
        else
        {
            maxTapNum = (uint32_t)((1000000000.0 / (freq * 2 * minDelayPerTapInNs[tapLineChecked])) + 0.5);
        }

        if(maxTapNum <= CY_SMIF_GetDelayTapsNumber(SMIF_USED))
        {
            break;
        }
    }

    if(tapLineChecked == 4)
    {
        tapLineChecked = 3;
    }

    while(Cy_SMIF_IsBusy(SMIF_USED));
    Cy_SMIF_Disable(SMIF_USED);
    Cy_SMIF_Set_DelayLineSel(SMIF_USED, (cy_en_smif_delay_line_t)tapLineChecked);
    Cy_SMIF_Enable(SMIF_USED, &smifContext);
}


cy_stc_smif_config_t smifSpiConfig =
{
    .txClk         = CY_SMIF_INV_FOR_DDR,
    .rxClk         = CY_SMIF_INV_OUTPUT_CLK, // Note
    .dlpAuto       = CY_SMIF_DLP_UPDATE_MANUAL,
    .capDelay      = CY_SMIF_CAPTURE_DELAY_0_CYCLE,
    .delaySel      = CY_SMIF_1_SEN_SEL_PER_TAP,
    .deselectDelay = CY_SMIF_MIN_DESELECT_1_CLK,
    .setupDelay    = CY_SMIF_SETUP_0_CLK_PULUS_MIN,
    .holdDelay     = CY_SMIF_HOLD_0_CLK_PULUS_MIN,
    .mode          = CY_SMIF_NORMAL, // MMIO mode
    .blockEvent    = CY_SMIF_BUS_ERROR,
};

// return 1, when one of selected device return error during erasing.
// return 0, when erasing completed without error.
static bool WaitDeviceBecomesReady(cy_en_smif_slave_select_t slave, cy_en_smif_semp_reg_latency_code_t regLc, cy_en_smif_trx_type_t transMode)
{
    while(1)
    {
        un_cy_smif_semp_STR1_t status1;
#if 0
        Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 slave, 
                                 &status1.u8, 
                                 CY_TRX_ADDR_StatusRegister1_V, 
                                 regLc,
                                 TEST_ADDR_LEN,
                                 transMode,
                                 &smifContext);
#else
        Cy_SMIF_SEMP_ReadStatusRegister1(SMIF_USED,
                                 slave, 
                                 &status1.u8, 
                                 regLc,
                                 transMode,
                                 &smifContext);
#endif
        // After transfer from Regacy SPI to OCTAL(SDR/DDR). RDYBSY may be also "0".
        if(status1.field.u1ERSERR == 1)
        {
            return 1;
        }

        if(status1.field.u1PRGERR == 1)
        {
            return 1;
        }

        if(status1.field.u1RDYBSY == 0)
        {
            break;
        }
    }

    return 0;
}

static void WriteRegisterSequence(cy_en_smif_slave_select_t slave, cy_en_smif_semp_reg_addr_t addr, uint8_t value, cy_en_smif_trx_addr_len_t addrLen, cy_en_smif_semp_reg_latency_code_t regLc, cy_en_smif_trx_type_t transMode)
{
    Cy_SMIF_SEMP_WriteEnable(SMIF_USED, slave, transMode, &smifContext);
    Cy_SMIF_SEMP_WriteAnyRegister(SMIF_USED, slave, value, addr, addrLen, transMode, &smifContext);
    WaitDeviceBecomesReady(slave, regLc, transMode);
}

// This function has a side effect that this function will set address length to 4 byte anyway.
static bool DetectVolatileRegLantency(cy_en_smif_slave_select_t slave, cy_en_smif_semp_reg_latency_code_t* detectRegLatency)
{
    cy_stc_smif_semp_id_t readId;
    Cy_SMIF_SEMP_ReadId(SMIF_USED, slave, &readId, CY_SMIF_SEMP_REG_LATENCY0, CY_SPI_TRANSACTION_1S1S1S, sizeof(cy_stc_smif_semp_id_t), &smifContext);
    if(readId.manufacturerId == 0x34)
    {
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY0; // or CY_SMIF_SEMP_REG_LATENCY1 (will decide later)
    }
    else if(readId.manufacturerId == 0x1A)
    {
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY2;
    }
    else if(readId.manufacturerId == 0x0D)
    {
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY3;
    }
    else
    {
        return false;
    }

    // At here, we don't know the device setting of address length
    // then try both 4 byte address and 3 byte address anyway!!
    un_cy_smif_semp_CFR2_t presetConfigReg2_0 = {0};

    presetConfigReg2_0.field.u4MEMLAT = CY_SMIF_SEMP_RD_LATENCY0;
    presetConfigReg2_0.field.u1ADRBYT = CY_TRX_ADDR_4BYTE;

    WriteRegisterSequence(slave,
                          CY_SEMP_REG_ADDR_CFR2_V,
                          presetConfigReg2_0.u8,
                          CY_TRX_ADDR_4BYTE,
                          *detectRegLatency,
                          CY_SPI_TRANSACTION_1S1S1S);

    WriteRegisterSequence(slave,
                          CY_SEMP_REG_ADDR_CFR2_V,
                          presetConfigReg2_0.u8,
                          CY_TRX_ADDR_3BYTE,
                          *detectRegLatency,
                          CY_SPI_TRANSACTION_1S1S1S);

    // Read back configuration register 2
    if(*detectRegLatency == CY_SMIF_SEMP_REG_LATENCY0)
    {
        un_cy_smif_semp_CFR2_t monitorConfigReg2_0 = {0};
        Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                     slave, 
                                     &monitorConfigReg2_0.u8, 
                                     CY_SEMP_REG_ADDR_CFR2_V, 
                                     CY_SMIF_SEMP_REG_LATENCY0,
                                     CY_TRX_ADDR_4BYTE,
                                     CY_SPI_TRANSACTION_1S1S1S,
                                     &smifContext);
        if(monitorConfigReg2_0.u8 == presetConfigReg2_0.u8)
        {
            *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY0;
            return true;
        }

        Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                     slave, 
                                     &monitorConfigReg2_0.u8, 
                                     CY_SEMP_REG_ADDR_CFR2_V, 
                                     CY_SMIF_SEMP_REG_LATENCY1,
                                     CY_TRX_ADDR_4BYTE,
                                     CY_SPI_TRANSACTION_1S1S1S,
                                     &smifContext);
        if(monitorConfigReg2_0.u8 == presetConfigReg2_0.u8)
        {
            *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY1;
        }
        else
        {
            CY_ASSERT(false);
        }
    }
    return true;
}

static void EnterHyperBusMode(volatile cy_stc_smif_reg_device_t *device)
{
    cy_en_smif_slave_select_t slaveSelect = (device == SMIF_DEVICE0)
                                          ? CY_SMIF_SLAVE_SELECT_0
                                          : CY_SMIF_SLAVE_SELECT_1;

    cy_stc_ex_dev_context_t smifDeviceContext;

    CY_ASSERT(Cy_SMIF_InitExMemContext(CY_SMIF_SEMP, &smifDeviceContext) == CY_SMIF_SUCCESS);

    /*************************/
    /* 1. Setting for SMIF 0 */
    /*************************/
    Cy_SMIF_DeInit(SMIF_USED);
    Cy_SMIF_Init(SMIF_USED, &smifSpiConfig, 1000, &smifContext);

    /************************************/
    /*  2. Setting for SMIF 0 Device 0  */
    /************************************/
    Cy_SMIF_DeviceSetDataSelect(device, CY_SMIF_DATA_SEL0);
    Cy_SMIF_DeviceWriteEnable(device);

    /********************/
    /* 3. Enable SMIF 0 */
    /********************/
    Cy_SMIF_Enable(SMIF_USED, &smifContext);

    /*********************************************************/
    /********* Read Status and Configuration registers *******/
    /*********************************************************/

    cy_en_smif_semp_reg_latency_code_t initialRegLatency;
    if(DetectVolatileRegLantency(slaveSelect, &initialRegLatency) == false)
    {
        return; // already Hyper bus mode.
    }

    // Setting Configuration Register 3
    un_cy_smif_semp_CFR3_t writeConfigReg3_0 ={0};
    writeConfigReg3_0.field.u1INTFTP = 1; // Hyper Bus
    WriteRegisterSequence(slaveSelect,
                          CY_SEMP_REG_ADDR_CFR3_V,
                          writeConfigReg3_0.u8,
                          CY_TRX_ADDR_4BYTE,
                          initialRegLatency,
                          CY_SPI_TRANSACTION_1S1S1S);
}

/* [] END OF FILE */

