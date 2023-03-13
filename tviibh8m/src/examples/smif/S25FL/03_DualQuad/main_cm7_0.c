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

/***** User Modifiable definitions *****/
#define DLP_VALUE (0x34u)
#define SMIF_CLK_FREQ_SETUP      (32000000UL) // Please modify as per your HW
#define SMIF_CLK_FREQ_SDR_RD     (32000000UL) // Please modify as per your HW
#define SMIF_CLK_FREQ_DDR_RD     (32000000UL) // Please modify as per your HW
#if (CY_SMIF_QUAD_MEMORY == CY_SMIF_S25FXXXS)
  #define TEST_SPI_READ_LC CY_SMIF_S25FL_RD_LATENCY2
  #define DEVICE_SECTOR_ERASE      (0x00010000) // 64KB
#elif (CY_SMIF_QUAD_MEMORY == CY_SMIF_S25FXXXL)
  #define TEST_SPI_READ_LC CY_SMIF_S25FL_RD_LATENCY8
  #define DEVICE_SECTOR_ERASE      (0x00001000) // 4KB
#endif
#define DEVICE_PROGRAM_BUFFER_SIZE (0x100)      // 256B
#define TEST_SPI_WRITE_LC          CY_SMIF_S25FL_WT_LATENCY0
#define TEST_PROGRAM_SECTOR        (0x01240000ul)
/**************************************/

#define TEST_DQUAD_XIP_PROGRAM_ADDR (TEST_PROGRAM_SECTOR * 2ul)
#define DQUAD_PROGRAM_BUFFER_SIZE   (DEVICE_PROGRAM_BUFFER_SIZE * 2ul)
#define DQUAD_SECTOR_ERASE          (DEVICE_SECTOR_ERASE * 2ul)

#define D_QUAD_SLAVES (cy_en_smif_slave_select_t)((uint8_t)CY_SMIF_SLAVE_SELECT_0|(uint8_t)CY_SMIF_SLAVE_SELECT_1)

/**
    TEST_PROGRAM_SECTOR is the address recognized by External Quad Devices.
    TEST_DQUAD_XIP_PROGRAM_ADDR is the address recognized by TVII in XIP, Dual Quad mode.
**/

// #define DLP_SETTING_UPDATE_AUTO

#ifdef DLP_SETTING_UPDATE_AUTO
  #define SETTING_DELAY_SEL CY_SMIF_1_NEW_SEL_PER_TAP
  #define SETTING_DLP_AUTO  CY_SMIF_DLP_UPDATE_AUTO
#else
  #define SETTING_DELAY_SEL CY_SMIF_NO_DELAY_SEL
  #define SETTING_DLP_AUTO  CY_SMIF_DLP_UPDATE_MANUAL
#endif

#define USE_QUAD_MODE 1 // 0: not quad, 1: quad

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
uint8_t readDataBuffer[TEST_PROGRAM_SIZE];

cy_stc_smif_config_t smifConfig =
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

cy_stc_device_config_t smifDevDualQuadCfg =
{
    .xipReadCmdId       = CY_SMIF_S25FL_ID_4QOR,
    .xipReadMode        = CY_SMIF_S25FL_RW_NON_CONTINUOUS,
    .writeEn            = true,
    .xipWriteCmdId      = CY_SMIF_S25FL_ID_4QPP,
    .xipWriteMode       = 0u,                      // Do not care
    .mergeEnable        = true,
    .mergeTimeout       = CY_SMIF_MER_TIMEOUT_4096_CYCLE,
    .totalTimeoutEnable = false,
    .totalTimeout       = 1000u,
    .addr               = 0,                       // will be updated in the application
    .size               = CY_SMIF_DEVICE_64M_BYTE, // 32Mbyte * 2(device)
    .rdLatencyCode      = TEST_SPI_READ_LC,
    .wtLatencyCode      = 0,                       // Do not care
};


static void TestFor_DQUAD_MMIO_WRITE_Then_MMIO_READ(uint16_t rd_cmdId);
static void TestFor_DQUAD_XIP_WRITE_XIP_READ(void);
static void TestFor_DQUAD_MMIO_WRITE_Then_XIP_CONTINUOUS_READ(uint16_t rd_cmdId);
static void TestFor_DQUAD_MMIO_WRITE_Then_MMIO_DDR_READ(uint16_t rd_cmdId);
static void TestFor_DQUAD_MMIO_WRITE_Then_XIP_DDR_CONTINUOUS_READ(uint16_t rd_cmdId);

static void SectorEraseProcedure(cy_en_smif_slave_select_t slave, uint32_t address);
static bool WaitDeviceBecomesReady(cy_en_smif_slave_select_t slave);
static void SetModeWithBusyCheck(volatile cy_stc_smif_reg_t *base, cy_en_smif_mode_t mode);

