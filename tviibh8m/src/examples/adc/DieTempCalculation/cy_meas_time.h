/***************************************************************************//**
* \file cy_meas_time.h
* \version 1.0
*
* \brief
* Provides an API declaration of the time measurement.
*
********************************************************************************
* \copyright
* Copyright 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

/******************************************************************************/
/* Header Guard                                                               */
/******************************************************************************/
#ifndef CY_MEAS_TIME_H
#define CY_MEAS_TIME_H

#if defined(__cplusplus)
extern "C" {
#endif


/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include "cy_project.h"
#include "cy_device_headers.h"


/******************************************************************************/
/* Macro Definitions                                                          */
/******************************************************************************/

/** Time measurement sample points in app */
#define MEAS_TIME_SAMPLE_POINTS         10

/** Time measurement counter configuration */
#define MEAS_TIME_TIMER_CH              TCPWM0_GRP0_CNT0
#define MEAS_TIME_TIMER_CH_CLK_SRC      PCLK_TCPWM0_CLOCKS0
#define MEAS_TIME_TIMER_CH_CLK_DIV      1u
#define MEAS_TIME_TIMER_CH_IRQ_SRC      tcpwm_0_interrupts_0_IRQn


/******************************************************************************/
/* Configuration Variables                                                    */
/******************************************************************************/

/** 
 * \var cy_stc_tcpwm_counter_config_t measTimerConfig
 * \brief Measurement timer configuration structure
 */
cy_stc_tcpwm_counter_config_t measTimerConfig =
{
    .period             = 1000 - 1,                             // 1,000,000 / 1000 = 1000Hz (1ms)
    .clockPrescaler     = CY_TCPWM_PRESCALER_DIVBY_2,           // 2,000,000Hz / 2 = 1,000,000Hz (1us)
    .runMode            = CY_TCPWM_COUNTER_CONTINUOUS,
    .countDirection     = CY_TCPWM_COUNTER_COUNT_UP,
    .debug_pause        = 0uL,
    .compareOrCapture   = CY_TCPWM_COUNTER_MODE_COMPARE,
    .compare0           = 0,
    .compare0_buff      = 0,
    .compare1           = 0,
    .compare1_buff      = 0,
    .enableCompare0Swap = false,
    .enableCompare1Swap = false,
    .interruptSources   = CY_TCPWM_INT_NONE,
    .capture0InputMode  = CY_TCPWM_INPUT_LEVEL,
    .capture0Input      = 0uL,
    .reloadInputMode    = CY_TCPWM_INPUT_LEVEL,
    .reloadInput        = 0uL,
    .startInputMode     = CY_TCPWM_INPUT_LEVEL,
    .startInput         = 0uL,
    .stopInputMode      = CY_TCPWM_INPUT_LEVEL,
    .stopInput          = 0uL,
    .capture1InputMode  = CY_TCPWM_INPUT_LEVEL,
    .capture1Input      = 0uL,
    .countInputMode     = CY_TCPWM_INPUT_LEVEL,
    .countInput         = 1uL,
    .trigger0EventCfg   = CY_TCPWM_COUNTER_OVERFLOW,
    .trigger1EventCfg   = CY_TCPWM_COUNTER_OVERFLOW,
};

/** 
 * \var cy_stc_sysint_irq_t timerOverflowIrqcfg
 * \brief Measurement timer overflow interrupt configuration structure
 */
cy_stc_sysint_irq_t timerOverflowIrqcfg = 
{
    .sysIntSrc  = MEAS_TIME_TIMER_CH_IRQ_SRC,
    .intIdx     = CPUIntIdx3_IRQn,
    .isEnabled  = true,
};

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

/** 
 * \var typedef struct cy_stc_meas_time_t
 * \brief Analog measurement values
 */
typedef struct {
    bool        isTimerRunning;                         /**< is timer running */
    uint8_t     timeMeasId;                             /**< time measurement id for log */
    uint8_t     multOneMilli[MEAS_TIME_SAMPLE_POINTS];  /**< multiplier for 1ms overflow */
    uint32_t    countOneMicro[MEAS_TIME_SAMPLE_POINTS]; /**< current count in us */
} cy_stc_meas_time_t;

/** Timer log measurement values */
cy_stc_meas_time_t timerLog;


/******************************************************************************/
/* Function Prototypes                                                        */
/******************************************************************************/

/** Function to write the codec control registers */
static void MeasTime_Handler(void);

/** Function to write the codec control registers */
void Cy_MeasTime_Init(void);

/** Function to read the codec control registers */
void Cy_MeasTime_Start(void);

/** Function to read the codec control registers */
void Cy_MeasTime_Stop(void);
         
/** Function to read the codec control registers */
void Cy_MeasTime_Clear(void);


/*******************************************************************************
* Function Name: MeasTime_Handler
********************************************************************************
*
* \brief 
* Timer overflow interrupt handler.
*
* \param
* none
* 
* \return
* none
*
*******************************************************************************/
static void MeasTime_Handler(void)
{
    /* Get the period match interrupt event */
    if(Cy_Tcpwm_Counter_GetTC_IntrMasked(MEAS_TIME_TIMER_CH))
    {
        /* Log the millisecond count for sample point id */
        timerLog.multOneMilli[timerLog.timeMeasId]++;
        /* Clear the interruupt */
        Cy_Tcpwm_Counter_ClearTC_Intr(MEAS_TIME_TIMER_CH);
    }
}

/*******************************************************************************
* Function Name: Cy_MeasTime_Init
********************************************************************************
*
* \brief 
* Initialize the measurement timer.
*
* \param
* none
* 
* \return
* none
*
*******************************************************************************/
void Cy_MeasTime_Init(void)
{
    /* Assign a programmable clock and divider for measurement timer */
    Cy_SysClk_PeriphAssignDivider(MEAS_TIME_TIMER_CH_CLK_SRC, (cy_en_divider_types_t)CY_SYSCLK_DIV_16_BIT, MEAS_TIME_TIMER_CH_CLK_DIV);
    Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(MEAS_TIME_TIMER_CH_CLK_SRC), CY_SYSCLK_DIV_16_BIT, MEAS_TIME_TIMER_CH_CLK_DIV, 39u); 
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(MEAS_TIME_TIMER_CH_CLK_SRC), CY_SYSCLK_DIV_16_BIT, MEAS_TIME_TIMER_CH_CLK_DIV);
      
    /* Set the Interrupt Priority & Enable the Interrupt */
    Cy_SysInt_InitIRQ(&timerOverflowIrqcfg);
    Cy_SysInt_SetSystemIrqVector(timerOverflowIrqcfg.sysIntSrc, MeasTime_Handler);
    NVIC_SetPriority(CPUIntIdx3_IRQn, 3u);
    NVIC_ClearPendingIRQ(CPUIntIdx3_IRQn);
    NVIC_EnableIRQ(CPUIntIdx3_IRQn);
    
    /* Initialize measurement timer & Enable */
    Cy_Tcpwm_Counter_Init(MEAS_TIME_TIMER_CH, &measTimerConfig);
    Cy_Tcpwm_Counter_Enable(MEAS_TIME_TIMER_CH);
    
    /* Enable Interrupt */
    Cy_Tcpwm_Counter_SetTC_IntrMask(MEAS_TIME_TIMER_CH);
}

