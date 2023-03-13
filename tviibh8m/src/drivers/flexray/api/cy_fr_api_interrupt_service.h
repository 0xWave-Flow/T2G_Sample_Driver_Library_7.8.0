/***************************************************************************//**
* \file cy_fr_api_interrupt_service.h
* \version 1.0
*
* \brief
* Prototypes for cy_fr_api_interrupt_service.c
*
* This file contains the Prototype for the interrupt service of the Cypress FlexRay 
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

#ifndef __CY_FR_API_INTERRUPT_SERVICE_H__
#define __CY_FR_API_INTERRUPT_SERVICE_H__

/**
* \addtogroup group_fru_functions
* \{
*/

/*---------------------------------------------------------------------------------
 * Type Definitions
 *--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
/** \brief      This function pointer shall be used to the specify the user callback
                function for an interrupt line.
 */
/*---------------------------------------------------------------------------------*/
typedef void (*CY_FR_USER_IRQ_CALLBACK_FUNCTION_PTR_TYPE)(volatile stc_FLEXRAY_t * base, CY_FR_INTERRUPT_TYPE cy_fr_int);


/*----------------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------------*/


CY_FR_RETURN_TYPE    cy_fr_api_interrupt_enable      ( volatile stc_FLEXRAY_t *                  base,
                                                       CY_FR_INTERRUPT_TYPE                      cy_fr_int);
                                                       
CY_FR_RETURN_TYPE    cy_fr_api_interrupt_disable     ( volatile stc_FLEXRAY_t *                  base,
                                                       CY_FR_INTERRUPT_TYPE                      cy_fr_int);
                                                       
CY_FR_RETURN_TYPE    cy_fr_api_interrupt_line_enable ( volatile stc_FLEXRAY_t *                  base,
                                                       CY_FR_INT_LINE_TYPE                       cy_fr_int_line );
                                                       
CY_FR_RETURN_TYPE    cy_fr_api_interrupt_line_disable( volatile stc_FLEXRAY_t *                  base,
                                                       CY_FR_INT_LINE_TYPE                       cy_fr_int_line );
                                                       
CY_FR_RETURN_TYPE    cy_fr_api_clear_interrupt_flag  ( volatile stc_FLEXRAY_t *                  base,
                                                       CY_FR_INTERRUPT_TYPE                      cy_fr_int);
                                                       
CY_FR_RETURN_TYPE    cy_fr_api_interrupt_line_set    ( volatile stc_FLEXRAY_t *                  base,
                                                       CY_FR_INTERRUPT_TYPE                      cy_fr_int,
                                                       CY_FR_INT_LINE_TYPE                       cy_fr_int_line );
                                                       
void                 cy_fr_api_interrupt_set_callback( volatile stc_FLEXRAY_t *                  base,
                                                       CY_FR_INT_LINE_TYPE                       cy_fr_int_line,
                                                       CY_FR_USER_IRQ_CALLBACK_FUNCTION_PTR_TYPE cy_fr_callback );
                                                       
void                 cy_fr_api_interrupt_dispatcher  ( volatile stc_FLEXRAY_t *                  base,
                                                       CY_FR_INT_LINE_TYPE                       cy_fr_int_line );

/** \} group_fru_functions */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_API_INTERRUPT_SERVICE_H__ */
/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


