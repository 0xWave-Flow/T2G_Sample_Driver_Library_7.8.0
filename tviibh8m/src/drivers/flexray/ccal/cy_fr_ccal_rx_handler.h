/***************************************************************************//**
* \file cy_fr_ccal_rx_handler.h
* \version 1.0
*
* \brief
* Prototypes for cy_fr_ccal_rx_handler.c.
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


#ifndef __CY_FR_CCAL_RX_HANDLER_H__
#define __CY_FR_CCAL_RX_HANDLER_H__


/**
* \addtogroup group_fru_functions
* \{
*/

/*---------------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE                  cy_fr_ccal_data_from_buffer  ( volatile stc_FLEXRAY_t *            base,
                                                                  uint32_t                            cy_fr_data,
                                                                  uint8_t                             cy_fr_data_length );
                                                                  
CY_FR_RETURN_TYPE                  cy_fr_ccal_rx_handler_buffer ( volatile stc_FLEXRAY_t *            base,
                                                                  uint32_t                            cy_fr_data,
                                                                  CY_FR_RX_BUFFER_HEADER_STRUCT      *cy_fr_buffer_header,
                                                                  uint8_t                             cy_fr_data_length,
                                                                  CY_FR_BUFFER_TYPE                   cy_fr_buffer,
                                                                  CY_FR_CHANNEL_TYPE                  cy_fr_channel,
                                                                  CY_FR_RX_STATUS_MODE_TYPE           cy_fr_mode );
                                                                  
CY_FR_RETURN_TYPE                  cy_fr_ccal_rx_handler_fifo   ( volatile stc_FLEXRAY_t *            base,
                                                                  uint32_t                            cy_fr_data,
                                                                  CY_FR_RX_BUFFER_HEADER_STRUCT      *cy_fr_buffer_header,
                                                                  uint8_t                             cy_fr_data_length,
                                                                  CY_FR_BUFFER_TYPE                   cy_fr_fifo,
                                                                  CY_FR_RX_STATUS_MODE_TYPE           cy_fr_mode );
                                                                  
CY_FR_RX_STATUS_MODE_TYPE          cy_fr_ccal_new_rx_data       ( volatile stc_FLEXRAY_t *            base,
                                                                  CY_FR_BUFFER_TYPE                   cy_fr_buffer );

CY_FR_RX_STATUS_MODE_TYPE          cy_fr_ccal_new_rx_data_fifo  ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE                  cy_fr_ccal_fifo_is_not_empty ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE                  cy_fr_ccal_fifo_is_overrun   ( volatile stc_FLEXRAY_t * base );
CY_FR_RETURN_TYPE                  cy_fr_ccal_fifo_is_critical  ( volatile stc_FLEXRAY_t * base );
uint8_t                            cy_fr_ccal_get_fifo_fill     ( volatile stc_FLEXRAY_t * base );

/** \} group_fru_functions */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_CCAL_RX_HANDLER_H__ */
/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


