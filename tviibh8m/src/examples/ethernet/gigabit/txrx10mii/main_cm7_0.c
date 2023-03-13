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
// #include <stdbool.h>
#include "drivers\ethernet\cy_ethif.h"

/********************************************************/
#define EMAC_MII                0
#define EMAC_RMII               1           /** Need to add manual delay about 6 [ns] for GTX_CLK **/
#define EMAC_GMII               2           /** On PSVP GMII cannot be tested   */
#define EMAC_RGMII              3

#define ETH_LINKSPEED_10        10
#define ETH_LINKSPEED_100       100
#define ETH_LINKSPEED_1000      1000

/********************************************************/
/** PHY Mode Selection      */
#define EMAC_INTERFACE              EMAC_MII
#define EMAC_LINKSPEEP              ETH_LINKSPEED_100
/*******************************************************/

#define USER_LED_PORT           CY_LED8_PORT
#define USER_LED_PIN            CY_LED8_PIN
#define USER_LED_PIN_MUX        CY_LED8_PIN_MUX

/********************************************************/

#define ETH_REG_BASE         CY_GIG_ETH_TYPE

#define ETH_INTR_SRC         (CY_GIG_ETH_IRQN0)
#define ETH_INTR_SRC_Q1      (CY_GIG_ETH_IRQN1)
#define ETH_INTR_SRC_Q2      (CY_GIG_ETH_IRQN2)

#define ETHx_TD0_PORT        CY_GIG_ETH_TD0_PORT
#define ETHx_TD0_PIN         CY_GIG_ETH_TD0_PIN
#define ETHx_TD0_PIN_MUX     CY_GIG_ETH_TD0_PIN_MUX

#define ETHx_TD1_PORT        CY_GIG_ETH_TD1_PORT
#define ETHx_TD1_PIN         CY_GIG_ETH_TD1_PIN
#define ETHx_TD1_PIN_MUX     CY_GIG_ETH_TD1_PIN_MUX

#define ETHx_TD2_PORT        CY_GIG_ETH_TD2_PORT
#define ETHx_TD2_PIN         CY_GIG_ETH_TD2_PIN
#define ETHx_TD2_PIN_MUX     CY_GIG_ETH_TD2_PIN_MUX

#define ETHx_TD3_PORT        CY_GIG_ETH_TD3_PORT
#define ETHx_TD3_PIN         CY_GIG_ETH_TD3_PIN
#define ETHx_TD3_PIN_MUX     CY_GIG_ETH_TD3_PIN_MUX

#define ETHx_TD4_PORT        CY_GIG_ETH_TD4_PORT
#define ETHx_TD4_PIN         CY_GIG_ETH_TD4_PIN
#define ETHx_TD4_PIN_MUX     CY_GIG_ETH_TD4_PIN_MUX

#define ETHx_TD5_PORT        CY_GIG_ETH_TD5_PORT
#define ETHx_TD5_PIN         CY_GIG_ETH_TD5_PIN
#define ETHx_TD5_PIN_MUX     CY_GIG_ETH_TD5_PIN_MUX

#define ETHx_TD6_PORT        CY_GIG_ETH_TD6_PORT
#define ETHx_TD6_PIN         CY_GIG_ETH_TD6_PIN
#define ETHx_TD6_PIN_MUX     CY_GIG_ETH_TD6_PIN_MUX

#define ETHx_TD7_PORT        CY_GIG_ETH_TD7_PORT
#define ETHx_TD7_PIN         CY_GIG_ETH_TD7_PIN
#define ETHx_TD7_PIN_MUX     CY_GIG_ETH_TD7_PIN_MUX

#define ETHx_TXER_PORT       CY_GIG_ETH_TXER_PORT
#define ETHx_TXER_PIN        CY_GIG_ETH_TXER_PIN
#define ETHx_TXER_PIN_MUX    CY_GIG_ETH_TXER_PIN_MUX

#define ETHx_TX_CTL_PORT     CY_GIG_ETH_TX_CTL_PORT
#define ETHx_TX_CTL_PIN      CY_GIG_ETH_TX_CTL_PIN
#define ETHx_TX_CTL_PIN_MUX  CY_GIG_ETH_TX_CTL_PIN_MUX

#define ETHx_RD0_PORT        CY_GIG_ETH_RD0_PORT
#define ETHx_RD0_PIN         CY_GIG_ETH_RD0_PIN
#define ETHx_RD0_PIN_MUX     CY_GIG_ETH_RD0_PIN_MUX

