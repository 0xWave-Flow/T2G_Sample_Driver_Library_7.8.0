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

#include <stdio.h>
#include <stdarg.h>

#include "cy_project.h"
#include "cy_device_headers.h"

/**********************************************************************/
/* Parameters Depending on TVII MCU Series                            */
/**********************************************************************/
#define DW1_CH_TO_SCB_UART_TX    (22)
#define DW1_CH_TO_SCB_UART_RX    (23)
#define UART_TX_TO_DMA_TRIG      (TRIG_IN_1TO1_2_SCB_TX_TO_PDMA13)
#define UART_RX_TO_DMA_TRIG      (TRIG_IN_1TO1_2_SCB_RX_TO_PDMA13)


/********************************/
/**** UART and DW Variables *****/
/********************************/
#define UART_RX_FIFO_LEVEL           (0ul)
#define UART_RX_BUFFER_SIZE          (256ul)
#define UART_RX_BUFFER_BLOCK_NUM     (UART_RX_BUFFER_SIZE / (UART_RX_FIFO_LEVEL + 1ul))
#define UART_TX_FIFO_LEVEL           (16ul)
#define UART_TX_BUFFER_SIZE          (256ul)

uint32_t g_uart_tx_data_amount = 0;
uint8_t  g_uart_tx_buffer[UART_TX_BUFFER_SIZE] = { '\0' };
uint8_t  g_uart_rx_buffer[UART_RX_BUFFER_SIZE] = { '\0' };
/************************/
/**** UART Utilities ****/
/************************/
#define UART_OVERSAMPLING (8)
cy_stc_scb_uart_context_t g_stc_uart_context;
cy_stc_scb_uart_config_t  g_stc_uart_config =
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
    .rxFifoTriggerLevel         = UART_RX_FIFO_LEVEL,
    .rxFifoIntEnableMask        = 0ul,
    .txFifoTriggerLevel         = UART_TX_FIFO_LEVEL,
    .txFifoIntEnableMask        = 0ul
};

void UART_Initialization(uint32_t boadrate, uint32_t sourceFreq)
{
    /* Port Configuration for UART */
    cy_stc_gpio_pin_config_t    stc_port_pin_cfg_uart = {0};
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
    Cy_SysClk_PeriphAssignDivider(CY_USB_SCB_UART_PCLK, CY_SYSCLK_DIV_24_5_BIT, 0u);
    // Set divider value
    {
        uint64_t targetFreq     = UART_OVERSAMPLING * boadrate;
        uint64_t sourceFreq_fp5 = ((uint64_t)sourceFreq << 5ull);
        uint32_t divSetting_fp5 = (uint32_t)(sourceFreq_fp5 / targetFreq);
        Cy_SysClk_PeriphSetFracDivider(Cy_SysClk_GetClockGroup(CY_USB_SCB_UART_PCLK),
                                       CY_SYSCLK_DIV_24_5_BIT,
                                       0u,
                                       ((divSetting_fp5 & 0x1FFFFFE0ul) >> 5ul),
                                       (divSetting_fp5 & 0x0000001Ful));
    }
    // Enable peripheral divider
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(CY_USB_SCB_UART_PCLK), CY_SYSCLK_DIV_24_5_BIT, 0u);
}

/****************/
/* DW Utilities */
/****************/
static  cy_stc_pdma_descr_t         dwUartTxDescr;
const   cy_stc_pdma_chnl_config_t   dwUartTxConfig =
{
    .PDMA_Descriptor = &dwUartTxDescr,
    .preemptable     = 0ul,
    .priority        = 0ul,
    .enable          = 1ul,        /* enabled after initialization */
};
static  cy_stc_pdma_descr_config_t  dw1ChUartTxConfig =
{
    .deact          =   0ul,
    .intrType       =   CY_PDMA_INTR_X_LOOP_CMPLT, // Won't used
    .trigoutType    =   CY_PDMA_TRIGOUT_X_LOOP_CMPLT, // Won't used
    .chStateAtCmplt =   CY_PDMA_CH_DISABLED,
    .triginType     =   CY_PDMA_TRIGIN_DESCR,
    .dataSize       =   CY_PDMA_BYTE,
    .srcTxfrSize    =   0ul, // same as "dataSize"
    .destTxfrSize   =   1ul, // 32bit width
    .descrType      =   CY_PDMA_2D_TRANSFER,
    .srcAddr        =   (void *)g_uart_tx_buffer,
    .destAddr       =   (void *)&CY_USB_SCB_UART_TYPE->unTX_FIFO_WR.u32Register,
    .srcXincr       =   1ul,
    .destXincr      =   0ul,
    .xCount         =   UART_TX_FIFO_LEVEL,
    .srcYincr       =   UART_TX_FIFO_LEVEL,
    .destYincr      =   0ul,
    .yCount         =   0ul, // will be updated in run time
    .descrNext      =   &dwUartTxDescr
};

