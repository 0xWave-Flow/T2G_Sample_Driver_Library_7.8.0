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

/* CAN in Use */
/* !!!! Please modify according to your HW !!! */
#define CY_CANFD_TYPE     CY_CANFD1_TYPE
#define CY_CANFD_RX_PORT  CY_CANFD1_RX_PORT
#define CY_CANFD_RX_PIN   CY_CANFD1_RX_PIN
#define CY_CANFD_TX_PORT  CY_CANFD1_TX_PORT
#define CY_CANFD_TX_PIN   CY_CANFD1_TX_PIN
#define CY_CANFD_RX_MUX   CY_CANFD1_RX_MUX
#define CY_CANFD_TX_MUX   CY_CANFD1_TX_MUX
#define CY_CANFD_PCLK     CY_CANFD1_PCLK
#define CY_CANFD_IRQN     CY_CANFD1_IRQN
#define NON_ISO_OPERATION (0)

static void PortInit(void);
void CanfdInterruptHandler(void);

void CAN_RxMsgCallback(bool bRxFifoMsg, uint8_t u8MsgBufOrRxFifoNum, cy_stc_canfd_msg_t* pstcCanFDmsg);
void CAN_RxFifoWithTopCallback(uint8_t u8FifoNum, uint8_t u8BufferSizeInWord, uint32_t* pu32RxBuf);

#if NON_ISO_OPERATION == 1
static void SetISOFormat(cy_pstc_canfd_type_t canfd);
#endif

/* Port configuration */
typedef struct
{
    volatile stc_GPIO_PRT_t* portReg;
    uint8_t                  pinNum;
    cy_stc_gpio_pin_config_t cfg;
} stc_pin_config;

/* Standard ID Filter configuration */
static const cy_stc_id_filter_t stdIdFilter[] = 
{
    CANFD_CONFIG_STD_ID_FILTER_CLASSIC_RXBUFF(0x010ul, 0ul),      /* ID=0x010, store into RX buffer Idx0 */
    CANFD_CONFIG_STD_ID_FILTER_CLASSIC_RXBUFF(0x020ul, 1ul),      /* ID=0x020, store into RX buffer Idx1 */
};

/* Extended ID Filter configuration */
static const cy_stc_extid_filter_t extIdFilter[] = 
{
    CANFD_CONFIG_EXT_ID_FILTER_CLASSIC_RXBUFF(0x10010ul, 2ul),    /* ID=0x10010, store into RX buffer Idx2 */
    CANFD_CONFIG_EXT_ID_FILTER_CLASSIC_RXBUFF(0x10020ul, 3ul),    /* ID=0x10020, store into RX buffer Idx3 */
};

