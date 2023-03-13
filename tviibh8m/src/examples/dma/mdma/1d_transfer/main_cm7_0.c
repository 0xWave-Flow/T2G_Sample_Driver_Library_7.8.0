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

/* Transmit data from memory array to memory array via MDMA. The DMA      */
/* copies every other one byte from source buffer into destination buffer */
/* The result will be like below                                          */
/* Src Buffer: |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |....... */
/* Dst Buffer: |  0 |  2 |  4 |  6 |  8 | 10 | 12 | 14 | 16 | 18 |....... */


/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "cy_project.h"
#include "cy_device_headers.h"

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/
#define DST_BUFFER_SIZE  (36ul)
#define SRC_BUFFER_SIZE  (DST_BUFFER_SIZE * 2ul)
#define DMAC_CHANNEL (0)
#define DMAC_CH_INTR (cpuss_interrupts_dmac_0_IRQn)

/*****************************************************************************/
/* Parameters depending on TVII MCU series                                   */
/*****************************************************************************/
#define DMAC_SW_TRIG (TRIG_OUT_MUX_3_MDMA_TR_IN0)

/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/
void DMA0_Ch_IntHandler(void);

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/
static  uint8_t                     au8DestBuffer[DST_BUFFER_SIZE] = {0u};
static  uint8_t                     au8SrcBuffer[SRC_BUFFER_SIZE];
static  cy_stc_mdma_descr_t         stcDescr;
static const   cy_stc_mdma_chnl_config_t   chnlConfig =
{
    .MDMA_Descriptor = &stcDescr,
    .preemptable     = 0ul,
    .priority        = 0ul,
    .enable          = 1ul,         /* enabled after initialization */
};
static const cy_stc_mdma_descr_config_t  stcDmaDescrConfig =
{
    .deact          = 0ul,
    .intrType       = CY_MDMA_INTR_DESCR_CMPLT,
    .trigoutType    = CY_MDMA_TRIGOUT_DESCR_CMPLT,
    .chStateAtCmplt = CY_MDMA_CH_DISABLED,
    .triginType     = CY_MDMA_TRIGIN_DESCR,
    .dataPrefetch   = 0ul,
    .dataSize       = CY_MDMA_BYTE,
    .srcTxfrSize    = 0ul,
    .destTxfrSize   = 0ul,
    .descrType      = CY_MDMA_1D_TRANSFER,
    .srcAddr        = au8SrcBuffer,
    .destAddr       = au8DestBuffer,
    .srcXincr       = 2ul,
    .destXincr      = 1ul,
    .xCount         = DST_BUFFER_SIZE,
    .descrNext      = NULL
};

static const cy_stc_sysint_irq_t stc_sysint_irq_cfg =
{
    .sysIntSrc = DMAC_CH_INTR,
    .intIdx    = CPUIntIdx2_IRQn,
    .isEnabled = true,
};

static bool isComplete = false;
/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/
int main(void)
{
    SystemInit();

    SCB_DisableDCache(); // Disables D cache because DMA also reads descriptor in the SRAM.
    __enable_irq(); /* Enable global interrupts. */

    /* Initialize & Enable DMA */
    Cy_MDMA_Disable(DMAC);
    Cy_MDMA_Chnl_DeInit(DMAC, DMAC_CHANNEL);
    Cy_MDMA_Descr_Init(&stcDescr,&stcDmaDescrConfig);
    Cy_MDMA_Chnl_Init(DMAC, DMAC_CHANNEL, &chnlConfig);
    Cy_MDMA_Chnl_Enable(DMAC, DMAC_CHANNEL);
    Cy_MDMA_Chnl_SetInterruptMask(DMAC, DMAC_CHANNEL, CY_MDMA_INTRCAUSE_COMPLETION);
    Cy_MDMA_Enable(DMAC);

    /* Prepare source buffer data */
    for(uint32_t i = 0ul; i < SRC_BUFFER_SIZE; i++)
    {
        au8SrcBuffer[i] = i;
    }

    /* Interrupt Initialization */
    Cy_SysInt_InitIRQ(&stc_sysint_irq_cfg);
    Cy_SysInt_SetSystemIrqVector(stc_sysint_irq_cfg.sysIntSrc, DMA0_Ch_IntHandler);
    NVIC_EnableIRQ(stc_sysint_irq_cfg.intIdx);

    /* SW Trigger */
    Cy_TrigMux_SwTrigger(DMAC_SW_TRIG, TRIGGER_TYPE_EDGE, 1ul);

    // wait for DMA completion
    while(isComplete == false);

    /* Compare source and destination buffers */
    for(uint32_t i = 0ul; i < DST_BUFFER_SIZE; i++)
    {
        CY_ASSERT(au8SrcBuffer[i*2ul] == au8DestBuffer[i]);
    }

    for(;;);
}

void DMA0_Ch_IntHandler(void)
{
    uint32_t masked;

    masked = Cy_MDMA_Chnl_GetInterruptStatusMasked(DMAC, DMAC_CHANNEL);
    if ((masked & CY_MDMA_INTRCAUSE_COMPLETION) != 0ul)
    {
        /* Clear Complete DMA interrupt flag */
        Cy_MDMA_Chnl_ClearInterrupt(DMAC, DMAC_CHANNEL);

        /* Mark the transmission as complete */
        isComplete = true;
    }
    else
    {
        CY_ASSERT(false);
    }
}

/* [] END OF FILE */