cy_stc_ex_dev_context_t smifDeviceContext;

/****************************/
/*** Cortex MPU Utilities ***/
/****************************/
#define BG_REGION_ADDR      (0x00000000UL)
#define SRAM_REGION_ADDR    (0x28000000UL)
#define FLASH_REGION_ADDR   (0x10000000UL)
#define SMIF0_DEV_XIP_ADDR  (0x60000000UL)
#define SMIF1_DEV_XIP_ADDR  (0x80000000UL) // Only For Cluster MCU
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

    ChangePLLFrequency(SMIF_CLK_FREQ_SETUP*2);

    SmifPortInit(smifPortCfg, SIZE_OF_PORT);

    CY_ASSERT(Cy_SMIF_InitExMemContext(CY_SMIF_S25FL, &smifDeviceContext) == CY_SMIF_SUCCESS);

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
    Cy_SMIF_DeviceSetDataSelect(SMIF_DEVICE1, CY_SMIF_DATA_SEL2);
    Cy_SMIF_DeviceWriteEnable(SMIF_DEVICE1);

    /********************/
    /* 3. Enable SMIF 0 */
    /********************/
    Cy_SMIF_Enable(SMIF_USED, &smifContext);

#if (CY_SMIF_QUAD_MEMORY == CY_SMIF_S25FXXXS)
    /***********************************/
    /* 4. Setting for external devices */
    /***********************************/
    // 4.1 Send Rest command
    Cy_SMIF_S25FL_SoftwareReset(SMIF_USED, D_QUAD_SLAVES, &smifContext);

    // wait at least 32us
    Cy_SysTick_DelayInUs(32ul);

    /*********************************************************/
    /********* Read Status and Configuration registers *******/
    /*********************************************************/
    // 4.2 Read Configuration register 1 and status register 1
    // Read current configuration register 1 and status register 1
    volatile un_cy_smif_s25fl128_CR1_t prevConfigReg0 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR1_t prevConfigReg1 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_SR1_t prevStatusReg0 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_SR1_t prevStatusReg1 = { .u8 = 0u };

    Cy_SMIF_S25FL_ReadConfigurationRegister(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, (uint8_t*)&prevConfigReg0, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, (uint8_t*)&prevConfigReg1, &smifContext);
    Cy_SMIF_S25FL_ReadStatusRegister1(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, (uint8_t*)&prevStatusReg0, &smifContext);
    Cy_SMIF_S25FL_ReadStatusRegister1(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, (uint8_t*)&prevStatusReg1, &smifContext);

    /*********************************************************/
    /******* Modify Status and Configuration registers *******/
    /*********************************************************/
    // Modify QUAD and LC field to be set
    prevConfigReg0.field.u1QUAD = USE_QUAD_MODE;
    prevConfigReg0.field.u2LC   = TEST_SPI_READ_LC;
    prevConfigReg1.field.u1QUAD = USE_QUAD_MODE;
    prevConfigReg1.field.u2LC   = TEST_SPI_READ_LC;
    prevStatusReg0.field.u3BP   = 0u; // no protection
    prevStatusReg1.field.u3BP   = 0u; // no protection

    /*********************************************************/
    /******* Write Status and Configuration registers ********/
    /*********************************************************/
    // Send Write Enable command
    Cy_SMIF_S25FL_WriteEnable(SMIF_USED, D_QUAD_SLAVES, &smifContext);

    // Write Registers
    stc_cy_smif_s25f_write_reg_t writeRegValue_0;
    writeRegValue_0.stsReg1 = prevStatusReg0.u8;
    writeRegValue_0.confgReg1 = prevConfigReg0.u8;

    stc_cy_smif_s25f_write_reg_t writeRegValue_1;
    writeRegValue_1.stsReg1 = prevStatusReg1.u8;
    writeRegValue_1.confgReg1 = prevConfigReg1.u8;

    // Write Registers
    Cy_SMIF_S25FL_WriteStatusAndConfigRegister(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, &writeRegValue_0, 2, &smifContext);
    Cy_SMIF_S25FL_WriteStatusAndConfigRegister(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, &writeRegValue_1, 2, &smifContext);

    // Wait writing registers complete
    CY_ASSERT_L1(WaitDeviceBecomesReady(D_QUAD_SLAVES) == 0u);

    /*********************************************************/
    /******* Verify Status and Configuration registers *******/
    /*********************************************************/
    // Check the config register have been writen properly
    volatile un_cy_smif_s25fl128_CR1_t readConfigReg0 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR1_t readConfigReg1 = { .u8 = 0u };
    Cy_SMIF_S25FL_ReadConfigurationRegister(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, (uint8_t*)&readConfigReg0, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, (uint8_t*)&readConfigReg1, &smifContext);
    CY_ASSERT_L1(readConfigReg0.field.u1QUAD == USE_QUAD_MODE);
    CY_ASSERT_L1(readConfigReg0.field.u2LC == TEST_SPI_READ_LC);
    CY_ASSERT_L1(readConfigReg1.field.u1QUAD == USE_QUAD_MODE);
    CY_ASSERT_L1(readConfigReg1.field.u2LC == TEST_SPI_READ_LC);
