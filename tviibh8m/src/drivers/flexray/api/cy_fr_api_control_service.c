/***************************************************************************//**
* \file cy_fr_api_control_service.c
* \version 1.0
*
* \brief
* User Interface for Control Service
*
* This File contains the user interface for the control service. This service 
* contain a function to switch the protocol operation control status
* cy_fr_api_poc_command(), a function for fast startup the FlexRay CC
* cy_fr_api_go_active(), and a function to read out the Version Register of
* the FlexRay CC: cy_fr_api_get_eray_version()
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
#include "flexray/ccal/cy_fr_ccal_control_service.h"


/*---------------------------------------------------------------------------------*/
/** \brief     This Function sends Protocol Operation Control Command (POCC) to the
               FlexRay CC if the Command is 'CY_FR_POCC_READY' or 'CY_FR_POCC_MONITOR_MODE'
               the unlock function is called to Unlock the Configuration mode if needed.

    \param[in] base         Pointer to FlexRay register base
    \param[in] cy_fr_cmd    a Protocol Operation Control Command ( a list of all
                            allowed values define in cy_fr_api_global.h at section
                            'CY_FR_POC_COMMAND_TYPE' )

    \retval    CY_FR_OKAY                  If command was accepted by the FlexRay CC
    \retval    CY_FR_COMMAND_NOT_ACCEPTED  In case the FlexRay CC did not accept the
                                           last command
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_poc_command ( volatile stc_FLEXRAY_t * base,
                                          CY_FR_POC_COMMAND_TYPE   cy_fr_cmd)
{
  if ( ( cy_fr_api_pocs_is_config(base) == CY_FR_TRUE) && /* if POC Status is CONFIG and */
     ( ( cy_fr_cmd == CY_FR_POCC_READY ) ||               /* if POC Command is READY or */
       ( cy_fr_cmd == CY_FR_POCC_MONITOR_MODE ) ) ){      /* POC Command is MONITOR_MODE */
    return cy_fr_ccal_unlock_config(base, cy_fr_cmd);     /* Unlock CONFIG status */
  }                                                     
  return cy_fr_ccal_poc_command(base, cy_fr_cmd);         /* else write POC Command and return*/
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function startup the FlexRay CC In case the is in CONFIG status
                ( if you use cy_fr_api_init_chi ( ) or cy_fr_api_init_man ( ) and they
                return no error the FlexRay CC is in CONFIG status) the Config status
                will be unlock and the FlexRay CC enter READY status. If the command
                was accepted by the FlexRay CC, then the host send the RUN command to
                startup the FlexRay Communication.

    \param[in]  base                        Pointer to FlexRay register base
    
    \retval     CY_FR_OKAY                  In case of no Error.
    \retval     CY_FR_COMMAND_NOT_ACCEPTED  If command Ready or RUN was not accepted by
                                            the FlexRay CC.
    \retval     CY_FR_NOT_ACTIVE            In case the FlexRay CC can not reached the
                                            NORMAL_ACTIVE status.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_go_active ( volatile stc_FLEXRAY_t * base )
{
  uint32_t cy_fr_wait = 0xFFFFF;                                          /* wait value */

  if ( cy_fr_api_pocs_is_config(base) == CY_FR_TRUE ){                    /* do only if FlexRay CC is in CONFIG */
    if (cy_fr_ccal_unlock_config(base, CY_FR_POCC_READY) != CY_FR_OKAY){  /* unlock CONFIG status and goto READY */
      return CY_FR_COMMAND_NOT_ACCEPTED;                                  /* return if READY was not accepted */
    }                                                                     /* */
    if ( cy_fr_ccal_poc_command(base, CY_FR_POCC_RUN) != CY_FR_OKAY){     /* startup the FleyRay CC */
      return CY_FR_COMMAND_NOT_ACCEPTED;                                  /* return if RUN was not accepted */
    }                                                                     /* */
    while(cy_fr_wait){                                                    /* wait loop to give the FlexRay CC */
      cy_fr_wait--;                                                       /* a change to sync up to the bus */
    }                                                                     /* before checking the active state */
    if ( cy_fr_api_get_poc_status(base) == CY_FR_POCS_NORMAL_ACTIVE ){    /* do only if FlexRay CC has entered NORMAL_ACTIVE status */
      return CY_FR_OKAY;                                                  /* return In case of success */
    } else {                                                              /* */
      return CY_FR_NOT_ACTIVE;                                            /* return In case NORMAL_ACTIVE status not reached */
    }
  } else {
    return CY_FR_NOT_CONFIG;                                              /* return In case FlexRay CC is not in Config status */
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Core Release Register ( CREL ) of the 
                E-Ray from Bosch. The function returns the 32 bit register value and 
                computes the Core Release Version and keep it to the 
                'CY_FR_CORE_RELEASE_STRUCT'.

    \param[in]  base                    Pointer to FlexRay register base
    \param[out] *cy_fr_release_struct   A pointer to a CY_FR_CORE_RELEASE_STRUCT where
                                        the caluated decimal values are stored in.

    \retval     uint32_t                The Core Release Register value ( CREL ).
*/
/*---------------------------------------------------------------------------------*/
uint32_t cy_fr_api_get_eray_version ( volatile stc_FLEXRAY_t *    base,
                                      CY_FR_CORE_RELEASE_STRUCT * cy_fr_release_struct)
{
  return cy_fr_ccal_get_eray_version(base, &(*cy_fr_release_struct)); /* read the Core Release Register and return */
}



/*---------------------------------------------------------------------------------*/
/** \brief      This function enables the FlexRay controller

    \param[in]  base                  Pointer to FlexRay register base    
 */
/*---------------------------------------------------------------------------------*/
void cy_fr_api_enable_controller ( volatile stc_FLEXRAY_t * base ) 
{
  cy_fr_ccal_enable_controller(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function disables the FlexRay controller

    \param[in]  base                  Pointer to FlexRay register base    
 */
/*---------------------------------------------------------------------------------*/
void cy_fr_api_disable_controller ( volatile stc_FLEXRAY_t * base ) 
{
  cy_fr_ccal_disable_controller(base);
}



#if defined(__cplusplus)
}
#endif


/* [] END OF FILE */
