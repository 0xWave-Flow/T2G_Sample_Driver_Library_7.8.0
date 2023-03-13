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

/* User Local Definition */
#define UART_OVERSAMPLING (8ul)
#define UART_RX_UNIT_BYTE (1ul)

#define USER_LED_PORT    CY_CB_USER_LED2_PORT
#define USER_LED_PIN     CY_CB_USER_LED2_PIN
#define USER_LED_PIN_MUX CY_CB_USER_LED2_PIN_MUX

cy_stc_gpio_pin_config_t user_led_port_pin_cfg =
{
    .outVal    = 0u,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = USER_LED_PIN_MUX,
    .intEdge   = 0u,
    .intMask   = 0u,
    .vtrip     = 0u,
    .slewRate  = 0u,
    .driveSel  = 0u,
};

/* Local Functions Declaration */
void UART_Init_With_Interrupt(uint32_t boadrate, uint32_t sourceFreq);
void Scb_UART_IntrISR(void);
void Scb_UART_Event(uint32_t locEvents);

/* Local Variables */
uint8_t                   g_uart_in_data[128]; // RX Buffer
cy_stc_scb_uart_context_t g_stc_uart_context;

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);

    /* UART Initialization */
    //                       boardrate, peri frequency
    UART_Init_With_Interrupt( 115200ul,     80000000ul);

    for(;;)
    { 
        /* Start receiving */
        Cy_SCB_UART_Receive(CY_USB_SCB_UART_TYPE, &g_uart_in_data[0], UART_RX_UNIT_BYTE, &g_stc_uart_context);

        Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);

        /* Wait for receive done */
        while((Cy_SCB_UART_GetReceiveStatus(CY_USB_SCB_UART_TYPE,&g_stc_uart_context) & CY_SCB_UART_RECEIVE_ACTIVE) == CY_SCB_UART_RECEIVE_ACTIVE);

        Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
    }
}

/****************************/
/* SCB - UART Configuration */
/****************************/
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

void UART_Init_With_Interrupt(uint32_t boadrate, uint32_t sourceFreq)
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
    Cy_SCB_UART_RegisterCallback(CY_USB_SCB_UART_TYPE, Scb_UART_Event, &g_stc_uart_context);
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

    /* Interrupt Configuration for UART */
    cy_stc_sysint_irq_t stc_sysint_irq_cfg_uart;
    stc_sysint_irq_cfg_uart.sysIntSrc = CY_USB_SCB_UART_IRQN;
    stc_sysint_irq_cfg_uart.intIdx    = CPUIntIdx2_IRQn;
    stc_sysint_irq_cfg_uart.isEnabled = true;
    Cy_SysInt_InitIRQ(&stc_sysint_irq_cfg_uart);
    Cy_SysInt_SetSystemIrqVector(stc_sysint_irq_cfg_uart.sysIntSrc, Scb_UART_IntrISR);
    NVIC_EnableIRQ(stc_sysint_irq_cfg_uart.intIdx);
}

void Scb_UART_IntrISR(void)
{
    /* UART interrupt handler */
    Cy_SCB_UART_Interrupt(CY_USB_SCB_UART_TYPE, &g_stc_uart_context);

}

void Scb_UART_Event(uint32_t locEvents)
{
    switch (locEvents)
    {
    case CY_SCB_UART_TRANSMIT_IN_FIFO_EVENT:
        break;

    case CY_SCB_UART_TRANSMIT_DONE_EVENT:
        break;

    case CY_SCB_UART_RECEIVE_DONE_EVENT:
        /* Transmit received data */
        Cy_SCB_UART_Transmit(CY_USB_SCB_UART_TYPE, &g_uart_in_data[0], UART_RX_UNIT_BYTE, &g_stc_uart_context);
        break;

    case CY_SCB_UART_RB_FULL_EVENT:
        break;

    case CY_SCB_UART_RECEIVE_ERR_EVENT:
        break;

    case CY_SCB_UART_TRANSMIT_ERR_EVENT:
        break;

    default:
        break;
    }
}

/* [] END OF FILE */

