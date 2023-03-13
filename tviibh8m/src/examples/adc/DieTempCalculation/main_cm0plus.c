
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

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cy_project.h"
#include "cy_device_headers.h"


/******************************************************************************/
/* Macro Definitions                                                          */
/******************************************************************************/

/** 
 * Enable logs in the example
 */
#define ENABLE_LOG
//#define ENABLE_MEAS_TIME

/** 
 * Conditional file header
 */
#ifdef ENABLE_MEAS_TIME
#include "cy_meas_time.h"
#endif


/** 
 * Macro definition for SFLASH trim
 */

/** Select the analaog power rails as per your hardware */
#define TRIM_CAL_LINEAR_SEARCH                  0u
#define TRIM_CAL_BINARY_SEARCH                  1u
#define USE_TRIM_CALIBRATION_METHOD             TRIM_CAL_BINARY_SEARCH

/** Select the analaog power rails as per your hardware */
#define USE_TRIM_FOR_3P3V_VDDA                  0u
#define USE_TRIM_FOR_5P0V_VDDA                  1u
#define USE_TEMPERATURE_TRIM_VALUES             USE_TRIM_FOR_5P0V_VDDA

/** Read from the SFLASH table */
#if (USE_TEMPERATURE_TRIM_VALUES == USE_TRIM_FOR_3P3V_VDDA)
    /** SFLASH table for VDDA - 3.3V */
    #define EPASS_TEMP_TRIM_TEMP_COLDSORT       0x17000654ul
    #define EPASS_TEMP_TRIM_TEMP_ROOMSORT       0x1700064Eul
    #define EPASS_TEMP_TRIM_TEMP_HOTCLASS       0x1700065Aul
    #define EPASS_TEMP_TRIM_DIODE_COLDSORT      0x17000656ul
    #define EPASS_TEMP_TRIM_DIODE_ROOMSORT      0x17000650ul
    #define EPASS_TEMP_TRIM_DIODE_HOTCLASS      0x1700065Cul
    #define EPASS_TEMP_TRIM_VBG_COLDSORT        0x17000658ul
    #define EPASS_TEMP_TRIM_VBG_ROOMSORT        0x17000652ul
    #define EPASS_TEMP_TRIM_VBG_HOTCLASS        0x1700065Eul
#else
    /** SFLASH table for VDDA - 5V */
    #define EPASS_TEMP_TRIM_TEMP_COLDSORT       0x17000654ul
    #define EPASS_TEMP_TRIM_TEMP_ROOMSORT       0x1700064Eul
    #define EPASS_TEMP_TRIM_TEMP_HOTCLASS       0x1700065Aul
    #define EPASS_TEMP_TRIM_DIODE_COLDSORT      0x1700066Eul
    #define EPASS_TEMP_TRIM_DIODE_ROOMSORT      0x1700066Aul
    #define EPASS_TEMP_TRIM_DIODE_HOTCLASS      0x17000672ul
    #define EPASS_TEMP_TRIM_VBG_COLDSORT        0x17000670ul
    #define EPASS_TEMP_TRIM_VBG_ROOMSORT        0x1700066Cul
    #define EPASS_TEMP_TRIM_VBG_HOTCLASS        0x17000674ul
#endif  /* (USE_TEMPERATURE_TRIM_VALUES == USE_TRIM_FOR_3P3V_VDDA) */

/** SFLASH table read macro */
#define GET_SFLASH_VALUE(x)                     CY_GET_REG16(x)
#define GET_TRIM_VALUE(x)                       (GET_SFLASH_VALUE(x) & 0xFFFFu)

/** Temperature trim values read from SFLASH */
#define TEMP_TRIM_TEMP_ROOMSORT                 GET_TRIM_VALUE(EPASS_TEMP_TRIM_TEMP_ROOMSORT)
#define TEMP_TRIM_TEMP_COLDSORT                 GET_TRIM_VALUE(EPASS_TEMP_TRIM_TEMP_COLDSORT)
#define TEMP_TRIM_TEMP_HOTCLASS                 GET_TRIM_VALUE(EPASS_TEMP_TRIM_TEMP_HOTCLASS)

#define TEMP_TRIM_DIODE_ROOMSORT                GET_TRIM_VALUE(EPASS_TEMP_TRIM_DIODE_ROOMSORT)
#define TEMP_TRIM_DIODE_COLDSORT                GET_TRIM_VALUE(EPASS_TEMP_TRIM_DIODE_COLDSORT)
#define TEMP_TRIM_DIODE_HOTCLASS                GET_TRIM_VALUE(EPASS_TEMP_TRIM_DIODE_HOTCLASS)

#define TEMP_TRIM_VBG_ROOMSORT                  GET_TRIM_VALUE(EPASS_TEMP_TRIM_VBG_ROOMSORT)
#define TEMP_TRIM_VBG_COLDSORT                  GET_TRIM_VALUE(EPASS_TEMP_TRIM_VBG_COLDSORT)
#define TEMP_TRIM_VBG_HOTCLASS                  GET_TRIM_VALUE(EPASS_TEMP_TRIM_VBG_HOTCLASS)

/** 
 * Macro definition for ADC instance and Temperature measurement channels
 */

/** ADC instance, clock and irq configuration macro */
#define CY_ADC_MACRO                            PASS0_SAR0
#define CY_ADC_MMIO_MACRO                       PASS0_EPASS_MMIO
#define CY_ADC_PCLK                             PCLK_PASS0_CLOCK_SAR0
#define CY_ADC_IRQN                             pass_0_interrupts_sar_0_IRQn

/** ADC channels for diode and temperature measurements */
#define CY_ADC_IN_VBG                           CY_ADC_PIN_ADDRESS_VBG
#define CY_ADC_IN_TEMP                          CY_ADC_PIN_ADDRESS_VTEMP

/** ADC logical channel to be used */
#define ADC_LOGICAL_CHANNEL                     0u

/** Number of iteration to get the ADC readings */
#define ADC_CH_NUM_OF_ITERATION                 16u     /* Should be in 2^n */
#define ADC_CH_AVG_RSHIFT_VALUE                 4u      /* Should be in n, in 2^n */

/** Sample time for sampling ADC channel */
#define ADC_CH_SAMPLE_TIME                      120u

/** Defines low and high range for range detection mode */
#define ADC_CH_RANGE_DETECT_LOW                 0x0000u
#define ADC_CH_RANGE_DETECT_HIGH                0x0FFFu

/** Select the internal current requirement for the die-temperature sensor 
    0->1uA, 2->2uA, 3->5uA, 3->10uA */
#define ADC_CAL_TS_CUR_SEL_VALUE                0u
#define ADC_CAL_TS_CUR_SEL_OFFSET               8u
#define ADC_CAL_TS_CUR_SEL_MASK                 (ADC_CAL_TS_CUR_SEL_VALUE << ADC_CAL_TS_CUR_SEL_OFFSET)

/** Select current or voltage output from the die-temperature sensor
    0->current, 1->voltage */
#define ADC_CAL_TS_VI_SEL_VALUE                 0u
#define ADC_CAL_TS_VI_SEL_OFFSET                6u
#define ADC_CAL_TS_VI_SEL_MASK                  (ADC_CAL_TS_VI_SEL_VALUE << ADC_CAL_TS_VI_SEL_OFFSET)

