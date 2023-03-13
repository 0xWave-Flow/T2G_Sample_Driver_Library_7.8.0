/***************************************************************************//**
* \file cy_fr_ccal_init.c
* \version 1.0
*
* \brief
* initialize the FlexRay Communication Controller
*
* This file contains all function needed to initialize the FlexRay Communication Controller
* with a chi file. 
* CHI file also contains the message buffer definitions All buffers are initialized.
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
#include "flexray/api/cy_fr_api_init.h"
#include "flexray/api/cy_fr_api_status_service.h"
#include "flexray/ccal/cy_fr_ccal_control_service.h"
#include "flexray/ccal/cy_fr_ccal_init.h"
#include "flexray/ccal/cy_fr_ccal_status_service.h"
#include "flexray/fhal/cy_fr_fhal.h"



/*----------------------------------------------------------------------------------
 * Global Variables
 *---------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/** \brief  Look up table that will store the frame ID and assigned channels for
            each buffer and for each IP instance.
 */
/*---------------------------------------------------------------------------------*/
uint16_t  cy_fr_lut_buffer_to_id[CY_IP_MXFLEXRAY_INSTANCES][CY_FR_MAX_BUFFERS][2];



/*----------------------------------------------------------------------------------
 * Function definitions
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/** \brief      This function initialize the FlexRay CC with a *.chi file. 
                At end of function the buffer
                checked which ID  and channel assigned to the buffers and save a list
                of the assignment to the global variable 'cy_fr_lut_buffer_to_id'.

    \param[in]  base                     Pointer to FlexRay register base
    \param[in]  cy_fr_chi_inc_func_ptr   Pointer to user provided function that
                                         has #included the CHI file in its body
    
    \retval     CY_FR_OKAY               Always, no error handling.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_init_chi( volatile stc_FLEXRAY_t *            base,
                                       CY_FR_CHI_INCLUDE_FUNCTION_PTR_TYPE cy_fr_chi_inc_func_ptr )
{

  if ( cy_fr_api_pocs_is_default_config(base) == CY_FR_TRUE ){   /* check POC Status is DEFAULT_CONFIG */
    cy_fr_ccal_poc_command(base, CY_FR_POCC_CONFIG);              /* if DEFAULT_CONFIG goto CONFIG */
  }
  if ( cy_fr_api_pocs_is_config(base) == CY_FR_TRUE ){           /* check POC Status is CONFIG */

    if ( cy_fr_chi_inc_func_ptr != (CY_FR_CHI_INCLUDE_FUNCTION_PTR_TYPE) 0 ){
      cy_fr_chi_inc_func_ptr(base);
    }    
    cy_fr_ccal_id_to_buffer(base);
    return CY_FR_OKAY;
  } else {
    return CY_FR_NOT_CONFIG;                       /* In case POC Status is not CONFIG */
  }
}




/*---------------------------------------------------------------------------------*/
/** \brief      This Function reads the Frame Id's and channels from all buffers and 
                write them in' cy_fr_lut_buffer_to_id[CY_FR_MAX_BUFFERS][2]'.
    
    \param[in]  base                  Pointer to FlexRay register base    
 */
/*---------------------------------------------------------------------------------*/
void cy_fr_ccal_id_to_buffer ( volatile stc_FLEXRAY_t * base )
{
  uint32_t cy_fr_mrc_temp;
  uint8_t  cy_fr_mrc_ffb, cy_fr_mrc_lcb, cy_fr_max_buffer, buffer_cnt=0;
  
  cy_fr_mrc_temp = cy_fr_fhal_read32(CY_FR_REG_MRC);  /* get message configuration register */
  cy_fr_mrc_lcb = ((cy_fr_mrc_temp & 0xff0000)>>16);  /* last configured buffer*/
  cy_fr_mrc_ffb = ((cy_fr_mrc_temp & 0x00ff00)>> 8);  /* first fifo buffer */
  if ( cy_fr_mrc_lcb < cy_fr_mrc_ffb ){
    cy_fr_max_buffer = cy_fr_mrc_lcb;
  } else {
    cy_fr_max_buffer = (cy_fr_mrc_ffb-1);
  }
  cy_fr_fhal_wait_till_cleared32 ( 0x00008000 , CY_FR_REG_OBCR );   /* wait for output buffer not busy */
  cy_fr_fhal_write32( 0x00000001              , CY_FR_REG_OBCM );   /* Read Header Section */
  cy_fr_fhal_write32( 0x00000200              , CY_FR_REG_OBCR );   /* Request buffer and start transfer */
  while(cy_fr_max_buffer>buffer_cnt){
    cy_fr_fhal_wait_till_cleared32 ( 0x00008000  , CY_FR_REG_OBCR );   /* wait for output buffer not busy */
    cy_fr_fhal_write32( 0x00000001               , CY_FR_REG_OBCM );   /* Read Header Section */
    cy_fr_fhal_write32((0x00000300+buffer_cnt+1) , CY_FR_REG_OBCR );   /* view buffer and Request next buffer and start transfer */  
    cy_fr_lut_buffer_to_id[CY_FR_GET_INSTANCE_FROM_BASE(base)][buffer_cnt][0] = (uint16_t) (cy_fr_fhal_read32(CY_FR_REG_RDHS1) & 0x000007FF);       /* Frame ID of the actual buffer */
    cy_fr_lut_buffer_to_id[CY_FR_GET_INSTANCE_FROM_BASE(base)][buffer_cnt][1] = (uint16_t)((cy_fr_fhal_read32(CY_FR_REG_RDHS1) & 0x03000000)>>24);  /* Assigned channel of the actual buffer */
    buffer_cnt++;
  }
  cy_fr_fhal_wait_till_cleared32 ( 0x00008000  , CY_FR_REG_OBCR );   /* wait for output buffer not busy */
  cy_fr_fhal_write32( 0x00000001 , CY_FR_REG_OBCM );                 /* Read Header Section */
  cy_fr_fhal_write32( 0x00000100 , CY_FR_REG_OBCR );                 /* view buffer and Request next buffer and start transfer */  
  cy_fr_lut_buffer_to_id[CY_FR_GET_INSTANCE_FROM_BASE(base)][buffer_cnt][0] = (uint16_t) (cy_fr_fhal_read32(CY_FR_REG_RDHS1) & 0x000007FF);       /* Frame ID of the actual buffer */
  cy_fr_lut_buffer_to_id[CY_FR_GET_INSTANCE_FROM_BASE(base)][buffer_cnt][1] = (uint16_t)((cy_fr_fhal_read32(CY_FR_REG_RDHS1) & 0x03000000)>>24);  /* Assigned channel of the actual buffer */
   
}




#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


