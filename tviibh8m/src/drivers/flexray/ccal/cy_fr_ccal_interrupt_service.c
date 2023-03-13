/***************************************************************************//**
* \file cy_fr_ccal_interrupt_service.h
* \version 1.0
*
* \brief
* Routines for the Interrupt Service.
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
#include "flexray/ccal/cy_fr_ccal_interrupt_service.h"
#include "flexray/fhal/cy_fr_fhal.h"


/*----------------------------------------------------------------------------------*/
/** \brief      This function reads out the all enabled error interrupt flags from the
                error interrupt register ( EIR )assigned to the Interrupt line 
                'cy_fr_int_line'.
            
    \param[in]  base            Pointer to FlexRay register base
    \param[in]  cy_fr_int_line  The interrupt line.

    \retval     uint32_t        The Register value of EIR with all enabled and to 
                                'cy_fr_int_line' assigned flags.
 */
/*----------------------------------------------------------------------------------*/
uint32_t cy_fr_ccal_get_error_interrupt_flags ( volatile stc_FLEXRAY_t * base,
                                                CY_FR_INT_LINE_TYPE      cy_fr_int_line )
{
  uint32_t cy_fr_return_value;
  
  cy_fr_return_value  = cy_fr_fhal_read32(CY_FR_REG_EIR);         /* readout error interrupt flags */
  if ( cy_fr_int_line == CY_FR_INT_LINE_0 ) {
    cy_fr_return_value &= ~(cy_fr_fhal_read32(CY_FR_REG_EILS));   /* delete not enabled for line 0 */
  }  else if ( cy_fr_int_line == CY_FR_INT_LINE_1 ) { 
    cy_fr_return_value &= (cy_fr_fhal_read32(CY_FR_REG_EILS));    /* delete not enabled for line 1 */
  }
  cy_fr_return_value &= cy_fr_fhal_read32(CY_FR_REG_EIES);        /* delete not enabled flags */
  cy_fr_fhal_write32(cy_fr_return_value,CY_FR_REG_EIR);           /* clear flags */
  
  return cy_fr_return_value;
}


/*----------------------------------------------------------------------------------*/
/** \brief      This function reads out the all enabled status interrupt flags from
                the status interrupt register ( SIR ) assigned to the Interrupt line 
                'cy_fr_int_line'.
            
    \param[in]  base            Pointer to FlexRay register base
    \param[in]  cy_fr_int_line  The interrupt line.

    \retval     uint32_t        The Register value of SIR with all enabled and to 
                                'cy_fr_int_line' assigned flags.
 */
/*----------------------------------------------------------------------------------*/
uint32_t cy_fr_ccal_get_status_interrupt_flags( volatile stc_FLEXRAY_t * base,
                                                CY_FR_INT_LINE_TYPE      cy_fr_int_line )
{
  uint32_t cy_fr_return_value;
  
  cy_fr_return_value  = cy_fr_fhal_read32(CY_FR_REG_SIR);
  if ( cy_fr_int_line == CY_FR_INT_LINE_0 ) {
    cy_fr_return_value &= ~(cy_fr_fhal_read32(CY_FR_REG_SILS));
  }  else if ( cy_fr_int_line == CY_FR_INT_LINE_1 ) {
    cy_fr_return_value &= (cy_fr_fhal_read32(CY_FR_REG_SILS));
  }
  cy_fr_return_value &= cy_fr_fhal_read32(CY_FR_REG_SIES);
  cy_fr_fhal_write32(cy_fr_return_value,CY_FR_REG_SIR);
  
  return cy_fr_return_value;
}
/*----------------------------------------------------------------------------------*/
/** \brief      This function checks all interrupt flags in the shadow registers 
                pointed by '*shadow_reg_sir' and '*shadow_reg_eir' and clears the not 
                enbaled flags. For speed optimization comment the not needed 'if' 
                statments out.

    \param[in]  *shadow_reg_eir       Pointer to the shadow register of EIR.
    \param[in]  *shadow_reg_sir       Pointer to the shadow register of SIR.

    \retval     CY_FR_INTERRUPT_TYPE  An interrupt value defined in cy_fr_api_global.h.
 */
