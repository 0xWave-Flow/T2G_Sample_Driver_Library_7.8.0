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

#define TEST_WRITE_LC        CY_SMIF_SEMP_WT_LATENCY0
#define TEST_READ_LC         CY_SMIF_SEMP_RD_LATENCY2  // SDR SPI: up to 81MHz, SDR OCTAL: up to 92MHz, DDR OCTAL: up to 85MHz
#define TEST_READ_REG_LC     CY_SMIF_SEMP_REG_LATENCY1 // SDR SPI: up to 133MHz, SDR OCTAL: up to 133MHz, DDR OCTAL: up to 66MHz
#define TEST_ADDR_LEN        CY_TRX_ADDR_4BYTE

/******************************************************/

#define TEST_PROGRAM_SECTOR (0x00123400ul)

#define DEVICE_PROGRAM_BUFFER_SIZE (0x100) // 256B
#define DEVICE_SECTOR_ERASE_SIZE   (0x00040000) // 256KB

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
uint8_t readDataBuffer[TEST_PROGRAM_SIZE] = {0};

cy_stc_smif_config_t smifConfig =
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

cy_stc_device_config_t smifDev0Cfg =
{
    .xipReadCmdId       = CY_SMIF_SEMP_RDID_1S1S1S_4ADR, // will be updated in application
    .xipReadMode        = 0x00,                    // Do not care
    .writeEn            = true,
    .xipWriteCmdId      = CY_SMIF_SEMP_WTID_1S1S1S_4ADR,
    .xipWriteMode       = 0u,                      // Do not care
    .mergeEnable        = true,
    .mergeTimeout       = CY_SMIF_MER_TIMEOUT_4096_CYCLE,
    .totalTimeoutEnable = false,
    .totalTimeout       = 1000u,
    .addr               = 0,                       // will be updated in the application
    .size               = CY_SMIF_DEVICE_64M_BYTE,
    .rdLatencyCode      = TEST_READ_LC,
    .wtLatencyCode      = TEST_WRITE_LC,
};


static bool WaitDeviceBecomesReady(cy_en_smif_slave_select_t slave, cy_en_smif_semp_reg_latency_code_t regLc, cy_en_smif_trx_type_t transMode);
static void SetModeWithBusyCheck(volatile cy_stc_smif_reg_t *base, cy_en_smif_mode_t mode);
static void WriteRegisterSequence(cy_en_smif_slave_select_t slave, cy_en_smif_semp_reg_addr_t addr, uint8_t value, cy_en_smif_trx_addr_len_t addrLen, cy_en_smif_semp_reg_latency_code_t regLc, cy_en_smif_trx_type_t transMode);
static void DetectVolatileTrModeAndRegLantency(cy_en_smif_slave_select_t slave, cy_en_smif_trx_type_t* detectTrMode, cy_en_smif_semp_reg_latency_code_t* detectRegLatency);

