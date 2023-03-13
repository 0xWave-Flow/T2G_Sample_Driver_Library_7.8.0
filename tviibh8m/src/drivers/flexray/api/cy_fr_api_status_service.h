/***************************************************************************//**
* \file cy_fr_api_status_service.h
* \version 1.0
*
* \brief
* Prototypes for cy_fr_api_status_service.c
*
* This file contains the Prototypes for the status service of the Cypress FlexRay
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


#ifndef __CY_FR_API_STATUS_SERVICE_H__
#define __CY_FR_API_STATUS_SERVICE_H__


/**
* \addtogroup group_fru_functions
* \{
*/

/*----------------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------------*/
CY_FR_POC_STATUS_TYPE      cy_fr_api_get_poc_status                ( volatile stc_FLEXRAY_t * base );
CY_FR_WAKEUP_STATUS_TYPE   cy_fr_api_get_wakeup_status             ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_pocs_is_wakeup                ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_pocs_is_startup               ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_pocs_is_normal_active         ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_pocs_is_normal_passive        ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_pocs_is_config                ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_pocs_is_default_config        ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_pocs_is_halt                  ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_is_coldstart_noisy            ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_is_coldstart_abort            ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_is_coldstart_inhibit          ( volatile stc_FLEXRAY_t * base );

CY_FR_RETURN_TYPE          cy_fr_api_get_aggregated_channel_status ( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_CHANNEL_TYPE       cy_fr_channel,
                                                                     CY_FR_AGGREGATED_CHANNEL_STATUS_STRUCT *data_struct );
                                                                     
CY_FR_RETURN_TYPE          cy_fr_api_get_sync_frame_id             ( volatile stc_FLEXRAY_t * base );

uint8_t                    cy_fr_api_get_interrupt_status          ( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_INTERRUPT_TYPE     cy_fr_int);
                                                                     
CY_FR_SLOT_MODE_TYPE       cy_fr_api_get_slot_mode                 ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_is_buffer_faulty              ( volatile stc_FLEXRAY_t * base );
CY_FR_BUFFER_TYPE          cy_fr_api_get_faulty_buffer             ( volatile stc_FLEXRAY_t * base );
CY_FR_BUFFER_TYPE          cy_fr_api_get_last_tx_buffer            ( volatile stc_FLEXRAY_t * base );
CY_FR_BUFFER_TYPE          cy_fr_api_get_last_updated_buffer       ( volatile stc_FLEXRAY_t * base );

CY_FR_RETURN_TYPE          cy_fr_api_get_buffer_parity_flags       ( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_BUFFER_PARITY_STRUCT *data_struct );
                                                                     
uint8_t                    cy_fr_api_get_remaining_coldstarts      ( volatile stc_FLEXRAY_t * base );
int16_t                    cy_fr_api_get_rate_correction           ( volatile stc_FLEXRAY_t * base );
int32_t                    cy_fr_api_get_offset_correction         ( volatile stc_FLEXRAY_t * base );
uint8_t                    cy_fr_api_get_clock_correction          ( volatile stc_FLEXRAY_t * base );
uint8_t                    cy_fr_api_get_pta_counter               ( volatile stc_FLEXRAY_t * base );
CY_FR_ERROR_MODE_TYPE      cy_fr_api_get_error_mode                ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_is_freeze_indicator_set       ( volatile stc_FLEXRAY_t * base );

CY_FR_RETURN_TYPE          cy_fr_api_is_syntax_error_symbol_window ( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_CHANNEL_TYPE       cy_fr_channel );
                                                                     
CY_FR_RETURN_TYPE          cy_fr_api_is_slot_boundary_symbol_window( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_CHANNEL_TYPE       cy_fr_channel );
                                                                     
CY_FR_RETURN_TYPE          cy_fr_api_is_tx_conflict_symbol_window  ( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_CHANNEL_TYPE       cy_fr_channel );
                                                                     
CY_FR_RETURN_TYPE          cy_fr_api_is_syntax_error_nit           ( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_CHANNEL_TYPE       cy_fr_channel );
                                                                     
CY_FR_RETURN_TYPE          cy_fr_api_is_slot_boundary_nit          ( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_CHANNEL_TYPE       cy_fr_channel );
                                                                     
CY_FR_RETURN_TYPE          cy_fr_api_get_network_managment_vector  ( volatile stc_FLEXRAY_t * base,
                                                                     uint8_t                  cy_fr_nmv[] );
                                                                     
CY_FR_RETURN_TYPE          cy_fr_api_is_missing_offset_correction  ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_is_offset_correction_limit    ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_is_missing_rate_correction    ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_api_is_rate_correction_limit      ( volatile stc_FLEXRAY_t * base );

uint8_t                    cy_fr_api_get_number_of_sync_frame      ( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_CHANNEL_TYPE       cy_fr_channel, 
                                                                     CY_FR_CYCLE_TYPE         cy_fr_cycle );
                                                                     
CY_FR_RETURN_TYPE          cy_fr_api_get_sync_frame                ( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_CYCLE_TYPE         cy_fr_cycle, 
                                                                     uint8_t                  cy_fr_frame_no,
                                                                     CY_FR_SYNC_FRAME_STRUCT *cy_fr_sync_frame );
               
CY_FR_POC_STATUS_TYPE      cy_fr_api_get_last_poc_status           ( volatile stc_FLEXRAY_t * base );

CY_FR_RETURN_TYPE          cy_fr_api_is_rx_mts                     ( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_CHANNEL_TYPE       cy_fr_channel );
                                                                     
CY_FR_RETURN_TYPE          cy_fr_api_status_not_updated            ( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_CHANNEL_TYPE       cy_fr_channel );
                                                                     
CY_FR_RETURN_TYPE          cy_fr_api_find_sequence_not_finished    ( volatile stc_FLEXRAY_t * base,
                                                                     CY_FR_CHANNEL_TYPE       cy_fr_channel );
                                                                     
CY_FR_RETURN_TYPE          cy_fr_api_write_attempt_header          ( volatile stc_FLEXRAY_t * base );

/** \} group_fru_functions */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_API_STATUS_SERVIC_H__ */
/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


