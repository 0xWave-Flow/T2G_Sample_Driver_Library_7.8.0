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
#define TEST_WRITE_LC        CY_SMIF_SEMP_WT_LATENCY0
#define TEST_READ_LC         CY_SMIF_SEMP_RD_LATENCY9
#define TEST_READ_REG_LC     CY_SMIF_SEMP_REG_LATENCY1 // SDR SPI: up to 133MHz, SDR OCTAL: up to 133MHz, DDR OCTAL: up to 66MHz
#define TEST_ADDR_LEN        CY_TRX_ADDR_4BYTE
#define TEST_DLP_VALUE       (0xAA)
/**************************************/

#define TEST_PROGRAM_SECTOR (0x00123400ul)

#define DEVICE_PROGRAM_BUFFER_SIZE (0x100) // 256B
#define DEVICE_SECTOR_ERASE_SIZE   (0x00040000) // 256KB

#define DATA_WIDTH_1_OR_2_BITS (0)
#define DATA_WIDTH_4_BITS      (1)
#define TRX_DATA_WIDTH         (CY_SPI_TRANSACTION_4S4S4S)
#define TEST_SLAVE             (cy_en_smif_slave_select_t)(CY_SMIF_SLAVE_SELECT_0 | CY_SMIF_SLAVE_SELECT_1)

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

// will be updated in the application
cy_stc_device_config_t smifDev0Cfg =
{
    .xipReadCmdId       = CY_SMIF_SEMP_RDID_1S1S1S_4ADR,
    .xipReadMode        = 0x00,
    .writeEn            = false,
    .xipWriteCmdId      = CY_SMIF_SEMP_WTID_1S1S1S_4ADR,
    .xipWriteMode       = 0u,
    .mergeEnable        = true,
    .mergeTimeout       = CY_SMIF_MER_TIMEOUT_1_CYCLE,
    .totalTimeoutEnable = true,
    .totalTimeout       = 1000u,
    .addr               = 0,
    .size               = CY_SMIF_DEVICE_64M_BYTE,
    .rdLatencyCode      = TEST_READ_LC,
    .wtLatencyCode      = TEST_WRITE_LC,
};

static bool WaitDeviceBecomesReady(cy_en_smif_slave_select_t slave, cy_en_smif_semp_reg_latency_code_t regLc, cy_en_smif_trx_type_t transMode);
static void SetModeWithBusyCheck(volatile cy_stc_smif_reg_t *base, cy_en_smif_mode_t mode);
static void WriteRegisterSequence(cy_en_smif_slave_select_t slave, cy_en_smif_semp_reg_addr_t addr, uint8_t value, cy_en_smif_trx_addr_len_t addrLen, cy_en_smif_semp_reg_latency_code_t regLc, cy_en_smif_trx_type_t transMode);
static void ReadbackMMIO(uint16_t rdCmdId, uint32_t size);
static void ReadbackXIP(uint16_t rdCmdId, uint32_t size);
static void ReadbackContinuousMode(uint16_t rdDummyCmdId, uint16_t rdCmdId, uint32_t size);

