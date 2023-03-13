/***************************************************************************//**
* \file cy_fr_ccal_timer_service.c
* \version 1.0
*
* \brief
* Routines for the Timer Service.
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
/** \brief      This function re initialize the Timer 0 with 'cy_fr_mt_offset',
                'cy_fr_cycle_code' and 'cy_fr_timer_mode'

    \param[in]  base              Pointer to FlexRay register base
    \param[in]  cy_fr_mt_offset   New value for Macrotick offset.
    \param[in]  cy_fr_cycle_code  New value for Cycle Code.
    \param[in]  cy_fr_timer_mode  New Value for Timer Mode.

    \retval     CY_FR_OKAY        If new values successful set.
    \retval     CY_FR_ERROR       In case of error.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_set_timer0 ( volatile stc_FLEXRAY_t * base,
                                          uint32_t                 cy_fr_mt_offset,
                                          uint8_t                  cy_fr_cycle_code,
                                          uint8_t                  cy_fr_timer_mode )
{
  uint32_t temp_reg = 0;

  temp_reg  = cy_fr_fhal_read32(CY_FR_REG_T0C);
  temp_reg &= 0x00000001;
  temp_reg += (cy_fr_mt_offset  << 16);
  temp_reg += (cy_fr_cycle_code <<  8);
  temp_reg += (cy_fr_timer_mode <<  1);
  cy_fr_fhal_write32(temp_reg,CY_FR_REG_T0C);
  if ((cy_fr_fhal_read32(CY_FR_REG_T0C) & 0x03FFF7F02) == temp_reg){
    return CY_FR_OKAY;
  } else {
    return CY_FR_ERROR;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function re initialize the Timer 1 with 'cy_fr_mt_count',
                and 'cy_fr_timer_mode'

    \param[in]  base              Pointer to FlexRay register base
    \param[in]  cy_fr_mt_count    New Value for Macrotick Count.
    \param[in]  cy_fr_timer_mode  New Value for Timer Mode.

    \retval     CY_FR_OKAY        If new values successful set.
    \retval     CY_FR_ERROR       In case of error.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_set_timer1 ( volatile stc_FLEXRAY_t * base,
                                          uint32_t                 cy_fr_mt_count,
                                          uint8_t                  cy_fr_timer_mode)
{
  cy_fr_fhal_write32(((cy_fr_mt_count << 16)+(cy_fr_timer_mode << 1)),CY_FR_REG_T1C);

  if ((cy_fr_fhal_read32(CY_FR_REG_T1C) & 0x03FFF0002) ==
       ((((uint32_t)cy_fr_mt_count) << 16) + (cy_fr_timer_mode << 1))){
    return CY_FR_OKAY;
  } else {
    return CY_FR_ERROR;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function starts the timer 0.

    \param[in]  base              Pointer to FlexRay register base
    
    \retval     CY_FR_OKAY        If start of timer 0 was successful.
    \retval     CY_FR_ERROR       In case timer 0 not started.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_start_timer0 ( volatile stc_FLEXRAY_t * base )
{
  cy_fr_fhal_write32(((cy_fr_fhal_read32(CY_FR_REG_T0C)&0xFFFFFFFE)+1),CY_FR_REG_T0C);

  if ( (cy_fr_fhal_read32(CY_FR_REG_T0C) & 0x1) == 1 ){
    return CY_FR_OKAY;
  } else {
    return CY_FR_ERROR;
  }
}


/*----------------------------------------------------------------------------------*/
/** \brief      This function starts the timer 1.

    \param[in]  base              Pointer to FlexRay register base
    
    \retval     CY_FR_OKAY        If start of timer 1 was successful.
    \retval     CY_FR_ERROR       In case timer 1 not started.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_start_timer1 ( volatile stc_FLEXRAY_t * base )
{
  cy_fr_fhal_write32(((cy_fr_fhal_read32(CY_FR_REG_T1C)&0xFFFFFFFE)+1),CY_FR_REG_T1C);

    if ( (cy_fr_fhal_read32(CY_FR_REG_T1C) & 0x1) == 1 ){
    return CY_FR_OKAY;
  } else {
    return CY_FR_ERROR;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function stops the timer 0.

    \param[in]  base              Pointer to FlexRay register base
    
    \retval     CY_FR_OKAY        If stop of timer 0 was successful.
    \retval     CY_FR_ERROR       In case timer 0 not stopped.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_stop_timer0 ( volatile stc_FLEXRAY_t * base )
{
  cy_fr_fhal_write32((cy_fr_fhal_read32(CY_FR_REG_T0C)&0xFFFFFFFE),CY_FR_REG_T0C);

  if ( (cy_fr_fhal_read32(CY_FR_REG_T0C) & 0x1) == 0 ){
    return CY_FR_OKAY;
  } else {
    return CY_FR_ERROR;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function stops the timer 1.

    \param[in]  base              Pointer to FlexRay register base
    
    \retval     CY_FR_OKAY        If stop of timer 1 was successful.
    \retval     CY_FR_ERROR       In case timer 1 not stopped.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_stop_timer1 ( volatile stc_FLEXRAY_t * base )
{
  cy_fr_fhal_write32((cy_fr_fhal_read32(CY_FR_REG_T1C)&0xFFFFFFFE),CY_FR_REG_T1C);

  if ( (cy_fr_fhal_read32(CY_FR_REG_T1C) & 0x1) == 0 ){
    return CY_FR_OKAY;
  } else {
    return CY_FR_ERROR;
  }
}


/*-------------------------------------------------------------------------------*/
/** \brief     This function reconfigure the Stop Watch Timer.

    \param[in]  base              Pointer to FlexRay register base
    \param[in] cy_fr_edge_select  New Value for the edge selection.
    \param[in] cy_fr_stpw_mode    New Value for Stop Watch mode.

    \retval    CY_FR_OKAY         If the new values successful set.
    \retval    CY_FR_ERROR        In case of error.
 */
