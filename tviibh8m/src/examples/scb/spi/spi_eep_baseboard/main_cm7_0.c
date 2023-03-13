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

/* User setting value */
#if (CY_USE_PSVP == 1)  
  #define SOURCE_CLOCK_FRQ 24000000ul
#else
  #define SOURCE_CLOCK_FRQ 80000000ul
#endif
#define SCB_SPI_BAUDRATE     50000ul /* Please set baudrate value of SPI you want */
#define SCB_SPI_OVERSAMPLING 8ul     /* Please set oversampling of SPI you want */
#define SCB_SPI_CLOCK_FREQ   (SCB_SPI_BAUDRATE * SCB_SPI_OVERSAMPLING)

#define DIVIDER_NO_1 (1u)

#define PAGE_WRITE_SIZE 32u
#define WRITE_TEST_SIZE 256u

static void SPI_Initialize(void);
static void WriteEEPROM(uint16_t address, uint8_t data);
static uint8_t ReadEEPROM(uint16_t address);
static uint8_t GetStatusEEPROM(void);
static uint8_t GetWEL_EEPROM(void);
static uint8_t GetWIP_EEPROM(void);
static void WriteEnableEEPROM(void);
static void PageWriteEEPROM(uint16_t address, uint8_t data[], uint8_t size);

/* SPi Context holder */
cy_stc_scb_spi_context_t spiCtx;

static const cy_stc_gpio_pin_prt_config_t g_eeprom_pin_cfg[] =
{
//  {                 portReg,                 pinNum, outVal,         driveMode,                      hsiom, intEdge, intMask, vtrip, slewRate, driveSel },
    { CY_EEPROM_SCB_MISO_PORT, CY_EEPROM_SCB_MISO_PIN,    0ul,  CY_GPIO_DM_HIGHZ, CY_EEPROM_SCB_MISO_PIN_MUX,     0ul,     0ul,   0ul,      0ul,     0ul, },
    { CY_EEPROM_SCB_MOSI_PORT, CY_EEPROM_SCB_MOSI_PIN,    0ul, CY_GPIO_DM_STRONG, CY_EEPROM_SCB_MOSI_PIN_MUX,     0ul,     0ul,   0ul,      0ul,     0ul, },
    {  CY_EEPROM_SCB_SCK_PORT,  CY_EEPROM_SCB_SCK_PIN,    0ul, CY_GPIO_DM_STRONG,  CY_EEPROM_SCB_SCK_PIN_MUX,     0ul,     0ul,   0ul,      0ul,     0ul, },
    { CY_EEPROM_SCB_SSEL_PORT, CY_EEPROM_SCB_SSEL_PIN,    1ul, CY_GPIO_DM_STRONG, CY_EEPROM_SCB_SSEL_PIN_MUX,     0ul,     0ul,   0ul,      0ul,     0ul, },
    {     CY_EEPROM_HOLD_PORT,     CY_EEPROM_HOLD_PIN,    1ul, CY_GPIO_DM_STRONG,     CY_EEPROM_HOLD_PIN_MUX,     0ul,     0ul,   0ul,      0ul,     0ul, },
    {       CY_EEPROM_WP_PORT,       CY_EEPROM_WP_PIN,    1ul, CY_GPIO_DM_STRONG,       CY_EEPROM_WP_PIN_MUX,     0ul,     0ul,   0ul,      0ul,     0ul, },
};
#define EEPROM_PORT_NUM (sizeof(g_eeprom_pin_cfg)/sizeof(g_eeprom_pin_cfg[0]))

int main(void)
{
    SystemInit();

    /* Enable global interrupts. */
    __enable_irq(); 

    /* Initialize ports. */
    Cy_GPIO_Multi_Pin_Init(g_eeprom_pin_cfg, EEPROM_PORT_NUM);

    /* Initialize SPI */
    SPI_Initialize();

    /* 1. Write 256 byte one by one from address of 0 to 255 */
    for(uint16_t i = 0u; i < WRITE_TEST_SIZE; i++)
    {
        static volatile uint8_t readData[WRITE_TEST_SIZE];

        /* Enable writing */
        do
        {
            WriteEnableEEPROM();
        } while(GetWEL_EEPROM() == 0u);

        /* Writing 1 Byte data */
        WriteEEPROM(/* addr */ i, /* data */(uint8_t)i);

        /* Waiting if previous writing is in progress */
        while(GetWIP_EEPROM() == 1u);

        /* Read from eeprom */
        readData[i] = ReadEEPROM(/* addr */ i);

        /* Verify read data */
        CY_ASSERT(readData[i] == i);

    }

    /* 2. Write 256 byte by 32byte page from address of 256 to 511 */
    for(uint16_t i = 0u ;i < (WRITE_TEST_SIZE / PAGE_WRITE_SIZE); i++)
    {
        uint8_t writeBuffer[PAGE_WRITE_SIZE];
        static volatile uint8_t readData2[WRITE_TEST_SIZE];

        uint16_t addrOffset = i * PAGE_WRITE_SIZE;
        uint16_t addr = WRITE_TEST_SIZE + addrOffset;

        /* prepare write buffer data */
        for(uint32_t j = 0ul; j < PAGE_WRITE_SIZE; j++)
        {
            writeBuffer[j] = (addr + j);
        }

        /* Enable writing */
        do
        {
            WriteEnableEEPROM();
        } while(GetWEL_EEPROM() == 0u);

        /* Writing 32 Byte data */
        PageWriteEEPROM(/* addr */ addr, /* data */writeBuffer, /* size */PAGE_WRITE_SIZE);

        /* Waiting if previous writing is in progress */
        while(GetWIP_EEPROM() == 1u);

        for(uint16_t j = 0u; j < PAGE_WRITE_SIZE; j++)
        {
            readData2[addrOffset + j] = ReadEEPROM(/* addr */ addr + j);
        }

        CY_ASSERT(memcmp(writeBuffer, (void*)&readData2[addrOffset], PAGE_WRITE_SIZE) == 0);
    }

    for(;;);
}

