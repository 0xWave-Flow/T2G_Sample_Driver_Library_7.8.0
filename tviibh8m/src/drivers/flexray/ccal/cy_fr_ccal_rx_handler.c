/***************************************************************************//**
* \file cy_fr_ccal_rx_handler.c
* \version 1.0
*
* \brief
* Receive Service for FlexRay Comunication Controler
*
* This file contains all routines for receive service. 'cy_fr_ccal_rx_handler_buffer'
* is for receiving from a buffer 'cy_fr_ccal_rx_handler_id' if for receiving by ID.
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
#include "flexray/fhal/cy_fr_fhal.h"


/*----------------------------------------------------------------------------------*/
/** \brief      This function reads out the data section of the actual output buffer 
                with length 'cy_fr_data_length' and transfer the data with correct
                byte order to the address pointed by 'cy_fr_data'.

    \param[in]  base                Pointer to FlexRay register base
    \param[in]  cy_fr_data_length   A 8 bit value of number of bytes should be 
                                    transferred.
    \param[out] cy_fr_data          Pointer to address where the data should be
                                    transferred to.
                                   
    \retval     CY_FR_OKAY          Always, no error handling.
*/
/*----------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_data_from_buffer( volatile stc_FLEXRAY_t * base,
                                               uint32_t                 cy_fr_data,
                                               uint8_t                  cy_fr_data_length )
{
  uint32_t cy_fr_data_pointer = 0;                                  /* temporary data pointer */
  uint32_t cy_fr_temp = 0;                                          /* temporary data 32 Bit */
  uint8_t *cy_fr_data_byte;                                         /* new 8bit target pointer */
  cy_fr_data_byte = &(*(uint8_t *)cy_fr_data);                      /* make 8BIT pointer from address */
  while ( cy_fr_data_length > cy_fr_data_pointer ){                 /* while not all data copied */
    cy_fr_temp = cy_fr_fhal_read32((CY_FR_REG_RDDS +                /* readout one register of the output */
                                           cy_fr_data_pointer));    /*  buffer data section */


    cy_fr_temp = cy_fr_fhal_switch(cy_fr_temp); 

                       
    *(cy_fr_data_byte + 0 + cy_fr_data_pointer) = cy_fr_temp >>  24;     /* copy first byte */
    if ( cy_fr_data_length > cy_fr_data_pointer + 1 ){                   /* check end of data should copied */
      *(cy_fr_data_byte + 1 + cy_fr_data_pointer) = cy_fr_temp >> 16;    /* copy second byte */
      if ( cy_fr_data_length > cy_fr_data_pointer + 2 ){                 /* check end of data should copied */
        *(cy_fr_data_byte + 2 + cy_fr_data_pointer) = cy_fr_temp >> 8;   /* copy third byte */
        if ( cy_fr_data_length > cy_fr_data_pointer + 3 ){               /* check end of data should copied */
          *(cy_fr_data_byte + 3 + cy_fr_data_pointer) = cy_fr_temp >> 0; /* copie fourth */
        }                                                                /* end if */
      }                                                                  /* end if */
    }                                                                    /* end if */
    cy_fr_data_pointer +=4;                                              /* increment the data pointer */
  }                                                                      /* end while */
  return CY_FR_OKAY;                                                     /* leave */
}                                                                        /* end of function */


/*----------------------------------------------------------------------------------*/
/** \brief      This function reads out the buffer 'cy_fr_buffer' and transfer the
                header section to global structure pointed by '*cy_fr_buffer_header'
                and the data section with length cy_fr_data_length to the global 
                variable pointed by 'cy_fr_data'. The Message request mode 'cy_fr_mode' 
                defines if Header and / or Data section should be transferred.

    \param[in]  base                 Pointer to FlexRay register base
    \param[out] cy_fr_data           Pointer to start address of target variable.
    \param[out] *cy_fr_buffer_header Pointer to start address of target structure.
    \param[in]  cy_fr_data_length    Data size which should copied to '*cy_fr_data'.
    \param[in]  cy_fr_buffer         The buffer number of the buffer which should read out.
    \param[in]  cy_fr_channel        The channel where the frame received.
    \param[in]  cy_fr_mode

    \retval     CY_FR_OKAY           If everything is allright.
    \retval     CY_FR_ACCESS_DENEID  If wrong buffer is shown.
 */