/** Setup test calibration register to measure the die-temperature */
#define ADC_CAL_TEMP_TEST_CTL_ADDR_OFFSET       (0x80)
#define ADC_CAL_TEMP_TEST_CTL_ADDR              ((uint32_t)CYREG_PASS0_PASS_CTL + ADC_CAL_TEMP_TEST_CTL_ADDR_OFFSET)
#define ADC_CAL_TEMP_TEST_CTL_MASK              (ADC_CAL_TS_CUR_SEL_MASK | ADC_CAL_TS_VI_SEL_MASK)

/** 
 * Macro definition for constants used for temperature measurement
 */

/** ADC calibration type macro */
#define ADC_CAL_TYPE_OFFSET                     0
#define ADC_CAL_TYPE_GAIN                       1

/** ADC calibration iteration macro */
#define ADC_CAL_GAIN_MAX_VALUE                  15
#define ADC_CAL_GAIN_MIN_VALUE                  -15
#define ADC_CAL_OFFSET_MAX_VALUE                127
#define ADC_CAL_OFFSET_MIN_VALUE                -128

/** ADC calibration conversion limiting macro */
#define ADC_VFERL_OFFSET_TRANSIT_VALUE          1               // initial value --> 0 
#define ADC_VFERH_OFFSET_TRANSIT_VALUE          4094            // initial value --> 4095
#define ADC_VFERL_GAIN_TRANSIT_VALUE            1               // initial value --> 0

/** ADC calibration liner trace points */
#define ADC_LINEAR_CAL_ITERATION_VALUE          (ADC_CH_AVG_RSHIFT_VALUE*2)


/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

/** 
 * \var cy_stc_adc_config_t adcConfig
 * \brief ADC-SAR configuration structure
 */
cy_stc_adc_config_t adcConfig =
{
    .preconditionTime    = 0u,
    .powerupTime         = 0u,
    .enableIdlePowerDown = false,
    .msbStretchMode      = CY_ADC_MSB_STRETCH_MODE_2CYCLE,
    .enableHalfLsbConv   = 0u,
    .sarMuxEnable        = true,
    .adcEnable           = true,
    .sarIpEnable         = true,
};

/** 
 * \var cy_stc_adc_channel_config_t adcChannelConfig
 * \brief ADC channel configuration structure
 */
cy_stc_adc_channel_config_t adcChannelConfig =
{
    .triggerSelection          = CY_ADC_TRIGGER_OFF,
    .channelPriority           = 0u,
    .preenptionType            = CY_ADC_PREEMPTION_FINISH_RESUME,
    .isGroupEnd                = true,
    .doneLevel                 = CY_ADC_DONE_LEVEL_PULSE,
    .pinAddress                = CY_ADC_PIN_ADDRESS_VREF_L,
    .portAddress               = CY_ADC_PORT_ADDRESS_SARMUX0,
    .extMuxSelect              = 0u,
    .extMuxEnable              = true,
    .preconditionMode          = CY_ADC_PRECONDITION_MODE_OFF,
    .overlapDiagMode           = CY_ADC_OVERLAP_DIAG_MODE_OFF,
    .sampleTime                = ADC_CH_SAMPLE_TIME,
    .calibrationValueSelect    = CY_ADC_CALIBRATION_VALUE_ALTERNATE,
    .postProcessingMode        = CY_ADC_POST_PROCESSING_MODE_AVG,
    .resultAlignment           = CY_ADC_RESULT_ALIGNMENT_RIGHT,
    .signExtention             = CY_ADC_SIGN_EXTENTION_UNSIGNED,
    .averageCount              = (ADC_CH_NUM_OF_ITERATION-1),
    .rightShift                = ADC_CH_AVG_RSHIFT_VALUE,
    .rangeDetectionMode        = CY_ADC_RANGE_DETECTION_MODE_INSIDE_RANGE,
    .rangeDetectionLoThreshold = ADC_CH_RANGE_DETECT_LOW,
    .rangeDetectionHiThreshold = ADC_CH_RANGE_DETECT_HIGH,
    .mask.grpDone              = true,
    .mask.grpCancelled         = false,
    .mask.grpOverflow          = false,
    .mask.chRange              = false,
    .mask.chPulse              = false,
    .mask.chOverflow           = false,
};

/** 
 * \var cy_stc_sysint_irq_t irq_cfg
 * \brief ADC interrupt configuration structure
 */
cy_stc_sysint_irq_t irq_cfg = 
{
    .sysIntSrc = (cy_en_intr_t)CY_ADC_IRQN,
    .intIdx    = CPUIntIdx3_IRQn,
    .isEnabled = true,
};

/** 
 * \var static bool isConversionComplete
 * \brief ADC conversion complete flag
 */
static bool isConversionComplete = false;

/** 
 * \var static uint16_t resultBuff
 * \brief ADC conversion result buffer place holder
 */
static uint16_t resultBuff;

/** 
 * \var static cy_stc_adc_ch_status_t statusBuff
 * \brief ADC conversion status buffer place holder
 */
static cy_stc_adc_ch_status_t  statusBuff;

/** 
 * \var static double temperatureData
 * \brief stores internal die-temperature value
 */
static double temperatureData;


/******************************************************************************/
/* Function Prototypes                                                        */
/******************************************************************************/

/** Local function to print logs on console */
static void AdcPrintLog(void *fmt, ...);

/** Local handler to handle ADC conversion events */
static void AdcIntHandler(void);

/** Local function to configure the ADC channels */
static void AdcConfigureChannel(cy_en_adc_pin_address_t channelAddress);

/** Local function to read the ADC channels */
static uint16_t AdcReadChannelData(cy_en_adc_pin_address_t channelAddress);

/** Local function to run procedure the calculate soft trim */
static cy_en_adc_status_t AdcCalculateSoftTrim(volatile stc_PASS_SAR_t * base);

/** Local function to check ADC soft trim */
static void AdcCheckSoftTrim(void);

/** Functions only used in binary trim search algorithm */
#if (USE_TRIM_CALIBRATION_METHOD == TRIM_CAL_BINARY_SEARCH)

/** Local function to read the ADC channels */
static uint16_t AdcReadChannelDataWithOffset(cy_en_adc_pin_address_t channelAddress, uint8_t calType, int8_t offset, int8_t gain);

/** Local function to get ADC soft trim offset */
static uint16_t AdcGetTrimOffset(int8_t leftEnd, int8_t rightEnd, uint8_t calType, int8_t offsetForGain, uint8_t connectHorL);

#endif


/*******************************************************************************
* Function Name: AdcPrintLog
********************************************************************************
*
* \brief 
* Print the log of the events in trim and temperature measurement procedure.
*
* \param
* void *    pointer to formatted string to print
*
* \return
* none
*
*******************************************************************************/
static void AdcPrintLog(void *fmt, ...)
{
#if defined ENABLE_LOG
    printf(fmt);
#endif
}

/*******************************************************************************
* Function Name: AdcIntHandler
********************************************************************************
*
* \brief 
* Handle the interrupt events from the ADC module.
*
* \param
* none
*
* \return
* none
*
*******************************************************************************/
static void AdcIntHandler(void)
{
    cy_stc_adc_interrupt_source_t intrSource = { false };

    /* Get the result(s) */
    Cy_Adc_Channel_GetResult(&CY_ADC_MACRO->CH[ADC_LOGICAL_CHANNEL], &resultBuff, &statusBuff);

    /* Clear interrupt source */
    Cy_Adc_Channel_GetInterruptMaskedStatus(&CY_ADC_MACRO->CH[ADC_LOGICAL_CHANNEL], &intrSource);
    Cy_Adc_Channel_ClearInterruptStatus(&CY_ADC_MACRO->CH[ADC_LOGICAL_CHANNEL], &intrSource);

    /* Raise the ADC conversion flag */
    isConversionComplete = true;
}

