/***************************************************************************//**
* \file cy_fr_ccal_init.h
* \version 1.0
*
* \brief
* Prototypes and Macros for cy_fr_ccal_init.c
*
* The macros needed for the *.chi file.
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


#ifndef __CY_FR_CCAL_INIT_H__
#define __CY_FR_CCAL_INIT_H__


#include "flexray/api/cy_fr_api_init.h"
#include "flexray/fhal/cy_fr_fhal.h"


/*----------------------------------------------------------------------------------
 * Global Variable
 *---------------------------------------------------------------------------------*/
extern uint16_t cy_fr_lut_buffer_to_id[CY_IP_MXFLEXRAY_INSTANCES][CY_FR_MAX_BUFFERS][2];

/**
* \addtogroup group_fru_functions
* \{
*/
/*----------------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_init_chi     ( volatile stc_FLEXRAY_t *            base,
                                            CY_FR_CHI_INCLUDE_FUNCTION_PTR_TYPE cy_fr_chi_inc_func_ptr );
                                        
void              cy_fr_ccal_id_to_buffer ( volatile stc_FLEXRAY_t * base );                                        

/** \} group_fru_functions */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_CCAL_INIT_H__ */
/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


