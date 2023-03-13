/***************************************************************************//**
* \file cy_fr_api_time_service.c
* \version 1.0
*
* \brief
* User Interface for Time Service
*
* This File contains the user interface for time service. With the time service
* you can read/write the FlexRay Communication Controller time registers.
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
#include "flexray/ccal/cy_fr_ccal_time_service.h"


/*---------------------------------------------------------------------------------*/
/** \brief      This function check the Protocol Operation Controll status and if it
                is not CONFIG or DEFAULT_CONFIG, 'cy_fr_ccal_get_mtick()' is call 
                which reads the actual macrotick value from the FlexRay Communication 
                Controller. In Error case it returns '0'.
                
    \param[in]  base          Pointer to FlexRay register base
    
    \retval     CY_FR_UINT16  The actual macrotick value.
*/
/*---------------------------------------------------------------------------------*/
uint16_t cy_fr_api_get_mtick ( volatile stc_FLEXRAY_t * base )
{
  if ( (cy_fr_api_pocs_is_config(base)         == CY_FR_FALSE) && 
       (cy_fr_api_pocs_is_default_config(base) == CY_FR_FALSE) ){
    return cy_fr_ccal_get_mtick(base);
  } else {
    return 0;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      Check the Protocol Operation Controll status and if its not CONFIG
                or DEFAULT_CONFIG, cy_fr_ccal_get_time is call which reads the actual
                macrotick value from the FlexRay Communication Controller and
                calculate the global time. In Error case it returns '0'.

    \param[in]  base      Pointer to FlexRay register base
    
    \retval     uint32_t  The global time in µsec ( approximated )
*/
/*---------------------------------------------------------------------------------*/
uint32_t cy_fr_api_get_time ( volatile stc_FLEXRAY_t * base )
{
  if ( (cy_fr_api_pocs_is_config(base)         == CY_FR_FALSE) && 
       (cy_fr_api_pocs_is_default_config(base) == CY_FR_FALSE) ){
    return cy_fr_ccal_get_time(base);
  } else {
    return 0;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the actual Cycle no ( MTCCV_CCV )

    \param[in]  base         Pointer to FlexRay register base
    
    \retval     CY_FR_UINT8  The actual Cycle no.
*/
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_api_get_cycle ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_cycle(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief      Read out the actual Slot Counter ( SCV )

    \param[in]  base          Pointer to FlexRay register base
    \param[in]  cy_fr_channel The channel for the slot counter.

    \retval     CY_FR_UINT8   The actual Slot Counter of 'cy_fr_channel'
*/
/*---------------------------------------------------------------------------------*/
uint16_t cy_fr_api_get_slot_counter ( volatile stc_FLEXRAY_t * base,
                                      CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  if ( cy_fr_channel == CY_FR_CHANNEL_A ){
    return cy_fr_ccal_get_slot_counter_a(base);
  } else if ( cy_fr_channel == CY_FR_CHANNEL_B ){
    return cy_fr_ccal_get_slot_counter_b(base);
  } else {
    return CY_FR_INVALID_CHANNEL;
  }
}


#if defined(__cplusplus)
}
#endif


/* [] END OF FILE */