/*******************************************************************************
* Function Name: AdcConfigureChannel
********************************************************************************
*
* \brief 
* Initialize and enable the ADC channel..
*
* \param
* cy_en_adc_pin_address_t   address of the ADC channel.
*
* \return
* none
*
*******************************************************************************/
static void AdcConfigureChannel(cy_en_adc_pin_address_t channelAddress)
{
    /* Configure the ADC channel to requested address */
    adcChannelConfig.pinAddress = channelAddress;
    Cy_Adc_Channel_Init(&CY_ADC_MACRO->CH[ADC_LOGICAL_CHANNEL], &adcChannelConfig);

    /* Enable ADC ch. */
    Cy_Adc_Channel_Enable(&CY_ADC_MACRO->CH[ADC_LOGICAL_CHANNEL]);
}

/*******************************************************************************
* Function Name: AdcReadChannelData
********************************************************************************
*
* \brief 
* Read the ADC conversion result of the target pin address.
*
* \param
* cy_en_adc_pin_address_t   address of the ADC channel.
*
* \return
* uint16_t  average conversion result.
*
*******************************************************************************/
static uint16_t AdcReadChannelData(cy_en_adc_pin_address_t channelAddress)
{
    /* Configure channel and read data */
    AdcConfigureChannel(channelAddress);

    /* Perform averaging for more accurate result, the averaging count and shift
     * value should be like: averagingCount = 2 ^ averageShift */
    Cy_Adc_Channel_SoftwareTrigger(&CY_ADC_MACRO->CH[ADC_LOGICAL_CHANNEL]);
    while(isConversionComplete != true);
    isConversionComplete = false;
    return resultBuff;
}

/* Trim calibration function based on the binary search algorithm */
#if (USE_TRIM_CALIBRATION_METHOD == TRIM_CAL_BINARY_SEARCH)

/*******************************************************************************
* Function Name: AdcReadChannelDataWithOffset
********************************************************************************
*
* \brief 
* Read the ADC conversion result with offset and gain trim on the target pin address.
*
* \param
* cy_en_adc_pin_address_t   address of the ADC channel.
* 
* \return
* calType   calibration type, 0- offset and 1- gain
*
* \param
* int8_t   offset trim value.
*
* \param
* int8_t   gain trim value.
*
* \return
* uint16_t  average conversion result.
*
*******************************************************************************/
static uint16_t AdcReadChannelDataWithOffset(cy_en_adc_pin_address_t channelAddress, uint8_t calType, int8_t offset, int8_t gain)
{
    cy_stc_adc_analog_calibration_conifg_t tempCalValue={0};
    
    /* set the calibration offset temporary */
    tempCalValue.offset = offset;
    
    /* if calibration type offset trim, then set the gain also */
    if (calType == ADC_CAL_TYPE_GAIN)
    {
        tempCalValue.gain = gain;
    }
    Cy_Adc_SetAltAnalogCalibrationValue(CY_ADC_MACRO, &tempCalValue);

    /* Perform averaging for more accurate result, the averaging count and shift
     * value should be like: averagingCount = 2 ^ averageShift */
    Cy_Adc_Channel_SoftwareTrigger(&CY_ADC_MACRO->CH[ADC_LOGICAL_CHANNEL]);
    while(isConversionComplete != true);
    isConversionComplete = false;
    return resultBuff;
}

