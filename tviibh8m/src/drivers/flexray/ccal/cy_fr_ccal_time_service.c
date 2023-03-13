/***************************************************************************//**
* \file cy_fr_ccal_time_service.c
* \version 1.0
*
* \brief
* Routines for the Time Service.
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


/*---------------------------------------------------------------------------------*/
/** \brief     This function read the actual macrotick value from the FlexRay CC. 
               Attend to that this time is read out asyncron.

    \param[in]  base                Pointer to FlexRay register base
    
    \retval     CY_FR_UINT16        The actual macrotick value.
*/
/*---------------------------------------------------------------------------------*/
uint16_t cy_fr_ccal_get_mtick ( volatile stc_FLEXRAY_t * base )
{
  return (cy_fr_fhal_read32(CY_FR_REG_MTCCV) & 0x3FFF);
}


/*---------------------------------------------------------------------------------*/
/** \brief     This function reads the actual macrotick value ( MTCCV_MTV )from
               the FlexRay CC and calculate the global time.  Attend to that this 
               time is read out asyncron an it need time to calculate, the time is 
               needed is strongly different from your MCU settings. To get the global 
               time faster use cy_fr_api_get_mtick () or use the Stop Watch function of 
               the FlexRay Controller. If you made your FlexRay Cycle setting with 1 
               Macrotick per µsec you can use cy_fr_api_get_mtick () instead of 
               cy_fr_api_get_time ().

    \param[in]  base            Pointer to FlexRay register base
    \retval     uint32_t        The global time in µsec ( approximated ).
*/
/*---------------------------------------------------------------------------------*/
uint32_t cy_fr_ccal_get_time ( volatile stc_FLEXRAY_t * base )
{
  uint32_t cy_fr_mtick, cy_fr_baudrate, cy_fr_sclk_div, cy_fr_mtick_cycle, cy_fr_utick_cycle, cy_fr_temp;
  
  cy_fr_mtick       =  ( cy_fr_fhal_read32(CY_FR_REG_MTCCV) & 0x3FFF );        /* actual macrotick value  */
  cy_fr_baudrate    = (( cy_fr_fhal_read32(CY_FR_REG_PRTC1) & 0x8000 ) >> 15); /* Baudrate Prescaler upper bit */
  cy_fr_utick_cycle =    cy_fr_fhal_read32(CY_FR_REG_GTUC1);                   /* Microticks per Cycle */
  cy_fr_mtick_cycle =  ( cy_fr_fhal_read32(CY_FR_REG_GTUC2) & 0x3FFF );        /* Macroticks per Cycle */

  cy_fr_sclk_div    = 0;

  if(cy_fr_baudrate != 0){                                      /* Baudrate Prescaler BRP[1:0] [00] or [01] */
    cy_fr_temp = cy_fr_utick_cycle * (25*(1<<cy_fr_sclk_div));  /* mycrotick per cycle * (1/sclk) = cycle length */
  } else {                                                      /* Baudrate Prescaler BRP[1:0] [10] or [11] */
    cy_fr_temp = cy_fr_utick_cycle * (50*(1<<cy_fr_sclk_div));  /* mycrotick per cycle * 50ns = cycle length */
  }
  cy_fr_temp = cy_fr_temp / cy_fr_mtick_cycle;   /* cycle length over macroticks per cycle = macrotick length */
  cy_fr_temp = cy_fr_temp * cy_fr_mtick;         /* time in ns till cycle start */
  cy_fr_temp = cy_fr_temp / 1000;                /* time in µs till cycle start */
  return cy_fr_temp;                             /* return global time */
}


/*---------------------------------------------------------------------------------*/
/** \brief     This function reads out the actual Cycle no ( MTCCV_CCV ).

    \param[in]  base                Pointer to FlexRay register base
    
    \retval     CY_FR_UINT8         The actual Cycle no.
*/
/*---------------------------------------------------------------------------------*/
uint8_t  cy_fr_ccal_get_cycle ( volatile stc_FLEXRAY_t * base )
{
  return ( ( cy_fr_fhal_read32( CY_FR_REG_MTCCV ) & 0x3F0000) >> 16);
}


/*---------------------------------------------------------------------------------*/
/** \brief     This function reads out the actual Slot Counter of channel A ( SCV_SCCA ).

    \param[in]  base                Pointer to FlexRay register base
    
    \retval     CY_FR_UINT16        The actual Slot Counter of Channel A
*/
/*---------------------------------------------------------------------------------*/
uint16_t cy_fr_ccal_get_slot_counter_a ( volatile stc_FLEXRAY_t * base )
{
  return ( cy_fr_fhal_read32( CY_FR_REG_SCV ) & 0x7FF);
}


/*---------------------------------------------------------------------------------*/
/** \brief     This funciton reads out the actual Slot Counter of channel B ( SCV_SCCB ).

    \param[in]  base                Pointer to FlexRay register base
    
    \retval     CY_FR_UINT16        The actual Slot Counter of Channel B
*/       
/*---------------------------------------------------------------------------------*/
uint16_t cy_fr_ccal_get_slot_counter_b ( volatile stc_FLEXRAY_t * base )
{
  return ( ( cy_fr_fhal_read32( CY_FR_REG_SCV ) & 0x7FF0000) >> 16);
}


#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