static  cy_stc_pdma_descr_t         dwUartRxDescr;
const   cy_stc_pdma_chnl_config_t   dwUartRxConfig =
{
    .PDMA_Descriptor = &dwUartRxDescr,
    .preemptable     = 0ul,
    .priority        = 0ul,
    .enable          = 1ul,        /* enabled after initialization */
};
static  cy_stc_pdma_descr_config_t  dw1ChUartRxConfig =
{
    .deact          =   0ul,
    .intrType       =   CY_PDMA_INTR_X_LOOP_CMPLT,
    .trigoutType    =   CY_PDMA_TRIGOUT_X_LOOP_CMPLT,
    .chStateAtCmplt =   CY_PDMA_CH_ENABLED,
    .triginType     =   CY_PDMA_TRIGIN_XLOOP,
    .dataSize       =   CY_PDMA_BYTE,
    .srcTxfrSize    =   1ul, // 32bit width
    .destTxfrSize   =   0ul, // Same as dataSize
    .descrType      =   CY_PDMA_2D_TRANSFER,
    .srcAddr        =   (void *)&CY_USB_SCB_UART_TYPE->unRX_FIFO_RD.u32Register,
    .destAddr       =   (void *)g_uart_rx_buffer,
    .srcXincr       =   0ul,
    .destXincr      =   1ul,
    .xCount         =   (UART_RX_FIFO_LEVEL+1ul),
    .srcYincr       =   0ul,
    .destYincr      =   (UART_RX_FIFO_LEVEL+1ul),
    .yCount         =   UART_RX_BUFFER_BLOCK_NUM,
    .descrNext      =   &dwUartRxDescr
};

/***************************/
/**** UART-DW Utilities ****/
/***************************/
void Term_printf(void *fmt, ...)
{
    va_list arg;

    /* Push input data to tx buffer */
    va_start(arg, fmt);
    int32_t writtenSize = vsprintf((char*)&g_uart_tx_buffer[g_uart_tx_data_amount], (char*)fmt, arg);
    g_uart_tx_data_amount += writtenSize;
    CY_ASSERT(g_uart_tx_data_amount < UART_TX_BUFFER_SIZE);
    va_end(arg);
}

void Term_fflush(void)
{
    /* DW transfer "UART_TX_FIFO_LEVEL" by "UART_TX_FIFO_LEVEL". thus, Padding rest space with ' ' */
    uint32_t paddingSize = UART_TX_FIFO_LEVEL - (g_uart_tx_data_amount % UART_TX_FIFO_LEVEL);
    memset(&g_uart_tx_buffer[g_uart_tx_data_amount], ' ', (paddingSize - 1));

    /* Add next line at the end */
    g_uart_tx_buffer[g_uart_tx_data_amount + paddingSize - 1] = '\n';

    /* Set Y count as its number will be (g_uart_rx_data_amount / UART_TX_FIFO_LEVEL) + 1 */
    dwUartTxDescr.unPDMA_DESCR_Y_CTL.stcField.u8Y_COUNT = (g_uart_tx_data_amount / UART_TX_FIFO_LEVEL);

    /* Set enabled status */
    Cy_PDMA_Chnl_Enable(DW1, DW1_CH_TO_SCB_UART_TX);

    /* Clear data amount */
    g_uart_tx_data_amount = 0;
}

/*********************/
/***** Main Code *****/
/*********************/
int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /**************************************************/
    /* DW dedicating to UART TX and RX Initialization */
    /**************************************************/
    Cy_PDMA_Disable(DW1);

    /* DW channel dedicating to UART TX Initialization */
    Cy_PDMA_Chnl_DeInit(DW1, DW1_CH_TO_SCB_UART_TX);
    Cy_PDMA_Descr_Init(&dwUartTxDescr, &dw1ChUartTxConfig);
    Cy_PDMA_Chnl_Init(DW1, DW1_CH_TO_SCB_UART_TX, &dwUartTxConfig);
    Cy_PDMA_Chnl_SetInterruptMask(DW1, DW1_CH_TO_SCB_UART_TX);

    /* DW channel dedicating to UART RX Initialization */
    Cy_PDMA_Chnl_DeInit(DW1, DW1_CH_TO_SCB_UART_RX);
    Cy_PDMA_Descr_Init(&dwUartRxDescr, &dw1ChUartRxConfig);
    Cy_PDMA_Chnl_Init(DW1, DW1_CH_TO_SCB_UART_RX, &dwUartRxConfig);
    Cy_PDMA_Chnl_SetInterruptMask(DW1, DW1_CH_TO_SCB_UART_RX);

    Cy_PDMA_Enable(DW1);
	
    /***********************/
    /* UART Initialization */
    /***********************/
    //                  boardrate, peri frequency
    UART_Initialization( 115200ul,     80000000ul);

    /**************************************/
    /* Trigger Multiplexer Initialization */
    /**************************************/
    /* UART TX To DW */
    Cy_TrigMux_Connect1To1(UART_TX_TO_DMA_TRIG, 0ul, TRIGGER_TYPE_LEVEL, 0ul);
    /* UART RX To DW */
    Cy_TrigMux_Connect1To1(UART_RX_TO_DMA_TRIG, 0ul, TRIGGER_TYPE_LEVEL, 0ul);

    /* Wait 1ms  */
    Cy_SysTick_DelayInUs(10000ul); // Need to check why this is required.

    Term_printf("Input data will be dumped at the interval of 5[s]");
    Term_fflush();

    for(;;)
    {
        /* Wait 5 [s]  */
        Cy_SysTick_DelayInUs(5000000ul);

        // Dump Rx Buffer
        Term_printf(g_uart_rx_buffer);
        Term_fflush();

        // Clean up RX buffer
        memset(g_uart_rx_buffer, '\0', UART_RX_BUFFER_SIZE);

        // Reset RX DW State
        Cy_PDMA_Chnl_DeInit(DW1, DW1_CH_TO_SCB_UART_RX);
        Cy_PDMA_Chnl_Init(DW1, DW1_CH_TO_SCB_UART_RX, &dwUartRxConfig);
    }
}

/* [] END OF FILE */