/*----------------------------------------------------------------------------------*/
CY_FR_INTERRUPT_TYPE cy_fr_ccal_interrupt_separator ( uint32_t *shadow_reg_eir,
                                                      uint32_t *shadow_reg_sir)
{
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_PEMC ) != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_PEMC;
    return CY_FR_EIR_POC_ERROR_MODE_CHANGED;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_CNA ) != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_CNA;
    return CY_FR_EIR_COMMAND_NOT_ACCEPTED;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_SFBM ) != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_SFBM;
    return CY_FR_EIR_SYNC_FRAME_BELOW_MINIMUM;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_SFO )  != 0 ) {
    *shadow_reg_eir &= ~SHADOW_REG_EIR_SFO;
    return CY_FR_EIR_SYNCFRAME_OVERFLOW;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_CCF )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_CCF;
    return CY_FR_EIR_CLOCK_CORRECTION_FAILURE;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_CCL )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_CCL;
    return CY_FR_EIR_CHI_COMMAND_LOCKED;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_PERR )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_PERR;
    return CY_FR_EIR_PARITY_ERROR;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_RFO )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_RFO;
    return CY_FR_EIR_RECEIVED_FIFO_OVERRUN;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_EFA )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_EFA;
    return CY_FR_EIR_EMPTY_FIFO_ACCESS;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_IIBA )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_IIBA;
    return CY_FR_EIR_ILLEGAL_INPUT_BUFFER_ACCESS;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_IOBA )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_IOBA;
    return CY_FR_EIR_ILLEGAL_OUTPUT_BUFFER_ACCESS;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_MHF )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_MHF;
    return CY_FR_EIR_MESSAGE_HANDLER_CONSTRAINTS_FLAG;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_EDA )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_EDA;
    return CY_FR_EIR_ERROR_DETECTED_CHANNEL_A;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_LTVA )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_LTVA;
    return CY_FR_EIR_LATEST_TRANSMIT_VIOLATION_DETECTED_CHANNEL_A;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_TABA )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_TABA;
    return CY_FR_EIR_TRANSMISSION_ACROSS_BOUNDARY_CHANNEL_A;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_EDB )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_EDB;
    return CY_FR_EIR_ERROR_DETECTED_CHANNEL_B;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_LTVB )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_LTVB;
    return CY_FR_EIR_LATEST_TRANSMIT_VIOLATION_DETECTED_CHANNEL_B;
  }
  if ( ( *shadow_reg_eir & SHADOW_REG_EIR_TABB )  != 0 ){
    *shadow_reg_eir &= ~SHADOW_REG_EIR_TABB;
    return CY_FR_EIR_TRANSMISSION_ACROSS_BOUNDARY_CHANNEL_B;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_WST )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_WST;
    return CY_FR_SIR_WAKEUP_STATUS;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_CAS )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_CAS;
    return CY_FR_SIR_COLLISION_AVOIDANCE_SYMBOL;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_CYCS )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_CYCS;
    return CY_FR_SIR_CYCLE_STARTUP_INTERRUPT;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_TXI )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_TXI;
    return CY_FR_SIR_TRANSMIT_INTERRUPT;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_RXI )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_RXI;
    return CY_FR_SIR_RECEIVE_INTERRUPT;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_RFNE )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_RFNE;
    return CY_FR_SIR_RECEIVE_FIFO_NOT_EMPTY;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_RFCL )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_RFCL;
    return CY_FR_SIR_RECEIVE_FIFO_CRITICAL_LEVEL;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_NMVC )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_NMVC;
    return CY_FR_SIR_NETWORK_MANAGEMENT_VECTOR_CHANGED;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_TI0 )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_TI0;
    return CY_FR_SIR_TIMER_INTERRUPT_0;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_TI1 )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_TI1;
    return CY_FR_SIR_TIMER_INTERRUPT_1;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_TIBC )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_TIBC;
    return CY_FR_SIR_TRANSFER_INPUT_BUFFER_COMPLETED;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_TOBC )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_TOBC;
    return CY_FR_SIR_TRANSFER_OUTPUT_BUFFER_COMPLETED;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_SWE )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_SWE;
    return CY_FR_SIR_STOP_WATCH_EVENT;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_SUCS )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_SUCS;
    return CY_FR_SIR_STARTUP_COMPLETED;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_MBSI )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_MBSI;
    return CY_FR_SIR_MESSAGE_BUFFER_STATUS_INTERRUPT;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_SDS )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_SDS;
    return CY_FR_SIR_START_OF_DYNAMIC_SEGMENT;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_WUPA )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_WUPA;
    return CY_FR_SIR_WAKEUP_PATTERN_CHANNEL_A;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_MTSA )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_MTSA;
    return CY_FR_SIR_MTS_RECEIVED_ON_CHANNEL_A;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_WUPB )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_WUPB;
    return CY_FR_SIR_WAKEUP_PATTERN_CHANNEL_B;
  }
  if ( ( *shadow_reg_sir & SHADOW_REG_SIR_MTSB )  != 0){
    *shadow_reg_sir &= ~SHADOW_REG_SIR_MTSB;
    return CY_FR_SIR_MTS_RECEIVED_ON_CHANNEL_B;
  }
  return (CY_FR_INTERRUPT_TYPE)0xFF;
}


