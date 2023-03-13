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

/*
    In this example, TVII works like as a serial memory.

    When master writes some data into TVII, master sends
    1 byte top address at first and then data consequentially.
    e.g.
        Master write 0x11, 0x22, 0x33 to address 0x01, 0x02, 0x03
        accordingly.
        - Master will send 0x01, 0x11, 0x22, 0x33

    When master reads some data from TVII, master sends 1 byte top address
    at first, and then sends read command.
     e.g.
        Master reads from address 0x04, 0x05, 0x06
        accordingly.
        - Master will send 0x04, then read 3 bytes.

    Own Address Buffer1(Primary):   0x08 (E_I2C_SLAVE_ADDR1)
    Own Address Buffer2(Secondary): 0x09 (E_I2C_SLAVE_ADDR2)
    Partner (Master) can access 2 memories of this EZI2C Slave
*/


/* Select Frequency */
/*   100KHz and 400KHz are checked on PSVP */
/*   1000KHz doesn't work on PSVP          */
#define E_I2C_DATARATE_100KHZ  100000
#define E_I2C_DATARATE_400KHZ  400000
#define E_I2C_DATARATE_1000KHZ 1000000
#define E_I2C_DATARATE         E_I2C_DATARATE_100KHZ

/* Select Frequency */
#if (CY_USE_PSVP == 1)
  #define E_SOURCE_CLK_FREQ     (24000000ul) // fixed
#else
  #define E_SOURCE_CLK_FREQ     (80000000u)  // fixed
#endif
/* Local Definition */
#if   (E_I2C_DATARATE == E_I2C_DATARATE_100KHZ)
  #define E_I2C_INCLK_TARGET_FREQ (2000000ul)
#elif (E_I2C_DATARATE == E_I2C_DATARATE_400KHZ)
  #define E_I2C_INCLK_TARGET_FREQ (9600000ul)
#else // (E_I2C_DATARATE == E_I2C_DATARATE_1000KHZ)
  #define E_I2C_INCLK_TARGET_FREQ (24000000ul)
#endif

#define DIVIDER_NO_1 (1u)

#define E_I2C_SLAVE_ADDR1           8
#define E_I2C_SLAVE_ADDR2           9
#define E_EZI2C_BUF1_SIZE           256
#define E_EZI2C_BUF1_REGION         128     // R/W:128 R:128
#define E_EZI2C_BUF2_SIZE           256
#define E_EZI2C_BUF2_REGION         192     // R/W:192 R:64

#define USER_I2C_SCB_TYPE           SCB0
#define USER_I2C_SCB_PCLK           PCLK_SCB0_CLOCK
#define USER_I2C_SCB_IRQN           scb_0_interrupt_IRQn

#define I2C_SDA_PORT     GPIO_PRT1
#define I2C_SDA_PORT_PIN (1)
#define I2C_SDA_PORT_MUX P1_1_SCB0_I2C_SDA

#define I2C_SCL_PORT     GPIO_PRT1
#define I2C_SCL_PORT_PIN (0)
#define I2C_SCL_PORT_MUX P1_0_SCB0_I2C_SCL

/* Local Functions Declaration */
void Scb_EZI2C_Slave_IntrISR(void);

/* SCB - EZI2C Configuration */
static cy_stc_scb_ezi2c_context_t  g_stc_ezi2c_context;
static cy_stc_scb_ezi2c_config_t   g_stc_ezi2c_config = 
{
    .numberOfAddresses   = CY_SCB_EZI2C_TWO_ADDRESSES,
    .slaveAddress1       = E_I2C_SLAVE_ADDR1,
    .slaveAddress2       = E_I2C_SLAVE_ADDR2,
    .subAddressSize      = CY_SCB_EZI2C_SUB_ADDR8_BITS,
    .enableWakeFromSleep = false
};

static cy_stc_sysint_irq_t irq_cfg =
{
    .sysIntSrc  = USER_I2C_SCB_IRQN,
    .intIdx     = CPUIntIdx3_IRQn,
    .isEnabled  = true,
};

