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

/********************************************************/
#define EMAC_MII                0
#define EMAC_RMII               1

#define ETH_LINKSPEED_100       100

/********************************************************/
/** PHY Mode Selection      */
#define EMAC_INTERFACE       EMAC_MII
#define EMAC_LINKSPEED       ETH_LINKSPEED_100

/*******************************************************/
#define USER_LED_PORT        CY_LED8_PORT
#define USER_LED_PIN         CY_LED8_PIN
#define USER_LED_PIN_MUX     CY_LED8_PIN_MUX

/********************************************************/

#define ETH_REG_BASE         CY_AUTO_ETH_TYPE

#define ETH_INTR_SRC         (CY_AUTO_ETH_IRQN0)
#define ETH_INTR_SRC_Q1      (CY_AUTO_ETH_IRQN1)
#define ETH_INTR_SRC_Q2      (CY_AUTO_ETH_IRQN2)

#define ETHx_TD0_PORT        CY_AUTO_ETH_TD0_PORT
#define ETHx_TD0_PIN         CY_AUTO_ETH_TD0_PIN
#define ETHx_TD0_PIN_MUX     CY_AUTO_ETH_TD0_PIN_MUX

#define ETHx_TD1_PORT        CY_AUTO_ETH_TD1_PORT
#define ETHx_TD1_PIN         CY_AUTO_ETH_TD1_PIN
#define ETHx_TD1_PIN_MUX     CY_AUTO_ETH_TD1_PIN_MUX

#define ETHx_TD2_PORT        CY_AUTO_ETH_TD2_PORT
#define ETHx_TD2_PIN         CY_AUTO_ETH_TD2_PIN
#define ETHx_TD2_PIN_MUX     CY_AUTO_ETH_TD2_PIN_MUX

#define ETHx_TD3_PORT        CY_AUTO_ETH_TD3_PORT
#define ETHx_TD3_PIN         CY_AUTO_ETH_TD3_PIN
#define ETHx_TD3_PIN_MUX     CY_AUTO_ETH_TD3_PIN_MUX

#define ETHx_TXER_PORT       CY_AUTO_ETH_TXER_PORT
#define ETHx_TXER_PIN        CY_AUTO_ETH_TXER_PIN
#define ETHx_TXER_PIN_MUX    CY_AUTO_ETH_TXER_PIN_MUX

#define ETHx_TX_CTL_PORT     CY_AUTO_ETH_TX_CTL_PORT
#define ETHx_TX_CTL_PIN      CY_AUTO_ETH_TX_CTL_PIN
#define ETHx_TX_CTL_PIN_MUX  CY_AUTO_ETH_TX_CTL_PIN_MUX

#define ETHx_RD0_PORT        CY_AUTO_ETH_RD0_PORT
#define ETHx_RD0_PIN         CY_AUTO_ETH_RD0_PIN
#define ETHx_RD0_PIN_MUX     CY_AUTO_ETH_RD0_PIN_MUX

#define ETHx_RD1_PORT        CY_AUTO_ETH_RD1_PORT
#define ETHx_RD1_PIN         CY_AUTO_ETH_RD1_PIN
#define ETHx_RD1_PIN_MUX     CY_AUTO_ETH_RD1_PIN_MUX

#define ETHx_RD2_PORT        CY_AUTO_ETH_RD2_PORT
#define ETHx_RD2_PIN         CY_AUTO_ETH_RD2_PIN
#define ETHx_RD2_PIN_MUX     CY_AUTO_ETH_RD2_PIN_MUX

#define ETHx_RD3_PORT        CY_AUTO_ETH_RD3_PORT
#define ETHx_RD3_PIN         CY_AUTO_ETH_RD3_PIN
#define ETHx_RD3_PIN_MUX     CY_AUTO_ETH_RD3_PIN_MUX

#define ETHx_RX_CTL_PORT     CY_AUTO_ETH_RX_CTL_PORT
#define ETHx_RX_CTL_PIN      CY_AUTO_ETH_RX_CTL_PIN
#define ETHx_RX_CTL_PIN_MUX  CY_AUTO_ETH_RX_CTL_PIN_MUX