cy_stc_ex_dev_context_t smifDevice0Context;
cy_stc_ex_dev_context_t smifDevice1Context;

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

    Cy_SysClk_HfClkEnable(SMIF_HF_CLOCK);
    Cy_SysClk_HfClockSetDivider(SMIF_HF_CLOCK, CY_SYSCLK_HFCLK_DIVIDE_BY_4);

    // Please modify according to your HW condition.
    ChangePLLFrequency(100000000); // SMIF out clock will be 50,000,000 //YOTS: for initial test

    SmifPortInit(smifPortCfg, SIZE_OF_PORT);

    CY_ASSERT(Cy_SMIF_InitExMemContext(CY_SMIF_SEMP, &smifDevice0Context) == CY_SMIF_SUCCESS);
    CY_ASSERT(Cy_SMIF_InitExMemContext(CY_SMIF_SEMP, &smifDevice1Context) == CY_SMIF_SUCCESS);

    /*************************/
    /* 1. Setting for SMIF 0 */
    /*************************/
    Cy_SMIF_DeInit(SMIF_USED);
    Cy_SMIF_Init(SMIF_USED, &smifConfig, 1000, &smifContext);

    /************************************/
    /* 2. Setting for SMIF 0 Device 0/1 */
    /************************************/
    Cy_SMIF_DeviceSetDataSelect(SMIF_DEVICE0, CY_SMIF_DATA_SEL0);
    Cy_SMIF_DeviceWriteEnable(SMIF_DEVICE0);
    Cy_SMIF_DeviceSetDataSelect(SMIF_DEVICE1, CY_SMIF_DATA_SEL0);
    Cy_SMIF_DeviceWriteEnable(SMIF_DEVICE1);

    /********************/
    /* 3. Enable SMIF 0 */
    /********************/
    Cy_SMIF_Enable(SMIF_USED, &smifContext);

    /*********************************************************/
    /********* Read Status and Configuration registers *******/
    /*********************************************************/
    cy_en_smif_trx_type_t initialTransMode;
    cy_en_smif_semp_reg_latency_code_t initialRegLatency;
    // this function will set address length to 4 byte anyway.
    DetectVolatileTrModeAndRegLantency(CY_SMIF_SLAVE_SELECT_0, &initialTransMode, &initialRegLatency);

#if 0
    // Software Rest 
    Cy_SMIF_SEMP_SoftwareResetEnable(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, g_TransMode, &smifContext);
    Cy_SMIF_SEMP_SoftwareReset(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, g_TransMode, &smifContext);
    CY_ASSERT(WaitDeviceBecomesReady(CY_SMIF_SLAVE_SELECT_0, DEFAULT_READ_REG_LC, g_TransMode) == 0);
#endif

    // Setting Configuration Register 2
    un_cy_smif_semp_CFR2_t writeConfigReg2_0 = {0};
    writeConfigReg2_0.field.u4MEMLAT = TEST_READ_LC;
    writeConfigReg2_0.field.u1ADRBYT = TEST_ADDR_LEN;
    WriteRegisterSequence(CY_SMIF_SLAVE_SELECT_0,
                          CY_SEMP_REG_ADDR_CFR2_V,
                          writeConfigReg2_0.u8,
                          CY_TRX_ADDR_4BYTE,
                          initialRegLatency,
                          initialTransMode);

    un_cy_smif_semp_CFR2_t readConfigReg2_0 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_0, 
                                 &readConfigReg2_0.u8, 
                                 CY_SEMP_REG_ADDR_CFR2_V, 
                                 initialRegLatency,
                                 TEST_ADDR_LEN,
                                 initialTransMode,
                                 &smifContext);
    CY_ASSERT(writeConfigReg2_0.field.u4MEMLAT == readConfigReg2_0.field.u4MEMLAT);
    CY_ASSERT(writeConfigReg2_0.field.u1ADRBYT == readConfigReg2_0.field.u1ADRBYT);

    // Setting Configuration Register 3
    un_cy_smif_semp_CFR3_t writeConfigReg3_0 ={0};
    writeConfigReg3_0.field.u2VRGLAT = TEST_READ_REG_LC;
    WriteRegisterSequence(CY_SMIF_SLAVE_SELECT_0,
                          CY_SEMP_REG_ADDR_CFR3_V,
                          writeConfigReg3_0.u8,
                          TEST_ADDR_LEN,
                          initialRegLatency,
                          initialTransMode);

    un_cy_smif_semp_CFR3_t readConfigReg3_0 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_0, 
                                 &readConfigReg3_0.u8, 
                                 CY_SEMP_REG_ADDR_CFR3_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 initialTransMode,
                                 &smifContext);
    CY_ASSERT(writeConfigReg3_0.field.u2VRGLAT == readConfigReg3_0.field.u2VRGLAT);

    // Setting Configuration Register 4
    un_cy_smif_semp_CFR4_t previousConfigReg4_0 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_0, 
                                 &previousConfigReg4_0.u8, 
                                 CY_SEMP_REG_ADDR_CFR4_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 initialTransMode,
                                 &smifContext);

    un_cy_smif_semp_CFR4_t writeConfigReg4_0 = previousConfigReg4_0;
    writeConfigReg4_0.field.u1RBSTWP = 0; // Read Wrapped Burst disable
    WriteRegisterSequence(CY_SMIF_SLAVE_SELECT_0,
                          CY_SEMP_REG_ADDR_CFR4_V,
                          writeConfigReg4_0.u8,
                          TEST_ADDR_LEN,
                          TEST_READ_REG_LC,
                          initialTransMode);

    un_cy_smif_semp_CFR4_t readConfigReg4_0 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_0, 
                                 &readConfigReg4_0.u8, 
                                 CY_SEMP_REG_ADDR_CFR4_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 initialTransMode,
                                 &smifContext);
    CY_ASSERT(writeConfigReg4_0.field.u1RBSTWP == readConfigReg4_0.field.u1RBSTWP);

    // Setting Configuration Register 5
    un_cy_smif_semp_CFR5_t previousConfigReg5_0 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_0, 
                                 &previousConfigReg5_0.u8, 
                                 CY_SEMP_REG_ADDR_CFR5_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 initialTransMode,
                                 &smifContext);

    un_cy_smif_semp_CFR5_t writeConfigReg5_0 = previousConfigReg5_0;
    writeConfigReg5_0.field.u1SDRDDR = 0; // SDR enabled                // Note!!
    writeConfigReg5_0.field.u1OPI_IT = 0; // Legacy Single SPI Protocol // Note!!
    WriteRegisterSequence(CY_SMIF_SLAVE_SELECT_0,
                          CY_SEMP_REG_ADDR_CFR5_V,
                          writeConfigReg5_0.u8,
                          TEST_ADDR_LEN,
                          TEST_READ_REG_LC,
                          initialTransMode);


    cy_en_smif_trx_type_t transMode = CY_SPI_TRANSACTION_1S1S1S;

    un_cy_smif_semp_CFR5_t readConfigReg5_0 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_0, 
                                 &readConfigReg5_0.u8, 
                                 CY_SEMP_REG_ADDR_CFR5_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 transMode,
                                 &smifContext);
    CY_ASSERT(writeConfigReg5_0.field.u1SDRDDR == readConfigReg5_0.field.u1SDRDDR);
    CY_ASSERT(writeConfigReg5_0.field.u1OPI_IT == readConfigReg5_0.field.u1OPI_IT);

    /*********************************************************/
    /*********  Erase/Write/Read/Verify in MMIO mode   *******/
    /*********************************************************/

    // erase
    Cy_SMIF_SEMP_WriteEnable(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, transMode, &smifContext);
    Cy_SMIF_SEMP_Erase_256KB_Sector(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, TEST_PROGRAM_SECTOR, transMode, &smifContext);
    CY_ASSERT(WaitDeviceBecomesReady(CY_SMIF_SLAVE_SELECT_0, TEST_READ_REG_LC, transMode) == 0);

    // write
    for(uint32_t i_AddrOffset = 0; i_AddrOffset < TEST_PROGRAM_SIZE; i_AddrOffset += DEVICE_PROGRAM_BUFFER_SIZE)
    {
        Cy_SMIF_SEMP_WriteEnable(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, transMode, &smifContext);
        Cy_SMIF_MMIO_Program_ExMem(SMIF_USED,
                                   CY_SMIF_SLAVE_SELECT_0,
                                   CY_SMIF_SEMP_WTID_1S1S1S_4ADR,
                                   (TEST_PROGRAM_SECTOR + i_AddrOffset),
                                   DEVICE_PROGRAM_BUFFER_SIZE,
                                   (uint8_t*)&programData[i_AddrOffset],
                                   CY_SMIF_BLOCKING,
                                   TEST_WRITE_LC,
                                   0, /* mode value */
                                   &smifDevice0Context,
                                   &smifContext);
        CY_ASSERT(WaitDeviceBecomesReady(CY_SMIF_SLAVE_SELECT_0, TEST_READ_REG_LC, transMode) == 0);
    }

    uint16_t readSPICommand;
    uint16_t readFastSPICommand;
    if(TEST_ADDR_LEN == CY_TRX_ADDR_4BYTE)
    {
        readSPICommand     = CY_SMIF_SEMP_RDID_1S1S1S_4ADR;
        readFastSPICommand = CY_SMIF_SEMP_RDID_1S1S1S_FAST_4ADR;
        smifDev0Cfg.size   = CY_SMIF_DEVICE_64M_BYTE;
    }
    else
    {
        readSPICommand     = CY_SMIF_SEMP_RDID_1S1S1S_3ADR;
        readFastSPICommand = CY_SMIF_SEMP_RDID_1S1S1S_FAST_3ADR;
        smifDev0Cfg.size   = CY_SMIF_DEVICE_8M_BYTE;
    }

    // read with read command
    memset(readDataBuffer, 0, TEST_PROGRAM_SIZE);
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, 
                            CY_SMIF_SLAVE_SELECT_0, 
                            readSPICommand,
                            TEST_PROGRAM_SECTOR, 
                            TEST_PROGRAM_SIZE, 
                            (uint8_t*)readDataBuffer, 
                            TEST_READ_LC, 
                            0, /* mode value */
                            CY_SMIF_BLOCKING, 
                            &smifDevice0Context, 
                            &smifContext);

    // verify
    for(uint32_t i = 0; i < TEST_PROGRAM_SIZE; i++)
    {
        if(programData[i] != readDataBuffer[i])
        {
            while(1); // read/write value mismatch.
        }
    }

    // read with fast read command
    memset(readDataBuffer, 0, TEST_PROGRAM_SIZE);
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, 
                            CY_SMIF_SLAVE_SELECT_0, 
                            readFastSPICommand, 
                            TEST_PROGRAM_SECTOR, 
                            TEST_PROGRAM_SIZE, 
                            (uint8_t*)readDataBuffer, 
                            TEST_READ_LC, 
                            0, /* mode value */
                            CY_SMIF_BLOCKING, 
                            &smifDevice0Context, 
                            &smifContext);

    // verify
    for(uint32_t i = 0; i < TEST_PROGRAM_SIZE; i++)
    {
        if(programData[i] != readDataBuffer[i])
        {
            while(1); // read/write value mismatch.
        }
    }

    /**************************************************/
    /*********  Write/Read/Verify in XIP mode   *******/
    /**************************************************/

    // erase in MMIO mode
    Cy_SMIF_SEMP_WriteEnable(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, transMode, &smifContext);
    Cy_SMIF_SEMP_Erase_256KB_Sector(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, TEST_PROGRAM_SECTOR, transMode, &smifContext);
    CY_ASSERT(WaitDeviceBecomesReady(CY_SMIF_SLAVE_SELECT_0, TEST_READ_REG_LC, transMode) == 0);

    // set up for XIP mode with read command
    smifDev0Cfg.addr = CY_SMIF_GetXIP_Address(SMIF_USED);
    smifDev0Cfg.xipReadCmdId = readSPICommand;
    CY_ASSERT(Cy_SMIF_InitXIPModeForExMem(SMIF_DEVICE0, CY_SMIF_DEV_SINGLE_BIT_0To1, &smifDev0Cfg, &smifDevice0Context) == CY_SMIF_SUCCESS);

    // write
    uint8_t* pbaseAddrXIP = (uint8_t*)smifDev0Cfg.addr;
    for(uint32_t i_AddrOffset = 0; i_AddrOffset < TEST_PROGRAM_SIZE; i_AddrOffset += DEVICE_PROGRAM_BUFFER_SIZE)
    {
        SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);
        Cy_SMIF_SEMP_WriteEnable(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, transMode, &smifContext);

        SetModeWithBusyCheck(SMIF_USED, CY_SMIF_MEMORY);
        // Make sure merge mode enabled
        for(uint32_t i = i_AddrOffset; i < (i_AddrOffset + DEVICE_PROGRAM_BUFFER_SIZE); i++)
        {
            pbaseAddrXIP[TEST_PROGRAM_SECTOR + i] = programData[i];
        }

        SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);
        CY_ASSERT(WaitDeviceBecomesReady(CY_SMIF_SLAVE_SELECT_0, TEST_READ_REG_LC, transMode) == 0);
    }
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_MEMORY);

    // verify
    for(uint32_t i = 0; i < TEST_PROGRAM_SIZE; i++)
    {
        if(programData[i] != pbaseAddrXIP[TEST_PROGRAM_SECTOR+i])
        {
            while(1); // read/write value mismatch.
        }
    }

    // set up for XIP mode with fast read command
    smifDev0Cfg.addr = CY_SMIF_GetXIP_Address(SMIF_USED);
    smifDev0Cfg.xipReadCmdId = readFastSPICommand;
    CY_ASSERT(Cy_SMIF_InitXIPModeForExMem(SMIF_DEVICE0, CY_SMIF_DEV_SINGLE_BIT_0To1, &smifDev0Cfg, &smifDevice0Context) == CY_SMIF_SUCCESS);
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_MEMORY);

    // verify
    pbaseAddrXIP = (uint8_t*)smifDev0Cfg.addr;
    for(uint32_t i = 0; i < TEST_PROGRAM_SIZE; i++)
    {
        if(programData[i] != pbaseAddrXIP[TEST_PROGRAM_SECTOR+i])
        {
            while(1); // read/write value mismatch.
        }
    }


    // Reaching here means this example worked properly.
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

