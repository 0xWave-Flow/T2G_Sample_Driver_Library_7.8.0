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

/* Select UART Echo Type                                                   */
/* Use Low-Level API. Polling & Receive by 1 byte unit                     */

/* Local Definition */
#define UART_OVERSAMPLING (8ul)

/* Local Functions Declaration */
void UART_Initialization(uint32_t boadrate, uint32_t sourceFreq);
void Term_Printf(void *fmt, ...);


/* SCB - UART Configuration */
cy_stc_scb_uart_context_t   g_stc_uart_context;
cy_stc_scb_uart_config_t    g_stc_uart_config =
{
    .uartMode                   = CY_SCB_UART_STANDARD,
    .oversample                 = UART_OVERSAMPLING,
    .dataWidth                  = 8ul,
    .enableMsbFirst             = false,
    .stopBits                   = CY_SCB_UART_STOP_BITS_1,
    .parity                     = CY_SCB_UART_PARITY_NONE,
    .enableInputFilter          = false,
    .dropOnParityError          = false,
    .dropOnFrameError           = false,
    .enableMutliProcessorMode   = false,
    .receiverAddress            = 0ul,
    .receiverAddressMask        = 0ul,
    .acceptAddrInFifo           = false,
    .irdaInvertRx               = false,
    .irdaEnableLowPowerReceiver = false,
    .smartCardRetryOnNack       = false,
    .enableCts                  = false,
    .ctsPolarity                = CY_SCB_UART_ACTIVE_LOW,
    .rtsRxFifoLevel             = 0ul,
    .rtsPolarity                = CY_SCB_UART_ACTIVE_LOW,
    .breakWidth                 = 0ul,
    .rxFifoTriggerLevel         = 0ul,
    .rxFifoIntEnableMask        = 0ul,
    .txFifoTriggerLevel         = 0ul,
    .txFifoIntEnableMask        = 0ul
};

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /* UART Initialization */
    //                  boardrate, peri frequency
    UART_Initialization( 115200ul,     80000000ul);

    /* Opening UART Comment */
    Term_Printf("\nUART TEST (driver ver=%d.%d)\n\r", CY_SCB_DRV_VERSION_MAJOR, CY_SCB_DRV_VERSION_MINOR);
    Term_Printf("POLLING 1BYTE ECHO\n\r");

    for(;;)
    {
        uint32_t    rx_num;
        /* Check Receive Data */
        rx_num = Cy_SCB_UART_GetNumInRxFifo(CY_USB_SCB_UART_TYPE);
        if (rx_num != 0ul)
        {
            uint8_t uart_in_data[128];
            Cy_SCB_UART_GetArray(CY_USB_SCB_UART_TYPE, uart_in_data, rx_num);
            Cy_SCB_UART_PutArray(CY_USB_SCB_UART_TYPE, uart_in_data, rx_num);
        }
    }
}

void UART_Initialization(uint32_t boadrate, uint32_t sourceFreq)
{
    /* Port Configuration for UART */
    cy_stc_gpio_pin_config_t    stc_port_pin_cfg_uart = {0ul};
    stc_port_pin_cfg_uart.driveMode = CY_GPIO_DM_HIGHZ;
    stc_port_pin_cfg_uart.hsiom     = CY_USB_SCB_UART_RX_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_USB_SCB_UART_RX_PORT, CY_USB_SCB_UART_RX_PIN, &stc_port_pin_cfg_uart);

    stc_port_pin_cfg_uart.driveMode = CY_GPIO_DM_STRONG_IN_OFF;
    stc_port_pin_cfg_uart.hsiom     = CY_USB_SCB_UART_TX_PIN_MUX;
    Cy_GPIO_Pin_Init(CY_USB_SCB_UART_TX_PORT, CY_USB_SCB_UART_TX_PIN, &stc_port_pin_cfg_uart);

    /* SCB-UART Initialization */
    Cy_SCB_UART_DeInit(CY_USB_SCB_UART_TYPE);
    Cy_SCB_UART_Init(CY_USB_SCB_UART_TYPE, &g_stc_uart_config, &g_stc_uart_context);
    Cy_SCB_UART_Enable(CY_USB_SCB_UART_TYPE);

    /* Clock Configuration for UART */
    // Assign a programmable divider
    Cy_SysClk_PeriphAssignDivider(CY_USB_SCB_UART_PCLK, CY_SYSCLK_DIV_24_5_BIT, 0ul);
    // Set divider value
    {
        uint64_t targetFreq     = UART_OVERSAMPLING * boadrate;
        uint64_t sourceFreq_fp5 = ((uint64_t)sourceFreq << 5ull);
        uint32_t divSetting_fp5 = (uint32_t)(sourceFreq_fp5 / targetFreq);
        Cy_SysClk_PeriphSetFracDivider(Cy_SysClk_GetClockGroup(CY_USB_SCB_UART_PCLK),
                                       CY_SYSCLK_DIV_24_5_BIT,
                                       0ul,
                                       ((divSetting_fp5 & 0x1FFFFFE0ul) >> 5ul),
                                       (divSetting_fp5 & 0x0000001Ful));
    }
    // Enable peripheral divider
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(CY_USB_SCB_UART_PCLK), CY_SYSCLK_DIV_24_5_BIT, 0ul);
}

void Term_Printf(void *fmt, ...)
{
    uint8_t uart_out_data[128];
    va_list arg;

    /* UART Print */
    va_start(arg, fmt);
    vsprintf((char*)&uart_out_data[0], (char*)fmt, arg);
    while (Cy_SCB_UART_IsTxComplete(CY_USB_SCB_UART_TYPE) != true) {};
    Cy_SCB_UART_PutArray(CY_USB_SCB_UART_TYPE, uart_out_data, strlen((char *)uart_out_data));
    va_end(arg);
}

/* [] END OF FILE */
