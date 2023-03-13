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

/* This example uses two TCPWM. One TCPWM outputs PWM pulse with constant
   frequency and duty. The other TCPWM receives the PWM pulse and captures
   count value when the rising/falling edge of the PWM detected as generating
   interrupts. Inside the ISR, CPU calculates the PWM's frequency and duty.
   Please check the variable "g_periodLow_us", "g_periodHigh_us", and "g_duty"
   to see the value is expected using debugger.
   Please make sure PWM output and TR_ONE_CNT_IN ports are connected seeing
   CPU board specification you are using.                   */

/* Capture Mode Configuration definitions */
#define TCPWMx_GRPx_CNTx_CAPTURE           TCPWM1_GRP0_CNT0
#define PCLK_TCPWMx_CLOCKSx_CAPTURE        PCLK_TCPWM1_CLOCKS0

#define TR_ONE_CNT_NR_USE                       0u // from 0 to 2

// TR_ONE_CNT_INx: x = (256 * GrpNum) + (CntNum)
#define TCPWMx_TR_ONE_CNT_INx_PORT         GPIO_PRT6
#define TCPWMx_TR_ONE_CNT_INx_PIN          1u
#define TCPWMx_TR_ONE_CNT_INx_MUX          P6_1_TCPWM1_TR_ONE_CNT_IN0

/* PWM Configuration definitions */
#define TCPWMx_GRPx_CNTx_PWM               TCPWM1_GRP0_CNT1
#define PCLK_TCPWMx_CLOCKSx_PWM            PCLK_TCPWM1_CLOCKS1

// LINEx: x = (256 * GrpNum) + (CntNum)
/* TCPWM0_LINE1 */
#define TCPWMx_LINEx_PORT                  GPIO_PRT6
#define TCPWMx_LINEx_PIN                   3u
#define TCPWMx_LINEx_MUX                   P6_3_TCPWM1_LINE1

#define TCPWMx_PERI_CLK_DIVIDER_NO         0u // common for PWM and capture TCPWM

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
cy_stc_gpio_pin_config_t tcpwm_trig_in_pin_cfg = 
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom     = TCPWMx_TR_ONE_CNT_INx_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

cy_stc_gpio_pin_config_t pwm_line_out_pin_cfg = 
{
    .outVal    = 0x01u,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = TCPWMx_LINEx_MUX,
    .intEdge   = 0u,
    .intMask   = 0u,
    .vtrip     = 0u,
    .slewRate  = 0u,
    .driveSel  = 0u,
};

cy_stc_tcpwm_counter_config_t const MyCounter_config =
{
    .period             = 0xFFFF,                             // A TCPWM in GRP0 has 16 bit counter
    .clockPrescaler     = CY_TCPWM_PRESCALER_DIVBY_4, 		  // 2,000,000 / 4 = 500,000 Hz
    .runMode            = CY_TCPWM_COUNTER_CONTINUOUS,
    .countDirection     = CY_TCPWM_COUNTER_COUNT_UP,
    .debug_pause        = false,
    .compareOrCapture   = CY_TCPWM_COUNTER_MODE_CAPTURE,
    .compare0           = 0ul,
    .compare0_buff      = 0ul,
    .compare1           = 0ul,
    .compare1_buff      = 0ul,
    .enableCompare0Swap = false,
    .enableCompare1Swap = false,
    .interruptSources   = CY_TCPWM_INT_NONE,
    .capture0InputMode  = CY_TCPWM_INPUT_RISING_EDGE,			// detect rising edge
    .capture0Input      = TR_ONE_CNT_NR_USE+2ul,              	// 0: always "0". 1: always "1". x (above 2): tr_one_cnt_in[offset+x]
    .reloadInputMode    = CY_TCPWM_INPUT_LEVEL,
    .reloadInput        = 0ul,
    .startInputMode     = CY_TCPWM_INPUT_LEVEL,
    .startInput         = 0ul,
    .stopInputMode      = CY_TCPWM_INPUT_LEVEL,
    .stopInput          = 0ul,
    .capture1InputMode  = CY_TCPWM_INPUT_FALLING_EDGE,          // detect falling edge
    .capture1Input      = TR_ONE_CNT_NR_USE+2ul,              	// 0: always "0". 1: always "1". x (above 2): tr_one_cnt_in[offset+x]
    .countInputMode     = CY_TCPWM_INPUT_LEVEL,
    .countInput         = 1ul,
    .trigger0EventCfg   = CY_TCPWM_COUNTER_CC0_MATCH,
	.trigger1EventCfg   = CY_TCPWM_COUNTER_OVERFLOW,
};

