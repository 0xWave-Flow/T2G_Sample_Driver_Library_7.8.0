/***************************************************************************//**
* \file cy_fr_ccal_control_service.c
* \version 1.0
*
* \brief
* Routines for the control Service
*
* This File contains all routines for the control service of the FlexRay Communication
* Controller.
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
#include "flexray/ccal/cy_fr_ccal_status_service.h"
#include "flexray/fhal/cy_fr_fhal.h"


/*---------------------------------------------------------------------------------*/
/** \brief      This function send the Protocol Operation Control Command 'cy_fr_cmd'
                to the FlexRay Communication Controller.

    \param[in]  base         Pointer to FlexRay register base
    \param[in]  cy_fr_cmd    The Protocol Operation Control Command (A list of all 
                             allowed values define inf frd_api_global.h at section
                             'CY_FR_POC_COMMAND_TYPE' ).

    \retval     CY_FR_OKAY                  In case of Command accepted.
    \retval     CY_FR_COMMAND_NOT_ACCEPTED  In case of command not accepted.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_poc_command ( volatile stc_FLEXRAY_t * base,
                                           CY_FR_POC_COMMAND_TYPE   cy_fr_cmd )
{
  while( ( cy_fr_fhal_read32(CY_FR_REG_SUCC1) & 0x80 ) == 0x80 );   /* check if POC Busy flag*/
  cy_fr_fhal_write32((cy_fr_cmd + (cy_fr_fhal_read32(CY_FR_REG_SUCC1)&0xfffffff0)), CY_FR_REG_SUCC1);
  while( ( cy_fr_fhal_read32(CY_FR_REG_SUCC1) & 0x80 ) == 0x80 );   /* check if POC Busy flag*/
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SUCC1) & 0xF) == cy_fr_cmd ){  /* check if command was accepted */
    if ( cy_fr_cmd == CY_FR_POCC_CLEAR_RAMS ){                      /* In case of Clear RAMs command */
      while ( cy_fr_ccal_is_clear_rams_active(base) == CY_FR_TRUE );/* wait till clear rams end */
      return CY_FR_OKAY;                                            /* */
    } else if ( cy_fr_cmd == CY_FR_POCC_HALT ){                     /* In case of HALT command */
      if ( ( cy_fr_fhal_read32(CY_FR_REG_CCSV) & 0x80 ) == 0 ){     /* check if halt Request is set */
        return CY_FR_ERROR;                                         /* In case of no halt request was set */
      } else {                                                      /* */
        return CY_FR_OKAY;                                          /* halt request was set */
      }                                                             /* */
    } else if ( cy_fr_cmd == CY_FR_POCC_FREEZE ){                   /* In case of FREEZE command */
      if ( ( cy_fr_fhal_read32(CY_FR_REG_CCSV) & 0x40 ) == 0 ){     /* check if Freeze indicator is set */
        return CY_FR_ERROR;                                         /* In case FREEZE indicator is not set */
      } else {                                                      /* */
        return CY_FR_OKAY;                                          /* FREEZE indicator is set */
      }                                                             /* */
    } else {                                                        /* */
      return CY_FR_OKAY;                                            /* for all other commands */
    }                                                               /* */
  } else if ( ( cy_fr_fhal_read32(CY_FR_REG_SUCC1) & 0xF) == 0 ){   /* if command was not accepted by FRCC */
    return CY_FR_COMMAND_NOT_ACCEPTED;                              /* */
  } else {                                                          /* In case of an unknown error */
    return CY_FR_ERROR;                                             /* */
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief       This function write unlock sequence to the Lock Register ( LCK ) and 
                 send the Protocol Operation Control Command 'CY_FR_POCC_READY' to the 
                 FlexRay Communication Controller.

    \param[in]  base                        Pointer to FlexRay register base
    
    \retval     CY_FR_OKAY                  In case of Command accepted.
    \retval     CY_FR_COMMAND_NOT_ACCEPTED  In case of command not accepted.
    \retval     CY_FR_ERROR                 If writing the command failed.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_unlock_config ( volatile stc_FLEXRAY_t * base,
                                             CY_FR_POC_COMMAND_TYPE   cy_fr_cmd )
{
  if( cy_fr_ccal_get_poc_status(base) == CY_FR_POCS_CONFIG){        /* check the if POC is config */
    cy_fr_fhal_write32(0x000000CE,CY_FR_REG_LCK);                   /* write the unlock sequence */
    cy_fr_fhal_write32(0x00000031,CY_FR_REG_LCK);                   /* to the LCK register to unlock */
  }                                                                 /* the config state */
  while( ( cy_fr_fhal_read32(CY_FR_REG_SUCC1) & 0x80 ) == 0x80 );   /* check if POC Busy flag*/
  cy_fr_fhal_write32((cy_fr_cmd + (cy_fr_fhal_read32(CY_FR_REG_SUCC1)&0xfffffff0)), CY_FR_REG_SUCC1); /* write command to SUCC1_CMD */
  while( ( cy_fr_fhal_read32(CY_FR_REG_SUCC1) & 0x80 ) == 0x80 );   /* check if POC Busy flag*/
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SUCC1) & 0xF) == cy_fr_cmd )   /* check if command was accepted */
    return CY_FR_OKAY;                                              /* in case of no error */
  else if ( ( cy_fr_fhal_read32(CY_FR_REG_SUCC1) & 0xF) == 0 )      /* check if COMMAND_NOT_ACCEPTED */
    return CY_FR_COMMAND_NOT_ACCEPTED;                              /* */
  else                                                              /* in case of an other error */
    return CY_FR_ERROR;                                             /* */
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Core Release Register by Bosch. The 
                BCD-coded register values are calculate to decimal values and stored
                in the 'CY_FR_CORE_RELEASE_STRUCT' pointed by '*cy_fr_release_struct'. The
                real register value used as return value of the function.

    \param[in]  base      Pointer to FlexRay register base
    
    \retval     uint32_t  The Core Release Register value.