/*----------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_rx_handler_buffer ( volatile stc_FLEXRAY_t *        base,
                                                 uint32_t                        cy_fr_data,
                                                 CY_FR_RX_BUFFER_HEADER_STRUCT * cy_fr_buffer_header,
                                                 uint8_t                         cy_fr_data_length,
                                                 CY_FR_BUFFER_TYPE               cy_fr_buffer,
                                                 CY_FR_CHANNEL_TYPE              cy_fr_channel,
                                                 CY_FR_RX_STATUS_MODE_TYPE       cy_fr_mode )
{
  cy_fr_fhal_wait_till_cleared32 ( 0x00008000  , CY_FR_REG_OBCR );   /* wait for output buffer not busy */
  if ( cy_fr_mode == CY_FR_RX_NEW_DATA ){
    cy_fr_fhal_write32( 3, CY_FR_REG_OBCM );   /* Read Data and Header Section */
  }
  else if ( cy_fr_mode == CY_FR_RX_STATUS_CHANGE ){
    cy_fr_fhal_write32( 1, CY_FR_REG_OBCM );   /* Only Header Section */
  }
  else if ( cy_fr_mode == CY_FR_RX_DATA_ONLY ){
    cy_fr_fhal_write32( 2, CY_FR_REG_OBCM );   /* Only Header Section */
  }
  cy_fr_fhal_write32((0x00000200 + cy_fr_buffer), CY_FR_REG_OBCR );   /* Request buffer and start transfer */
  cy_fr_fhal_wait_till_cleared32 ( 0x00008000   , CY_FR_REG_OBCR );   /* wait for output buffer not busy */
  cy_fr_fhal_write32 ( 0x00000100               , CY_FR_REG_OBCR );   /* view requested buffer */
  cy_fr_fhal_wait_till_cleared32 ( 0x00008000   , CY_FR_REG_OBCR );   /* wait for output buffer not busy */
  if ( ((cy_fr_fhal_read32(CY_FR_REG_OBCR) & 0x7F0000) >> 16 ) == cy_fr_buffer ){ /* if correct buffer is shown */
    if ( ( cy_fr_mode == CY_FR_RX_NEW_DATA ) || ( cy_fr_mode == CY_FR_RX_DATA_ONLY ) ){

      cy_fr_ccal_data_from_buffer(base, cy_fr_data, cy_fr_data_length);   /* copy the data section to global variable pointed by *cy_fr_data */   

    }
    if ( ( cy_fr_mode == CY_FR_RX_NEW_DATA ) || ( cy_fr_mode == CY_FR_RX_STATUS_CHANGE ) ){
      cy_fr_buffer_header->RDHS1 = cy_fr_fhal_read32(CY_FR_REG_RDHS1);/* copy the header section */
      cy_fr_buffer_header->RDHS2 = cy_fr_fhal_read32(CY_FR_REG_RDHS2);/*  into the global structur */
      cy_fr_buffer_header->RDHS3 = cy_fr_fhal_read32(CY_FR_REG_RDHS3);/*  pointed by *cy_fr_buffer_header */
      cy_fr_buffer_header->MBS   = cy_fr_fhal_read32(CY_FR_REG_MBS  );/*  RHDSn = Header Section n; MBS = message buffer status */
    } else {
      cy_fr_buffer_header->RDHS1 = 0;
      cy_fr_buffer_header->RDHS2 = 0;
      cy_fr_buffer_header->RDHS3 = 0;
      cy_fr_buffer_header->MBS   = 0;
    }

    return CY_FR_OKAY;                                            /* leave */
  } else {                                                        /* if wrong buffer is shown */
    return CY_FR_ACCESS_DENIED;                                   /* leave with error */
  }
}                                                                 /* end of functtion */


/*----------------------------------------------------------------------------------*/
/** \brief      This function reads out the next buffer of the FiFo started with buffer 
                'cy_fr_buffer' and transfer the header section to global structure 
                pointed by '*cy_fr_buffer_header' and the data section with length 
                cy_fr_data_length to the global variable pointed by 'cy_fr_data'. The 
                Message request mode 'cy_fr_mode' defines if Header and / or Data 
                section should be transferred.

    \param[in]  base                 Pointer to FlexRay register base
    \param[out] cy_fr_data           Pointer to start address of target variable.
    \param[out] *cy_fr_buffer_header Pointer to start address of target structure.
    \param[in]  cy_fr_data_length    Data size which should copied to '*cy_fr_data'.
    \param[in]  cy_fr_fifo           The buffer number of the buffer which should read out.
    \param[in]  cy_fr_mode

    \retval     CY_FR_OKAY           If everything is allright.
    \retval     CY_FR_ACCESS_DENEID  If wrong buffer is shown.
 */
