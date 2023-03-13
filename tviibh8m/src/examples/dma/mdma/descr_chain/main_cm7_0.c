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

/* Transmit data with 4byte endian conversion using 2D transfer                       */
/* The buffer is consisted of lines. The line order also inverted during transmission */
/* The result will be like below when line number is 4.                               */
/* Src Buffer: |   0 |   1 |   2 |   3 |   4 |   5 |   6 |   7 |   8 |   9 |.......   */
/*             |  64 |  65 |  66 |  67 |  68 |  69 |  70 |  71 |  72 |  73 |.......   */
/*             | 128 | 129 | 130 | 131 | 132 | 133 | 134 | 135 | 136 | 137 |.......   */
/*             | 192 | 193 | 194 | 195 | 196 | 197 | 198 | 199 | 200 | 201 |.......   */

/* Dst Buffer: | 195 | 194 | 193 | 192 | 199 | 198 | 197 | 196 | 203 | 202 |.......   */
/*             | 131 | 130 | 129 | 128 | 135 | 134 | 133 | 132 | 139 | 138 |.......   */
/*             |  67 |  66 |  65 |  64 |  71 |  72 |  71 |  70 |  75 |  74 |.......   */
/*             |   3 |   2 |   1 |   0 |   7 |   6 |   5 |   4 |  11 |  10 |.......   */

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "cy_project.h"
#include "cy_device_headers.h"

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/
#define BUFFER_LINE_SIZE_IN_WORD (16ul)
#define BUFFER_LINE_SIZE_IN_BYTE (BUFFER_LINE_SIZE_IN_WORD * 4ul)
#define BUFFER_LINE_NUMBER       (4ul)
#define BUFFER_SIZE_IN_BYTE (BUFFER_LINE_SIZE_IN_BYTE * BUFFER_LINE_NUMBER)
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
uint32_t InvEndian4Byte(uint32_t in);

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/
static uint8_t au8DestBuffer[BUFFER_LINE_NUMBER][BUFFER_LINE_SIZE_IN_BYTE] = {0u};
static uint8_t au8SrcBuffer[BUFFER_LINE_NUMBER][BUFFER_LINE_SIZE_IN_BYTE];
static cy_stc_mdma_descr_t stcDescrForLine[BUFFER_LINE_NUMBER];
static const cy_stc_mdma_chnl_config_t   chnlConfig =
{
    .MDMA_Descriptor = stcDescrForLine,
    .preemptable     = 0ul,
    .priority        = 0ul,
    .enable          = 1ul,         /* enabled after initialization */
};
static cy_stc_mdma_descr_config_t  stcDmaDescrConfig =
{
    .deact          = 0ul,
    .intrType       = CY_MDMA_INTR_DESCRCHAIN_CMPLT,
    .trigoutType    = CY_MDMA_TRIGOUT_DESCR_CMPLT, // won't be used
    .chStateAtCmplt = CY_MDMA_CH_ENABLED,
    .triginType     = CY_MDMA_TRIGIN_DESCRCHAIN,
    .dataPrefetch   = 0ul,
    .dataSize       = CY_MDMA_BYTE,
    .srcTxfrSize    = 0ul,
    .destTxfrSize   = 0ul,
    .descrType      = CY_MDMA_2D_TRANSFER,
    .srcAddr        = NULL, // will be updated in run time
    .destAddr       = NULL, // will be updated in run time
    .srcXincr       = 4,
    .destXincr      = 4,
    .xCount         = BUFFER_LINE_SIZE_IN_WORD,
    .srcYincr       = -1,
    .destYincr      = 1,
    .yCount         = 4ul,
    .descrNext      = NULL // will be updated in run time
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
    for(uint32_t i = 0ul; i < BUFFER_LINE_NUMBER; i++)
    {
        stcDmaDescrConfig.srcAddr   = &au8SrcBuffer[i][3];
        stcDmaDescrConfig.destAddr  = &au8DestBuffer[BUFFER_LINE_NUMBER - (i + 1ul)][0];
        if((i + 1ul) == BUFFER_LINE_NUMBER) // Is last descriptor?
        {
            stcDmaDescrConfig.chStateAtCmplt = CY_MDMA_CH_DISABLED;
            stcDmaDescrConfig.descrNext      = NULL;
        }
        else
        {
            stcDmaDescrConfig.chStateAtCmplt = CY_MDMA_CH_ENABLED;
            stcDmaDescrConfig.descrNext      = &stcDescrForLine[i + 1ul];
        }
        Cy_MDMA_Descr_Init(&stcDescrForLine[i], &stcDmaDescrConfig);
    }
    Cy_MDMA_Chnl_Init(DMAC, DMAC_CHANNEL, &chnlConfig);
    Cy_MDMA_Chnl_Enable(DMAC, DMAC_CHANNEL);
    Cy_MDMA_Chnl_SetInterruptMask(DMAC, DMAC_CHANNEL, CY_MDMA_INTRCAUSE_COMPLETION);
    Cy_MDMA_Enable(DMAC);

    /* Prepare source buffer data */
    for(uint32_t i = 0ul; i < BUFFER_SIZE_IN_BYTE; i++)
    {
        au8SrcBuffer[0][i] = i;
    }

    /* Interrupt Initialization */
    Cy_SysInt_InitIRQ(&stc_sysint_irq_cfg);
    Cy_SysInt_SetSystemIrqVector(stc_sysint_irq_cfg.sysIntSrc, DMA0_Ch_IntHandler);
    NVIC_EnableIRQ(stc_sysint_irq_cfg.intIdx);

    /* SW Trigger */
    Cy_TrigMux_SwTrigger(DMAC_SW_TRIG, TRIGGER_TYPE_EDGE, 1ul);

    /* wait for DMA completion */
    while(isComplete == false);

    /* Check the transmission result */
    for(uint32_t i_line = 0; i_line < BUFFER_LINE_NUMBER; i_line++)
    {
        uint32_t* p32SrcBuf = (uint32_t*)&au8SrcBuffer[i_line][0];
        uint32_t* p32DestBuf = (uint32_t*)&au8DestBuffer[BUFFER_LINE_NUMBER - (i_line + 1ul)][0];
        for(uint32_t i_word = 0; i_word < BUFFER_LINE_SIZE_IN_WORD; i_word++)
        {
            CY_ASSERT(p32SrcBuf[i_word] == InvEndian4Byte(p32DestBuf[i_word]));
        }
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

uint32_t InvEndian4Byte(uint32_t in)
{
    return ((in & 0xFF000000ul) >> 24ul) |
           ((in & 0x00FF0000ul) >> 8ul)  |
           ((in & 0x0000FF00ul) << 8ul)  |
           ((in & 0x000000FFul) << 24ul);
}

/* [] END OF FILE */