/*----------------------------------------------------------------------------------*/
/** \brief      This function enables the error interrupt 'cy_fr_int'.
            
    \param[in]  base         Pointer to FlexRay register base
    \param[in]  cy_fr_int    An interrupt (A list of all allowed values define in          
                             cy_fr_api_global.h at section 'CY_FR_INT_TYPE' ).

    \retval     CY_FR_OKAY   If the interrupt is successfully enabled.
    \retval     CY_FR_ERROR  In case the interrupt is not enabled.
 */
/*----------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_error_interrupt_enable ( volatile stc_FLEXRAY_t * base,
                                                      CY_FR_INTERRUPT_TYPE     cy_fr_int )
{
  uint32_t cy_fr_temp_reg = 0;
  
  cy_fr_temp_reg = (0x00000001 << cy_fr_int);
  cy_fr_fhal_write32(cy_fr_temp_reg,CY_FR_REG_EIES);
  if ( ( cy_fr_fhal_read32 ( CY_FR_REG_EIES ) & cy_fr_temp_reg ) != 0 ){
    return CY_FR_OKAY;
  } else {
    return CY_FR_ERROR;
  }
}


/*----------------------------------------------------------------------------------*/
/** \brief      This function disables the error interrupt 'cy_fr_int'.
            
    \param[in]  base         Pointer to FlexRay register base
    \param[in]  cy_fr_int    An interrupt (A list of all allowed values define in          
                             cy_fr_api_global.h at section 'CY_FR_INT_TYPE' )

    \retval     CY_FR_OKAY   If the interrupt is successfully disabled.
    \retval     CY_FR_ERROR  In case the interrupt is not disabled.
 */
/*----------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_error_interrupt_disable ( volatile stc_FLEXRAY_t * base,
                                                       CY_FR_INTERRUPT_TYPE     cy_fr_int )
{
  uint32_t cy_fr_temp_reg = 0;
  
  cy_fr_temp_reg = (0x00000001 << cy_fr_int);
  cy_fr_fhal_write32 ( cy_fr_temp_reg, CY_FR_REG_EIER );
  if ( ( cy_fr_fhal_read32 ( CY_FR_REG_EIER ) & cy_fr_temp_reg ) == 0 ){
    return CY_FR_OKAY;
  } else {
    return CY_FR_ERROR;
  }
}


/*----------------------------------------------------------------------------------*/
/** \brief      This function enables the status interrupt 'cy_fr_int'.
            
    \param[in]  base         Pointer to FlexRay register base
    \param[in]  cy_fr_int    An interrupt (A list of all allowed values define in          
                             cy_fr_api_global.h at section 'CY_FR_INT_TYPE' )

    \retval     CY_FR_OKAY   If the status interrupt successfully enabled.
    \retval     CY_FR_ERROR  In case the status interrupt is not enabled.
 */