/* Local Variables */
static uint8_t g_ezi2c_buf1[E_EZI2C_BUF1_SIZE] = {0u}; // Primary
static uint8_t g_ezi2c_buf2[E_EZI2C_BUF2_SIZE] = {0u}; // Secondary

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

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /*--------------------*/
    /* Port Configuration */
    /*--------------------*/
    cy_stc_gpio_pin_config_t    stc_port_pin_cfg = {0};
    stc_port_pin_cfg.driveMode = CY_GPIO_DM_OD_DRIVESLOW;
    stc_port_pin_cfg.hsiom     = I2C_SDA_PORT_MUX;
    Cy_GPIO_Pin_Init(I2C_SDA_PORT, I2C_SDA_PORT_PIN, &stc_port_pin_cfg);

    stc_port_pin_cfg.driveMode = CY_GPIO_DM_OD_DRIVESLOW;
    stc_port_pin_cfg.hsiom     = I2C_SCL_PORT_MUX;
    Cy_GPIO_Pin_Init(I2C_SCL_PORT, I2C_SCL_PORT_PIN, &stc_port_pin_cfg);

    /*---------------------*/
    /* Clock Configuration */
    /*---------------------*/
    /* Assign a programmable divider */
    Cy_SysClk_PeriphAssignDivider(USER_I2C_SCB_PCLK, CY_SYSCLK_DIV_24_5_BIT, DIVIDER_NO_1);
    SetPeripheFracDiv24_5(E_I2C_INCLK_TARGET_FREQ, E_SOURCE_CLK_FREQ, DIVIDER_NO_1);
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(USER_I2C_SCB_PCLK), CY_SYSCLK_DIV_24_5_BIT, DIVIDER_NO_1);

    /*--------------------------*/
    /* Interrupt Configuration */
    /*--------------------------*/
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, Scb_EZI2C_Slave_IntrISR);
    NVIC_SetPriority(irq_cfg.intIdx, 3ul);
    NVIC_EnableIRQ(irq_cfg.intIdx);

    /* Initialize & Enable EZI2C */
    Cy_SCB_EZI2C_DeInit(USER_I2C_SCB_TYPE);
    Cy_SCB_EZI2C_Init(USER_I2C_SCB_TYPE, &g_stc_ezi2c_config, &g_stc_ezi2c_context);
    Cy_SCB_I2C_SetDataRate(USER_I2C_SCB_TYPE, E_I2C_DATARATE, E_I2C_INCLK_TARGET_FREQ);
    Cy_SCB_EZI2C_SetBuffer1(USER_I2C_SCB_TYPE, &g_ezi2c_buf1[0], E_EZI2C_BUF1_SIZE, E_EZI2C_BUF1_REGION, &g_stc_ezi2c_context);
    Cy_SCB_EZI2C_SetBuffer2(USER_I2C_SCB_TYPE, &g_ezi2c_buf2[0], E_EZI2C_BUF2_SIZE, E_EZI2C_BUF2_REGION, &g_stc_ezi2c_context);
    Cy_SCB_SetEzMode(USER_I2C_SCB_TYPE, false); // To execute SW EZI2C, EZ mode need to be disable
    Cy_SCB_EZI2C_Enable(USER_I2C_SCB_TYPE);

    /* EZI2C Slave Mode Test      */
    /*   Primary Address:    0x08 */
    /*   Secondary Address:  0x09 */

    /* Set Initial Value to Buffer for Buffer Test */
    for (uint32_t i = 0; i < E_EZI2C_BUF1_SIZE; i++)
    {
        g_ezi2c_buf1[i] = i;
    }

    for(;;);
}

void Scb_EZI2C_Slave_IntrISR(void)
{
    /* EZI2C interrupt handler */
    Cy_SCB_EZI2C_Interrupt(USER_I2C_SCB_TYPE, &g_stc_ezi2c_context);
}

/* [] END OF FILE */