#define ETHx_RD1_PORT        CY_GIG_ETH_RD1_PORT
#define ETHx_RD1_PIN         CY_GIG_ETH_RD1_PIN
#define ETHx_RD1_PIN_MUX     CY_GIG_ETH_RD1_PIN_MUX

#define ETHx_RD2_PORT        CY_GIG_ETH_RD2_PORT
#define ETHx_RD2_PIN         CY_GIG_ETH_RD2_PIN
#define ETHx_RD2_PIN_MUX     CY_GIG_ETH_RD2_PIN_MUX

#define ETHx_RD3_PORT        CY_GIG_ETH_RD3_PORT
#define ETHx_RD3_PIN         CY_GIG_ETH_RD3_PIN
#define ETHx_RD3_PIN_MUX     CY_GIG_ETH_RD3_PIN_MUX

#define ETHx_RD4_PORT        CY_GIG_ETH_RD4_PORT
#define ETHx_RD4_PIN         CY_GIG_ETH_RD4_PIN
#define ETHx_RD4_PIN_MUX     CY_GIG_ETH_RD4_PIN_MUX

#define ETHx_RD5_PORT        CY_GIG_ETH_RD5_PORT
#define ETHx_RD5_PIN         CY_GIG_ETH_RD5_PIN
#define ETHx_RD5_PIN_MUX     CY_GIG_ETH_RD5_PIN_MUX

#define ETHx_RD6_PORT        CY_GIG_ETH_RD6_PORT
#define ETHx_RD6_PIN         CY_GIG_ETH_RD6_PIN
#define ETHx_RD6_PIN_MUX     CY_GIG_ETH_RD6_PIN_MUX

#define ETHx_RD7_PORT        CY_GIG_ETH_RD7_PORT
#define ETHx_RD7_PIN         CY_GIG_ETH_RD7_PIN
#define ETHx_RD7_PIN_MUX     CY_GIG_ETH_RD7_PIN_MUX

#define ETHx_RX_CTL_PORT     CY_GIG_ETH_RX_CTL_PORT
#define ETHx_RX_CTL_PIN      CY_GIG_ETH_RX_CTL_PIN
#define ETHx_RX_CTL_PIN_MUX  CY_GIG_ETH_RX_CTL_PIN_MUX

#define ETHx_RX_ER_PORT      CY_GIG_ETH_RX_ER_PORT
#define ETHx_RX_ER_PIN       CY_GIG_ETH_RX_ER_PIN
#define ETHx_RX_ER_PIN_MUX   CY_GIG_ETH_RX_ER_PIN_MUX

#define ETHx_TX_CLK_PORT     CY_GIG_ETH_TX_CLK_PORT
#define ETHx_TX_CLK_PIN      CY_GIG_ETH_TX_CLK_PIN
#define ETHx_TX_CLK_PIN_MUX  CY_GIG_ETH_TX_CLK_PIN_MUX

#define ETHx_RX_CLK_PORT     CY_GIG_ETH_RX_CLK_PORT
#define ETHx_RX_CLK_PIN      CY_GIG_ETH_RX_CLK_PIN
#define ETHx_RX_CLK_PIN_MUX  CY_GIG_ETH_RX_CLK_PIN_MUX

#define ETHx_REF_CLK_PORT    CY_GIG_ETH_REF_CLK_PORT
#define ETHx_REF_CLK_PIN     CY_GIG_ETH_REF_CLK_PIN
#define ETHx_REF_CLK_PIN_MUX CY_GIG_ETH_REF_CLK_PIN_MUX

#define ETHx_MDC_PORT        CY_GIG_ETH_MDC_PORT
#define ETHx_MDC_PIN         CY_GIG_ETH_MDC_PIN
#define ETHx_MDC_PIN_MUX     CY_GIG_ETH_MDC_PIN_MUX

#define ETHx_MDIO_PORT       CY_GIG_ETH_MDIO_PORT
#define ETHx_MDIO_PIN        CY_GIG_ETH_MDIO_PIN
#define ETHx_MDIO_PIN_MUX    CY_GIG_ETH_MDIO_PIN_MUX

/** Interrupt Handlers for Ethernet 1     */
void Cy_Ethx_InterruptHandler (void)
{
    Cy_EthIf01_DecodeEvent();
}

