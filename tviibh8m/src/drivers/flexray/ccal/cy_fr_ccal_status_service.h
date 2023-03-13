/***************************************************************************//**
* \file cy_fr_ccal_status_service.h
* \version 1.0
*
* \brief
* Prototypes for cy_fr_ccal_status_service.c.
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


#ifndef __CY_FR_CCAL_STATUS_SERVICE_H__
#define __CY_FR_CCAL_STATUS_SERVICE_H__

/**
* \addtogroup group_fru_functions
* \{
*/


/*---------------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------------*/
CY_FR_POC_STATUS_TYPE      cy_fr_ccal_get_poc_status                  ( volatile stc_FLEXRAY_t * base );
CY_FR_WAKEUP_STATUS_TYPE   cy_fr_ccal_get_wakeup_status               ( volatile stc_FLEXRAY_t * base );

CY_FR_RETURN_TYPE          cy_fr_ccal_get_error_interrupt_status      ( volatile stc_FLEXRAY_t * base,
                                                                        CY_FR_INTERRUPT_TYPE     cy_fr_int );
                                                                        
CY_FR_RETURN_TYPE          cy_fr_ccal_get_status_interrupt_status     ( volatile stc_FLEXRAY_t * base,
                                                                        CY_FR_INTERRUPT_TYPE     cy_fr_int );
                                                                        
CY_FR_RETURN_TYPE          cy_fr_ccal_get_aggregated_channel_a_status ( volatile stc_FLEXRAY_t *                 base,
                                                                        CY_FR_AGGREGATED_CHANNEL_STATUS_STRUCT * data_struct );
                                                                        
CY_FR_RETURN_TYPE          cy_fr_ccal_get_aggregated_channel_b_status ( volatile stc_FLEXRAY_t *                 base,
                                                                        CY_FR_AGGREGATED_CHANNEL_STATUS_STRUCT * data_struct );
                                                                        
CY_FR_SLOT_MODE_TYPE       cy_fr_ccal_get_slot_mode                   ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_coldstart_noisy              ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_coldstart_abort              ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_coldstart_inhibit            ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_clear_rams_active            ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_buffer_faulty                ( volatile stc_FLEXRAY_t * base );
CY_FR_BUFFER_TYPE          cy_fr_ccal_get_faulty_buffer               ( volatile stc_FLEXRAY_t * base );
CY_FR_BUFFER_TYPE          cy_fr_ccal_get_last_tx_buffer              ( volatile stc_FLEXRAY_t * base );
CY_FR_BUFFER_TYPE          cy_fr_ccal_get_last_updated_buffer         ( volatile stc_FLEXRAY_t * base );

CY_FR_RETURN_TYPE          cy_fr_ccal_get_buffer_parity_flags         ( volatile stc_FLEXRAY_t *     base,
                                                                        CY_FR_BUFFER_PARITY_STRUCT * data_struct );
                                                                        
uint8_t                    cy_fr_ccal_get_remaining_coldstarts        ( volatile stc_FLEXRAY_t * base );
int16_t                    cy_fr_ccal_get_rate_correction             ( volatile stc_FLEXRAY_t * base );
int32_t                    cy_fr_ccal_get_offset_correction           ( volatile stc_FLEXRAY_t * base );
uint8_t                    cy_fr_ccal_get_clock_correction            ( volatile stc_FLEXRAY_t * base );
uint8_t                    cy_fr_ccal_get_pta_counter                 ( volatile stc_FLEXRAY_t * base );
CY_FR_ERROR_MODE_TYPE      cy_fr_ccal_get_error_mode                  ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_freeze_indicator_set         ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_syntax_error_symbol_window_a ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_syntax_error_symbol_window_b ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_slot_boundary_symbol_window_a( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_slot_boundary_symbol_window_b( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_tx_conflict_symbol_window_a  ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_tx_conflict_symbol_window_b  ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_syntax_error_nit_a           ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_syntax_error_nit_b           ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_slot_boundary_nit_a          ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_slot_boundary_nit_b          ( volatile stc_FLEXRAY_t * base );

CY_FR_RETURN_TYPE          cy_fr_ccal_get_network_managment_vector    ( volatile stc_FLEXRAY_t * base,
                                                                        uint8_t                  cy_fr_nmv[] );
                                                                        
CY_FR_RETURN_TYPE          cy_fr_ccal_is_missing_offset_correction    ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_offset_correction_limit      ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_missing_rate_correction      ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_rate_correction_limit        ( volatile stc_FLEXRAY_t * base );
uint8_t                    cy_fr_ccal_get_number_of_even_sync_frame_a ( volatile stc_FLEXRAY_t * base );
uint8_t                    cy_fr_ccal_get_number_of_odd_sync_frame_a  ( volatile stc_FLEXRAY_t * base );
uint8_t                    cy_fr_ccal_get_number_of_even_sync_frame_b ( volatile stc_FLEXRAY_t * base );
uint8_t                    cy_fr_ccal_get_number_of_odd_sync_frame_b  ( volatile stc_FLEXRAY_t * base );

CY_FR_RETURN_TYPE          cy_fr_ccal_get_sync_frame                  ( volatile stc_FLEXRAY_t *  base,
                                                                        CY_FR_CYCLE_TYPE          cy_fr_cycle,
                                                                        uint8_t                   cy_fr_frame_no,
                                                                        CY_FR_SYNC_FRAME_STRUCT * cy_fr_sync_frame );

CY_FR_POC_STATUS_TYPE      cy_fr_ccal_get_last_poc_status             ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_rx_mts_a                     ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_is_rx_mts_b                     ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_status_not_updated_a            ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_status_not_updated_b            ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_find_sequence_not_finished_a    ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_find_sequence_not_finished_b    ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE          cy_fr_ccal_write_attempt_header            ( volatile stc_FLEXRAY_t * base );

/** \} group_fru_functions */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_CCAL_STATUS_SERVICE_H__ */
/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