/* SPI Interrupt Handler */
void SpiInterruptHandler(void)
{
    Cy_SCB_SPI_Interrupt(CY_EEPROM_SCB_TYPE, &spiCtx);
    NVIC_ClearPendingIRQ(CPUIntIdx2_IRQn);
}

void SetPeripheFracDiv24_5(uint64_t targetFreq, uint64_t sourceFreq, uint8_t divNum)
{
    uint64_t temp = ((uint64_t)sourceFreq << 5ull);
    uint32_t divSetting;

    divSetting = (uint32_t)(temp / targetFreq);
    Cy_SysClk_PeriphSetFracDivider(Cy_SysClk_GetClockGroup(CY_EEPROM_SCB_PCLK), 
                                   CY_SYSCLK_DIV_24_5_BIT, divNum, 
                                   (((divSetting >> 5ul) & 0x00000FFFul) - 1ul), 
                                   (divSetting & 0x0000001Ful));
}

/* SPi Initialize */
static void SPI_Initialize(void)
{
    Cy_SysClk_PeriphAssignDivider(CY_EEPROM_SCB_PCLK, CY_SYSCLK_DIV_24_5_BIT, DIVIDER_NO_1);
    SetPeripheFracDiv24_5(SCB_SPI_CLOCK_FREQ, SOURCE_CLOCK_FRQ, DIVIDER_NO_1);
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(CY_EEPROM_SCB_PCLK), CY_SYSCLK_DIV_24_5_BIT, DIVIDER_NO_1);

    /* De-initialize SPI */
    Cy_SCB_SPI_DeInit(CY_EEPROM_SCB_TYPE);
    
    /* Setup the IRQ */
    cy_stc_sysint_irq_t irq_cfg;
    irq_cfg = (cy_stc_sysint_irq_t){
        .sysIntSrc  = CY_EEPROM_SCB_IRQN,
        .intIdx     = CPUIntIdx2_IRQn,
        .isEnabled  = true,
    };
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, SpiInterruptHandler);
    NVIC_EnableIRQ(CPUIntIdx2_IRQn);

    cy_stc_scb_spi_config_t spiCfg = 
    {
        .spiMode                    = CY_SCB_SPI_MASTER,
        .subMode                    = CY_SCB_SPI_MOTOROLA,
        .sclkMode                   = CY_SCB_SPI_CPHA0_CPOL0,
        .oversample                 = 8u,
        .rxDataWidth                = 8u,
        .txDataWidth                = 8u,
        .enableMsbFirst             = true,
        .enableFreeRunSclk          = false,
        .enableInputFilter          = false,
        .enableMisoLateSample       = false,
        .enableTransferSeperation   = false,
        .ssPolarity0                = CY_SCB_SPI_ACTIVE_LOW,
        .ssPolarity1                = CY_SCB_SPI_ACTIVE_LOW,
        .ssPolarity2                = CY_SCB_SPI_ACTIVE_LOW,
        .ssPolarity3                = CY_SCB_SPI_ACTIVE_LOW,
        .enableWakeFromSleep        = false,
        .txFifoTriggerLevel         = 0,
        .rxFifoTriggerLevel         = 1,
        .rxFifoIntEnableMask        = 0,
        .txFifoIntEnableMask        = 0,
        .masterSlaveIntEnableMask   = 0,
        .enableSpiDoneInterrupt     = false,
        .enableSpiBusErrorInterrupt = false,
    };
    
    /* Initialize the SPI */
    Cy_SCB_SPI_Init(CY_EEPROM_SCB_TYPE, &spiCfg, &spiCtx);
    Cy_SCB_SPI_SetActiveSlaveSelect(CY_EEPROM_SCB_TYPE, CY_EEPROM_SCB_SS_IDX);
    Cy_SCB_SPI_Enable(CY_EEPROM_SCB_TYPE);
}