#elif (CY_SMIF_QUAD_MEMORY == CY_SMIF_S25FXXXL)
    // 4.1 Send Rest command
    // YOTS: Not implemented yet.

    /*********************************************************/
    /********* Read Status and Configuration registers *******/
    /*********************************************************/
    // 4.2 Read Configuration register 1 and status register 1
    // Read current configuration register 1 and status register 1
    volatile un_cy_smif_s25fl128_SR1_t prevStatusReg1_0 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR1_t prevConfigReg1_0 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR2_t prevConfigReg2_0 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR3_t prevConfigReg3_0 = { .u8 = 0u };

    volatile un_cy_smif_s25fl128_SR1_t prevStatusReg1_1 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR1_t prevConfigReg1_1 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR2_t prevConfigReg2_1 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR3_t prevConfigReg3_1 = { .u8 = 0u };

    Cy_SMIF_S25FL_ReadStatusRegister1(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, (uint8_t*)&prevStatusReg1_0, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, (uint8_t*)&prevConfigReg1_0, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister2(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, (uint8_t*)&prevConfigReg2_0, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister3(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, (uint8_t*)&prevConfigReg3_0, &smifContext);

    Cy_SMIF_S25FL_ReadStatusRegister1(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, (uint8_t*)&prevStatusReg1_1, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, (uint8_t*)&prevConfigReg1_1, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister2(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, (uint8_t*)&prevConfigReg2_1, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister3(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, (uint8_t*)&prevConfigReg3_1, &smifContext);

    /*********************************************************/
    /******* Modify Status and Configuration registers *******/
    /*********************************************************/
    prevStatusReg1_0.u8 = 0;
    prevConfigReg1_0.field.u1QUAD = USE_QUAD_MODE;
    prevConfigReg1_0.field.u1CMP  = 0u;
    prevConfigReg2_0.field.u1QPI  = 0u; // Quad instruction disabled
    prevConfigReg3_0.field.u4RL   = TEST_SPI_READ_LC; // Latency code
    prevConfigReg3_0.field.u1WE   = 1u; // Wrap Disabled

    prevStatusReg1_1.u8 = 0;
    prevConfigReg1_1.field.u1QUAD = USE_QUAD_MODE;
    prevConfigReg1_1.field.u1CMP  = 0u;
    prevConfigReg2_1.field.u1QPI  = 0u; // Quad instruction disabled
    prevConfigReg3_1.field.u4RL   = TEST_SPI_READ_LC; // Latency code
    prevConfigReg3_1.field.u1WE   = 1u; // Wrap Disabled

    /*********************************************************/
    /******* Write Status and Configuration registers ********/
    /*********************************************************/
    // Send Write Enable command
    Cy_SMIF_S25FL_WriteEnable(SMIF_USED, D_QUAD_SLAVES, &smifContext);

    stc_cy_smif_s25f_write_reg_t writeRegValue_0;
    writeRegValue_0.stsReg1 = prevStatusReg1_0.u8;
    writeRegValue_0.confgReg1 = prevConfigReg1_0.u8;
    writeRegValue_0.confgReg2 = prevConfigReg2_0.u8;
    writeRegValue_0.confgReg3 = prevConfigReg3_0.u8;

    stc_cy_smif_s25f_write_reg_t writeRegValue_1;
    writeRegValue_1.stsReg1 = prevStatusReg1_1.u8;
    writeRegValue_1.confgReg1 = prevConfigReg1_1.u8;
    writeRegValue_1.confgReg2 = prevConfigReg2_1.u8;
    writeRegValue_1.confgReg3 = prevConfigReg3_1.u8;

    // Write Registers
    Cy_SMIF_S25FL_WriteStatusAndConfigRegister(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, &writeRegValue_0, 4, &smifContext);
    Cy_SMIF_S25FL_WriteStatusAndConfigRegister(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, &writeRegValue_1, 4, &smifContext);

    // Wait writing registers complete
    CY_ASSERT_L1(WaitDeviceBecomesReady(D_QUAD_SLAVES) == 0u);

    /*********************************************************/
    /******* Verify Status and Configuration registers *******/
    /*********************************************************/
    // Check the config register have been written properly
    volatile un_cy_smif_s25fl128_SR1_t readStatusReg1_0 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR1_t readConfigReg1_0 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR2_t readConfigReg2_0 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR3_t readConfigReg3_0 = { .u8 = 0u };
    Cy_SMIF_S25FL_ReadStatusRegister1(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, (uint8_t*)&readStatusReg1_0, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, (uint8_t*)&readConfigReg1_0, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister2(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, (uint8_t*)&readConfigReg2_0, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister3(SMIF_USED, CY_SMIF_SLAVE_SELECT_0, (uint8_t*)&readConfigReg3_0, &smifContext);

    CY_ASSERT_L1(readStatusReg1_0.u8 == 0u);
    CY_ASSERT_L1(readConfigReg1_0.field.u1QUAD == USE_QUAD_MODE);
    CY_ASSERT_L1(readConfigReg1_0.field.u1CMP == 0u);
    CY_ASSERT_L1(readConfigReg2_0.field.u1QPI == 0u);
    CY_ASSERT_L1(readConfigReg3_0.field.u4RL == TEST_SPI_READ_LC);
    CY_ASSERT_L1(readConfigReg3_0.field.u1WE == 1u);

    volatile un_cy_smif_s25fl128_SR1_t readStatusReg1_1 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR1_t readConfigReg1_1 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR2_t readConfigReg2_1 = { .u8 = 0u };
    volatile un_cy_smif_s25fl128_CR3_t readConfigReg3_1 = { .u8 = 0u };
    Cy_SMIF_S25FL_ReadStatusRegister1(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, (uint8_t*)&readStatusReg1_1, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, (uint8_t*)&readConfigReg1_1, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister2(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, (uint8_t*)&readConfigReg2_1, &smifContext);
    Cy_SMIF_S25FL_ReadConfigurationRegister3(SMIF_USED, CY_SMIF_SLAVE_SELECT_1, (uint8_t*)&readConfigReg3_1, &smifContext);

    CY_ASSERT_L1(readStatusReg1_1.u8 == 0u);
    CY_ASSERT_L1(readConfigReg1_1.field.u1QUAD == USE_QUAD_MODE);
    CY_ASSERT_L1(readConfigReg1_1.field.u1CMP == 0u);
    CY_ASSERT_L1(readConfigReg2_1.field.u1QPI == 0u);
    CY_ASSERT_L1(readConfigReg3_1.field.u4RL == TEST_SPI_READ_LC);
    CY_ASSERT_L1(readConfigReg3_1.field.u1WE == 1u);
#endif

    // 4.4 Set DLP
    uint8_t dlp = DLP_VALUE;
    Cy_SMIF_S25FL_WriteVDLR(SMIF_USED, D_QUAD_SLAVES, dlp, &smifContext);

    smifDevDualQuadCfg.addr = CY_SMIF_GetXIP_Address(SMIF_USED);
    /*********************************************************/
    /******** Write/Read Test (Quad Output Read) *************/
    /*********************************************************/
    ChangePLLFrequency(SMIF_CLK_FREQ_SDR_RD*2);

    TestFor_DQUAD_MMIO_WRITE_Then_MMIO_READ(CY_SMIF_S25FL_ID_4QOR);

    smifDevDualQuadCfg.xipReadCmdId = CY_SMIF_S25FL_ID_4QOR;
    Cy_SMIF_InitXIPModeForExMem(SMIF_DEVICE0, CY_SMIF_DEV_D_QUAD_BIT_0To3, &smifDevDualQuadCfg, &smifDeviceContext);
    Cy_SMIF_InitXIPModeForExMem(SMIF_DEVICE1, CY_SMIF_DEV_D_QUAD_BIT_4To7, &smifDevDualQuadCfg, &smifDeviceContext);
    TestFor_DQUAD_XIP_WRITE_XIP_READ();

    /********************************************************/
    /********* Write/Read Test (Quad I/O Read) **************/
    /********************************************************/
    TestFor_DQUAD_MMIO_WRITE_Then_MMIO_READ(CY_SMIF_S25FL_ID_4QIOR);

    smifDevDualQuadCfg.xipReadCmdId = CY_SMIF_S25FL_ID_4QIOR_CONT;
    smifDevDualQuadCfg.xipReadMode  = CY_SMIF_S25FL_RW_CONTINUOUS;
    Cy_SMIF_InitXIPModeForExMem(SMIF_DEVICE0, CY_SMIF_DEV_D_QUAD_BIT_0To3, &smifDevDualQuadCfg, &smifDeviceContext);
    Cy_SMIF_InitXIPModeForExMem(SMIF_DEVICE1, CY_SMIF_DEV_D_QUAD_BIT_4To7, &smifDevDualQuadCfg, &smifDeviceContext);
    TestFor_DQUAD_MMIO_WRITE_Then_XIP_CONTINUOUS_READ(CY_SMIF_S25FL_ID_4QIOR);

    /********************************************************/
    /******* Write/Read Test (DDR Quad I/O Read) ************/
    /********************************************************/
    ChangePLLFrequency(SMIF_CLK_FREQ_DDR_RD*2);

    smifConfig.rxClk = CY_SMIF_INV_FEEDBACK_CLK; // !!!! Try CY_SMIF_INV_OUTPUT_CLK if it was not working. !!!!
    Cy_SMIF_Init(SMIF_USED, &smifConfig, 1000, &smifContext);

    TestFor_DQUAD_MMIO_WRITE_Then_MMIO_DDR_READ(CY_SMIF_S25FL_ID_4DDRQIOR);

    smifDevDualQuadCfg.xipReadCmdId = CY_SMIF_S25FL_ID_4DDRQIOR_CONT;
    smifDevDualQuadCfg.xipReadMode  = CY_SMIF_S25FL_RW_CONTINUOUS;
    Cy_SMIF_InitXIPModeForExMem(SMIF_DEVICE0, CY_SMIF_DEV_D_QUAD_BIT_0To3, &smifDevDualQuadCfg, &smifDeviceContext);
    Cy_SMIF_InitXIPModeForExMem(SMIF_DEVICE1, CY_SMIF_DEV_D_QUAD_BIT_4To7, &smifDevDualQuadCfg, &smifDeviceContext);
    TestFor_DQUAD_MMIO_WRITE_Then_XIP_DDR_CONTINUOUS_READ(CY_SMIF_S25FL_ID_4DDRQIOR);

    // Reaching here means this example worked properly.
    for(;;);
}

uint8_t swappedProgramData[TEST_PROGRAM_SIZE];

static void TestFor_DQUAD_MMIO_WRITE_Then_MMIO_READ(uint16_t rd_cmdId)
{
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);
    for(uint32_t i_EraseSectorTopAddr = TEST_PROGRAM_SECTOR; i_EraseSectorTopAddr < (TEST_PROGRAM_SECTOR+TEST_PROGRAM_SIZE); i_EraseSectorTopAddr+= DQUAD_SECTOR_ERASE)
    {
        SectorEraseProcedure(D_QUAD_SLAVES, ((i_EraseSectorTopAddr + TEST_PROGRAM_SECTOR) / 2));
    }

    /*  Program 64K byte data */
    volatile uint32_t programAddress    = TEST_PROGRAM_SECTOR;
    volatile uint32_t programSizeInByte = TEST_PROGRAM_SIZE;

    for(uint32_t i_WriteOffsetAddrOfQuadDevice = 0; i_WriteOffsetAddrOfQuadDevice < (programSizeInByte / 2); i_WriteOffsetAddrOfQuadDevice+=DEVICE_PROGRAM_BUFFER_SIZE)
    {
        uint32_t writeOffsetAddrOfMCU = i_WriteOffsetAddrOfQuadDevice * 2ul;
        uint32_t writeStartAddress = i_WriteOffsetAddrOfQuadDevice + programAddress;

        // Write enable
        Cy_SMIF_S25FL_WriteEnable(SMIF_USED, D_QUAD_SLAVES, &smifContext);

        // Issue program
        Cy_SMIF_MMIO_Program_ExMem(SMIF_USED, D_QUAD_SLAVES, CY_SMIF_S25FL_ID_4QPP, writeStartAddress, DQUAD_PROGRAM_BUFFER_SIZE, (uint8_t*)&programData[writeOffsetAddrOfMCU], CY_SMIF_BLOCKING, TEST_SPI_WRITE_LC, 0, &smifDeviceContext, &smifContext);

        // Wait for completion
        CY_ASSERT_L1(WaitDeviceBecomesReady(D_QUAD_SLAVES) == 0u);
    }

    /* Read 64K byte data */
    volatile uint32_t readAddress    = TEST_PROGRAM_SECTOR;
    volatile uint32_t readSizeInByte = TEST_PROGRAM_SIZE;

    // Read the programmed data
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, D_QUAD_SLAVES, rd_cmdId, readAddress, readSizeInByte, (uint8_t*)readDataBuffer, TEST_SPI_READ_LC, CY_SMIF_S25FL_RW_NON_CONTINUOUS, CY_SMIF_BLOCKING, &smifDeviceContext, &smifContext);

    // Check read data
    CY_ASSERT_L1((memcmp(readDataBuffer, programData, TEST_PROGRAM_SIZE) == 0));
}

static void TestFor_DQUAD_XIP_WRITE_XIP_READ(void)
{
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);
    for(uint32_t i_EraseSectorTopAddr = TEST_PROGRAM_SECTOR; i_EraseSectorTopAddr < (TEST_PROGRAM_SECTOR+TEST_PROGRAM_SIZE); i_EraseSectorTopAddr+= DQUAD_SECTOR_ERASE)
    {
        SectorEraseProcedure(D_QUAD_SLAVES, ((i_EraseSectorTopAddr + TEST_PROGRAM_SECTOR) / 2));
    }

    // Issue program
    const uint32_t writeBaseAddress = CY_SMIF_GetXIP_Address(SMIF_USED) + TEST_DQUAD_XIP_PROGRAM_ADDR;
    for(uint32_t i_WriteOffsetAddress = 0; i_WriteOffsetAddress < TEST_PROGRAM_SIZE; i_WriteOffsetAddress += DQUAD_PROGRAM_BUFFER_SIZE)
    {
        // Write enable
        SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);
        Cy_SMIF_S25FL_WriteEnable(SMIF_USED, D_QUAD_SLAVES, &smifContext);
        SetModeWithBusyCheck(SMIF_USED, CY_SMIF_MEMORY);

        memcpy((void*)(writeBaseAddress + i_WriteOffsetAddress), (void*)&programData[i_WriteOffsetAddress], DQUAD_PROGRAM_BUFFER_SIZE);

        SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);
        CY_ASSERT_L1(WaitDeviceBecomesReady(D_QUAD_SLAVES) == 0u);
        SetModeWithBusyCheck(SMIF_USED, CY_SMIF_MEMORY);
    }

    // Read the programmed data
    memcpy(readDataBuffer, (void*)writeBaseAddress, TEST_PROGRAM_SIZE);

    // Check read data
    CY_ASSERT_L1((memcmp(readDataBuffer, programData, TEST_PROGRAM_SIZE) == 0));
}

static void TestFor_DQUAD_MMIO_WRITE_Then_XIP_CONTINUOUS_READ(uint16_t rd_cmdId)
{
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);
    for(uint32_t i_EraseSectorTopAddr = TEST_PROGRAM_SECTOR; i_EraseSectorTopAddr < (TEST_PROGRAM_SECTOR+TEST_PROGRAM_SIZE); i_EraseSectorTopAddr+= DQUAD_SECTOR_ERASE)
    {
        SectorEraseProcedure(D_QUAD_SLAVES, ((i_EraseSectorTopAddr + TEST_PROGRAM_SECTOR) / 2));
    }

    /* Program 64K byte data */
    volatile uint32_t programAddress    = TEST_PROGRAM_SECTOR;
    volatile uint32_t programSizeInByte = TEST_PROGRAM_SIZE;

    for(uint32_t i_WriteOffsetAddrOfQuadDevice = 0; i_WriteOffsetAddrOfQuadDevice < (programSizeInByte / 2); i_WriteOffsetAddrOfQuadDevice+=DEVICE_PROGRAM_BUFFER_SIZE)
    {
        uint32_t writeOffsetAddrOfMCU = i_WriteOffsetAddrOfQuadDevice * 2ul;
        uint32_t writeStartAddress = i_WriteOffsetAddrOfQuadDevice + programAddress;

        // Write enable
        Cy_SMIF_S25FL_WriteEnable(SMIF_USED, D_QUAD_SLAVES, &smifContext);

        // Issue program
        Cy_SMIF_MMIO_Program_ExMem(SMIF_USED, D_QUAD_SLAVES, CY_SMIF_S25FL_ID_4QPP, writeStartAddress, DQUAD_PROGRAM_BUFFER_SIZE, (uint8_t*)&programData[writeOffsetAddrOfMCU], CY_SMIF_BLOCKING, TEST_SPI_WRITE_LC, 0, &smifDeviceContext, &smifContext);

        // Wait for completion
        CY_ASSERT_L1(WaitDeviceBecomesReady(D_QUAD_SLAVES) == 0u);
    }

    // Dummy Read to get device continuous mode
    uint32_t dummyRead = 0ul;
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, D_QUAD_SLAVES, rd_cmdId, TEST_PROGRAM_SECTOR, 4u, (uint8_t*)&dummyRead, TEST_SPI_READ_LC, CY_SMIF_S25FL_RW_CONTINUOUS, CY_SMIF_BLOCKING, &smifDeviceContext, &smifContext);

    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_MEMORY);

    // Read the programmed data
    const uint32_t testBaseAddress = CY_SMIF_GetXIP_Address(SMIF_USED) + TEST_DQUAD_XIP_PROGRAM_ADDR;
    memcpy(readDataBuffer, (void*)testBaseAddress, TEST_PROGRAM_SIZE);

    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);

    // Dummy Read to get device non continuous mode
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, D_QUAD_SLAVES, rd_cmdId, TEST_PROGRAM_SECTOR, 4u, (uint8_t*)&dummyRead, TEST_SPI_READ_LC, CY_SMIF_S25FL_RW_NON_CONTINUOUS, CY_SMIF_BLOCKING, &smifDeviceContext, &smifContext);

    // Check read data
    CY_ASSERT_L1((memcmp(readDataBuffer, programData, TEST_PROGRAM_SIZE) == 0));
}