#define ETHx_RX_ER_PORT      CY_AUTO_ETH_RX_ER_PORT
#define ETHx_RX_ER_PIN       CY_AUTO_ETH_RX_ER_PIN
#define ETHx_RX_ER_PIN_MUX   CY_AUTO_ETH_RX_ER_PIN_MUX

#define ETHx_TX_CLK_PORT     CY_AUTO_ETH_TX_CLK_PORT
#define ETHx_TX_CLK_PIN      CY_AUTO_ETH_TX_CLK_PIN
#define ETHx_TX_CLK_PIN_MUX  CY_AUTO_ETH_TX_CLK_PIN_MUX

#define ETHx_RX_CLK_PORT     CY_AUTO_ETH_RX_CLK_PORT
#define ETHx_RX_CLK_PIN      CY_AUTO_ETH_RX_CLK_PIN
#define ETHx_RX_CLK_PIN_MUX  CY_AUTO_ETH_RX_CLK_PIN_MUX

#define ETHx_REF_CLK_PORT    CY_AUTO_ETH_REF_CLK_PORT
#define ETHx_REF_CLK_PIN     CY_AUTO_ETH_REF_CLK_PIN
#define ETHx_REF_CLK_PIN_MUX CY_AUTO_ETH_REF_CLK_PIN_MUX

#define ETHx_MDC_PORT        CY_AUTO_ETH_MDC_PORT
#define ETHx_MDC_PIN         CY_AUTO_ETH_MDC_PIN
#define ETHx_MDC_PIN_MUX     CY_AUTO_ETH_MDC_PIN_MUX

#define ETHx_MDIO_PORT       CY_AUTO_ETH_MDIO_PORT
#define ETHx_MDIO_PIN        CY_AUTO_ETH_MDIO_PIN
#define ETHx_MDIO_PIN_MUX    CY_AUTO_ETH_MDIO_PIN_MUX

/** Interrupt Handlers for Ethernet 0     */
void Cy_Ethx_InterruptHandler (void)
{
    Cy_EthIf00_DecodeEvent();
}

/********************************************************/

/** PHY related constants   */  
#define PHY_ADDR                    (0)                         // Value depends on PHY and its hardware configurations

#define ETH_FRAME_SIZE               64 // Min: 64 byte, Max:1518 byte
#define ETH_FRAME_SIZE_FCS            4 // Frame check sequence length. This is constatnt.
#define ETH_FRAME_SIZE_HEADER        14 // 6byte for destination address, 6 byte for source address, 2 byte for Type (This is constant)
#define ETH_FRAME_SIZE_EXCLUDING_FCS (ETH_FRAME_SIZE - ETH_FRAME_SIZE_FCS)
#define ETH_FRAME_SIZE_DATA          (ETH_FRAME_SIZE_EXCLUDING_FCS - ETH_FRAME_SIZE_HEADER)

/********************************************************/

cy_stc_gpio_pin_config_t user_led_port_pin_cfg =
{
    .outVal    = 0u,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = USER_LED_PIN_MUX,
    .intEdge   = 0u,
    .intMask   = 0u,
    .vtrip     = 0u,
    .slewRate  = 0u,
    .driveSel  = 0u,
};

/********************************************************/
// EMAC *********

static void Cy_App_Init_EthernetPortpins(void);
static void Ethx_RxFrameCB ( cy_pstc_eth_type_t pstcEth, uint8_t * u8RxBuffer, uint32_t u32Length );
static void InitPHY_TJA110 (void);
static bool Check_TJA1100_LinkStatus (void);
static void LED_Control_FromLED0ToLED7(uint8_t dataToBeDisplayed);

/* Port configuration */
typedef struct
{
    volatile stc_GPIO_PRT_t* portReg;
    uint8_t                  pinNum;
    cy_stc_gpio_pin_config_t cfg;
} stc_pin_config;

