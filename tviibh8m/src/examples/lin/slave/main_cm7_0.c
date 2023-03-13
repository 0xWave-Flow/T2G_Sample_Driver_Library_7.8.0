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

/* LIN Channel in use */
#define CY_LINCH_TYPE       CY_LINCH0_TYPE
#define CY_LINCH_RX_PORT    CY_LINCH0_RX_PORT
#define CY_LINCH_RX_PIN     CY_LINCH0_RX_PIN
#define CY_LINCH_RX_PIN_MUX CY_LINCH0_RX_PIN_MUX
#define CY_LINCH_TX_PORT    CY_LINCH0_TX_PORT
#define CY_LINCH_TX_PIN     CY_LINCH0_TX_PIN
#define CY_LINCH_TX_PIN_MUX CY_LINCH0_TX_PIN_MUX
#define CY_LINCH_PCLK       CY_LINCH0_PCLK
#define CY_LINCH_IRQN       CY_LINCH0_IRQN

/* Port configuration */
typedef struct
{
    volatile stc_GPIO_PRT_t* portReg;
    uint8_t                  pinNum;
    cy_stc_gpio_pin_config_t cfg;
}stc_pin_config;

/* Master state type */
typedef enum
{
    LIN_STATE_IDLE,
    LIN_STATE_RX_HEADER,
    LIN_STATE_TX_RESPONSE,
    LIN_STATE_RX_RESPONSE
}en_lin_slave_state_t;

/* LIN message direction type */
typedef enum
{
    LIN_RX_RESPONSE,
    LIN_TX_RESPONSE,
}lin_response_direction;

/* LIN message type configuration */
typedef struct
{
    uint8_t                id;
    lin_response_direction responseDirection;
    en_lin_checksum_type_t checksumType;
    uint8_t                dataLength;
    uint8_t                dataBuffer[CY_LIN_DATA_LENGTH_MAX];
} lin_message_context;

/* LIN port configuration */
static const stc_pin_config lin_pin_cfg[] =
{
    /* LIN RX */
    {
        .portReg = CY_LINCH_RX_PORT,
        .pinNum  = CY_LINCH_RX_PIN,
        {
            .outVal    = 0u,
            .driveMode = CY_GPIO_DM_HIGHZ,
            .hsiom     = CY_LINCH_RX_PIN_MUX,
            .intEdge   = 0u,
            .intMask   = 0u,
            .vtrip     = 0u,
            .slewRate  = 0u,
            .driveSel  = 0u,
        }
    },
    /* LIN TX */
    {
        .portReg = CY_LINCH_TX_PORT,
        .pinNum  = CY_LINCH_TX_PIN,
        {
            .outVal    = 1u,
            .driveMode = CY_GPIO_DM_STRONG,
            .hsiom     = CY_LINCH_TX_PIN_MUX,
            .intEdge   = 0u,
            .intMask   = 0u,
            .vtrip     = 0u,
            .slewRate  = 0u,
            .driveSel  = 0u,
        }
    }
};

lin_message_context msgContext[] =
{
    {0x01, LIN_TX_RESPONSE, LinChecksumTypeExtended, 8u,},
    {0x02, LIN_RX_RESPONSE, LinChecksumTypeExtended, 8u,},
    {0x10, LIN_TX_RESPONSE, LinChecksumTypeExtended, 1u,},
    {0x11, LIN_RX_RESPONSE, LinChecksumTypeExtended, 1u,},
};
en_lin_slave_state_t lin_state;
uint8_t currentMsgIdx;

static void PortInit(void);
void LIN_IntHandler(void);

cy_stc_sysint_irq_t irq_cfg =
{
    .sysIntSrc  = CY_LINCH_IRQN,
    .intIdx     = CPUIntIdx3_IRQn,
    .isEnabled  = true,
};

stc_lin_config_t lin_config =
{
    .bMasterMode               = false,                        //If TRUE, corresponding channel = master mode, If FALSE, slave mode.
    .bLinTransceiverAutoEnable = true,                         //If TRUE, corresponding LIN channel transceiver is enabled automatically, If FALSE, firmware has to handle the transceiver enable signal manually
    .u8BreakFieldLength        = 11u,                          //Break field length
    .enBreakDelimiterLength    = LinBreakDelimiterLength1bits, //Break delimiter length {LinBreakDelimiterLength1bits = 1-bit length}
    .enStopBit                 = LinOneStopBit,                //Stop bit length { LinOneStopBit = 1, LinTwoStopBit = 3 }
    .bFilterEnable             = true                          //If TRUE, lin_rx_in filter operates. Median 3 operates on the last three "lin_rx_in" values
};

