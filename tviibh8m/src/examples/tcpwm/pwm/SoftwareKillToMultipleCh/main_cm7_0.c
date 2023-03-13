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

/* To check this example is functional,
   please see following pin outputs the 976.5625Hz pulse with duty 50%.  */

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/

#define TCPWM_PERI_CLK_DIVIDER_COMMON_NO   0u

typedef struct
{
    volatile stc_TCPWM_GRP_CNT_t* tcpwm;
    en_clk_dst_t                  clkDest;
    volatile stc_GPIO_PRT_t*      port;
    uint32_t                      pin;
    en_hsiom_sel_t                hsiom;
} cy_stc_pwm_info_t;

// All PWM information to be used in this example
const cy_stc_pwm_info_t g_pwms[] =
{
//  {            tcpwm,               clkDest,       port, pin,                hsiom },
    { TCPWM1_GRP0_CNT0,   PCLK_TCPWM1_CLOCKS0,  GPIO_PRT6, 1ul,    P6_1_TCPWM1_LINE0 },
    { TCPWM1_GRP0_CNT1,   PCLK_TCPWM1_CLOCKS1,  GPIO_PRT6, 3ul,    P6_3_TCPWM1_LINE1 },
    { TCPWM1_GRP0_CNT2,   PCLK_TCPWM1_CLOCKS2,  GPIO_PRT6, 5ul,    P6_5_TCPWM1_LINE2 },
    { TCPWM1_GRP0_CNT3,   PCLK_TCPWM1_CLOCKS3,  GPIO_PRT6, 7ul,    P6_7_TCPWM1_LINE3 },
    { TCPWM1_GRP0_CNT4,   PCLK_TCPWM1_CLOCKS4,  GPIO_PRT2, 3ul,    P2_3_TCPWM1_LINE4 },
    { TCPWM1_GRP0_CNT5,   PCLK_TCPWM1_CLOCKS5,  GPIO_PRT4, 1ul,    P4_1_TCPWM1_LINE5 },
    { TCPWM1_GRP1_CNT0, PCLK_TCPWM1_CLOCKS256,  GPIO_PRT3, 5ul,  P3_5_TCPWM1_LINE256 },
    { TCPWM1_GRP1_CNT1, PCLK_TCPWM1_CLOCKS257,  GPIO_PRT3, 4ul,  P3_4_TCPWM1_LINE257 },
    { TCPWM1_GRP2_CNT0, PCLK_TCPWM1_CLOCKS512, GPIO_PRT18, 0ul, P18_0_TCPWM1_LINE512 },
    { TCPWM1_GRP2_CNT1, PCLK_TCPWM1_CLOCKS513, GPIO_PRT18, 2ul, P18_2_TCPWM1_LINE513 },
};
#define PWM_NUM (sizeof(g_pwms) / sizeof(g_pwms[0]))

/* TCWPM prescalar & Period configuration for all instances */
#define TCPWMx_PWM_PRESCALAR_DIV_x CY_TCPWM_PRESCALER_DIVBY_128 // 2,000,000 / 128 = 15,625Hz
#define TCPWMx_PERIOD              16u                              // 15,625 / 16 = 976.5625Hz (PWM frequency)
#define TCPWMx_COMPARE0            8u                               // 8 / 16 = 0.5 (PWM duty)

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
cy_stc_gpio_pin_config_t pwm_line_out_pin_cfg = 
{
    .outVal    = 1ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = HSIOM_SEL_GPIO, // will be updated in runtime
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
    .deadTime           = 0ul,
    .deadTimeComp       = 0ul,
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
    .killMode           = CY_TCPWM_PWM_SYNCH_STOP_ON_KILL,
    .switchInputMode    = CY_TCPWM_INPUT_LEVEL,
    .switchInput        = 0ul,
    .reloadInputMode    = CY_TCPWM_INPUT_LEVEL,
    .reloadInput        = 0ul,
    .startInputMode     = CY_TCPWM_INPUT_LEVEL,
    .startInput         = 0ul,
    .kill0InputMode     = CY_TCPWM_INPUT_LEVEL,
    .kill0Input         = 5ul,                            // Initialize counter for kill on tr_all_cnt_in[0]
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
    CY_ASSERT((sourceFreq % targetFreq) == 0ul);

    /* Assuming every TCPWM in common clock group */
    /* Sets the 16-bit divider */
    Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(g_pwms[0].clkDest), CY_SYSCLK_DIV_16_BIT, TCPWM_PERI_CLK_DIVIDER_COMMON_NO, (divNum - 1ul));
    /* Enable the divider */
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(g_pwms[0].clkDest), CY_SYSCLK_DIV_16_BIT, TCPWM_PERI_CLK_DIVIDER_COMMON_NO);

    for(uint32_t i = 0; i < PWM_NUM; i++)
    {
        /* Assign a programmable divider for TCPWM */
        Cy_SysClk_PeriphAssignDivider(g_pwms[i].clkDest, CY_SYSCLK_DIV_16_BIT, TCPWM_PERI_CLK_DIVIDER_COMMON_NO);

        /*----------------------------*/
        /* Port Configuration for PWM */
        /*----------------------------*/
        pwm_line_out_pin_cfg.hsiom = g_pwms[i].hsiom;
        Cy_GPIO_Pin_Init(g_pwms[i].port, g_pwms[i].pin, &pwm_line_out_pin_cfg);

        /* Initialize PWM & Enable */
        Cy_Tcpwm_Pwm_Init(g_pwms[i].tcpwm, &MyPWM_config);
        Cy_Tcpwm_Pwm_Enable(g_pwms[i].tcpwm);
        Cy_Tcpwm_TriggerStart(g_pwms[i].tcpwm);
    }

    // wait for 2 [s]
    Cy_SysTick_DelayInUs(2000000);

    /* Trigger tr_all_cnt_in[0] by SW */
    Cy_TrigMux_SwTrigger(TRIG_OUT_MUX_5_TCPWM1_ALL_CNT_TR_IN0, TRIGGER_TYPE_LEVEL, 1 /*output*/);

    for(;;);
}

/* [] END OF FILE */