/********************************************************/

/** PHY related constants   */  
#define PHY_ADDR                    (0)                         // Value depends on PHY and its hardware configurations

#define ETH_FRAME_SIZE               64 // Min: 64 byte, Max:1518 byte
#define ETH_FRAME_SIZE_FCS            4 // (This is contatnt value)
#define ETH_FRAME_SIZE_HEADER        14 // 6byte for destination address, 6 byte for source address, 2 byte for Type (This is constant)
#define ETH_FRAME_SIZE_EXCLUDING_FCS (ETH_FRAME_SIZE - ETH_FRAME_SIZE_FCS)
#define ETH_FRAME_SIZE_DATA          (ETH_FRAME_SIZE_EXCLUDING_FCS - ETH_FRAME_SIZE_HEADER)

/********************************************************/

cy_stc_gpio_pin_config_t user_led_port_pin_cfg =
{
    .outVal    = 0x00,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = USER_LED_PIN_MUX,
    .intEdge   = 0,
    .intMask   = 0,
    .vtrip     = 0,
    .slewRate  = 0,
    .driveSel  = 0,
    .vregEn    = 0,
    .ibufMode  = 0,
    .vtripSel  = 0,
    .vrefSel   = 0,
    .vohSel    = 0,
};


/********************************************************/
// EMAC *********

static void Cy_App_Init_EthernetPortpins(void);
static void Ethx_RxFrameCB ( cy_pstc_eth_type_t pstcEth, uint8_t * u8RxBuffer, uint32_t u32Length );
static void Ethx_TSUIncrement ( cy_pstc_eth_type_t pstcEth );
static void InitPHY_DP83867IR(void);
static bool Check_DP83867IR_LinkStatus (void);
static void LED_Control_FromLED0ToLED7(uint8_t dataToBeDisplayed);

// The different interface modes are specified with different input voltage threshold levels 
#if (EMAC_INTERFACE == EMAC_MII) || (EMAC_INTERFACE == EMAC_RMII)
    #define ETH_VTRIP_SEL     1
#elif (EMAC_INTERFACE == EMAC_RGMII)
    #define ETH_VTRIP_SEL     0
#elif (EMAC_INTERFACE == EMAC_GMII)
    #define ETH_VTRIP_SEL     3
#endif


/**                      PortPinName.outVal||  driveMode               hsiom             ||intEdge||intMask||vtrip||slewRate||driveSel||vregEn||ibufMode||     vtripSel       ||vrefSel||vohSel*/
cy_stc_gpio_pin_config_t ethx_tx0   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TD0_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_tx1   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TD1_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_tx2   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TD2_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_tx3   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TD3_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};

cy_stc_gpio_pin_config_t ethx_txer  = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TXER_PIN_MUX,    0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_txctl = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TX_CTL_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
                                                                                           
cy_stc_gpio_pin_config_t ethx_rx0   = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RD0_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_rx1   = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RD1_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_rx2   = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RD2_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_rx3   = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RD3_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};

cy_stc_gpio_pin_config_t ethx_rxctl = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RX_CTL_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_rxer  = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RX_ER_PIN_MUX,   0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
                                                                       
#if EMAC_INTERFACE == EMAC_MII
cy_stc_gpio_pin_config_t ethx_txclk = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_TX_CLK_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
#else
cy_stc_gpio_pin_config_t ethx_txclk = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TX_CLK_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
#endif

cy_stc_gpio_pin_config_t ethx_rxclk =  {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RX_CLK_PIN_MUX, 0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_refclk = {0x00, CY_GPIO_DM_HIGHZ,        ETHx_REF_CLK_PIN_MUX, 0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};

cy_stc_gpio_pin_config_t ethx_mdc   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_MDC_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        1,                    0,       0};
cy_stc_gpio_pin_config_t ethx_mdio  = {0x00, CY_GPIO_DM_STRONG,        ETHx_MDIO_PIN_MUX,    0,       0,       0,     0,        0,        0,      0,        1,                    0,       0};

