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

// EMAC
#include "drivers\ethernet\cy_ethif.h"

#define USER_LED_PORT           CY_LED8_PORT
#define USER_LED_PIN            CY_LED8_PIN
#define USER_LED_PIN_MUX        CY_LED8_PIN_MUX

#define ETH_REG_BASE         CY_AUTO_ETH_TYPE

#define ETH_INTR_SRC         (CY_AUTO_ETH_IRQN0)
#define ETH_INTR_SRC_Q1      (CY_AUTO_ETH_IRQN1)
#define ETH_INTR_SRC_Q2      (CY_AUTO_ETH_IRQN2)

#define ETH0_TD0_PORT        CY_AUTO_ETH_TD0_PORT
#define ETH0_TD0_PIN         CY_AUTO_ETH_TD0_PIN
#define ETH0_TD0_PIN_MUX     CY_AUTO_ETH_TD0_PIN_MUX

#define ETH0_TD1_PORT        CY_AUTO_ETH_TD1_PORT
#define ETH0_TD1_PIN         CY_AUTO_ETH_TD1_PIN
#define ETH0_TD1_PIN_MUX     CY_AUTO_ETH_TD1_PIN_MUX

#define ETH0_TD2_PORT        CY_AUTO_ETH_TD2_PORT
#define ETH0_TD2_PIN         CY_AUTO_ETH_TD2_PIN
#define ETH0_TD2_PIN_MUX     CY_AUTO_ETH_TD2_PIN_MUX

#define ETH0_TD3_PORT        CY_AUTO_ETH_TD3_PORT
#define ETH0_TD3_PIN         CY_AUTO_ETH_TD3_PIN
#define ETH0_TD3_PIN_MUX     CY_AUTO_ETH_TD3_PIN_MUX

#define ETH0_TXER_PORT       CY_AUTO_ETH_TXER_PORT
#define ETH0_TXER_PIN        CY_AUTO_ETH_TXER_PIN
#define ETH0_TXER_PIN_MUX    CY_AUTO_ETH_TXER_PIN_MUX

#define ETH0_TX_CTL_PORT     CY_AUTO_ETH_TX_CTL_PORT
#define ETH0_TX_CTL_PIN      CY_AUTO_ETH_TX_CTL_PIN
#define ETH0_TX_CTL_PIN_MUX  CY_AUTO_ETH_TX_CTL_PIN_MUX

#define ETH0_RD0_PORT        CY_AUTO_ETH_RD0_PORT
#define ETH0_RD0_PIN         CY_AUTO_ETH_RD0_PIN
#define ETH0_RD0_PIN_MUX     CY_AUTO_ETH_RD0_PIN_MUX

#define ETH0_RD1_PORT        CY_AUTO_ETH_RD1_PORT
#define ETH0_RD1_PIN         CY_AUTO_ETH_RD1_PIN
#define ETH0_RD1_PIN_MUX     CY_AUTO_ETH_RD1_PIN_MUX

#define ETH0_RD2_PORT        CY_AUTO_ETH_RD2_PORT
#define ETH0_RD2_PIN         CY_AUTO_ETH_RD2_PIN
#define ETH0_RD2_PIN_MUX     CY_AUTO_ETH_RD2_PIN_MUX

#define ETH0_RD3_PORT        CY_AUTO_ETH_RD3_PORT
#define ETH0_RD3_PIN         CY_AUTO_ETH_RD3_PIN
#define ETH0_RD3_PIN_MUX     CY_AUTO_ETH_RD3_PIN_MUX

#define ETH0_RX_CTL_PORT     CY_AUTO_ETH_RX_CTL_PORT
#define ETH0_RX_CTL_PIN      CY_AUTO_ETH_RX_CTL_PIN
#define ETH0_RX_CTL_PIN_MUX  CY_AUTO_ETH_RX_CTL_PIN_MUX

#define ETH0_RX_ER_PORT      CY_AUTO_ETH_RX_ER_PORT
#define ETH0_RX_ER_PIN       CY_AUTO_ETH_RX_ER_PIN
#define ETH0_RX_ER_PIN_MUX   CY_AUTO_ETH_RX_ER_PIN_MUX

#define ETH0_TX_CLK_PORT     CY_AUTO_ETH_TX_CLK_PORT
#define ETH0_TX_CLK_PIN      CY_AUTO_ETH_TX_CLK_PIN
#define ETH0_TX_CLK_PIN_MUX  CY_AUTO_ETH_TX_CLK_PIN_MUX

