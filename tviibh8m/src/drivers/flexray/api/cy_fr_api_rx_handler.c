/***************************************************************************//**
* \file cy_fr_api_rx_handler.c
* \version 1.0
*
* \brief
* User Interface for receiving Messages for the buffers
*
* This file contains the user interface for the receive routines to get the received frames
* from the buffer by buffer, id or fifo.
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
#include "flexray/api/cy_fr_api_status_service.h"
#include "flexray/ccal/cy_fr_ccal_init.h"
#include "flexray/ccal/cy_fr_ccal_rx_handler.h"


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the buffer 'cy_fr_buffer' and transfer the
                header section to global structure pointed by '*cy_fr_buffer_header' and
                the data section with length 'cy_fr_data_length' to the global variable
                pointed by '*cy_fr_data'.
                If only the header section or the data section should be copied it must
                select by 'cy_fr_mode':
                \li   CY_FR_NO_NEW_FRAME   get no header or data section,
                \li   CY_FR_NEW_FRAME      get all,
                \li   CY_FR_STATUS_CHANGE  get header section only,
                \li   CY_FR_DATA_ONLY      get data section only
                If you use 'cy_fr_api_new_rx_data_buffer ( cy_fr_buffer );' as parameter
                for 'cy_fr_mode' only new data will requested, In case the the Message
                Buffer status has been changed only the header section will be
                transferred and in case of new Data, header and data section will be
                copied.

    \param[in]  base                  Pointer to FlexRay register base
    \param[out] cy_fr_data            The start address of the global variable where
                                      the data section of the requested buffer should
                                      be copied to.
    \param[out] *cy_fr_buffer_header  A pointer to a 'CY_FR_RX_BUFFER_HEADER_STRUCT'
                                      which get the Header section of requested buffer.
    \param[in]  cy_fr_data_length     The length of the data section.
    \param[in]  cy_fr_buffer          The number of the requested Buffer.
    \param[in]  cy_fr_channel         The channel where the frame was received.
    \param[in]  cy_fr_mode            Mode byte, to select header section and/or data
                                      section.

    \retval     CY_FR_OKAY            If everything is allright.
    \retval     CY_FR_ACCESS_DENIED   In case wrong buffer is shown.
    \retval     CY_FR_NOT_ACTIVE      If the FlexRay CC is in NORMAL_PASSIVE state.
    \retval     CY_FR_NOT_PASSIVE     In case of the FlexRay CC is not NORMAL_ACTIVE
                                      or NORMAL_PASSIVE.
    \retval     CY_FR_NO_DATA         In case of not data requested.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_rx_handler_buffer( volatile stc_FLEXRAY_t *        base,
                                               uint32_t                        cy_fr_data,
                                               CY_FR_RX_BUFFER_HEADER_STRUCT * cy_fr_buffer_header,
                                               uint8_t                         cy_fr_data_length,
                                               CY_FR_BUFFER_TYPE               cy_fr_buffer,
                                               CY_FR_CHANNEL_TYPE              cy_fr_channel,
                                               CY_FR_RX_STATUS_MODE_TYPE       cy_fr_mode )
{
  if ( (cy_fr_mode != CY_FR_RX_NO_NEW_FRAME) && (cy_fr_mode != CY_FR_RX_STATUS_UNKNOWN_BUFFER_INVALID) ){ /* if data and/or header section is requested */
    if( cy_fr_api_pocs_is_normal_active(base) == CY_FR_TRUE ){     /* if the FlexRay CC is NORMAL_ACTIVE */
      return cy_fr_ccal_rx_handler_buffer(base,
                                          cy_fr_data,              /* start address of global variable for data section */
                                          &(*cy_fr_buffer_header), /* pointer to a global structure for the header section */
                                          cy_fr_data_length ,      /* requested length in Bytes of the data section*/
                                          cy_fr_buffer,            /* number of buffer */
                                          cy_fr_channel,           /* The channel where the frame received */
                                          cy_fr_mode);             /* mode of the request*/
    } else if ( cy_fr_api_pocs_is_normal_passive(base) == CY_FR_TRUE ){ /* if the FlexRay CC is NORMAL_PASSIVE */
        cy_fr_ccal_rx_handler_buffer(base,
                                     cy_fr_data,                   /* start address of global variable for data section */
                                     &(*cy_fr_buffer_header),      /* pointer to a global structure for the header section */
                                     cy_fr_data_length ,           /* requested length in Bytes of the data section*/
                                     cy_fr_buffer,                 /* number of buffer */
                                     cy_fr_channel,                /* The channel where the frame received */
                                     cy_fr_mode );                 /* mode of the request*/
        return CY_FR_NOT_ACTIVE;                                   /* return with a warning */
    } else {                                                       /* if not NORMAL_ACTIVE or NORMAL_PASSIVE */
      return CY_FR_NOT_PASSIVE;                                    /* return with error */
    }                                                              /* end of if( cy_fr_api_pocs_is_normal_active ( ) ) */
  } else {
    if ( cy_fr_mode == CY_FR_RX_NO_NEW_FRAME ){
      return CY_FR_NO_DATA;  
    } else {                                                       /* CY_FR_RX_STATUS_UNKNOWN_BUFFER_INVALID */
      return CY_FR_INVALID_BUFFER; 
    }
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the buffer 'cy_fr_buffer' and transfer the
                header section to global structure pointed by '*cy_fr_buffer_header' and
                the data section with length 'cy_fr_data_length' to the global variable
                with start address 'cy_fr_data'.
                If only the header section or the data section should copied it must
                select by 'cy_fr_mode':
                \li   CY_FR_RX_NO_NEW_FRAME   get no header or data section,
                \li   CY_FR_RX_NEW_FRAME      get all,
                \li   CY_FR_RX_STATUS_CHANGE  get header section only,
                \li   CY_FR_RX_DATA_ONLY      get data section only
                If you use 'cy_fr_api_new_rx_data_fifo ( cy_fr_fifo );' as parameter
                for 'cy_fr_mode' only new data will requested, In case the the Message
                Buffer status has been changed only the header section will be
                transferred and In case of new Data, header and data section will be
                copied.

    \param[in]  base                  Pointer to FlexRay register base
    \param[out] cy_fr_data            The start address of the global variable where
                                      the data section of the requested buffer should
                                      copied to.
    \param[out] *cy_fr_buffer_header  A pointer to a CY_FR_RX_BUFFER_HEADER_STRUCT
                                      which  get the Header section of requested buffer
    \param[in]  cy_fr_data_length     the length of the data section
    \param[in]  cy_fr_fifo            The number of the first Buffer of the FiFo
    \param[in]  cy_fr_mode            Mode byte to select header section and/or data
                                      section.

    \retval     CY_FR_OKAY            If everything is allright,
    \retval     CY_FR_ACCESS_DENEID   In case wrong buffer is shown ,
    \retval     CY_FR_NOT_ACTIVE      If the FlexRay CC is in NORMAL_PASSIVE state
    \retval     CY_FR_NOT_PASSIVE     In case of the FlexRay CC is not NORMAL_ACTIVE
                                      or NORMAL_PASSIVE.
    \retval     CY_FR_FIFO_EMPTY      In case of not data requested.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_rx_handler_fifo ( volatile stc_FLEXRAY_t *        base,
                                              uint32_t                        cy_fr_data,
                                              CY_FR_RX_BUFFER_HEADER_STRUCT * cy_fr_buffer_header,
                                              uint8_t                         cy_fr_data_length,
                                              CY_FR_BUFFER_TYPE               cy_fr_fifo,
                                              CY_FR_RX_STATUS_MODE_TYPE       cy_fr_mode )
{
  if ( (cy_fr_mode != CY_FR_RX_NO_NEW_FRAME) && (cy_fr_mode != CY_FR_RX_STATUS_UNKNOWN_BUFFER_INVALID) ){  /* if data and/or header section is requested */
    if( cy_fr_api_pocs_is_normal_active(base) == CY_FR_TRUE ){      /* if the FlexRay CC is NORMAL_ACTIVE */
      return cy_fr_ccal_rx_handler_fifo(base,
                                        cy_fr_data,                 /* start address of global variable for data section */
                                        &(*cy_fr_buffer_header),    /* pointer to a global structure for the header section */
                                        cy_fr_data_length ,         /* requested length in Bytes of the data section*/
                                        cy_fr_fifo,                 /* number of buffer */
                                        cy_fr_mode);                /* mode of the request*/
    } else if ( cy_fr_api_pocs_is_normal_passive(base) == CY_FR_TRUE ){ /* if the FlexRay CC is NORMAL_PASSIVE */
        cy_fr_ccal_rx_handler_fifo(base,
                                   cy_fr_data,                      /* start address of global variable for data section */
                                   &(*cy_fr_buffer_header),         /* pointer to a global structure for the header section */
                                   cy_fr_data_length,               /* requested length in Bytes of the data section*/
                                   cy_fr_fifo,                      /* number of buffer */
                                   cy_fr_mode);                     /* mode of the request*/
        return CY_FR_NOT_ACTIVE;                                    /* return with a warning */
    } else {                                                        /* if not NORMAL_ACTIVE or NORMAL_PASSIVE */
      return CY_FR_NOT_PASSIVE;                                     /* return with error */
    }                                                               /* end of if( cy_fr_api_pocs_is_normal_active ( ) ) */
  } else {
    if ( cy_fr_mode == CY_FR_RX_NO_NEW_FRAME ){
      return CY_FR_FIFO_EMPTY;  
    } else {                                                        /* CY_FR_RX_STATUS_UNKNOWN_BUFFER_INVALID */
      return CY_FR_INVALID_BUFFER; 
    }
  }
}

/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the buffer assigned to the frame id
                'cy_fr_frame_id' and transfer the header section to global structure
                pointed by '*cy_fr_buffer_header' and the data section with length
                cy_fr_data_length to the global variable with start address 'cy_fr_data'.
                If only the header section or the data section should copied it must
                select by 'cy_fr_mode':
                \li   CY_FR_RX_NO_NEW_FRAME   get no header or data section,
                \li   CY_FR_RX_NEW_FRAME      get all,
                \li   CY_FR_RX_STATUS_CHANGE  get header section only,
                \li   CY_FR_RX_DATA_ONLY      get data section only
                If you use 'cy_fr_api_new_rx_data_id ( cy_fr_frame_id );' as parameter
                for 'cy_fr_mode' only new data will requested, In case the the Message
                Buffer status has been changed only the header section will be
                transferred and In case of new Data, header and data section will be
                copied.

    \param[in]  base                  Pointer to FlexRay register base
    \param[out] cy_fr_data            The start address of the global variable where
                                      the data section of the requested buffer should
                                      copied to.
    \param[out] *cy_fr_buffer_header  A pointer to a CY_FR_RX_BUFFER_HEADER_STRUCT
                                      which get the Header section of requested buffer
    \param[in]  cy_fr_data_length     the length of the data section
    \param[in]  cy_fr_frame_id        The number of the requested Frame ID.
    \param[in]  cy_fr_channel         The channel where the frame received
    \param[in]  cy_fr_mode            Mode byte to select header section and/or data
                                      section.

    \retval     CY_FR_OKAY            If everything is allright,
    \retval     CY_FR_ACCESS_DENEID   In case wrong buffer is shown ,
    \retval     CY_FR_NOT_ACTIVE      If the FlexRay CC is in NORMAL_PASSIVE state
    \retval     CY_FR_NOT_PASSIVE     In case of the FlexRay CC is not NORMAL_ACTIVE
                                      or NORMAL_PASSIVE.
    \retval     CY_FR_NO_DATA         In case of not data requested.
    \retval     CY_FR_INVALID_BUFFER  In case no buffer found with the Frame ID
                                      cy_fr_frame_id  and channel cy_fr_channel.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_rx_handler_id ( volatile stc_FLEXRAY_t *        base,
                                            uint32_t                        cy_fr_data,
                                            CY_FR_RX_BUFFER_HEADER_STRUCT * cy_fr_buffer_header,
                                            uint8_t                         cy_fr_data_length,
                                            CY_FR_FRAME_ID_TYPE             cy_fr_frame_id,
                                            CY_FR_CHANNEL_TYPE              cy_fr_channel,
                                            CY_FR_RX_STATUS_MODE_TYPE       cy_fr_mode )
{
  CY_FR_BUFFER_TYPE cy_fr_buffer=0;

  if ( (cy_fr_mode != CY_FR_RX_NO_NEW_FRAME) && (cy_fr_mode != CY_FR_RX_STATUS_UNKNOWN_BUFFER_INVALID) ){   /* if data and/or header section is requested */
    while(( cy_fr_frame_id != cy_fr_lut_buffer_to_id[CY_FR_GET_INSTANCE_FROM_BASE(base)][cy_fr_buffer][0]) || /* check which buffer assigned to the requested */
          ( cy_fr_channel  != cy_fr_lut_buffer_to_id[CY_FR_GET_INSTANCE_FROM_BASE(base)][cy_fr_buffer][1])) { /* buffer and if the buffer received the correct channel */
      cy_fr_buffer++;                                                 /* */
      if ( cy_fr_buffer >= CY_FR_MAX_BUFFERS ) return CY_FR_INVALID_BUFFER;         /* if max number of buffer reached return with error */
    }                                                                 /* */
    if( cy_fr_api_pocs_is_normal_active(base) == CY_FR_TRUE ){        /* if the FlexRay CC is NORMAL_ACTIVE */
      return cy_fr_ccal_rx_handler_buffer(base,
                                          cy_fr_data,                 /* start address of global variable for data section */
                                          &(*cy_fr_buffer_header),    /* pointer to a global structure for the header section */
                                          cy_fr_data_length ,         /* requested length in Bytes of the data section*/
                                          cy_fr_buffer,               /* number of buffer */
                                          cy_fr_channel,              /* The channel where the frame received */
                                          cy_fr_mode);                /* mode of the request*/
    } else if ( cy_fr_api_pocs_is_normal_passive(base) == CY_FR_TRUE ){ /* if the FlexRay CC is NORMAL_PASSIVE */
        cy_fr_ccal_rx_handler_buffer(base,
                                     cy_fr_data,                      /* start address of global variable for data section */
                                     &(*cy_fr_buffer_header),         /* pointer to a global structure for the header section */
                                     cy_fr_data_length ,              /* requested length in Bytes of the data section*/
                                     cy_fr_buffer,                    /* number of buffer */
                                     cy_fr_channel,                   /* The channel where the frame received */
                                     cy_fr_mode);                     /* mode of the request*/
        return CY_FR_NOT_ACTIVE;                                      /* return with a warning */
    } else {                                                          /* if not NORMAL_ACTIVE or NORMAL_PASSIVE */
      return CY_FR_NOT_PASSIVE;                                       /* return with error */
    }                                                                 /* end of if( cy_fr_api_pocs_is_normal_active ( ) ) */
  } else {
    if ( cy_fr_mode == CY_FR_RX_NO_NEW_FRAME ){
      return CY_FR_NO_DATA;  
    } else {                                                          /* CY_FR_RX_STATUS_UNKNOWN_BUFFER_INVALID */
      return CY_FR_INVALID_BUFFER; 
    }
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks, if new data received or Message Buffer Changed
                for the buffer 'cy_fr_buffer'. The return value could used as parameter
                for the mode of cy_fr_api_rx_handler_buffer ( ).

    \param[in]  base                 Pointer to FlexRay register base
    \param[in]  cy_fr_buffer         The number of the buffer which should be
                                     checked.

    \retval     CY_FR_NEW_DATA       If a new frame was received
    \retval     CY_FR_STATUS_CHANGE  In case of failed received frame
    \retval     CY_FR_NO_NEW_FRAME   If no new frame was received
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RX_STATUS_MODE_TYPE cy_fr_api_new_rx_data_buffer ( volatile stc_FLEXRAY_t * base,
                                                        CY_FR_BUFFER_TYPE        cy_fr_buffer )
{
  return cy_fr_ccal_new_rx_data(base, cy_fr_buffer); /* check if new data received or message buffer status changed */
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks, if the fifo is not empty flag is set. 

    \param[in]  base                Pointer to FlexRay register base
    
    \retval     CY_FR_NEW_DATA,     If a new frame was received,
    \retval     CY_FR_NO_NEW_FRAME  If no new frame was received.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RX_STATUS_MODE_TYPE cy_fr_api_new_rx_data_fifo ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_new_rx_data_fifo(base);  /* check the FiFo not empty flag */
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks, if new data received or Message Buffer Changed
                for the buffer assigned to the frame id 'cy_fr_frame_id'. The return
                value could used as parameter for the mode of
                cy_fr_api_rx_handler_id ( ).

    \param[in]  base                                    Pointer to FlexRay register base
    \param[in]  cy_fr_channel                           The channel where the frame received
    \param[in]  cy_fr_frame_id                          The number of the frame id which should be
                                                        checked.

    \retval     CY_FR_NEW_DATA                          If a new frame was received
    \retval     CY_FR_STATUS_CHANGE                     In case of failed received frame
    \retval     CY_FR_NO_NEW_FRAME                      If no new frame was received
    \retval     CY_FR_RX_STATUS_UNKNOWN_BUFFER_INVALID  If corresponding buffer for cy_fr_frame_id not found

*/
/*---------------------------------------------------------------------------------*/
CY_FR_RX_STATUS_MODE_TYPE cy_fr_api_new_rx_data_id ( volatile stc_FLEXRAY_t * base,
                                                    CY_FR_FRAME_ID_TYPE      cy_fr_frame_id,
                                                    CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  CY_FR_BUFFER_TYPE cy_fr_buffer = 0;

  while(( cy_fr_frame_id != cy_fr_lut_buffer_to_id[CY_FR_GET_INSTANCE_FROM_BASE(base)][cy_fr_buffer][0]) || /* check which buffer assigned to the requested */
        ( cy_fr_channel  != cy_fr_lut_buffer_to_id[CY_FR_GET_INSTANCE_FROM_BASE(base)][cy_fr_buffer][1])) { /* buffer and if the buffer received the correct channel */
    cy_fr_buffer++;                                                 /* */
    if ( cy_fr_buffer >= CY_FR_MAX_BUFFERS ) return CY_FR_RX_STATUS_UNKNOWN_BUFFER_INVALID;         /* if max number of buffer reached return with error */
  }                                                                 /* */
  return cy_fr_ccal_new_rx_data(base, cy_fr_buffer);                /* check if new data received or message buffer status changed */
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks if the FiFo fill level reached the critical
                level programmed in FiFo Critical Level Register ( FCL ).

    \param[in]  base         Pointer to FlexRay register base
    
    \retval     CY_FR_TRUE   If critical level is reached
    \retval     CY_FR_FALSE  If critical fill level is not reached.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_fifo_is_critical ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_fifo_is_critical(base);  /* read the FiFo critical fill level flag */
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks if the FiFo is over filled ( in this case
                messages lost ).

    \param[in]  base         Pointer to FlexRay register base
    
    \retval     CY_FR_TRUE   If FiFo is overrun
    \retval     CY_FR_FALSE  If FiFo is not overrun
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_fifo_is_overrun ( volatile stc_FLEXRAY_t * base )
{
   return cy_fr_ccal_fifo_is_overrun(base);  /* read the FiFo overrun flag */
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the fill level of the fifo.

    \param[in]  base      Pointer to FlexRay register base
    
    \retval     uint8_t   The number of filled buffers of the FiFo
 */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_api_get_fifo_fill ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_fifo_fill(base);   /* read out the FiFo fill level */
}



/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif


/* [] END OF FILE */
