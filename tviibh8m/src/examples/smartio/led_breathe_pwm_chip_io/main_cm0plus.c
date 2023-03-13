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

/******************************************************************************
* Dated        : August 28, 2018
* Example      : SmartIO
* Description  : Breathing LED using PWM and SMART IO.
* SmartIO is used to generate wire logic on the selected IO and pins with the 
*  particular input's that can be Data or any IO.
* SmartIO is having 8 LUTs (LUT0 - LUT7) that will generate the output on the
*  selected port pins. Ex: LUT1 can generate the output on SMARTIO_PRTx_1 pin.
* All the LUTs have three inputs and that can be data or selected port input.
*  In case of IO, LUT0-LUT3 can take the inputs from IO0-IO3 and LUT4-LUT7 can 
*  take the inputs from IO4-IO7. If only a single input is configured then all
*  inputs of LUT should be feed with same.
* LUT is having a map and block logical type. Map is 8 bit truth table output
*  for a particular LUT that is generated based on the three inputs. LUT logic 
*  can be combinational, gated or sequential type.
* 
* Steps to configure Smart IO:
*      This example is used to demonstrate SMART IO function with PWM. There are
*      two PWM channels which are used to generate different time period waveform
*      which are fed into the SMART IO block, LUT1 will do x-or operation on
*      them and output is available on LED1 as breathing effect.
*
*                            -------------
*      pwm2   PortX_A -----> |           |
*                            |    LUTx   |----->  PortX_C  LED1
*      pwm1   PortX_B -----> |           |
*                            -------------
*                          
*      pwm1   ___________             ___________
*            |           |___________|           |___________|       33.33ms
*            
*      pwm2   __________            __________            ______
*            |          |__________|          |__________|           30ms
*            
*      LED1            _           __         ___         ____
*            _________| |_________|  |_______|   |_______|    |___   pwm2 ^ pwm3
*            
*              
* 1. Initialize the IO port X, which are going to use with Smart IO.
*      IO port X is configured as a SmartIO port -- CY_SMARTIO_PORT
*      Port X Pin A is taken as Input -- TCPWM_LINE (PWM 1)
*      Port X Pin B is taken as Input -- TCPWM_LINE (PWM 2)
*      Port X Pin C is taken as Output -- CY_SMARTIO_OUT_PIN
*
* 2. Setup the clock for the peripheral with CY_SMARTIO_CLK and clock divider.
*      IO port X clock can be configured as -- CY_SMARTIO_CLK
*       
* 3. Initialize the Smart IO structure with selected parameter.
*      Clock selection for SmartIO port -- CY_SMARTIO_CLK_DIVACT
*      Bypass channel mask is selected as -- SMARTIO_BYPASS_CH_MASK
*              (Bypass channel mask will connect the configured IO to SmartIO
*              if respected bit is selected 0 in the mask)
*
* 4. Configure the LUTx parameter.
*      For Port X pin C configured LUT -- LUTx
*      LUTx logic implementation type -- CY_SMARTIO_LUTOPC_COMB
*      LUTx input selection -- all feed from same input
*              tr0 -- CY_SMARTIO_LUTTR_CHIP0
*              tr1 -- CY_SMARTIO_LUTTR_CHIP2
*              tr2 -- CY_SMARTIO_LUTTR_CHIP2
*      LUTx map is output for three inputs -- 0x42 (tr0 is MSB) 
*              tr0     tr1     tr2     LUTx_OUT_MAP
*              0       0       0       0
*              0       0       1       1
*              0       1       0       0
*              0       1       1       0
*              1       0       0       0
*              1       0       1       0
*              1       1       0       1
*              1       1       1       0
*
* 5. Enable the SMART IO after all configuration.
*
******************************************************************************/ 
/* App Include */
#include <stdlib.h>
#include "cy_project.h"
#include "cy_device_headers.h"


/* Smart port pin definition */
#define CY_SMARTIO_MACRO                SMARTIO
#define CY_SMARTIO_PORT                 SMARTIO_PRT8
#define CY_SMARTIO_CLK                  PCLK_SMARTIO8_CLOCK

/* Short J203 pin 2 and J200 pin 1 to connect LED2 */
#define CY_SMARTIO_OUT_PORT             GPIO_PRT8
#define CY_SMARTIO_OUT_PIN              2
#define CY_SMARTIO_OUT_PORT_MUX         P8_2_GPIO       // LD2