/*******************************************************************************
* Function Name: AdcGetTrimOffset
********************************************************************************
*
* \brief 
* Function get a offset trim value with the help of binary search.
* 
* \return
* leftEnd   left end point for trim calibration
* 
* \return
* rightEnd   right end point for trim calibration
* 
* \return
* calType   calibration type, 0- offset and 1- gain
* 
* \return
* offsetForGain   offset trim value calculated in the procedure
*
* \param
* connectHorL   flag for the connected analog refence, true - VREF_H and false - VREF_L .
* 
* \return
* uint16_t   offset trim value & status
*
*******************************************************************************/
static uint16_t AdcGetTrimOffset(int8_t leftEnd, int8_t rightEnd, uint8_t calType, int8_t offsetForGain, uint8_t connectHorL)
{
    int8_t left = leftEnd, right = rightEnd, adcTargetRefOffset = 0;
    int8_t topmid = 0, leftDiff = 0, rightDiff = 0;
    uint16_t transitVref = 0, targetVref = 0, midCalVref = 0, leftCalVref = 0, rightCalVref = 0;
    uint16_t adcChannelId = 0, currentAdcAvgValue = 0xFFFF, traceDir = 0;
    
    /* Calculate parameter based on the connected reference */
    transitVref = connectHorL ? ADC_VFERH_OFFSET_TRANSIT_VALUE : ADC_VFERL_OFFSET_TRANSIT_VALUE;
    targetVref = connectHorL ? (ADC_VFERH_OFFSET_TRANSIT_VALUE + 1) : (ADC_VFERL_OFFSET_TRANSIT_VALUE - 1);
    adcChannelId = connectHorL ? CY_ADC_PIN_ADDRESS_VREF_H : CY_ADC_PIN_ADDRESS_VREF_L;
    
    /* Configure channel and read data */
    AdcConfigureChannel((cy_en_adc_pin_address_t)adcChannelId);
    
    /* Check for the exit condition in binary search */
    while (left < right)
    {   
        /* calculate offset at topmid */
        topmid = left + (right - left) / 2;
        
        /* if calibration type offset trim */
        if (calType == ADC_CAL_TYPE_OFFSET)
        {
            /* take the average value from adc */
            midCalVref = AdcReadChannelDataWithOffset((cy_en_adc_pin_address_t)adcChannelId, calType, topmid, 0);

            /* calculate offset at leftmi side, take the average value from adc */
            leftCalVref = AdcReadChannelDataWithOffset((cy_en_adc_pin_address_t)adcChannelId, calType, left, 0);

            /* calculate offset at rightmid side, take the average value from adc */
            rightCalVref = AdcReadChannelDataWithOffset((cy_en_adc_pin_address_t)adcChannelId, calType, right, 0);
        }
        /* if calibration type gain trim */
        else
        {
            /* take the average value from adc */
            midCalVref = AdcReadChannelDataWithOffset((cy_en_adc_pin_address_t)adcChannelId, calType, offsetForGain, topmid);

            /* calculate offset at leftmi side, take the average value from adc */
            leftCalVref = AdcReadChannelDataWithOffset((cy_en_adc_pin_address_t)adcChannelId, calType, offsetForGain, left);

            /* calculate offset at rightmid side, take the average value from adc */
            rightCalVref = AdcReadChannelDataWithOffset((cy_en_adc_pin_address_t)adcChannelId, calType, offsetForGain, right);
        }
        
        /* Print points */
        AdcPrintLog("\n\r points\t\t %d\t : %d\t : %d", left, topmid, right);
        AdcPrintLog("\n\r values\t\t %d\t : %d\t : %d", leftCalVref, midCalVref, rightCalVref);
        
        /* Check if the theww values are in order to minimize the noise, this can be removed for optimized system */
        if (((leftCalVref >= midCalVref) && (midCalVref >= rightCalVref)) || ((leftCalVref <= midCalVref) && (midCalVref <= rightCalVref)))
        {
            /* Check if transit reference is achieved on any of three points */
            if ((leftCalVref == transitVref) || (midCalVref == transitVref) || (rightCalVref == transitVref))
            {
                /* Check if target vref is achieved at topmid value */
                if (midCalVref == transitVref)
                {
                    currentAdcAvgValue = midCalVref;
                    adcTargetRefOffset = topmid;
                    if(connectHorL)
                    {
                        /* if left side value is greater (left 4095 <- 4094 mid), then calibration should follow left side */
                        traceDir = (leftCalVref > rightCalVref) ? 0 : 1;
                    }
                    else
                    {
                        /* if left side value is smaller (left 0 <- 1 mid), then calibration should follow left side */
                        traceDir = (leftCalVref < rightCalVref) ? 0 : 1;
                    }
                    break;
                }
                /* Check if target vref is achieved at leftmid value */
                if (leftCalVref == transitVref)
                {
                    currentAdcAvgValue = leftCalVref;
                    adcTargetRefOffset = left;
                    if(connectHorL)
                    {
                        /* if left side value is greater (left 4095 <- 4094 mid), then calibration should follow left side */
                        traceDir = (leftCalVref > midCalVref) ? 0 : 1;
                    }
                    else
                    {
                        /* if left side value is smaller (left 0 <- 1 mid), then calibration should follow left side */
                        traceDir = (leftCalVref < midCalVref) ? 0 : 1;
                    }
                    break;
                }
                /* Check if target vref is achieved at righttmid value */
                if (rightCalVref == transitVref)
                {
                    currentAdcAvgValue = rightCalVref;
                    adcTargetRefOffset = right;
                    if(connectHorL)
                    {
                        /* if right side value is smaller (mid 4095 <- 4094 right), then calibration should follow left side */
                        traceDir = (rightCalVref < midCalVref) ? 0 : 1;
                    }
                    else
                    {
                        /* if right side value is greater (mid 0 <- 1 right), then calibration should follow right side */
                        traceDir = (rightCalVref > midCalVref) ? 0 : 1;
                    }
                    break;
                }
            }
            else
            {
                /* get the difference from both end */
                leftDiff   = (transitVref >= leftCalVref) ? (transitVref - leftCalVref) : (leftCalVref - transitVref);
                rightDiff  = (transitVref >= rightCalVref) ? (transitVref - rightCalVref) : (rightCalVref - transitVref);
                
                /* Check if the serch is for VREF_H */
                if(connectHorL)
                {
                    /* if left side value is lower, then calibration should follow left side */
                    if (leftDiff < rightDiff)
                    {
                        if ((midCalVref > transitVref) && (midCalVref == leftCalVref))
                        {
                            left = topmid+1;
                            traceDir = 1;
                            AdcPrintLog("\n\r going right...\n\r");
                        }
                        else
                        {
                            right = topmid-1;
                            traceDir = 0;
                            AdcPrintLog("\n\r going left...\n\r");
                        }
                    }
                    else if (rightDiff < leftDiff)
                    {
                        if ((midCalVref > transitVref) && (midCalVref == rightCalVref))
                        {
                            right = topmid-1;
                            traceDir = 0;
                            AdcPrintLog("\n\r going left...\n\r");
                        }
                        else
                        {
                            left = topmid+1;
                            traceDir = 1;
                            AdcPrintLog("\n\r going right...\n\r");
                        }
                    }
                    else
                    {
                        left = left+1;
                        right = right-1;
                        AdcPrintLog("\n\r");
                    }
                }
                /* Check if the serch is for VREF_L */
                else
                {
                    /* if left side value is lower, then calibration should follow left side */
                    if (leftDiff < rightDiff)
                    {
                        if ((midCalVref < transitVref) && (midCalVref == leftCalVref))
                        {
                            left = topmid+1;
                            traceDir = 1;
                            AdcPrintLog("\n\r going right...\n\r");
                        }
                        else
                        {
                            right = topmid-1;
                            traceDir = 0;
                            AdcPrintLog("\n\r going left...\n\r");
                        }
                    }
                    else if (rightDiff < leftDiff)
                    {
                        if ((midCalVref < transitVref) && (midCalVref == rightCalVref))
                        {
                            right = topmid-1;
                            traceDir = 0;
                            AdcPrintLog("\n\r going left...\n\r");
                        }
                        else
                        {
                            left = topmid+1;
                            traceDir = 1;
                            AdcPrintLog("\n\r going right...\n\r");
                        }
                    }
                    else
                    {
                        left = left+1;
                        right = right-1;
                        AdcPrintLog("\n\r");
                    }
                }
            }
        }
        else
        {
            left = left+1;
            right = right-1;
            AdcPrintLog("\n\r invalid sample values...");
        }
    }

    if(currentAdcAvgValue != 0xFFFF)
    {
        /* Now check the transit point 1->0 or 4094->4095 */
        AdcPrintLog("\n\n\r searching for target vref...");
        
        /* if trace direction is right check in right by incrementing offset by 1 */
        if(traceDir)
        {
            AdcPrintLog("\n\r going right...\n\r");
            
            /* Currently checking upto ADC_LINEAR_CAL_ITERATION_VALUE points, increase if needed */
            for(uint8_t i=0; i<ADC_LINEAR_CAL_ITERATION_VALUE; i++)
            {
                /* increment the offset value */
                adcTargetRefOffset += 1;
                
                /* if calibration type offset trim */
                if (calType == ADC_CAL_TYPE_OFFSET)
                {
                    rightCalVref = AdcReadChannelDataWithOffset((cy_en_adc_pin_address_t)adcChannelId, calType, adcTargetRefOffset, 0);
                }
                /* if calibration type gain trim */
                else
                {
                    rightCalVref = AdcReadChannelDataWithOffset((cy_en_adc_pin_address_t)adcChannelId, calType, offsetForGain, adcTargetRefOffset);
                }
                
                /* If target vref found, then return the value */
                if(rightCalVref == targetVref)
                {
                    currentAdcAvgValue = (uint8_t)adcTargetRefOffset; 
                    AdcPrintLog("\n\r Found Target Vref  : %d", rightCalVref);
                    AdcPrintLog("\n\r Found Target Value : %d\n\r", adcTargetRefOffset);
                    break;
                }
                else if(adcTargetRefOffset == ADC_CAL_OFFSET_MAX_VALUE)
                {
                    AdcPrintLog("\n\r Error : Target value not found !!\n\r");
                }
                else if(i == (ADC_LINEAR_CAL_ITERATION_VALUE-1))
                {
                    AdcPrintLog("\n\r Error : Linear search is not enough !!\n\r");
                }
            }
        }
        /* if trace direction is left check in left by decreasing offset by 1 */
        else
        {
            AdcPrintLog("\n\r going left...\n\r");
            
            /* Currently checking upto ADC_LINEAR_CAL_ITERATION_VALUE points, increase if needed */
            for(uint8_t i=0; i<ADC_LINEAR_CAL_ITERATION_VALUE; i++)
            {
                /* decrement the offset value */
                adcTargetRefOffset -= 1;
                
                /* if calibration type offset trim */
                if (calType == ADC_CAL_TYPE_OFFSET)
                {
                    leftCalVref = AdcReadChannelDataWithOffset((cy_en_adc_pin_address_t)adcChannelId, calType, adcTargetRefOffset, 0);
                }
                /* if calibration type gain trim */
                else
                {
                    leftCalVref = AdcReadChannelDataWithOffset((cy_en_adc_pin_address_t)adcChannelId, calType, offsetForGain, adcTargetRefOffset);
                }
                    
                /* If target vref found, then return the value */
                if(leftCalVref == targetVref)
                {
                    currentAdcAvgValue = (uint8_t)adcTargetRefOffset;
                    AdcPrintLog("\n\r Found Target Vref  : %d", leftCalVref);
                    AdcPrintLog("\n\r Found Target Value : %d\n\r", adcTargetRefOffset);
                    break;
                }
                else if(adcTargetRefOffset == ADC_CAL_OFFSET_MIN_VALUE)
                {
                    AdcPrintLog("\n\r Error : Target value not found !!\n\r");
                }
                else if(i == (ADC_LINEAR_CAL_ITERATION_VALUE-1)) 
                {
                    AdcPrintLog("\n\r Error : Linear search is not enough !!\n\r");
                }
            }
        }
    }
    else
    {
        AdcPrintLog("\n\r Error : Target value not found !!\n\r");
    }
    
    return currentAdcAvgValue;
}