#define ETH0_RX_CLK_PORT     CY_AUTO_ETH_RX_CLK_PORT
#define ETH0_RX_CLK_PIN      CY_AUTO_ETH_RX_CLK_PIN
#define ETH0_RX_CLK_PIN_MUX  CY_AUTO_ETH_RX_CLK_PIN_MUX

#define ETH0_REF_CLK_PORT    CY_AUTO_ETH_REF_CLK_PORT
#define ETH0_REF_CLK_PIN     CY_AUTO_ETH_REF_CLK_PIN
#define ETH0_REF_CLK_PIN_MUX CY_AUTO_ETH_REF_CLK_PIN_MUX

#define ETH0_MDC_PORT        CY_AUTO_ETH_MDC_PORT
#define ETH0_MDC_PIN         CY_AUTO_ETH_MDC_PIN
#define ETH0_MDC_PIN_MUX     CY_AUTO_ETH_MDC_PIN_MUX

#define ETH0_MDIO_PORT       CY_AUTO_ETH_MDIO_PORT
#define ETH0_MDIO_PIN        CY_AUTO_ETH_MDIO_PIN
#define ETH0_MDIO_PIN_MUX    CY_AUTO_ETH_MDIO_PIN_MUX

/** Interrupt Handlers for Ethernet 0     */
void Cy_Eth0_InterruptHandler (void)
{
    Cy_EthIf00_DecodeEvent();       
}

cy_stc_gpio_pin_config_t user_led_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = USER_LED_PIN_MUX,                           
    .intEdge = 0,                                  
    .intMask = 0,                                  
    .vtrip = 0,                                    
    .slewRate = 0,                                 
    .driveSel = 0,                                 
    .vregEn = 0,                                   
    .ibufMode = 0,                                 
    .vtripSel = 0,                                 
    .vrefSel = 0,                                  
    .vohSel = 0,                                   
};

// EMAC *********
/**                      PortPinName.outVal||  driveMode               hsiom             ||intEdge||intMask||vtrip||slewRate||driveSel||vregEn||ibufMode||vtripSel||vrefSel||vohSel*/
cy_stc_gpio_pin_config_t eth0_tx0   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETH0_TD0_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_tx1   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETH0_TD1_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_tx2   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETH0_TD2_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_tx3   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETH0_TD3_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_txer  = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETH0_TXER_PIN_MUX,    0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_txctl = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETH0_TX_CTL_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
                                                                                           