static void TestFor_DQUAD_MMIO_WRITE_Then_MMIO_DDR_READ(uint16_t rd_cmdId)
{
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);
    for(uint32_t i_EraseSectorTopAddr = TEST_PROGRAM_SECTOR; i_EraseSectorTopAddr < (TEST_PROGRAM_SECTOR+TEST_PROGRAM_SIZE); i_EraseSectorTopAddr+= DQUAD_SECTOR_ERASE)
    {
        SectorEraseProcedure(D_QUAD_SLAVES, ((i_EraseSectorTopAddr + TEST_PROGRAM_SECTOR) / 2));
    }

    /*  Program 64K byte data */
    // !!!!! User need to swap data to be written like below, when write in SDR Octal mode and read in DDR Octalmode !!!
    for(uint32_t i_swappingDataPos = 0; i_swappingDataPos < TEST_PROGRAM_SIZE; i_swappingDataPos += 2)
    {
        swappedProgramData[i_swappingDataPos]     = programData[i_swappingDataPos + 1];
        swappedProgramData[i_swappingDataPos + 1] = programData[i_swappingDataPos];
    }

    volatile uint32_t programAddress    = TEST_PROGRAM_SECTOR;
    volatile uint32_t programSizeInByte = TEST_PROGRAM_SIZE;

    for(uint32_t i_WriteOffsetAddrOfQuadDevice = 0; i_WriteOffsetAddrOfQuadDevice < (programSizeInByte / 2); i_WriteOffsetAddrOfQuadDevice+=DEVICE_PROGRAM_BUFFER_SIZE)
    {
        uint32_t writeOffsetAddrOfMCU = i_WriteOffsetAddrOfQuadDevice * 2ul;
        uint32_t writeStartAddress = i_WriteOffsetAddrOfQuadDevice + programAddress;

        // Write enable
        Cy_SMIF_S25FL_WriteEnable(SMIF_USED, D_QUAD_SLAVES, &smifContext);

        // Issue program
        Cy_SMIF_MMIO_Program_ExMem(SMIF_USED, D_QUAD_SLAVES, CY_SMIF_S25FL_ID_4QPP, writeStartAddress, DQUAD_PROGRAM_BUFFER_SIZE, (uint8_t*)&swappedProgramData[writeOffsetAddrOfMCU], CY_SMIF_BLOCKING, TEST_SPI_WRITE_LC, 0, &smifDeviceContext, &smifContext);

        // Wait for completion
        CY_ASSERT_L1(WaitDeviceBecomesReady(D_QUAD_SLAVES) == 0u);
    }

    /* Read 64K byte data */
    volatile uint32_t readAddress    = TEST_PROGRAM_SECTOR;
    volatile uint32_t readSizeInByte = TEST_PROGRAM_SIZE;

    // Read the programmed data
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, D_QUAD_SLAVES, rd_cmdId, readAddress, readSizeInByte, (uint8_t*)readDataBuffer, TEST_SPI_READ_LC, CY_SMIF_S25FL_RW_NON_CONTINUOUS, CY_SMIF_BLOCKING, &smifDeviceContext, &smifContext);

    // Check read data
    CY_ASSERT_L1((memcmp(readDataBuffer, programData, TEST_PROGRAM_SIZE) == 0));
}