#define CY_SMARTIO_IN_PORT              GPIO_PRT8
#define CY_SMARTIO_IN_PIN               1
#define CY_SMARTIO_IN_PORT_MUX          P8_1_GPIO
#define CY_SMARTIO_IN_PORT_MUX_PWM      P8_1_TCPWM0_LINE532

#define CY_SMARTIO_IN2_PORT             GPIO_PRT8
#define CY_SMARTIO_IN2_PIN              3
#define CY_SMARTIO_IN2_PORT_MUX         P8_3_GPIO
#define CY_SMARTIO_IN2_PORT_MUX_PWM     P8_3_TCPWM0_LINE533

/* PWM PR Mode Configuration def */
#define CY_SMARTIO_TCPWM_GRP0_CNT       TCPWM0_GRP2_CNT20
#define CY_SMARTIO_TCPWM_PCLK           PCLK_TCPWM0_CLOCKS532

#define CY_SMARTIO_TCPWM_GRP0_CNT2      TCPWM0_GRP2_CNT21
#define CY_SMARTIO_TCPWM_PCLK2          PCLK_TCPWM0_CLOCKS532

/* Smart IO port selection macro */
/******************************************************************************/
#define SMART_IO_PORT                   CY_SMARTIO_PORT

/* Bypass channel mask */
#define SMARTIO_BYPASS_CH_MASK          (~((0x01 << CY_SMARTIO_IN_PIN) | (0x01 << CY_SMARTIO_IN2_PIN) | (0x01 << CY_SMARTIO_OUT_PIN)))

/* LUT output LED pin configuration */
#define LUTx_OUT_LED_PORT               CY_SMARTIO_OUT_PORT
#define LUTx_OUT_LED_PIN                CY_SMARTIO_OUT_PIN
#define LUTx_OUT_LED_PIN_MUX            CY_SMARTIO_OUT_PORT_MUX

/* LUT output map */
#define LUTx_OUT_MAP                    0x42

/* LUT logic circuit type macro */
#define LUTx_LOGIC_OPCODE               CY_SMARTIO_LUTOPC_COMB

/* PWM PR Mode Configuration def */
#define TCPWM1_GRP0_CNT                 CY_SMARTIO_TCPWM_GRP0_CNT
#define TCPWM1_PCLK_CLOCKS              CY_SMARTIO_TCPWM_PCLK

#define TCPWM2_GRP0_CNT                 CY_SMARTIO_TCPWM_GRP0_CNT2
#define TCPWM2_PCLK_CLOCKS              CY_SMARTIO_TCPWM_PCLK2

/* TCPWM_LINE2 and TCPWM_LINE3 macro configuration */
#define TCPWM_LINE1_PORT                CY_SMARTIO_IN_PORT
#define TCPWM_LINE1_PIN                 CY_SMARTIO_IN_PIN
#define TCPWM_LINE1_MUX                 CY_SMARTIO_IN_PORT_MUX_PWM

#define TCPWM_LINE2_PORT                CY_SMARTIO_IN2_PORT
#define TCPWM_LINE2_PIN                 CY_SMARTIO_IN2_PIN
#define TCPWM_LINE2_MUX                 CY_SMARTIO_IN2_PORT_MUX_PWM

/* Peripheral clock divider index */
#define TCPWM1_PERI_CLK_DIVIDER_INDEX   0
#define TCPWM2_PERI_CLK_DIVIDER_INDEX   2
#define SMART_IO_PERI_CLK_DIVIDER_INDEX 1

#if CY_SMARTIO_IN_PIN == 0
  #define CY_SMARTIO_LUTTR_IO_1         CY_SMARTIO_LUTTR_CHIP0
#elif CY_SMARTIO_IN_PIN == 1
  #define CY_SMARTIO_LUTTR_IO_1         CY_SMARTIO_LUTTR_CHIP1
#elif CY_SMARTIO_IN_PIN == 2
  #define CY_SMARTIO_LUTTR_IO_1         CY_SMARTIO_LUTTR_CHIP2
#elif CY_SMARTIO_IN_PIN == 3
  #define CY_SMARTIO_LUTTR_IO_1         CY_SMARTIO_LUTTR_CHIP3
#elif CY_SMARTIO_IN_PIN == 4
  #define CY_SMARTIO_LUTTR_IO_1         CY_SMARTIO_LUTTR_CHIP4
