/***************************************************************************//**
* \file cy_fr_ccal_tx_handler.c
* \version 1.0
*
* \brief
* Transmit service for FlexRay Communication Controller
*
* This file contains the routines to transmit frames by select buffer or ID.
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
#include "flexray/fhal/cy_fr_fhal.h"


/*---------------------------------------------------------------------------------*/
/** \brief      This Function transfer the data pointed by 'cy_fr_data_adr' with 
                'cy_fr_data_length' to the input buffer data section.
    
    \param[in]  base               Pointer to FlexRay register base
    \param[in]  cy_fr_data_length  An 8 bit value, number of bytes to transfer.
    \param[in]  cy_fr_data_adr     Start address of the memory area with data to transmit.

    \retval     CY_FR_OKAY         Always, no Error handling.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_data_to_buffer ( volatile stc_FLEXRAY_t * base,
                                              uint32_t                 cy_fr_data_adr,
                                              uint8_t                  cy_fr_data_length )
{

  uint32_t cy_fr_data_pointer = 0;                                              /* byte wise data pointer */
  uint32_t cy_fr_temp_data;
  uint8_t *cy_fr_data_byte;                                                     /* 8BIT data pointer */
                                                                           
  cy_fr_data_byte = &(*(uint8_t *)cy_fr_data_adr);                              /* make pointer from 'cy_fr_data_adr' */
                                                                           
  cy_fr_fhal_wait_till_cleared32(0x00008000,CY_FR_REG_IBCR);                    /* wait for input buffer not busy */
  while ( cy_fr_data_length > cy_fr_data_pointer ){                             /* while not all bytes copied */
   cy_fr_temp_data  = 0;          
   cy_fr_temp_data += (*(cy_fr_data_byte + cy_fr_data_pointer + 0) << 24 );     /* copy first byte */
   if ( cy_fr_data_length > cy_fr_data_pointer+1 ){                             /* check if all bytes copied */
   cy_fr_temp_data += (*(cy_fr_data_byte + cy_fr_data_pointer + 1) << 16 );     /* copy second byte */
    if ( cy_fr_data_length > cy_fr_data_pointer+2 ){                            /* check if all bytes copied */
       cy_fr_temp_data += (*(cy_fr_data_byte + cy_fr_data_pointer + 2) << 8 );  /* copy third byte */
     if ( cy_fr_data_length > cy_fr_data_pointer+3 ){                           /* check if all bytes copied */
       cy_fr_temp_data += (*(cy_fr_data_byte + cy_fr_data_pointer + 3) << 0 );  /* copy fourth byte */
     }                                                                          /* end if ( cy_fr_data_length > cy_fr_data_pointer+1 ) */
    }                                                                           /* end if ( cy_fr_data_length > cy_fr_data_pointer+2 ) */
   }
   cy_fr_temp_data  = cy_fr_fhal_switch(cy_fr_temp_data);                       /* swap byte positions for correct byte order */
   cy_fr_fhal_write32(cy_fr_temp_data, CY_FR_REG_WRDS + cy_fr_data_pointer);    /* write data to data section */
                                    
   cy_fr_data_pointer+=4;                                                       /* increse data_pointer by 4 bytes */
  }                                                                             /* end while ( cy_fr_data_length > cy_fr_data_pointer ) */

  return CY_FR_OKAY;                                                            /* leave */
}                                                                               /* end of function */



/*---------------------------------------------------------------------------------*/
/** \brief      This Function write the data pointed by 'cy_fr_data_adr' into the 
                buffer 'cy_fr_buffer' and transfer the data with lengtht 
                'cy_fr_data_length' to the write data section of the input buffer.

    \param[in]  base                  Pointer to FlexRay register base
    \param[in]  cy_fr_data_adr        Start address of the memory area with data to transmit.
    \param[in]  cy_fr_data_length     An 8 bit value of number of bytes should copy.
    \param[in]  cy_fr_buffer          Number of buffer in which the data should copied.
    \param[in]  cy_fr_channel         The channel where the frame transmit.

    \retval     CY_FR_OKAY            In case of success.
                CY_FR_ACCESS_DENIED   If not the buffer 'cy_fr_buffer' was shown.
                CY_FR_INVALID_BUFFER  In case a buffer higher then the last configured
                                      buffer was requested.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_tx_handler_buffer ( volatile stc_FLEXRAY_t * base,
                                                 uint32_t                 cy_fr_data_adr,
                                                 uint8_t                  cy_fr_data_length,
                                                 CY_FR_BUFFER_TYPE        cy_fr_buffer,
                                                 CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  if ( cy_fr_buffer <= ((cy_fr_fhal_read32(CY_FR_REG_MRC)>>16) & 0x00FF)){
    cy_fr_ccal_data_to_buffer(base, cy_fr_data_adr, cy_fr_data_length);                /* copy data to data section of input buffer */
    cy_fr_fhal_write32(0x00000006 ,CY_FR_REG_IBCM);                                /* select header and data section for transfer to message RAM */
    cy_fr_fhal_write32(cy_fr_buffer,CY_FR_REG_IBCR);                               /* start transfer to message RAM */
    if ( ((cy_fr_fhal_read32(CY_FR_REG_IBCR) & 0x7F0000) >> 16 ) == cy_fr_buffer){ /* if write to correct buffer */
      return CY_FR_OKAY;                                                           /* leave with out error */
    } else {                                                                       /* if write to wrong buffer */
      return CY_FR_ACCESS_DENIED;                                                  /* leave in error case */
    }                                                                              /* */
  } else {                                                                         /* */ 
    return CY_FR_INVALID_BUFFER;                                                   /* buffer no is higher then last configured buffer */
  }
} 