cy_stc_tcpwm_pwm_config_t const MyPWM_config =
{
    .pwmMode            = CY_TCPWM_PWM_MODE_PWM,
    .clockPrescaler     = CY_TCPWM_PRESCALER_DIVBY_1,
    .debug_pause        = false,
    .cc0MatchMode       = CY_TCPWM_PWM_TR_CTRL2_CLEAR,
    .overflowMode       = CY_TCPWM_PWM_TR_CTRL2_SET,
    .underflowMode      = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE,
    .cc1MatchMode       = CY_TCPWM_PWM_TR_CTRL2_NO_CHANGE,
    .deadTime           = 0ul,
    .deadTimeComp       = 0ul,
    .runMode            = CY_TCPWM_PWM_CONTINUOUS,
    .period             = 2560ul - 1ul,                        // Period = 2560 / 2000000 = 1280 [us]
    .period_buff        = 0ul,
    .enablePeriodSwap   = false,
    .compare0           = 640ul,                               // Duty 25% (High duration: 320 [us], Low duration: 960 [us])
    .compare1           = 0ul,
    .enableCompare0Swap = false,
    .enableCompare1Swap = false,
    .interruptSources   = CY_TCPWM_INT_NONE,
    .invertPWMOut       = 0ul,
    .invertPWMOutN      = 0ul,
    .killMode           = CY_TCPWM_PWM_STOP_ON_KILL,
    .switchInputMode    = CY_TCPWM_INPUT_LEVEL ,
    .switchInput        = 0ul,
    .reloadInputMode    = CY_TCPWM_INPUT_LEVEL ,
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

cy_stc_sysint_irq_t irq_cfg = 
{
    .sysIntSrc  = tcpwm_1_interrupts_0_IRQn,
    .intIdx     = CPUIntIdx3_IRQn,
    .isEnabled  = true,
};

uint32_t g_periodLow_us  = 0ul;
uint32_t g_periodHigh_us = 0ul;
uint32_t g_duty          = 0ul;

void capture_isr_handler(void)
{
    static uint16_t countCC0Match = 0u;
    static uint16_t countCC1Match = 0u;
    static uint16_t countLow      = 0u;
    static uint16_t countHigh     = 0u;

    if(Cy_Tcpwm_Counter_GetCC0_IntrMasked(TCPWMx_GRPx_CNTx_CAPTURE))
    {
        // CCO would capture rising edge of input pulse.
        Cy_Tcpwm_Counter_ClearCC0_Intr(TCPWMx_GRPx_CNTx_CAPTURE);

        countCC0Match = Cy_Tcpwm_Counter_GetCompare0(TCPWMx_GRPx_CNTx_CAPTURE);
        countLow = countCC0Match - countCC1Match; // Using data under flow behavior.
        g_periodLow_us = (uint32_t)countLow * 2ul; // frequency = 0.5 [MHz] -> 1 count = 2 [us]
    }

    if(Cy_Tcpwm_Counter_GetCC1_IntrMasked(TCPWMx_GRPx_CNTx_CAPTURE))
    {
        // CC1 would capture falling edge of input pulse.
        Cy_Tcpwm_Counter_ClearCC1_Intr(TCPWMx_GRPx_CNTx_CAPTURE);

        countCC1Match = Cy_Tcpwm_Counter_GetCompare1(TCPWMx_GRPx_CNTx_CAPTURE);
        countHigh = countCC1Match - countCC0Match;  // Using data under flow behavior.
        g_periodHigh_us = (uint32_t)countHigh * 2ul; // frequency = 0.5 [MHz] -> 1 count = 2 [us]
    }

    g_duty = (g_periodHigh_us * 100ul) / (g_periodHigh_us + g_periodLow_us);
}

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

    /* Assign a programmable divider  for TCPWM0_GRPx_CNTx_COUNTER */
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWMx_CLOCKSx_CAPTURE, CY_SYSCLK_DIV_16_BIT, TCPWMx_PERI_CLK_DIVIDER_NO);
    Cy_SysClk_PeriphAssignDivider(PCLK_TCPWMx_CLOCKSx_PWM, CY_SYSCLK_DIV_16_BIT, TCPWMx_PERI_CLK_DIVIDER_NO);
    /* Sets the 16-bit divider */
    Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(PCLK_TCPWMx_CLOCKSx_CAPTURE), CY_SYSCLK_DIV_16_BIT, TCPWMx_PERI_CLK_DIVIDER_NO, (divNum - 1ul));
    /* Enable the divider */
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(PCLK_TCPWMx_CLOCKSx_CAPTURE), CY_SYSCLK_DIV_16_BIT, TCPWMx_PERI_CLK_DIVIDER_NO);

    /*--------------------------------*/
    /* Interrupt setting for Capture  */
    /*--------------------------------*/
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, capture_isr_handler);
    /* Set the Interrupt Priority & Enable the Interrupt */
    NVIC_SetPriority(irq_cfg.intIdx, 3ul);
    NVIC_EnableIRQ(irq_cfg.intIdx);

    /*------------------------------------------*/
    /* Port Configuration for PWM, Counter Trig */
    /*------------------------------------------*/
    Cy_GPIO_Pin_Init(TCPWMx_TR_ONE_CNT_INx_PORT, TCPWMx_TR_ONE_CNT_INx_PIN, &tcpwm_trig_in_pin_cfg);
    Cy_GPIO_Pin_Init(TCPWMx_LINEx_PORT, TCPWMx_LINEx_PIN, &pwm_line_out_pin_cfg);

    /* Initialize PCLK_TCPWM0_CLOCKSx_CAPTURE as Capture Mode & Enable */
    Cy_Tcpwm_Counter_Init(TCPWMx_GRPx_CNTx_CAPTURE, &MyCounter_config);
    Cy_Tcpwm_Counter_SetCC0_IntrMask(TCPWMx_GRPx_CNTx_CAPTURE);
    Cy_Tcpwm_Counter_SetCC1_IntrMask(TCPWMx_GRPx_CNTx_CAPTURE);
    Cy_Tcpwm_Counter_Enable(TCPWMx_GRPx_CNTx_CAPTURE);
    Cy_Tcpwm_TriggerStart(TCPWMx_GRPx_CNTx_CAPTURE); 

    /* Initialize TCPWM0_GRPx_CNTx_PWM as PWM & Enable */
    Cy_Tcpwm_Pwm_Init(TCPWMx_GRPx_CNTx_PWM, &MyPWM_config);
    Cy_Tcpwm_Pwm_Enable(TCPWMx_GRPx_CNTx_PWM);
    Cy_Tcpwm_TriggerStart(TCPWMx_GRPx_CNTx_PWM);

    for(;;);
}

/* [] END OF FILE */