/*----------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_status_interrupt_enable ( volatile stc_FLEXRAY_t * base,
                                                       CY_FR_INTERRUPT_TYPE     cy_fr_int )
{
  uint8_t cy_fr_bit_shift;
  uint32_t cy_fr_temp_reg;
  CY_FR_RETURN_TYPE  cy_fr_temp_reg2;

  cy_fr_bit_shift = (uint8_t)cy_fr_int - 0x40;
  cy_fr_temp_reg = (0x00000001 << cy_fr_bit_shift);
  cy_fr_fhal_write32 ( cy_fr_temp_reg, CY_FR_REG_SIES );
  if ( ( cy_fr_fhal_read32 ( CY_FR_REG_SIES ) & cy_fr_temp_reg ) != 0 ){
    cy_fr_temp_reg2 = CY_FR_OKAY;
  } else {
    cy_fr_temp_reg2 = CY_FR_ERROR;
  }

  return cy_fr_temp_reg2;
}


/*----------------------------------------------------------------------------------*/
/** \brief      This function disables the status interrupt 'cy_fr_int'
            
    \param[in]  base         Pointer to FlexRay register base
    \param[in]  cy_fr_int    An interrupt (A list of all allowed values define in          
                             cy_fr_api_global.h at section CY_FR_INT_TYPE ).

    \retval     CY_FR_OKAY   If the status interrupt successfully disabled.
    \retval     CY_FR_ERROR  In case the status interrupt is not disabled.
 */