/*----------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_rx_handler_fifo ( volatile stc_FLEXRAY_t *        base,
                                               uint32_t                        cy_fr_data,
                                               CY_FR_RX_BUFFER_HEADER_STRUCT * cy_fr_buffer_header,
                                               uint8_t                         cy_fr_data_length,
                                               CY_FR_BUFFER_TYPE               cy_fr_fifo,
                                               CY_FR_RX_STATUS_MODE_TYPE       cy_fr_mode )
{
  cy_fr_fhal_wait_till_cleared32 ( 0x00008000  , CY_FR_REG_OBCR );   /* wait for output buffer not busy */
  if ( cy_fr_mode == CY_FR_RX_NEW_DATA ){
    cy_fr_fhal_write32( 3, CY_FR_REG_OBCM );   /* Read Data and Header Section */
  }
  else if ( cy_fr_mode == CY_FR_RX_STATUS_CHANGE ){
    cy_fr_fhal_write32( 1, CY_FR_REG_OBCM );   /* Only Header Section */
  }
  else if ( cy_fr_mode == CY_FR_RX_DATA_ONLY ){
    cy_fr_fhal_write32( 2, CY_FR_REG_OBCM );   /* Only Header Section */
  }
  cy_fr_fhal_write32((0x00000200 + cy_fr_fifo), CY_FR_REG_OBCR );   /* Request buffer and start transfer */
  cy_fr_fhal_wait_till_cleared32 ( 0x00008000 , CY_FR_REG_OBCR );   /* wait for output buffer not busy */
  cy_fr_fhal_write32 ( 0x00000100             , CY_FR_REG_OBCR );   /* view requested buffer */
  if ( ( cy_fr_mode == CY_FR_RX_NEW_DATA ) || ( cy_fr_mode == CY_FR_RX_DATA_ONLY ) ){
    cy_fr_ccal_data_from_buffer(base, cy_fr_data, cy_fr_data_length);   /* copy the data section to global variable pointed by *cy_fr_data */   
  }
  if ( ( cy_fr_mode == CY_FR_RX_NEW_DATA ) || ( cy_fr_mode == CY_FR_RX_STATUS_CHANGE ) ){
    cy_fr_buffer_header->RDHS1 = cy_fr_fhal_read32(CY_FR_REG_RDHS1);/* copy the header section */
    cy_fr_buffer_header->RDHS2 = cy_fr_fhal_read32(CY_FR_REG_RDHS2);/*  into the global structur */
    cy_fr_buffer_header->RDHS3 = cy_fr_fhal_read32(CY_FR_REG_RDHS3);/*  pointed by *cy_fr_buffer_header */
    cy_fr_buffer_header->MBS   = cy_fr_fhal_read32(CY_FR_REG_MBS  );/*  RHDSn = Header Section n; MBS = message buffer status */
  } else {
    cy_fr_buffer_header->RDHS1 = 0;
    cy_fr_buffer_header->RDHS2 = 0;
    cy_fr_buffer_header->RDHS3 = 0;
    cy_fr_buffer_header->MBS   = 0;
  }

  return CY_FR_OKAY; 
}   