/** Wrapper configuration   */
cy_str_ethif_wrapper_config stcWrapperConfig = 
{
  #if ((EMAC_INTERFACE == EMAC_MII) && (EMAC_LINKSPEEP == ETH_LINKSPEED_10))
    .stcInterfaceSel = CY_ETHIF_CTL_MII_10,    /** 10 Mbps MII */ 
  #elif ((EMAC_INTERFACE == EMAC_MII) && (EMAC_LINKSPEEP == ETH_LINKSPEED_100))
    .stcInterfaceSel = CY_ETHIF_CTL_MII_100,    /** 100 Mbps MII */ 
  #elif ((EMAC_INTERFACE == EMAC_RGMII) && (EMAC_LINKSPEEP == ETH_LINKSPEED_10))
    .stcInterfaceSel = CY_ETHIF_CTL_RGMII_10,    /** 10 Mbps RGMII */ 
  #elif ((EMAC_INTERFACE == EMAC_RGMII) && (EMAC_LINKSPEEP == ETH_LINKSPEED_100))
    .stcInterfaceSel = CY_ETHIF_CTL_RGMII_100,    /** 100 Mbps RGMII */ 
  #endif
    .bRefClockSource = CY_ETHIF_EXTERNAL_HSIO, /** assigning Ref_Clk to HSIO Clock, it is recommended to use external clock coming from HSIO  */
  #if EMAC_LINKSPEEP == ETH_LINKSPEED_10
    .u8RefClkDiv = 10,                         /** Divide Refclock by 10 to have 2.5 MHz tx clock  */
  #elif EMAC_LINKSPEEP == ETH_LINKSPEED_100
    .u8RefClkDiv = 1,                         /** Divide Refclock by 1 to have 25 MHz Tx clock  */
  #endif
};

/** Default Timer register values   */
CEDI_1588TimerVal stc1588TimerValue = 
{
    .secsUpper = 0,
    .secsLower = 0,
    .nanosecs  = 0,
};

/** Increment values for each clock cycles      */
/** This Increment values are calculated for source clock of 24 MHz */
CEDI_TimerIncrement stc1588TimerIncValue =
{
    .nanoSecsInc = 41,    /** Having source clock of 24 MHz, with each clock cycle Nanosecond counter shall be incremented by 41  */
    .subNsInc    = 43690, /** incrementing just Nanosecond counter will not give accuracy, so sub-nanosecond counter also must be incremented  */  
    .lsbSubNsInc = 170,   /** 170 (0xAA) is the lower 8 bits increment value for subNanosecond and 43690 (0xAAAA) is the higher 16 bits of the counter */
    .altIncCount = 0,     /** This example is not using alternate increment mode, also it is not recommended by IP provider       */
    .altNanoSInc = 0,     /** When Alt increment mode is disabled, then this counter does not play any role          */
};

/** To calculate the value to write to the sub-ns register you take the decimal value of 
    the sub-nanosecond value, then multiply it by 2 to the power of 24 (16777216) and 
    convert the result to hexadecimal. For example for a sub-nanosecond value of 0.33333333 
    you would write 0x55005555.  */ 
/** TSU configuration   */
const cy_str_ethif_TSU_Config stcTSUConfig = 
{
    .pstcTimerValue         = &stc1588TimerValue,    /** start value for the counter     */
    .pstcTimerIncValue      = &stc1588TimerIncValue, /** Increment value for each clock cycle    */
    .bOneStepTxSyncEnable   = false,                 /** useful when PTP protocol is in place    */
    .enTxDescStoreTimeStamp = CEDI_TX_TS_DISABLED,
    .enRxDescStoreTimeStamp = CEDI_RX_TS_DISABLED,
    .bStoreNSinRxDesc       = false,
};

/** General Ethernet configuration  */
cy_stc_ethif_configuration_t stcENETConfig =
{
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
    .pstcTSUConfig       = NULL,                        /** No TSU settings */
    .btxq0enable         = 1,                           /** Tx Q0 Enabled   */
    .btxq1enable         = 0,                           /** Tx Q1 Disabled  */            
    .btxq2enable         = 0,                           /** Tx Q2 Disabled  */
    .brxq0enable         = 1,                           /** Rx Q0 Enabled   */
    .brxq1enable         = 0,                           /** Rx Q1 Disabled  */
    .brxq2enable         = 0,                           /** Rx Q2 Disabled  */
};
    
/** Interrupt configurations    */
cy_stc_ethif_interruptconfig_t stcInterruptConfig =
{
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
    .rxframecb  = Ethx_RxFrameCB,
    .txerrorcb  = NULL,
    .txcompletecb = NULL,
    .tsuSecondInccb = NULL,
};


    /** Enable Ethernet Interrupts  */