/*---------------------------------------------------------------------------------*/
/** \brief      This function check if the last transmit request for the message
                buffer 'cy_fr_buffer' was processed.

    \param[in]  base             Pointer to FlexRay register base
    \param[in]  cy_fr_buffer     The number of message buffer.

    \retval     CY_FR_TX_READY      If the transmit request was processed.
    \retval     CY_FR_TX_NOT_READY  In case the transmission is ongoing.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_TX_STATUS_TYPE cy_fr_ccal_tx_data_ready ( volatile stc_FLEXRAY_t * base,
                                                    CY_FR_BUFFER_TYPE        cy_fr_buffer )
{
  uint32_t cy_fr_txrq = 0;                                /* temporary register value */

  if ( cy_fr_buffer <= 31 ){                              /* buffer 0 - 31 */
    cy_fr_txrq = cy_fr_fhal_read32 ( CY_FR_REG_TXRQ1 );   /* read out the transmission request flags */
  } else if ( cy_fr_buffer <= 63 ){                       /* buffer 32 - 63 */
    cy_fr_txrq = cy_fr_fhal_read32 ( CY_FR_REG_TXRQ2 );   /* read out the transmission request flags */
    cy_fr_buffer -= 32;                                   /* reduce the buffer offset */
  } else if ( cy_fr_buffer <= 95 ){                       /* buffer 64 - 95 */
    cy_fr_txrq = cy_fr_fhal_read32 ( CY_FR_REG_TXRQ3 );   /* read out the transmission request flags */
    cy_fr_buffer -= 64;                                   /* reduce the buffer offset */               
  } else if ( cy_fr_buffer <= 127 ){                      /* buffer 96 - 127 */
    cy_fr_txrq = cy_fr_fhal_read32 ( CY_FR_REG_TXRQ4 );   /* read out the transmission request flags */
    cy_fr_buffer -= 96;                                   /* reduce the buffer offset */               
  }                                                       /* */
  if ( ( ( cy_fr_txrq >> cy_fr_buffer ) & 1 ) == 0){      /* check transmission request flags for buffer 'cy_fr_buffer' */
    return CY_FR_TX_READY;                                /* if  flag is cleared */
  }                                                       /* */
  return CY_FR_TX_NOT_READY;                              /* if flag is not cleared */
      
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Last Dynamic Transmit Slot ( LDTS ) and 
                separated the slot for channel A.

    \param[in]  base                 Pointer to FlexRay register base
    
    \retval     CY_FR_FRAME_ID_TYPE  The last dynamic frame id from channel A.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_FRAME_ID_TYPE cy_fr_ccal_last_dyn_tx_a ( volatile stc_FLEXRAY_t * base )
{
  return ((CY_FR_FRAME_ID_TYPE)( cy_fr_fhal_read32(CY_FR_REG_LDTS) & 0x7FF ));
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Last Dynamic Transmit Slot ( LDTS ) and 
                separated the slot for channel B.

    \param[in]  base                 Pointer to FlexRay register base
    
    \retval     CY_FR_FRAME_ID_TYPE  The last dynamic frame id from channel B.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_FRAME_ID_TYPE cy_fr_ccal_last_dyn_tx_b ( volatile stc_FLEXRAY_t * base )
{
  return ((CY_FR_FRAME_ID_TYPE)( ( cy_fr_fhal_read32(CY_FR_REG_LDTS) & 0x7FF0000 ) >> 16));
}


/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