*/
/*---------------------------------------------------------------------------------*/
uint32_t cy_fr_ccal_get_eray_version ( volatile stc_FLEXRAY_t *    base,
                                       CY_FR_CORE_RELEASE_STRUCT * cy_fr_release_struct)
{ 
  uint32_t cy_fr_temp,cy_fr_temp_reg;
  
  cy_fr_temp_reg = cy_fr_fhal_read32(CY_FR_REG_CREL);                   /* read register value */
  cy_fr_temp = cy_fr_temp_reg & 0xFF;                                   /* day value */
  cy_fr_temp = ( (cy_fr_temp >> 4 ) * 10 ) +  ( cy_fr_temp & 0x0F );    /* from BCD coded to 10based value */
  cy_fr_release_struct->DAY  = cy_fr_temp;                              /* write value to struct */
                                                                     
  cy_fr_temp = ( cy_fr_temp_reg & 0xFF00 ) >> 8;                        /* month value */
  cy_fr_temp = ( (cy_fr_temp >> 4 ) * 10 ) +  ( cy_fr_temp & 0x0F );    /* from BCD coded to 10based value */
  cy_fr_release_struct->MON  = cy_fr_temp;                              /* write value to struct */
                                                                     
  cy_fr_release_struct->YEAR = (cy_fr_temp_reg & 0xF0000 ) >> 16;       /* year value */
  
  cy_fr_temp = (cy_fr_temp_reg & 0xFF00000 ) >> 20;                     /* step value */
  cy_fr_temp = (( (cy_fr_temp >> 4 ) * 10 ) +  ( cy_fr_temp & 0x0F ) ); /* from BCD coded to 10based value */
  cy_fr_release_struct->STEP  = cy_fr_temp;                             /* write value to struct */
  
  cy_fr_release_struct->REL = (cy_fr_temp_reg & 0xF0000000 ) >> 28;     /* release value */

  return cy_fr_temp_reg;                                                /* return register value */
}




/*---------------------------------------------------------------------------------*/
/** \brief      This function enables the FlexRay controller
    
    \param[in]  base                  Pointer to FlexRay register base    
 */
/*---------------------------------------------------------------------------------*/
void cy_fr_ccal_enable_controller ( volatile stc_FLEXRAY_t * base ) 
{
  cy_fr_fhal_write32(0x80000000, CY_FR_REG_CTL);
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function disables the FlexRay controller

    \param[in]  base                  Pointer to FlexRay register base    
 */
/*---------------------------------------------------------------------------------*/
void cy_fr_ccal_disable_controller ( volatile stc_FLEXRAY_t * base ) 
{
  cy_fr_fhal_write32(0x00000000, CY_FR_REG_CTL);
}




/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


