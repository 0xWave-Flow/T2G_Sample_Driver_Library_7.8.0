/***************************************************************************//**
* \file cy_fr_ccal_status_service.c
* \version 1.0
*
* \brief
* Routines for the Status Service.
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
/** \brief      This function reads out the Protocol Operation Control Status Bits
                in the System Universal Control Configuration Register 1 ( SUCC1 ).

    \param[in]  base                   Pointer to FlexRay register base
    
    \retval     CY_FR_POC_STATUS_TYPE  The status of the FlexRay CC (A list of all
                                       values define in cy_fr_api_global.h at section
                                       'CY_FR_POC_STATUS_TYPE' )
 */
/*---------------------------------------------------------------------------------*/
CY_FR_POC_STATUS_TYPE cy_fr_ccal_get_poc_status ( volatile stc_FLEXRAY_t * base )
{
  return (CY_FR_POC_STATUS_TYPE)(cy_fr_fhal_read32(CY_FR_REG_CCSV) & 0x3F);  /* read POC status (POCS) */
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Protocol Operation Control Status LOG
                Bits in the Communication Controller Status Vector ( CCVS ).

    \param[in]  base                   Pointer to FlexRay register base
    
    \retval     CY_FR_POC_STATUS_TYPE  The status of the FlexRay CC (A list of all
                                       values define in cy_fr_api_global.h at section
                                       'CY_FR_POC_STATUS_TYPE' ).
 */
/*---------------------------------------------------------------------------------*/
CY_FR_POC_STATUS_TYPE cy_fr_ccal_get_last_poc_status ( volatile stc_FLEXRAY_t * base )
{
  return (CY_FR_POC_STATUS_TYPE)( ( cy_fr_fhal_read32(CY_FR_REG_CCSV) & 0x3F000000) >> 24 );  /* read POC status log (PSL) */
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Wakeup Status Bits in the Communication
                Controller Status Vector ( CCVS ).

    \param[in]  base         Pointer to FlexRay register base
    
    \retval     CY_FR_TRUE   If FlexRay CC is in a Wakeup state.
    \retval     CY_FR_FALSE  In case the FlexRay CC is not in any Wakeup state.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_WAKEUP_STATUS_TYPE cy_fr_ccal_get_wakeup_status ( volatile stc_FLEXRAY_t * base )
{
  return (CY_FR_WAKEUP_STATUS_TYPE)((cy_fr_fhal_read32(CY_FR_REG_CCSV) & 0x30000) >> 16 );  /* read Wakeup status vector (WSV) */
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the error interrupt flag 'cy_fr_int' from the
                error interrupt register ( EIR ).

    \param[in]  base         Pointer to FlexRay register base    
    \param[in]  cy_fr_int    An interrupt (A list of all allowed values define in
                             cy_fr_api_global.h at section 'CY_FR_INT_TYPE' )

    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_get_error_interrupt_status ( volatile stc_FLEXRAY_t * base,
                                                          CY_FR_INTERRUPT_TYPE     cy_fr_int )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_EIR) & ( 0x00000001 << cy_fr_int ) ) >> cy_fr_int ) {
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the status interrupt flag 'cy_fr_int' from the
                status interrupt register ( SIR ).

    \param[in]  base         Pointer to FlexRay register base    
    \param[in]  cy_fr_int    An interrupt (A list of all allowed values define in
                             cy_fr_api_global.h at section 'CY_FR_INT_TYPE' )

    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_get_status_interrupt_status ( volatile stc_FLEXRAY_t * base,
                                                           CY_FR_INTERRUPT_TYPE     cy_fr_int )
{
  uint8_t cy_fr_bit_shift;
  cy_fr_bit_shift = (uint8_t)cy_fr_int - 0x40;
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SIR) & ( 0x00000001 << cy_fr_bit_shift ) ) >> cy_fr_bit_shift ) {
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Aggregated channel status for channel A
                and writes the five status flags in the struct pointed by
                '*cy_fr_data_struct'.

    \param[in]  base                Pointer to FlexRay register base    
    \param[out] *cy_fr_data_struct  A structure of type 'CY_FR_AGGREGATED_CHANNEL_STATUS_STRUCT'.

    \retval     CY_FR_OKAY          Always, no error handling.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_get_aggregated_channel_a_status ( volatile stc_FLEXRAY_t *                 base,
                                                               CY_FR_AGGREGATED_CHANNEL_STATUS_STRUCT * cy_fr_data_struct )
{
  uint32_t cy_fr_temp_reg;

  cy_fr_temp_reg = cy_fr_fhal_read32(CY_FR_REG_ACS);
  cy_fr_fhal_write32(0x1F1F,CY_FR_REG_ACS);
  cy_fr_data_struct->VALID_FRAME             = ((cy_fr_temp_reg & 0x01) != 0) ? true : false;      /* VFRA */
  cy_fr_data_struct->SYNTAX_ERROR            = ((cy_fr_temp_reg & 0x02) != 0) ? true : false;      /* SEDA */
  cy_fr_data_struct->CONTENT_ERROR           = ((cy_fr_temp_reg & 0x04) != 0) ? true : false;      /* CEDA */
  cy_fr_data_struct->COMMUNICATION_INDICATOR = ((cy_fr_temp_reg & 0x08) != 0) ? true : false;      /* CIA  */
  cy_fr_data_struct->SLOT_BOUNDARY_VIOLATION = ((cy_fr_temp_reg & 0x10) != 0) ? true : false;      /* SBVA */
  return CY_FR_OKAY;
}

/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Aggregated channel status for channel B
                and writes the five status flags in the struct pointed by
                '*cy_fr_data_struct'.

    \param[in]  base                Pointer to FlexRay register base    
    \param[out] *cy_fr_data_struct  A structure of type 'CY_FR_AGGREGATED_CHANNEL_STATUS_STRUCT'.

    \retval     CY_FR_OKAY  Always, no error handling.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_get_aggregated_channel_b_status ( volatile stc_FLEXRAY_t *                 base,
                                                               CY_FR_AGGREGATED_CHANNEL_STATUS_STRUCT * cy_fr_data_struct )
{
  uint32_t cy_fr_temp_reg;

  cy_fr_temp_reg = cy_fr_fhal_read32(CY_FR_REG_ACS);
  cy_fr_fhal_write32(0x1F1F,CY_FR_REG_ACS);
  cy_fr_data_struct->VALID_FRAME             = ((cy_fr_temp_reg & 0x0100) != 0) ? true : false;      /* VFRB */
  cy_fr_data_struct->SYNTAX_ERROR            = ((cy_fr_temp_reg & 0x0200) != 0) ? true : false;      /* SEDB */
  cy_fr_data_struct->CONTENT_ERROR           = ((cy_fr_temp_reg & 0x0400) != 0) ? true : false;      /* CEDB */
  cy_fr_data_struct->COMMUNICATION_INDICATOR = ((cy_fr_temp_reg & 0x0800) != 0) ? true : false;      /* CIB  */
  cy_fr_data_struct->SLOT_BOUNDARY_VIOLATION = ((cy_fr_temp_reg & 0x1000) != 0) ? true : false;      /* SBVB */
  return CY_FR_OKAY;
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Slot Mode ( CCSV_SLM ) of the FlexRay CC.

    \param[in]  base               Pointer to FlexRay register base    
    
    \retval     CY_FR_SINGLE       If the FlexRay CC in Slot mode SINGLE.
    \retval     CY_FR_ALL_PENDING  In case the FlexRay CC is in Mode ALL_PENDING.
    \retval     CY_FR_PENDING      If the CC is in Slot mode PENDING.

 */
/*---------------------------------------------------------------------------------*/
CY_FR_SLOT_MODE_TYPE cy_fr_ccal_get_slot_mode ( volatile stc_FLEXRAY_t * base )
{
  return (CY_FR_SLOT_MODE_TYPE)( ( cy_fr_fhal_read32(CY_FR_REG_CCSV) & 0x300 ) >> 8 );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This Function checks the cold start noisy flag in the Communication
                Controller Status Vector ( CCSV ).

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_coldstart_noisy ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_CCSV) & 0x1000 ) == 0x1000 ){ /* cold start noisy indicator flag */
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This Function checks the cold start abort flag in the Communication
                Controller Status Vector ( CCSV ).

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_coldstart_abort ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_CCSV) & 0x2000 ) == 0x2000 ){ /* cold start abort indicator flag */
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This Function checks the cold start inhibit flag in the Communication
                Controller Status Vector ( CCSV ).

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_coldstart_inhibit ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_CCSV) & 0x4000 ) == 0x4000 ){ /* cold start inhibit flag */
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This Function checks the Clear rams active flag in the Message
                Handler Status Register ( MHDS ).

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_clear_rams_active ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_MHDS) & 0x80 ) == 0x80 ) {
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Status Flags for Faulty message Buffer
                Detected and Multiple Faulty message Buffer Detected and clear the
                flags. To get the Faulty Buffer use cy_fr_api_get_faulty_buffer ( ).

    \param[in]  base                         Pointer to FlexRay register base    
    
    \retval     CY_FR_FALSE                  If no faulty buffer was detected.
    \retval     CY_FR_FAULTY_BUFFER          In case one buffer is faulty.
    \retval     CY_FR_MULTIPLE_FAULTY_BUFFER If more than one buffer is faulty.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_buffer_faulty ( volatile stc_FLEXRAY_t * base )
{
  switch( ( cy_fr_fhal_read32 ( CY_FR_REG_MHDS ) & 0x60 ) >> 5 ){
    case 0 : {
      return CY_FR_FALSE;
    }
    case 1 : {
      cy_fr_fhal_write32(0x10,CY_FR_REG_MHDS);
      return CY_FR_FAULTY_BUFFER;
    }
    case 3 : {
      cy_fr_fhal_write32(0x30,CY_FR_REG_MHDS);
      return CY_FR_MULTIPLE_FAULTY_BUFFER;
    }
  }
  return CY_FR_FALSE; /* no reachable because all possible options checked before */
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the number of Faulty message Buffer. The Flag
                MHDS_FMBD is not check, to check it use cy_fr_api_is_buffer_faulty ( ).

    \param[in]  base                  Pointer to FlexRay register base    
    
    \retval     CY_FR_BUFFER_TYPE     The number of Faulty buffer.
    \retval     CY_FR_INVALID_BUFFER  In case of an invalid buffer.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_BUFFER_TYPE cy_fr_ccal_get_faulty_buffer ( volatile stc_FLEXRAY_t * base )
{
  uint32_t cy_fr_temp_reg;

  cy_fr_temp_reg = cy_fr_fhal_read32(CY_FR_REG_MHDS);
  if ( ( cy_fr_temp_reg & 0x1D ) > 0 ){
    return ( ( cy_fr_temp_reg & 0x7f00 ) >> 8 );
  } else {
    return CY_FR_INVALID_BUFFER;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the buffer number of last transmited message
                Buffer.

    \param[in]  base                  Pointer to FlexRay register base    
    
    \retval     CY_FR_BUFFER_TYPE     The number of last transmitted buffer.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_BUFFER_TYPE cy_fr_ccal_get_last_tx_buffer ( volatile stc_FLEXRAY_t * base )
{
  return ( ( cy_fr_fhal_read32(CY_FR_REG_MHDS) & 0x7f0000 ) >> 16 );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function readout the buffer number of last updated message Buffer.

    \param[in]  base               Pointer to FlexRay register base    
    
    \retval     CY_FR_BUFFER_TYPE  The number of last updated buffer.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_BUFFER_TYPE cy_fr_ccal_get_last_updated_buffer ( volatile stc_FLEXRAY_t * base )
{
  return ( ( cy_fr_fhal_read32(CY_FR_REG_MHDS) & 0x7f000000 ) >> 24 );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the buffer parity flags and transfer them
                into the struct pointed by '*cy_fr_data_struct'.

    \param[in]  base        Pointer to FlexRay register base    
    \retval     CY_FR_OKAY  Always, no error handling.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_get_buffer_parity_flags ( volatile stc_FLEXRAY_t *     base,
                                                       CY_FR_BUFFER_PARITY_STRUCT * cy_fr_data_struct )
{
  uint32_t cy_fr_temp_reg;

  cy_fr_temp_reg = cy_fr_fhal_read32(CY_FR_REG_MHDS);
  cy_fr_fhal_write32(0x0000001F,CY_FR_REG_MHDS);
  cy_fr_data_struct->PIBF  = (( cy_fr_temp_reg & 0x01 ) != 0) ? true : false;
  cy_fr_data_struct->POBF  = (( cy_fr_temp_reg & 0x02 ) != 0) ? true : false;
  cy_fr_data_struct->PMR   = (( cy_fr_temp_reg & 0x04 ) != 0) ? true : false;
  cy_fr_data_struct->PTBF1 = (( cy_fr_temp_reg & 0x08 ) != 0) ? true : false;
  cy_fr_data_struct->PTBF2 = (( cy_fr_temp_reg & 0x10 ) != 0) ? true : false;
  return  CY_FR_OKAY;
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the number of remaining cold starts of the
                FlexRay CC.

    \param[in]  base     Pointer to FlexRay register base    
    \retval     uint8_t  The number of remaining cold starts.
 */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_ccal_get_remaining_coldstarts ( volatile stc_FLEXRAY_t * base )
{
  return ( ( cy_fr_fhal_read32(CY_FR_REG_CCSV) & 0x00F80000 ) >> 19 );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the rate correction value of the FlexRay CC.

    \param[in]  base     Pointer to FlexRay register base    
    
    \retval     int16_t  The rate correction value.
 */
/*---------------------------------------------------------------------------------*/
int16_t cy_fr_ccal_get_rate_correction ( volatile stc_FLEXRAY_t * base )
{
  return (uint16_t)cy_fr_fhal_read32(CY_FR_REG_RCV);
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the offset correction value of the FlexRay
                CC.

    \param[in]  base     Pointer to FlexRay register base    
    
    \retval     int32_t  The offset correction value.
 */
/*---------------------------------------------------------------------------------*/
int32_t cy_fr_ccal_get_offset_correction ( volatile stc_FLEXRAY_t * base )
{
  return cy_fr_fhal_read32(CY_FR_REG_OCV);
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the clock correction value of the FlexRay CC.

    \param[in]  base     Pointer to FlexRay register base    
    
    \retval     uint8_t  The clock correction value.
 */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_ccal_get_clock_correction ( volatile stc_FLEXRAY_t * base )
{
  return ( cy_fr_fhal_read32(CY_FR_REG_CCEV)& 0xF );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Passive to Active Counter value of the
                FlexRay CC.

    \param[in]  base    Pointer to FlexRay register base    
    
    \retval     uint8_t The Passive to Active counter value.
 */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_ccal_get_pta_counter ( volatile stc_FLEXRAY_t * base )
{
  return ( ( cy_fr_fhal_read32(CY_FR_REG_CCEV) & 0x1800 ) >> 8 );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the Error mode of the Protocol Operation
                Controll.

    \param[in]  base             Pointer to FlexRay register base    
    
    \retval     CY_FR_ACTIVE     In case Error mode is ACTIVE.
                CY_FR_PASSIVE    If Error mode is PASSIVE.
                CY_FR_COMM_HALT  In case Error mode is COMM_HALT.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_ERROR_MODE_TYPE cy_fr_ccal_get_error_mode ( volatile stc_FLEXRAY_t * base )
{
  return (CY_FR_ERROR_MODE_TYPE)( ( cy_fr_fhal_read32(CY_FR_REG_CCEV) & 0xC0 ) >> 6 );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the status not updated flags for channel A

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_status_not_updated_a ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_MHDF) & 0x01 ) == 0x01 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the status not updated flags for channel B

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_status_not_updated_b ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_MHDF) & 0x02 ) == 0x02 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function the find sequence not finished flag for channel A.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_find_sequence_not_finished_a ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_MHDF) & 0x04 ) == 0x04 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function the find sequence not finished flag for channel B.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_find_sequence_not_finished_b ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_MHDF) & 0x08 ) == 0x08 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the write attempt to header partition flag.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_write_attempt_header ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_MHDF) & 0x100 )  == 0x100 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the Media Access Test Symbol received flag for
                channel A.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_rx_mts_a ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SWNIT) & 0x40 ) == 0x40 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the Media Access Test Symbol received flag for
                channel B.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_rx_mts_b ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SWNIT) & 0x80 ) == 0x80 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the freeze indicator flag.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_freeze_indicator_set ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_CCSV) & 0x40 ) == 0x40 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the syntax error at symbol window flag for
                channel A.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_syntax_error_symbol_window_a ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SWNIT) & 0x1 ) == 0x1 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the syntax error at symbol window flag for
                channel B.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_syntax_error_symbol_window_b ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SWNIT) & 0x8 ) == 0x8 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the slot boundary Violation in symbol window
                flag for channel A.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_slot_boundary_symbol_window_a ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SWNIT) & 0x2 ) == 0x2 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the slot boundary Violation in symbol window
                flag for channel B.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_slot_boundary_symbol_window_b ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SWNIT) & 0x10 ) == 0x10 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the transmission conflict in symbol window
                flag for channel A.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_tx_conflict_symbol_window_a ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SWNIT) & 0x4 ) == 0x4 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the transmission conflict in symbol window
                flag for channel B.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_tx_conflict_symbol_window_b ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SWNIT) & 0x20 ) == 0x20 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the syntax error during Network Idle Time flag
                for channel A.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_syntax_error_nit_a ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SWNIT) & 0x100 ) == 0x100 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the syntax error during Network Idle Time flag
                for channel B.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_syntax_error_nit_b ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SWNIT) & 0x400 ) == 0x400 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the slot boundary during Network Idle Time flag
                for channel A.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_slot_boundary_nit_a ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SWNIT) & 0x200 ) == 0x200 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the slot boundary during Network Idle Time flag
                for channel B.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the Flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_slot_boundary_nit_b ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SWNIT) & 0x800 ) == 0x800 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the network management vector and transfer
                the data to the memory pointed by 'cy_fr_nmv[]'.

    \param[in]  base        Pointer to FlexRay register base    
    
    \retval     CY_FR_OKAY  Always, no error handling.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_get_network_managment_vector ( volatile stc_FLEXRAY_t * base,
                                                            uint8_t                  cy_fr_nmv[] )
{
  uint32_t cy_fr_temp_reg = 0;

  cy_fr_temp_reg = cy_fr_fhal_read32(CY_FR_REG_NMV1);
  cy_fr_nmv[0] = cy_fr_temp_reg & 0xFF;
  cy_fr_temp_reg = cy_fr_temp_reg >> 8 ;
  cy_fr_nmv[1] = cy_fr_temp_reg & 0xFF;
  cy_fr_temp_reg = cy_fr_temp_reg >> 8 ;
  cy_fr_nmv[2] = cy_fr_temp_reg & 0xFF;
  cy_fr_temp_reg = cy_fr_temp_reg >> 8 ;
  cy_fr_nmv[3] = cy_fr_temp_reg & 0xFF;
  cy_fr_temp_reg = cy_fr_temp_reg >> 8 ;
  cy_fr_temp_reg = cy_fr_fhal_read32(CY_FR_REG_NMV2);
  cy_fr_nmv[4] = cy_fr_temp_reg & 0xFF;
  cy_fr_temp_reg = cy_fr_temp_reg >> 8 ;
  cy_fr_nmv[5] = cy_fr_temp_reg & 0xFF;
  cy_fr_temp_reg = cy_fr_temp_reg >> 8 ;
  cy_fr_nmv[6] = cy_fr_temp_reg & 0xFF;
  cy_fr_temp_reg = cy_fr_temp_reg >> 8 ;
  cy_fr_nmv[7] = cy_fr_temp_reg & 0xFF;
  cy_fr_temp_reg = cy_fr_temp_reg >> 8 ;
  cy_fr_temp_reg = cy_fr_fhal_read32(CY_FR_REG_NMV3);
  cy_fr_nmv[8] = cy_fr_temp_reg & 0xFF;
  cy_fr_temp_reg = cy_fr_temp_reg >> 8 ;
  cy_fr_nmv[9] = cy_fr_temp_reg & 0xFF;
  cy_fr_temp_reg = cy_fr_temp_reg >> 8 ;
  cy_fr_nmv[10] = cy_fr_temp_reg & 0xFF;
  cy_fr_temp_reg = cy_fr_temp_reg >> 8 ;
  cy_fr_nmv[11] = cy_fr_temp_reg & 0xFF;
  cy_fr_temp_reg = cy_fr_temp_reg >> 8 ;
  return CY_FR_OKAY;
}