cy_stc_ex_dev_context_t smifDevice0Context;

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

    /********* Issue Software Reset ******/
    Cy_SMIF_SEMP_SoftwareResetEnable(SMIF_USED,
                                     TEST_SLAVE,
                                     CY_SPI_TRANSACTION_1S1S1S,
                                     &smifContext);
    Cy_SMIF_SEMP_SoftwareReset(SMIF_USED,
                               TEST_SLAVE,
                               CY_SPI_TRANSACTION_1S1S1S,
                               &smifContext);
    WaitDeviceBecomesReady(TEST_SLAVE, CY_SMIF_SEMP_REG_LATENCY0, CY_SPI_TRANSACTION_1S1S1S);

    /*********************************************************/
    /********* Write Status and Configuration registers ******/
    /*********************************************************/

    /*** Write Configuration register ***/
    // Setting Status Register 1
    un_cy_smif_semp_STR1_t writeStatusReg1 = {0};

    // Setting Configuration Register 1
    un_cy_smif_semp_CFR1_t writeConfigReg1 = {0};
    writeConfigReg1.field.u1QUADIT = DATA_WIDTH_1_OR_2_BITS; 

    // Setting Configuration Register 2
    un_cy_smif_semp_CFR2_t writeConfigReg2 = {0};
    writeConfigReg2.field.u4MEMLAT = TEST_READ_LC;
    writeConfigReg2.field.u1QPI_IT = DATA_WIDTH_4_BITS;
    writeConfigReg2.field.u1ADRBYT = TEST_ADDR_LEN; 

    // Setting Configuration Register 3
    un_cy_smif_semp_CFR3_t writeConfigReg3 ={0};
    writeConfigReg3.field.u2VRGLAT = TEST_READ_REG_LC;

    // Setting Configuration Register 4
    un_cy_smif_semp_CFR4_t writeConfigReg4 = {0};
    writeConfigReg4.field.u1RBSTWP = 0; // Read Wrapped Burst disable

    // Write Enable Volatile register
    // !! Attention if "Write Enable" command issued, following
    //    "Write Register" command will write to Non-Volatile registers.
    Cy_SMIF_SEMP_WriteEnableVolatileRegister(SMIF_USED,
                                        TEST_SLAVE,
                                        CY_SPI_TRANSACTION_1S1S1S,
                                        &smifContext);

    // Write registers
    Cy_SMIF_SEMP_WriteRegister(SMIF_USED,
                               TEST_SLAVE,
                               writeStatusReg1.u8,
                               writeConfigReg1.u8,
                               writeConfigReg2.u8,
                               writeConfigReg3.u8,
                               writeConfigReg4.u8,
                               CY_SPI_TRANSACTION_1S1S1S,
                               &smifContext);
    CY_ASSERT(WaitDeviceBecomesReady(TEST_SLAVE, TEST_READ_REG_LC, TRX_DATA_WIDTH) == 0);

    /*** Read back and Verify Configuration register ***/
    // Read and Verify Setting Configuration Register 1
    un_cy_smif_semp_CFR1_t readConfigReg1_0 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_0, 
                                 &readConfigReg1_0.u8, 
                                 CY_SEMP_REG_ADDR_CFR1_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 TRX_DATA_WIDTH,
                                 &smifContext);
    CY_ASSERT(writeConfigReg1.field.u1QUADIT == readConfigReg1_0.field.u1QUADIT);

    un_cy_smif_semp_CFR1_t readConfigReg1_1 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_1, 
                                 &readConfigReg1_0.u8, 
                                 CY_SEMP_REG_ADDR_CFR1_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 TRX_DATA_WIDTH,
                                 &smifContext);
    CY_ASSERT(writeConfigReg1.field.u1QUADIT == readConfigReg1_1.field.u1QUADIT);


    // Read and Verify Setting Configuration Register 2
    un_cy_smif_semp_CFR2_t readConfigReg2_0 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_0, 
                                 &readConfigReg2_0.u8, 
                                 CY_SEMP_REG_ADDR_CFR2_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 TRX_DATA_WIDTH,
                                 &smifContext);
    CY_ASSERT(writeConfigReg2.field.u4MEMLAT == readConfigReg2_0.field.u4MEMLAT);
    CY_ASSERT(writeConfigReg2.field.u1QPI_IT == readConfigReg2_0.field.u1QPI_IT);
    CY_ASSERT(writeConfigReg2.field.u1ADRBYT == readConfigReg2_0.field.u1ADRBYT);

    un_cy_smif_semp_CFR2_t readConfigReg2_1 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_1, 
                                 &readConfigReg2_1.u8, 
                                 CY_SEMP_REG_ADDR_CFR2_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 TRX_DATA_WIDTH,
                                 &smifContext);
    CY_ASSERT(writeConfigReg2.field.u4MEMLAT == readConfigReg2_1.field.u4MEMLAT);
    CY_ASSERT(writeConfigReg2.field.u1QPI_IT == readConfigReg2_1.field.u1QPI_IT);
    CY_ASSERT(writeConfigReg2.field.u1ADRBYT == readConfigReg2_1.field.u1ADRBYT);


    // Read and Verify Setting Configuration Register 3
    un_cy_smif_semp_CFR3_t readConfigReg3_0 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_0, 
                                 &readConfigReg3_0.u8, 
                                 CY_SEMP_REG_ADDR_CFR3_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 TRX_DATA_WIDTH,
                                 &smifContext);
    CY_ASSERT(writeConfigReg3.field.u2VRGLAT == readConfigReg3_0.field.u2VRGLAT);

    un_cy_smif_semp_CFR3_t readConfigReg3_1 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_1, 
                                 &readConfigReg3_1.u8, 
                                 CY_SEMP_REG_ADDR_CFR3_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 TRX_DATA_WIDTH,
                                 &smifContext);
    CY_ASSERT(writeConfigReg3.field.u2VRGLAT == readConfigReg3_1.field.u2VRGLAT);

    // Read and Verify Setting Configuration Register 4
    un_cy_smif_semp_CFR4_t readConfigReg4_0 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_0, 
                                 &readConfigReg4_0.u8, 
                                 CY_SEMP_REG_ADDR_CFR4_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 TRX_DATA_WIDTH,
                                 &smifContext);
    CY_ASSERT(writeConfigReg4.field.u1RBSTWP == readConfigReg4_0.field.u1RBSTWP);

    un_cy_smif_semp_CFR4_t readConfigReg4_1 = {0};
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_1, 
                                 &readConfigReg4_1.u8, 
                                 CY_SEMP_REG_ADDR_CFR4_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 TRX_DATA_WIDTH,
                                 &smifContext);
    CY_ASSERT(writeConfigReg4.field.u1RBSTWP == readConfigReg4_1.field.u1RBSTWP);


    /*********************************************************/
    /*******          Write/Read DLP register           ******/
    /*********************************************************/
    // just for read/write test for now
    WriteRegisterSequence(TEST_SLAVE,
                          CY_SEMP_REG_ADDR_DLP_V,
                          TEST_DLP_VALUE,
                          TEST_ADDR_LEN,
                          TEST_READ_REG_LC,
                          TRX_DATA_WIDTH);
    uint8_t readbackDLP = 0u;
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_0, 
                                 &readbackDLP, 
                                 CY_SEMP_REG_ADDR_DLP_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 TRX_DATA_WIDTH,
                                 &smifContext);
    CY_ASSERT(readbackDLP == TEST_DLP_VALUE);

    readbackDLP = 0u;
    Cy_SMIF_SEMP_ReadAnyVolatileRegister(SMIF_USED,
                                 CY_SMIF_SLAVE_SELECT_1, 
                                 &readbackDLP, 
                                 CY_SEMP_REG_ADDR_DLP_V, 
                                 TEST_READ_REG_LC,
                                 TEST_ADDR_LEN,
                                 TRX_DATA_WIDTH,
                                 &smifContext);
    CY_ASSERT(readbackDLP == TEST_DLP_VALUE);

    /***********************************************/
    /*********  Erase/Write/Read/Verify Test *******/
    /***********************************************/
    uint16_t sdrQuadCommand0;
    uint16_t ddrQuadCommand0;
    uint16_t sdrQuadCmdCont;
    uint16_t ddrQuadCmdCont;
    uint16_t writeQuadCommand;
    if(TEST_ADDR_LEN == CY_TRX_ADDR_4BYTE)
    {
        sdrQuadCommand0   = CY_SMIF_SEMP_RDID_4S4S4S_4ADR_DLP;
        ddrQuadCommand0   = CY_SMIF_SEMP_RDID_4D4D4D_4ADR_DLP;
        sdrQuadCmdCont    = CY_SMIF_SEMP_RDID_4S4S_4ADR_DLP;
        ddrQuadCmdCont    = CY_SMIF_SEMP_RDID_4D4D_4ADR_DLP;
        writeQuadCommand  = CY_SMIF_SEMP_WTID_4S4S4S_4ADR;
        smifDev0Cfg.size  = CY_SMIF_DEVICE_64M_BYTE;
    }
    else
    {
        sdrQuadCommand0   = CY_SMIF_SEMP_RDID_4S4S4S_3ADR_DLP;
        ddrQuadCommand0   = CY_SMIF_SEMP_RDID_4D4D4D_3ADR_DLP;
        sdrQuadCmdCont    = CY_SMIF_SEMP_RDID_4S4S_3ADR_DLP;
        ddrQuadCmdCont    = CY_SMIF_SEMP_RDID_4D4D_3ADR_DLP;
        writeQuadCommand  = CY_SMIF_SEMP_WTID_4S4S4S_3ADR;
        smifDev0Cfg.size  = CY_SMIF_DEVICE_8M_BYTE;
    }

    /***********************************************/
    /*********           SDR TEST            *******/
    /***********************************************/
    /*** erase ***/
    // Write Enable
    Cy_SMIF_SEMP_WriteEnable(SMIF_USED, TEST_SLAVE, TRX_DATA_WIDTH, &smifContext);

    // Issue Erase Sector
    Cy_SMIF_SEMP_Erase_256KB_Sector(SMIF_USED, TEST_SLAVE, TEST_PROGRAM_SECTOR, TRX_DATA_WIDTH, &smifContext);
    CY_ASSERT(WaitDeviceBecomesReady(TEST_SLAVE, TEST_READ_REG_LC, TRX_DATA_WIDTH) == 0);

    // Read back with read command
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, 
                            TEST_SLAVE, 
                            sdrQuadCommand0,
                            TEST_PROGRAM_SECTOR, 
                            DEVICE_PROGRAM_BUFFER_SIZE, 
                            (uint8_t*)readDataBuffer, 
                            TEST_READ_LC, 
                            Cy_SMIF_SEMP_MODE_NOT_CONTINUOUS, /* mode value */
                            CY_SMIF_BLOCKING, 
                            &smifDevice0Context, 
                            &smifContext);
    // verify
    for(uint32_t i = 0; i < DEVICE_PROGRAM_BUFFER_SIZE; i++)
    {
        if(0xFF != readDataBuffer[i])
        {
            while(1); // read/write value mismatch.
        }
    }

    memset(readDataBuffer, 0x00, DEVICE_PROGRAM_BUFFER_SIZE);

    /*** write ***/
    // Write Enable
    Cy_SMIF_SEMP_WriteEnable(SMIF_USED, TEST_SLAVE, TRX_DATA_WIDTH, &smifContext);
    // Write test data
    Cy_SMIF_MMIO_Program_ExMem(SMIF_USED,
                               TEST_SLAVE, 
                               writeQuadCommand,
                               TEST_PROGRAM_SECTOR,
                               DEVICE_PROGRAM_BUFFER_SIZE, 
                               (uint8_t*)&programData[0], 
                               CY_SMIF_BLOCKING,
                               TEST_WRITE_LC, 
                               Cy_SMIF_SEMP_MODE_NOT_CONTINUOUS, /* mode value */
                               &smifDevice0Context, 
                               &smifContext);
    CY_ASSERT(WaitDeviceBecomesReady(TEST_SLAVE, TEST_READ_REG_LC, TRX_DATA_WIDTH) == 0);

    /*** Enable DLP calibration (Only for SMIF_3 and SMIF_4 which have DLP calibration HW) ***/
    Cy_SMIF_Disable(SMIF_USED);
    smifConfig.dlpAuto = CY_SMIF_DLP_UPDATE_AUTO;
    smifConfig.rxClk   = CY_SMIF_INV_INTERNAL_CLK;
    Cy_SMIF_Init(SMIF_USED, &smifConfig, 1000, &smifContext);
    Cy_SMIF_SetMasterDLP(SMIF_USED, TEST_DLP_VALUE);
    Cy_SMIF_SetMasterDLP_Size(SMIF_USED, 8ul);
    Cy_SMIF_SetMasterDLP_WarnLevel(SMIF_USED, 3ul);
    Cy_SMIF_Enable(SMIF_USED, &smifContext);

    /*********  Read/Verify in SDR mode   *******/
    /*** Read back and verify in MMIO mode ***/
    ReadbackMMIO(sdrQuadCommand0, DEVICE_PROGRAM_BUFFER_SIZE);

    /*** Read back and Verify in XIP mode ***/
    ReadbackXIP(sdrQuadCommand0, DEVICE_PROGRAM_BUFFER_SIZE);

    /*** Read back and Verify Using Continuous command ***/
    ReadbackContinuousMode(sdrQuadCommand0, sdrQuadCmdCont, DEVICE_PROGRAM_BUFFER_SIZE);

    /***********************************************/
    /*********          DDR TEST            ********/
    /***********************************************/
    /*** Get Rx Output Clock  ***/
    Cy_SMIF_Disable(SMIF_USED);
    smifConfig.dlpAuto = CY_SMIF_DLP_UPDATE_MANUAL;
    smifConfig.rxClk   = CY_SMIF_INV_OUTPUT_CLK;
    Cy_SMIF_Init(SMIF_USED, &smifConfig, 1000, &smifContext);
    Cy_SMIF_Enable(SMIF_USED, &smifContext);

    /*** erase ***/
    // Write Enable
    Cy_SMIF_SEMP_WriteEnable(SMIF_USED, TEST_SLAVE, TRX_DATA_WIDTH, &smifContext);

    // Issue Erase Sector
    Cy_SMIF_SEMP_Erase_256KB_Sector(SMIF_USED, TEST_SLAVE, TEST_PROGRAM_SECTOR, TRX_DATA_WIDTH, &smifContext);
    WaitDeviceBecomesReady(TEST_SLAVE, TEST_READ_REG_LC, TRX_DATA_WIDTH);

    uint32_t dualQuadProgBufSize = DEVICE_PROGRAM_BUFFER_SIZE*2;
    static uint8_t swapedProgramData[DEVICE_PROGRAM_BUFFER_SIZE*2];

    // Read back with read command
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, 
                            TEST_SLAVE, 
                            sdrQuadCommand0,
                            TEST_PROGRAM_SECTOR, 
                            dualQuadProgBufSize, 
                            (uint8_t*)readDataBuffer, 
                            TEST_READ_LC, 
                            Cy_SMIF_SEMP_MODE_NOT_CONTINUOUS, /* mode value */
                            CY_SMIF_BLOCKING, 
                            &smifDevice0Context, 
                            &smifContext);
    // verify
    for(uint32_t i = 0; i < dualQuadProgBufSize; i++)
    {
        if(0xFF != readDataBuffer[i])
        {
            while(1); // read/write value mismatch.
        }
    }

    memset(readDataBuffer, 0x00, dualQuadProgBufSize);

    /*** write ***/
    // Prepare swapped data
    for(uint32_t i = 0; i < dualQuadProgBufSize; i+=2)
    {
        swapedProgramData[i] = programData[i+1];
        swapedProgramData[i+1] = programData[i];
    }

    // Write Enable
    Cy_SMIF_SEMP_WriteEnable(SMIF_USED, TEST_SLAVE, TRX_DATA_WIDTH, &smifContext);

    // Write test data
    Cy_SMIF_MMIO_Program_ExMem(SMIF_USED,
                               TEST_SLAVE, 
                               writeQuadCommand,
                               TEST_PROGRAM_SECTOR,
                               dualQuadProgBufSize,
                               (uint8_t*)&swapedProgramData[0], 
                               CY_SMIF_BLOCKING,
                               TEST_WRITE_LC, 
                               Cy_SMIF_SEMP_MODE_NOT_CONTINUOUS, /* mode value */
                               &smifDevice0Context, 
                               &smifContext);
    WaitDeviceBecomesReady(TEST_SLAVE, TEST_READ_REG_LC, TRX_DATA_WIDTH);

    /*** Get Rx Clock Internal which supports DLP calibration ***/
    Cy_SMIF_Disable(SMIF_USED);
    smifConfig.dlpAuto = CY_SMIF_DLP_UPDATE_AUTO;
    smifConfig.rxClk   = CY_SMIF_INV_INTERNAL_CLK;
    Cy_SMIF_Init(SMIF_USED, &smifConfig, 1000, &smifContext);
    Cy_SMIF_Enable(SMIF_USED, &smifContext);

    /*********  Read/Verify in DDR mode   *******/
    /*** Read back and verify in MMIO mode ***/
    ReadbackMMIO(ddrQuadCommand0, dualQuadProgBufSize);

    /*** Read back and Verify in XIP mode ***/
    ReadbackXIP(ddrQuadCommand0, dualQuadProgBufSize);

    /*** Read back and Verify Using Continuous command ***/
    ReadbackContinuousMode(ddrQuadCommand0, ddrQuadCmdCont, dualQuadProgBufSize);

    // Reaching here means this example worked properly.
    for(;;);
}

