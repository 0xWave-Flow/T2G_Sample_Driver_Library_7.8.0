/***************************************************************************//**
* \file cy_fr_ccal_interrupt_service.h
* \version 1.0
*
* \brief
* Prototypes for cy_fr_ccal_interrupt_service.c.
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


#ifndef __CY_FR_CCAL_INTERRUPT_SERVICE_H__
#define __CY_FR_CCAL_INTERRUPT_SERVICE_H__


/**
* \addtogroup group_fru_functions
* \{
*/

/*---------------------------------------------------------------------------------
 * Prototypes
 *---------------------------------------------------------------------------------*/
CY_FR_INTERRUPT_TYPE cy_fr_ccal_interrupt_separator         ( uint32_t             *shadow_reg_eir,
                                                              uint32_t             *shadow_reg_sir );
                                                              
uint32_t             cy_fr_ccal_get_error_interrupt_flags   ( volatile stc_FLEXRAY_t * base,
                                                              CY_FR_INT_LINE_TYPE      cy_fr_int_line );
                                                              
uint32_t             cy_fr_ccal_get_status_interrupt_flags  ( volatile stc_FLEXRAY_t * base,
                                                              CY_FR_INT_LINE_TYPE      cy_fr_int_line );
                                                              
CY_FR_RETURN_TYPE    cy_fr_ccal_interrupt_line_enable       ( volatile stc_FLEXRAY_t * base,
                                                              CY_FR_INT_LINE_TYPE      cy_fr_int_line );
                                                              
CY_FR_RETURN_TYPE    cy_fr_ccal_interrupt_line_disable      ( volatile stc_FLEXRAY_t * base,
                                                              CY_FR_INT_LINE_TYPE      cy_fr_int_line );
                                                              
CY_FR_RETURN_TYPE    cy_fr_ccal_error_interrupt_enable      ( volatile stc_FLEXRAY_t * base,
                                                              CY_FR_INTERRUPT_TYPE     cy_fr_int );
                                                              
CY_FR_RETURN_TYPE    cy_fr_ccal_error_interrupt_disable     ( volatile stc_FLEXRAY_t * base,
                                                              CY_FR_INTERRUPT_TYPE     cy_fr_int );
                                                              
CY_FR_RETURN_TYPE    cy_fr_ccal_status_interrupt_enable     ( volatile stc_FLEXRAY_t * base,
                                                              CY_FR_INTERRUPT_TYPE     cy_fr_int );
                                                              
CY_FR_RETURN_TYPE    cy_fr_ccal_status_interrupt_disable    ( volatile stc_FLEXRAY_t * base,
                                                              CY_FR_INTERRUPT_TYPE     cy_fr_int );
                                                              
CY_FR_RETURN_TYPE    cy_fr_ccal_clear_error_interrupt_flag  ( volatile stc_FLEXRAY_t * base,
                                                              CY_FR_INTERRUPT_TYPE     cy_fr_int );
                                                              
CY_FR_RETURN_TYPE    cy_fr_ccal_clear_status_interrupt_flag ( volatile stc_FLEXRAY_t * base,
                                                              CY_FR_INTERRUPT_TYPE     cy_fr_int );
                                                              
CY_FR_RETURN_TYPE    cy_fr_ccal_error_interrupt_line_set    ( volatile stc_FLEXRAY_t * base,
                                                              CY_FR_INTERRUPT_TYPE     cy_fr_int,
                                                              CY_FR_INT_LINE_TYPE      cy_fr_int_line );
                                                              
CY_FR_RETURN_TYPE    cy_fr_ccal_status_interrupt_line_set   ( volatile stc_FLEXRAY_t * base,
                                                              CY_FR_INTERRUPT_TYPE     cy_fr_int,
                                                              CY_FR_INT_LINE_TYPE      cy_fr_int_line );

/** \} group_fru_functions */

/**
* \addtogroup group_fru_macros
* \{
*/

/*---------------------------------------------------------------------------------
 * Defines
 *
 * This defines needed as bit mask for the interrupt flag register
 *---------------------------------------------------------------------------------*/
  #define SHADOW_REG_EIR_PEMC   0x00000001
  #define SHADOW_REG_EIR_CNA    0x00000002
  #define SHADOW_REG_EIR_SFBM   0x00000004
  #define SHADOW_REG_EIR_SFO    0x00000008
  #define SHADOW_REG_EIR_CCF    0x00000010
  #define SHADOW_REG_EIR_CCL    0x00000020
  #define SHADOW_REG_EIR_PERR   0x00000040
  #define SHADOW_REG_EIR_RFO    0x00000080
  #define SHADOW_REG_EIR_EFA    0x00000100
  #define SHADOW_REG_EIR_IIBA   0x00000200
  #define SHADOW_REG_EIR_IOBA   0x00000400
  #define SHADOW_REG_EIR_MHF    0x00000800
  #define SHADOW_REG_EIR_EDA    0x00010000
  #define SHADOW_REG_EIR_LTVA   0x00020000
  #define SHADOW_REG_EIR_TABA   0x00040000
  #define SHADOW_REG_EIR_EDB    0x01000000
  #define SHADOW_REG_EIR_LTVB   0x02000000
  #define SHADOW_REG_EIR_TABB   0x04000000


  #define SHADOW_REG_SIR_WST    0x00000001
  #define SHADOW_REG_SIR_CAS    0x00000002
  #define SHADOW_REG_SIR_CYCS   0x00000004
  #define SHADOW_REG_SIR_TXI    0x00000008
  #define SHADOW_REG_SIR_RXI    0x00000010
  #define SHADOW_REG_SIR_RFNE   0x00000020
  #define SHADOW_REG_SIR_RFCL   0x00000040
  #define SHADOW_REG_SIR_NMVC   0x00000080
  #define SHADOW_REG_SIR_TI0    0x00000100
  #define SHADOW_REG_SIR_TI1    0x00000200
  #define SHADOW_REG_SIR_TIBC   0x00000400
  #define SHADOW_REG_SIR_TOBC   0x00000800
  #define SHADOW_REG_SIR_SWE    0x00001000
  #define SHADOW_REG_SIR_SUCS   0x00002000
  #define SHADOW_REG_SIR_MBSI   0x00004000
  #define SHADOW_REG_SIR_SDS    0x00008000
  #define SHADOW_REG_SIR_WUPA   0x00010000
  #define SHADOW_REG_SIR_MTSA   0x00020000
  #define SHADOW_REG_SIR_WUPB   0x01000000
  #define SHADOW_REG_SIR_MTSB   0x02000000

/** \} group_fru_macros */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_CCAL_INTERRUPT_SERVICE_H__ */
/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