/*******************************************************************************
* Function Name: AdcCalculateSoftTrim
********************************************************************************
*
* \brief 
* Function uses a procedure the calculte the soft trim for ADC instance.
*
* \param
* stc_PASS_SAR_t*  pointer to the base address of SAR instance.
*
* \return
* cy_en_adc_status_t status of the applied trim
*
*******************************************************************************/
static cy_en_adc_status_t AdcCalculateSoftTrim(volatile stc_PASS_SAR_t * base)
{
    bool overallCalibration = true;
    int8_t vreflGain = 0, finalGain = 0;
    int8_t vreflOffset = 0, vrefhOffset = 0, finalOffset = 0;
    uint16_t retStatus = 0xFFFF;
    cy_stc_adc_analog_calibration_conifg_t tempCalValue = {0};
    cy_stc_adc_analog_calibration_conifg_t analogCalValue = {0};
    
    /* Get the ADC calibration value from trim */ 
    AdcPrintLog("\n\r -------------------------------------------------------\n\r");
    AdcPrintLog(" ADC offset calibration ...");
    AdcPrintLog("\n\r -------------------------------------------------------\n\r");
    
    //Cy_Adc_GetAnalogCalibrationValue(base, &analogCalValue);
    analogCalValue.offset = ADC_CAL_OFFSET_MAX_VALUE;
    analogCalValue.gain = ADC_CAL_GAIN_MAX_VALUE;
    
    AdcPrintLog("\n Starting Offset value\t: %d \n\r", analogCalValue.offset);
    AdcPrintLog(" Starting Gain value\t: %d \n\r", analogCalValue.gain);
    
    /***************************************************************************
     * Calibrate the offset value for SAR
     **************************************************************************/
    
    /* Configure the ADC channel to VREFL */
    AdcPrintLog("\n\r Configuring VREF_L channel");
    AdcPrintLog("\n\r ----------------------------------------\n\r");
    
    /* Perform offset calibration operation on Vref_L */
    retStatus = AdcGetTrimOffset(ADC_CAL_OFFSET_MIN_VALUE, ADC_CAL_OFFSET_MAX_VALUE, ADC_CAL_TYPE_OFFSET, 0, false);
    
    /* If calibration is done update the vreflOffset value */
    if(retStatus != 0xFFFF)
    {
        vreflOffset = (int8_t) retStatus;
        AdcPrintLog(" First step calibration done.");
        AdcPrintLog(" vreflOffset : %d \n\r", vreflOffset);
    }
    else
    {
        vreflOffset = 0xFF;
        overallCalibration = false;
        AdcPrintLog(" Error : Not able to find the calibration value for vreflOffset. \n\r");
    }
    AdcPrintLog("\n\r ----------------------------------------\n\r");
    
    /* Configure the ADC channel to VREFH */
    AdcPrintLog("\n Configuring VREF_H channel");
    AdcPrintLog("\n\r ----------------------------------------\n\r");
       
    /* Perform offset calibration operation on Vref_H */
    retStatus = AdcGetTrimOffset(ADC_CAL_OFFSET_MIN_VALUE, ADC_CAL_OFFSET_MAX_VALUE, ADC_CAL_TYPE_OFFSET, 0, true);

    /* If calibration is done update the vrefhOffset value */
    if(retStatus != 0xFFFF)
    {
        vrefhOffset = (int8_t) retStatus;
        AdcPrintLog(" Second step calibration done.");
        AdcPrintLog(" vrefhOffset : %d \n\r", vrefhOffset);
    }
    else
    {
        vrefhOffset = 0xFF;
        overallCalibration = false;
        AdcPrintLog(" Error : Not able to find the calibration value for vrefhOffset. \n\r");
    }
    AdcPrintLog("\n\r ----------------------------------------\n\r");
    
    /* Calculate overall offset value */
    if(overallCalibration == true)
    {
        finalOffset = (int8_t)((vreflOffset + vrefhOffset) >> 1) + 2;
        AdcPrintLog("\n\r final offset value A : %d \n\r", finalOffset);

        if(floor(finalOffset) > 125)
        {
            overallCalibration = false; 
            AdcPrintLog(" Error : Calculated calibration value is out of range for vrefhOffset. \n\r");
        }
    }

    /***************************************************************************
     * Calibrate the Gain value for SAR
     **************************************************************************/

    AdcPrintLog("\n\r -------------------------------------------------------\n\r");
    AdcPrintLog(" ADC gain calibration ...");
    AdcPrintLog("\n\r -------------------------------------------------------\n\r");
    
    /* Configure the ADC channel to VREFL */
    AdcPrintLog("\n Configuring VREF_L channel");
    AdcPrintLog("\n\r ----------------------------------------\n\r");
    
    /* Update the final offset value for gain calibration */
    tempCalValue.offset = finalOffset;
    AdcPrintLog("\n Set offset value : %d \n\r", tempCalValue.offset);
    
    /* Perform gain calibration operation on Vref_L */
    retStatus = AdcGetTrimOffset(ADC_CAL_GAIN_MIN_VALUE, ADC_CAL_GAIN_MAX_VALUE, ADC_CAL_TYPE_GAIN, tempCalValue.offset, false);
    
    /* If calibration is done update the vreflGain value */
    if(retStatus != 0xFFFF)
    {
        //vreflGain = (int8_t) retStatus & 0x001F;
        vreflGain = (int8_t) retStatus;
        AdcPrintLog(" Third step calibration done.");
        AdcPrintLog(" vreflGain : %d \n\r", vreflGain);

        if(vreflGain == -14)
        {
            overallCalibration = false; 
            AdcPrintLog(" Error : Calculated gain value is out of range for vreflOffset. \n\r");
        }
    }
    else
    {
        overallCalibration = false; 
        AdcPrintLog(" Error : Not able to find the calibration value for vreflGain. \n\r");
    }
    AdcPrintLog("\n\r ----------------------------------------\n\r");
    
    /* Calculate overall gain value */
    if(overallCalibration == true)
    {
        //finalGain = (vreflGain - 1) & 0x001F;
        finalGain = (vreflGain - 1);
        AdcPrintLog("\n\r final gain value B : %d \n\r", finalGain);
    }
    
    /***************************************************************************
     * Update the calibration register for SAR
     **************************************************************************/

    if(overallCalibration == true)
    {
        AdcPrintLog("\n\r -------------------------------------------------------\n\r");
        AdcPrintLog(" ADC calibration update ...");
        AdcPrintLog("\n\r -------------------------------------------------------\n\r");
        
        /* Update final gain and offset to the alternate calibration register */
        analogCalValue.offset = finalOffset;
        analogCalValue.gain = finalGain;
        Cy_Adc_SetAltAnalogCalibrationValue(base, &analogCalValue);
        
        AdcPrintLog("\n\r Updating ADC calibration register...\n\r");
        
        /* Trigger calibration for coherent update */
        Cy_Adc_TriggerCalibrationUpdate(base);
        
        /* Check if update done */
        if(Cy_Adc_IsCalibrationUpdateDone(base))
        {
            /* Get and check for the proper update to calibration register */
            Cy_Adc_GetAnalogCalibrationValue(base, &analogCalValue);
            /* Check if it is a 5-bit -ve value, adjust it for 8-bit signed value */
            if (analogCalValue.gain & 0x10)
            {
                analogCalValue.gain = 0xF0 | analogCalValue.gain;
            }
            CY_ASSERT_L1(analogCalValue.offset == finalOffset);
            CY_ASSERT_L1(analogCalValue.gain == finalGain);
            
            AdcPrintLog("\n ADC calibration offset\t: %d \n\r", analogCalValue.offset);
            AdcPrintLog(" ADC calibration gain\t: %d \n\r", analogCalValue.gain);
            AdcPrintLog("\n ADC Calibration Done !!\n\r");
            
            return CY_ADC_SUCCESS;
        }
        else
        {
            AdcPrintLog("\n Error : Not able to update calibration register. \n\r");
            return CY_ADC_BAD_PARAM;
        }
    }
    else
    {
        AdcPrintLog("\n Error : Failed to find calibration values. \n\r");
        AdcPrintLog("\n ADC Calibration Failed !!\n\r");
        return CY_ADC_BAD_PARAM;
    }
}