static void TestFor_DQUAD_MMIO_WRITE_Then_XIP_DDR_CONTINUOUS_READ(uint16_t rd_cmdId)
{
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);
    for(uint32_t i_EraseSectorTopAddr = TEST_PROGRAM_SECTOR; i_EraseSectorTopAddr < (TEST_PROGRAM_SECTOR+TEST_PROGRAM_SIZE); i_EraseSectorTopAddr+= DQUAD_SECTOR_ERASE)
    {
        SectorEraseProcedure(D_QUAD_SLAVES, ((i_EraseSectorTopAddr + TEST_PROGRAM_SECTOR) / 2));
    }

    /* Program 64K byte data */
    // !!!!! User need to swap data to be written like below, when write in SDR Octal mode and read in DDR Octalmode !!!
    for(uint32_t i_swappingDataPos = 0; i_swappingDataPos < TEST_PROGRAM_SIZE; i_swappingDataPos += 2)
    {
        swappedProgramData[i_swappingDataPos]     = programData[i_swappingDataPos + 1];
        swappedProgramData[i_swappingDataPos + 1] = programData[i_swappingDataPos];
    }

    volatile uint32_t programAddress    = TEST_PROGRAM_SECTOR;
    volatile uint32_t programSizeInByte = TEST_PROGRAM_SIZE;

    for(uint32_t i_WriteOffsetAddrOfQuadDevice = 0; i_WriteOffsetAddrOfQuadDevice < (programSizeInByte / 2); i_WriteOffsetAddrOfQuadDevice+=DEVICE_PROGRAM_BUFFER_SIZE)
    {
        uint32_t writeOffsetAddrOfMCU = i_WriteOffsetAddrOfQuadDevice * 2ul;
        uint32_t writeStartAddress = i_WriteOffsetAddrOfQuadDevice + programAddress;

        // Write enable
        Cy_SMIF_S25FL_WriteEnable(SMIF_USED, D_QUAD_SLAVES, &smifContext);

        // Issue program
        Cy_SMIF_MMIO_Program_ExMem(SMIF_USED, D_QUAD_SLAVES, CY_SMIF_S25FL_ID_4QPP, writeStartAddress, DQUAD_PROGRAM_BUFFER_SIZE, (uint8_t*)&swappedProgramData[writeOffsetAddrOfMCU], CY_SMIF_BLOCKING, TEST_SPI_WRITE_LC, 0, &smifDeviceContext, &smifContext);

        // Wait for completion
        CY_ASSERT_L1(WaitDeviceBecomesReady(D_QUAD_SLAVES) == 0u);
    }

    // Dummy Read to get device continuous mode
    uint32_t dummyRead = 0ul;
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, D_QUAD_SLAVES, rd_cmdId, TEST_PROGRAM_SECTOR, 4u, (uint8_t*)&dummyRead, TEST_SPI_READ_LC, CY_SMIF_S25FL_RW_CONTINUOUS, CY_SMIF_BLOCKING, &smifDeviceContext, &smifContext);

    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_MEMORY);

    // Read the programmed data
    const uint32_t testBaseAddress = CY_SMIF_GetXIP_Address(SMIF_USED) + TEST_DQUAD_XIP_PROGRAM_ADDR;
    memcpy(readDataBuffer, (void*)testBaseAddress, TEST_PROGRAM_SIZE);

    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);

    // Dummy Read to get device non continuous mode
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, D_QUAD_SLAVES, rd_cmdId, TEST_PROGRAM_SECTOR, 4u, (uint8_t*)&dummyRead, TEST_SPI_READ_LC, CY_SMIF_S25FL_RW_NON_CONTINUOUS, CY_SMIF_BLOCKING, &smifDeviceContext, &smifContext);

    // Check read data
    CY_ASSERT_L1((memcmp(readDataBuffer, programData, TEST_PROGRAM_SIZE) == 0));
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
static bool WaitDeviceBecomesReady(cy_en_smif_slave_select_t slave)
{
    for(uint8_t i_slaveNum = 0; i_slaveNum < CY_SMIF_GetDeviceNumber(SMIF_USED); i_slaveNum++)
    {
        cy_en_smif_slave_select_t slaveToBeChecked = (cy_en_smif_slave_select_t)((uint8_t)slave & (0x01 << i_slaveNum));
        if((uint8_t)slaveToBeChecked == 0u)
        {
            // not selected slave
            continue;
        }

        // only selected slave will reach hare.
        while(1)
        {
            volatile un_cy_smif_s25fl128_SR1_t sr1 = { .u8 = 0u };
            Cy_SMIF_S25FL_ReadStatusRegister1(SMIF_USED, slaveToBeChecked, (uint8_t*)&sr1, &smifContext);
            if(sr1.field.u1WIP == 0u)
            {
                break;
            }

#if (CY_SMIF_QUAD_MEMORY == CY_SMIF_S25FXXXS)
            if((sr1.field.u1E_ERR == 1u) || (sr1.field.u1P_ERR == 1u))
            {
                return 1; // error
            }
#elif (CY_SMIF_QUAD_MEMORY == CY_SMIF_S25FXXXL)
            volatile un_cy_smif_s25fl128_SR2_t sr2 = { .u8 = 0u };
            Cy_SMIF_S25FL_ReadStatusRegister2(SMIF_USED, slaveToBeChecked, (uint8_t*)&sr2, &smifContext);
            if((sr2.field.u1E_ERR == 1u) || (sr2.field.u1P_ERR == 1u))
            {
                 Cy_SMIF_S25FL_ClearStatusRegister(SMIF_USED,
                                        slaveToBeChecked,
                                        &smifContext);

                return 1; // error
            }
#endif
        }
    }
    return 0;

}

static void SectorEraseProcedure(cy_en_smif_slave_select_t slave, uint32_t address)
{
    /*  Erase sector        */
    // Write enable
    Cy_SMIF_S25FL_WriteEnable(SMIF_USED, slave, &smifContext);

    // Sector erase
    Cy_SMIF_S25FL_SectorErase(SMIF_USED, slave, address, &smifContext);

    // Wait for completion
    CY_ASSERT_L1(WaitDeviceBecomesReady(D_QUAD_SLAVES) == 0);
}


/* [] END OF FILE */
