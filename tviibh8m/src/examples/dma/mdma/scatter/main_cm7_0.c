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

/* Scatter data from memory to memory via MDMA. The CPU shuffles source   */
/* buffer's destination address before DMA transmission.                  */
/* The result will be like below                                          */
/* Src Buffer: |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |....... */
/* Dst Buffer: | 30 | 46 | 62 | 42 | 52 | 81 | 60 | 86 | 16 | 75 |....... */


/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "cy_project.h"
#include "cy_device_headers.h"
#include <stdlib.h>

/*****************************************************************************/
/* Local Structure ('typedef struct')                                        */
/*****************************************************************************/
#pragma pack(1)
typedef struct
{
    uint32_t address;
    uint32_t data;
} stc_dma_scatter_t;
#pragma pack()

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/
#define BUFFER_SIZE  (100ul)
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
void ShuffleScatterAddress(stc_dma_scatter_t array[], uint32_t size);

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/
static  uint32_t          au32DestBuffer[BUFFER_SIZE] = {0u};
static  stc_dma_scatter_t srcScatterBuffer[BUFFER_SIZE];
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
    .dataPrefetch   = 0ul,
    .chStateAtCmplt = CY_MDMA_CH_DISABLED,
    .triginType     = CY_MDMA_TRIGIN_DESCR,
    .dataSize       = CY_MDMA_WORD,
    .srcTxfrSize    = 0ul,
    .destTxfrSize   = 0ul,
    .descrType      = CY_MDMA_SCATTER_TRANSFER,
    .srcAddr        = srcScatterBuffer,
    .xCount         = (BUFFER_SIZE * 2ul), // xCount contains data and also address.
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
    for(uint32_t i = 0ul; i < BUFFER_SIZE; i++)
    {
        srcScatterBuffer[i].data = i;
        srcScatterBuffer[i].address = (uint32_t)&au32DestBuffer[i];
    }

    ShuffleScatterAddress(srcScatterBuffer, BUFFER_SIZE);

    /* Interrupt Initialization */
    Cy_SysInt_InitIRQ(&stc_sysint_irq_cfg);
    Cy_SysInt_SetSystemIrqVector(stc_sysint_irq_cfg.sysIntSrc, DMA0_Ch_IntHandler);
    NVIC_EnableIRQ(stc_sysint_irq_cfg.intIdx);

    /* SW Trigger */
    Cy_TrigMux_SwTrigger(DMAC_SW_TRIG, TRIGGER_TYPE_EDGE, 1ul);

    // wait for DMA completion
    while(isComplete == false);

    /* Compare source and destination buffers */
    for(uint32_t i = 0ul; i < BUFFER_SIZE; i++)
    {
        CY_ASSERT(srcScatterBuffer[i].data == *(uint32_t*)srcScatterBuffer[i].address);
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

void SwapScatterAddress(stc_dma_scatter_t array[], uint32_t pos1, uint32_t pos2)
{
    uint32_t tmp = array[pos1].address;
    array[pos1].address = array[pos2].address;
    array[pos2].address = tmp;
}

void ShuffleScatterAddress(stc_dma_scatter_t array[], uint32_t size)
{
    for(uint32_t i = (size - 1ul); i > 0ul; i--)
    {
        uint32_t pos = rand() % i;
        SwapScatterAddress(array, pos, i);
    }
}

/* [] END OF FILE */
