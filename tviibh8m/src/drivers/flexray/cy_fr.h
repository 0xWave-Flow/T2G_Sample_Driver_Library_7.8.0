/***************************************************************************//**
* \file cy_fr.h
* \version 1.0
*
* \brief
* Include all required API header files of the FlexRay driver
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

/**
* \defgroup group_fru FlexRay Unit (FRU)
* \{
* This driver provides global FRU defines and API function definitions.
*
* \defgroup group_fru_macros Macro
* \defgroup group_fru_functions Functions
* \defgroup group_fru_data_structures Data structures
* \defgroup group_fru_enums Enumerated types
*/

#if !defined(CY_FR_H)
#define CY_FR_H

#include "cy_device_headers.h"

#include "flexray/api/cy_fr_api_global.h"
#include "flexray/api/cy_fr_api_control_service.h"
#include "flexray/api/cy_fr_api_init.h"
#include "flexray/api/cy_fr_api_interrupt_service.h"
#include "flexray/api/cy_fr_api_rx_handler.h"
#include "flexray/api/cy_fr_api_status_service.h"
#include "flexray/api/cy_fr_api_time_service.h"
#include "flexray/api/cy_fr_api_timer_service.h"
#include "flexray/api/cy_fr_api_tx_handler.h"

#include "flexray/fhal/cy_fr_fhal.h"

#endif /* CY_FR_H */

/* [] END OF FILE */
