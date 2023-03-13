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
#include "example_config.h"
#include <stdio.h>

#define BB_POTI_ANALOG_MACRO        CY_ADC_POT_MACRO
#define BB_POTI_ANALOG_PCLK         CY_ADC_POT_PCLK
#define BB_POTI_ANALOG_INPUT_NO    ((cy_en_adc_pin_address_t)CY_ADC_POT_IN_NO)

// Definition of SAR ADC AC Specified Parameters
#if VDDA_VOLTAGE == VDDA_VOLTAGE_4_5V_TO_5_5V
  #define ADC_OPERATION_FREQUENCY_MAX_IN_HZ (26670000ul)
  #define ANALOG_IN_SAMPLING_TIME_MIN_IN_NS (412ull)
#else // VDDA_VOLTAGE == VDDA_VOLTAGE_2_7V_TO_4_5V
  #define ADC_OPERATION_FREQUENCY_MAX_IN_HZ (13340000ul)
  #define ANALOG_IN_SAMPLING_TIME_MIN_IN_NS (600ull)
#endif

#define DIV_ROUND_UP(a,b) (((a) + (b)/2) / (b))

// Array indices
enum
{
    ADC_GROUP_CHANNEL_INDEX_BB_POTI = 0u, // potentiometer on base board
    ADC_GROUP_CHANNEL_INDEX_VTEMP,        // internal temperature sensor
    ADC_GROUP_CHANNEL_INDEX_VBG,          // internal bandgap reference
    ADC_GROUP_CHANNEL_INDEX_VREF_H,       // ADC reference voltage (high)
    ADC_GROUP_NUMBER_OF_CHANNELS,         // ADC channel number in the group
};

cy_en_adc_pin_address_t aenAnalogInput[ADC_GROUP_NUMBER_OF_CHANNELS] =
{
    BB_POTI_ANALOG_INPUT_NO,
    CY_ADC_PIN_ADDRESS_VTEMP,
    CY_ADC_PIN_ADDRESS_VBG,
    CY_ADC_PIN_ADDRESS_VREF_H
};

#define ADC_GROUP_FIRST_CHANNEL         ADC_LOGICAL_CHANNEL
#define ADC_GROUP_LAST_CHANNEL          (ADC_GROUP_FIRST_CHANNEL + ADC_GROUP_NUMBER_OF_CHANNELS - 1u)

uint16_t               resultBuff[ADC_GROUP_NUMBER_OF_CHANNELS][16];
cy_stc_adc_ch_status_t statusBuff[ADC_GROUP_NUMBER_OF_CHANNELS][16];
uint8_t                resultIdx;

void AdcIntHandler(void)
{
    cy_stc_adc_interrupt_source_t intrSource = { false };

    /* Get interrupt source */
    Cy_Adc_Channel_GetInterruptMaskedStatus(&BB_POTI_ANALOG_MACRO->CH[ADC_GROUP_LAST_CHANNEL], &intrSource);

    if(intrSource.grpDone)
    {
        printf("\r");

        /* Get the result(s) */
        for(uint8_t ch = ADC_GROUP_FIRST_CHANNEL; ch < (ADC_GROUP_FIRST_CHANNEL + ADC_GROUP_NUMBER_OF_CHANNELS); ch++)
        {
            uint8_t i = ch - ADC_GROUP_FIRST_CHANNEL; // i is 0-based for array indexing
            Cy_Adc_Channel_GetResult(&BB_POTI_ANALOG_MACRO->CH[ch], &resultBuff[i][resultIdx], &statusBuff[i][resultIdx]);
            printf("AN%d = %04d, ", aenAnalogInput[i], resultBuff[i][resultIdx]);
        }

        /* Increment result idx */
        resultIdx = (resultIdx + 1u) % (sizeof(resultBuff[0]) / sizeof(resultBuff[0][0]));

        /* Clear interrupt source(s) (Only last channel is required) */
        Cy_Adc_Channel_ClearInterruptStatus(&BB_POTI_ANALOG_MACRO->CH[ADC_GROUP_LAST_CHANNEL], &intrSource);

        /* Trigger next conversion */
        Cy_Adc_Channel_SoftwareTrigger(&BB_POTI_ANALOG_MACRO->CH[ADC_GROUP_FIRST_CHANNEL]);
    }
    else
    {
        // Unexpected interrupt
        CY_ASSERT(false);
    }
}

