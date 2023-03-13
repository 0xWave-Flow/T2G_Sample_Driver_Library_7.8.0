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
#define EMAC_RMII               1
#define EMAC_GMII               2
#define EMAC_RGMII              3

#define ETH_LINKSPEED_10        10
#define ETH_LINKSPEED_100       100
#define ETH_LINKSPEED_1000      1000

/********************************************************/
/** PHY Mode Selection      */
#define EMAC_INTERFACE              EMAC_RGMII
#define EMAC_LINKSPEED              ETH_LINKSPEED_1000

/********************************************************/

#define USER_LED_PORT           GPIO_PRT23
#define USER_LED_PIN            2
#define USER_LED_PIN_MUX        P23_2_GPIO

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

#define ETH_FRAME_SIZE              1500

/********************************************************/

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

// The different interface modes are specified with different input voltage threshold levels 
#if (EMAC_INTERFACE == EMAC_MII) || (EMAC_INTERFACE == EMAC_RMII)
    #define ETH_VTRIP_SEL     1
#elif (EMAC_INTERFACE == EMAC_RGMII)
    #define ETH_VTRIP_SEL     0
#elif (EMAC_INTERFACE == EMAC_GMII)
    #define ETH_VTRIP_SEL     3
#endif

/********************************************************/
// EMAC *********
/**                      PortPinName.outVal||  driveMode               hsiom             ||intEdge||intMask||vtrip||slewRate||driveSel||vregEn||ibufMode||    vtripSel        ||vrefSel||vohSel*/
cy_stc_gpio_pin_config_t ethx_tx0   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TD0_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_tx1   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TD1_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_tx2   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TD2_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_tx3   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TD3_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
                                                                                                                                
#if EMAC_INTERFACE == EMAC_GMII                                                                                                 
cy_stc_gpio_pin_config_t ethx_tx4   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TD4_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_tx5   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TD5_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_tx6   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TD6_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_tx7   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TD7_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
#endif                                                                       

cy_stc_gpio_pin_config_t ethx_txer  = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TXER_PIN_MUX,    0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_txctl = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TX_CTL_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
                                                                                        
cy_stc_gpio_pin_config_t ethx_rx0   = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RD0_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_rx1   = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RD1_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_rx2   = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RD2_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_rx3   = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RD3_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};

#if EMAC_INTERFACE == EMAC_GMII
cy_stc_gpio_pin_config_t ethx_rx4   = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RD4_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_rx5   = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RD5_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_rx6   = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RD6_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_rx7   = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RD7_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
#endif                                                                          

cy_stc_gpio_pin_config_t ethx_rxctl = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RX_CTL_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_rxer  = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_RX_ER_PIN_MUX,   0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
                                                                       
