/***************************************************************************//**
* \file cy_fr_ccal_time_service.h
* \version 1.0
*
* \brief
* Prototypes for cy_fr_ccal_time_service.c.
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


#ifndef __CY_FR_CCAL_TIME_SERVICE_H__
#define __CY_FR_CCAL_TIME_SERVICE_H__


/**
* \addtogroup group_fru_functions
* \{
*/

/*----------------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------------*/
uint16_t cy_fr_ccal_get_mtick          ( volatile stc_FLEXRAY_t * base );
uint32_t cy_fr_ccal_get_time           ( volatile stc_FLEXRAY_t * base );
uint8_t  cy_fr_ccal_get_cycle          ( volatile stc_FLEXRAY_t * base );
uint16_t cy_fr_ccal_get_slot_counter_a ( volatile stc_FLEXRAY_t * base );
uint16_t cy_fr_ccal_get_slot_counter_b ( volatile stc_FLEXRAY_t * base );

/** \} group_fru_functions */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_CCAL_TIME_SERVICE_H__ */
/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


