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

#include <stdio.h>
#include <stdarg.h>

#include "cy_project.h"
#include "cy_device_headers.h"

/* I2C Master Mode Test                              */
/*                                                   */
/* Partner Address(Slave): 0x08 (E_I2C_SLAVE_ADDR)   */

#define USER_I2C_SCB_TYPE       SCB0
#define USER_I2C_SCB_PCLK       PCLK_SCB0_CLOCK
#define USER_I2C_SCB_IRQN       scb_0_interrupt_IRQn

#define I2C_SDA_PORT     GPIO_PRT1
#define I2C_SDA_PORT_PIN (1)
#define I2C_SDA_PORT_MUX P1_1_SCB0_I2C_SDA

#define I2C_SCL_PORT     GPIO_PRT1
#define I2C_SCL_PORT_PIN (0)
#define I2C_SCL_PORT_MUX P1_0_SCB0_I2C_SCL

#define DIVIDER_NO_1 (1u)

/* Select Frequency */
#if (CY_USE_PSVP == 1)
  #define E_SOURCE_CLK_FREQ     (24000000ul) // fixed
#else
  #define E_SOURCE_CLK_FREQ     (80000000u)  // fixed
#endif
#define E_I2C_INCLK_TARGET_FREQ (2000000ul)  // modifiable
#define E_I2C_DATARATE          (100000ul)   // modifiable

#define E_I2C_SLAVE_ADDR        8
#define E_I2C_RECV_SIZE         9

static cy_stc_gpio_pin_config_t I2S_port_pin_cfg =
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
    .sysIntSrc  = USER_I2C_SCB_IRQN,
    .intIdx     = CPUIntIdx3_IRQn,
    .isEnabled  = true,
};

/* SCB - I2C Configuration */
static cy_stc_scb_i2c_context_t g_stc_i2c_context;
static const cy_stc_scb_i2c_config_t  g_stc_i2c_config =
{
    .i2cMode             = CY_SCB_I2C_MASTER,
    .useRxFifo           = true,
    .useTxFifo           = true,
    .slaveAddress        = E_I2C_SLAVE_ADDR,
    .slaveAddressMask    = E_I2C_SLAVE_ADDR,
    .acceptAddrInFifo    = false,
    .ackGeneralAddr      = false,
    .enableWakeFromSleep = false
};

static cy_stc_scb_i2c_master_xfer_config_t g_stc_i2c_master_config =
{
    .slaveAddress = E_I2C_SLAVE_ADDR,
    .buffer       = 0,
    .bufferSize   = 0,
    .xferPending  = false
};

/* Local Variables */

void Scb_I2C_IntrISR(void)
{
    /* I2C interrupt handler for High-Level APIs */
    Cy_SCB_I2C_Interrupt(USER_I2C_SCB_TYPE, &g_stc_i2c_context);
}

void SetPeripheFracDiv24_5(uint64_t targetFreq, uint64_t sourceFreq, uint8_t divNum)
{
    uint64_t temp = ((uint64_t)sourceFreq << 5ull);
    uint32_t divSetting;

    divSetting = (uint32_t)(temp / targetFreq);
    Cy_SysClk_PeriphSetFracDivider(Cy_SysClk_GetClockGroup(USER_I2C_SCB_PCLK), 
                                   CY_SYSCLK_DIV_24_5_BIT, divNum, 
                                   (((divSetting >> 5u) & 0x00000FFFul) - 1ul), 
                                   (divSetting & 0x0000001Ful));
}

void Scb_I2C_Master_LowLevelAPI_Test(void)
{
    /*------------------------*/
    /* I2C Master Byte Write  */
    /*------------------------*/

    /* Make sure TX FIFO empty */
    while(Cy_SCB_GetNumInTxFifo(USER_I2C_SCB_TYPE) != 0ul);

    /* Send START and Receive ACK/NACK */
    CY_ASSERT(Cy_SCB_I2C_MasterSendStart(USER_I2C_SCB_TYPE, E_I2C_SLAVE_ADDR, CY_SCB_I2C_WRITE_XFER, 2000ul, &g_stc_i2c_context) == CY_SCB_I2C_SUCCESS);

    /* Transmit One Byte Data */
    static uint8_t g_send_byte = 0xF1u;
    CY_ASSERT(Cy_SCB_I2C_MasterWriteByte(USER_I2C_SCB_TYPE, g_send_byte, 2000ul, &g_stc_i2c_context) == CY_SCB_I2C_SUCCESS);

    /* Send STOP */
    CY_ASSERT(Cy_SCB_I2C_MasterSendWriteStop(USER_I2C_SCB_TYPE, 2000ul, &g_stc_i2c_context) == CY_SCB_I2C_SUCCESS);

    /*------------------------*/
    /* I2C Master Byte Read   */
    /*------------------------*/

    /* Make sure RX FIFO empty */
    while(Cy_SCB_GetNumInRxFifo(USER_I2C_SCB_TYPE) != 0ul);

    /* Send START and Receive ACK/NACK */
    CY_ASSERT(Cy_SCB_I2C_MasterSendStart(USER_I2C_SCB_TYPE, E_I2C_SLAVE_ADDR, CY_SCB_I2C_READ_XFER, 2000ul, &g_stc_i2c_context) == CY_SCB_I2C_SUCCESS);

    /* Receive One Byte Data */
    static uint8_t g_recv_byte = 0x00u;
    CY_ASSERT(Cy_SCB_I2C_MasterReadByte(USER_I2C_SCB_TYPE, CY_SCB_I2C_NAK, &g_recv_byte, 2000ul, &g_stc_i2c_context) == CY_SCB_I2C_SUCCESS);

    /* Send NACK (and stop) */
    CY_ASSERT(Cy_SCB_I2C_MasterSendReadStop(USER_I2C_SCB_TYPE, 2000ul, &g_stc_i2c_context) == CY_SCB_I2C_SUCCESS);
}

void Scb_I2C_Master_HighLevelAPI_Test(void)
{
    /* I2C Master Write */
    static uint8_t g_i2c_tx_buf[32] = "ABCDEFG012345!";
    g_stc_i2c_master_config.buffer     = &g_i2c_tx_buf[0];
    g_stc_i2c_master_config.bufferSize = strlen((char *)g_i2c_tx_buf);
    CY_ASSERT(Cy_SCB_I2C_MasterWrite(USER_I2C_SCB_TYPE, &g_stc_i2c_master_config, &g_stc_i2c_context) == CY_SCB_I2C_SUCCESS);

    Cy_SysTick_DelayInUs(100000ul);

    /* I2C Master Read */
    static uint8_t g_i2c_rx_buf[32];
    g_stc_i2c_master_config.buffer     = &g_i2c_rx_buf[0];
    g_stc_i2c_master_config.bufferSize = E_I2C_RECV_SIZE;
    CY_ASSERT(Cy_SCB_I2C_MasterRead(USER_I2C_SCB_TYPE, &g_stc_i2c_master_config, &g_stc_i2c_context) == CY_SCB_I2C_SUCCESS);
}

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /*---------------------*/
    /* Clock Configuration */
    /*---------------------*/
    Cy_SysClk_PeriphAssignDivider(USER_I2C_SCB_PCLK, CY_SYSCLK_DIV_24_5_BIT, DIVIDER_NO_1);
    SetPeripheFracDiv24_5(E_I2C_INCLK_TARGET_FREQ, E_SOURCE_CLK_FREQ, DIVIDER_NO_1);
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(USER_I2C_SCB_PCLK), CY_SYSCLK_DIV_24_5_BIT, DIVIDER_NO_1);

    /*--------------------*/
    /* Port Configuration */
    /*--------------------*/
    I2S_port_pin_cfg.driveMode = CY_GPIO_DM_OD_DRIVESLOW;
    I2S_port_pin_cfg.hsiom     = I2C_SDA_PORT_MUX;
    Cy_GPIO_Pin_Init(I2C_SDA_PORT, I2C_SDA_PORT_PIN, &I2S_port_pin_cfg);

    I2S_port_pin_cfg.driveMode = CY_GPIO_DM_OD_DRIVESLOW;
    I2S_port_pin_cfg.hsiom     = I2C_SCL_PORT_MUX;
    Cy_GPIO_Pin_Init(I2C_SCL_PORT, I2C_SCL_PORT_PIN, &I2S_port_pin_cfg);

    /*--------------------------*/
    /* Interrupt Configuration */
    /*--------------------------*/
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, Scb_I2C_IntrISR);
    NVIC_SetPriority(irq_cfg.intIdx, 3ul);
    NVIC_EnableIRQ(irq_cfg.intIdx);

    /*--------------------------*/
    /*  Initialize & Enable I2C  */
    /*--------------------------*/
    Cy_SCB_I2C_DeInit(USER_I2C_SCB_TYPE);
    Cy_SCB_I2C_Init(USER_I2C_SCB_TYPE, &g_stc_i2c_config, &g_stc_i2c_context);
    Cy_SCB_I2C_SetDataRate(USER_I2C_SCB_TYPE, E_I2C_DATARATE, E_I2C_INCLK_TARGET_FREQ);
    Cy_SCB_I2C_RegisterEventCallback(USER_I2C_SCB_TYPE, NULL, &g_stc_i2c_context);
    Cy_SCB_I2C_Enable(USER_I2C_SCB_TYPE);

    /* I2C Master Mode Test */
    Scb_I2C_Master_LowLevelAPI_Test();

    Scb_I2C_Master_HighLevelAPI_Test();

    for(;;);
}
/* [] END OF FILE */