const cy_stc_sysint_irq_t irq_cfg_ethx_q0 = {.sysIntSrc  = ETH_INTR_SRC,    .intIdx  = CPUIntIdx3_IRQn, .isEnabled  = true};
const cy_stc_sysint_irq_t irq_cfg_ethx_q1 = {.sysIntSrc  = ETH_INTR_SRC_Q1, .intIdx  = CPUIntIdx3_IRQn, .isEnabled  = true};
const cy_stc_sysint_irq_t irq_cfg_ethx_q2 = {.sysIntSrc  = ETH_INTR_SRC_Q2, .intIdx  = CPUIntIdx3_IRQn, .isEnabled  = true};

#pragma pack(1)
typedef struct
{
    uint8_t desMacAddr[6];
    uint8_t srcMacAddr[6];
    uint16_t type;
    uint8_t data[ETH_FRAME_SIZE_DATA];
} cy_stc_ether_frame_t;
#pragma pack()

/********************************************************/

/** Transmit source buffer  */
#pragma alignvar (8) 
//__attribute((aligned(8))) 
cy_stc_ether_frame_t g_txFrameTest =
{
    .desMacAddr = { 0x04, 0x00, 0x00, 0x00, 0x00, 0x04, },  // Broad cast
    .srcMacAddr = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x02, },
    .type       = 0xF022,
    .data       =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 
    }
};

cy_stc_eth_filter_config_t fileter1Config = 
{
    .typeFilter      = CY_ETH_FILTER_TYPE_DESTINATION,
    .filterAddr.byte = {0x02, 0x00, 0x00, 0x00, 0x00, 0x02},
    .ignoreBytes     = 0x00,
};

cy_stc_ether_frame_t g_rxFrameTest = { 0 };

bool bFrameReceived = false;

#define CLEAR_ALL_TRUE (true)
#define TRANSMIT_BUF_TRUE (true)
#define TRANSMIT_BUF_FALSE (true)
#define LAST_BUFFER_TRUE (true)

/********************************************************/

//EMAC End ***********

/********************************************************/
int main(void)
{
    SystemInit();

    // Example had been originally tested with "cache off", so ensure that caches are turned off (may have been enabled by new startup.c module)
    SCB_DisableICache(); // Disables and invalidates instruction cache
    SCB_DisableDCache(); // Disables, cleans and invalidates data cache

    __enable_irq();

    /*******************************/
    /******** Port Settings ********/
    /*******************************/
    /* configure GPIO Port pins */
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);

    /** Configure Ethernet Port pins    */
    Cy_App_Init_EthernetPortpins();

    /************************************/
    /******** Interrupt settings ********/
    /************************************/
    Cy_SysInt_InitIRQ(&irq_cfg_ethx_q0);
    Cy_SysInt_SetSystemIrqVector(irq_cfg_ethx_q0.sysIntSrc, Cy_Ethx_InterruptHandler);

    Cy_SysInt_InitIRQ(&irq_cfg_ethx_q1);
    Cy_SysInt_SetSystemIrqVector(irq_cfg_ethx_q1.sysIntSrc, Cy_Ethx_InterruptHandler);

    Cy_SysInt_InitIRQ(&irq_cfg_ethx_q2);
    Cy_SysInt_SetSystemIrqVector(irq_cfg_ethx_q2.sysIntSrc, Cy_Ethx_InterruptHandler);
    
    NVIC_SetPriority(CPUIntIdx3_IRQn, 3);
    NVIC_ClearPendingIRQ(CPUIntIdx3_IRQn);
    NVIC_EnableIRQ(CPUIntIdx3_IRQn);
    

    /********************************************************/
    // EMAC*********
    cy_en_ethif_status_t ethStatus;

    /** Initialize ENET MAC */
    ethStatus = Cy_EthIf_Init(ETH_REG_BASE, &stcENETConfig, &stcInterruptConfig);
    CY_ASSERT(ethStatus == CY_ETHIF_SUCCESS);

    /* Disable copy all frames */
    Cy_EthIf_SetCopyAllFrames(ETH_REG_BASE, false);

    /* Reject Broad cast frames */
    Cy_EthIf_SetNoBroadCast(ETH_REG_BASE, true);

    /* Filter settings */
    ethStatus = Cy_EthIf_SetFilterAddress(ETH_REG_BASE, CY_ETH_FILTER_NUM_1, &fileter1Config);
    CY_ASSERT(ethStatus == CY_ETHIF_SUCCESS);

    /** PHY initialization  */
    InitPHY_DP83867IR();

    /** Wait for the PHY becoming ready  */
    while(1)
    {
        if (Check_DP83867IR_LinkStatus() == true)
        {
            break;
        }
    }

    /** Configures Ethernet Pause frames  | Not checked with respect to quantums */
    // Cy_EthIf_ConfigPause(ETH_REG_BASE, 1000);
    // Cy_EthIf_TxPauseFrame(ETH_REG_BASE, 0);
    
    // EMAC End**********
    for(;;)
    {
        if (bFrameReceived)
        {
            bFrameReceived = false;

            /** Transmit Fixed Dummy Frame  */
            ethStatus = Cy_EthIf_TransmitFrame(ETH_REG_BASE, (uint8_t*)&g_txFrameTest, ETH_FRAME_SIZE_EXCLUDING_FCS, CY_ETH_QS0_0, LAST_BUFFER_TRUE);
            CY_ASSERT(ethStatus == CY_ETHIF_SUCCESS);

            /** Clear all released buffer for both Transmit and Receive    */
            Cy_EthIF_ClearReleasedBuf(CLEAR_ALL_TRUE, TRANSMIT_BUF_TRUE);
            Cy_EthIF_ClearReleasedBuf(CLEAR_ALL_TRUE, TRANSMIT_BUF_FALSE);
        }
    }
}

