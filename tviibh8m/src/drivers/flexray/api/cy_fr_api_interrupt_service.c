/***************************************************************************//**
* \file cy_fr_api_interrupt_service.c
* \version 1.0
*
* \brief
* User Interface for Interrupt Service
*
* This File contains the user interface for interrupt service.
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
#include "flexray/api/cy_fr_api_interrupt_service.h"
#include "flexray/ccal/cy_fr_ccal_interrupt_service.h"


/*----------------------------------------------------------------------------------
 * Static Variables
 *---------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/** \brief  Stores the user IRQ callback functions for both interrupt lines
            and for each IP instance.
 */
/*---------------------------------------------------------------------------------*/
static CY_FR_USER_IRQ_CALLBACK_FUNCTION_PTR_TYPE cy_fr_user_interrupt_callback[CY_IP_MXFLEXRAY_INSTANCES][2];


/*----------------------------------------------------------------------------------
 * Function definitions
 *---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/** \brief      This function enables the interrupt 'cy_fr_int'. It verified if 
                'cy_fr_int' is an error or a status interrupt and enables the selected
                interrupt.

    \param[in]  base        Pointer to FlexRay register base
    \param[in]  cy_fr_int   The interrupt which should be enabled. (A list of all
                            allowed values define in cy_fr_api_global.h at section
                            'CY_FR_INTERRUPT_TYPE' )

    \retval     CY_FR_OKAY               In case of success
    \retval     CY_FR_INVALID_INTERRUPT  If 'cy_fr_int' is an invalid interrupt
    \retval     CY_FR_ERROR              In case the interrupt is not enabled

*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_interrupt_enable ( volatile stc_FLEXRAY_t * base,
                                               CY_FR_INTERRUPT_TYPE     cy_fr_int )
{
  if ( cy_fr_int < 0x1A ){                                       /* for error interrupt */
    return cy_fr_ccal_error_interrupt_enable(base, cy_fr_int);   /* enable selected error interrupt and return */
  } else if ( ( cy_fr_int >= 0x40 ) && ( cy_fr_int < 0x5a ) ){   /* for status interrupt */
    return cy_fr_ccal_status_interrupt_enable(base, cy_fr_int);  /* enable selected status interrupt and return */
  } else {                                                       /* for invalid interrupt */
    return CY_FR_INVALID_INTERRUPT;                              /* return with error */
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function disables the interrupt 'cy_fr_int'. It verified if
                'cy_fr_int' is an error or a status interrupt and disables the selected
                interrupt.

    \param[in]  base        Pointer to FlexRay register base
    \param[in]  cy_fr_int   The interrupt which should be disabled. (A list of all
                            allowed values define in cy_fr_api_global.h at section
                            'CY_FR_INTERRUPT_TYPE' )

    \retval     CY_FR_OKAY               In case of success
    \retval     CY_FR_INVALID_INTERRUPT  If 'cy_fr_int' is an invalid interrupt
    \retval     CY_FR_ERROR              In case the interrupt is not disabled
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_interrupt_disable ( volatile stc_FLEXRAY_t * base,
                                               CY_FR_INTERRUPT_TYPE      cy_fr_int )
{
  if ( cy_fr_int < 0x1A ){                                       /* for error interrupt */
    return cy_fr_ccal_error_interrupt_disable(base, cy_fr_int);  /* disable selected error interrupt and return */
  } else if ( ( cy_fr_int >= 0x40 ) && ( cy_fr_int < 0x5a ) ){   /* for status interrupt */
    return cy_fr_ccal_status_interrupt_disable(base, cy_fr_int); /* disable selected status interrupt and return */
  } else {                                                       /* for invalid interrupt */
    return CY_FR_INVALID_INTERRUPT;                              /* return with error */
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function set the interrupt line for the interrupt cy_fr_int. It
                verified if 'cy_fr_int' is an error or a status interrupt and set the
                interrupt line for the selected interrupt to 'cy_fr_int_line'.

    \param[in]  base               Pointer to FlexRay register base
    \param[in]  cy_fr_int          The interrupt which line should be set. ( a list
                                   of all allowed values define in cy_fr_api_global.h
                                   at section 'CY_FR_INTERRUPT_TYPE' )

    \param[in]  cy_fr_int_line     The interrupt line which should assigned to 'cy_fr_int'
                                   (A list of all allowed values define in
                                   cy_fr_api_global.h at section 'CY_FR_INT_LINE_TYPE' )

    \retval     CY_FR_OKAY               In case of success
    \retval     CY_FR_INVALID_INTERRUPT  If 'cy_fr_int' is an invalid interrupt
    \retval     CY_FR_ERROR              In case the interrupt line is not set
    \retval     CY_FR_INVALID_INT_LINE   If cy_fr_int_line is  an invalid interrupt
                                         line
*/
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_interrupt_line_set ( volatile stc_FLEXRAY_t * base,
                                                 CY_FR_INTERRUPT_TYPE     cy_fr_int,
                                                 CY_FR_INT_LINE_TYPE      cy_fr_int_line )
{
  if ( cy_fr_int < 0x1A ){                                                        /* for error interrupt */
    return cy_fr_ccal_error_interrupt_line_set(base, cy_fr_int, cy_fr_int_line);  /* set selected interrupt line for the selected error interrupt and return */
  } else if ( ( cy_fr_int >= 0x40 ) && ( cy_fr_int < 0x5a ) ){                    /* for status interrupt */
    return cy_fr_ccal_status_interrupt_line_set(base, cy_fr_int, cy_fr_int_line); /* set selected interrupt line for the selected status interrupt and return */
  } else {                                                                        /* for invalid interrupt */
    return CY_FR_INVALID_INTERRUPT;                                               /* return with error */
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function clears the interrupt flag for interrupt 'cy_fr_int'. It
                verified if 'cy_fr_int' is an error or a status interrupt and clears the
                selected interrupt flag.

    \param[in]  base        Pointer to FlexRay register base
    \param[in]  cy_fr_int   The interrupt which flag should be cleared. (A list of all
                            allowed values define in cy_fr_api_global.h at section
                            CY_FR_INTERRUPT_TYPE )

    \retval     CY_FR_OKAY               In case of success
    \retval     CY_FR_INVALID_INTERRUPT  If cy_fr_int is an invalid interrupt
    \retval     CY_FR_ERROR              In case the interrupt flag is not cleared
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_clear_interrupt_flag ( volatile stc_FLEXRAY_t * base,
                                                   CY_FR_INTERRUPT_TYPE     cy_fr_int)
{
  if ( cy_fr_int < 0x1A ){                                          /* for error interrupt */
    return cy_fr_ccal_clear_error_interrupt_flag(base, cy_fr_int);  /* clear selected error interrupt flag and return */
  } else if ( ( cy_fr_int >= 0x40 ) && ( cy_fr_int < 0x5a ) ){      /* for status interrupt */
    return cy_fr_ccal_clear_status_interrupt_flag(base, cy_fr_int); /* clear selected status interrupt flag  and return */
  } else {                                                          /* for invalid interrupt */
    return CY_FR_INVALID_INTERRUPT;                                 /* return with error */
  }
}


/*---------------------------------------------------------------------------------*/
/** \brief      This Function enables the interrupt line 'cy_fr_int_line'.

    \param[in]  base               Pointer to FlexRay register base
    \param[in]  cy_fr_int_line     The interrupt line which should be enabled.
                                   (A list of all allowed values define in
                                   cy_fr_api_global.h at section 'CY_FR_INT_LINE_TYPE' )

    \retval     CY_FR_OKAY               In case of success
    \retval     CY_FR_ERROR              In case the interrupt line is not enabled
    \retval     CY_FR_INVALID_INT_LINE   If cy_fr_int_line is an invalid interrupt
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_interrupt_line_enable ( volatile stc_FLEXRAY_t * base,
                                                    CY_FR_INT_LINE_TYPE      cy_fr_int_line )
{
  return cy_fr_ccal_interrupt_line_enable(base, cy_fr_int_line); /* enable the selected interrupt line */
}


/*---------------------------------------------------------------------------------*/
/** \brief      This Function disables the interrupt line 'cy_fr_int_line'.

    \param[in]  base               Pointer to FlexRay register base
    \param[in]  cy_fr_int_line     The interrupt line which should be disabled.
                                   (A list of all allowed values define in
                                   cy_fr_api_global.h at section 'CY_FR_INT_LINE_TYPE')

    \retval     CY_FR_OKAY               In case of success
    \retval     CY_FR_ERROR              In case the interrupt line is not disabled
    \retval     CY_FR_INVALID_INT_LINE   If cy_fr_int_line is an invalid interrupt
 */
/*---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_interrupt_line_disable( volatile stc_FLEXRAY_t * base,
                                                    CY_FR_INT_LINE_TYPE      cy_fr_int_line )
{
  return cy_fr_ccal_interrupt_line_disable(base, cy_fr_int_line); /* disable the selected interrupt line */
}



/*---------------------------------------------------------------------------------*/
/** \brief      This function sets the user's interrupt callback functions for the
                given interrupt line
    
    \param[in]  base                  Pointer to FlexRay register base
    \param[in]  cy_fr_int_line        Interrupt line for the callback
    \param[in]  cy_fr_callback        User callback function
    
*/
/*---------------------------------------------------------------------------------*/
void cy_fr_api_interrupt_set_callback( volatile stc_FLEXRAY_t *                  base,
                                       CY_FR_INT_LINE_TYPE                       cy_fr_int_line,
                                       CY_FR_USER_IRQ_CALLBACK_FUNCTION_PTR_TYPE cy_fr_callback  )
{
  cy_fr_user_interrupt_callback[CY_FR_GET_INSTANCE_FROM_BASE(base)][cy_fr_int_line] = cy_fr_callback;
}


/*---------------------------------------------------------------------------------*/
/** \brief      This function is the interrupt dispatcher.
                User needs to provide the interrupt handler and integrate this
                dispatcher function and call it with correct parameters.
                It checks the interrupt flag, enable and line select register which
                interrupt  belong to the interrupt line and call the interrupt
                handler for the triggered interrupt, if more then one
                interrupt flag for the interrupt line are set, all interrupts are
                handled (first error  interrupts from LSB to MSB then Status
                Interrupt also from LSB to MSB). The interrupt flags belonging to this
                interrupt line are cleared.
    
    \param[in]  base                  Pointer to FlexRay register base
    \param[in]  cy_fr_int_line        Interrupt line whose IRQs shall be dispatched
*/
/*---------------------------------------------------------------------------------*/
void cy_fr_api_interrupt_dispatcher( volatile stc_FLEXRAY_t * base,
                                     CY_FR_INT_LINE_TYPE      cy_fr_int_line )
{
  uint32_t cy_fr_interrupt_flag_shadow_reg_eir;
  uint32_t cy_fr_interrupt_flag_shadow_reg_sir;
  CY_FR_INTERRUPT_TYPE cy_fr_current_int;

  cy_fr_interrupt_flag_shadow_reg_eir = cy_fr_ccal_get_error_interrupt_flags (base, cy_fr_int_line);
  cy_fr_interrupt_flag_shadow_reg_sir = cy_fr_ccal_get_status_interrupt_flags(base, cy_fr_int_line);

  /* Loops through all enabled and active interrupts for this interrupt line and calls the user callback (if set)
     once for each active interrupt. */
  while((cy_fr_interrupt_flag_shadow_reg_eir != 0) ||
        (cy_fr_interrupt_flag_shadow_reg_sir != 0)    ){
    cy_fr_current_int = cy_fr_ccal_interrupt_separator(&cy_fr_interrupt_flag_shadow_reg_eir,
                                                       &cy_fr_interrupt_flag_shadow_reg_sir );
        
    if( cy_fr_user_interrupt_callback[CY_FR_GET_INSTANCE_FROM_BASE(base)][cy_fr_int_line] != (void *) 0 ) {
      cy_fr_user_interrupt_callback[CY_FR_GET_INSTANCE_FROM_BASE(base)][cy_fr_int_line](base, cy_fr_current_int);
    }
  }
}


#if defined(__cplusplus)
}
#endif


/* [] END OF FILE */


