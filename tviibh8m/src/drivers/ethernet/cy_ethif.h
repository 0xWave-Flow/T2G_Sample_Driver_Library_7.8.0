/***************************************************************************//**
* \file cy_ethif.h
* \version 1.0
*
* \brief
* Provides an API declaration of the Ethernet Interface [Cy_EthIf] driver
*
********************************************************************************
* \copyright
* Copyright 2016-2022, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef CY_ETHIF_H
#define CY_ETHIF_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "cy_device_headers.h"

/** \addtogroup group_ethif_macros
* \{
*/

/*  Device Depending Parameters End */

/** Driver major version */
#define CY_ETHIF_DRV_VERSION_MAJOR       1

/** Driver minor version */
#define CY_ETHIF_DRV_VERSION_MINOR       0

/** EthIf driver ID */
#define CY_ETHIF_ID                     CY_PDL_DRV_ID(0xXXu)    // TODO: CY_PDL_DRV_ID for EtherIf


/***************************************
*        Constants
***************************************/

#define CY_ETHIF_NO_BUFFER_AVAILABLE (0xFFFFFFFF)
#define CY_ETHIF_BUFFER_AVAILABLE    (0xFFFFFFFE)


/** \} group_ethif_macros */

/***************************************
*        Enumerations
***************************************/
/**
* \addtogroup group_ethif_enums
* \{
*/

/**
 *****************************************************************************
 ** \brief EthIf driver error codes.
 ** 
 *****************************************************************************/
typedef enum
{
    CY_ETHIF_SUCCESS = 0,               /** Returned successful */
    CY_ETHIF_BAD_PARAM,                 /** Bad parameter was passed */
    CY_ETHIF_MEMORY_NOT_ENOUGH,         /** Assigned memory for BDs not enough */
    CY_ETHIF_LINK_DOWN,                 /** Link between nodes is not up */
    CY_ETHIF_LINK_UP,                   /** Link between nodes is up */
    CY_ETHIF_BUFFER_NOT_AVAILABLE,      /** No local buffer available to send the frame */
} cy_en_ethif_status_t;

/**
 *****************************************************************************
 ** \brief EthIf driver error codes.
 ** 
 *****************************************************************************/
typedef enum 
{
    CY_ETHIF_BUFFER_STATUS
} cy_en_ethif_buffer_status;

/**
 *****************************************************************************
 ** \brief EMAC Drive Modes.
 ** 
 *****************************************************************************/
typedef enum 
{
    CY_ETHIF_IFC_SPEED_SEL
} cy_en_ethif_Interface_Speed_Sel;

/**
 *****************************************************************************
 ** \brief Reference Clock for RMII/RGMII.
 ** 
 *****************************************************************************/
typedef enum 
{
    CY_ETHIF_EXTERNAL_HSIO = 0,         /** Clock coming from HSIO */
    CY_ETHIF_INTERNAL_PLL,              /** Clock referenced from Internal PLL source */
} cy_en_ethif_clock_ref;

typedef enum
{
    CY_ETHIF_PTP_EVENT_PRIMARY = 0, /* event for Sync and Delay_Req */
    CY_ETHIF_PTP_EVENT_PEER    = 1, /* event for Pdelay_Req and Pdelay_Resp */
} cy_en_ethif_ptp_event_type_t;

/** \} group_ethif_enums */

/***************************************
*        Configuration Structures
***************************************/
/**
* \addtogroup group_ethif_data_structures
* \{
*/

/**
 *****************************************************************************
 ** \brief Ethernet MAC Unit Type
 *****************************************************************************/
typedef  volatile stc_ETH_t*  cy_pstc_eth_type_t;

/**
 *****************************************************************************
 ** \brief Wrapper Configuration 
 *****************************************************************************/
typedef struct
{
    uint8_t wrapper_cfg;
} cy_str_ethif_wrapper_config;

/**
 *****************************************************************************
 ** \brief Queue status
 *****************************************************************************/
typedef struct
{
    uint8_t queue_disable_status;
} cy_str_ethif_queueDisablestatus;

/**
 *****************************************************************************
 ** \brief Buffer status
 *****************************************************************************/
typedef struct
{
    cy_en_ethif_buffer_status   enBufStatus;            /** Buffer status */
} cy_str_ethif_bufstatus;

/**
 *****************************************************************************
 ** \brief Time Stamp Unit Init
 *****************************************************************************/
typedef struct
{
    uint8_t tsu_cfg;
} cy_str_ethif_TSU_Config;

 /**
 *****************************************************************************
 ** \brief Ethernet MAC call back handlers
 *****************************************************************************/
 typedef struct
 {
    uint8_t call_back;
 } cy_ethif_callbackfunctions;
 
/**
 *****************************************************************************
 ** \brief Ethernet MAC detailed configurations 
 *****************************************************************************/
typedef struct
{
    bool bintrEnable;                              /** interrupts/events to enable on start   */
} cy_stc_ethif_configuration_t;

/**
 ******************************************************************************
 ** \brief Interrupt configuration for Ethernet MAC
 ******************************************************************************/
typedef struct
{
    uint8_t interrupt_cfg;
} cy_stc_ethif_interruptconfig_t;

/** \} group_ethif_data_structures */

/**
* \addtogroup group_ethif_enums
* \{
*/

/**
 ******************************************************************************
 ** \brief filter configuration for Ethernet MAC
 ******************************************************************************/
typedef enum
{
    CY_ETH_FILTER_TYPE_DESTINATION = 0,
    CY_ETH_FILTER_TYPE_SOURCE      = 1,
} cy_en_eth_filter_type_t;

typedef enum
{
    CY_ETH_FILTER_NUM_INV,
} cy_en_eth_filter_num_t;

/** \} group_ethif_enums */

/**
* \addtogroup group_ethif_data_structures
* \{
*/

typedef struct
{
    cy_en_eth_filter_type_t typeFilter;
} cy_stc_eth_filter_config_t;

/** \} group_ethif_data_structures */

/***************************************
*        Function Prototypes
***************************************/
/**
* \addtogroup group_ethif_functions
* \{
*/

cy_en_ethif_status_t Cy_EthIf_Init(cy_pstc_eth_type_t pstcEth, cy_stc_ethif_configuration_t * pstcEthIfConfig, cy_stc_ethif_interruptconfig_t * pstcInterruptList);
uint32_t Cy_EthIf_PhyRegRead(cy_pstc_eth_type_t pstcEth, uint8_t u8RegNo, uint8_t u8PHYAddr);
cy_en_ethif_status_t Cy_EthIf_PhyRegWrite(cy_pstc_eth_type_t pstcEth, uint8_t u8RegNo, uint16_t u16Data, uint8_t u8PHYAddr);

/** \} group_ethif_functions */

#if defined(__cplusplus)
}
#endif

#endif /* CY_ETHIF_H */

/** \} cy_ethif */

/* [] END OF FILE */