stc_pin_config g_EtherPortConfigTbl[] =
{
  //{              port,              pin, outVal,                driveMode,                hsiom, intEdge, intMask, vtrip, slewRate, driveSel },
    {     ETHx_TD0_PORT,     ETHx_TD0_PIN,     0u, CY_GPIO_DM_STRONG_IN_OFF,     ETHx_TD0_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {     ETHx_TD1_PORT,     ETHx_TD1_PIN,     0u, CY_GPIO_DM_STRONG_IN_OFF,     ETHx_TD1_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {    ETHx_TXER_PORT,    ETHx_TXER_PIN,     0u, CY_GPIO_DM_STRONG_IN_OFF,    ETHx_TXER_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {  ETHx_TX_CTL_PORT,  ETHx_TX_CTL_PIN,     0u, CY_GPIO_DM_STRONG_IN_OFF,  ETHx_TX_CTL_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {     ETHx_RD0_PORT,     ETHx_RD0_PIN,     0u,         CY_GPIO_DM_HIGHZ,     ETHx_RD0_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {     ETHx_RD1_PORT,     ETHx_RD1_PIN,     0u,         CY_GPIO_DM_HIGHZ,     ETHx_RD1_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {   ETHx_RX_ER_PORT,   ETHx_RX_ER_PIN,     0u,         CY_GPIO_DM_HIGHZ,   ETHx_RX_ER_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {  ETHx_RX_CTL_PORT,  ETHx_RX_CTL_PIN,     0u,         CY_GPIO_DM_HIGHZ,  ETHx_RX_CTL_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {     ETHx_MDC_PORT,     ETHx_MDC_PIN,     0u, CY_GPIO_DM_STRONG_IN_OFF,     ETHx_MDC_PIN_MUX,      0u,      0u,    0u,       0u,       0u },
    {    ETHx_MDIO_PORT,    ETHx_MDIO_PIN,     0u,        CY_GPIO_DM_STRONG,    ETHx_MDIO_PIN_MUX,      0u,      0u,    0u,       0u,       0u },
#if EMAC_INTERFACE == EMAC_MII
    {     ETHx_TD2_PORT,     ETHx_TD2_PIN,     0u, CY_GPIO_DM_STRONG_IN_OFF,     ETHx_TD2_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {     ETHx_TD3_PORT,     ETHx_TD3_PIN,     0u, CY_GPIO_DM_STRONG_IN_OFF,     ETHx_TD3_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {  ETHx_TX_CLK_PORT,  ETHx_TX_CLK_PIN,     0u,         CY_GPIO_DM_HIGHZ,  ETHx_TX_CLK_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {     ETHx_RD2_PORT,     ETHx_RD2_PIN,     0u,         CY_GPIO_DM_HIGHZ,     ETHx_RD2_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {     ETHx_RD3_PORT,     ETHx_RD3_PIN,     0u,         CY_GPIO_DM_HIGHZ,     ETHx_RD3_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
    {  ETHx_RX_CLK_PORT,  ETHx_RX_CLK_PIN,     0u,         CY_GPIO_DM_HIGHZ,  ETHx_RX_CLK_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
#else // EMAC_RMII
    { ETHx_REF_CLK_PORT, ETHx_REF_CLK_PIN,     0u,         CY_GPIO_DM_HIGHZ, ETHx_REF_CLK_PIN_MUX,      0u,      0u,    1u,       0u,       0u },
#endif
};
#define NUM_EHTER_PORTS (sizeof(g_EtherPortConfigTbl)/sizeof(stc_pin_config))

/** Wrapper configuration   */
cy_str_ethif_wrapper_config stcWrapperConfig = 
{
  #if (EMAC_INTERFACE == EMAC_MII)
    .stcInterfaceSel = CY_ETHIF_CTL_MII_100,    /** 100 Mbps MII */ 
  #elif (EMAC_INTERFACE == EMAC_RMII)
    .stcInterfaceSel = CY_ETHIF_CTL_RMII_100,   /** 100 Mbps MII */ 
  #endif
    .bRefClockSource = CY_ETHIF_EXTERNAL_HSIO,  /** assigning Ref_Clk to HSIO Clock, it is recommended to use external clock coming from HSIO  */
    .u8RefClkDiv     = 1u,                      /** Divide Refclock by 1 to have 25 MHz Tx clock  */
};

/** Default Timer register values   */
CEDI_1588TimerVal stc1588TimerValue = 
{
    .secsUpper = 0u,
    .secsLower = 0ul,
    .nanosecs  = 0ul,
};

/** Increment values for each clock cycles      */
/** This Increment values are calculated for source clock of 24 MHz */
CEDI_TimerIncrement stc1588TimerIncValue =
{
    .nanoSecsInc = 41u,     /** Having source clock of 24 MHz, with each clock cycle Nanosecond counter shall be incremented by 41  */
    .subNsInc    = 43690ul, /** incrementing just Nanosecond counter will not give accuracy, so sub-nanosecond counter also must be incremented  */  
    .lsbSubNsInc = 170u,    /** 170 (0xAA) is the lower 8 bits increment value for subNanosecond and 43690 (0xAAAA) is the higher 16 bits of the counter */
    .altIncCount = 0u,      /** This example is not using alternate increment mode, also it is not recommended by IP provider       */
    .altNanoSInc = 0u,      /** When Alt increment mode is disabled, then this counter does not play any role          */
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
    .bintrEnable         = true,                        /** Interrupt enable  */
    .dmaDataBurstLen     = CEDI_DMA_DBUR_LEN_4, 
    .u8dmaCfgFlags       = CEDI_CFG_DMA_FRCE_TX_BRST,
#if (CY_USE_PSVP == 1)
    .mdcPclkDiv          = CEDI_MDC_DIV_BY_16,          /** For PSVP, source clock is 24 MHz and MDC must be less than 2.5MHz   */
#else
    .mdcPclkDiv          = CEDI_MDC_DIV_BY_48,          /** source clock is 80 MHz and MDC must be less than 2.5MHz   */
#endif
    .u8rxLenErrDisc      = 0u,                          /** Length error frame not discarded  */
    .u8disCopyPause      = 0u,
    .u8chkSumOffEn       = 0u,                          /** Checksum for both Tx and Rx disabled    */
    .u8rx1536ByteEn      = 1u,                          /** Enable receive frame up to 1536    */
    .u8rxJumboFrEn       = 0u,
    .u8enRxBadPreamble   = 1u,
    .u8ignoreIpgRxEr     = 0u,
    .u8storeUdpTcpOffset = 0u,
    .u8aw2wMaxPipeline   = 2u,                          /** Value must be > 0   */
    .u8ar2rMaxPipeline   = 2u,                          /** Value must be > 0   */
    .u8pfcMultiQuantum   = 0u,
    .pstcWrapperConfig   = &stcWrapperConfig,
    .pstcTSUConfig       = NULL,                        /** No TSU settings */
    .btxq0enable         = true,                        /** Tx Q0 Enabled   */
    .btxq1enable         = false,                       /** Tx Q1 Disabled  */
    .btxq2enable         = false,                       /** Tx Q2 Disabled  */
    .brxq0enable         = true,                        /** Rx Q0 Enabled   */
    .brxq1enable         = false,                       /** Rx Q1 Disabled  */
    .brxq2enable         = false,                       /** Rx Q2 Disabled  */
};
    
/** Interrupt configurations    */
cy_stc_ethif_interruptconfig_t stcInterruptConfig =
{
    .btsu_time_match        = false,          /** Time stamp unit time match event */
    .bwol_rx                = false,          /** Wake on LAN event received */
    .blpi_ch_rx             = false,          /** LPI indication status bit change received */
    .btsu_sec_inc           = true,          /** TSU seconds register increment */
    .bptp_tx_pdly_rsp       = false,          /** PTP pdelay_resp frame transmitted */
    .bptp_tx_pdly_req       = false,          /** PTP pdelay_req frame transmitted */
    .bptp_rx_pdly_rsp       = false,          /** PTP pdelay_resp frame received */
    .bptp_rx_pdly_req       = false,          /** PTP pdelay_req frame received */
    .bptp_tx_sync           = false,          /** PTP sync frame transmitted */
    .bptp_tx_dly_req        = false,          /** PTP delay_req frame transmitted */
    .bptp_rx_sync           = false,          /** PTP sync frame received */
    .bptp_rx_dly_req        = false,          /** PTP delay_req frame received */
    .bext_intr              = false,          /** External input interrupt detected */
    .bpause_frame_tx        = false,          /** Pause frame transmitted */
    .bpause_time_zero       = false,          /** Pause time reaches zero or zeroq pause frame received */
    .bpause_nz_qu_rx        = false,          /** Pause frame with non-zero quantum received */
    .bhresp_not_ok          = false,          /** DMA hresp not OK */
    .brx_overrun            = true,          /** Rx overrun error */
    .bpcs_link_change_det   = false,          /** Link status change detected by PCS */
    .btx_complete           = true,          /** Frame has been transmitted successfully */
    .btx_fr_corrupt         = true,          /** Tx frame corruption */
    .btx_retry_ex_late_coll = true,          /** Retry limit exceeded or late collision */
    .btx_underrun           = true,          /** Tx underrun */
    .btx_used_read          = true,          /** Used bit set has been read in Tx descriptor list */
    .brx_used_read          = true,          /** Used bit set has been read in Rx descriptor list */
    .brx_complete           = true,          /** Frame received successfully and stored */
    .bman_frame             = false,          /** Management Frame Sent */   

    /** call back function  */
    .rxframecb              = Ethx_RxFrameCB,
    .txerrorcb              = NULL,
    .txcompletecb           = NULL,
    .tsuSecondInccb         = NULL,
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
    .desMacAddr = { 0x04u, 0x00u, 0x00u, 0x00u, 0x00u, 0x04u, },  // Broad cast
    .srcMacAddr = { 0x02u, 0x00u, 0x00u, 0x00u, 0x00u, 0x02u, },
    .type       = 0xF022u,
    .data       =
    {
        0x00u, 0x01u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u, 0x08u, 0x09u, 0x0Au, 0x0Bu, 0x0Cu, 0x0Du, 0x0Eu, 0x0Fu,
        0x10u, 0x11u, 0x12u, 0x13u, 0x14u, 0x15u, 0x16u, 0x17u, 0x18u, 0x19u, 0x1Au, 0x1Bu, 0x1Cu, 0x1Du, 0x1Eu, 0x1Fu,
        0x20u, 0x21u, 0x22u, 0x23u, 0x24u, 0x25u, 0x26u, 0x27u, 0x28u, 0x29u, 0x2Au, 0x2Bu, 0x2Cu, 0x2Du,
    }
};

cy_stc_eth_filter_config_t fileter1Config = 
{
    .typeFilter      = CY_ETH_FILTER_TYPE_DESTINATION,
    .filterAddr.byte = {0x02u, 0x00u, 0x00u, 0x00u, 0x00u, 0x02u},
    .ignoreBytes     = 0x00u,
};

cy_stc_ether_frame_t g_rxFrameTest = { 0u };

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

    SCB_DisableDCache(); // Disables, Because DMA in ETH will also read/write SRAM
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

    NVIC_SetPriority(CPUIntIdx3_IRQn, 3ul);
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
    InitPHY_TJA110();
    while(1)
    {
        if (Check_TJA1100_LinkStatus() == true)
        {
            break;
        }
        /* Extended control register (Register 17) */
        Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x11u, 0x0002u, PHY_ADDR); // link control disable
        Cy_SysTick_DelayInUs(200ul);
        Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x11u, 0x8002u, PHY_ADDR); // link control enabled
    }

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
    uint8_t macSrcAddrToBeReply[6] = {0x04u,0x00u,0x00u,0x00u,0x00u,0x04u};

    if(memcmp(macSrcAddrToBeReply, &u8RxBuffer[6], 6u) == 0)
    {
        memcpy((uint8_t*)&g_rxFrameTest, u8RxBuffer, sizeof(cy_stc_ether_frame_t));
        bFrameReceived = true;

        Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
        LED_Control_FromLED0ToLED7(g_rxFrameTest.data[0]);
    }
}


/** Ethernet Port Pin settings  */
static void Cy_App_Init_EthernetPortpins (void)
{
    for(uint32_t i = 0ul; i < NUM_EHTER_PORTS; i++)
    {
        Cy_GPIO_Pin_Init(g_EtherPortConfigTbl[i].portReg, g_EtherPortConfigTbl[i].pinNum, &g_EtherPortConfigTbl[i].cfg);
    }
}

/*******************************************************************************
* Function Name: InitPHY_TJA110
****************************************************************************//**
*
* \brief Dedicated to initialize PHY TJA110
* Function configures the PHY with 10 Mbps link speed, full duplex communication and Auto Negotiation off
*
* \Note:
* If hardware consist of any other PHY than TJA110, dedicated function shall be written to configure relevant 
* registers in side PHY
*******************************************************************************/
static void InitPHY_TJA110 (void)
{
    while(1)
    {
        /* Reset PHY */
        Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x00u, 0x8000u, PHY_ADDR);
        while(1)
        {
            uint16_t read_data0 = Cy_EthIf_PhyRegRead(ETH_REG_BASE, 0x00u, PHY_ADDR);
            if ((read_data0 & 0x8000u) == 0u) // RESET = 0
            {
                break;
            }
        }

        uint16_t read_data21 = Cy_EthIf_PhyRegRead(ETH_REG_BASE, 0x15u, PHY_ADDR);
        if(read_data21 & 0x0800u)  // PHY_INIT_FAIL = 1
        {
            continue;
        }
        else
        {
            break;
        }
    }
    Cy_SysTick_DelayInUs(3000ul);

    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x11u, 0x0006u, PHY_ADDR); /* extended control tegister (reg17) CONFIG_EN=1*/

#if EMAC_INTERFACE == EMAC_MII
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x12u, 0x8810u, PHY_ADDR); /* Register 18 Configuration register 1 master, MII */
#else
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x12u, 0x8910u, PHY_ADDR); /* Register 18 Configuration register 1 master, RMII 50MHz */
#endif
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x16u, 0xFFFFu, PHY_ADDR); /* Enable all of interrupt */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x00u, 0x2100u, PHY_ADDR); /** 100M, Full Duplex and Auto Negotiation OFF  */

    while(1)
    {
        Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x11u, 0x1802u, PHY_ADDR); // Normal mode, config disable
        Cy_SysTick_DelayInUs(4000ul);

        uint16_t read_data17 = Cy_EthIf_PhyRegRead(ETH_REG_BASE, 0x11u, PHY_ADDR);
        if ((read_data17 & 0x1800u) == 0x1800u) // make sure it became Normal mode.
        {
            break;
        }
    }

    /* Extended control register (Register 17) */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x11u, 0x8002u, PHY_ADDR); // link control enabled
}

/*******************************************************************************
* Function Name: Check_TJA1100_LinkStatus
****************************************************************************//**
*
* \brief Function reads specific register from TJA1100 to learn Link status
*
* \param 
* \return true Link up
* \return false Link Down 
*
*******************************************************************************/
static bool Check_TJA1100_LinkStatus (void)
{
    uint16_t    u32ReadData = 0u;
    /** PHY register might take some time to update internal registers */    
    for(uint32_t i = 0ul; i < 20ul; i++)
    {
    	Cy_SysTick_DelayInUs(4000ul);
        u32ReadData = Cy_EthIf_PhyRegRead(ETH_REG_BASE, 0x17u, PHY_ADDR);       /** PHY status register: 0x0011 */
        if (u32ReadData & 0x8000u)
        {
            /** Link up */
            return true;
        }
    }

    /** Link down   */
    return false;

}
// EMAC END *******

static void LED_Control_FromLED0ToLED7(uint8_t dataToBeDisplayed)
{
    static bool IsThisFirstTime = true;

    cy_stc_gpio_pin_config_t portConfigForLED = { 0ul };
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
    ledLevel = ((dataToBeDisplayed >> 0u) & 0x01u) == 0x01u;
    Cy_GPIO_Write(CY_LED0_PORT, CY_LED0_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 1u) & 0x01u) == 0x01u;
    Cy_GPIO_Write(CY_LED1_PORT, CY_LED1_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 2u) & 0x01u) == 0x01u;
    Cy_GPIO_Write(CY_LED2_PORT, CY_LED2_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 3u) & 0x01u) == 0x01u;
    Cy_GPIO_Write(CY_LED3_PORT, CY_LED3_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 4u) & 0x01u) == 0x01u;
    Cy_GPIO_Write(CY_LED4_PORT, CY_LED4_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 5u) & 0x01u) == 0x01u;
    Cy_GPIO_Write(CY_LED5_PORT, CY_LED5_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 6u) & 0x01u) == 0x01u;
    Cy_GPIO_Write(CY_LED6_PORT, CY_LED6_PIN, ledLevel);

    ledLevel = ((dataToBeDisplayed >> 7u) & 0x01u) == 0x01u;
    Cy_GPIO_Write(CY_LED7_PORT, CY_LED7_PIN, ledLevel);
}

/* [] END OF FILE */
