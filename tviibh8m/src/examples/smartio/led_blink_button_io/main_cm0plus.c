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
* Dated        : August 27, 2018
* Example      : SmartIO
* Description  :
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
*      This example is used to demonstrate SMART IO function with IO. On pressing
*      BUTTON1, LED1 will toggle (can be configured by truth table). 
*                      -------------
*         PortX_A -----> |           |
*         PortX_A -----> |   LUTx    |----->  PortX_B
*         PortX_A -----> |           |
*                      -------------
*              
* 1. Initialize the IO port, which are going to use with Smart IO.
*      IO port X is configured as a SmartIO port -- CY_SMARTIO_PORT
*      Port X Pin A is taken as Input -- CY_SMARTIO_IN_PIN
*      Port X Pin B is taken as Output -- CY_SMARTIO_OUT_PIN
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
*      For Port X pin B configured LUT -- LUTx
*      LUTx logic implementation type -- CY_SMARTIO_LUTOPC_COMB
*      LUTx input selection -- all feed from same input
*              tr0 -- CY_SMARTIO_LUTTR_IO0
*              tr1 -- CY_SMARTIO_LUTTR_IO0
*              tr2 -- CY_SMARTIO_LUTTR_IO0
*      LUTx map is output for three inputs -- 0x80 (tr0 is MSB) 
*              tr0     tr1     tr2     LUTx_OUT_MAP
*              0       0       0       0
*              0       0       1       0
*              0       1       0       0
*              0       1       1       0
*              1       0       0       0
*              1       0       1       0
*              1       1       0       0
*              1       1       1       1
*
* 5. Enable the SMART IO after all configuration done.
*
******************************************************************************/ 

/* App Include */
#include <stdlib.h>
#include "cy_project.h"
#include "cy_device_headers.h"

/* Smart IO clock source selection */
#define SMART_IO_CLK_ACTIVE             1

/* Smart IO port selections macro */
#define SMART_IO_PORT                   CY_SMARTIO_PORT

/* Bypass channel mask */
#define SMARTIO_BYPASS_CH_MASK          (~((0x01 << CY_SMARTIO_IN_PIN) | (0x01 << CY_SMARTIO_OUT_PIN)))

/* IO sync channel mask */
#define SMARTIO_IOSYNC_CH_MASK          ((0x01 << CY_SMARTIO_IN_PIN) | (0x01 << CY_SMARTIO_OUT_PIN))

/* Lut input button pin configuration */
#define LUT_IP_BUTTON_PORT              CY_SMARTIO_IN_PORT
#define LUT_IP_BUTTON_PIN               CY_SMARTIO_IN_PIN
#define LUT_IP_BUTTON_PIN_MUX           CY_SMARTIO_IN_PORT_MUX

#if CY_SMARTIO_IN_PIN == 0
  #define CY_SMARTIO_LUTTR_IO           CY_SMARTIO_LUTTR_IO0
#elif CY_SMARTIO_IN_PIN == 1
  #define CY_SMARTIO_LUTTR_IO           CY_SMARTIO_LUTTR_IO1
#elif CY_SMARTIO_IN_PIN == 2
  #define CY_SMARTIO_LUTTR_IO           CY_SMARTIO_LUTTR_IO2
#elif CY_SMARTIO_IN_PIN == 3
  #define CY_SMARTIO_LUTTR_IO           CY_SMARTIO_LUTTR_IO3
#elif CY_SMARTIO_IN_PIN == 4
  #define CY_SMARTIO_LUTTR_IO           CY_SMARTIO_LUTTR_IO4
#elif CY_SMARTIO_IN_PIN == 5
  #define CY_SMARTIO_LUTTR_IO           CY_SMARTIO_LUTTR_IO5
#elif CY_SMARTIO_IN_PIN == 6
  #define CY_SMARTIO_LUTTR_IO           CY_SMARTIO_LUTTR_IO6