/*
 * This example is usage for slave node.
 * Use 20kbps as a baudrate.
 * This example does:
 *      - Master node sends 5 type header:
 *        0x02 and 0x11 are master tx response to this node
 *        0x01 and 0x10 are slave tx response to master node
 *        0x20 is master tx response to other node (this node ignore it)
 *      - Slave send tx response as received master node response.
 *        Slave node copies rx response to tx response (RX ID - 1 = TX ID)
 */
int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */
    
    /* LIN baudrate setting */
    /* LIN baudrate setting */
    /* Note: LIN IP does over sampling and over sampling count is fixed 16.
             Therefore LIN baudrate = LIN input clock / 16.                */
    uint32_t targetBaudrate = 20000ul; // 20Kbps
    uint32_t linFreq        = targetBaudrate * 16ul;
    uint32_t periFreq       = 80000000ul;
    uint32_t dividerNum     = periFreq / linFreq;
    Cy_SysClk_PeriphAssignDivider(CY_LINCH_PCLK, CY_SYSCLK_DIV_16_BIT, 0ul);
    Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(CY_LINCH_PCLK), CY_SYSCLK_DIV_16_BIT, 0ul, (dividerNum - 1ul));
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(CY_LINCH_PCLK), CY_SYSCLK_DIV_16_BIT, 0ul);

    /* Initialize port settings */
    PortInit();

    /* Initialize LIN */
    Cy_Lin_Init(CY_LINCH_TYPE, &lin_config);
    lin_state = LIN_STATE_IDLE;

    /* Register LIN interrupt handler and enable interrupt */
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, LIN_IntHandler);
    NVIC_SetPriority(irq_cfg.intIdx, 3ul);
    NVIC_EnableIRQ(irq_cfg.intIdx);

    /* LIN operation */
    lin_state = LIN_STATE_RX_HEADER;
    Cy_Lin_SetInterruptMask(CY_LINCH_TYPE, CY_LIN_INTR_RX_HEADER_DONE | CY_LIN_INTR_RX_RESPONSE_DONE | CY_LIN_INTR_ALL_ERROR_MASK_SLAVE);
    Cy_Lin_SetCmd(CY_LINCH_TYPE, CY_LIN_CMD_RX_HEADER_RX_RESPONSE);

    for(;;)
    {
        /* Do nothing, all operations are handled at IRQ handler */
    }
}

/* Initialize LIN regarding pins */
static void PortInit(void)
{
    for (uint8_t i = 0u; i < (sizeof(lin_pin_cfg) / sizeof(lin_pin_cfg[0])); i++)
    {
        Cy_GPIO_Pin_Init(lin_pin_cfg[i].portReg, lin_pin_cfg[i].pinNum, &lin_pin_cfg[i].cfg);
    }
}