// EMAC *****************
static void Ethx_RxFrameCB ( cy_pstc_eth_type_t pstcEth, uint8_t * u8RxBuffer, uint32_t u32Length )
{
    uint8_t macSrcAddrToBeReply[6] = {0x04,0x00,0x00,0x00,0x00,0x04};

    if(memcmp(macSrcAddrToBeReply, &u8RxBuffer[6], 6u) == 0)
    {
        memcpy((uint8_t*)&g_rxFrameTest, u8RxBuffer, sizeof(cy_stc_ether_frame_t));
        bFrameReceived = true;

        Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
        LED_Control_FromLED0ToLED7(g_rxFrameTest.data[0]);
    }
}

/** TSU One Second Increment Interrupt call back function   */
static void Ethx_TSUIncrement ( cy_pstc_eth_type_t pstcEth )
{
    Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
}

/** Ethernet Port Pin settings  */
static void Cy_App_Init_EthernetPortpins (void)
{
    // Ethernet Port Pin init
    Cy_GPIO_Pin_Init(ETHx_TD0_PORT, ETHx_TD0_PIN, &ethx_tx0);                       /** TX0 */
    Cy_GPIO_Pin_Init(ETHx_TD1_PORT, ETHx_TD1_PIN, &ethx_tx1);                       /** TX1 */
    Cy_GPIO_Pin_Init(ETHx_TD2_PORT, ETHx_TD2_PIN, &ethx_tx2);                       /** TX2 */
    Cy_GPIO_Pin_Init(ETHx_TD3_PORT, ETHx_TD3_PIN, &ethx_tx3);                       /** TX3 */
#if EMAC_INTERFACE == EMAC_GMII
    Cy_GPIO_Pin_Init(ETHx_TD4_PORT, ETHx_TD4_PIN, &ethx_tx4);                       /** TX4 */
    Cy_GPIO_Pin_Init(ETHx_TD5_PORT, ETHx_TD5_PIN, &ethx_tx5);                       /** TX5 */
    Cy_GPIO_Pin_Init(ETHx_TD6_PORT, ETHx_TD6_PIN, &ethx_tx6);                       /** TX6 */
    Cy_GPIO_Pin_Init(ETHx_TD7_PORT, ETHx_TD7_PIN, &ethx_tx7);                       /** TX7 */
#endif

    Cy_GPIO_Pin_Init(ETHx_TXER_PORT, ETHx_TXER_PIN, &ethx_txer);
    Cy_GPIO_Pin_Init(ETHx_TX_CTL_PORT, ETHx_TX_CTL_PIN, &ethx_txctl);               /** TX_CTL  */
    
    Cy_GPIO_Pin_Init(ETHx_RD0_PORT, ETHx_RD0_PIN, &ethx_rx0);                       /** RX0 */
    Cy_GPIO_Pin_Init(ETHx_RD1_PORT, ETHx_RD1_PIN, &ethx_rx1);                       /** RX1 */
    Cy_GPIO_Pin_Init(ETHx_RD2_PORT, ETHx_RD2_PIN, &ethx_rx2);                       /** RX2 */
    Cy_GPIO_Pin_Init(ETHx_RD3_PORT, ETHx_RD3_PIN, &ethx_rx3);                       /** RX3 */
#if EMAC_INTERFACE == EMAC_GMII    
    Cy_GPIO_Pin_Init(ETHx_RD4_PORT, ETHx_RD4_PIN, &ethx_rx4);                       /** RX4 */
    Cy_GPIO_Pin_Init(ETHx_RD5_PORT, ETHx_RD5_PIN, &ethx_rx5);                       /** RX5 */
    Cy_GPIO_Pin_Init(ETHx_RD6_PORT, ETHx_RD6_PIN, &ethx_rx6);                       /** RX6 */
    Cy_GPIO_Pin_Init(ETHx_RD7_PORT, ETHx_RD7_PIN, &ethx_rx7);                       /** RX7 */
#endif   

#if EMAC_INTERFACE != EMAC_RGMII
    Cy_GPIO_Pin_Init(ETHx_RX_ER_PORT, ETHx_RX_ER_PIN, &ethx_rxer);                  /** RX_ER   */
#endif
    Cy_GPIO_Pin_Init(ETHx_RX_CTL_PORT, ETHx_RX_CTL_PIN, &ethx_rxctl);               /** RX_CTL  */
        
#if EMAC_INTERFACE != EMAC_MII
    Cy_GPIO_Pin_Init(ETHx_REF_CLK_PORT, ETHx_REF_CLK_PIN, &ethx_refclk);            /** REF_CLK */
#endif

    Cy_GPIO_Pin_Init(ETHx_TX_CLK_PORT, ETHx_TX_CLK_PIN, &ethx_txclk);               /** TX_CLK  */
    Cy_GPIO_Pin_Init(ETHx_RX_CLK_PORT, ETHx_RX_CLK_PIN, &ethx_rxclk);               /** RX_CLK  */

    Cy_GPIO_Pin_Init(ETHx_MDC_PORT,  ETHx_MDC_PIN, &ethx_mdc);                      /** MDC     */
    Cy_GPIO_Pin_Init(ETHx_MDIO_PORT, ETHx_MDIO_PIN, &ethx_mdio);                    /** MDIO    */
}