void ReadbackMMIO(uint16_t rdCmdId, uint32_t size)
{
    memset(readDataBuffer, 0x00, size);
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);

    // Read back with read command
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, 
                            TEST_SLAVE, 
                            rdCmdId,
                            TEST_PROGRAM_SECTOR, 
                            size, 
                            (uint8_t*)readDataBuffer, 
                            TEST_READ_LC, 
                            Cy_SMIF_SEMP_MODE_NOT_CONTINUOUS, /* mode value */
                            CY_SMIF_BLOCKING, 
                            &smifDevice0Context, 
                            &smifContext);

    // verify
    for(uint32_t i = 0; i < size; i++)
    {
        if(programData[i] != readDataBuffer[i])
        {
            while(1); // read/write value mismatch.
        }
    }
}

void ReadbackXIP(uint16_t rdCmdId, uint32_t size)
{
    // Set up for XIP mode
    smifDev0Cfg.addr = CY_SMIF_GetXIP_Address(SMIF_USED);
    smifDev0Cfg.xipReadCmdId = rdCmdId;
    smifDev0Cfg.xipReadMode  = Cy_SMIF_SEMP_MODE_NOT_CONTINUOUS;
    CY_ASSERT(Cy_SMIF_InitXIPModeForExMem(SMIF_DEVICE0, CY_SMIF_DEV_D_QUAD_BIT_0To3, &smifDev0Cfg, &smifDevice0Context) == CY_SMIF_SUCCESS);
    CY_ASSERT(Cy_SMIF_InitXIPModeForExMem(SMIF_DEVICE1, CY_SMIF_DEV_D_QUAD_BIT_4To7, &smifDev0Cfg, &smifDevice0Context) == CY_SMIF_SUCCESS);
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_MEMORY);

    // Read and verify
    uint8_t* pbaseAddrXIP = (uint8_t*)smifDev0Cfg.addr;
    for(uint32_t i = 0; i < size; i++)
    {
        if(programData[i] != pbaseAddrXIP[(TEST_PROGRAM_SECTOR*2)+i])
        {
            while(1); // read/write value mismatch.
        }
    }
}

