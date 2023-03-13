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

/* 
   This example works as SPI MOTOROLA mode slave.
   It prepares four 16bits length data defined in
   "initialWriteData" into its TX FIFO previously.
   If master sends data to TVII it will response with the data and
   will store received data into its TX FIFO.
*/

/* Slave Settings */
#define SCB_MISO_DRIVE_MODE CY_GPIO_DM_STRONG_IN_OFF
#define SCB_MOSI_DRIVE_MODE CY_GPIO_DM_HIGHZ
#define SCB_CLK_DRIVE_MODE  CY_GPIO_DM_HIGHZ
#define SCB_SEL0_DRIVE_MODE CY_GPIO_DM_HIGHZ

/* User setting value */
#if (CY_USE_PSVP == 1)  
  #define SOURCE_CLOCK_FRQ 24000000ul
#else
  #define SOURCE_CLOCK_FRQ 80000000ul
#endif
#define SCB_SPI_BAUDRATE     125000ul /* Please set baudrate value of SPI you want */
#define SCB_SPI_OVERSAMPLING 16ul     /* Please set oversampling of SPI you want */
#define SCB_SPI_CLOCK_FREQ   (SCB_SPI_BAUDRATE * SCB_SPI_OVERSAMPLING)

#define DIVIDER_NO_1 (1u)

