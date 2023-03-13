/***************************************************************************//**
* \file example_config.h
*
* \version 1.0
*
* \brief Supporting example file
*
********************************************************************************
* \copyright
* Copyright 2016-2020, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef EXAMPLE_CONFIG_H
#define EXAMPLE_CONFIG_H


#define EXAMPLE_CONFIG__CPU_CM0P    0
#define EXAMPLE_CONFIG__CPU_CM7     7

// Select which CPU shall run the example
#define EXAMPLE_CONFIG__TARGET_CPU  EXAMPLE_CONFIG__CPU_CM7

// ADC logical channel to be used
#define ADC_LOGICAL_CHANNEL         0

// VDDA Voltage
#define VDDA_VOLTAGE_4_5V_TO_5_5V   0
#define VDDA_VOLTAGE_2_7V_TO_4_5V   1
#define VDDA_VOLTAGE                VDDA_VOLTAGE_4_5V_TO_5_5V

#endif // EXAMPLE_CONFIG_H