/*----------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_status_interrupt_disable ( volatile stc_FLEXRAY_t * base,
                                                        CY_FR_INTERRUPT_TYPE     cy_fr_int )
{
  uint8_t cy_fr_bit_shift;
  uint32_t cy_fr_temp_reg;
  CY_FR_RETURN_TYPE cy_fr_temp_reg2;

  cy_fr_bit_shift = (uint8_t)cy_fr_int - 0x40;
  cy_fr_temp_reg = (0x00000001 << cy_fr_bit_shift);
  cy_fr_fhal_write32 ( cy_fr_temp_reg, CY_FR_REG_SIER );
  if ( ( cy_fr_fhal_read32 ( CY_FR_REG_SIER ) & cy_fr_temp_reg ) == 0 ){
    cy_fr_temp_reg2 = CY_FR_OKAY;
  } else {
    cy_fr_temp_reg2 = CY_FR_ERROR;
  }

  return cy_fr_temp_reg2;
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function clears the error interrupt flag 'cy_fr_int'.
            
    \param[in]  base         Pointer to FlexRay register base
    \param[in]  cy_fr_int    An interrupt (A list of all allowed values define in          
                             cy_fr_api_global.h at section 'CY_FR_INT_TYPE' )

    \retval     CY_FR_OKAY   If the error interrupt flag successfully cleared.
    \retval     CY_FR_ERROR  In case the error interrupt flag is not cleared.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_clear_error_interrupt_flag ( volatile stc_FLEXRAY_t * base,
                                                          CY_FR_INTERRUPT_TYPE     cy_fr_int )
{
  cy_fr_fhal_write32 ( ( 0x00000001 << cy_fr_int ), CY_FR_REG_EIR );
  if ( ( cy_fr_fhal_read32(CY_FR_REG_EIR) & ( 0x00000001 << cy_fr_int ) ) == 0 )
  {
    return CY_FR_OKAY ;
  } else {
    return CY_FR_ERROR;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function clears the status interrupt flag 'cy_fr_int'.
            
    \param[in]  base         Pointer to FlexRay register base
    \param[in]  cy_fr_int    An interrupt ( A list of all allowed values define in          
                             cy_fr_api_global.h at section 'CY_FR_INT_TYPE' ).

    \retval     CY_FR_OKAY   If the status interrupt flag successfully cleared.
    \retval     CY_FR_ERROR  In case the status interrupt flag is not cleared.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_clear_status_interrupt_flag ( volatile stc_FLEXRAY_t * base,
                                                           CY_FR_INTERRUPT_TYPE     cy_fr_int )
{
  uint8_t cy_fr_bit_shift;
  CY_FR_RETURN_TYPE cy_fr_temp_reg;

  cy_fr_bit_shift = (uint8_t)cy_fr_int - 0x40;
  cy_fr_fhal_write32 ( ( 0x00000001 << cy_fr_bit_shift ), CY_FR_REG_SIR );
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SIR) & ( 0x00000001 << cy_fr_bit_shift ) ) == 0 )
  {
    cy_fr_temp_reg = CY_FR_OKAY ;
  } else {
    cy_fr_temp_reg = CY_FR_ERROR;
  }

  return cy_fr_temp_reg;
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function set the error interrupt line for the interrupt 
                'cy_fr_int' to the interrupt line 'cy_fr_int_line'.
            
    \param[in]  base                    Pointer to FlexRay register base
    \param[in]  cy_fr_int               An interrupt (A list of all allowed values          
                                        define in cy_fr_api_global.h at section 
                                        'CY_FR_INT_TYPE' ).
    \param[in]  cy_fr_int_line          The interrupt line (A list of all allowed          
                                        values define in cy_fr_api_global.h at 
                                        section 'CY_FR_INT_LINE_TYPE' ).
                                       
    \retval     CY_FR_OKAY              If the interrupt line successfully set.
    \retval     CY_FR_ERROR             In case the interrupt line is not set.
    \retval     CY_FR_INVALID_INT_LINE  If the interrupt line 'cy_fr_int_line' is not
                                        allowed.
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_error_interrupt_line_set ( volatile stc_FLEXRAY_t * base,
                                                        CY_FR_INTERRUPT_TYPE     cy_fr_int,
                                                        CY_FR_INT_LINE_TYPE      cy_fr_int_line )
{
  uint32_t cy_fr_temp, cy_fr_temp_reg;
  
  cy_fr_temp = ( 1 << cy_fr_int );
  cy_fr_temp_reg  = cy_fr_fhal_read32(CY_FR_REG_EILS);
  if ( cy_fr_int_line == CY_FR_INT_LINE_1){
    cy_fr_fhal_write32((cy_fr_temp|cy_fr_temp_reg),CY_FR_REG_EILS); 
  } else if ( cy_fr_int_line == CY_FR_INT_LINE_0){
    cy_fr_fhal_write32(((~cy_fr_temp)&cy_fr_temp_reg),CY_FR_REG_EILS);    
  } else {
    return CY_FR_INVALID_INT_LINE;
  }
  if ( ( ( cy_fr_fhal_read32(CY_FR_REG_EILS) & cy_fr_temp ) >> cy_fr_int) == cy_fr_int_line ){
    return CY_FR_OKAY;
  } else {
    return CY_FR_ERROR;
  }
}
                                                            

/*---------------------------------------------------------------------------------*/
/** \brief      This function set the status interrupt line for the interrupt 
                'cy_fr_int' to the interrupt line 'cy_fr_int_line'.
            
    \param[in]  base                    Pointer to FlexRay register base
    \param[in]  cy_fr_int               An interrupt (A list of all allowed values          
                                        define in cy_fr_api_global.h at section 
                                        'CY_FR_INT_TYPE' ).
    \param[in]  cy_fr_int_line          The interrupt line (A list of all allowed          
                                        values define in  cy_fr_api_global.h at 
                                        section 'CY_FR_INT_LINE_TYPE' ).
    
    \retval     CY_FR_OKAY              If the interrupt line successfully set.
    \retval     CY_FR_ERROR             In case the interrupt line is not set.
    \retval     CY_FR_INVALID_INT_LINE  If the interrupt line 'cy_fr_int_line' is not
                                        allowed.
*/
/*---------------------------------------------------------------------------------*/                                                            
CY_FR_RETURN_TYPE cy_fr_ccal_status_interrupt_line_set ( volatile stc_FLEXRAY_t * base,
                                                         CY_FR_INTERRUPT_TYPE     cy_fr_int,
                                                         CY_FR_INT_LINE_TYPE      cy_fr_int_line )
{
  uint32_t cy_fr_temp, cy_fr_reg;
  
  cy_fr_temp = ( 1 << ( cy_fr_int - 0x40 ) );
  cy_fr_reg  = cy_fr_fhal_read32(CY_FR_REG_SILS);
  if ( cy_fr_int_line == CY_FR_INT_LINE_1){
    cy_fr_fhal_write32((cy_fr_temp|cy_fr_reg),CY_FR_REG_SILS); 
  } else if ( cy_fr_int_line == CY_FR_INT_LINE_0){
    cy_fr_fhal_write32(((~cy_fr_temp)&cy_fr_reg),CY_FR_REG_SILS); 
  } else {
    return CY_FR_INVALID_INT_LINE;
  }
  if ( ( ( cy_fr_fhal_read32(CY_FR_REG_SILS) & cy_fr_temp ) >> ( cy_fr_int - 0x40) ) == cy_fr_int_line ){
    return CY_FR_OKAY;
  } else {
    return CY_FR_ERROR;
  }  
}



