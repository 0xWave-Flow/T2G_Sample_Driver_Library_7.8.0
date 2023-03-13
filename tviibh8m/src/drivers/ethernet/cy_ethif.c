/***************************************************************************//**
* \file cy_ethif.c
* \version 1.0
*
* \brief
* Provides an API implementation of the Ethernet Interface driver
*
********************************************************************************
* \copyright
* Copyright 2016-2022, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cy_ethif.h"
#include "cy_project.h"
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
*       Local Variables
***************************************/


/*****************************************************************************
* Local function prototypes ('static')
*****************************************************************************/


/*****************************************************************************
* Local Call back function supplied to Cadence driver
*****************************************************************************/



/*******************************************************************************
* Function Name: Cy_EthIf_Init
****************************************************************************//**
*
* \brief Initializes the Ethernet MAC, Cadence Driver, EthIf and PHY 
*
* \param pstcEth
* Pointer to register area of Ethernet MAC
*
* \param pstcEthIfConfig
* Pointer to Ethernet configuration parameters
*
* \param pstcInterruptList
* List of Interrupts to enable
* 
* \return CY_ETHIF_SUCCESS      Ethernet MAC has been initialized along with Cadence driver and external PHY
* 
* \ Note:
* This function Initializes the Ethernet MAC, Cadence driver, EthIf layer and external PHY with the provided parameters
* Port init for the Ethernet must be done before calling Cy_EthIf_Init function
* Buffer configuration parameters shall be done in EthIf.h file
*
*******************************************************************************/
cy_en_ethif_status_t Cy_EthIf_Init(cy_pstc_eth_type_t pstcEth, cy_stc_ethif_configuration_t * pstcEthIfConfig, cy_stc_ethif_interruptconfig_t * pstcInterruptList)
{
    return CY_ETHIF_SUCCESS;
}


/*******************************************************************************
* Function Name: Cy_EthIf_PhyRegRead
****************************************************************************//**
*
* \brief Local function used by other APIs to read the PHY register
*
* \param pstcEth Ethernet instance
* \param u8RegNo Register to read
* \return read data from the register
*
* \Note: CY_ETH_PHY_ADDR must match with PHY. PHY data-sheet and hardware schematic shall be checked 
*
*******************************************************************************/
uint32_t Cy_EthIf_PhyRegRead(cy_pstc_eth_type_t pstcEth, uint8_t u8RegNo, uint8_t u8PHYAddr)
{
    uint16_t    u16ReadData = 0;

    return (uint32_t)u16ReadData;
}

/*******************************************************************************
* Function Name: Cy_EthIf_PhyRegWrite
****************************************************************************//**
*
* \brief Local function used by other APIs to write the PHY register
*
* \param pstcEth Ethernet instance
* \param u8RegNo Register to write
* \param u16Data data to write
* \return 
*
* \Note: u8PHYAddr must match with PHY. PHY data-sheet and hardware schematic shall be checked 
*
*******************************************************************************/
cy_en_ethif_status_t Cy_EthIf_PhyRegWrite(cy_pstc_eth_type_t pstcEth, uint8_t u8RegNo, uint16_t u16Data, uint8_t u8PHYAddr)
{
    return CY_ETHIF_SUCCESS;
}


#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */
