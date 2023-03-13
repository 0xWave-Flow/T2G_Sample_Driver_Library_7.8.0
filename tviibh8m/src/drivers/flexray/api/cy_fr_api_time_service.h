/***************************************************************************//**
* \file cy_fr_api_time_service.h
* \version 1.0
*
* \brief
* Prototypes for cy_fr_api_time_service.c
*
* This file contains the Prototypes for the time service of the Cypress FlexRay
* Driver. 
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


#ifndef __CY_FR_API_TIME_SERVICE_H__
#define __CY_FR_API_TIME_SERVICE_H__


/**
* \addtogroup group_fru_functions
* \{
*/


/*----------------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------------*/
uint16_t cy_fr_api_get_mtick        ( volatile stc_FLEXRAY_t * base );

uint32_t cy_fr_api_get_time         ( volatile stc_FLEXRAY_t * base );

uint8_t  cy_fr_api_get_cycle        ( volatile stc_FLEXRAY_t * base );

uint16_t cy_fr_api_get_slot_counter ( volatile stc_FLEXRAY_t * base,
                                      CY_FR_CHANNEL_TYPE       cy_fr_channel );

/** \} group_fru_functions */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_API_TIME_SERVICE_H__ */
/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


