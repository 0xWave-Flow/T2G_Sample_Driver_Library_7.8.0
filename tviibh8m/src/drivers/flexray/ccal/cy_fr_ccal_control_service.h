/***************************************************************************//**
* \file cy_fr_ccal_control_service.h
* \version 1.0
*
* \brief
* Prototypes for the cy_fr_ccal_control_service.c
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


#ifndef __CY_FR_CCAL_CONTROL_SERVICE_H__
#define __CY_FR_CCAL_CONTROL_SERVICE_H__


/**
* \addtogroup group_fru_functions
* \{
*/

/*---------------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_poc_command         ( volatile stc_FLEXRAY_t * base,
                                                   CY_FR_POC_COMMAND_TYPE   cy_fr_cmd );
                                                   
CY_FR_RETURN_TYPE cy_fr_ccal_unlock_config       ( volatile stc_FLEXRAY_t * base,
                                                   CY_FR_POC_COMMAND_TYPE   cy_fr_cmd );
                                                   
uint32_t          cy_fr_ccal_get_eray_version    ( volatile stc_FLEXRAY_t *    base,
                                                   CY_FR_CORE_RELEASE_STRUCT * cy_fr_release_struct);
                                                   
void              cy_fr_ccal_enable_controller   ( volatile stc_FLEXRAY_t * base );

void              cy_fr_ccal_disable_controller  ( volatile stc_FLEXRAY_t * base );

/** \} group_fru_functions */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_CCAL_CONTROL_SERVICE_H__ */
/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */

