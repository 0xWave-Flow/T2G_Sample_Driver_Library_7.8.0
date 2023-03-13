/***************************************************************************//**
* \file cy_fr_api_control_service.h
* \version 1.0
*
* \brief
* Prototypes for cy_fr_api_control_service.c
*
* This file contains the Prototypes for the control service of the FlexRay 
* Communication Controller.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef __CY_FR_API_CONTROL_SERVICE_H__
#define __CY_FR_API_CONTROL_SERVICE_H__


#include "flexray/api/cy_fr_api_global.h"

/**
* \addtogroup group_fru_functions
* \{
*/

/*----------------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_poc_command         ( volatile stc_FLEXRAY_t *    base,
                                                  CY_FR_POC_COMMAND_TYPE      cy_fr_cmd );
                                                  
CY_FR_RETURN_TYPE cy_fr_api_go_active           ( volatile stc_FLEXRAY_t *    base );

uint32_t          cy_fr_api_get_eray_version    ( volatile stc_FLEXRAY_t *    base,
                                                  CY_FR_CORE_RELEASE_STRUCT * cy_fr_release_struct);
                                                  
void              cy_fr_api_enable_controller   ( volatile stc_FLEXRAY_t *    base );

void              cy_fr_api_disable_controller  ( volatile stc_FLEXRAY_t *    base );

/** \} group_fru_functions */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_API_CONTROL_SERVICE_H__ */
/*---------------------------------------------------------------------------------*/


#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */
