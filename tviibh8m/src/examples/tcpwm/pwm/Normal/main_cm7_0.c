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

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/

/* To check this example is functional,
   Please check the PWM output seeing CPU board specification you are using.
   Please see following pin outputs the 3.814Hz pulse with duty 50%.  */


/* PWM Mode Configuration def */
#define TCPWMx_GRPx_CNTx_PWM            TCPWM1_GRP0_CNT0
#define PCLK_TCPWMx_CLOCKSx_PWM         PCLK_TCPWM1_CLOCKS0
#define TCPWM_PERI_CLK_DIVIDER_NO_PWM   0u

#define TCPWMx_PWM_PRESCALAR_DIV_x      CY_TCPWM_PRESCALER_DIVBY_128  // 2,000,000 / 128 = 15,625Hz
#define TCPWMx_PERIOD                   0x1000    // 15,625Hz / 4096 (0x1000) = 3.815Hz (PWM frequency)
#define TCPWMx_COMPARE0                 0x800     // 0x800 / 0x1000 = 0.5 (PWM duty)

// LINEx: x = (256 * GrpNum) + (CntNum)
/* TCPWM_LINE0 */
#define TCPWMx_LINEx_PORT                GPIO_PRT6
#define TCPWMx_LINEx_PIN                 1u
#define TCPWMx_LINEx_MUX                 P6_1_TCPWM1_LINE0

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
cy_stc_gpio_pin_config_t pin_cfg1 = 
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = TCPWMx_LINEx_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

cy_stc_tcpwm_pwm_config_t const MyPWM_config =
{
    .pwmMode            = CY_TCPWM_PWM_MODE_PWM,
    .clockPrescaler     = TCPWMx_PWM_PRESCALAR_DIV_x,
    .debug_pause        = false,
    .cc0MatchMode       = CY_TCPWM_PWM_TR_CTRL2_CLEAR,
    .overflowMode       = CY_TCPWM_PWM_TR_CTRL2_SET,
    .underflowMode      = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE,
    .cc1MatchMode       = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE,
    .deadTime           = 0u,
    .deadTimeComp       = 0u,
    .runMode            = CY_TCPWM_PWM_CONTINUOUS,
    .period             = TCPWMx_PERIOD - 1ul,
    .period_buff        = 0ul,
    .enablePeriodSwap   = false,
    .compare0           = TCPWMx_COMPARE0,
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

/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/
int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /*--------------------------------*/
    /* Clock Configuration for TCPWMs */
    /*--------------------------------*/
  #if (CY_USE_PSVP == 1u)
    uint32_t sourceFreq = 24000000ul;
  #else
    uint32_t sourceFreq = 80000000ul;
  #endif
    uint32_t targetFreq = 2000000ul;
    uint32_t divNum = (sourceFreq / targetFreq);
    CY_ASSERT((sourceFreq % targetFreq) == 0ul); // target frequency accuracy

    /* Assign a programmable divider  for TCPWMx_GRPx_CNTx_PWM */
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWMx_CLOCKSx_PWM, CY_SYSCLK_DIV_16_BIT, TCPWM_PERI_CLK_DIVIDER_NO_PWM);
    /* Sets the 16-bit divider */
    Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(PCLK_TCPWMx_CLOCKSx_PWM), CY_SYSCLK_DIV_16_BIT, TCPWM_PERI_CLK_DIVIDER_NO_PWM, (divNum-1ul));
    /* Enable the divider */
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(PCLK_TCPWMx_CLOCKSx_PWM), CY_SYSCLK_DIV_16_BIT, TCPWM_PERI_CLK_DIVIDER_NO_PWM);

    /*------------------------------*/
    /* Port Configuration for TCPWM */
    /*------------------------------*/    
    Cy_GPIO_Pin_Init(TCPWMx_LINEx_PORT, TCPWMx_LINEx_PIN, &pin_cfg1);

    /* Initialize TCPWM0_GRPx_CNTx_PWM_PR as PWM Mode & Enable */
    Cy_Tcpwm_Pwm_Init(TCPWMx_GRPx_CNTx_PWM, &MyPWM_config);
    Cy_Tcpwm_Pwm_Enable(TCPWMx_GRPx_CNTx_PWM);
    Cy_Tcpwm_TriggerStart(TCPWMx_GRPx_CNTx_PWM);

    for(;;);
}

/* [] END OF FILE */