static cy_stc_gpio_pin_config_t SPI_port_pin_cfg =
{
    .outVal    = 0ul,
    .driveMode = 0ul,            /* Will be updated in runtime */
    .hsiom     = HSIOM_SEL_GPIO, /* Will be updated in runtime */
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

static cy_stc_sysint_irq_t irq_cfg =
{
    .sysIntSrc  = CY_SPI_SCB_IRQN,
    .intIdx     = CPUIntIdx3_IRQn,
    .isEnabled  = true,
};
uint16_t readData[2];
uint16_t initialWriteData[4] = {0x1122u, 0x3344u, 0x5566u, 0x7788u};
#define SIZE_OF_INITIAL_DATA (sizeof(initialWriteData)/sizeof(uint16_t))

static const cy_stc_scb_spi_config_t SCB_SPI_cfg =
{
    .spiMode                    = CY_SCB_SPI_SLAVE,       /*** Specifies the mode of operation    ***/
    .subMode                    = CY_SCB_SPI_MOTOROLA,    /*** Specifies the submode of SPI operation    ***/
    .sclkMode                   = CY_SCB_SPI_CPHA0_CPOL0, /*** Clock is active low, data is changed on first edge ***/
    .oversample                 = SCB_SPI_OVERSAMPLING,   /*** SPI_CLOCK divided by SCB_SPI_OVERSAMPLING should be baudrate  ***/
    .rxDataWidth                = 16ul,                   /*** The width of RX data (valid range 4-16). It must be the same as \ref txDataWidth except in National sub-mode. ***/
    .txDataWidth                = 16ul,                   /*** The width of TX data (valid range 4-16). It must be the same as \ref rxDataWidth except in National sub-mode. ***/
    .enableMsbFirst             = true,                   /*** Enables the hardware to shift out the data element MSB first, otherwise, LSB first ***/
    .enableFreeRunSclk          = false,                  /*** Enables the master to generate a continuous SCLK regardless of whether there is data to send  ***/
    .enableInputFilter          = false,                  /*** Enables a digital 3-tap median filter to be applied to the input of the RX FIFO to filter glitches on the line. ***/
    .enableMisoLateSample       = true,                   /*** Enables the master to sample MISO line one half clock later to allow better timings. ***/
    .enableTransferSeperation   = true,                   /*** Enables the master to transmit each data element separated by a de-assertion of the slave select line (only applicable for the master mode) ***/
    .ssPolarity0                = false,                  /*** SS0: active low ***/
    .ssPolarity1                = false,                  /*** SS1: active low ***/
    .ssPolarity2                = false,                  /*** SS2: active low ***/
    .ssPolarity3                = false,                  /*** SS3: active low ***/
    .enableWakeFromSleep        = false,                  /*** When set, the slave will wake the device when the slave select line becomes active. Note that not all SCBs support this mode. Consult the device datasheet to determine which SCBs support wake from deep sleep. ***/
    .rxFifoTriggerLevel         = 1ul,                    /*** Interrupt occurs, when there are more entries of 2 in the RX FIFO */
    .rxFifoIntEnableMask        = 1ul,                    /*** Bits set in this mask will allow events to cause an interrupt  */
    .txFifoTriggerLevel         = 1ul,                    /*** When there are fewer entries in the TX FIFO, then at this level the TX trigger output goes high. This output can be connected to a DMA channel through a trigger mux. Also, it controls the \ref CY_SCB_SPI_TX_TRIGGER interrupt source. */
    .txFifoIntEnableMask        = 1ul,                    /*** Bits set in this mask allow events to cause an interrupt  */
    .masterSlaveIntEnableMask   = 0ul,                    /*** Bits set in this mask allow events to cause an interrupt  */
    .enableSpiDoneInterrupt     = false,
    .enableSpiBusErrorInterrupt = false,
};

void SetPeripheFracDiv24_5(uint64_t targetFreq, uint64_t sourceFreq, uint8_t divNum)
{
    uint64_t temp = ((uint64_t)sourceFreq << 5ull);
    uint32_t divSetting;

    divSetting = (uint32_t)(temp / targetFreq);
    Cy_SysClk_PeriphSetFracDivider(Cy_SysClk_GetClockGroup(CY_SPI_SCB_PCLK), 
                                   CY_SYSCLK_DIV_24_5_BIT, divNum, 
                                   (((divSetting >> 5ul) & 0x00000FFFul) - 1ul), 
                                   (divSetting & 0x0000001Ful));
}

void irqSCB(void)
{
    uint32_t status;

    status = Cy_SCB_SPI_GetRxFifoStatus(CY_SPI_SCB_TYPE);
    if(status & CY_SCB_SPI_RX_TRIGGER)
    {
        /*** Read data from RX FIFO ***/
        Cy_SCB_SPI_ReadArray(CY_SPI_SCB_TYPE, (void*)readData, 2ul);
        Cy_SCB_SPI_WriteArray(CY_SPI_SCB_TYPE, (void*)readData, 2ul);
        Cy_SCB_SPI_ClearRxFifoStatus(CY_SPI_SCB_TYPE, CY_SCB_SPI_RX_TRIGGER);
    }

    status = Cy_SCB_SPI_GetTxFifoStatus(CY_SPI_SCB_TYPE);
    if(status & CY_SCB_SPI_TX_TRIGGER)
    {
        /*** Write back the data to TX FIFO ***/
        Cy_SCB_SPI_WriteArray(CY_SPI_SCB_TYPE, (void*)initialWriteData, SIZE_OF_INITIAL_DATA);
        Cy_SCB_SPI_ClearTxFifoStatus(CY_SPI_SCB_TYPE, CY_SCB_SPI_TX_TRIGGER);
    }
}

int main(void)
{
    SystemInit();

    /******************************************************/
    /******* Calculate divider setting for the SCB ********/
    /******************************************************/

    Cy_SysClk_PeriphAssignDivider(CY_SPI_SCB_PCLK, CY_SYSCLK_DIV_24_5_BIT, DIVIDER_NO_1);
    SetPeripheFracDiv24_5(SCB_SPI_CLOCK_FREQ, SOURCE_CLOCK_FRQ, DIVIDER_NO_1);
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(CY_SPI_SCB_PCLK), CY_SYSCLK_DIV_24_5_BIT, 1u);

    __enable_irq(); /* Enable global interrupts. */

    /*******************************************/
    /*   De-initialization for peripherals     */
    /*******************************************/
    Cy_SCB_SPI_DeInit(CY_SPI_SCB_TYPE);

    /*******************************************/
    /* Interrupt setting for SPI communication */
    /*******************************************/
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, irqSCB);
    NVIC_EnableIRQ(irq_cfg.intIdx);

    /**************************************/
    /* Port Setting for SPI communication */
    /**************************************/
    /* According to the HW environment to change SCB CH*/
    SPI_port_pin_cfg.driveMode = SCB_MISO_DRIVE_MODE;
    SPI_port_pin_cfg.hsiom = CY_SPI_SCB_MISO_MUX;
    Cy_GPIO_Pin_Init(CY_SPI_SCB_MISO_PORT, CY_SPI_SCB_MISO_PIN, &SPI_port_pin_cfg);

    SPI_port_pin_cfg.driveMode = SCB_MOSI_DRIVE_MODE;
    SPI_port_pin_cfg.hsiom = CY_SPI_SCB_MOSI_MUX;
    Cy_GPIO_Pin_Init(CY_SPI_SCB_MOSI_PORT, CY_SPI_SCB_MOSI_PIN, &SPI_port_pin_cfg);

    SPI_port_pin_cfg.driveMode = SCB_CLK_DRIVE_MODE;
    SPI_port_pin_cfg.hsiom = CY_SPI_SCB_CLK_MUX;
    Cy_GPIO_Pin_Init(CY_SPI_SCB_CLK_PORT,CY_SPI_SCB_CLK_PIN, &SPI_port_pin_cfg);
    
    SPI_port_pin_cfg.driveMode = SCB_SEL0_DRIVE_MODE;
    SPI_port_pin_cfg.hsiom = CY_SPI_SCB_SEL0_MUX;
    Cy_GPIO_Pin_Init(CY_SPI_SCB_SEL0_PORT, CY_SPI_SCB_SEL0_PIN, &SPI_port_pin_cfg);

    /********************************************/
    /* SCB initialization for SPI communication */
    /********************************************/
    Cy_SCB_SPI_Init(CY_SPI_SCB_TYPE, &SCB_SPI_cfg, NULL);
    Cy_SCB_SPI_SetActiveSlaveSelect(CY_SPI_SCB_TYPE, 0ul);
    Cy_SCB_SPI_Enable(CY_SPI_SCB_TYPE);

    for(;;);
}
/* [] END OF FILE */