#elif CY_SMARTIO_IN_PIN == 5
  #define CY_SMARTIO_LUTTR_IO_1         CY_SMARTIO_LUTTR_CHIP5
#elif CY_SMARTIO_IN_PIN == 6
  #define CY_SMARTIO_LUTTR_IO_1         CY_SMARTIO_LUTTR_CHIP6
#else // CY_SMARTIO_IN_PIN == 7
  #define CY_SMARTIO_LUTTR_IO_1         CY_SMARTIO_LUTTR_CHIP7
#endif

#if CY_SMARTIO_IN2_PIN == 0
  #define CY_SMARTIO_LUTTR_IO_2         CY_SMARTIO_LUTTR_CHIP0
#elif CY_SMARTIO_IN2_PIN == 1
  #define CY_SMARTIO_LUTTR_IO_2         CY_SMARTIO_LUTTR_CHIP1
#elif CY_SMARTIO_IN2_PIN == 2
  #define CY_SMARTIO_LUTTR_IO_2         CY_SMARTIO_LUTTR_CHIP2
#elif CY_SMARTIO_IN2_PIN == 3
  #define CY_SMARTIO_LUTTR_IO_2         CY_SMARTIO_LUTTR_CHIP3
#elif CY_SMARTIO_IN2_PIN == 4
  #define CY_SMARTIO_LUTTR_IO_2         CY_SMARTIO_LUTTR_CHIP4
#elif CY_SMARTIO_IN2_PIN == 5
  #define CY_SMARTIO_LUTTR_IO_2         CY_SMARTIO_LUTTR_CHIP5
#elif CY_SMARTIO_IN2_PIN == 6
  #define CY_SMARTIO_LUTTR_IO_2         CY_SMARTIO_LUTTR_CHIP6
#else // CY_SMARTIO_IN2_PIN == 7
  #define CY_SMARTIO_LUTTR_IO_2         CY_SMARTIO_LUTTR_CHIP7
#endif

/* LED output configuration */
/******************************************************************************/
cy_stc_gpio_pin_config_t pin_cfg =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = LUTx_OUT_LED_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

/* PWM pin configuration */
cy_stc_gpio_pin_config_t pwm_pin_cfg =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = TCPWM_LINE1_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

/* PWM setup and configuration */
cy_stc_tcpwm_pwm_config_t const pwm1_config =
{
    .pwmMode            = CY_TCPWM_PWM_MODE_PWM,
    .clockPrescaler     = CY_TCPWM_PRESCALER_DIVBY_1,  // 2,000,000 / 1 = 2MHz
    .debug_pause        = false,
    .cc0MatchMode       = CY_TCPWM_PWM_TR_CTRL2_CLEAR,
    .overflowMode       = CY_TCPWM_PWM_TR_CTRL2_SET,
    .underflowMode      = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE,
    .cc1MatchMode       = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE,
    .deadTime           = 0u,
    .deadTimeComp       = 0u,
    .runMode            = CY_TCPWM_PWM_CONTINUOUS,
    .period             = 65535ul,                         // 2000000Hz / 65535 = 30.52Hz (PWM frequency)
    .period_buff        = 0ul,
    .enablePeriodSwap   = false,
    .compare0           = 32768ul,                         // 32768 / 65535 = 0.5 (PWM duty)
    .compare1           = 0ul,
    .enableCompare0Swap = false,
    .enableCompare1Swap = false,
    .interruptSources   = CY_TCPWM_INT_NONE,
    .invertPWMOut       = 0ul,
    .invertPWMOutN      = 0ul,
    .killMode           = CY_TCPWM_PWM_STOP_ON_KILL,
    .switchInputMode    = CY_TCPWM_INPUT_LEVEL,
    .switchInput        = 0ul,
    .reloadInputMode    = CY_TCPWM_INPUT_LEVEL,
    .reloadInput        = 0ul,
    .startInputMode     = CY_TCPWM_INPUT_LEVEL,
    .startInput         = 0ul,
    .kill0InputMode     = CY_TCPWM_INPUT_LEVEL,
    .kill0Input         = 0ul,
    .kill1InputMode     = CY_TCPWM_INPUT_LEVEL,
    .kill1Input         = 0ul,
    .countInputMode     = CY_TCPWM_INPUT_LEVEL,
    .countInput         = 1ul,
};