#else // CY_SMARTIO_IN_PIN == 7
  #define CY_SMARTIO_LUTTR_IO           CY_SMARTIO_LUTTR_IO7
#endif
   
/* LUT output LED pin configuration */
#define LUT_OUT_LED_PORT                CY_SMARTIO_OUT_PORT
#define LUT_OUT_LED_PIN                 CY_SMARTIO_OUT_PIN
#define LUT_OUT_LED_PIN_MUX             CY_SMARTIO_OUT_PORT_MUX

/* LUT output map */
#define LUTx_OUT_MAP                    0x80

/* LUT logic circuit type macro */
#define LUTx_LOGIC_OPCODE               CY_SMARTIO_LUTOPC_COMB


/* LED output configuration */
/******************************************************************************/
cy_stc_gpio_pin_config_t led_cfg = 
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = LUT_OUT_LED_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

/* Button input configuration */
cy_stc_gpio_pin_config_t button_cfg =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom     = LUT_IP_BUTTON_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};


/* IO port initialization */
/******************************************************************************/
void Init_IO_Pin(void)
{
    /* Please check ReadMe.txt for proper connection of Input and Output */
    Cy_GPIO_Pin_Init(LUT_OUT_LED_PORT, LUT_OUT_LED_PIN, &led_cfg);
    Cy_GPIO_Pin_Init(LUT_IP_BUTTON_PORT, LUT_IP_BUTTON_PIN, &button_cfg);
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

#ifdef SMART_IO_CLK_ACTIVE
    /* Active clock source is selected */
    smart_io_cfg.clkSrc = (cy_en_smartio_clksrc_t)CY_SMARTIO_CLK_DIVACT;
#else
    /* Asynchronous clock source is selected */
    smart_io_cfg.clkSrc = (cy_en_smartio_clksrc_t)CY_SMARTIO_CLK_ASYNC;
#endif  // SMART_IO_CLK_ACTIVE

    /* Bypass channel mask for input and output pin */
    smart_io_cfg.bypassMask = SMARTIO_BYPASS_CH_MASK;

    /* IO channel sync mask for selected pin */
    smart_io_cfg.ioSyncEn = SMARTIO_IOSYNC_CH_MASK;

    /* Lut configuration for output, check description above */
    lutCfgLut.opcode = LUTx_LOGIC_OPCODE;
    lutCfgLut.lutMap = LUTx_OUT_MAP;

    /* Lut configuration for input */
    lutCfgLut.tr0 = (cy_en_smartio_luttr_t)CY_SMARTIO_LUTTR_IO;
    lutCfgLut.tr1 = (cy_en_smartio_luttr_t)CY_SMARTIO_LUTTR_IO;
    lutCfgLut.tr2 = (cy_en_smartio_luttr_t)CY_SMARTIO_LUTTR_IO;
    smart_io_cfg.lutCfg[LUT_OUT_LED_PIN] = &lutCfgLut;

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
    SystemInit();

    __enable_irq();

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Init_IO_Pin();

    /* Deinit before Init */
    Cy_SmartIO_Deinit(SMART_IO_PORT);

    /* SmartIO peripheral clock divider setting */
    {
        Cy_SysClk_PeriphAssignDivider(CY_SMARTIO_CLK, CY_SYSCLK_DIV_16_BIT, 0ul);
      #if (CY_USE_PSVP == 1)
        uint32_t sourceFreq = 24000000ul;
      #else
        uint32_t sourceFreq = 80000000ul;
      #endif
        uint32_t targetFreq = 12000000ul;
        uint32_t divNum = (sourceFreq / targetFreq);

        Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(CY_SMARTIO_CLK), CY_SYSCLK_DIV_16_BIT, 0u, (divNum - 1ul));
        Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(CY_SMARTIO_CLK), CY_SYSCLK_DIV_16_BIT, 0u);
    }

    /* Initialization call for the Smart IO */
    Init_SmartIO();

    for(;;);
}

/******************************************************************************/

/* [] END OF FILE */

/******************************************************************************/