void ReadbackContinuousMode(uint16_t rdDummyCmdId, uint16_t rdCmdId, uint32_t size)
{
    // Get to continuous mode by dummy reading
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);
    memset(readDataBuffer, 0x00, size);
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, 
                            TEST_SLAVE, 
                            rdDummyCmdId,
                            TEST_PROGRAM_SECTOR, 
                            2, 
                            (uint8_t*)readDataBuffer, 
                            TEST_READ_LC, 
                            Cy_SMIF_SEMP_MODE_CONTINUOUS, /* mode value Attention !!! */
                            CY_SMIF_BLOCKING, 
                            &smifDevice0Context, 
                            &smifContext);

    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_MEMORY);
    // Set up for XIP mode
    smifDev0Cfg.addr = CY_SMIF_GetXIP_Address(SMIF_USED);
    smifDev0Cfg.xipReadCmdId = rdCmdId;
    smifDev0Cfg.xipReadMode  = Cy_SMIF_SEMP_MODE_CONTINUOUS;
    CY_ASSERT(Cy_SMIF_InitXIPModeForExMem(SMIF_DEVICE0, CY_SMIF_DEV_D_QUAD_BIT_0To3, &smifDev0Cfg, &smifDevice0Context) == CY_SMIF_SUCCESS);
    CY_ASSERT(Cy_SMIF_InitXIPModeForExMem(SMIF_DEVICE1, CY_SMIF_DEV_D_QUAD_BIT_4To7, &smifDev0Cfg, &smifDevice0Context) == CY_SMIF_SUCCESS);
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_MEMORY);

    // Read and verify
    uint8_t* pbaseAddrXIP = (uint8_t*)smifDev0Cfg.addr;
    for(uint32_t i = 0; i < size; i++)
    {
        if(programData[i] != pbaseAddrXIP[(TEST_PROGRAM_SECTOR*2)+i])
        {
            while(1); // read/write value mismatch.
        }
    }

    // Get to continuous mode by dummy reading
    SetModeWithBusyCheck(SMIF_USED, CY_SMIF_NORMAL);
    Cy_SMIF_MMIO_Read_ExMem(SMIF_USED, 
                            TEST_SLAVE, 
                            rdDummyCmdId,
                            TEST_PROGRAM_SECTOR, 
                            2, 
                            (uint8_t*)readDataBuffer, 
                            TEST_READ_LC, 
                            Cy_SMIF_SEMP_MODE_NOT_CONTINUOUS, /* mode value Attention !!! */
                            CY_SMIF_BLOCKING, 
                            &smifDevice0Context, 
                            &smifContext);

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
            un_cy_smif_semp_STR1_t status1;
            Cy_SMIF_SEMP_ReadStatusRegister1(SMIF_USED,
                                     slaveToBeChecked, 
                                     &status1.u8, 
                                     regLc,
                                     transMode,
                                     &smifContext);
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
    }

    return 0;
}

static void WriteRegisterSequence(cy_en_smif_slave_select_t slave, cy_en_smif_semp_reg_addr_t addr, uint8_t value, cy_en_smif_trx_addr_len_t addrLen, cy_en_smif_semp_reg_latency_code_t regLc, cy_en_smif_trx_type_t transMode)
{
    Cy_SMIF_SEMP_WriteEnable(SMIF_USED, slave, transMode, &smifContext);
    Cy_SMIF_SEMP_WriteAnyRegister(SMIF_USED, slave, value, addr, addrLen, transMode, &smifContext);
    CY_ASSERT(WaitDeviceBecomesReady(slave, regLc, transMode) == 0);
}

/* [] END OF FILE */