#if EMAC_INTERFACE == EMAC_MII
cy_stc_gpio_pin_config_t ethx_txclk = {0x00, CY_GPIO_DM_HIGHZ,         ETHx_TX_CLK_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
#else
cy_stc_gpio_pin_config_t ethx_txclk = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_TX_CLK_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
#endif

cy_stc_gpio_pin_config_t ethx_rxclk  = {0x00, CY_GPIO_DM_HIGHZ,        ETHx_RX_CLK_PIN_MUX,  0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};
cy_stc_gpio_pin_config_t ethx_refclk = {0x00, CY_GPIO_DM_HIGHZ,        ETHx_REF_CLK_PIN_MUX, 0,       0,       0,     0,        0,        0,      0,        ETH_VTRIP_SEL,        0,       0};

cy_stc_gpio_pin_config_t ethx_mdc   = {0x00, CY_GPIO_DM_STRONG_IN_OFF, ETHx_MDC_PIN_MUX,     0,       0,       0,     0,        0,        0,      0,        1,                    0,       0};
cy_stc_gpio_pin_config_t ethx_mdio  = {0x00, CY_GPIO_DM_STRONG,        ETHx_MDIO_PIN_MUX,    0,       0,       0,     0,        0,        0,      0,        1,                    0,       0};

/********************************************************/

/** Transmit source buffer  */
uint8_t u8DummyTxBuf[1536] __ALIGNED(8) = {0};

bool bFrameReceived = false;
bool bFrameTransmit = false;
uint16_t u16TransmitLength = 0;

/********************************************************/

static void Cy_App_Init_EthernetPortpins(void);
static void Ethx_RxFrameCB ( cy_pstc_eth_type_t pstcEth, uint8_t * u8RxBuffer, uint32_t u32Length );
static void Ethx_TSUIncrement ( cy_pstc_eth_type_t pstcEth );
static void Ethx_TxFrameSuccessful ( cy_pstc_eth_type_t pstcEth, uint8_t u8QueueIndex, uint32_t bufIdx);

static void InitPHY_DP83867IR(void);
static bool Check_DP83867IR_LinkStatus (void);
static bool Phy_DP83867IR_MDIO_validation (void);

//EMAC End ***********

/********************************************************/

int main(void)
{
    uint32_t cy_Hfclk4FreqHz   = 0UL;
    uint32_t cy_Hfclk5FreqHz   = 0UL;
    uint32_t cy_PeriFreqHz     = 0UL;
    uint8_t u8CLK_GR4Divider   = 0UL;
    uint32_t u32ReadData       = 0UL;
    
    SystemInit();

    // Example had been originally tested with "cache off", so ensure that caches are turned off (may have been enabled by new startup.c module)
    SCB_DisableICache(); // Disables and invalidates instruction cache
    SCB_DisableDCache(); // Disables, cleans and invalidates data cache
    
    __enable_irq();

    /** Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);    
    
    /********************************************************/
    /***************** EMAC Configuration *******************/
    bool bClearAll = true;
    bool bTransmitBuf = true;
    bool bLastBuffer = true;
    
    /** Clocks check for Debug only */
    Cy_SysClk_GetHfClkFrequency(CY_SYSCLK_HFCLK_5, &cy_Hfclk5FreqHz);       /** Clock for TSU */    
    Cy_SysClk_GetHfClkFrequency(CY_SYSCLK_HFCLK_4, &cy_Hfclk4FreqHz);       /** Internal reference Clock for RGMII/RMII mode */      
    // Cy_SysClk_SetClkGrDiv(4, 1);                                           /** Divider set for CLK_GR4 */
    u8CLK_GR4Divider = Cy_SysClk_GetClkGrDiv(4);                            /** GR4Divider  */
    Cy_SysClk_GetClkPeriFrequency(&cy_PeriFreqHz);                          /** Clock for MDC: Peri -> GR4Divider -> Ethernet */
    
    /** Configure Ethernet Port pins    */
    Cy_App_Init_EthernetPortpins();      
    
    /** Wrapper configuration   */
    cy_str_ethif_wrapper_config stcWrapperConfig = {
        #if ((EMAC_INTERFACE == EMAC_MII) && (EMAC_LINKSPEED == ETH_LINKSPEED_10))
        .stcInterfaceSel = CY_ETHIF_CTL_MII_10,       /** 10 Mbps MII */ 
        #elif ((EMAC_INTERFACE == EMAC_MII) && (EMAC_LINKSPEED == ETH_LINKSPEED_100))
        .stcInterfaceSel = CY_ETHIF_CTL_MII_100,      /** 100 Mbps MII */ 
        #elif ((EMAC_INTERFACE == EMAC_GMII) && (EMAC_LINKSPEED == ETH_LINKSPEED_1000))
        .stcInterfaceSel = CY_ETHIF_CTL_GMII_1000,    /** 1000 Mbps GMII */ 
        #elif ((EMAC_INTERFACE == EMAC_RGMII) && (EMAC_LINKSPEED == ETH_LINKSPEED_10))
        .stcInterfaceSel = CY_ETHIF_CTL_RGMII_10,     /** 10 Mbps RGMII */ 
        #elif ((EMAC_INTERFACE == EMAC_RGMII) && (EMAC_LINKSPEED == ETH_LINKSPEED_100))
        .stcInterfaceSel = CY_ETHIF_CTL_RGMII_100,    /** 100 Mbps RGMII */ 
        #elif ((EMAC_INTERFACE == EMAC_RGMII) && (EMAC_LINKSPEED == ETH_LINKSPEED_1000))
        .stcInterfaceSel = CY_ETHIF_CTL_RGMII_1000,    /** 1000 Mbps RGMII */ 
        #elif ((EMAC_INTERFACE == EMAC_RMII) && (EMAC_LINKSPEED == ETH_LINKSPEED_10))
        .stcInterfaceSel = CY_ETHIF_CTL_RMII_10,      /** 10 Mbps RMII */ 
        #elif ((EMAC_INTERFACE == EMAC_RMII) && (EMAC_LINKSPEED == ETH_LINKSPEED_100))
        .stcInterfaceSel = CY_ETHIF_CTL_RMII_100,     /** 100 Mbps RMII */     
        #else
        .stcInterfaceSel = 8,                         /** Error in configuration */     
        #endif
        
        .bRefClockSource = CY_ETHIF_EXTERNAL_HSIO,    /** assigning Ref_Clk to HSIO Clock, it is recommended to use external clock coming from HSIO  */
        
        #if EMAC_LINKSPEED == ETH_LINKSPEED_10
        .u8RefClkDiv = 10,                         /** RefClk: 25MHz, Divide Refclock by 10 to have 2.5 MHz tx clock  */
        #elif EMAC_LINKSPEED == ETH_LINKSPEED_100
        .u8RefClkDiv = 1,                          /** RefClk: 25MHz, Divide Refclock by 1 to have 25 MHz Tx clock  */
        #elif EMAC_LINKSPEED == ETH_LINKSPEED_1000
        .u8RefClkDiv = 1,                          /** RefClk: 125MHz, Divide Refclock by 1 to have 125 MHz Tx clock || Although only relevant in RGMII/GMII modes */        
        #endif
    };
    
    /** Default Timer register values   */
    CEDI_1588TimerVal stc1588TimerValue = {
        .secsUpper = 0,
        .secsLower = 0,
        .nanosecs = 0,
    };
    
    /** Increment values for each clock cycles      */
    CEDI_TimerIncrement stc1588TimerIncValue = { /** This Increment values are calculated for source clock of 196.607999 MHz */
        .nanoSecsInc = 5,                        /** Having source clock of 196.607999 MHz, with each clock cycle Nanosecond counter shall be incremented by 5  */
        .subNsInc = 0x1615,                      /** incrementing just Nanosecond counter will not give accuracy, so sub-nanosecond counter also must be incremented  */  
        .lsbSubNsInc = 0x55,                     /** 0x55 is the lower 8 bits increment value for subNanosecond and 0x1615 is the higher 16 bits of the counter */
        .altIncCount = 0,                        /** This example is not using alternate increment mode, also it is not recommended by IP provider          */
        .altNanoSInc = 0,                        /** When Alt increment mode is disabled, then this counter does not play any role                          */
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
                    .mdcPclkDiv          = CEDI_MDC_DIV_BY_48,          /** source clock is 80 MHz and MDC must be less than 2.5MHz   */
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
                    .pstcTSUConfig       = &stcTSUConfig,               /** TSU settings    */
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
                    .btsu_sec_inc           = 0,          /** TSU seconds register increment */
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
                    
                    /** call back functions  */
                    .rxframecb  = Ethx_RxFrameCB,
                    .txerrorcb  = NULL,
                    .txcompletecb = Ethx_TxFrameSuccessful,     /** Set it to NULL, if do not wish to have callback */
                    .tsuSecondInccb = Ethx_TSUIncrement,
    };

    /** Enable Ethernet Interrupts  */
    const cy_stc_sysint_irq_t irq_cfg_ethx_q0 = {.sysIntSrc  = ETH_INTR_SRC,    .intIdx  = CPUIntIdx3_IRQn, .isEnabled  = true};
    const cy_stc_sysint_irq_t irq_cfg_ethx_q1 = {.sysIntSrc  = ETH_INTR_SRC_Q1, .intIdx  = CPUIntIdx3_IRQn, .isEnabled  = true};
    const cy_stc_sysint_irq_t irq_cfg_ethx_q2 = {.sysIntSrc  = ETH_INTR_SRC_Q2, .intIdx  = CPUIntIdx3_IRQn, .isEnabled  = true};
    
    Cy_SysInt_InitIRQ(&irq_cfg_ethx_q0);
    Cy_SysInt_SetSystemIrqVector(irq_cfg_ethx_q0.sysIntSrc, Cy_Ethx_InterruptHandler);

    Cy_SysInt_InitIRQ(&irq_cfg_ethx_q1);
    Cy_SysInt_SetSystemIrqVector(irq_cfg_ethx_q1.sysIntSrc, Cy_Ethx_InterruptHandler);

    Cy_SysInt_InitIRQ(&irq_cfg_ethx_q2);
    Cy_SysInt_SetSystemIrqVector(irq_cfg_ethx_q2.sysIntSrc, Cy_Ethx_InterruptHandler);
    
    NVIC_SetPriority(CPUIntIdx3_IRQn, 3);
    NVIC_ClearPendingIRQ(CPUIntIdx3_IRQn);
    NVIC_EnableIRQ(CPUIntIdx3_IRQn);
    
    /** Initialize ENET MAC */
    if (CY_ETHIF_SUCCESS != Cy_EthIf_Init(ETH_REG_BASE, &stcENETConfig, &stcInterruptConfig))
    {
        // Error        
    }    
    
    /** PHY initialization  */
    InitPHY_DP83867IR();    
    while (true != Check_DP83867IR_LinkStatus());    

    /** Load Destination MAC address    */
    u8DummyTxBuf[0] =  0xFF;
	u8DummyTxBuf[1] =  0xFF;
	u8DummyTxBuf[2] =  0xFF;
	u8DummyTxBuf[3] =  0xFF;
	u8DummyTxBuf[4] =  0xFF;
	u8DummyTxBuf[5] =  0xFF;
    
    /** Load source MAC address  */
	u8DummyTxBuf[6] =  0x02;
	u8DummyTxBuf[7] =  0x00;
	u8DummyTxBuf[8] =  0x00;
	u8DummyTxBuf[9] =  0x00;
	u8DummyTxBuf[10] = 0x00;
	u8DummyTxBuf[11] = 0x02;
    
    /** Load Ethertype  */
	u8DummyTxBuf[12] = 0x00;
	u8DummyTxBuf[13] = 0x00;
    
    /** Load Dummy payload  */
    for (uint16_t i = 0; i < 1500; i++)	
		u8DummyTxBuf[i + 14] = (uint8_t)i;

    if (CY_ETHIF_SUCCESS != Cy_EthIf_TransmitFrame(ETH_REG_BASE, u8DummyTxBuf, ETH_FRAME_SIZE, CY_ETH_QS0_0, bLastBuffer))
    {
        // Error while transmitting        
    }
    else
    {
        // Frame started transmitting        
    }
    
    /** Free Release Buffer, this routine shall be called at regular interval of time to free "Released" buffers */
    /** Release all Transmit buffer */
    Cy_EthIF_ClearReleasedBuf(bClearAll, bTransmitBuf);
    
    /** Configures Ethernet Pause frames  | Not checked with respect to quantum */
    // Cy_EthIf_ConfigPause(ETH_REG_BASE, 1000);
    // Cy_EthIf_TxPauseFrame(ETH_REG_BASE, 0);
    
// EMAC End**********

    for(;;)
    {
        // Cy_SysTick_DelayInUs(50000);       
        // Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);        
        
//        if (bFrameReceived || bFrameTransmit)
        if (bFrameReceived)
        {
            bFrameReceived = false;
            bFrameTransmit = false;
            
            #if 0
                /** Debug routine   */
                u32ReadData = Cy_EthIf_PhyRegRead(ETH_REG_BASE, (uint8_t)0x15, PHY_ADDR);       /** Read the RGMII mode status  */            
                
                Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x001F, PHY_ADDR);                     /** REGCR  */
                Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x0135, PHY_ADDR);                     /** ADDAR, 0x0086 Delay config register  */
                Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x401F, PHY_ADDR);                     /** REGCR, will force next write/read access non incremental  */
                u32ReadData = Cy_EthIf_PhyRegRead(ETH_REG_BASE, (uint8_t)0x0E, PHY_ADDR);       /** Read the RGMII mode status  */    
                /** Debug Routine ends  */
            #endif
                        
            /** Transmit Received Frame  */
            if (CY_ETHIF_SUCCESS != Cy_EthIf_TransmitFrame(ETH_REG_BASE, u8DummyTxBuf, u16TransmitLength, CY_ETH_QS0_0, bLastBuffer))
            {
                // Error while transmitting
            }
            
            /** Clear all released buffer for both Transmit and Receive    */
            Cy_EthIF_ClearReleasedBuf(bClearAll, bTransmitBuf);  
            bTransmitBuf = false;
            Cy_EthIF_ClearReleasedBuf(bClearAll, bTransmitBuf);  
            bTransmitBuf = true;  
        }             
    }
}