/** Please read data sheet section 8.4.2.1 for procedure in detail */
static void PHY_DP83867IR_WriteExtendedReg(cy_pstc_eth_type_t pstcEth, uint16_t u16RegNo, uint16_t u16Data, uint8_t u8PHYAddr)
{
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x001F,   PHY_ADDR);
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, u16RegNo, PHY_ADDR);
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x401F,   PHY_ADDR);
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, u16Data,  PHY_ADDR);
}

static uint16_t PHY_DP83867IR_ReadExtendedReg(cy_pstc_eth_type_t pstcEth, uint16_t u16RegNo, uint8_t u8PHYAddr)
{
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x001F,   PHY_ADDR);
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, u16RegNo, PHY_ADDR);
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x401F,   PHY_ADDR);
    return (Cy_EthIf_PhyRegRead(ETH_REG_BASE, 0x0E, PHY_ADDR));
}

/*******************************************************************************
* Function Name: InitPHY_DP83867IR
****************************************************************************//**
*
* \brief Dedicated to initialize PHY DP83867IR
* Function configures the PHY with 10 Mbps link speed, full duplex communication and Auto Negotiation off
*
* \Note:
* If hardware consist of any other PHY than DP83867IR, dedicated function shall be written to configure relevant 
* registers in side PHY
*******************************************************************************/
static void InitPHY_DP83867IR (void)
{
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x1F, 0x8000, PHY_ADDR); /** Reg CTRl: Perform a full reset, including All registers   */ 

#if EMAC_LINKSPEEP == ETH_LINKSPEED_10
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x00, 0x0100, PHY_ADDR); /** 10M, Full Duplex and Auto Negotiation OFF  */
#elif EMAC_LINKSPEEP == ETH_LINKSPEED_100
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x00, 0x2100, PHY_ADDR); /** 100M, Full Duplex and Auto Negotiation OFF  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x10, 0x5028, PHY_ADDR); /** Disable auto neg for MDI/MDI-X **/
#elif EMAC_LINKSPEEP == ETH_LINKSPEED_1000
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x00, 0x0140, PHY_ADDR); /** 1000M, Full Duplex and Auto Negotiation OFF  */
#endif

