/***************************************************************************//**
* \file cy_fr_api_init.h
* \version 1.0
*
* \brief
* Prototypes for cy_fr_api_init.c
*
* This file contains the Prototypes for the initialization of the FlexRay 
* Communication Controller with a *.chi file.
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


#ifndef __CY_FR_API_INIT_H__
#define __CY_FR_API_INIT_H__


#include "flexray/fhal/cy_fr_fhal.h"

/*---------------------------------------------------------------------------------
 * Type Definitions
 *--------------------------------------------------------------------------------*/

 /**
* \addtogroup group_fru_functions
* \{
*/

/*--------------------------------------------------------------------------------*/
/** \brief      This function pointer shall be used for the user function that 
                includes the CHI file

                The same argument name ("base") must be used in the user's function!
 */
/*---------------------------------------------------------------------------------*/
typedef void (*CY_FR_CHI_INCLUDE_FUNCTION_PTR_TYPE)(volatile stc_FLEXRAY_t * base);

/** \} group_fru_functions */


/*----------------------------------------------------------------------------------
 * Macros
 *---------------------------------------------------------------------------------*/

 /**
* \addtogroup group_fru_macros
* \{
*/

/*---------------------------------------------------------------------------------*/
/** \fn         WRITE32(val,adr)

    \brief      This Macro writes the value 'val' to the address 'adr'
                'base' is function parameter of the function where this macro will be executed

    \param[in]  val value which should be copied
    \param[in]  adr the address where \a val should be copied
 */
/*---------------------------------------------------------------------------------*/
#define WRITE32(val,adr) \
\
  *((volatile uint32_t*)((uint32_t) base + adr)) = val

  
/*---------------------------------------------------------------------------------*/
/** \fn         SET_CONFIGMODE(void)

    \brief      This Macro waits till the value at address 'adr' and 'mask' equal 0
                'base' is function parameter of the function where this macro will be executed

    \param[in]  mask   masking the addressed register
    \param[in]  adr    addressing the register
 */
/*---------------------------------------------------------------------------------*/
#define WAIT_TILL_CLEARED32(mask,adr) \
\
  while(((*((volatile uint32_t*)((uint32_t) base + adr))) & mask) != 0)

    
// Following functions/macros occur in the CHI file, but are not needed by this driver.
// Therefore, the macros are defined "empty"

#define SET_CONFIGMODE()
#define CONFIG_COMPLETE()
#define CHECK_CTRL_TYPE(ctrl_type)

/** \} group_fru_macros */

/**
* \addtogroup group_fru_functions
* \{
*/

/*---------------------------------------------------------------------------------
 * prototypes
 ---------------------------------------------------------------------------------*/
CY_FR_RETURN_TYPE cy_fr_api_init_chi ( volatile stc_FLEXRAY_t *            base,
                                       CY_FR_CHI_INCLUDE_FUNCTION_PTR_TYPE cy_fr_chi_inc_func_ptr );
                                           
/** \} group_fru_functions */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_API_INIT_H__ */
/*---------------------------------------------------------------------------------*/


#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