// EMAC *****************
static void Ethx_RxFrameCB ( cy_pstc_eth_type_t pstcEth, uint8_t * u8RxBuffer, uint32_t u32Length )
{
    bFrameReceived = true;
    
    /** Copy frame to transmit buffer */
    /** Starting from 14, as actual data starts from there. */
    /** Copy function also includes last 4 bytes of CDC     */
    for (uint32_t cntr = 14; cntr < (u32Length - 4); cntr++)
    {
        u8DummyTxBuf[cntr] = u8RxBuffer[cntr];
    }
    
    u16TransmitLength = (uint16_t) (u32Length - 4);     /** Echo frame will have last 4 bytes of CRC from EMAC CRC generator */
    
    u16TransmitLength = u16TransmitLength + 64;
    if (u16TransmitLength > 1500)
    {
        u16TransmitLength = 64;
    }
    
    /** Receive buffer should be release to be reused again; done in main while loop along with transmit buffer  */
    
    /** Toggle LED  */
    Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);        
}

static void Ethx_TxFrameSuccessful ( cy_pstc_eth_type_t pstcEth, uint8_t u8QueueIndex, uint32_t bufIdx)
{
    bFrameTransmit = true;
    u16TransmitLength = u16TransmitLength + 64;
    if (u16TransmitLength > 1500)
    {
        u16TransmitLength = 64;
    }
}