/*******************************************************************************
* Function Name: Cy_MeasTime_Init
********************************************************************************
*
* \brief 
* Start the measurement timer.
*
* \param
* none
* 
* \return
* none
*
*******************************************************************************/
void Cy_MeasTime_Start(void)
{
    /* If timer started first time start for id 0 */
    if (timerLog.isTimerRunning == false)
    {
        Cy_MeasTime_Clear();
        timerLog.timeMeasId = 0;
        timerLog.isTimerRunning = true;
        Cy_Tcpwm_TriggerStart(MEAS_TIME_TIMER_CH);
    }
    /* else log the time for current id and move to the next sample point */
    else
    {
        if (timerLog.timeMeasId < MEAS_TIME_SAMPLE_POINTS)
        {
            /* Log the microsecond count for last sample point id */
            timerLog.countOneMicro[timerLog.timeMeasId] = Cy_Tcpwm_Counter_GetCounter(MEAS_TIME_TIMER_CH);
            /* Go to the next sample point and reload the timer */
            timerLog.timeMeasId++;
            Cy_Tcpwm_TriggerReloadOrIndex(MEAS_TIME_TIMER_CH);
        }
    }
}

/*******************************************************************************
* Function Name: Cy_MeasTime_Stop
********************************************************************************
*
* \brief 
* Stop the measurement timer.
*
* \param
* none
* 
* \return
* none
*
*******************************************************************************/
void Cy_MeasTime_Stop(void)
{
    if (timerLog.isTimerRunning == true)
    {
        /* Log the microsecond count for last sample point id */
        timerLog.countOneMicro[timerLog.timeMeasId] = Cy_Tcpwm_Counter_GetCounter(MEAS_TIME_TIMER_CH);
        timerLog.timeMeasId = 0;
        timerLog.isTimerRunning = false;
        Cy_Tcpwm_TriggerStopOrKill(MEAS_TIME_TIMER_CH);
    }
}

/*******************************************************************************
* Function Name: Cy_MeasTime_Clear
********************************************************************************
*
* \brief 
* Clear the measurement timer.
*
* \param
* none
* 
* \return
* none
*
*******************************************************************************/
void Cy_MeasTime_Clear(void)
{
    /* Clear the timer log data */
    memset(&timerLog, 0, sizeof(timerLog));
}


#if defined(__cplusplus)
}
#endif  /* __cplusplus */

#endif  /* CY_MEAS_TIME_H */

/* [] END OF FILE */