/* CAN configuration */
cy_stc_canfd_config_t canCfg = 
{
    .txCallback            = NULL,              // Unused.
    .rxCallback            = CAN_RxMsgCallback,
    .rxFifoWithTopCallback = NULL,              //CAN_RxFifoWithTopCallback,
    .statusCallback        = NULL,              // Un-supported now
    .errorCallback         = NULL,              // Un-supported now
    .canFDMode             = true,              // Use CANFD mode
    // 40 MHz
    .bitrate =       // Nominal bit rate settings (sampling point = 75%)
    {
        .prescaler     = 10u - 1u, // cclk/10, When using 500kbps, 1bit = 8tq
        .timeSegment1  = 5u  - 1u, // tseg1 = 5tq
        .timeSegment2  = 2u  - 1u, // tseg2 = 2tq
        .syncJumpWidth = 2u  - 1u, // sjw   = 2tq
    },
    
    .fastBitrate =       // Fast bit rate settings (sampling point = 75%)
    {
        .prescaler     = 5u - 1u, // cclk/5, When using 1Mbps, 1bit = 8tq
        .timeSegment1  = 5u - 1u, // tseg1 = 5tq,
        .timeSegment2  = 2u - 1u, // tseg2 = 2tq
        .syncJumpWidth = 2u - 1u, // sjw   = 2tq
    },
    .tdcConfig =       // Transceiver delay compensation, unused.
    {
        .tdcEnabled      = false,
        .tdcOffset       = 0u,
        .tdcFilterWindow = 0u,
    },
    .sidFilterConfig =   // Standard ID filter
    {
        .numberOfSIDFilters = sizeof(stdIdFilter) / sizeof(stdIdFilter[0]),
        .sidFilter          = stdIdFilter,
    },
    .extidFilterConfig  =   // Extended ID filter
    {
        .numberOfEXTIDFilters   = sizeof(extIdFilter) / sizeof(extIdFilter[0]),
        .extidFilter            = extIdFilter,
        .extIDANDMask           = 0x1FFFFFFFul,   // No pre filtering.
    },
    .globalFilterConfig =   // Global filter
    {
        .nonMatchingFramesStandard  = CY_CANFD_ACCEPT_IN_RXFIFO_0, // Reject none match IDs
        .nonMatchingFramesExtended  = CY_CANFD_ACCEPT_IN_RXFIFO_1, // Reject none match IDs
        .rejectRemoteFramesStandard = true,                        // No remote frame
        .rejectRemoteFramesExtended = true,                        // No remote frame
    },
    .rxBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_64,
    .rxFifo1DataSize  = CY_CANFD_BUFFER_DATA_SIZE_64,
    .rxFifo0DataSize  = CY_CANFD_BUFFER_DATA_SIZE_64,
    .txBufferDataSize = CY_CANFD_BUFFER_DATA_SIZE_64,
    .rxFifo0Config    = // RX FIFO0, unused.
    {
        .mode                   = CY_CANFD_FIFO_MODE_BLOCKING,
        .watermark              = 10u,
        .numberOfFifoElements   = 8u,
        .topPointerLogicEnabled = false,
    },
    .rxFifo1Config    = // RX FIFO1, unused.
    {
        .mode                   = CY_CANFD_FIFO_MODE_BLOCKING,
        .watermark              = 10u,
        .numberOfFifoElements   = 8u,
        .topPointerLogicEnabled = false,                       // true,
    },
    .noOfRxBuffers  = 4u,
    .noOfTxBuffers  = 4u,
};

/* CAN port configuration */
static const stc_pin_config can_pin_cfg[] =
{
    /* CAN0_2 RX */
    {
        .portReg = CY_CANFD_RX_PORT,
        .pinNum  = CY_CANFD_RX_PIN,
        .cfg     =
        {
            .outVal    = 0ul,
            .driveMode = CY_GPIO_DM_HIGHZ,
            .hsiom     = CY_CANFD_RX_MUX,
            .intEdge   = 0ul,
            .intMask   = 0ul,
            .vtrip     = 0ul,
            .slewRate  = 0ul,
            .driveSel  = 0ul,
        }
    },
    /* CAN0_2 TX */
    {
        .portReg = CY_CANFD_TX_PORT,
        .pinNum  = CY_CANFD_TX_PIN,
        .cfg     =
        {
            .outVal    = 1ul,
            .driveMode = CY_GPIO_DM_STRONG,
            .hsiom     = CY_CANFD_TX_MUX,
            .intEdge   = 0ul,
            .intMask   = 0ul,
            .vtrip     = 0ul,
            .slewRate  = 0ul,
            .driveSel  = 0ul,
        }
    }
};

cy_stc_sysint_irq_t irq_cfg =
{
    .sysIntSrc = CY_CANFD_IRQN,   /* Use interrupt LINE0 */
    .intIdx    = CPUIntIdx2_IRQn,
    .isEnabled = true,
};

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /* Setup CAN clock (cclk).
     * This clock is used as base clock of the CAN communication.
     */
    uint32_t targetFreq = 40000000ul; // 40MHz
    uint32_t periFreq   = 80000000ul; // 80MHz
    uint32_t dividerNum = periFreq / targetFreq;
    Cy_SysClk_PeriphAssignDivider(CY_CANFD_PCLK, CY_SYSCLK_DIV_8_BIT, 0ul);
    Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(CY_CANFD_PCLK), CY_SYSCLK_DIV_8_BIT, 0ul, (dividerNum - 1ul));
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(CY_CANFD_PCLK), CY_SYSCLK_DIV_8_BIT, 0ul);

    /* DeInit to initialize CANFD IP */
    Cy_CANFD_DeInit(CY_CANFD_TYPE);

    /* Initialize CAN ports and the CAN transceiver. */
    PortInit();

    /* Setup CANFD interrupt */
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, CanfdInterruptHandler);
    NVIC_SetPriority(irq_cfg.intIdx, 0ul);
    NVIC_EnableIRQ(irq_cfg.intIdx);

    /* Initialize CANFD */
    Cy_CANFD_Init(CY_CANFD_TYPE, &canCfg);