/*---------------------------------------------------------------------------------*/
/** \brief      This function enables interrupt line 'cy_fr_int_line'.
            
    \param[in]  base                    Pointer to FlexRay register base
    \param[in]  cy_fr_int_line          The interrupt line (A list of all allowed          
                                        values define in  cy_fr_api_global.h at 
                                        section 'CY_FR_INT_LINE_TYPE' ).
    
    \retval     CY_FR_OKAY              If the interrupt line successfully enabled.
    \retval     CY_FR_ERROR             In case the interrupt line is not enabled.
    \retval     CY_FR_INVALID_INT_LINE  If the interrupt line 'cy_fr_int_line' is not
                                        allowed.
*/
/*---------------------------------------------------------------------------------*/ 
CY_FR_RETURN_TYPE cy_fr_ccal_interrupt_line_enable ( volatile stc_FLEXRAY_t * base,
                                                     CY_FR_INT_LINE_TYPE      cy_fr_int_line )
{
  uint32_t cy_fr_temp_reg;
  
  cy_fr_temp_reg = cy_fr_fhal_read32 ( CY_FR_REG_ILE );
  if ( cy_fr_int_line == CY_FR_INT_LINE_0 ){
    cy_fr_fhal_write32( ( cy_fr_temp_reg | 1 ), CY_FR_REG_ILE );
    if ( ( cy_fr_fhal_read32(CY_FR_REG_ILE) & 0x1 ) == 0x1 ){
      return CY_FR_OKAY;
    } else {
      return CY_FR_ERROR;
    }
  } else if ( cy_fr_int_line == CY_FR_INT_LINE_1 ){
    cy_fr_fhal_write32( ( cy_fr_temp_reg | 2 ), CY_FR_REG_ILE );
    if ( ( cy_fr_fhal_read32(CY_FR_REG_ILE) & 0x2 ) == 0x2 ){
      return CY_FR_OKAY;
    } else {
      return CY_FR_ERROR;
    }
  } else {
    return CY_FR_INVALID_INT_LINE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function disables interrupt line 'cy_fr_int_line'.
            
    \param[in]  base                    Pointer to FlexRay register base
    \param[in]  cy_fr_int_line          The interrupt line (A list of all allowed          
                                        values define in  cy_fr_api_global.h at 
                                        section 'CY_FR_INT_LINE_TYPE' ).
    
    \retval     CY_FR_OKAY              If the interrupt line successfully disabled.
    \retval     CY_FR_ERROR             In case the interrupt line is not disabled.
    \retval     CY_FR_INVALID_INT_LINE  If the interrupt line 'cy_fr_int_line' is not
                                        allowed.
*/
/*---------------------------------------------------------------------------------*/ 
CY_FR_RETURN_TYPE cy_fr_ccal_interrupt_line_disable ( volatile stc_FLEXRAY_t * base,
                                                      CY_FR_INT_LINE_TYPE      cy_fr_int_line )
{
  uint32_t cy_fr_temp_reg;
  
  cy_fr_temp_reg = cy_fr_fhal_read32 ( CY_FR_REG_ILE );
  if ( cy_fr_int_line == CY_FR_INT_LINE_0 ){
    cy_fr_fhal_write32( ( cy_fr_temp_reg & 2 ), CY_FR_REG_ILE );
    if ( ( cy_fr_fhal_read32(CY_FR_REG_ILE) & 0x1 ) == 0 ){
      return CY_FR_OKAY;
    } else {
      return CY_FR_ERROR;
    }
  } else if ( cy_fr_int_line == CY_FR_INT_LINE_1 ){
    cy_fr_fhal_write32( ( cy_fr_temp_reg & 1 ), CY_FR_REG_ILE );
    if ( ( cy_fr_fhal_read32(CY_FR_REG_ILE) & 0x2 ) == 0 ){
      return CY_FR_OKAY;
    } else {
      return CY_FR_ERROR;
    }
  } else {
    return CY_FR_INVALID_INT_LINE;
  }
}


#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