/* This is an ADC example file for following settings:
 *  - Group conversion (4 channel)
 *  - Sw trigger
 *  - handling group conversion done interrupt then get the results into the buffer
 *  - the result is displayed UART terminal on PC.
 *  - if PSVP is not used, the potentiometer on the Traveo II base board is used as analog source
 */

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

  #if (EXAMPLE_CONFIG__TARGET_CPU == EXAMPLE_CONFIG__CPU_CM7)

    Cy_Semihosting_InitAll(CY_USB_SCB_UART_TYPE, 115200ul, NULL, false);

    /* ADC port setting (Note default port setting after reset is just fine) */
    {
        cy_stc_gpio_pin_config_t adcPinConfig =
        {
            .outVal    = 0ul,
            .driveMode = CY_GPIO_DM_ANALOG,
            .hsiom     = CY_ADC_POT_PIN_MUX,
            .intEdge   = 0ul,
            .intMask   = 0ul,
            .vtrip     = 0ul,
            .slewRate  = 0ul,
            .driveSel  = 0ul,
        };
        Cy_GPIO_Pin_Init(CY_ADC_POT_PORT, CY_ADC_POT_PIN, &adcPinConfig);
    }

    /* ADC clock divider setting */
    uint32_t actualAdcOperationFreq;
    {
      #if (CY_USE_PSVP == 1)
        uint32_t periFreq = 24000000ul;
      #else
        uint32_t periFreq = 80000000ul;
      #endif
        uint32_t divNum = DIV_ROUND_UP(periFreq, ADC_OPERATION_FREQUENCY_MAX_IN_HZ);
        actualAdcOperationFreq = periFreq / divNum;
        Cy_SysClk_PeriphAssignDivider(CY_ADC_POT_PCLK, CY_SYSCLK_DIV_16_BIT, 0u);
        Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(CY_ADC_POT_PCLK), CY_SYSCLK_DIV_16_BIT, 0u, (divNum - 1ul));
        Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(CY_ADC_POT_PCLK), CY_SYSCLK_DIV_16_BIT, 0u);
    }

    /* Initialize ADC */
    {
        uint32_t samplingCycle = (uint32_t)DIV_ROUND_UP((ANALOG_IN_SAMPLING_TIME_MIN_IN_NS * (uint64_t)actualAdcOperationFreq), 1000000000ull);

        cy_stc_adc_config_t adcConfig =
        {
            .preconditionTime    = 0u,
            .powerupTime         = 0u,
            .enableIdlePowerDown = false,
            .msbStretchMode      = CY_ADC_MSB_STRETCH_MODE_1CYCLE,
            .enableHalfLsbConv   = 0u,
            .sarMuxEnable        = true,
            .adcEnable           = true,
            .sarIpEnable         = true,
        };
        cy_stc_adc_channel_config_t adcChannelConfig =
        {
            .triggerSelection          = CY_ADC_TRIGGER_OFF,
            .channelPriority           = 0u,
            .preenptionType            = CY_ADC_PREEMPTION_FINISH_RESUME,
            .isGroupEnd                = false,
            .doneLevel                 = CY_ADC_DONE_LEVEL_PULSE,
            .pinAddress                = BB_POTI_ANALOG_INPUT_NO,
            .portAddress               = CY_ADC_PORT_ADDRESS_SARMUX0,
            .extMuxSelect              = 0u,
            .extMuxEnable              = true,
            .preconditionMode          = CY_ADC_PRECONDITION_MODE_OFF,
            .overlapDiagMode           = CY_ADC_OVERLAP_DIAG_MODE_OFF,
            .sampleTime                = samplingCycle,
            .calibrationValueSelect    = CY_ADC_CALIBRATION_VALUE_REGULAR,
            .postProcessingMode        = CY_ADC_POST_PROCESSING_MODE_NONE,
            .resultAlignment           = CY_ADC_RESULT_ALIGNMENT_RIGHT,
            .signExtention             = CY_ADC_SIGN_EXTENTION_UNSIGNED,
            .averageCount              = 0u,
            .rightShift                = 0u,
            .rangeDetectionMode        = CY_ADC_RANGE_DETECTION_MODE_INSIDE_RANGE,
            .rangeDetectionLoThreshold = 0x0000u,
            .rangeDetectionHiThreshold = 0x0FFFu,
            .mask.grpDone              = false,
            .mask.grpCancelled         = false,
            .mask.grpOverflow          = false,
            .mask.chRange              = false,
            .mask.chPulse              = false,
            .mask.chOverflow           = false,
        };

        Cy_Adc_Init(BB_POTI_ANALOG_MACRO, &adcConfig);

        adcChannelConfig.isGroupEnd   = false;
        adcChannelConfig.mask.grpDone = false;
        for (uint8_t ch = ADC_GROUP_FIRST_CHANNEL; ch < (ADC_GROUP_FIRST_CHANNEL + ADC_GROUP_NUMBER_OF_CHANNELS); ch++)
        {
            uint8_t i = ch - ADC_GROUP_FIRST_CHANNEL; // i is 0-based for array indexing

            if(ch == ADC_GROUP_LAST_CHANNEL)
            {
                adcChannelConfig.isGroupEnd   = true;
                adcChannelConfig.mask.grpDone = true;
            }
            adcChannelConfig.pinAddress = aenAnalogInput[i];

            Cy_Adc_Channel_Init(&BB_POTI_ANALOG_MACRO->CH[ch], &adcChannelConfig);
        }
    }

    /* Register ADC interrupt handler and enable interrupt */
    {
        cy_stc_sysint_irq_t irq_cfg;
        irq_cfg = (cy_stc_sysint_irq_t){
            .sysIntSrc  = (cy_en_intr_t)((uint32_t)CY_ADC_POT_IRQN + ADC_GROUP_LAST_CHANNEL),
            .intIdx     = CPUIntIdx3_IRQn,
            .isEnabled  = true,
        };
        Cy_SysInt_InitIRQ(&irq_cfg);
        Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, AdcIntHandler);

        NVIC_SetPriority(irq_cfg.intIdx, 0ul);
        NVIC_EnableIRQ(irq_cfg.intIdx);
    }

    /* Enable ADC ch. */
    for (uint8_t ch = ADC_GROUP_FIRST_CHANNEL; ch < (ADC_GROUP_FIRST_CHANNEL + ADC_GROUP_NUMBER_OF_CHANNELS); ch++)
    {
        Cy_Adc_Channel_Enable(&CY_ADC_POT_MACRO->CH[ch]);
    }

    /* Issue SW trigger */
    Cy_Adc_Channel_SoftwareTrigger(&CY_ADC_POT_MACRO->CH[ADC_GROUP_FIRST_CHANNEL]);

  #endif

    for(;;)
    {
    }
}



/* [] END OF FILE */