/** TSU One Second Increment Interrupt call back function   */
static void Ethx_TSUIncrement ( cy_pstc_eth_type_t pstcEth )
{
    Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
}

/*******************************************************************************
* Function Name: Cy_App_Init_EthernetPortpins
****************************************************************************//**
*
* \brief Initializes Ethernet Port Pins. 
* 
* \Note:
*******************************************************************************/
static void Cy_App_Init_EthernetPortpins (void)
{    
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

    Cy_GPIO_Pin_Init(ETHx_TXER_PORT, ETHx_TXER_PIN, &ethx_txer);                    /** TX_ER   */
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
    uint32_t    u32ReadData = 0;
    
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x1F, 0x8000, PHY_ADDR);         /** Reg CTRl: Perform a full reset, including All registers   */ 
    
#if 0

    /** Test Routines   */
    if (false == Phy_DP83867IR_MDIO_validation ())
    {
        /** PHY is not out of reset sequence    */
    }
    
#endif
      
#if EMAC_LINKSPEED == ETH_LINKSPEED_10
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x00, 0x0100, PHY_ADDR);         /** 10M, Full Duplex and Auto Negotiation OFF  */
    
#elif EMAC_LINKSPEED == ETH_LINKSPEED_100
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x00, 0x2100, PHY_ADDR);         /** 100M, Full Duplex and Auto Negotiation OFF  */
//    Cy_EthIf_PhyRegWrite(CY_ETH1_TYPE, 0x10, 0x5028, PHY_ADDR);         /** Disable auto neg for MDI/MDI-X YOTS in my case this is required **/
    // Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x00, 0x1100, PHY_ADDR);         /** 100M, Full Duplex and Auto Negotiation ON  */
    