static void OffsetLatencyMismatchInReadTx(uint8_t* iData, uint8_t* oData, uint32_t size, uint8_t mismatchCount)
{
    uint8_t mask = ~(1 << (8-mismatchCount) - 1);
    for(uint32_t i = 0; i < size; i += 1)
    {
        oData[i] = (iData[i] << mismatchCount) + ((iData[i+1] & mask) >> (8 - mismatchCount));
    }
}

#define S28H_MANUFACTURE_ID (0x34)
#define S28H_DEVICE_ID_MSB  (0x5B)
#define S28H_DEVICE_ID_LSB  (0x1A)
#define S28H_ID_LENGTH      (0x0F)
#define S28H_PHY_SECTOR_ARC (0x03)
#define S28H_FAMILY_ID      (0x90)
const cy_stc_smif_semp_id_t gManufactureId = {S28H_MANUFACTURE_ID, S28H_DEVICE_ID_MSB, S28H_DEVICE_ID_LSB, S28H_ID_LENGTH, S28H_PHY_SECTOR_ARC, S28H_FAMILY_ID};
// This function has a side effect that this function will set address length to 4 byte anyway.
static void DetectVolatileTrModeAndRegLantency(cy_en_smif_slave_select_t slave, cy_en_smif_trx_type_t* detectTrMode, cy_en_smif_semp_reg_latency_code_t* detectRegLatency)
{
    uint8_t readID_1S[7];  // sizeof(cy_stc_smif_semp_id_t) + 1
    uint8_t readID_8S[9];  // sizeof(cy_stc_smif_semp_id_t) + 3
    uint8_t readID_8D[12]; // sizeof(cy_stc_smif_semp_id_t) + 3*2
    Cy_SMIF_SEMP_ReadId(SMIF_USED, slave, (void*)readID_1S, CY_SMIF_SEMP_REG_LATENCY0, CY_SPI_TRANSACTION_1S1S1S, sizeof(readID_1S), &smifContext);
    Cy_SMIF_SEMP_ReadId(SMIF_USED, slave, (void*)readID_8S, CY_SMIF_SEMP_REG_LATENCY0, CY_SPI_TRANSACTION_8S8S8S, sizeof(readID_8S), &smifContext);
    Cy_SMIF_SEMP_ReadId(SMIF_USED, slave, (void*)readID_8D, CY_SMIF_SEMP_REG_LATENCY0, CY_SPI_TRANSACTION_8D8D8D, sizeof(readID_8D), &smifContext);

    uint8_t readID_1S_0[6];
    uint8_t readID_1S_1[6];
    OffsetLatencyMismatchInReadTx(readID_1S, readID_1S_0, sizeof(readID_1S_0), 1);
    OffsetLatencyMismatchInReadTx(readID_1S, readID_1S_1, sizeof(readID_1S_1), 2);

    if(memcmp(&readID_1S[0], &gManufactureId, sizeof(cy_stc_smif_semp_id_t)) == 0)
    {
        *detectTrMode     = CY_SPI_TRANSACTION_1S1S1S;
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY0; // or CY_SMIF_SEMP_REG_LATENCY1. will decide below.
    }
    else if(memcmp(&readID_1S_0[0], &gManufactureId, sizeof(cy_stc_smif_semp_id_t)) == 0)
    {
        *detectTrMode     = CY_SPI_TRANSACTION_1S1S1S;
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY2;
    }
    else if(memcmp(&readID_1S_1[0], &gManufactureId, sizeof(cy_stc_smif_semp_id_t)) == 0)
    {
        *detectTrMode     = CY_SPI_TRANSACTION_1S1S1S;
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY3;
    }
    else if(memcmp(&readID_8S[0], &gManufactureId, sizeof(cy_stc_smif_semp_id_t)) == 0)
    {
        *detectTrMode     = CY_SPI_TRANSACTION_8S8S8S;
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY0;
    }
    else if(memcmp(&readID_8S[1], &gManufactureId, sizeof(cy_stc_smif_semp_id_t)) == 0)
    {
        *detectTrMode     = CY_SPI_TRANSACTION_8S8S8S;
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY1;
    }
    else if(memcmp(&readID_8S[2], &gManufactureId, sizeof(cy_stc_smif_semp_id_t)) == 0)
    {
        *detectTrMode     = CY_SPI_TRANSACTION_8S8S8S;
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY2;
    }
    else if(memcmp(&readID_8S[3], &gManufactureId, sizeof(cy_stc_smif_semp_id_t)) == 0)
    {
        *detectTrMode     = CY_SPI_TRANSACTION_8S8S8S;
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY3;
    }
    else if(memcmp(&readID_8D[0], &gManufactureId, sizeof(cy_stc_smif_semp_id_t)) == 0)
    {
        *detectTrMode     = CY_SPI_TRANSACTION_8D8D8D;
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY0;
    }
    else if(memcmp(&readID_8D[2], &gManufactureId, sizeof(cy_stc_smif_semp_id_t)) == 0)
    {
        *detectTrMode     = CY_SPI_TRANSACTION_8D8D8D;
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY1;
    }
    else if(memcmp(&readID_8D[4], &gManufactureId, sizeof(cy_stc_smif_semp_id_t)) == 0)
    {
        *detectTrMode     = CY_SPI_TRANSACTION_8D8D8D;
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY2;
    }
    else if(memcmp(&readID_8D[6], &gManufactureId, sizeof(cy_stc_smif_semp_id_t)) == 0)
    {
        *detectTrMode     = CY_SPI_TRANSACTION_8D8D8D;
        *detectRegLatency = CY_SMIF_SEMP_REG_LATENCY3;
    }
    else
    {
        // mode was not detected.
        CY_ASSERT(false);
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
                          *detectTrMode);

    WriteRegisterSequence(slave,
                          CY_SEMP_REG_ADDR_CFR2_V,
                          presetConfigReg2_0.u8,
                          CY_TRX_ADDR_3BYTE,
                          *detectRegLatency,
                          *detectTrMode);

    if((*detectTrMode  == CY_SPI_TRANSACTION_1S1S1S) && (*detectRegLatency == CY_SMIF_SEMP_REG_LATENCY0))
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
            return;
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
}


/* [] END OF FILE */
