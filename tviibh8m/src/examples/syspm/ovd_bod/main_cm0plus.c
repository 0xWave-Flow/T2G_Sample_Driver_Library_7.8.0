/***************************************************************************//**
* \file main_cm0plus.c
*
* \version 1.0
*
* \brief Main example file for CM0+
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

#if (CY_USE_PSVP == 1)
  #error "This example won't work on PSVP environment."
#endif

cy_stc_gpio_pin_config_t user_led_port_pin_cfg = 
{
    .outVal    = 0u,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = CY_CB_USER_LED1_PIN_MUX,
    .intEdge   = 0u,
    .intMask   = 0u,
    .vtrip     = 0u,
    .slewRate  = 0u,
    .driveSel  = 0u,
};

/* Uart Functions Declaration */
void UART_Initialization(uint32_t boadrate, uint32_t sourceFreq);
void Term_Printf(void *fmt, ...);

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

                   //   baudrate, perifreq
    UART_Initialization(  115200, 80000000);

    Cy_GPIO_Pin_Init(CY_CB_USER_LED1_PORT, CY_CB_USER_LED1_PIN, &user_led_port_pin_cfg);

    /*********************************************************************/
    /*****         Catch reset caused by illegal voltage             *****/
    /*********************************************************************/
    uint32_t resetCause = Cy_SysReset_GetResetReason();

    Term_Printf("Reset cause: 0x%08X", resetCause);

    if(resetCause & CY_SYSRESET_BODVDDD)
    {
        Term_Printf(": BODVDDD occurred\n");
    }
    else if(resetCause & CY_SYSRESET_BODVDDA)
    {
        Term_Printf(": BODVDDA occurred\n");
    }
    else if(resetCause & CY_SYSRESET_BODVCCD)
    {
        Term_Printf(": BODVCCD occurred\n");
    }
    else if(resetCause & CY_SYSRESET_OVDVDDD)
    {
        Term_Printf(": OVDVDDD occurred\n");
    }
    else if(resetCause & CY_SYSRESET_OVDVDDA)
    {
        Term_Printf(": OVDVDDA occurred\n");
    }
    else if(resetCause & CY_SYSRESET_OVDVCCD)
    {
        Term_Printf(": OVDVCCD occurred\n");
    }
    else
    {
        Term_Printf("\n");
    }

    Cy_SysReset_ClearAllResetReasons();

    // wait 1 [s] to flush message
    Cy_SysTick_DelayInUs(1000000ul);

    /*********************************************************************/
    /*****                OVD settings                               *****/
    /*********************************************************************/
    // OVD Action Select
    Cy_Syspm_OvdActionSelect(CY_SYSPM_OVD_ACTION_RESET);
    
    // Set the VDDD threshold select for OVD
    Cy_SysPm_OvdVdddSelect(CY_SYSPM_OVD_VDDD_5_5V);
    // Enable OVD on VDDD
    Cy_SysPm_OvdEnable(CY_SYSPM_OVD_ON_VDDD);
    
    // Set the VDDA threshold select for OVD
    Cy_SysPm_OvdVddaSelect(CY_SYSPM_OVD_VDDA_5_5V);
    // Enable OVD on VDDA
    Cy_SysPm_OvdEnable(CY_SYSPM_OVD_ON_VDDA);
    
    /*********************************************************************/
    /*****                BOD settings                               *****/
    /*********************************************************************/
    // BOD Action Select
    Cy_Syspm_BodActionSelect(CY_SYSPM_BOD_ACTION_RESET);
    
    // Set the VDDD threshold select for BOD
    Cy_SysPm_BodVdddSelect(CY_SYSPM_BOD_VDDD_3_0V);
    // Enable BOD on VDDD
    Cy_SysPm_BodEnable(CY_SYSPM_BOD_ON_VDDD);
    
    // Set the VDDA threshold select for BOD
    Cy_SysPm_BodVddaSelect(CY_SYSPM_BOD_VDDA_3_0V);
    // Enable BOD on VDDA
    Cy_SysPm_BodEnable(CY_SYSPM_BOD_ON_VDDA);

    /*********************************************************************/
    /*****           Blink LED to notice MCU is working              *****/
    /*********************************************************************/
    for(;;)
    {
        Cy_SysTick_DelayInUs(50000ul);
        Cy_GPIO_Inv(CY_CB_USER_LED1_PORT, CY_CB_USER_LED1_PIN);
    }
}

/****************************************************/
/*               Uart Utilities                     */
/****************************************************/
#include <stdio.h>
#include <stdarg.h>
/* Local Definition */
#define UART_OVERSAMPLING (8ul)

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
