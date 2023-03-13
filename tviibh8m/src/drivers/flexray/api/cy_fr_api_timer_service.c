/***************************************************************************//**
* \file cy_fr_api_timer_service.c
* \version 1.0
*
* \brief
* User Interface for Timer Service
*
* This File contains the user interface for timer service. With the timer service
* you can start,stop and reconfigure the Timer 0 and 1,and reconfigure the Stop
* Watch.
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
#include "flexray/ccal/cy_fr_ccal_timer_service.h"


/*---------------------------------------------------------------------------------*/
/** \brief      This function starts the timer 'cy_fr_timer'.

    \param[in]  base          Pointer to FlexRay register base
    \param[in]  cy_fr_timer   Number of the timer should started.

    \retval     CY_FR_OKAY    If the timer was started.
    \retval     CY_FR_ERROR   In case the timer run control bit is not set.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_start_timer ( volatile stc_FLEXRAY_t * base,
                                         CY_FR_TIMER_TYPE          cy_fr_timer )
{
  if ( (cy_fr_api_pocs_is_normal_active(base)  == CY_FR_TRUE) ||
       (cy_fr_api_pocs_is_normal_passive(base) == CY_FR_TRUE) ){
    if(cy_fr_timer == CY_FR_TIMER_T0){
      return cy_fr_ccal_start_timer0 ( base );
    } else if(cy_fr_timer == CY_FR_TIMER_T1){
      return cy_fr_ccal_start_timer1 ( base );
    } else {
      return CY_FR_INVALID_TIMER;
    }
  } else {
    return CY_FR_NOT_ACTIVE ;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function stops the timer 'cy_fr_timer'.

    \param[in]  base          Pointer to FlexRay register base
    \param[in]  cy_fr_timer   Number of the timer should stopped.

    \retval     CY_FR_OKAY    If the timer was stopped.
    \retval     CY_FR_ERROR   In case the timer run control bit is not cleared.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_stop_timer ( volatile stc_FLEXRAY_t * base,
                                         CY_FR_TIMER_TYPE         cy_fr_timer )
{
  if(cy_fr_timer == CY_FR_TIMER_T0){
    return cy_fr_ccal_stop_timer0 ( base );
  }
  else if(cy_fr_timer == CY_FR_TIMER_T1){
    return cy_fr_ccal_stop_timer1 ( base );
  }
  else
    return CY_FR_INVALID_TIMER;
}


/*---------------------------------------------------------------------------------*/
/** \brief      This Function re initialize the Timer 0.

    \param[in]  base              Pointer to FlexRay register base
    \param[in]  cy_fr_mt_offset   New value for Macrotick offset.
    \param[in]  cy_fr_cycle_code  New value for Cycle Code.
    \param[in]  cy_fr_timer_mode  New Value for Timer Mode.

    \retval     CY_FR_OKAY        If the new setting was accepted.
    \retval     CY_FR_ERROR       In case the new settings not accepted.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_set_timer0 ( volatile stc_FLEXRAY_t * base,
                                         uint32_t                 cy_fr_mt_offset,
                                         uint8_t                  cy_fr_cycle_code,
                                         uint8_t                  cy_fr_timer_mode )
{
  return cy_fr_ccal_set_timer0 ( base, cy_fr_mt_offset, cy_fr_cycle_code, cy_fr_timer_mode );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function re initialize the Timer 1.

    \param[in]  base              Pointer to FlexRay register base
    \param[in]  cy_fr_mt_count    New value for macro count.
    \param[in]  cy_fr_timer_mode  New value for timer mode.

    \retval     CY_FR_OKAY        If the new setting was accepted.
    \retval     CY_FR_ERROR       In case the new settings not accepted.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_set_timer1 ( volatile stc_FLEXRAY_t * base,
                                         uint16_t                 cy_fr_mt_count,
                                         uint8_t                  cy_fr_timer_mode )
{
  return cy_fr_ccal_set_timer1 ( base, cy_fr_mt_count, cy_fr_timer_mode );
}


/*-------------------------------------------------------------------------------*/
/** \brief      This function reconfiguration of Stop Watch.

    \param[in]  base               Pointer to FlexRay register base
    \param[in]  cy_fr_edge_select  New Value for the edge selection.
    \param[in]  cy_fr_stpw_mode    New Value for Stop Watch mode.

    \retval     CY_FR_OKAY         If the new setting was accepted.
    \retval     CY_FR_ERROR        In case the new settings not accepted.
 */
/*-------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_set_stpw ( volatile stc_FLEXRAY_t * base,
                                       uint8_t                  cy_fr_edge_select,
                                       uint8_t                  cy_fr_stpw_mode )
{
  return cy_fr_ccal_set_stpw ( base, cy_fr_edge_select, cy_fr_stpw_mode );
}


/*-------------------------------------------------------------------------------*/
/** \brief      This function reads out the Stop Watch registers 1 and 2 into a
                structure from type 'CY_FR_STOP_WATCH_STRUCT'.

    \param[in]  base                     Pointer to FlexRay register base
    \param[in]  cy_fr_stop_watch_struct  Pointer to a structure for the caught
                                         values.

    \retval     CY_FR_OKAY               Always, no error handling.
 */
/*-------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_get_stpw ( volatile stc_FLEXRAY_t *  base,
                                       CY_FR_STOP_WATCH_STRUCT * cy_fr_stop_watch_struct)
{
  return cy_fr_ccal_get_stpw ( base, &(*cy_fr_stop_watch_struct) );
}


/*-------------------------------------------------------------------------------*/
/** \brief      This function starts capturing with the Stop Watch function In case 
                of software trigger directly else wait for selected trigger signal.

    \param[in]  base                  Pointer to FlexRay register base
    \param[in]  cy_fr_trigger_source  Selects  the trigger pin of the Stop Watch.

    \retval     CY_FR_OKAY            Always, no error handling.
 */
/*-------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_start_stpw ( volatile stc_FLEXRAY_t *      base,
                                         CY_FR_STOP_WATCH_TRIGGER_TYPE cy_fr_trigger_source)
{
  return cy_fr_ccal_start_stpw ( base, cy_fr_trigger_source );
}


#if defined(__cplusplus)
}
#endif


/* [] END OF FILE */