/* PWM setup and configuration */
cy_stc_tcpwm_pwm_config_t const pwm2_config =
{
    .pwmMode            = CY_TCPWM_PWM_MODE_PWM,
    .clockPrescaler     = CY_TCPWM_PRESCALER_DIVBY_2,  // 2,000,000 / 2 = 1MHz
    .debug_pause        = false,
    .cc0MatchMode       = CY_TCPWM_PWM_TR_CTRL2_CLEAR,
    .overflowMode       = CY_TCPWM_PWM_TR_CTRL2_SET,
    .underflowMode      = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE,
    .cc1MatchMode       = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE,
    .deadTime           = 0u,
    .deadTimeComp       = 0u,
    .runMode            = CY_TCPWM_PWM_CONTINUOUS,
    .period             = 33333ul,                         // 1000000Hz / 33333 = 30Hz (PWM frequency)
    .period_buff        = 0ul,
    .enablePeriodSwap   = false,
    .compare0           = 16666ul,                         // 33333 / 16666 = 0.5 (PWM duty)
    .compare1           = 0ul,
    .enableCompare0Swap = false,
    .enableCompare1Swap = false,
    .interruptSources   = CY_TCPWM_INT_NONE,
    .invertPWMOut       = 0ul,
    .invertPWMOutN      = 0ul,
    .killMode           = CY_TCPWM_PWM_STOP_ON_KILL,
    .switchInputMode    = CY_TCPWM_INPUT_LEVEL,
    .switchInput        = 0ul,
    .reloadInputMode    = CY_TCPWM_INPUT_LEVEL,
    .reloadInput        = 0ul,
    .startInputMode     = CY_TCPWM_INPUT_LEVEL,
    .startInput         = 0ul,
    .kill0InputMode     = CY_TCPWM_INPUT_LEVEL,
    .kill0Input         = 0ul,
    .kill1InputMode     = CY_TCPWM_INPUT_LEVEL,
    .kill1Input         = 0ul,
    .countInputMode     = CY_TCPWM_INPUT_LEVEL,
    .countInput         = 1ul,
};


/* IO port initialization */
/******************************************************************************/
void Init_IO_Pin(void)
{
    /* Please check ReadMe.txt for Input and Output connection */
    pin_cfg.hsiom = LUTx_OUT_LED_PIN_MUX;
    Cy_GPIO_Pin_Init(LUTx_OUT_LED_PORT, LUTx_OUT_LED_PIN, &pin_cfg);
}

/* TCPWM channel initialization */
/******************************************************************************/
void Init_TCPWM_Pin(void)
{
    /* Configure io to generate a PWM 1 */
    pwm_pin_cfg.hsiom = TCPWM_LINE1_MUX;
    Cy_GPIO_Pin_Init(TCPWM_LINE1_PORT, TCPWM_LINE1_PIN, &pwm_pin_cfg);

    /* Configure io to generate a PWM 2 */
    pwm_pin_cfg.hsiom = TCPWM_LINE2_MUX;
    Cy_GPIO_Pin_Init(TCPWM_LINE2_PORT, TCPWM_LINE2_PIN, &pwm_pin_cfg);

    /* Initialize TCPWM0_GRPx_CNTx_PWM_PR as PWM-PR Mode & Enable */
    Cy_Tcpwm_Pwm_Init(TCPWM1_GRP0_CNT, &pwm1_config);
    Cy_Tcpwm_Pwm_Enable(TCPWM1_GRP0_CNT);
    Cy_Tcpwm_TriggerStart(TCPWM1_GRP0_CNT);

    Cy_Tcpwm_Pwm_Init(TCPWM2_GRP0_CNT, &pwm2_config);
    Cy_Tcpwm_Pwm_Enable(TCPWM2_GRP0_CNT);
    Cy_Tcpwm_TriggerStart(TCPWM2_GRP0_CNT);
}