/* LIN0 IRQ Handler */
void LIN_IntHandler(void)
{
    uint32_t maskStatus;
    Cy_Lin_GetInterruptMaskedStatus(CY_LINCH_TYPE, &maskStatus);
    Cy_Lin_ClearInterrupt(CY_LINCH_TYPE, maskStatus);    /* Clear all accepted interrupt. */
    cy_en_lin_status_t apiResponse;


    if ((maskStatus & CY_LIN_INTR_ALL_ERROR_MASK_SLAVE) != 0ul)
    {
        /* There are some error */
        /* Handle error if needed. */

        /* Disable the channel to reset LIN status */
        Cy_Lin_Disable(CY_LINCH_TYPE);
        /* Re-enable the channel */
        Cy_Lin_Enable(CY_LINCH_TYPE);
        /* Re enable header RX */
        lin_state = LIN_STATE_RX_HEADER;
        Cy_Lin_SetInterruptMask(CY_LINCH_TYPE, CY_LIN_INTR_RX_HEADER_DONE | CY_LIN_INTR_RX_RESPONSE_DONE | CY_LIN_INTR_ALL_ERROR_MASK_SLAVE);
        Cy_Lin_SetCmd(CY_LINCH_TYPE, CY_LIN_CMD_RX_HEADER_RX_RESPONSE);
    }
    else
    {
        bool acceptedId = false;
        uint8_t id, parity;
        switch(lin_state)
        {
        case LIN_STATE_RX_HEADER:
            /* Rx header complete with no error */
            Cy_Lin_GetHeader(CY_LINCH_TYPE, &id, &parity);
            /* Analyze ID */
            for (uint8_t i = 0u; i < (sizeof(msgContext) / sizeof(msgContext[0])); i++)
            {
                if (id == msgContext[i].id)
                {
                    currentMsgIdx = i;
                    acceptedId = true;
                    break;
                }
            }
            if (acceptedId)
            {
                /* Setup checksum type and data length */
                Cy_Lin_SetDataLength(CY_LINCH_TYPE, msgContext[currentMsgIdx].dataLength);
                Cy_Lin_SetChecksumType(CY_LINCH_TYPE, msgContext[currentMsgIdx].checksumType);
                if (msgContext[currentMsgIdx].responseDirection == LIN_TX_RESPONSE)
                {
                    Cy_Lin_SetInterruptMask(CY_LINCH_TYPE, CY_LIN_INTR_TX_RESPONSE_DONE | CY_LIN_INTR_RX_RESPONSE_DONE | CY_LIN_INTR_ALL_ERROR_MASK_SLAVE);
                    Cy_Lin_WriteData(CY_LINCH_TYPE, msgContext[currentMsgIdx].dataBuffer, msgContext[currentMsgIdx].dataLength);
                    lin_state = LIN_STATE_TX_RESPONSE;
                    Cy_Lin_SetCmd(CY_LINCH_TYPE, CY_LIN_CMD_TX_RESPONSE);
                }
                else
                {
                    lin_state = LIN_STATE_RX_RESPONSE;
                }
            }
            else
            {
                /* Message to be ignored */
                /* Disable the channel to reset LIN status */
                Cy_Lin_Disable(CY_LINCH_TYPE);
                /* Re-enable the channel */
                Cy_Lin_Enable(CY_LINCH_TYPE);
                /* Re enable header RX */
                lin_state = LIN_STATE_RX_HEADER;
                Cy_Lin_SetInterruptMask(CY_LINCH_TYPE, CY_LIN_INTR_RX_HEADER_DONE | CY_LIN_INTR_RX_RESPONSE_DONE | CY_LIN_INTR_ALL_ERROR_MASK_SLAVE);
                Cy_Lin_SetCmd(CY_LINCH_TYPE, CY_LIN_CMD_RX_HEADER_RX_RESPONSE);
            }
            break;
        case LIN_STATE_TX_RESPONSE:
            /* Tx response complete with no error */
            /* Check if RX_DONE interrupt occurs or not */
            /* If RX_DONE interrupt occurs, response collision occurs */
            if ((maskStatus & CY_LIN_INTR_RX_RESPONSE_DONE) != 0)
            {
                /* Data colision occurs */
                /* Do error handling if needed */
            }
            /* Re enable header RX */
            lin_state = LIN_STATE_RX_HEADER;
            Cy_Lin_SetInterruptMask(CY_LINCH_TYPE, CY_LIN_INTR_RX_HEADER_DONE | CY_LIN_INTR_RX_RESPONSE_DONE | CY_LIN_INTR_ALL_ERROR_MASK_SLAVE);
            Cy_Lin_SetCmd(CY_LINCH_TYPE, CY_LIN_CMD_RX_HEADER_RX_RESPONSE);
            break;
        case LIN_STATE_RX_RESPONSE:

            /* Rx response complete with no error */
            while(1)
            {
                apiResponse = Cy_Lin_ReadData(CY_LINCH_TYPE, msgContext[currentMsgIdx].dataBuffer);
                if(apiResponse == CY_LIN_SUCCESS)
                {
                    break;
                }
            }
            /* For testing
             * Set rx data to tx data. Rx ID - 1 => Tx ID
             */
            memcpy(msgContext[currentMsgIdx - 1].dataBuffer, msgContext[currentMsgIdx].dataBuffer, CY_LIN_DATA_LENGTH_MAX);
            /* Re enable header RX */
            lin_state = LIN_STATE_RX_HEADER;
            Cy_Lin_SetInterruptMask(CY_LINCH_TYPE, CY_LIN_INTR_RX_HEADER_DONE | CY_LIN_INTR_RX_RESPONSE_DONE | CY_LIN_INTR_ALL_ERROR_MASK_SLAVE);
            Cy_Lin_SetCmd(CY_LINCH_TYPE, CY_LIN_CMD_RX_HEADER_RX_RESPONSE);
            break;
        default:
            break;
        }
    }
}

/* [] END OF FILE */
