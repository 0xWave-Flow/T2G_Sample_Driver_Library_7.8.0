/***************************************************************************//**
* \file cy_fr_fhal.h
* \version 1.0
*
* \brief
* This file contains Macros for the FlexRay Hardware Layer
* 
* In this file all macros are defined to read from or write to the FlexRay Communication
* Controller. The base address of the FlexRay Communication Controller is added.
* Some other functions are added as well.
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


#ifndef __CY_FR_FHAL_H__
#define __CY_FR_FHAL_H__


#include "flexray/fhal/cy_fr_fhal_register.h"

/**
* \addtogroup group_fru_macros
* \{
*/

/*---------------------------------------------------------------------------------*/
/** \fn         cy_fr_fhal_read32 ( adr )

    \brief      This macro add 'CY_FR_FRCC_OFFSET' defined in cy_fr_api_global_def.h to 
                the parameter 'adr' and returns the read value.

    \param[in]  adr 16 bit address of the register should read
 */
/*---------------------------------------------------------------------------------*/
#define cy_fr_fhal_read32(adr) \
\
  *((volatile uint32_t *)((adr) + (uint32_t)base))


/*---------------------------------------------------------------------------------*/
/** \fn         #define cy_fr_fhal_write32 ( val, adr )

    \brief      This macro add the address offset 'CY_FR_FRCC_OFFSET' defined in 
                cy_fr_api_global_def.h and writes the value 'val' to the address 'adr'

    \param[in]  val  Value to write.
    \param[in]  adr  Address.
 */
/*---------------------------------------------------------------------------------*/
#define cy_fr_fhal_write32(val,adr) \
\
  *((volatile uint32_t *)((adr) + (uint32_t)base)) = val


/*---------------------------------------------------------------------------------*/
/** \fn         cy_fr_fhal_wait_till_cleared32(mask,adr)

    \brief      Add 'CY_FR_FRCC_OFFSET' defined in cy_fr_fapi_global_def.h to 'adr' and
                wait until the register value and mask equal 0.

    \param[in]  mask     Value to mask the register pointed by 'adr'.
    \param[in]  adr      The register address.
 */
/*---------------------------------------------------------------------------------*/
#define cy_fr_fhal_wait_till_cleared32(mask,adr) \
\
  while(((*((volatile uint32_t *)((adr) + (uint32_t)base))) & mask) != 0)
  

/*---------------------------------------------------------------------------------*/
/** \fn         cy_fr_fhal_switch(in)

    \brief      Change byte order (1234) --> (4321).

    \param[in]  in 32 bit value.
 */
/*---------------------------------------------------------------------------------*/
#define cy_fr_fhal_switch(in) \
  ((( in & 0xFF000000L ) >> 24 ) + \
   (( in & 0x00FF0000L ) >>  8 ) + \
   (( in & 0x0000FF00L ) <<  8 ) + \
   (( in & 0x000000FFL ) << 24 ))

/** \} group_fru_macros */
  
  
  
/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_FHAL_H__ */
/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


