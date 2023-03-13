/***************************************************************************//**
* \file cy_fr_api_tx_handler.c
* \version 1.0
*
* \brief
* User Interface for transmitting Messages for the buffers
*
* This file contains the user interface for the transmit routines to copy the data
* and header sections to the buffer, selected by buffer or id.
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


#include "flexray/api/cy_fr_api_global.h"
#include "flexray/api/cy_fr_api_tx_handler.h"
#include "flexray/api/cy_fr_api_status_service.h"
#include "flexray/ccal/cy_fr_ccal_init.h"
#include "flexray/ccal/cy_fr_ccal_tx_handler.h"


/*---------------------------------------------------------------------------------*/
/** \brief      This function writes into the buffer 'cy_fr_buffer' and copy the data
                pointed by 'cy_fr_data' with length 'cy_fr_data_length' to the write
                data section of the input buffer. Transmission is only possible in
                NORMAL_ACTIVE state.

    \param[in]  base                  Pointer to FlexRay register base
    \param[in]  cy_fr_data_adr        Start address of the memory area with data to
                                      transmit.
    \param[in]  cy_fr_data_length     An 8 bit value of number of bytes should copy.
    \param[in]  cy_fr_buffer          The number of buffer in which the data should copied.
    \param[in]  cy_fr_channel         The channel where the frame transmit.

    \retval     CY_FR_OKAY            In case no error detected.
    \retval     CY_FR_ACCESS_DENEID   If the access to buffer is inhibit.
    \retval     CY_FR_NOT_ACTIVE      In case the FlexRay CC is not in NORMAL_ACTIVE
                                      Status.
    \retval     CY_FR_INVALID_BUFFER  If a buffer higher then the last configured
                                      Buffer is requested.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_tx_handler_buffer(volatile stc_FLEXRAY_t *    base,
                                              uint32_t                    cy_fr_data_adr,
                                              uint8_t                     cy_fr_data_length,
                                              CY_FR_BUFFER_TYPE           cy_fr_buffer,
                                              CY_FR_CHANNEL_TYPE          cy_fr_channel )
{
  if( cy_fr_api_pocs_is_normal_active ( base ) == CY_FR_TRUE ){
    return cy_fr_ccal_tx_handler_buffer ( base, 
                                          cy_fr_data_adr,
                                          cy_fr_data_length,
                                          cy_fr_buffer,
                                          cy_fr_channel );
  } else {
    return CY_FR_NOT_ACTIVE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function search for the buffer with id 'cy_fr_frame_id' and copy 
                the data pointed by 'cy_fr_data' with length 'cy_fr_data_length' to the 
                write data section of this input buffer. Transmission is only possible 
                in NORMAL_ACTIVE state.

    \param[in]  base                  Pointer to FlexRay register base
    \param[in]  cy_fr_data_adr        Start address of the memory area with data to
                                      transmit.
    \param[in]  cy_fr_data_length     An 8 bit value of number of bytes should copy.
    \param[in]  cy_fr_frame_id        Frame ID for the requested transmission.
    \param[in]  cy_fr_channel         The channel where the frame transmit.

    \retval     CY_FR_OKAY            In case no error detected.
    \retval     CY_FR_ACCESS_DENEID   If the access to buffer is inhibit.
    \retval     CY_FR_NOT_ACTIVE      In case the FlexRay CC is not in NORMAL_ACTIVE
                                      Status.
    \retval     CY_FR_INVALID_BUFFER  If a buffer higher then the last configured
                                      Buffer is requested or no buffer found with
                                      'cy_fr_frame_id' AND 'cy_fr_channel'.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_tx_handler_id     ( volatile stc_FLEXRAY_t * base,
                                                uint32_t                 cy_fr_data_adr,
                                                uint8_t                  cy_fr_data_length,
                                                CY_FR_FRAME_ID_TYPE      cy_fr_frame_id,
                                                CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  CY_FR_BUFFER_TYPE cy_fr_buffer = 0;

  if( cy_fr_api_pocs_is_normal_active ( base ) == CY_FR_TRUE ){
    while(( cy_fr_frame_id != cy_fr_lut_buffer_to_id[CY_FR_GET_INSTANCE_FROM_BASE(base)][cy_fr_buffer][0]) || /* check which buffer assigned to the requested */
          ( cy_fr_channel  != cy_fr_lut_buffer_to_id[CY_FR_GET_INSTANCE_FROM_BASE(base)][cy_fr_buffer][1])) { /* buffer and if the buffer received the correct channel */
      cy_fr_buffer++;                                                 /* */
      if ( cy_fr_buffer >= CY_FR_MAX_BUFFERS ) return CY_FR_INVALID_BUFFER;         /* if max number of buffer reached return with error */
    }
    return cy_fr_ccal_tx_handler_buffer ( base, 
                                          cy_fr_data_adr,
                                          cy_fr_data_length ,
                                          cy_fr_buffer,
                                          cy_fr_channel);
  } else {
    return CY_FR_NOT_ACTIVE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks if the last transmit request for the buffer
                'cy_fr_buffer' was processed or not. In case the requested Buffer is set
                to continuous transmit this function does not work, because the
                Transmission Request Flag will not be cleared after transmission.

    \param[in]  base              Pointer to FlexRay register base
    \param[in]  cy_fr_buffer      The number of the buffer should be checked.

    \retval     CY_FR_TX_READY       In case the last transmission is ready.
    \retval     CY_FR_TX_NOT_READY   If the last frame was not transmitted.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_TX_STATUS_TYPE cy_fr_api_tx_data_ready_buffer  ( volatile stc_FLEXRAY_t * base,
                                                           CY_FR_BUFFER_TYPE        cy_fr_buffer )
{
  return cy_fr_ccal_tx_data_ready ( base, cy_fr_buffer );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks if the last transmit request for the Frame ID
                'cy_fr_frame_id' was processed or not. In case the requested Buffer is set
                to continuous transmit this function does not work, because the
                Transmission Request Flag will not be cleared after transmission.

    \param[in]  base                  Pointer to FlexRay register base
    \param[in]  cy_fr_frame_id        The Frame ID should be checked.
    \param[in]  cy_fr_channel         The channel where the frame transmit .

    \retval     CY_FR_TX_READY                          In case the last transmission is ready
    \retval     CY_FR_TX_NOT_READY                      If the last frame was not transmitted
    \retval     CY_FR_TX_STATUS_UNKNOWN_BUFFER_INVALID  If a buffer higher then the last configured
                                                        Buffer is requested or no buffer found with
                                                        'cy_fr_frame_id' AND 'cy_fr_channel'.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_TX_STATUS_TYPE cy_fr_api_tx_data_ready_id ( volatile stc_FLEXRAY_t * base,
                                                      CY_FR_FRAME_ID_TYPE      cy_fr_frame_id,
                                                      CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  CY_FR_BUFFER_TYPE cy_fr_buffer=0;

    while(( cy_fr_frame_id != cy_fr_lut_buffer_to_id[CY_FR_GET_INSTANCE_FROM_BASE(base)][cy_fr_buffer][0]) || /* check which buffer assigned to the requested */
          ( cy_fr_channel  != cy_fr_lut_buffer_to_id[CY_FR_GET_INSTANCE_FROM_BASE(base)][cy_fr_buffer][1])) { /* buffer and if the buffer received the correct channel */
      cy_fr_buffer++;                                                 /* */
      if ( cy_fr_buffer >= CY_FR_MAX_BUFFERS ) return CY_FR_TX_STATUS_UNKNOWN_BUFFER_INVALID;         /* if max number of buffer reached return with error */
    }
  return cy_fr_ccal_tx_data_ready ( base, cy_fr_buffer );
}



/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the last transmitted frame id from the last
                dynamic transmit slot register LDTS.
                
    \param[in]  base                 Pointer to FlexRay register base
    \param[in]  cy_fr_channel        The Channel for that, the last dynamic tx is 
                                     requested.

    \retval     CY_FR_FRAME_ID_TYPE  Last transmitted dynamic frame id from channel
                                     'cy_fr_channel'

*/
/*---------------------------------------------------------------------------------*/
CY_FR_FRAME_ID_TYPE cy_fr_api_last_dyn_tx ( volatile stc_FLEXRAY_t * base,
                                            CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  if ( cy_fr_channel == CY_FR_CHANNEL_A ){
    return cy_fr_ccal_last_dyn_tx_a ( base );
  } else if ( cy_fr_channel == CY_FR_CHANNEL_B ){
    return cy_fr_ccal_last_dyn_tx_b ( base );
  } else {
    return 0;
  }
}


/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif


/* [] END OF FILE */

