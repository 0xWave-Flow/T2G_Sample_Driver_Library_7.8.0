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

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "cy_project.h"
#include "cy_device_headers.h"

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/
#define BUFFER_SIZE 100
#define DW_CHANNEL0 0
#define DW_CHANNEL1 1
#define DW_CH1_INTR (cpuss_interrupts_dw0_1_IRQn)

/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/

/*****************************************************************************/
/* Local type definitions ('typedef')                                        */
/*****************************************************************************/

/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/

/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/
static  uint8_t                     au8SrcBuffer  [BUFFER_SIZE];
static  uint8_t                     au8DestBuffer0[BUFFER_SIZE];
static  uint8_t                     au8DestBuffer1[BUFFER_SIZE];
static  cy_stc_pdma_descr_t         stcDescr0;
const   cy_stc_pdma_chnl_config_t   chnl0Config = 
{
    .PDMA_Descriptor = &stcDescr0,
    .preemptable     = 0ul,
    .priority        = 0ul,
    .enable          = 1ul,        /*enabled after initialization*/
};
static  cy_stc_pdma_descr_config_t  stcDmaDescr0Config= 
{
    .deact          = 0ul,                            /*Do not wait for trigger de-activation*/
    .intrType       = CY_PDMA_INTR_1ELEMENT_CMPLT,
    .trigoutType    = CY_PDMA_TRIGOUT_1ELEMENT_CMPLT,
    .chStateAtCmplt = CY_PDMA_CH_DISABLED,
    .triginType     = CY_PDMA_TRIGIN_DESCR,
    .dataSize       = CY_PDMA_BYTE,
    .srcTxfrSize    = 0ul,                            /*= dataSize*/
    .destTxfrSize   = 0ul,                            /*= dataSize*/
    .descrType      = CY_PDMA_1D_TRANSFER,
    .srcAddr        = au8SrcBuffer,
    .destAddr       = au8DestBuffer0,
    .srcXincr       = 1ul,
    .destXincr      = 1ul,
    .xCount         = BUFFER_SIZE,
    .descrNext      = NULL
};
static  cy_stc_pdma_descr_t         stcDescr1;
const   cy_stc_pdma_chnl_config_t   chnl1Config = 
{
    .PDMA_Descriptor = &stcDescr1,
    .preemptable     = 0ul,
    .priority        = 0ul,
    .enable          = 1ul,        /*enabled after initialization*/
};
static  cy_stc_pdma_descr_config_t  stcDmaDescr1Config= 
{
    .deact          = 0ul,                            /*Do not wait for trigger de-activation*/
    .intrType       = CY_PDMA_INTR_1ELEMENT_CMPLT,
    .trigoutType    = CY_PDMA_TRIGOUT_1ELEMENT_CMPLT,
    .chStateAtCmplt = CY_PDMA_CH_DISABLED,
    .triginType     = CY_PDMA_TRIGIN_DESCR,
    .dataSize       = CY_PDMA_BYTE,
    .srcTxfrSize    = 0ul,                            /*= dataSize*/
    .destTxfrSize   = 0ul,                            /*= dataSize*/
    .descrType      = CY_PDMA_1D_TRANSFER,
    .srcAddr        = au8DestBuffer0,
    .destAddr       = au8DestBuffer1,
    .srcXincr       = 1ul,
    .destXincr      = 1ul,
    .xCount         = BUFFER_SIZE,
    .descrNext      = NULL
};

cy_stc_gpio_pin_config_t user_led0_port_pin_cfg =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = CY_LED0_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

static const cy_stc_sysint_irq_t dw_irq_cfg =
{
    .sysIntSrc = DW_CH1_INTR,
    .intIdx    = CPUIntIdx2_IRQn,
    .isEnabled = true,
};

static bool isComplete = false;

void DW0_Ch_IntHandler(void);

/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/
int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /* In this example, CPU will check data modified by DMA therefore needs keeping coherency of data. */
    SCB_DisableDCache();

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(CY_LED0_PORT, CY_LED0_PIN, &user_led0_port_pin_cfg);

    for(uint32_t u32Idx = 0u; u32Idx < BUFFER_SIZE; u32Idx++)
    {
        au8DestBuffer1[u32Idx] = 0u;
        au8DestBuffer0[u32Idx] = 0u;
        au8SrcBuffer[u32Idx]   = u32Idx;
    }

    Cy_PDMA_Disable(DW0);

    //Channel 0
    Cy_PDMA_Chnl_DeInit(DW0, DW_CHANNEL0);
    Cy_PDMA_Descr_Init(&stcDescr0,&stcDmaDescr0Config);
    Cy_PDMA_Chnl_Init(DW0, DW_CHANNEL0, &chnl0Config);
    //Channel 1
    Cy_PDMA_Chnl_DeInit(DW0, DW_CHANNEL1);
    Cy_PDMA_Descr_Init(&stcDescr1,&stcDmaDescr1Config);
    Cy_PDMA_Chnl_Init(DW0, DW_CHANNEL1, &chnl1Config);
    Cy_PDMA_Chnl_SetInterruptMask(DW0, DW_CHANNEL1);

    Cy_PDMA_Enable(DW0);

    /* Interrupt Initialization */
    Cy_SysInt_InitIRQ(&dw_irq_cfg);
    Cy_SysInt_SetSystemIrqVector(dw_irq_cfg.sysIntSrc, DW0_Ch_IntHandler);
    NVIC_EnableIRQ(dw_irq_cfg.intIdx);

    /*Connect completion of Channel 0 with trigger of Channel 1*/
    cy_en_trigmux_status_t trigmux_status;
    trigmux_status = Cy_TrigMux_Connect(TRIG_IN_MUX_0_PDMA0_TR_OUT0,
                                        TRIG_OUT_MUX_0_PDMA0_TR_IN1,
                                        CY_TR_MUX_TR_INV_DISABLE,
                                        TRIGGER_TYPE_EDGE,
                                        0ul /* Output trigger is not blocked in DBG mode */);
    CY_ASSERT(trigmux_status == CY_TRIGMUX_SUCCESS);

    /*Trigger DMA by SW*/
    trigmux_status = Cy_TrigMux_SwTrigger(TRIG_OUT_MUX_0_PDMA0_TR_IN0,
                                          TRIGGER_TYPE_EDGE,
                                          1ul /*output*/);
    CY_ASSERT(trigmux_status == CY_TRIGMUX_SUCCESS);

    // wait for DW0 channel1 completion
    while(isComplete == false);

    CY_ASSERT(memcmp(au8SrcBuffer, au8DestBuffer0, BUFFER_SIZE) == 0);
    CY_ASSERT(memcmp(au8SrcBuffer, au8DestBuffer1, BUFFER_SIZE) == 0);

    for(;;)
    {
        /* wait 1.0 [s] */
        Cy_SysTick_DelayInUs(1000000);
        Cy_GPIO_Inv(CY_LED0_PORT, CY_LED0_PIN);
    }
}

void DW0_Ch_IntHandler(void)
{
    uint32_t masked;

    masked = Cy_PDMA_Chnl_GetInterruptStatusMasked(DW0, DW_CHANNEL1);
    if ((masked & CY_PDMA_INTRCAUSE_COMPLETION) != 0ul)
    {
        /* Clear Complete DMA interrupt flag */
        Cy_PDMA_Chnl_ClearInterrupt(DW0, DW_CHANNEL1);

        /* Mark the transmission as complete */
        isComplete = true;
    }
    else
    {
        CY_ASSERT(false);
    }
}


/* [] END OF FILE */