/* Smart IO block and LUT configuration */
/******************************************************************************/
cy_en_smartio_status_t Init_SmartIO_Cfg(void)
{
    cy_stc_smartio_lutcfg_t lutCfgLut;
    cy_stc_smartio_config_t smart_io_cfg;
    cy_en_smartio_status_t retStatus = (cy_en_smartio_status_t)0xFF;

    /* initialize the Smart IO structure */
    memset(&lutCfgLut, 0, sizeof(cy_stc_smartio_lutcfg_t));
    memset(&smart_io_cfg, 0, sizeof(cy_stc_smartio_config_t));

    /* Active clock source is selected */
    smart_io_cfg.clkSrc = (cy_en_smartio_clksrc_t)CY_SMARTIO_CLK_DIVACT;

    /* Bypass channel mask is input and output pin */
    smart_io_cfg.bypassMask = SMARTIO_BYPASS_CH_MASK;

    /* Lut configuration for output, check description above */
    lutCfgLut.opcode = LUTx_LOGIC_OPCODE;
    lutCfgLut.lutMap = LUTx_OUT_MAP;

    /* Lut configuration for output and input connection */
    lutCfgLut.tr0 = (cy_en_smartio_luttr_t)CY_SMARTIO_LUTTR_IO_1;
    lutCfgLut.tr1 = (cy_en_smartio_luttr_t)CY_SMARTIO_LUTTR_IO_2;
    lutCfgLut.tr2 = (cy_en_smartio_luttr_t)CY_SMARTIO_LUTTR_IO_2;
    smart_io_cfg.lutCfg[LUTx_OUT_LED_PIN] = &lutCfgLut;

    /* Initialization of Smart IO structure */
    retStatus = Cy_SmartIO_Init(SMART_IO_PORT, &smart_io_cfg);
    return retStatus;
}

/* Smart IO module initialization */
/******************************************************************************/
void Init_SmartIO(void)
{
    cy_en_smartio_status_t retStatus = (cy_en_smartio_status_t)0xFF;

    retStatus = Init_SmartIO_Cfg();
    if(retStatus == CY_SMARTIO_SUCCESS)
    {
        /* After all the configuration, enable SMART IO */
        Cy_SmartIO_Enable(SMART_IO_PORT);
    }
}

/* Main app function */
/******************************************************************************/
int main(void)
{
    __enable_irq();
    
    SystemInit();

    /* Enable CM7_0. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    //Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Init_IO_Pin();

    /* Deinit before Init */
    Cy_SmartIO_Deinit(SMART_IO_PORT);

    {
      #if (CY_USE_PSVP == 1)
        uint32_t periFreq = 24000000ul;
      #else
        uint32_t periFreq = 80000000ul;
      #endif
        uint32_t targetFreq = 2000000ul;
        uint32_t divNum = (periFreq / targetFreq);

        /* Assign a programmable divider  for TCPWM 1 */
        Cy_SysClk_PeriphAssignDivider(TCPWM1_PCLK_CLOCKS, CY_SYSCLK_DIV_16_BIT, TCPWM1_PERI_CLK_DIVIDER_INDEX);
        Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(TCPWM1_PCLK_CLOCKS), CY_SYSCLK_DIV_16_BIT, TCPWM1_PERI_CLK_DIVIDER_INDEX, (divNum-1ul));
        Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(TCPWM1_PCLK_CLOCKS), CY_SYSCLK_DIV_16_BIT, TCPWM1_PERI_CLK_DIVIDER_INDEX);

        /* Assign a programmable divider  for TCPWM 2 */
        Cy_SysClk_PeriphAssignDivider(TCPWM2_PCLK_CLOCKS, CY_SYSCLK_DIV_16_BIT, TCPWM2_PERI_CLK_DIVIDER_INDEX);
        Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(TCPWM2_PCLK_CLOCKS), CY_SYSCLK_DIV_16_BIT, TCPWM2_PERI_CLK_DIVIDER_INDEX, (divNum-1ul));
        Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(TCPWM2_PCLK_CLOCKS), CY_SYSCLK_DIV_16_BIT, TCPWM2_PERI_CLK_DIVIDER_INDEX);
        
        /* SmartIO peripheral clock divider setting */
        Cy_SysClk_PeriphAssignDivider(CY_SMARTIO_CLK, CY_SYSCLK_DIV_16_BIT, SMART_IO_PERI_CLK_DIVIDER_INDEX);
        Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(CY_SMARTIO_CLK), CY_SYSCLK_DIV_16_BIT, SMART_IO_PERI_CLK_DIVIDER_INDEX, (divNum-1ul));
        Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(CY_SMARTIO_CLK), CY_SYSCLK_DIV_16_BIT, SMART_IO_PERI_CLK_DIVIDER_INDEX);
    }

    /* Initialization call for the TCPWM */
    Init_TCPWM_Pin();

    /* Now PWM is being output. But will stop after Smart IO initialization done. */
    /* Initialization call for the Smart IO */
    Init_SmartIO();

    for(;;);
}

/******************************************************************************/

/* [] END OF FILE */

/******************************************************************************/