#elif EMAC_LINKSPEED == ETH_LINKSPEED_1000
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x00, 0x1140, PHY_ADDR);         /** 1000M, Full Duplex and Auto Negotiation ON  */
    
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x001F, PHY_ADDR);         /** Begin write access to Extended register     */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x0170, PHY_ADDR);
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x401F, PHY_ADDR);
    u32ReadData = Cy_EthIf_PhyRegRead(ETH_REG_BASE, (uint8_t)0x0E, PHY_ADDR);
    u32ReadData = u32ReadData & 0x0000;                                 /** changing IO impedance on the PHY    */
    u32ReadData = u32ReadData | 0x010C;
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, u32ReadData, PHY_ADDR);         /** Enable clock from PHY -> Route it to MCU    */
    u32ReadData = Cy_EthIf_PhyRegRead(ETH_REG_BASE, (uint8_t)0x0E, PHY_ADDR);
#endif
        
    /** Disable RGMII by accessing extended register set || Please read data sheet section 8.4.2.1 for procedure in detail */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x001F, PHY_ADDR);                     /** REGCR  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x0032, PHY_ADDR);                     /** ADDAR, 0x0032 RGMII config register  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x401F, PHY_ADDR);                     /** REGCR, will force next write/read access non incremental  */
#if EMAC_INTERFACE != EMAC_RGMII    
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x0000, PHY_ADDR);                     /** Disable RGMII  */    
    u32ReadData = Cy_EthIf_PhyRegRead(ETH_REG_BASE, (uint8_t)0x0E, PHY_ADDR);       /** Read the RGMII mode status  */
#else
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x00D3, PHY_ADDR);                     /** Enable Tx and RX Clock delay in RGMII configuration register  */ 
    
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x001F, PHY_ADDR);                     /** REGCR  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x0086, PHY_ADDR);                     /** ADDAR, 0x0086 Delay config register  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x401F, PHY_ADDR);                     /** REGCR, will force next write/read access non incremental  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x0066, PHY_ADDR);                     /** Adjust Tx and Rn Clock delays in PHY  */    
    
#endif
        
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x1F, 0x4000, PHY_ADDR);         /** CTRL   */      
    u32ReadData = Cy_EthIf_PhyRegRead(ETH_REG_BASE, (uint8_t)0x11, PHY_ADDR);   
    
#if 0
    /** Enable Loop Back Functions; Tested only for RGMII  */
    // Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x10, 0x4000, PHY_ADDR);          /** Reset; PHYCR   */    
    
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x001F, PHY_ADDR);             /** REGCR  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x00FE, PHY_ADDR);             /** ADDAR, 0x00FE Loopback Configuration Register  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x401F, PHY_ADDR);             /** REGCR, will force next write/read access non incremental  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0xE720, PHY_ADDR);             /** Enable Loopback configurations  */    
    
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x00, 0x4140, PHY_ADDR);             /** 1000M, Full Duplex and Auto Negotiation OFF, Loopback enabled   */
    
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x001F, PHY_ADDR);             /** Begin write access to Extended register     */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x0170, PHY_ADDR);
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x401F, PHY_ADDR);
    u32ReadData = Cy_EthIf_PhyRegRead(ETH_REG_BASE, (uint8_t)0x0E, PHY_ADDR);
    u32ReadData = u32ReadData & 0x0000;                                     /** changing IO impedance on the PHY    */
    u32ReadData = u32ReadData | 0x0101;
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, u32ReadData, PHY_ADDR);        /** Enable clock from PHY -> Route it to MCU    */
    
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x16, 0xD001, PHY_ADDR);             /** BISCR   */  
    
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x001F, PHY_ADDR);             /** REGCR  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x0086, PHY_ADDR);             /** ADDAR, 0x0086 Delay config register  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x401F, PHY_ADDR);             /** REGCR, will force next write/read access non incremental  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x0066, PHY_ADDR);             /** Control Clock delay on Tx and Rx Line  */    
    
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x001F, PHY_ADDR);             /** REGCR  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x0032, PHY_ADDR);             /** ADDAR, 0x0032 RGMII config register  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0D, 0x401F, PHY_ADDR);             /** REGCR, will force next write/read access non incremental  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x0E, 0x00D3, PHY_ADDR);             /** Enable Tx and RX Clock delay in RGMII configuration register  */    
    
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x1F, 0x4000, PHY_ADDR);             /** CTRL   */   
        
    Cy_EthIf_WaitUsec(5000);                                                /** Some more delay to get PHY adapted to new interface	*/    

    /** Please note: when Loop-back is enabled. Do not check for Link status in the main function.   */
    
#endif
    
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

/*******************************************************************************
* Function Name: Phy_DP83867IR_MDIO_validation
****************************************************************************//**
*
* \brief Function checks MDIO interface and functioning of PHY
*
* \param 
* \return true Test Passed
* \return false Tests fail 
*
*******************************************************************************/
static bool Phy_DP83867IR_MDIO_validation (void)
{
    uint32_t u32ReadData = 0;
    
    /** LED Testing; please check on the RJ 45 connector */  
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x19, 0x0777, PHY_ADDR);         /** LED ON  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x19, 0x0555, PHY_ADDR);         /** LED OFF */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x19, 0x0777, PHY_ADDR);         /** LED ON  */  
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x19, 0x0555, PHY_ADDR);         /** LED OFF */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x19, 0x4444, PHY_ADDR);         /** Restoring default LED configuration, show status of Link and Speed  */
    Cy_EthIf_PhyRegWrite(ETH_REG_BASE, 0x18, 0xD032, PHY_ADDR);
    
    /** Dummy Read Operation for DP83867IR PHY   */
    u32ReadData = Cy_EthIf_PhyRegRead(ETH_REG_BASE, (uint8_t)0x02, PHY_ADDR);       /** PHY Identifier Register #1  */
    if (u32ReadData != 0x2000)
    {
        /** Error   */
        return false;
    }
        
    u32ReadData = Cy_EthIf_PhyRegRead(ETH_REG_BASE, (uint8_t)0x03, PHY_ADDR);       /** PHY Identifier Register #2  */
    if (u32ReadData != 0xA231)
    {
        /** Error   */
        return false;
    }
    
    return true;
}

// EMAC END *******

/* [] END OF FILE */