cy_stc_gpio_pin_config_t eth0_rx0   = {0x00, CY_GPIO_DM_HIGHZ,         ETH0_RD0_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_rx1   = {0x00, CY_GPIO_DM_HIGHZ,         ETH0_RD1_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_rx2   = {0x00, CY_GPIO_DM_HIGHZ,         ETH0_RD2_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_rx3   = {0x00, CY_GPIO_DM_HIGHZ,         ETH0_RD3_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_rxctl = {0x00, CY_GPIO_DM_HIGHZ,         ETH0_RX_CTL_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_rxer  = {0x00, CY_GPIO_DM_HIGHZ,         ETH0_RX_ER_PIN_MUX,   0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
                                                                       
cy_stc_gpio_pin_config_t eth0_txclk = {0x00, CY_GPIO_DM_HIGHZ,         ETH0_TX_CLK_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_rxclk = {0x00, CY_GPIO_DM_HIGHZ,         ETH0_RX_CLK_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_refclk = {0x00, CY_GPIO_DM_HIGHZ,        ETH0_REF_CLK_PIN_MUX, 0,       0,       0,     0,        0,        0,      0,        1,        0,       0};

cy_stc_gpio_pin_config_t eth0_mdc   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETH0_MDC_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        1,        0,       0};
cy_stc_gpio_pin_config_t eth0_mdio  = {0x00, CY_GPIO_DM_STRONG,        ETH0_MDIO_PIN_MUX,    0,       0,       0,     0,        0,        0,      0,        1,        0,       0};

/** Transmit source buffer  */
#pragma alignvar (8) 
//__attribute((aligned(8))) 
uint8_t u8DummyTxBuf[1536] = {0};
bool bFrameReceived = false;

#define ETH_FRAME_SIZE  150

static void Cy_App_Init_EthernetPortpins(void);
static void Eth0_RxFrameCB ( cy_pstc_eth_type_t pstcEth, uint8_t * u8RxBuffer, uint32_t u32Length );
static void Eth0_TSUIncrement ( cy_pstc_eth_type_t pstcEth );
//EMAC End ***********


int main(void)
{
    uint32_t cy_Hfclk4FreqHz  = 0UL;
    uint32_t cy_Hfclk0FreqHz  = 0UL;
    uint32_t cy_ClkPeriFreqHz  = 0UL;
    uint32_t cy_ClkSlowFreqHz  = 0UL;
    uint8_t u8CLK_GR4Divider = 0;
    
    SystemInit();

    // Example had been originally tested with "cache off", so ensure that caches are turned off (may have been enabled by new startup.c module)
    SCB_DisableICache(); // Disables and invalidates instruction cache
    SCB_DisableDCache(); // Disables, cleans and invalidates data cache
    
    __enable_irq();

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);    
    
    Cy_SysClk_GetHfClkFrequency(CY_SYSCLK_HFCLK_0, &cy_Hfclk0FreqHz);       /** Clock routed for MDC clock    */    
    Cy_SysClk_GetHfClkFrequency(CY_SYSCLK_HFCLK_4, &cy_Hfclk4FreqHz);       /** Internal reference Clock for RGMII mode    */  
    Cy_SysClk_GetClkSlowFrequency (&cy_ClkSlowFreqHz);                      /** Slow clock frequency    */
    // Cy_SysClk_SetClkGrDiv(4, 1);
    u8CLK_GR4Divider = Cy_SysClk_GetClkGrDiv(4);
    Cy_SysClk_GetClkPeriFrequency (&cy_ClkPeriFreqHz);
    
    
    // EMAC*********
    bool bClearAll = true;
    bool bTransmitBuf = true;
    bool bLastBuffer = true;
    
    /** Configure Ethernet Port pins    */
    Cy_App_Init_EthernetPortpins();      
    
    /** Wrapper configuration   */
    cy_str_ethif_wrapper_config stcWrapperConfig = {
        .stcInterfaceSel = CY_ETHIF_CTL_MII_100,   /** MII 100M */ 
        .bRefClockSource = CY_ETHIF_INTERNAL_PLL,  /** Although MII interface does not use this clock, assigning Ref_Clk to internal PLL clock  */
        .u8RefClkDiv = 1,                          /** RefClk is not going to be used in MII mode  */
    };
    
    /** Default Timer register values   */
    CEDI_1588TimerVal stc1588TimerValue = {
        .secsUpper = 0,
        .secsLower = 0,
        .nanosecs = 0,
    };   
    
    /** Increment values for each clock cycles      */
    CEDI_TimerIncrement stc1588TimerIncValue = { /** This Increment values are calculated for source clock of 24 MHz */
        .nanoSecsInc = 41,                       /** Having source clock of 24 MHz, with each clock cycle Nanosecond counter shall be incremented by 41  */
        .subNsInc = 43690,                       /** incrementing just Nanosecond counter will not give accuracy, so sub-nanosecond counter also must be incremented  */  
        .lsbSubNsInc = 170,                      /** 170 (0xAA) is the lower 8 bits increment value for subNanosecond and 43690 (0xAAAA) is the higher 16 bits of the counter */
        .altIncCount = 0,                        /** This example is not using alternate increment mode, also it is not recommended by IP provider       */
        .altNanoSInc = 0,                        /** When Alt increment mode is disabled, then this counter does not play any role          */
    };
    
    /** To calculate the value to write to the sub-ns register you take the decimal value of 
        the sub-nanosecond value, then multiply it by 2 to the power of 24 (16777216) and 
        convert the result to hexadecimal. For example for a sub-nanosecond value of 0.33333333 
        you would write 0x55005555.  */ 
    
    /** TSU configuration   */
    cy_str_ethif_TSU_Config stcTSUConfig = {
        .pstcTimerValue    = &stc1588TimerValue,            /** start value for the counter     */
        .pstcTimerIncValue = &stc1588TimerIncValue,         /** Increment value for each clock cycle    */
        .bOneStepTxSyncEnable = false,                      /** useful when PTP protocol is in place    */
        .enTxDescStoreTimeStamp = CEDI_TX_TS_DISABLED,      
        .enRxDescStoreTimeStamp = CEDI_RX_TS_DISABLED,
        .bStoreNSinRxDesc = false,
    };
    
    /** General Ethernet configuration  */
    cy_stc_ethif_configuration_t stcENETConfig = {                    
                    .bintrEnable         = 1,                           /** Interrupt enable  */
                    .dmaDataBurstLen     = CEDI_DMA_DBUR_LEN_4, 
                    .u8dmaCfgFlags       = CEDI_CFG_DMA_FRCE_TX_BRST,
                    .mdcPclkDiv          = CEDI_MDC_DIV_BY_16,          /** For PSVP, source clock is 24 MHz and MDC must be less than 2.5MHz   */
                    .u8rxLenErrDisc      = 0,                           /** Length error frame not discarded  */
                    .u8disCopyPause      = 0,                           
                    .u8chkSumOffEn       = 0,                           /** Checksum for both Tx and Rx disabled    */
                    .u8rx1536ByteEn      = 1,                           /** Enable receive frame up to 1536    */
                    .u8rxJumboFrEn       = 0,
                    .u8enRxBadPreamble   = 1,
                    .u8ignoreIpgRxEr     = 0,
                    .u8storeUdpTcpOffset = 0,
                    .u8aw2wMaxPipeline   = 2,                           /** Value must be > 0   */
                    .u8ar2rMaxPipeline   = 2,                           /** Value must be > 0   */
                    .u8pfcMultiQuantum   = 0,
                    .pstcWrapperConfig   = &stcWrapperConfig,
                    .pstcTSUConfig       = &stcTSUConfig,               /** No TSU settings */
                    .btxq0enable         = 1,                           /** Tx Q0 Enabled   */
                    .btxq1enable         = 0,                           /** Tx Q1 Disabled  */            
                    .btxq2enable         = 0,                           /** Tx Q2 Disabled  */
                    .brxq0enable         = 1,                           /** Rx Q0 Enabled   */
                    .brxq1enable         = 0,                           /** Rx Q1 Disabled  */
                    .brxq2enable         = 0,                           /** Rx Q2 Disabled  */
    };
    
    /** Interrupt configurations    */
    cy_stc_ethif_interruptconfig_t stcInterruptConfig = {
                    .btsu_time_match        = 0,          /** Time stamp unit time match event */
                    .bwol_rx                = 0,          /** Wake on LAN event received */
                    .blpi_ch_rx             = 0,          /** LPI indication status bit change received */
                    .btsu_sec_inc           = 1,          /** TSU seconds register increment */
                    .bptp_tx_pdly_rsp       = 0,          /** PTP pdelay_resp frame transmitted */
                    .bptp_tx_pdly_req       = 0,          /** PTP pdelay_req frame transmitted */
                    .bptp_rx_pdly_rsp       = 0,          /** PTP pdelay_resp frame received */
                    .bptp_rx_pdly_req       = 0,          /** PTP pdelay_req frame received */
                    .bptp_tx_sync           = 0,          /** PTP sync frame transmitted */
                    .bptp_tx_dly_req        = 0,          /** PTP delay_req frame transmitted */
                    .bptp_rx_sync           = 0,          /** PTP sync frame received */
                    .bptp_rx_dly_req        = 0,          /** PTP delay_req frame received */
                    .bext_intr              = 0,          /** External input interrupt detected */
                    .bpause_frame_tx        = 0,          /** Pause frame transmitted */
                    .bpause_time_zero       = 0,          /** Pause time reaches zero or zeroq pause frame received */
                    .bpause_nz_qu_rx        = 0,          /** Pause frame with non-zero quantum received */
                    .bhresp_not_ok          = 0,          /** DMA hresp not OK */
                    .brx_overrun            = 1,          /** Rx overrun error */
                    .bpcs_link_change_det   = 0,          /** Link status change detected by PCS */
                    .btx_complete           = 1,          /** Frame has been transmitted successfully */
                    .btx_fr_corrupt         = 1,          /** Tx frame corruption */
                    .btx_retry_ex_late_coll = 1,          /** Retry limit exceeded or late collision */
                    .btx_underrun           = 1,          /** Tx underrun */
                    .btx_used_read          = 1,          /** Used bit set has been read in Tx descriptor list */
                    .brx_used_read          = 1,          /** Used bit set has been read in Rx descriptor list */
                    .brx_complete           = 1,          /** Frame received successfully and stored */
                    .bman_frame             = 0,          /** Management Frame Sent */   
                    
                    /** call back function  */
                    .rxframecb  = Eth0_RxFrameCB,
                    .txerrorcb  = NULL,
                    .txcompletecb = NULL,
                    .tsuSecondInccb = Eth0_TSUIncrement,
    };
    
    /** Enable Ethernet Interrupts  */
    cy_stc_sysint_irq_t irq_cfg_eth0_q0 = {.sysIntSrc  = ETH_INTR_SRC,    .intIdx  = CPUIntIdx3_IRQn, .isEnabled  = true};
    cy_stc_sysint_irq_t irq_cfg_eth0_q1 = {.sysIntSrc  = ETH_INTR_SRC_Q1, .intIdx  = CPUIntIdx3_IRQn, .isEnabled  = true};
    cy_stc_sysint_irq_t irq_cfg_eth0_q2 = {.sysIntSrc  = ETH_INTR_SRC_Q2, .intIdx  = CPUIntIdx3_IRQn, .isEnabled  = true};    
    
    Cy_SysInt_InitIRQ(&irq_cfg_eth0_q0);
    Cy_SysInt_SetSystemIrqVector(irq_cfg_eth0_q0.sysIntSrc, Cy_Eth0_InterruptHandler);
    Cy_SysInt_InitIRQ(&irq_cfg_eth0_q1);
    Cy_SysInt_SetSystemIrqVector(irq_cfg_eth0_q1.sysIntSrc, Cy_Eth0_InterruptHandler);
    Cy_SysInt_InitIRQ(&irq_cfg_eth0_q2);
    Cy_SysInt_SetSystemIrqVector(irq_cfg_eth0_q2.sysIntSrc, Cy_Eth0_InterruptHandler);
    NVIC_SetPriority(CPUIntIdx3_IRQn, 3);
    NVIC_ClearPendingIRQ(CPUIntIdx3_IRQn);
    NVIC_EnableIRQ(CPUIntIdx3_IRQn);
    
    /** Initialize ENET MAC */
    if (CY_ETHIF_SUCCESS != Cy_EthIf_Init(ETH0, &stcENETConfig, &stcInterruptConfig))
    {
        // Error
    }
    
// EMAC End**********

    for(;;)
    {
        Cy_SysTick_DelayInUs(50000);    
        
        // Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);           
    }
}

// EMAC *****************
static void Eth0_RxFrameCB ( cy_pstc_eth_type_t pstcEth, uint8_t * u8RxBuffer, uint32_t u32Length )
{
    // TODO: copy frame or anything
    bFrameReceived = true;
    // Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);        
}

/** TSU One Second Increment Interrupt call back function   */
static void Eth0_TSUIncrement ( cy_pstc_eth_type_t pstcEth )
{
    Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
}

/** Ethernet Port Pin settings  */
static void Cy_App_Init_EthernetPortpins (void)
{
    // Ethernet Port Pin init
    Cy_GPIO_Pin_Init(ETH0_TD0_PORT, ETH0_TD0_PIN, &eth0_tx0);
    Cy_GPIO_Pin_Init(ETH0_TD1_PORT, ETH0_TD1_PIN, &eth0_tx1);
    Cy_GPIO_Pin_Init(ETH0_TD2_PORT, ETH0_TD2_PIN, &eth0_tx2);
    Cy_GPIO_Pin_Init(ETH0_TD3_PORT, ETH0_TD3_PIN, &eth0_tx3);
    
    // Cy_GPIO_Pin_Init(ETH0_TXER_PORT, ETH0_TXER_PIN, &eth0_txer);
    Cy_GPIO_Pin_Init(ETH0_TX_CTL_PORT, ETH0_TX_CTL_PIN, &eth0_txctl);
    
    Cy_GPIO_Pin_Init(ETH0_RD0_PORT, ETH0_RD0_PIN, &eth0_rx0);
    Cy_GPIO_Pin_Init(ETH0_RD1_PORT, ETH0_RD1_PIN, &eth0_rx1);
    Cy_GPIO_Pin_Init(ETH0_RD2_PORT, ETH0_RD2_PIN, &eth0_rx2);
    Cy_GPIO_Pin_Init(ETH0_RD3_PORT, ETH0_RD3_PIN, &eth0_rx3);
    
    Cy_GPIO_Pin_Init(ETH0_RX_ER_PORT, ETH0_RX_ER_PIN, &eth0_rxer);
    Cy_GPIO_Pin_Init(ETH0_RX_CTL_PORT, ETH0_RX_CTL_PIN, &eth0_rxctl);
        
    Cy_GPIO_Pin_Init(ETH0_TX_CLK_PORT, ETH0_TX_CLK_PIN, &eth0_txclk);
    Cy_GPIO_Pin_Init(ETH0_RX_CLK_PORT, ETH0_RX_CLK_PIN, &eth0_rxclk);
    Cy_GPIO_Pin_Init(ETH0_REF_CLK_PORT, ETH0_REF_CLK_PIN, &eth0_refclk);
    
    Cy_GPIO_Pin_Init(ETH0_MDC_PORT,  ETH0_MDC_PIN, &eth0_mdc);
    Cy_GPIO_Pin_Init(ETH0_MDIO_PORT, ETH0_MDIO_PIN, &eth0_mdio); 
}
// EMAC END *******

/* [] END OF FILE */