/* Trim calibration function based on the linear search algorithm */
#else   /* (USE_TRIM_CALIBRATION_METHOD == TRIM_CAL_LINEAR_SEARCH) */

/*******************************************************************************
* Function Name: AdcCalculateSoftTrim
********************************************************************************
*
* \brief 
* Function uses a procedure the calculte the soft trim for ADC instance.
*
* \param
* stc_PASS_SAR_t*  pointer to the base address of SAR instance.
*
* \return
* cy_en_adc_status_t status of the applied trim
*
*******************************************************************************/
static cy_en_adc_status_t AdcCalculateSoftTrim(volatile stc_PASS_SAR_t * base)
{
    bool isCalibrationDone=false, overallCalibration=true;
    int8_t vreflGain=0, finalGain=0;
    int8_t vreflOffset=0, vrefhOffset=0, finalOffset=0;
    int32_t currentAdcAvgValue=0xFF, previousAdcAvgValue=0xFF;
    cy_stc_adc_analog_calibration_conifg_t tempCalValue={0};
    cy_stc_adc_analog_calibration_conifg_t analogCalValue={0};
    
    /* Get the ADC calibration value from trim */ 
    AdcPrintLog("\n\r -------------------------------------------------------\n\r");
    AdcPrintLog(" ADC offset calibration ...");
    AdcPrintLog("\n\r -------------------------------------------------------\n\r");
    
    //Cy_Adc_GetAnalogCalibrationValue(base, &analogCalValue);
    analogCalValue.offset = ADC_CAL_OFFSET_MAX_VALUE;
    analogCalValue.gain = ADC_CAL_GAIN_MAX_VALUE;
    
    AdcPrintLog("\n Starting Offset value\t: %d \n\r", analogCalValue.offset);
    AdcPrintLog(" Starting Gain value\t: %d \n\r", analogCalValue.gain);
    
    /***************************************************************************
     * Calibrate the offset value for SAR
     **************************************************************************/
    
    /* Configure the ADC channel to VREFL */
    AdcPrintLog("\n\r Configuring VREF_L channel");
    AdcPrintLog("\n\r ----------------------------------------\n\r");
    
    /* Perform offset calibration operation on Vref_L */
    for(int16_t offset=ADC_CAL_OFFSET_MAX_VALUE; offset>=ADC_CAL_OFFSET_MIN_VALUE; offset--)
    {
        /* Update the offset value in ALT_CAL */
        tempCalValue.offset = offset;
        Cy_Adc_SetAltAnalogCalibrationValue(base, &tempCalValue);
        AdcPrintLog("\n\r Set offset value : %d \n\r", tempCalValue.offset);
        
        isConversionComplete = false;
        currentAdcAvgValue = 0;
        
        /* take the average value from adc */
        currentAdcAvgValue = AdcReadChannelData(CY_ADC_PIN_ADDRESS_VREF_L);
        AdcPrintLog(" Current Avg Value : %d \n\r", currentAdcAvgValue);
        
        /* check for the ADC reading changes from 0x001 -> 0x000 */
        if((currentAdcAvgValue < ADC_VFERL_OFFSET_TRANSIT_VALUE) && (previousAdcAvgValue == ADC_VFERL_OFFSET_TRANSIT_VALUE) && (previousAdcAvgValue != 0x00))
        {
            isCalibrationDone = true;
            break;
        }
        else
        {
            previousAdcAvgValue = currentAdcAvgValue;
        }
    }
    
    /* If calibration is done update the vreflOffset value */
    if(isCalibrationDone)
    {
        vreflOffset = (int8_t) tempCalValue.offset;
        AdcPrintLog(" First step calibration done.");
        AdcPrintLog(" vreflOffset : %d \n\r", vreflOffset);
    }
    else
    {
        vreflOffset = 0xFF;
        overallCalibration = false;
        AdcPrintLog(" Error : Not able to find the calibration value for vreflOffset. \n\r");
    }
    AdcPrintLog("\n\r ----------------------------------------\n\r");
    
    /* Configure the ADC channel to VREFH */
    AdcPrintLog("\n Configuring VREF_H channel");
    AdcPrintLog("\n\r ----------------------------------------\n\r");
       
    /* Perform offset calibration operation on Vref_H */
    for(int16_t offset=ADC_CAL_OFFSET_MIN_VALUE; offset<=ADC_CAL_OFFSET_MAX_VALUE; offset++)
    {
        /* Update the offset value in ALT_CAL */
        tempCalValue.offset = offset;
        Cy_Adc_SetAltAnalogCalibrationValue(base, &tempCalValue);
        AdcPrintLog("\n\r Set offset value : %d \n\r", tempCalValue.offset);
        
        isConversionComplete = false;
        currentAdcAvgValue = 0;
        
        /* take the average value from adc */
        currentAdcAvgValue = AdcReadChannelData(CY_ADC_PIN_ADDRESS_VREF_H);
        AdcPrintLog("Current Avg Value : %d \n\r", currentAdcAvgValue);
        
        /* check for the ADC reading changes from 0xFFE -> 0xFFF */
        if((currentAdcAvgValue > ADC_VFERH_OFFSET_TRANSIT_VALUE) && (previousAdcAvgValue == ADC_VFERH_OFFSET_TRANSIT_VALUE))
        {         
            isCalibrationDone = true;
            break;
        }
        else
        {
            previousAdcAvgValue = currentAdcAvgValue;
        }
    }

    /* If calibration is done update the vrefhOffset value */
    if(isCalibrationDone)
    {
        vrefhOffset = (int8_t) tempCalValue.offset;
        AdcPrintLog(" Second step calibration done.");
        AdcPrintLog(" vrefhOffset : %d \n\r", vrefhOffset);
    }
    else
    {
        vrefhOffset = 0xFF;
        overallCalibration = false;
        AdcPrintLog(" Error : Not able to find the calibration value for vrefhOffset. \n\r");
    }
    AdcPrintLog("\n\r ----------------------------------------\n\r");
    
    /* Calculate overall offset value */
    if(overallCalibration == true)
    {
        finalOffset = (int8_t)((vreflOffset + vrefhOffset) >> 1) + 2;
        AdcPrintLog("\n\r final offset value A : %d \n\r", finalOffset);
    }

    /***************************************************************************
     * Calibrate the Gain value for SAR
     **************************************************************************/

    AdcPrintLog("\n\r -------------------------------------------------------\n\r");
    AdcPrintLog(" ADC gain calibration ...");
    AdcPrintLog("\n\r -------------------------------------------------------\n\r");
    
    /* Configure the ADC channel to VREFL */
    AdcPrintLog("\n Configuring VREF_L channel");
    AdcPrintLog("\n\r ----------------------------------------\n\r");
    
    /* Perform gain calibration operation on Vref_L */
    isConversionComplete = false;
    tempCalValue.offset = finalOffset;
    AdcPrintLog("\n Set offset value : %d \n\r", tempCalValue.offset);
        
    for(int8_t gain=ADC_CAL_GAIN_MAX_VALUE; gain>=ADC_CAL_GAIN_MIN_VALUE; gain--)
    {
        /* Update the gain value in ALT_CAL */
        tempCalValue.gain = gain;
        Cy_Adc_SetAltAnalogCalibrationValue(base, &tempCalValue);
        AdcPrintLog("\n\r Set gain value : %d\n\r", tempCalValue.gain);
        
        isConversionComplete = false;
        currentAdcAvgValue = 0;
        
        /* take the average value from adc */
        currentAdcAvgValue = AdcReadChannelData(CY_ADC_PIN_ADDRESS_VREF_L);
        AdcPrintLog(" Current Avg Value : %d\n\r", currentAdcAvgValue);
        
        /* check for the ADC reading changes from 0x001 -> 0x000 */
        if((currentAdcAvgValue < ADC_VFERL_GAIN_TRANSIT_VALUE) && (previousAdcAvgValue == ADC_VFERL_GAIN_TRANSIT_VALUE))
        {
            isCalibrationDone = true;
            break;
        }
        else
        {
            previousAdcAvgValue = currentAdcAvgValue;
        }
    }
    
    /* If calibration is done update the vreflGain value */
    if(isCalibrationDone)
    {
        //vreflGain = tempCalValue.gain & 0x001F;
        vreflGain = (int8_t) tempCalValue.gain;
        AdcPrintLog(" Third step calibration done.");
        AdcPrintLog(" vreflGain : %d \n\r", vreflGain);
    }
    else
    {
        overallCalibration = false; 
        AdcPrintLog(" Error : Not able to find the calibration value for vreflGain. \n\r");
    }
    AdcPrintLog("\n\r ----------------------------------------\n\r");
    
    /* Calculate overall gain value */
    if(overallCalibration == true)
    {
        //finalGain = (vreflGain - 1) & 0x001F;
        finalGain = (vreflGain - 1);
        AdcPrintLog("\n\r final gain value B : %d \n\r", finalGain);
    }
    
    /***************************************************************************
     * Update the calibration register for SAR
     **************************************************************************/

    if(overallCalibration == true)
    {
        AdcPrintLog("\n\r -------------------------------------------------------\n\r");
        AdcPrintLog(" ADC calibration update ...");
        AdcPrintLog("\n\r -------------------------------------------------------\n\r");
        
        /* Update final gain and offset to the alternate calibration register */
        analogCalValue.offset = finalOffset;
        analogCalValue.gain = finalGain;
        Cy_Adc_SetAltAnalogCalibrationValue(base, &analogCalValue);
        
        AdcPrintLog("\n\r Updating ADC calibration register...\n\r");
        
        /* Trigger calibration for coherent update */
        Cy_Adc_TriggerCalibrationUpdate(base);
        
        /* Check if update done */
        if(Cy_Adc_IsCalibrationUpdateDone(base))
        {
            /* Get and check for the proper update to calibration register */
            Cy_Adc_GetAnalogCalibrationValue(base, &analogCalValue);
            /* Check if it is a 5-bit -ve value, adjust it for 8-bit signed value */
            if (analogCalValue.gain & 0x10)
            {
                analogCalValue.gain = 0xF0 | analogCalValue.gain;
            }
            CY_ASSERT_L1(analogCalValue.offset == finalOffset);
            CY_ASSERT_L1(analogCalValue.gain == finalGain);
            
            AdcPrintLog("\n ADC calibration offset\t: %d \n\r", analogCalValue.offset);
            AdcPrintLog(" ADC calibration gain\t: %d \n\r", analogCalValue.gain);
            AdcPrintLog("\n ADC Calibration Done !!\n\r");
            
            return CY_ADC_SUCCESS;
        }
        else
        {
            AdcPrintLog("\n Error : Not able to update calibration register. \n\r");
            return CY_ADC_BAD_PARAM;
        }
    }
    else
    {
        AdcPrintLog("\n Error : Failed to find calibration values. \n\r");
        AdcPrintLog("\n ADC Calibration Failed !!\n\r");
        return CY_ADC_BAD_PARAM;
    }
}