#if EMAC_INTERFACE == EMAC_RGMII
    PHY_DP83867IR_WriteExtendedReg(ETH_REG_BASE, 0x0032, 0x00D3, PHY_ADDR); /** Enter Tx and RX Clock delay in RGMII configuration register  */ 
    PHY_DP83867IR_WriteExtendedReg(ETH_REG_BASE, 0x0086, 0x00FF, PHY_ADDR); /** Internal Delay Configuration  */
                                                                            /** Note: TX CLK delay = 4[ns](maximum). This is possibly not sufficient. If transmission won't succeeded, please try manual delay TX clock. */
#else
    PHY_DP83867IR_WriteExtendedReg(ETH_REG_BASE, 0x0032, 0x0000, PHY_ADDR); /** Disable RGMII  */
#endif

    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x1F, 0x4000, PHY_ADDR); /** Reg CTRl: perform restart */ 
}

/*******************************************************************************
* Function Name: Check_DP83867IR_LinkStatus
****************************************************************************//**
*
* \brief Function reads specific register from DP83867IR to learn Link status
*
* \param 
* \return true Link up
* \return false Link Down 
*
*******************************************************************************/
static bool Check_DP83867IR_LinkStatus (void)
{
    uint32_t    u32ReadData = 0;
    
    /** PHY register might take some time to update internal registers */    
    u32ReadData = Cy_EthIf_PhyRegRead(ETH_REG_BASE, (uint8_t)0x11, PHY_ADDR);       /** PHY status register: 0x0011 */
    if (u32ReadData & 0x0400)
    {
        /** Link up */
        return true;
    }
    else
    {
        /** Link down   */
        return false;
    }   
}

// EMAC END *******

static void LED_Control_FromLED0ToLED7(uint8_t dataToBeDisplayed)
{
    static bool IsThisFirstTime = true;

    cy_stc_gpio_pin_config_t portConfigForLED = { 0u };
    portConfigForLED.driveMode = CY_GPIO_DM_STRONG_IN_OFF;

    if(IsThisFirstTime)
    {
        Cy_GPIO_Pin_Init(CY_LED0_PORT, CY_LED0_PIN, &portConfigForLED);
        Cy_GPIO_Pin_Init(CY_LED1_PORT, CY_LED1_PIN, &portConfigForLED);
        Cy_GPIO_Pin_Init(CY_LED2_PORT, CY_LED2_PIN, &portConfigForLED);
        Cy_GPIO_Pin_Init(CY_LED3_PORT, CY_LED3_PIN, &portConfigForLED);
        Cy_GPIO_Pin_Init(CY_LED4_PORT, CY_LED4_PIN, &portConfigForLED);
        Cy_GPIO_Pin_Init(CY_LED5_PORT, CY_LED5_PIN, &portConfigForLED);
        Cy_GPIO_Pin_Init(CY_LED6_PORT, CY_LED6_PIN, &portConfigForLED);
        Cy_GPIO_Pin_Init(CY_LED7_PORT, CY_LED7_PIN, &portConfigForLED);
        IsThisFirstTime = false;
    }

    bool ledLevel;
    ledLevel = ((dataToBeDisplayed >> 0) & 0x01) == 0x01;
    Cy_GPIO_Write(CY_LED0_PORT, CY_LED0_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 1) & 0x01) == 0x01;
    Cy_GPIO_Write(CY_LED1_PORT, CY_LED1_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 2) & 0x01) == 0x01;
    Cy_GPIO_Write(CY_LED2_PORT, CY_LED2_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 3) & 0x01) == 0x01;
    Cy_GPIO_Write(CY_LED3_PORT, CY_LED3_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 4) & 0x01) == 0x01;
    Cy_GPIO_Write(CY_LED4_PORT, CY_LED4_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 5) & 0x01) == 0x01;
    Cy_GPIO_Write(CY_LED5_PORT, CY_LED5_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 6) & 0x01) == 0x01;
    Cy_GPIO_Write(CY_LED6_PORT, CY_LED6_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 7) & 0x01) == 0x01;
    Cy_GPIO_Write(CY_LED7_PORT, CY_LED7_PIN, ledLevel);
}

/* [] END OF FILE */
