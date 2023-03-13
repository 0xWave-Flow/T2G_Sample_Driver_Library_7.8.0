/***************************************************************************//**
* \file cy_fr_api_status_service.c
* \version 1.0
*
* \brief
* User Interface for Status Service
*
* This File contains the user interface for status service. With the status service
* you can read the FlexRay Communication Controller status flags and registers.
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


/*---------------------------------------------------------------------------------*/
/** \brief  This function reads out the Wakeup Status Bits ( WSV ) in the CC Status
            Vector ( CCSV ).

    \param[in]  base                 Pointer to FlexRay register base
    
    \retval CY_FR_WAKEUP_STATUS_TYPE The status of the FlexRay Communication Controller.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_WAKEUP_STATUS_TYPE cy_fr_api_get_wakeup_status( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_wakeup_status(base);   /* read the Wakeup status bits */
}


/*---------------------------------------------------------------------------------*/
/** \brief  This Function checks the POC status bits, whether Protocol Operation
            Control is in a Wakeup state or not. The Wakeup states defined as
            follow: POCS[5:0] 01 xxxx.

    \param[in]  base     Pointer to FlexRay register base
    
    \retval CY_FR_TRUE   If FlexRay CC is in a Wakeup state.
    \retval CY_FR_FALSE  In case the FlexRay CC is not in any Wakeup state.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_pocs_is_wakeup ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_ccal_get_poc_status(base) & 0x30 ) == 0x10 ){ /* POCS[5:0] = 01 xxxx */
    return CY_FR_TRUE;                                       /* POC is in wakeup path */
  } else {
    return CY_FR_FALSE;                                      /* POC is not in wakeup path */
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief  This Function checks the POC status bits, whether Protocol Operation
            Control is in a Startup state or not. The Startup states defined as
            follow: POCS[5:0] 10 xxxx.

    \param[in]  base     Pointer to FlexRay register base
    
    \retval CY_FR_TRUE   If FlexRay CC is in a Startup state.
    \retval CY_FR_FALSE  In case the FlexRay CC is not in any Startup state.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_pocs_is_startup ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_ccal_get_poc_status(base) & 0x30 ) == 0x20 ){ /* POCS[5:0] = 10 xxxx */
    return CY_FR_TRUE;                                       /* POC is in startup path */
  } else {
    return CY_FR_FALSE;                                      /* POC is not in startup path */
  }
}