#endif  /* (USE_TRIM_CALIBRATION_METHOD == TRIM_CAL_BINARY_SEARCH) */ 

/*******************************************************************************
* Function Name: AdcCheckSoftTrim
********************************************************************************
*
* \brief 
* Read and check the VrefL and VrefH after appling the trim values.
*
* \param
* none
*
* \return
* none
*
*******************************************************************************/
static void AdcCheckSoftTrim(void)
{
    float currentAdcAvgValue=0;
    
    AdcPrintLog("\n\r -------------------------------------------------------\n\r");
    AdcPrintLog(" ADC calibration test ...");
    AdcPrintLog("\n\r -------------------------------------------------------\n\r");
    
    /* Use updated calibration parameter */
    adcChannelConfig.calibrationValueSelect = CY_ADC_CALIBRATION_VALUE_REGULAR;
    
    /* Configure VREF_L channel and measure */
    AdcPrintLog("\n Configuring VREF_L channel");
    isConversionComplete = false;
    
    /* Check for measurement result */
    currentAdcAvgValue = AdcReadChannelData(CY_ADC_PIN_ADDRESS_VREF_L);
    AdcPrintLog("\n\r Current Avg Value L : %f \n\r", currentAdcAvgValue);
    
    /* Configure VREF_H channel and measure */
    AdcPrintLog("\n Configuring VREF_H channel");
    isConversionComplete = false;
    currentAdcAvgValue = 0;
      
    /* Check for measurement result */
    currentAdcAvgValue = AdcReadChannelData(CY_ADC_PIN_ADDRESS_VREF_H);
    AdcPrintLog("\n\r Current Avg Value H : %f \n\r", currentAdcAvgValue);
    AdcPrintLog("\n\r ADC Calibration Test Done !!\n\r");
}


