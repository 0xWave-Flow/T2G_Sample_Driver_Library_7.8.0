/***************************************************************************//**
* \file flexray_port_config.h
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

#ifndef FLEXRAY_PORT_CONFIG_H
#define FLEXRAY_PORT_CONFIG_H

#include "cy_project.h"
#include "cy_device_headers.h"

typedef struct
{
    volatile stc_GPIO_PRT_t* portReg;
    uint8_t pinNum;
    cy_stc_gpio_pin_config_t cfg;
}stc_pin_config;



/* FlexRay port configuration */
static const stc_pin_config flexray_pin_cfg[] =
{
    /* FLEXRAY_RXDA */
    {
        .portReg = CY_FRA_RXD_PORT, 
        .pinNum  = CY_FRA_RXD_PIN,
        {
            .outVal = 0,
            .driveMode = CY_GPIO_DM_HIGHZ,
            .hsiom = CY_FRA_RXD_PIN_MUX,
            .intEdge = 0,
            .intMask = 0,
            .vtrip = 0,
            .slewRate = 0,
            .driveSel = 0,
            .vregEn = 0,
            .ibufMode = 0,
            .vtripSel = 0,
            .vrefSel = 0,
            .vohSel = 0,
        }
    },
    /* FLEXRAY_TXDA */
    {
        .portReg = CY_FRA_TXD_PORT,
        .pinNum  = CY_FRA_TXD_PIN,
        {
            .outVal = 1,
            .driveMode = CY_GPIO_DM_STRONG,
            .hsiom = CY_FRA_TXD_PIN_MUX,
            .intEdge = 0,
            .intMask = 0,
            .vtrip = 0,
            .slewRate = 0,
            .driveSel = 0,
            .vregEn = 0,
            .ibufMode = 0,
            .vtripSel = 0,
            .vrefSel = 0,
            .vohSel = 0,
        }
    },
    /* FLEXRAY_TXENA_N */
    {
        .portReg = CY_FRA_TXEN_PORT,
        .pinNum  = CY_FRA_TXEN_PIN,
        {
            .outVal = 1,
            .driveMode = CY_GPIO_DM_STRONG,
            .hsiom = CY_FRA_TXEN_PIN_MUX,
            .intEdge = 0,
            .intMask = 0,
            .vtrip = 0,
            .slewRate = 0,
            .driveSel = 0,
            .vregEn = 0,
            .ibufMode = 0,
            .vtripSel = 0,
            .vrefSel = 0,
            .vohSel = 0,
        }
    },
    /* FLEXRAY_RXDB */
    {
        .portReg = CY_FRB_RXD_PORT, 
        .pinNum  = CY_FRB_RXD_PIN,
        {
            .outVal = 0,
            .driveMode = CY_GPIO_DM_HIGHZ,
            .hsiom = CY_FRB_RXD_PIN_MUX,
            .intEdge = 0,
            .intMask = 0,
            .vtrip = 0,
            .slewRate = 0,
            .driveSel = 0,
            .vregEn = 0,
            .ibufMode = 0,
            .vtripSel = 0,
            .vrefSel = 0,
            .vohSel = 0,
        }
    },
    /* FLEXRAY_TXDB */
    {
        .portReg = CY_FRB_TXD_PORT,
        .pinNum  = CY_FRB_TXD_PIN,
        {
            .outVal = 1,
            .driveMode = CY_GPIO_DM_STRONG,
            .hsiom = CY_FRB_TXD_PIN_MUX,
            .intEdge = 0,
            .intMask = 0,
            .vtrip = 0,
            .slewRate = 0,
            .driveSel = 0,
            .vregEn = 0,
            .ibufMode = 0,
            .vtripSel = 0,
            .vrefSel = 0,
            .vohSel = 0,
        }
    },
    /* FLEXRAY_TXENB_N */
    {
        .portReg = CY_FRB_TXEN_PORT,
        .pinNum  = CY_FRB_TXEN_PIN,
        {
            .outVal = 1,
            .driveMode = CY_GPIO_DM_STRONG,
            .hsiom = CY_FRB_TXEN_PIN_MUX,
            .intEdge = 0,
            .intMask = 0,
            .vtrip = 0,
            .slewRate = 0,
            .driveSel = 0,
            .vregEn = 0,
            .ibufMode = 0,
            .vtripSel = 0,
            .vrefSel = 0,
            .vohSel = 0,
        }
    },
};


#endif