#if NON_ISO_OPERATION == 1
    SetISOFormat(CY_CANFD_TYPE);
#endif

     /* Now a ch configured as CANFD is working. */

    /* Prepare CANFD message to transmit*/
    cy_stc_canfd_msg_t stcMsg;
    stcMsg.canFDFormat               = true;
    stcMsg.idConfig.extended         = false;
    stcMsg.idConfig.identifier       = 0x525ul;
    stcMsg.dataConfig.dataLengthCode = 15u;
    stcMsg.dataConfig.data[0]        = 0x70190523ul;
    stcMsg.dataConfig.data[1]        = 0x70190819ul;
    stcMsg.dataConfig.data[2]        = 0x33332222ul;
    stcMsg.dataConfig.data[3]        = 0x33332222ul;
    stcMsg.dataConfig.data[4]        = 0x55554444ul;
    stcMsg.dataConfig.data[5]        = 0x77776666ul;
    stcMsg.dataConfig.data[6]        = 0x99998888ul;
    stcMsg.dataConfig.data[7]        = 0xBBBBAAAAul;
    stcMsg.dataConfig.data[8]        = 0xDDDDCCCCul;
    stcMsg.dataConfig.data[9]        = 0xFFFFEEEEul;
    stcMsg.dataConfig.data[10]       = 0x78563412ul;
    stcMsg.dataConfig.data[11]       = 0x00000000ul;
    stcMsg.dataConfig.data[12]       = 0x11111111ul;
    stcMsg.dataConfig.data[13]       = 0x22222222ul;
    stcMsg.dataConfig.data[14]       = 0x33333333ul;
    stcMsg.dataConfig.data[15]       = 0x44444444ul;

    Cy_CANFD_UpdateAndTransmitMsgBuffer(CY_CANFD_TYPE, 0u, &stcMsg);

    for(;;)
    {
    }
}

#if NON_ISO_OPERATION == 1
static void SetISOFormat(cy_pstc_canfd_type_t canfd)
{
    canfd->M_TTCAN.unCCCR.stcField.u1INIT = 1ul;
    while(canfd->M_TTCAN.unCCCR.stcField.u1INIT != 1ul);
    /* Cancel protection by setting CCE */
    canfd->M_TTCAN.unCCCR.stcField.u1CCE = 1ul;
    canfd->M_TTCAN.unCCCR.stcField.u1NISO = 1ul;

    canfd->M_TTCAN.unCCCR.stcField.u1INIT = 0ul;
    while(canfd->M_TTCAN.unCCCR.stcField.u1INIT != 0ul);
}
#endif

/* Initialize CAN regarding pins */
static void PortInit(void)
{
    for (uint8_t i = 0u; i < (sizeof(can_pin_cfg) / sizeof(can_pin_cfg[0])); i++)
    {
        Cy_GPIO_Pin_Init(can_pin_cfg[i].portReg, can_pin_cfg[i].pinNum, &can_pin_cfg[i].cfg);
    }
}

/* CANFD reception callback */
void CAN_RxMsgCallback(bool bRxFifoMsg, uint8_t u8MsgBufOrRxFifoNum, cy_stc_canfd_msg_t* pstcCanFDmsg)
{
    /* Just loop back to the sender with +1 ID */
    pstcCanFDmsg->idConfig.identifier += 1u;
    Cy_CANFD_UpdateAndTransmitMsgBuffer
    (
        CY_CANFD_TYPE,
        0u,
        pstcCanFDmsg
    );
}

void CAN_RxFifoWithTopCallback(uint8_t u8FifoNum, uint8_t   u8BufferSizeInWord, uint32_t* pu32RxBuf)
{
    /*TODO*/
}

/* CANFD interrupt handler */
void CanfdInterruptHandler(void)
{
    /* Just invoking */
    Cy_CANFD_IrqHandler(CY_CANFD_TYPE);
}

/* [] END OF FILE */