/*******************************************************************************
* Function Name: main
********************************************************************************
*
* \brief 
* Example to demonstrate functions of ADC temperature measurements.
* 
* \param 
* None.
*
* \return
* int
*
* \note 
* None.
*******************************************************************************/
int main(void)
{
    cy_stc_adc_temp_ref_t tempRefValue;
    cy_stc_adc_temp_raw_t tempRawValue;

    SystemInit();

    /* Enable global interrupts. */
    __enable_irq();

    /* ADC clock divider setting */
    Cy_SysClk_PeriphAssignDivider(CY_ADC_PCLK, CY_SYSCLK_DIV_16_BIT, 0u);
    Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(CY_ADC_PCLK), CY_SYSCLK_DIV_16_BIT, 0u, 5u);
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(CY_ADC_PCLK), CY_SYSCLK_DIV_16_BIT, 0u);

    /* Initializing console uart interface */
    Cy_Semihosting_InitAll(CY_USB_SCB_UART_TYPE, 115200, NULL, false);

    /* Initializing timer measurement channel */
#if defined ENABLE_MEAS_TIME
    Cy_MeasTime_Init();
#endif

    /* Print the hardware setup information */
    AdcPrintLog(" -------------------------------------------------------\n\r");
    AdcPrintLog(" -            Traveo-II ADC Die Temperature            -\n\r");
    AdcPrintLog(" -------------------------------------------------------\n\r");
    AdcPrintLog(" Device\t\t: CYTVII-B-H-8M-%d\n\r", CY_GPIO_PIN_COUNT);
    AdcPrintLog(" Silicon ID\t: 0x%X\n\r", CY_SILICON_ID);

#if (USE_TEMPERATURE_TRIM_VALUES == USE_TRIM_FOR_3P3V_VDDA)
    AdcPrintLog(" VDDA\t\t: 3.3V \n\r");
#else    
    AdcPrintLog(" VDDA\t\t: 5V \n\r");
#endif    
    
    /* Initialize ADC */
    Cy_Adc_DeInit(PASS0_SAR1);
    //Cy_Adc_DeInit(PASS0_SAR2);                                                    // Note: Not avilable in package
    Cy_Adc_Init(CY_ADC_MACRO, &adcConfig);

    /* Register ADC interrupt handler and enable interrupt */
    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, AdcIntHandler);
    NVIC_SetPriority(irq_cfg.intIdx, 0ul);
    NVIC_EnableIRQ(irq_cfg.intIdx);

    /* Update the sort temperature value matrix A */
    tempRefValue.adcTempValues.coldValue = -(TEMP_TRIM_TEMP_COLDSORT / 10.0);       // Note: Temperature data read from sFLASH is multiple of 10.
    tempRefValue.adcTempValues.roomValue = (TEMP_TRIM_TEMP_ROOMSORT / 10.0);        // Note: Temperature data read from sFLASH is multiple of 10.
    tempRefValue.adcTempValues.hotValue = (TEMP_TRIM_TEMP_HOTCLASS / 10.0);         // Note: Temperature data read from sFLASH is multiple of 10.

    /* Update the sort temperature adc value matrix b */
    tempRefValue.adcDiodeValues.coldValue = (uint16_t)TEMP_TRIM_DIODE_COLDSORT;
    tempRefValue.adcDiodeValues.roomValue = (uint16_t)TEMP_TRIM_DIODE_ROOMSORT;
    tempRefValue.adcDiodeValues.hotValue = (uint16_t)TEMP_TRIM_DIODE_HOTCLASS;

    /* Update the reference Vbg values */
    tempRefValue.adcVbgValues.coldValue = (uint16_t)TEMP_TRIM_VBG_COLDSORT;         // Note: All the Vbg trim values are almost same.
    tempRefValue.adcVbgValues.roomValue = (uint16_t)TEMP_TRIM_VBG_ROOMSORT;
    tempRefValue.adcVbgValues.hotValue = (uint16_t)TEMP_TRIM_VBG_HOTCLASS;

    AdcPrintLog(" \n\r Type\t|\tTemp\t|\tVbe\t|\tVbg\n\r");
    AdcPrintLog(" -------------------------------------------------------\n\r");
    AdcPrintLog(" Cold\t| %f\t|", tempRefValue.adcTempValues.coldValue);
    AdcPrintLog(" %f\t|", tempRefValue.adcDiodeValues.coldValue);
    AdcPrintLog(" %f\n\r", tempRefValue.adcVbgValues.coldValue);
    AdcPrintLog(" Room\t| %f\t|", tempRefValue.adcTempValues.roomValue);
    AdcPrintLog(" %f\t|", tempRefValue.adcDiodeValues.roomValue);
    AdcPrintLog(" %f\n\r", tempRefValue.adcVbgValues.roomValue);
    AdcPrintLog(" Hot\t| %f\t|", tempRefValue.adcTempValues.hotValue);
    AdcPrintLog(" %f\t|", tempRefValue.adcDiodeValues.hotValue);
    AdcPrintLog(" %f\n\n\r", tempRefValue.adcVbgValues.hotValue);

    /* This block needs to be called every time when reading the die temperature */
    {
        /* SoftTrim API */
        AdcCalculateSoftTrim(CY_ADC_MACRO);
        
        /* Test with the updatded calibration values */
        AdcCheckSoftTrim();
        
        AdcPrintLog("\n\r -------------------------------------------------------\n\r");
        AdcPrintLog(" Measurement die-temperature ...");
        AdcPrintLog("\n\r -------------------------------------------------------\n\r");
        
        /* Set reference buffered mode on - to pump Vbg from SRSS */
        Cy_Adc_SetReferenceBufferMode(CY_ADC_MMIO_MACRO, CY_ADC_REF_BUF_MODE_ON);
        
        /* Set current configuration for temperature sensor to 1uA for better accuracy */
        CY_SET_REG32(ADC_CAL_TEMP_TEST_CTL_ADDR, ADC_CAL_TEMP_TEST_CTL_MASK);
        
        /* Read and update the raw values for VBG and Temp Sensor */
        adcChannelConfig.calibrationValueSelect = CY_ADC_CALIBRATION_VALUE_ALTERNATE;
        
        tempRawValue.adcVbgRawValue = AdcReadChannelData(CY_ADC_IN_VBG);
        tempRawValue.adcVtempRawValue = AdcReadChannelData(CY_ADC_IN_TEMP);
        AdcPrintLog("\n\r adcVbgRawValue\t\t: %d \n\r", tempRawValue.adcVbgRawValue);
        AdcPrintLog(" adcDiodeRawValue\t: %d \n\r", tempRawValue.adcVtempRawValue);
        
        /* Calculate the internal die temperature */
        AdcPrintLog("\n\r Calculating die-temperature...");
        temperatureData = Cy_Adc_CalculateDieTemperature(&tempRefValue, &tempRawValue);
        AdcPrintLog("\n\r dieTemperature\t: %f degC\n\r", temperatureData);
        AdcPrintLog("\n\r -------------------------------------------------------\n\r");
    }
    
    for(;;)
    {
        if(temperatureData == 0.0)
        {
            /* Reaching here means SFLASH doesn't have valid values */
            CY_ASSERT(false);
        }
    }
}

/* [] END OF FILE */