/*-------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_set_stpw ( volatile stc_FLEXRAY_t * base,
                                        uint8_t                  cy_fr_edge_select,
                                        uint8_t                  cy_fr_stpw_mode )
{
  uint32_t temp_reg = 0;

  temp_reg += ( cy_fr_edge_select << 2 );
  temp_reg += ( cy_fr_stpw_mode   << 1 );
  cy_fr_fhal_write32 ( temp_reg, CY_FR_REG_STPW1 );
  if (cy_fr_fhal_read32 ( CY_FR_REG_STPW1 ) == temp_reg ){
    return CY_FR_OKAY;
  } else {
    return CY_FR_ERROR;
  }
}


/*-------------------------------------------------------------------------------*/
/** \brief     This function reads out the Stop Watch registers 1 and 2 and 
               transfer the values into a structure of type 'CY_FR_STOP_WATCH_STRUCT'.

    \param[in]  base                    Pointer to FlexRay register base
    \param[in] cy_fr_stop_watch_struct  Pointer to a structure for the caught values.

    \retval    CY_FR_OKAY               Always, no error handling.
 */
/*-------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_get_stpw ( volatile stc_FLEXRAY_t *  base,
                                        CY_FR_STOP_WATCH_STRUCT * cy_fr_stop_watch_struct)
{
  uint32_t temp_reg;

  temp_reg = cy_fr_fhal_read32(CY_FR_REG_STPW1);
  cy_fr_stop_watch_struct->MACROTICK     = ((temp_reg & 0x3FFF0000) >> 16);
  cy_fr_stop_watch_struct->CYCLE_COUNTER = ((temp_reg & 0x00003F00) >>  8);
  
  temp_reg = cy_fr_fhal_read32(CY_FR_REG_STPW2);
  cy_fr_stop_watch_struct->SLOT_COUNTER_CHANNEL_A = ((temp_reg & 0x03FF0000) >> 16);
  cy_fr_stop_watch_struct->SLOT_COUNTER_CHANNEL_B = ((temp_reg & 0x000003FF) >>  0);

  return CY_FR_OKAY;
}


/*-------------------------------------------------------------------------------*/
/** \brief     start capturing with the Stop Watch function In case of software trigger
               directly else wait for selected trigger signal.

    \param[in]  base                    Pointer to FlexRay register base
    \param[in]  cy_fr_trigger_source    Selecets the trigger type of the Stop Watch .

    \retval     CY_FR_OKAY              Always, no error handling.
 */
/*-------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_start_stpw ( volatile stc_FLEXRAY_t *      base,
                                          CY_FR_STOP_WATCH_TRIGGER_TYPE cy_fr_trigger_source )
{
  uint32_t temp_reg = 0;
  
  temp_reg = cy_fr_fhal_read32(CY_FR_REG_STPW1);
  temp_reg &=  0x06;
  cy_fr_fhal_write32(temp_reg,CY_FR_REG_STPW1); /* disable STPW and clear all trigger sources*/
  temp_reg |= (cy_fr_trigger_source << 3);
  cy_fr_fhal_write32(temp_reg,CY_FR_REG_STPW1);
  if ( cy_fr_trigger_source != CY_FR_STOP_WATCH_SOFTWARE_TRIGGER ){
    temp_reg |= 0x01;
    cy_fr_fhal_write32(temp_reg,CY_FR_REG_STPW1);
  }
  return CY_FR_OKAY;
}


#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


