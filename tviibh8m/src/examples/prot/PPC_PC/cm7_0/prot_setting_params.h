/***************************************************************************//**
* \file prot_setting_params.h
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

#ifndef _CY_PROT_EXAMPLE_HEADERS_H_
#define _CY_PROT_EXAMPLE_HEADERS_H_

#define TP_PRIVILEGED         (1)              /* privileged */
#define TP_SECURE             (0)              /* non secure */
#define TP_PERMITTED_PC       (CY_PROT_PC6)    /* context 6 */
#define TP_PROHIBITED_PC      (CY_PROT_PC5)    /* context 5 */
#define TP_PERMITTED_PC_MASK  (1 << (TP_PERMITTED_PC-1u))
#define TP_PROHIBITED_PC_MASK (1 << (TP_PROHIBITED_PC-1u))

#endif