/* External EEPROM Utilities */

#define EEP_READ_CMD   03 // READ       instruction code
#define EEP_WRITE_CMD  02 // WRITE      instruction code
#define EEP_WRDI_CMD   04 // WR DISBALE instruction code
#define EEP_WREN_CMD   06 // WR ENABLE  instruction code
#define EEP_RDSR_CMD   05 // RD STATUS  instruction code
#define EEP_WRSR_CMD   01 // WR STATUS  instruction code
#define EEP_NOPROT_CMD 00 // NO WRITE PROTECTION

static void WriteEEPROM(uint16_t address, uint8_t data)
{
    uint8_t txData[4];
    uint8_t rxData[4];
    txData[0] = EEP_WRITE_CMD;
    txData[1] = (uint8_t)((address & 0xFF00) >> 8u);
    txData[2] = (uint8_t)(address & 0x00FF);
    txData[3] = data;

    uint32_t status;
    Cy_SCB_SPI_Transfer(CY_EEPROM_SCB_TYPE, &txData, &rxData, 4u, &spiCtx);
    /* wait for completion */
    do
    {
            status = Cy_SCB_SPI_GetTransferStatus(CY_EEPROM_SCB_TYPE, &spiCtx);
    } while((status & CY_SCB_SPI_TRANSFER_ACTIVE) != 0u);
}

static uint8_t ReadEEPROM(uint16_t address)
{
    uint8_t txData[4];
    uint8_t rxData[4];
    txData[0] = EEP_READ_CMD;
    txData[1] = (uint8_t)((address & 0xFF00) >> 8u);
    txData[2] = (uint8_t)(address & 0x00FF);
    txData[3] = 0u;

    uint32_t status;
    Cy_SCB_SPI_Transfer(CY_EEPROM_SCB_TYPE, &txData, &rxData, 4u, &spiCtx);
    /* wait for completion */
    do
    {
            status = Cy_SCB_SPI_GetTransferStatus(CY_EEPROM_SCB_TYPE, &spiCtx);
    } while((status & CY_SCB_SPI_TRANSFER_ACTIVE) != 0u);
    
    return rxData[3];
}

static uint8_t GetStatusEEPROM(void)
{
    /* RDSR */
    uint8_t txData[2];
    uint8_t rxData[2];

    txData[0] = EEP_RDSR_CMD;
    txData[1] = 0u;

    uint32_t status;
    Cy_SCB_SPI_Transfer(CY_EEPROM_SCB_TYPE, &txData, &rxData, 2u, &spiCtx);
    /* wait for completion */
    do
    {
            status = Cy_SCB_SPI_GetTransferStatus(CY_EEPROM_SCB_TYPE, &spiCtx);
    } while((status & CY_SCB_SPI_TRANSFER_ACTIVE) != 0u);
    
    return rxData[1];
}

static uint8_t GetWEL_EEPROM(void)
{
    return((GetStatusEEPROM() & 0x02u) >> 1u);
}

static uint8_t GetWIP_EEPROM(void)
{
    return(GetStatusEEPROM() & 0x01u);
}

static void WriteEnableEEPROM(void)
{
    uint8_t txData = EEP_WREN_CMD;
    uint8_t rxData;
    uint32_t status;

    Cy_SCB_SPI_Transfer(CY_EEPROM_SCB_TYPE, &txData, &rxData, 1u, &spiCtx);

    /* wait for completion */
    do
    {
        status = Cy_SCB_SPI_GetTransferStatus(CY_EEPROM_SCB_TYPE, &spiCtx);
    } while((status & CY_SCB_SPI_TRANSFER_ACTIVE) != 0u);
}

static void PageWriteEEPROM(uint16_t address, uint8_t data[], uint8_t size)
{
    uint32_t status;
    uint8_t rxData[35];
    uint8_t txData[35];

    if(size > 32u)
    {
        return;
    }

    txData[0] = EEP_WRITE_CMD;
    txData[1] = (uint8_t)((address & 0xFF00) >> 8u);
    txData[2] = (uint8_t)(address & 0x00FF);
    for(uint32_t i = 0ul; i < size; i++)
    {
        txData[3ul+i] = data[i];
    }

    Cy_SCB_SPI_Transfer(CY_EEPROM_SCB_TYPE, &txData, &rxData, (3ul+size), &spiCtx);

    /* wait for completion */
    do
    {
        status = Cy_SCB_SPI_GetTransferStatus(CY_EEPROM_SCB_TYPE, &spiCtx);
    } while((status & CY_SCB_SPI_TRANSFER_ACTIVE) != 0u);
}

/* [] END OF FILE */
