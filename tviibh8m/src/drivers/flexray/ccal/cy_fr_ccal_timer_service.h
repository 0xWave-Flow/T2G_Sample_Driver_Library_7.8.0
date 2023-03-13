/***************************************************************************//**
* \file cy_fr_ccal_timer_service.h
* \version 1.0
*
* \brief
* Prototypes for cy_fr_ccal_timer_service.c.
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


#ifndef __CY_FR_CCAL_TIMER_SERVICE_H__
#define __CY_FR_CCAL_TIMER_SERVICE_H__


/**
* \addtogroup group_fru_functions
* \{
*/

/*---------------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_set_timer0   ( volatile stc_FLEXRAY_t *      base,
                                            uint32_t                      cy_fr_mt_offset,
                                            uint8_t                       cy_fr_cycle_code,
                                            uint8_t                       cy_fr_timer_mode);
                                            
CY_FR_RETURN_TYPE cy_fr_ccal_set_timer1   ( volatile stc_FLEXRAY_t *      base,
                                            uint16_t                      cy_fr_mt_count,
                                            uint8_t                       cy_fr_timer_mode);
                                            
CY_FR_RETURN_TYPE cy_fr_ccal_start_timer0 ( volatile stc_FLEXRAY_t *      base );
CY_FR_RETURN_TYPE cy_fr_ccal_start_timer1 ( volatile stc_FLEXRAY_t *      base );
CY_FR_RETURN_TYPE cy_fr_ccal_stop_timer0  ( volatile stc_FLEXRAY_t *      base );
CY_FR_RETURN_TYPE cy_fr_ccal_stop_timer1  ( volatile stc_FLEXRAY_t *      base );

CY_FR_RETURN_TYPE cy_fr_ccal_set_stpw     ( volatile stc_FLEXRAY_t *      base,
                                            uint8_t                       cy_fr_edge_select,
                                            uint8_t                       cy_fr_stpw_mode);
                                            
CY_FR_RETURN_TYPE cy_fr_ccal_start_stpw   ( volatile stc_FLEXRAY_t *      base,
                                            CY_FR_STOP_WATCH_TRIGGER_TYPE cy_fr_trigger_source);
                                            
CY_FR_RETURN_TYPE cy_fr_ccal_get_stpw     ( volatile stc_FLEXRAY_t *      base,
                                            CY_FR_STOP_WATCH_STRUCT *     cy_fr_stop_watch_struct);

/** \} group_fru_functions */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_CCAL_TIMER_SERVICE_H__ */
/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