/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the missing offset correction flag.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_missing_offset_correction ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SFS) & 0x10000 ) == 0x10000 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the offset correction limit reached flag.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_offset_correction_limit ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SFS) & 0x20000 ) == 0x20000 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks missing rate correction signal flag.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_missing_rate_correction ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SFS) & 0x40000 ) == 0x40000 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function checks the rate correction limit reached flag.

    \param[in]  base         Pointer to FlexRay register base    
    
    \retval     CY_FR_TRUE   If the flag is set.
    \retval     CY_FR_FALSE  In case the flag is not set.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_is_rate_correction_limit ( volatile stc_FLEXRAY_t * base )
{
  if ( ( cy_fr_fhal_read32(CY_FR_REG_SFS) & 0x80000 ) == 0x80000 ){
    return CY_FR_TRUE;
  } else {
    return CY_FR_FALSE;
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the number of valid sync frames, received in
                even communication cycles on channel A.

    \param[in]  base     Pointer to FlexRay register base    
    
    \retval     uint8_t  The number of even sync frames for channel A.
 */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_ccal_get_number_of_even_sync_frame_a ( volatile stc_FLEXRAY_t * base )
{
  return ( cy_fr_fhal_read32(CY_FR_REG_SFS) & 0x0F );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the number of valid sync frames, received in
                odd communication cycles on channel A.

    \param[in]  base     Pointer to FlexRay register base    
    
    \retval     uint8_t  The number of odd sync frames for channel A.
 */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_ccal_get_number_of_odd_sync_frame_a ( volatile stc_FLEXRAY_t * base )
{
  return ( ( cy_fr_fhal_read32(CY_FR_REG_SFS) & 0xF0 ) >> 4 );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the number of valid sync frames, received in
                even communication cycles on channel B.

    \param[in]  base     Pointer to FlexRay register base    
    
    \retval     uint8_t  The number of even sync frames for channel B.
 */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_ccal_get_number_of_even_sync_frame_b ( volatile stc_FLEXRAY_t * base )
{
  return ( ( cy_fr_fhal_read32(CY_FR_REG_SFS) & 0x0F00 ) >> 8 );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function reads out the number of valid sync frames, received in
                odd communication cycles on channel B.

    \param[in]  base     Pointer to FlexRay register base    
    
    \retval     uint8_t  The number of odd sync frames for channel B.
 */
/*---------------------------------------------------------------------------------*/
uint8_t cy_fr_ccal_get_number_of_odd_sync_frame_b ( volatile stc_FLEXRAY_t * base )
{
  return ( ( cy_fr_fhal_read32(CY_FR_REG_SFS) & 0xF000 ) >> 12 );
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function read out the Frame ID and the Channel for sync frame
                number 'cy_fr_frame_number' received in communication cycle
                'cy_fr_cycle' and transfer the data to the struct pointed by
                '*cy_fr_sync_frame_struct'

    \param[in]  base                      Pointer to FlexRay register base    
    \param[in]  cy_fr_cycle               Request even or odd cycle.
    \param[in]  cy_fr_frame_no            Number of requested Sync Frame.
    \param[out] *cy_fr_sync_frame_struct  Pointer to 'CY_FR_SYNC_FRAME_STRUCT'.

    \retval     CY_FR_OKAY   In case of success.
                CY_FR_ERROR  In case of invalid request.
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_ccal_get_sync_frame ( volatile stc_FLEXRAY_t *  base,
                                              CY_FR_CYCLE_TYPE          cy_fr_cycle,
                                              uint8_t                   cy_fr_frame_no,
                                              CY_FR_SYNC_FRAME_STRUCT * cy_fr_sync_frame_struct )
{
  uint32_t cy_fr_temp_reg = 0;

  if ( cy_fr_cycle == CY_FR_EVEN ){
    cy_fr_temp_reg = cy_fr_fhal_read32(CY_FR_REG_ESID+((cy_fr_frame_no-1)*4));
    cy_fr_sync_frame_struct->FRAME_ID = ( cy_fr_temp_reg & 0x03FF );
    cy_fr_sync_frame_struct->CHANNEL  = (CY_FR_CHANNEL_TYPE)( ( cy_fr_temp_reg & 0xC000 ) >> 14 );
    return CY_FR_OKAY;
  } else if( cy_fr_cycle == CY_FR_ODD ){
    cy_fr_temp_reg = cy_fr_fhal_read32(CY_FR_REG_OSID+((cy_fr_frame_no-1)*4));
    cy_fr_sync_frame_struct->FRAME_ID = ( cy_fr_temp_reg & 0x03FF );
    cy_fr_sync_frame_struct->CHANNEL  = (CY_FR_CHANNEL_TYPE)( ( cy_fr_temp_reg & 0xC000 ) >> 14 );
    return CY_FR_OKAY;
  } else {
    return CY_FR_ERROR;
  }
}


/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


