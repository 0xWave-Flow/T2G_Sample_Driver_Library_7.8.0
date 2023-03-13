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

/* Using Shift Register (SR) function of TCPWM, this example copies signal to
   the Line input into PWM output.
   This example uses three ports. One is GPIP output for generating signal to
   be copied from. The second is PWM which will output copied signal.
   And the other is TCPWM Line input to receive GPIO output.
   To make sure the functionality of this example, please connect GPIO output
   and TCPWM Line input ports are seeing CPU board specification you are using.
   After starting this program, please check the output of PWM output using
   an oscilloscope to see the signal has same figure as GPIO output.
   */

#define TCPWMx_GRPx_CNTx_PWM_SR           TCPWM1_GRP0_CNT0
#define PCLK_TCPWMx_CLOCKSx_PWM_SR        PCLK_TCPWM1_CLOCKS0
#define TCPWMx_PERI_CLK_DIVIDER_NO_PWM_SR 0

// LINEx: x = (256 * GrpNum) + (CntNum)
#define TCPWMx_LINEx_PORT                 GPIO_PRT6
#define TCPWMx_LINEx_PIN                  1u
#define TCPWMx_LINEx_MUX                  P6_1_TCPWM1_LINE0

#define TR_ONE_CNT_NR_USE                 1u // from 0 to 2

// TR_ONE_CNT_INx: x = (256 * GrpNum) + (CntNum) + TR_ONE_CNT_NR_USE
#define TCPWMx_LINEx_IN_PORT              GPIO_PRT6
#define TCPWMx_LINEx_IN_PIN               2u
#define TCPWMx_LINEx_IN_MUX               P6_2_TCPWM1_TR_ONE_CNT_IN1

#define GPIO_TOBE_COPYED_PORT             GPIO_PRT6
#define GPIO_TOBE_COPYED_PIN              3u
#define GPIO_TOBE_COPYED_MUX              P6_3_GPIO

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

cy_stc_gpio_pin_config_t gpio_port_pin_cfg = 
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = GPIO_TOBE_COPYED_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

cy_stc_gpio_pin_config_t pwm_line_out_pin_cfg = 
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

cy_stc_gpio_pin_config_t pwm_line_in_pin_cfg = 
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom     = TCPWMx_LINEx_IN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

cy_stc_tcpwm_sr_config_t const MyPWM_config =
{
    .clockPrescaler     = CY_TCPWM_PRESCALER_DIVBY_1,
    .debug_pause        = false,
    .runMode            = CY_TCPWM_COUNTER_CONTINUOUS,
    .period             = 0x0000ul,
    .period_buff        = 0x0001ul,
    .compare0           = 0x0001ul,
    .compare1           = 0x0001ul,
    .enableCompare0Swap = false,
    .enableCompare1Swap = false,
    .interruptSources   = CY_TCPWM_INT_NONE,
    .serialInInputMode  = CY_TCPWM_INPUT_LEVEL, 	// level detect
    .serialInInput      = TR_ONE_CNT_NR_USE+2ul, 	// 0: always "0". 1: always "1". x (above 2): tr_one_cnt_in[offset+x]
    .reloadInputMode    = CY_TCPWM_INPUT_LEVEL,
    .reloadInput        = 0ul,
    .startInputMode     = CY_TCPWM_INPUT_LEVEL,
    .startInput         = 0ul,
    .stopInputMode      = CY_TCPWM_INPUT_LEVEL,
    .stopInput          = 0ul,
    .shiftInputMode     = CY_TCPWM_INPUT_LEVEL,
    .shiftInput         = 1ul,
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
    /* Assign a programmable divider  for TCPWM0_GRPx_CNTx_PWM_SR */
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWMx_CLOCKSx_PWM_SR, CY_SYSCLK_DIV_16_BIT, TCPWMx_PERI_CLK_DIVIDER_NO_PWM_SR);
    /* Sets the 16-bit divider */
    Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(PCLK_TCPWMx_CLOCKSx_PWM_SR), CY_SYSCLK_DIV_16_BIT, TCPWMx_PERI_CLK_DIVIDER_NO_PWM_SR, (divNum-1ul));
    /* Enable the divider */
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(PCLK_TCPWMx_CLOCKSx_PWM_SR), CY_SYSCLK_DIV_16_BIT, TCPWMx_PERI_CLK_DIVIDER_NO_PWM_SR);

    /*----------------------------------------*/
    /* Port Configuration for TCPWM PR, GPIO  */
    /*----------------------------------------*/
    Cy_GPIO_Pin_Init(GPIO_TOBE_COPYED_PORT, GPIO_TOBE_COPYED_PIN, &gpio_port_pin_cfg);
    Cy_GPIO_Pin_Init(TCPWMx_LINEx_PORT, TCPWMx_LINEx_PIN, &pwm_line_out_pin_cfg);
    Cy_GPIO_Pin_Init(TCPWMx_LINEx_IN_PORT, TCPWMx_LINEx_IN_PIN, &pwm_line_in_pin_cfg);
    
    /* Initialize TCPWM0_GRPx_CNTx_PWM_SR & Enable */
    Cy_Tcpwm_Sr_Init(TCPWMx_GRPx_CNTx_PWM_SR, &MyPWM_config);
    Cy_Tcpwm_Sr_Enable(TCPWMx_GRPx_CNTx_PWM_SR);
    Cy_Tcpwm_TriggerStart(TCPWMx_GRPx_CNTx_PWM_SR);

    for(;;)
    {
        Cy_GPIO_Inv(GPIO_TOBE_COPYED_PORT, GPIO_TOBE_COPYED_PIN);

        Cy_SysTick_DelayInUs(1000ul);
    }
}

/* [] END OF FILE */