/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the interrupt flag for the interrupt
                'cy_fr_int' from the Error Interrupt Register ( EIR ) and Status
                Interrupt Register ( SIR ).

    \param[in]  base         Pointer to FlexRay register base
    \param[in]  cy_fr_int    The Interrupt which flag should be checked ( a list of
                             all allowed values define in cy_fr_api_global.h at section
                             'CY_FR_INTERRUPT_TYPE' ).

    \retval     CY_FR_TRUE   If the requested interrupt flag is set.
    \retval     CY_FR_FALSE  In case the requested interrupt flag is not set.
 */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_api_get_interrupt_status ( volatile stc_FLEXRAY_t * base,
                                         CY_FR_INTERRUPT_TYPE     cy_fr_int){
  if ( cy_fr_int <= 0x1A ){                                          /* check if error interrupt */
    return cy_fr_ccal_get_error_interrupt_status(base, cy_fr_int);   /* get error interrupt flag */
  } else if ( cy_fr_int <= 0x59 ){                                   /* check if status interrupt */
    return cy_fr_ccal_get_status_interrupt_status(base, cy_fr_int);  /* get status interrupt flag */
  } else {
    return CY_FR_INVALID_INTERRUPT;                                  /* not allowed interrupt */
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function checks if the FlexRay CC is in NORMAL_ACTIVE state or not.
            The NORMAL_ACTIVE state defined as follow: POCS[5:0] 00 0010.


    \param[in]  base     Pointer to FlexRay register base
    
    \retval CY_FR_TRUE   If FlexRay CC is in NORMAL_ACTIVE state.
    \retval CY_FR_FALSE  In case the FlexRay CC is not in NORMAL_ACTIVE state.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_pocs_is_normal_active ( volatile stc_FLEXRAY_t * base )
{
  if ( cy_fr_ccal_get_poc_status(base) == CY_FR_POCS_NORMAL_ACTIVE){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function checks if the FlexRay CC is in NORMAL_PASSIVE state or not.
            The NORMAL_ACTIVE state defined as follow: POCS[5:0] 00 0011.

    \param[in]  base     Pointer to FlexRay register base
    
    \retval CY_FR_TRUE   If FlexRay CC is in NORMAL_PASSIVE state.
    \retval CY_FR_FALSE  In case the FlexRay CC is not in NORMAL_PASSIVE state.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_pocs_is_normal_passive ( volatile stc_FLEXRAY_t * base )
{
  if ( cy_fr_ccal_get_poc_status(base) == CY_FR_POCS_NORMAL_PASSIVE){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function checks if the FlexRay CC is in CONFIG state or not.
            The NORMAL_ACTIVE state defined as follow: POCS[5:0] 00 1111.

    \param[in]  base     Pointer to FlexRay register base
    
    \retval CY_FR_TRUE   If FlexRay CC is in CONFIG state.
    \retval CY_FR_FALSE  In case the FlexRay CC is not in CONFIG state.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_pocs_is_config ( volatile stc_FLEXRAY_t * base )
{
  if ( cy_fr_ccal_get_poc_status(base) == CY_FR_POCS_CONFIG ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function checks if the FlexRay CC is in DEFAULT_CONFIG state or not.
            The NORMAL_ACTIVE state defined as follow: POCS[5:0] 00 0000.

    \param[in]  base     Pointer to FlexRay register base
    
    \retval CY_FR_TRUE   If FlexRay CC is in DEFAULT_CONFIG state.
    \retval CY_FR_FALSE  In case the FlexRay CC is not in DEFAULT_CONFIG state.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_pocs_is_default_config ( volatile stc_FLEXRAY_t * base )
{
  if ( cy_fr_ccal_get_poc_status(base) == CY_FR_POCS_DEFAULT_CONFIG ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function checks if the FlexRay CC is in HALT state or not.
            The NORMAL_ACTIVE state defined as follow: POCS[5:0] 00 0100.

    \param[in]  base     Pointer to FlexRay register base
    
    \retval CY_FR_TRUE   If FlexRay CC is in HALT state.
    \retval CY_FR_FALSE  In case the FlexRay CC is not in HALT state.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_pocs_is_halt ( volatile stc_FLEXRAY_t * base )
{
  if ( cy_fr_ccal_get_poc_status(base) == CY_FR_POCS_HALT ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function checks the cold start noisy flag, in case the FlexRay CC is
            in one of the following states, STARTUP, NORMAL_ACTIVE,
            NORMAL_PASSIVE or HALT. Only in this states the flag has a usable value.

    \param[in]  base                    Pointer to FlexRay register base
    
    \retval CY_FR_TRUE                  If the cold start noisy flag is set.
    \retval CY_FR_FALSE                 In case the cold start noisy flag is not set.
    \retval CY_FR_COMMAND_NOT_ACCEPTED  If the FlexRay CC is in wrong state.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_coldstart_noisy ( volatile stc_FLEXRAY_t * base )
{
  if ( (cy_fr_api_pocs_is_startup(base)        == CY_FR_TRUE) || 
       (cy_fr_api_pocs_is_normal_active(base)  == CY_FR_TRUE) ||
       (cy_fr_api_pocs_is_normal_passive(base) == CY_FR_TRUE) ||
       (cy_fr_api_pocs_is_halt(base)           == CY_FR_TRUE)    ){
    return cy_fr_ccal_is_coldstart_noisy(base);
  } else {
    return CY_FR_COMMAND_NOT_ACCEPTED;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function checks the cold start abort flag, in case the FlexRay CC is
            in one of the following states, STARTUP, NORMAL_ACTIVE,
            NORMAL_PASSIVE or HALT. Only in this states the flag has a usable value.

    \param[in]  base                    Pointer to FlexRay register base
    
    \retval CY_FR_TRUE                  If the cold start abort flag is set.
    \retval CY_FR_FALSE                 In case the cold start abort flag is not set.
    \retval CY_FR_COMMAND_NOT_ACCEPTED  If the FlexRay CC is in wrong state.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_coldstart_abort ( volatile stc_FLEXRAY_t * base )
{
  if ( (cy_fr_api_pocs_is_startup(base)        == CY_FR_TRUE) || 
       (cy_fr_api_pocs_is_normal_active(base)  == CY_FR_TRUE) ||
       (cy_fr_api_pocs_is_normal_passive(base) == CY_FR_TRUE) ||
       (cy_fr_api_pocs_is_halt(base)           == CY_FR_TRUE)    ){
    return cy_fr_ccal_is_coldstart_abort(base);
  } else {
    return CY_FR_COMMAND_NOT_ACCEPTED;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function checks the cold start inhibit flag, in case the FlexRay CC
            is in one of the following states, STARTUP, NORMAL_ACTIVE,
            NORMAL_PASSIVE or HALT. Only in this states the flag has a usable value.

    \param[in]  base                    Pointer to FlexRay register base
    
    \retval CY_FR_TRUE                  If the cold start inhibit flag is set.
    \retval CY_FR_FALSE                 In case the cold start inhibit flag is not set.
    \retval CY_FR_COMMAND_NOT_ACCEPTED  If the FlexRay CC is in wrong state.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_coldstart_inhibit ( volatile stc_FLEXRAY_t * base )
{
  if ( (cy_fr_api_pocs_is_startup(base)        == CY_FR_TRUE) || 
       (cy_fr_api_pocs_is_normal_active(base)  == CY_FR_TRUE) ||
       (cy_fr_api_pocs_is_normal_passive(base) == CY_FR_TRUE) ||
       (cy_fr_api_pocs_is_halt(base)           == CY_FR_TRUE)    ){
    return cy_fr_ccal_is_coldstart_inhibit(base);
  } else {
    return CY_FR_COMMAND_NOT_ACCEPTED;
  }
}



/*---------------------------------------------------------------------------------*/
/** \brief      This function check the requested channel and call
                cy_fr_ccal_get_aggregated_channel_a_status ( ) for 'cy_fr_channel' equal
                'CY_FR_CHANNEL_A' or cy_fr_ccal_get_aggregated_channel_b_status ( ) for
                'cy_fr_channel' equal 'CY_FR_CHANNEL_B'.

    \param[in]  base                   Pointer to FlexRay register base
    \param[in]  cy_fr_channel          The requested channel.
    \param[out] *data_struct           A structure with all flags.

    \retval     CY_FR_OKAY             In case of no error.
    \retval     CY_FR_INVALID_CHANNEL  If a channel is requested else then
                                       'CY_FR_CHANNEL_A' or 'CY_FR_CHANNEL_B'.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_get_aggregated_channel_status ( volatile stc_FLEXRAY_t *                 base,
                                                            CY_FR_CHANNEL_TYPE                       cy_fr_channel,
                                                            CY_FR_AGGREGATED_CHANNEL_STATUS_STRUCT * data_struct )
{
  if ( cy_fr_channel == CY_FR_CHANNEL_A ){
    return cy_fr_ccal_get_aggregated_channel_a_status(base, &(*data_struct));
  } else if ( cy_fr_channel == CY_FR_CHANNEL_B ){
    return cy_fr_ccal_get_aggregated_channel_b_status(base, &(*data_struct));
  } else {
    return CY_FR_INVALID_CHANNEL;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function reads out the Slot Mode.

    \param[in]  base                   Pointer to FlexRay register base
    
    \retval CY_FR_SLOT_MODE_TYPE       The actual Slot Mode
 */
/*---------------------------------------------------------------------------------*/
CY_FR_SLOT_MODE_TYPE cy_fr_api_get_slot_mode ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_slot_mode(base);
}

/*---------------------------------------------------------------------------------*/
/** \brief  This function reads out the Protocol Operation Control Status Bits ( CC 
            Status Vector POCS [5:0] ).

    \param[in]  base                   Pointer to FlexRay register base
    
    \retval CY_FR_POC_STATUS_TYPE      The status of the FlexRay Communication Controller
 */
/*---------------------------------------------------------------------------------*/
CY_FR_POC_STATUS_TYPE cy_fr_api_get_poc_status( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_poc_status(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Protocol Operation Control Status Log 
                Bits ( CC Status Vector PSL [5:0] ) in HALT state.

    \param[in]  base                    Pointer to FlexRay register base
    
    \retval     CY_FR_POC_STATUS_TYPE   The last status of the FlexRay Communication
                                        Controller before entering halt
 */
/*---------------------------------------------------------------------------------*/
CY_FR_POC_STATUS_TYPE cy_fr_api_get_last_poc_status( volatile stc_FLEXRAY_t * base )
{
  if ( cy_fr_api_pocs_is_halt(base) == CY_FR_TRUE ){
    return cy_fr_ccal_get_last_poc_status(base);
  } else {
    return CY_FR_POCS_LOG_INVALID_NOT_HALT;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief   This function reads out the Status Flags for Faulty message Buffer
             Detected and Multiple Faulty message Buffer Detected and clear the flags.

    \param[in]  base                       Pointer to FlexRay register base
    
    \retval  CY_FR_FALSE                   In case not faulty buffer detected
    \retval  CY_FR_FAULTY_BUFFER           If one buffer is faulty
    \retval  CY_FR_MULTIPLE_FAULTY_BUFFER  If more then one buffer is faulty
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_buffer_faulty ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_is_buffer_faulty(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function reads out the faulty buffer number ( Message Handler Status
            Register FMB[6:0] ).

    \param[in]  base                   Pointer to FlexRay register base
    
    \retval CY_FR_BUFFER_TYPE          The buffer number of the faulty buffer.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_BUFFER_TYPE cy_fr_api_get_faulty_buffer ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_faulty_buffer(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function reads out the last transmitted buffer number ( Message
            Handler Status Register MBT[6:0] ).

    \param[in]  base                   Pointer to FlexRay register base
    
    \retval CY_FR_BUFFER_TYPE          The buffer number of the last transmitted buffer
 */
/*---------------------------------------------------------------------------------*/
CY_FR_BUFFER_TYPE cy_fr_api_get_last_tx_buffer ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_last_tx_buffer(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function reads out the last updated buffer number ( Message Handler
            Status Register MBU[6:0] ).

    \param[in]  base                   Pointer to FlexRay register base
    
    \retval CY_FR_BUFFER_TYPE          The buffer number of the last updated buffer.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_BUFFER_TYPE cy_fr_api_get_last_updated_buffer ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_last_updated_buffer(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out following the buffer parity flags
                \li Parity Error Input Buffer RAM 1,2      PIPF
                \li Parity Error Output Buffer RAM 1,2     POBF
                \li Parity Error Message RAM               PMR
                \li Parity Error Transient Buffer RAM A    PTBF1
                \li Parity Error Transient Buffer RAM B    PTBF2
                \n of the Message Handler Status Register ( MHDS ) and clears the
                flags.

    \param[in]  base                  Pointer to FlexRay register base
    \param[in]  *data_struct          Pointer to a 'CY_FR_BUFFER_PARITY_STRUCT'.

    \retval     CY_FR_OKAY            Always
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_get_buffer_parity_flags ( volatile stc_FLEXRAY_t *     base,
                                                      CY_FR_BUFFER_PARITY_STRUCT * data_struct )
{
  return  cy_fr_ccal_get_buffer_parity_flags(base, &(*data_struct));
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function reads out the Remaining Coldstarts Attempts of the
            Communication Controller Status Vector ( CCSV ).

    \param[in]  base     Pointer to FlexRay register base
    
    \retval uint8_t      The remaining coldstarts.

     */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_api_get_remaining_coldstarts ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_remaining_coldstarts(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function read out the actual rate correction value in the
            Rate Correction Value Register ( RCV ).

    \param[in]  base     Pointer to FlexRay register base
    
    \retval int16_t      The actual rate correction value.
 */
/*---------------------------------------------------------------------------------*/
int16_t cy_fr_api_get_rate_correction ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_rate_correction(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function read out the actual offset correction value in the
            Offset Correction Value Register ( OCV ).

    \param[in]  base     Pointer to FlexRay register base
    
    \retval int32_t      The actual offset correction value.
 */
/*---------------------------------------------------------------------------------*/
int32_t cy_fr_api_get_offset_correction ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_offset_correction(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function read out the actual clock correction value in the
            Communication Controller Error Vector ( CCEV )

    \param[in]  base     Pointer to FlexRay register base
    
    \retval uint8_t      The actual clock correction value.
 */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_api_get_clock_correction ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_clock_correction(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function reads out the actual Protocol Operation Control Error Mode
            in the Communication Error Vector ( CCEV )

    \param[in]  base               Pointer to FlexRay register base
    
    \retval CY_FR_ERROR_MODE_TYPE  The actual error mode of the FlexRay CC
 */
/*---------------------------------------------------------------------------------*/
CY_FR_ERROR_MODE_TYPE cy_fr_api_get_error_mode ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_error_mode(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function reads out the Passive to Active Counter in the
            Communication Controller Error Vector ( CCEV )

    \param[in]  base     Pointer to FlexRay register base
    
    \retval uint8_t      The Passive to Active counter value.
 */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_api_get_pta_counter ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_get_pta_counter(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the Status not Updated flag in the Message
                Handler Constraint Flag Register ( MHDF ) for channel 'cy_fr_channel'.

    \param[in]  base                   Pointer to FlexRay register base
    \param[in]  cy_fr_channel          The channel number ( allowed values are
                                       'CY_FR_CHANNEL_A' and 'CY_FR_CHANNEL_B' ).

    \retval     CY_FR_TRUE             In case the flag is set
    \retval     CY_FR_FALSE            If the flag is not set.
    \retval     CY_FR_INVALID_CHANNEL  If a channel is requested else then
                                       'CY_FR_CHANNEL_A' or 'CY_FR_CHANNEL_B'
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_status_not_updated ( volatile stc_FLEXRAY_t * base,
                                                 CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  if ( cy_fr_channel == CY_FR_CHANNEL_A ){
    return cy_fr_ccal_status_not_updated_a(base);
  } else if ( cy_fr_channel == CY_FR_CHANNEL_B ){
    return cy_fr_ccal_status_not_updated_b(base);
  } else {
    return CY_FR_INVALID_CHANNEL;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the Find Sequence not finished flag in the
                Message Handler Constraint Flag Register ( MHDF ) for channel
                'cy_fr_channel'.

    \param[in]  base                   Pointer to FlexRay register base
    \param[in]  cy_fr_channel          The channel number ( allowed values are
                                       'CY_FR_CHANNEL_A' and 'CY_FR_CHANNEL_B' ).

    \retval     CY_FR_TRUE             In case the flag is set.
    \retval     CY_FR_FALSE            If the flag is not set.
    \retval     CY_FR_INVALID_CHANNEL  If a channel is requested else then
                                       'CY_FR_CHANNEL_A' or 'CY_FR_CHANNEL_B'.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_find_sequence_not_finished ( volatile stc_FLEXRAY_t * base,
                                                         CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  if ( cy_fr_channel == CY_FR_CHANNEL_A ){
    return cy_fr_ccal_find_sequence_not_finished_a(base);
  } else if ( cy_fr_channel == CY_FR_CHANNEL_B ){
    return cy_fr_ccal_find_sequence_not_finished_b(base);
  } else {
    return CY_FR_INVALID_CHANNEL;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the Write Attempt to Header Partition flag in
                the Message Handler Constraint Flag Register ( MHDF ).

    \param[in]  base                   Pointer to FlexRay register base
    
    \retval     CY_FR_TRUE             In case the flag is set.
    \retval     CY_FR_FALSE            If the flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_write_attempt_header ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_write_attempt_header(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the Freeze Indicator flag of the Communication
                Controller Status Vector ( CCSV ).

    \param[in]  base                   Pointer to FlexRay register base
    
    \retval     CY_FR_TRUE             In case the flag is set.
    \retval     CY_FR_FALSE            If the flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_freeze_indicator_set ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_is_freeze_indicator_set(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the Media Access Test Symbol received flag in
                the Symbol Window and Network Idle Time status register ( SWNIT )
                for channel 'cy_fr_channel'.

    \param[in]  base                   Pointer to FlexRay register base    
    \param[in]  cy_fr_channel          The channel number ( allowed values are
                                       'CY_FR_CHANNEL_A' and 'CY_FR_CHANNEL_B' ).

    \retval     CY_FR_TRUE             In case the flag is set.
    \retval     CY_FR_FALSE            If the flag is not set.
    \retval     CY_FR_INVALID_CHANNEL  If a channel is requested else then
                                       'CY_FR_CHANNEL_A' or 'CY_FR_CHANNEL_B'.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_rx_mts ( volatile stc_FLEXRAY_t * base,
                                        CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  if ( cy_fr_channel == CY_FR_CHANNEL_A ){
    return cy_fr_ccal_is_rx_mts_a(base);
  } else if ( cy_fr_channel == CY_FR_CHANNEL_B ){
    return cy_fr_ccal_is_rx_mts_b(base);
  } else {
    return CY_FR_INVALID_CHANNEL;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the Syntax Error in Symbol Window flag
                in the Symbol Window and Network Idle Time status register ( SWNIT )
                for channel 'cy_fr_channel'.

    \param[in]  base                   Pointer to FlexRay register base
    \param[in]  cy_fr_channel          The channel number ( allowed values are
                                       'CY_FR_CHANNEL_A' and 'CY_FR_CHANNEL_B' ).

    \retval     CY_FR_TRUE             In case the flag is set.
    \retval     CY_FR_FALSE            If the flag is not set.
    \retval     CY_FR_INVALID_CHANNEL  If a channel is requested else then
                                       'CY_FR_CHANNEL_A' or 'CY_FR_CHANNEL_B'.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_syntax_error_symbol_window ( volatile stc_FLEXRAY_t * base,
                                                            CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  if ( cy_fr_channel == CY_FR_CHANNEL_A ){
    return cy_fr_ccal_is_syntax_error_symbol_window_a(base);
  } else if ( cy_fr_channel == CY_FR_CHANNEL_B ){
    return cy_fr_ccal_is_syntax_error_symbol_window_b(base);
  } else {
    return CY_FR_INVALID_CHANNEL;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the Slot Boundary Violation in Symbol Window flag
                in the Symbol Window and Network Idle Time status register ( SWNIT )
                for channel 'cy_fr_channel'.

    \param[in]  base                   Pointer to FlexRay register base
    \param[in]  cy_fr_channel          The channel number ( allowed values are
                                       'CY_FR_CHANNEL_A' and 'CY_FR_CHANNEL_B' ).

    \retval     CY_FR_TRUE             In case the flag is set.
    \retval     CY_FR_FALSE            If the flag is not set.
    \retval     CY_FR_INVALID_CHANNEL  If a channel is requested else then
                                       'CY_FR_CHANNEL_A' or 'CY_FR_CHANNEL_B'.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_slot_boundary_symbol_window ( volatile stc_FLEXRAY_t * base,
                                                             CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  if ( cy_fr_channel == CY_FR_CHANNEL_A ){
    return cy_fr_ccal_is_slot_boundary_symbol_window_a(base);
  } else if ( cy_fr_channel == CY_FR_CHANNEL_B ){
    return cy_fr_ccal_is_slot_boundary_symbol_window_b(base);
  } else {
    return CY_FR_INVALID_CHANNEL;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the Transmission Conflict in Symbol Window flag
                in the Symbol Window and Network Idle Time status register ( SWNIT )
                for channel 'cy_fr_channel'.

    \param[in]  base                   Pointer to FlexRay register base
    \param[in]  cy_fr_channel          The channel number ( allowed values are
                                       'CY_FR_CHANNEL_A' and 'CY_FR_CHANNEL_B' ).

    \retval     CY_FR_TRUE             In case the flag is set.
    \retval     CY_FR_FALSE            If the flag is not set.
    \retval     CY_FR_INVALID_CHANNEL  If a channel is requested else then
                                       'CY_FR_CHANNEL_A' or 'CY_FR_CHANNEL_B'.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_tx_conflict_symbol_window ( volatile stc_FLEXRAY_t * base,
                                                           CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  if ( cy_fr_channel == CY_FR_CHANNEL_A ){
    return cy_fr_ccal_is_tx_conflict_symbol_window_a(base);
  } else if ( cy_fr_channel == CY_FR_CHANNEL_B ){
    return cy_fr_ccal_is_tx_conflict_symbol_window_b(base);
  } else {
    return CY_FR_INVALID_CHANNEL;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the Syntax Error during Network Idle Time flag
                in the Symbol Window and Network Idle Time status register ( SWNIT )
                for channel 'cy_fr_channel'.

    \param[in]  base                   Pointer to FlexRay register base
    \param[in]  cy_fr_channel          The channel number ( allowed values are
                                       'CY_FR_CHANNEL_A' and 'CY_FR_CHANNEL_B' ).

    \retval     CY_FR_TRUE             In case the flag is set.
    \retval     CY_FR_FALSE            If the flag is not set.
    \retval     CY_FR_INVALID_CHANNEL  If a channel is requested else then
                                       'CY_FR_CHANNEL_A' or 'CY_FR_CHANNEL_B'.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_syntax_error_nit ( volatile stc_FLEXRAY_t * base,
                                                  CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  if ( cy_fr_channel == CY_FR_CHANNEL_A ){
    return cy_fr_ccal_is_syntax_error_nit_a(base);
  } else if ( cy_fr_channel == CY_FR_CHANNEL_B ){
    return cy_fr_ccal_is_syntax_error_nit_b(base);
  } else {
    return CY_FR_INVALID_CHANNEL;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the Slot Boundary Violation during Network Idle
                Time flag in the Symbol Window and Network Idle Time status register
                ( SWNIT ) for channel 'cy_fr_channel'.

    \param[in]  base                   Pointer to FlexRay register base
    \param[in]  cy_fr_channel          The channel number ( allowed values are
                                       'CY_FR_CHANNEL_A' and 'CY_FR_CHANNEL_B' ).

    \retval     CY_FR_TRUE             In case the flag is set.
    \retval     CY_FR_FALSE            If the flag is not set.
    \retval     CY_FR_INVALID_CHANNEL  If a channel is requested else then
                                       'CY_FR_CHANNEL_A' or 'CY_FR_CHANNEL_B'.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_slot_boundary_nit ( volatile stc_FLEXRAY_t * base,
                                                   CY_FR_CHANNEL_TYPE       cy_fr_channel )
{
  if ( cy_fr_channel == CY_FR_CHANNEL_A ){
    return cy_fr_ccal_is_slot_boundary_nit_a(base);
  } else if ( cy_fr_channel == CY_FR_CHANNEL_B ){
    return cy_fr_ccal_is_slot_boundary_nit_b(base);
  } else {
    return CY_FR_INVALID_CHANNEL;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the complete Network Management Vector of
                12 bytes into an array pointed by *cy_fr_nmv[0].

    \param[in]   base         Pointer to FlexRay register base
    \param[out]  cy_fr_nmv    Pointer to a 12 byte array where the NMV will be stored

    \retval      CY_FR_OKAY   Always, no error handling.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_get_network_managment_vector ( volatile stc_FLEXRAY_t * base,
                                                           uint8_t                  cy_fr_nmv[] )
{
  return cy_fr_ccal_get_network_managment_vector(base, cy_fr_nmv);
}



/*---------------------------------------------------------------------------------*/
/** \brief  This function checks if the missing offset correction signal flag is set
            in the Sync Frame Status Register ( SFS ).

    \param[in]  base         Pointer to FlexRay register base
    
    \retval     CY_FR_TRUE   In case the flag is set.
    \retval     CY_FR_FALSE  If the flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_missing_offset_correction ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_is_missing_offset_correction(base);
}



/*---------------------------------------------------------------------------------*/
/** \brief  This function checks if the offset correction limit reached flag is set
            in the Sync Frame Status Register ( SFS ).

    \param[in]  base     Pointer to FlexRay register base
    
    \retval CY_FR_TRUE   In case the flag is set.
    \retval CY_FR_FALSE  If the flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_offset_correction_limit ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_is_offset_correction_limit(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function checks if the missing rate correction signal flag is set in
            the Sync Frame Status Register ( SFS ).

    \param[in]  base     Pointer to FlexRay register base
    
    \retval CY_FR_TRUE   In case the flag is set.
    \retval CY_FR_FALSE  If the flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_missing_rate_correction ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_is_missing_rate_correction(base);
}


/*---------------------------------------------------------------------------------*/
/** \brief  This function checks if the rate correction limit reached flag is set in
            the Sync Frame Status Register ( SFS ).

    \param[in]  base          Pointer to FlexRay register base
    
    \retval     CY_FR_TRUE    In case the flag is set.
    \retval     CY_FR_FALSE   If the flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_is_rate_correction_limit ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_ccal_is_rate_correction_limit(base);
}



/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the number of received Sync Frames for even
                or odd cycle and for Channel A or Channel B.

    \param[in]  base           Pointer to FlexRay register base    
    \param[in]  cy_fr_channel  The channel number ( allowed values are 'CY_FR_CHANNEL_A'
                               and 'CY_FR_CHANNEL_B' ).
    \param[in]  cy_fr_cycle    Selcet an even or odd cycle ( allowed values are
                               'CY_FR_EVEN' and 'CY_FR_ODD' ).

    \retval     uint8_t                The number of received sync_frames ( 0 - 15 ).
    \retval     CY_FR_INVALID_CHANNEL  An invalid channel requested ( 16 ).
    \retval     CY_FR_INVALID_CYCLE    An invalid cycle rquested ( 17 ).
 */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_api_get_number_of_sync_frame ( volatile stc_FLEXRAY_t * base,
                                             CY_FR_CHANNEL_TYPE       cy_fr_channel,
                                             CY_FR_CYCLE_TYPE         cy_fr_cycle )
{
  if ( cy_fr_channel == CY_FR_CHANNEL_A ){
    if ( cy_fr_cycle == CY_FR_EVEN ){
      return cy_fr_ccal_get_number_of_even_sync_frame_a(base);
    } else if ( cy_fr_cycle == CY_FR_ODD ){
      return cy_fr_ccal_get_number_of_odd_sync_frame_a(base);
    } else {
      return CY_FR_INVALID_CYCLE;
    }
  } else if ( cy_fr_channel == CY_FR_CHANNEL_B ){
    if ( cy_fr_cycle == CY_FR_EVEN ) {
      return cy_fr_ccal_get_number_of_even_sync_frame_b(base);
    } else if ( cy_fr_cycle == CY_FR_ODD ){
      return cy_fr_ccal_get_number_of_odd_sync_frame_b(base);
    } else {
      return CY_FR_INVALID_CYCLE;
    }
  } else {
    return CY_FR_INVALID_CHANNEL;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief       This function reads out the received Sync Frame 'cy_fr_frame_no' for
                 'cy_fr_cycle' ( 'CY_FR_EVEN' or 'CY_FR_ODD' ) and copy the Frame ID and
                 the channel number to the structure pointed by '*cy_fr_sync_frame'.

    \param[in]   base               Pointer to FlexRay register base
    \param[in]   cy_fr_cycle        The cycle for the sync frame ( 'CY_FR_EVEN' or
                                    'CY_FR_ODD' )
    \param[in]   cy_fr_frame_no     The number of the Sync frame.
    \param[out]  *cy_fr_sync_frame  A pointer to a 'CY_FR_SYNC_FRAME_STRUCT'.

    \retval      CY_FR_OKAY         In case of success
    \retval      CY_FR_ERROR        If 'cy_fr_cycle' is not 'CY_FR_EVEN' or 'CY_FR_ODD'
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_get_sync_frame ( volatile stc_FLEXRAY_t *  base,
                                             CY_FR_CYCLE_TYPE          cy_fr_cycle,
                                             uint8_t                   cy_fr_frame_no,
                                             CY_FR_SYNC_FRAME_STRUCT * cy_fr_sync_frame )
{
  return cy_fr_ccal_get_sync_frame (base, cy_fr_cycle, cy_fr_frame_no, &(*cy_fr_sync_frame));
}


/*---------------------------------------------------------------------------------*/


#if defined(__cplusplus)
}
#endif


/* [] END OF FILE */