/*---------------------------------------------------------------------------------*/
/** \brief      This function check if new data received or Message Buffer Changed 
                for the message buffer 'cy_fr_buffer'.

    \param[in]  base                 Pointer to FlexRay register base
    \param[in]  cy_fr_buffer         The number of the buffer.
    
    \retval     CY_FR_RX_NEW_DATA       If the new data flag is set.
    \retval     CY_FR_RX_STATUS_CHANGE  In case only the Message buffer change flag is set.
    \retval     CY_FR_RX_NO_NEW_FRAME   If no new frame was received on this buffer.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RX_STATUS_MODE_TYPE cy_fr_ccal_new_rx_data ( volatile stc_FLEXRAY_t * base,
                                                   CY_FR_BUFFER_TYPE        cy_fr_buffer )
{
  uint32_t cy_fr_ndat = 0;
  uint32_t cy_fr_mbsc = 0;

  if ( cy_fr_buffer <= 31 ){
    cy_fr_ndat = cy_fr_fhal_read32 ( CY_FR_REG_NDAT1 );
    cy_fr_mbsc = cy_fr_fhal_read32 ( CY_FR_REG_MBSC1 );
  } else if ( cy_fr_buffer <= 63 ){
    cy_fr_ndat = cy_fr_fhal_read32 ( CY_FR_REG_NDAT2 );
    cy_fr_mbsc = cy_fr_fhal_read32 ( CY_FR_REG_MBSC2 );
    cy_fr_buffer -= 32;
  } else if ( cy_fr_buffer <= 95 ){
    cy_fr_ndat = cy_fr_fhal_read32 ( CY_FR_REG_NDAT3 );
    cy_fr_mbsc = cy_fr_fhal_read32 ( CY_FR_REG_MBSC3 );
    cy_fr_buffer -= 64;
  } else if ( cy_fr_buffer <= 127 ){
    cy_fr_ndat = cy_fr_fhal_read32 ( CY_FR_REG_NDAT4 );
    cy_fr_mbsc = cy_fr_fhal_read32 ( CY_FR_REG_MBSC4 );
    cy_fr_buffer -= 96;
  }
  if ( ( cy_fr_ndat >> cy_fr_buffer ) & 1 ){
    return CY_FR_RX_NEW_DATA; 
  }
  if ( ( cy_fr_mbsc >> cy_fr_buffer ) & 1 ){
    return CY_FR_RX_STATUS_CHANGE;
  }
  return CY_FR_RX_NO_NEW_FRAME;
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks if new data received on fifo. 

    \param[in]  base                 Pointer to FlexRay register base
    
    \retval     CY_FR_RX_NEW_DATA       If the new data flag is set.
    \retval     CY_FR_RX_NO_NEW_FRAME   If no new frame was received on this buffer. 
*/
/*---------------------------------------------------------------------------------*/
  CY_FR_RX_STATUS_MODE_TYPE cy_fr_ccal_new_rx_data_fifo ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_FSR) & 0x1 ) == 0x1 ){
    return CY_FR_RX_NEW_DATA;       
  } else {
    return CY_FR_RX_NO_NEW_FRAME;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This Function checks if the FiFo not empty flag in the FiFo status
                Register ( FSR ) is set or not.
    
    \param[in]  base         Pointer to FlexRay register base
    
    \retval     CY_FR_TRUE   The flag is set.
    \retval     CY_FR_FALSE  The flag is not set.
 */
/*----------------------------------------------------------------------------------*/    
CY_FR_RETURN_TYPE cy_fr_ccal_fifo_is_not_empty ( volatile stc_FLEXRAY_t * base )
{                
  if ( ( cy_fr_fhal_read32(CY_FR_REG_FSR) & 0x1 ) == 1 ){
    return CY_FR_TRUE;       
  } else {
    return CY_FR_FALSE;
  }
}                
 
             
/*---------------------------------------------------------------------------------*/
/** \brief      This Function checks if the FiFo critical level flag in the FiFo 
                status register ( FSR ) is set or not.

    \param[in]  base         Pointer to FlexRay register base
    \retval     CY_FR_TRUE   The flag is set.
    \retval     CY_FR_FALSE  The flag is not set .
 */
/*----------------------------------------------------------------------------------*/    
CY_FR_RETURN_TYPE cy_fr_ccal_fifo_is_critical ( volatile stc_FLEXRAY_t * base )
{                
  if ( ( ( cy_fr_fhal_read32(CY_FR_REG_FSR) & 0x2 ) >> 1 ) == 1 ){
    return CY_FR_TRUE;       
  } else {
    return CY_FR_FALSE;
  }
}
 
           
/*---------------------------------------------------------------------------------*/
/** \brief      This Function checks if the FiFo overrun flag in the FiFo status 
                register ( FSR ) is set or not.
    
    \param[in]  base         Pointer to FlexRay register base
    
    \retval     CY_FR_TRUE   The flag is set
    \retval     CY_FR_FALSE  The flag is not set
 */
/*----------------------------------------------------------------------------------*/    
CY_FR_RETURN_TYPE cy_fr_ccal_fifo_is_overrun ( volatile stc_FLEXRAY_t * base )
{                
  if ( ( ( cy_fr_fhal_read32(CY_FR_REG_FSR) & 0x4 ) >> 2 ) == 1 ){
    return CY_FR_TRUE;       
  } else {
    return CY_FR_FALSE;
  }             
}


/*---------------------------------------------------------------------------------*/
/** \brief      This Function reads out the FiFo fill level of the FiFo status 
                register ( FSR ).
                
    \param[in]  base     Pointer to FlexRay register base
    
    \retval     uint8_t  The number of fill buffer of the fifo .
 */
/*----------------------------------------------------------------------------------*/    
uint8_t cy_fr_ccal_get_fifo_fill ( volatile stc_FLEXRAY_t * base )
{
  return ( ( cy_fr_fhal_read32(CY_FR_REG_FSR) & 0xFF00 ) >> 8 );
}


/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


