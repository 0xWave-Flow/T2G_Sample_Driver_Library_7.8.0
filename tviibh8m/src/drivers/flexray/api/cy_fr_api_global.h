/***************************************************************************//**
* \file cy_fr_api_global.h
* \version 1.0
*
* \brief
* Header file with Type definitions
*
* This file contains the data type and structure definitions for all types used in 
* the Cypress FlexRay Driver. For most types, the values have special accounts, 
* only these values are allowed to use.
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


#ifndef __CY_FR_API_GLOBAL_H__
#define __CY_FR_API_GLOBAL_H__

#include "cy_device_headers.h"
#include <stdint.h>
#include <stdbool.h>

/**
* \addtogroup group_fru_macros
* \{
*/

/*---------------------------------------------------------------------------------
 * Defines/Macros
 *--------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
/** \brief   Maximum message buffers of the FleyRay IP
 */
/*---------------------------------------------------------------------------------*/
#define CY_FR_MAX_BUFFERS    128

/*---------------------------------------------------------------------------------*/
/** \brief   Reserved address space for one FlexRay IP
 */
/*---------------------------------------------------------------------------------*/
#define CY_FR_MXFLEXRAY_IP_SIZE    0x1000UL

/*---------------------------------------------------------------------------------*/
/** \brief    Returns the FlexRay IP instance number derived from the register base address
 */
/*---------------------------------------------------------------------------------*/
#define CY_FR_GET_INSTANCE_FROM_BASE(base)   ((((uint32_t)base) - FLEXRAY0_BASE) / CY_FR_MXFLEXRAY_IP_SIZE)

/** \} group_fru_macros */

/**
* \addtogroup group_fru_data_structures
* \{
*/

/*---------------------------------------------------------------------------------
 * Type Definitions
 *--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*/
/** \brief      This type is for the Frame ID

    \par Allowed Values

    All value between 0 and 128.
 */
/*---------------------------------------------------------------------------------*/
typedef uint8_t CY_FR_BUFFER_TYPE;


/*--------------------------------------------------------------------------------*/
/** \brief      This type is for the Frame ID

    \par Allowed Values
    
                All value greater then 0 and lower then 2048.
 */
/*---------------------------------------------------------------------------------*/
typedef uint16_t CY_FR_FRAME_ID_TYPE;

/** \} group_fru_data_structures */

/**
* \addtogroup group_fru_enums
* \{
*/

/*---------------------------------------------------------------------------------*/
/** \brief      This Type is used as Channel selection for all functions has a 
                parameter 'Channel'

    \par Allowed Values

    \arg \b     CY_FR_NO_CHANNEL     \n \b Value: 0x00 \b Description: No Channel selected                 
    \arg \b     CY_FR_CHANNEL_A      \n \b Value: 0x01 \b Description: Only Channel A is selected          
    \arg \b     CY_FR_CHANNEL_B      \n \b Value: 0x02 \b Description: Only Channel B is selected          
    \arg \b     CY_FR_CHANNEL_A_B    \n \b Value: 0x03 \b Description: Channel A and Channel B selected    
 */
/*---------------------------------------------------------------------------------*/
typedef enum { 
  CY_FR_NO_CHANNEL  = 0x00,
  CY_FR_CHANNEL_A   = 0x01,
  CY_FR_CHANNEL_B   = 0x02,
  CY_FR_CHANNEL_A_B = 0x03,
} CY_FR_CHANNEL_TYPE;
  
  
/*---------------------------------------------------------------------------------*/
/** \brief      This Type is for all functions with out an own return type.
    
    \par Allowed Values

    \arg \b     CY_FR_FALSE                   \n \b Value: 0x00 \b Description: The answer is false                                  
    \arg \b     CY_FR_TRUE                    \n \b Value: 0x01 \b Description: The answer is true                                   
    \arg \b     CY_FR_OKAY                    \n \b Value: 0x10 \b Description: Every thing okay or no error checking                
    \arg \b     CY_FR_ERROR                   \n \b Value: 0x11 \b Description: An error detected                                    
    \arg \b     CY_FR_COMMAND_NOT_ACCEPTED    \n \b Value: 0x20 \b Description: The POC command was not accepted                     
    \arg \b     CY_FR_ACCESS_DENIED           \n \b Value: 0x21 \b Description: The access to buffer denied                          
    \arg \b     CY_FR_NO_DATA                 \n \b Value: 0x22 \b Description:                                                      
    \arg \b     CY_FR_FIFO_EMPTY              \n \b Value: 0x23 \b Description:                                                      
    \arg \b     CY_FR_NOT_ACTIVE              \n \b Value: 0x30 \b Description: POC state is not NORMAL_ACTIVE                       
    \arg \b     CY_FR_NOT_PASSIVE             \n \b Value: 0x31 \b Description: POC state is not NORMAL_PASSIVE                      
    \arg \b     CY_FR_NOT_CONFIG              \n \b Value: 0x32 \b Description: POC state is not CONFIG                              
    \arg \b     CY_FR_NOT_HALT                \n \b Value: 0x33 \b Description: POC state is not HALT                                
    \arg \b     CY_FR_FAULTY_BUFFER           \n \b Value: 0x40 \b Description: A faulty buffer was detected                         
    \arg \b     CY_FR_MULTIPLE_FAULTY_BUFFER  \n \b Value: 0x41 \b Description: More than one buffer is faulty                       
    \arg \b     CY_FR_INVALID_BUFFER          \n \b Value: 0x80 \b Description: The Buffer does not exist                            
    \arg \b     CY_FR_INVALID_CHANNEL         \n \b Value: 0x81 \b Description: Not allowed channel                                  
    \arg \b     CY_FR_INVALID_CYCLE           \n \b Value: 0x82 \b Description: Not allowed cycle                                    
    \arg \b     CY_FR_INVALID_DATA            \n \b Value: 0x84 \b Description:                                                      
    \arg \b     CY_FR_INVALID_INT_LINE        \n \b Value: 0x85 \b Description: Not allowed Interrupt line (see CY_FR_INT_LINE_TYPE)  
    \arg \b     CY_FR_INVALID_INTERRUPT       \n \b Value: 0x86 \b Description: Not allowed interrupt (see CY_FR_INTERRUPT_TYPE )     
    \arg \b     CY_FR_INVALID_TIMER           \n \b Value: 0x87 \b Description: Not allowed Timer (see CY_FR_TIMER_TYPE)              
 */
/*---------------------------------------------------------------------------------*/
typedef enum {
  CY_FR_FALSE                    = 0x00,
  CY_FR_TRUE                     = 0x01,
  CY_FR_OKAY                     = 0x10,
  CY_FR_ERROR                    = 0x11,
  CY_FR_COMMAND_NOT_ACCEPTED     = 0x20,
  CY_FR_ACCESS_DENIED            = 0x21,
  CY_FR_NO_DATA                  = 0x22,
  CY_FR_FIFO_EMPTY               = 0x23,
  CY_FR_NOT_ACTIVE               = 0x30,
  CY_FR_NOT_PASSIVE              = 0x31,
  CY_FR_NOT_CONFIG               = 0x32,
  CY_FR_NOT_HALT                 = 0x33,
  CY_FR_FAULTY_BUFFER            = 0x40,  
  CY_FR_MULTIPLE_FAULTY_BUFFER   = 0x41,  
  CY_FR_INVALID_BUFFER           = 0x80,    /* blocked value (must not be less than 128!) */
  CY_FR_INVALID_CHANNEL          = 0x81,    /* blocked value (must not be less than 16!)*/
  CY_FR_INVALID_CYCLE            = 0x82,    /* blocked value (must not be less than 16!)*/
  CY_FR_INVALID_DATA             = 0x83,
  CY_FR_INVALID_INT_LINE         = 0x84,
  CY_FR_INVALID_INTERRUPT        = 0x85,
  CY_FR_INVALID_TIMER            = 0x86,  
} CY_FR_RETURN_TYPE;


/*---------------------------------------------------------------------------------*/
/** \brief      This Type is used as return type for the function that checks
                whether tx is ready.

    \par Allowed Values

    \arg \b     CY_FR_TX_NOT_READY                     \n \b Value: 0x00 \b Description: Transmission request not processed
    \arg \b     CY_FR_TX_READY                         \n \b Value: 0x01 \b Description: Transmission request processed
    \arg \b     CY_FR_TX_STATUS_UNKNOWN_BUFFER_INVALID \n \b Value: 0xff \b Description: Buffer for ID not found
    
 */
/*---------------------------------------------------------------------------------*/
typedef enum { 
  CY_FR_TX_NOT_READY                      = 0x00,
  CY_FR_TX_READY                          = 0x01,
  CY_FR_TX_STATUS_UNKNOWN_BUFFER_INVALID  = 0xff,
} CY_FR_TX_STATUS_TYPE;


/*---------------------------------------------------------------------------------*/
/** \brief      This Type is used as return type for the functions that checks if new
                data was received. It is also used as mode for the receive handler
                functions.

    \par Allowed Values

    \arg \b     CY_FR_RX_NO_NEW_FRAME                  \n \b Value: 0x00 \b Description: No new Frame received
    \arg \b     CY_FR_RX_NEW_DATA                      \n \b Value: 0x01 \b Description: New valid frame receive
    \arg \b     CY_FR_RX_STATUS_CHANGE                 \n \b Value: 0x02 \b Description: Faulty frame receive
    \arg \b     CY_FR_RX_DATA_ONLY                     \n \b Value: 0x03 \b Description: Request data only
    \arg \b     CY_FR_RX_STATUS_UNKNOWN_BUFFER_INVALID \n \b Value: 0xff \b Description: Buffer for ID not found
    
 */
/*---------------------------------------------------------------------------------*/
typedef enum { 
  CY_FR_RX_NO_NEW_FRAME                   = 0x00,
  CY_FR_RX_NEW_DATA                       = 0x01,
  CY_FR_RX_STATUS_CHANGE                  = 0x02,
  CY_FR_RX_DATA_ONLY                      = 0x03,
  CY_FR_RX_STATUS_UNKNOWN_BUFFER_INVALID  = 0xff,
} CY_FR_RX_STATUS_MODE_TYPE;



/*---------------------------------------------------------------------------------*/
/** \brief      This type is used to specify the FR interrupt line

    \par Allowed Values

    \arg \b     CY_FR_INT_LINE_0    \n \b Value: 0x00 \b Description: Interrupt line 0
    \arg \b     CY_FR_INT_LINE_1    \n \b Value: 0x01 \b Description: Interrupt line 1
 */
/*---------------------------------------------------------------------------------*/
typedef enum {
  CY_FR_INT_LINE_0  = 0x00,
  CY_FR_INT_LINE_1  = 0x01,
} CY_FR_INT_LINE_TYPE;


/*---------------------------------------------------------------------------------*/
/** \brief    Error and Status interrupt flags

    \par Allowed Values

    \par        Error Interrupts

    \arg \b     CY_FR_EIR_POC_ERROR_MODE_CHANGED                        \n \b Value: 0x00 \b Description:
    \arg \b     CY_FR_EIR_COMMAND_NOT_ACCEPTED                          \n \b Value: 0x01 \b Description:
    \arg \b     CY_FR_EIR_SYNC_FRAME_BELOW_MINIMUM                      \n \b Value: 0x02 \b Description:
    \arg \b     CY_FR_EIR_SYNCFRAME_OVERFLOW                            \n \b Value: 0x03 \b Description:
    \arg \b     CY_FR_EIR_CLOCK_CORRECTION_FAILURE                      \n \b Value: 0x04 \b Description:
    \arg \b     CY_FR_EIR_CHI_COMMAND_LOCKED                            \n \b Value: 0x05 \b Description:
    \arg \b     CY_FR_EIR_PARITY_ERROR                                  \n \b Value: 0x06 \b Description:
    \arg \b     CY_FR_EIR_RECEIVED_FIFO_OVERRUN                         \n \b Value: 0x07 \b Description:
    \arg \b     CY_FR_EIR_EMPTY_FIFO_ACCESS                             \n \b Value: 0x08 \b Description:
    \arg \b     CY_FR_EIR_ILLEGAL_INPUT_BUFFER_ACCESS                   \n \b Value: 0x09 \b Description:
    \arg \b     CY_FR_EIR_ILLEGAL_OUTPUT_BUFFER_ACCESS                  \n \b Value: 0x0A \b Description:
    \arg \b     CY_FR_EIR_MESSAGE_HANDLER_CONSTRAINTS_FLAG              \n \b Value: 0x0B \b Description:
    \arg \b     CY_FR_EIR_ERROR_DETECTED_CHANNEL_A                      \n \b Value: 0x10 \b Description:
    \arg \b     CY_FR_EIR_LATEST_TRANSMIT_VIOLATION_DETECTED_CHANNEL_A  \n \b Value: 0x11 \b Description:
    \arg \b     CY_FR_EIR_TRANSMISSION_ACROSS_BOUNDARY_CHANNEL_A        \n \b Value: 0x12 \b Description:
    \arg \b     CY_FR_EIR_ERROR_DETECTED_CHANNEL_B                      \n \b Value: 0x18 \b Description:
    \arg \b     CY_FR_EIR_LATEST_TRANSMIT_VIOLATION_DETECTED_CHANNEL_B  \n \b Value: 0x19 \b Description:
    \arg \b     CY_FR_EIR_TRANSMISSION_ACROSS_BOUNDARY_CHANNEL_B        \n \b Value: 0x1A \b Description:

    
    \par        Status Interrupts

    \arg \b     CY_FR_SIR_WAKEUP_STATUS                                 \n \b Value: 0x40 \b Description:
    \arg \b     CY_FR_SIR_COLLISION_AVOIDANCE_SYMBOL                    \n \b Value: 0x41 \b Description:
    \arg \b     CY_FR_SIR_CYCLE_STARTUP_INTERRUPT                       \n \b Value: 0x42 \b Description:
    \arg \b     CY_FR_SIR_TRANSMIT_INTERRUPT                            \n \b Value: 0x43 \b Description:
    \arg \b     CY_FR_SIR_RECEIVE_INTERRUPT                             \n \b Value: 0x44 \b Description:
    \arg \b     CY_FR_SIR_RECEIVE_FIFO_NOT_EMPTY                        \n \b Value: 0x45 \b Description:
    \arg \b     CY_FR_SIR_RECEIVE_FIFO_CRITICAL_LEVEL                   \n \b Value: 0x46 \b Description:
    \arg \b     CY_FR_SIR_NETWORK_MANAGEMENT_VECTOR_CHANGED             \n \b Value: 0x47 \b Description:
    \arg \b     CY_FR_SIR_TIMER_INTERRUPT_0                             \n \b Value: 0x48 \b Description:
    \arg \b     CY_FR_SIR_TIMER_INTERRUPT_1                             \n \b Value: 0x49 \b Description:
    \arg \b     CY_FR_SIR_TRANSFER_INPUT_BUFFER_COMPLETED               \n \b Value: 0x4A \b Description:
    \arg \b     CY_FR_SIR_TRANSFER_OUTPUT_BUFFER_COMPLETED              \n \b Value: 0x4B \b Description:
    \arg \b     CY_FR_SIR_STOP_WATCH_EVENT                              \n \b Value: 0x4C \b Description:
    \arg \b     CY_FR_SIR_STARTUP_COMPLETED                             \n \b Value: 0x4D \b Description:
    \arg \b     CY_FR_SIR_MESSAGE_BUFFER_STATUS_INTERRUPT               \n \b Value: 0x4E \b Description:
    \arg \b     CY_FR_SIR_START_OF_DYNAMIC_SEGMENT                      \n \b Value: 0x4F \b Description:
    \arg \b     CY_FR_SIR_WAKEUP_PATTERN_CHANNEL_A                      \n \b Value: 0x50 \b Description:
    \arg \b     CY_FR_SIR_MTS_RECEIVED_ON_CHANNEL_A                     \n \b Value: 0x51 \b Description:
    \arg \b     CY_FR_SIR_WAKEUP_PATTERN_CHANNEL_B                      \n \b Value: 0x58 \b Description:
    \arg \b     CY_FR_SIR_MTS_RECEIVED_ON_CHANNEL_B                     \n \b Value: 0x59 \b Description:

 */
/*---------------------------------------------------------------------------------*/
typedef enum {
  CY_FR_EIR_POC_ERROR_MODE_CHANGED                         = 0x00,
  CY_FR_EIR_COMMAND_NOT_ACCEPTED                           = 0x01,
  CY_FR_EIR_SYNC_FRAME_BELOW_MINIMUM                       = 0x02,
  CY_FR_EIR_SYNCFRAME_OVERFLOW                             = 0x03,
  CY_FR_EIR_CLOCK_CORRECTION_FAILURE                       = 0x04,
  CY_FR_EIR_CHI_COMMAND_LOCKED                             = 0x05,
  CY_FR_EIR_PARITY_ERROR                                   = 0x06,
  CY_FR_EIR_RECEIVED_FIFO_OVERRUN                          = 0x07,
  CY_FR_EIR_EMPTY_FIFO_ACCESS                              = 0x08,
  CY_FR_EIR_ILLEGAL_INPUT_BUFFER_ACCESS                    = 0x09,
  CY_FR_EIR_ILLEGAL_OUTPUT_BUFFER_ACCESS                   = 0x0A,
  CY_FR_EIR_MESSAGE_HANDLER_CONSTRAINTS_FLAG               = 0x0B,
  CY_FR_EIR_ERROR_DETECTED_CHANNEL_A                       = 0x10,
  CY_FR_EIR_LATEST_TRANSMIT_VIOLATION_DETECTED_CHANNEL_A   = 0x11,
  CY_FR_EIR_TRANSMISSION_ACROSS_BOUNDARY_CHANNEL_A         = 0x12,
  CY_FR_EIR_ERROR_DETECTED_CHANNEL_B                       = 0x18,
  CY_FR_EIR_LATEST_TRANSMIT_VIOLATION_DETECTED_CHANNEL_B   = 0x19,
  CY_FR_EIR_TRANSMISSION_ACROSS_BOUNDARY_CHANNEL_B         = 0x1A,

  CY_FR_SIR_WAKEUP_STATUS                                  = 0x40,
  CY_FR_SIR_COLLISION_AVOIDANCE_SYMBOL                     = 0x41,
  CY_FR_SIR_CYCLE_STARTUP_INTERRUPT                        = 0x42,
  CY_FR_SIR_TRANSMIT_INTERRUPT                             = 0x43,
  CY_FR_SIR_RECEIVE_INTERRUPT                              = 0x44,
  CY_FR_SIR_RECEIVE_FIFO_NOT_EMPTY                         = 0x45,
  CY_FR_SIR_RECEIVE_FIFO_CRITICAL_LEVEL                    = 0x46,
  CY_FR_SIR_NETWORK_MANAGEMENT_VECTOR_CHANGED              = 0x47,
  CY_FR_SIR_TIMER_INTERRUPT_0                              = 0x48,
  CY_FR_SIR_TIMER_INTERRUPT_1                              = 0x49,
  CY_FR_SIR_TRANSFER_INPUT_BUFFER_COMPLETED                = 0x4A,
  CY_FR_SIR_TRANSFER_OUTPUT_BUFFER_COMPLETED               = 0x4B,
  CY_FR_SIR_STOP_WATCH_EVENT                               = 0x4C,
  CY_FR_SIR_STARTUP_COMPLETED                              = 0x4D,
  CY_FR_SIR_MESSAGE_BUFFER_STATUS_INTERRUPT                = 0x4E,
  CY_FR_SIR_START_OF_DYNAMIC_SEGMENT                       = 0x4F,
  CY_FR_SIR_WAKEUP_PATTERN_CHANNEL_A                       = 0x50,
  CY_FR_SIR_MTS_RECEIVED_ON_CHANNEL_A                      = 0x51,
  CY_FR_SIR_WAKEUP_PATTERN_CHANNEL_B                       = 0x58,
  CY_FR_SIR_MTS_RECEIVED_ON_CHANNEL_B                      = 0x59,  
} CY_FR_INTERRUPT_TYPE;  


/*---------------------------------------------------------------------------------*/
/** \brief    Protocol Operation Control Status

    \par Allowed Values

    \arg \b     CY_FR_POCS_DEFAULT_CONFIG                    \n \b Value: 0x00 \b Description: 
    \arg \b     CY_FR_POCS_READY                             \n \b Value: 0x01 \b Description: 
    \arg \b     CY_FR_POCS_NORMAL_ACTIVE                     \n \b Value: 0x02 \b Description: 
    \arg \b     CY_FR_POCS_NORMAL_PASSIVE                    \n \b Value: 0x03 \b Description: 
    \arg \b     CY_FR_POCS_HALT                              \n \b Value: 0x04 \b Description: 
    \arg \b     CY_FR_POCS_MONITOR_MODE                      \n \b Value: 0x05 \b Description: 
    \arg \b     CY_FR_POCS_CONFIG                            \n \b Value: 0x0F \b Description: 
    \arg \b     CY_FR_POCS_WAKEUP_STANDBY                    \n \b Value: 0x10 \b Description: 
    \arg \b     CY_FR_POCS_WAKEUP_LISTEN                     \n \b Value: 0x11 \b Description: 
    \arg \b     CY_FR_POCS_WAKEUP_SEND                       \n \b Value: 0x12 \b Description: 
    \arg \b     CY_FR_POCS_WAKEUP_DETECT                     \n \b Value: 0x13 \b Description: 
    \arg \b     CY_FR_POCS_STARTUP_PREPARE                   \n \b Value: 0x20 \b Description: 
    \arg \b     CY_FR_POCS_COLDSTART_LISTEN                  \n \b Value: 0x21 \b Description: 
    \arg \b     CY_FR_POCS_COLDSTART_COLLISION_RESOLUTION    \n \b Value: 0x22 \b Description: 
    \arg \b     CY_FR_POCS_COLDSTART_CONSISTENCY_CHECK       \n \b Value: 0x23 \b Description: 
    \arg \b     CY_FR_POCS_COLDSTART_GAB                     \n \b Value: 0x24 \b Description: 
    \arg \b     CY_FR_POCS_COLDSTART_JOIN                    \n \b Value: 0x25 \b Description: 
    \arg \b     CY_FR_POCS_INTEGRATION_COLDSTART_CHECK       \n \b Value: 0x26 \b Description: 
    \arg \b     CY_FR_POCS_INTEGRATION_LISTEN                \n \b Value: 0x27 \b Description: 
    \arg \b     CY_FR_POCS_INTEGRATION_CONSISTENCY_CHECK     \n \b Value: 0x28 \b Description: 
    \arg \b     CY_FR_POCS_INITIALIZE_SCHEDULE               \n \b Value: 0x29 \b Description: 
    \arg \b     CY_FR_POCS_ABORT_STARTUP                     \n \b Value: 0x2A \b Description: 
    \arg \b     CY_FR_POCS_LOG_INVALID_NOT_HALT              \n \b Value: 0x2A \b Description:     

 */
/*---------------------------------------------------------------------------------*/
typedef enum {
  CY_FR_POCS_DEFAULT_CONFIG                  = 0x00,
  CY_FR_POCS_READY                           = 0x01,
  CY_FR_POCS_NORMAL_ACTIVE                   = 0x02,
  CY_FR_POCS_NORMAL_PASSIVE                  = 0x03,
  CY_FR_POCS_HALT                            = 0x04,
  CY_FR_POCS_MONITOR_MODE                    = 0x05,
  CY_FR_POCS_CONFIG                          = 0x0F,
  CY_FR_POCS_WAKEUP_STANDBY                  = 0x10,
  CY_FR_POCS_WAKEUP_LISTEN                   = 0x11,
  CY_FR_POCS_WAKEUP_SEND                     = 0x12,
  CY_FR_POCS_WAKEUP_DETECT                   = 0x13,
  CY_FR_POCS_STARTUP_PREPARE                 = 0x20,
  CY_FR_POCS_COLDSTART_LISTEN                = 0x21,
  CY_FR_POCS_COLDSTART_COLLISION_RESOLUTION  = 0x22,
  CY_FR_POCS_COLDSTART_CONSISTENCY_CHECK     = 0x23,
  CY_FR_POCS_COLDSTART_GAB                   = 0x24,
  CY_FR_POCS_COLDSTART_JOIN                  = 0x25,
  CY_FR_POCS_INTEGRATION_COLDSTART_CHECK     = 0x26,
  CY_FR_POCS_INTEGRATION_LISTEN              = 0x27,
  CY_FR_POCS_INTEGRATION_CONSISTENCY_CHECK   = 0x28,
  CY_FR_POCS_INITIALIZE_SCHEDULE             = 0x29,
  CY_FR_POCS_ABORT_STARTUP                   = 0x2A,
  CY_FR_POCS_LOG_INVALID_NOT_HALT            = 0xFF,  
} CY_FR_POC_STATUS_TYPE;


/*---------------------------------------------------------------------------------*/
/** \brief      Protocol Operation Control Error Mode indicates the actual error mode
                of the POC.

    \par Allowed Values

    \arg \b     CY_FR_ACTIVE       \n \b Value: 0x00 \b Description: ACTIVE (green)
    \arg \b     CY_FR_PASSIVE      \n \b Value: 0x01 \b Description: PASSIVE (yellow)
    \arg \b     CY_FR_COMM_HALT    \n \b Value: 0x02 \b Description: COMM_HALT (red)

 */
/*---------------------------------------------------------------------------------*/
typedef enum {
  CY_FR_ACTIVE    = 0x00,
  CY_FR_PASSIVE   = 0x01,
  CY_FR_COMM_HALT = 0x02,
} CY_FR_ERROR_MODE_TYPE;


/*---------------------------------------------------------------------------------*/
/** \brief      Protocol Operation Control Commands to change the POC state of the
                FlexRay CC.
                
    \par Allowed Values

    \arg \b     CY_FR_POCC_COMMAND_NOT_ACCEPTED     \n \b Value: 0x00 \b Description: COMMAND_NOT_ACCEPTED
    \arg \b     CY_FR_POCC_CONFIG                   \n \b Value: 0x01 \b Description: CONFIG
    \arg \b     CY_FR_POCC_READY                    \n \b Value: 0x02 \b Description: READY
    \arg \b     CY_FR_POCC_WAKEUP                   \n \b Value: 0x03 \b Description: WAKEUP
    \arg \b     CY_FR_POCC_RUN                      \n \b Value: 0x04 \b Description: RUN
    \arg \b     CY_FR_POCC_ALL_SLOTS                \n \b Value: 0x05 \b Description: ALL_SLOTS
    \arg \b     CY_FR_POCC_HALT                     \n \b Value: 0x06 \b Description: HALT
    \arg \b     CY_FR_POCC_FREEZE                   \n \b Value: 0x07 \b Description: FREEZE
    \arg \b     CY_FR_POCC_SEND_MTS                 \n \b Value: 0x08 \b Description: SEND_MTS
    \arg \b     CY_FR_POCC_ALLOW_COLDSTART          \n \b Value: 0x09 \b Description: ALLOW_COLDSTART
    \arg \b     CY_FR_POCC_RESET_STATUS_INDICATORS  \n \b Value: 0x0A \b Description: RESET_STATUS_INDICATORS
    \arg \b     CY_FR_POCC_MONITOR_MODE             \n \b Value: 0x0B \b Description: MONITOR_MODE
    \arg \b     CY_FR_POCC_CLEAR_RAMS               \n \b Value: 0x0C \b Description: CLEAR_RAMS
                                               
 */
/*---------------------------------------------------------------------------------*/
typedef enum {
  CY_FR_POCC_COMMAND_NOT_ACCEPTED     = 0x00,
  CY_FR_POCC_CONFIG                   = 0x01,
  CY_FR_POCC_READY                    = 0x02,
  CY_FR_POCC_WAKEUP                   = 0x03,
  CY_FR_POCC_RUN                      = 0x04,
  CY_FR_POCC_ALL_SLOTS                = 0x05,
  CY_FR_POCC_HALT                     = 0x06,
  CY_FR_POCC_FREEZE                   = 0x07,
  CY_FR_POCC_SEND_MTS                 = 0x08,
  CY_FR_POCC_ALLOW_COLDSTART          = 0x09,
  CY_FR_POCC_RESET_STATUS_INDICATORS  = 0x0A,
  CY_FR_POCC_MONITOR_MODE             = 0x0B,
  CY_FR_POCC_CLEAR_RAMS               = 0x0C,
} CY_FR_POC_COMMAND_TYPE;


/*---------------------------------------------------------------------------------*/
/** \brief      This type defines the both timer of the E-ray

    \par Allowed Values

    \arg \b     CY_FR_TIMER_T0  \n \b Value: 0x00 \b Description: Timer 0
    \arg \b     CY_FR_TIMER_T1  \n \b Value: 0x01 \b Description: Timer 1

 */
/*---------------------------------------------------------------------------------*/
typedef enum {
  CY_FR_TIMER_T0 = 0x00,
  CY_FR_TIMER_T1 = 0x01,
} CY_FR_TIMER_TYPE;


/*---------------------------------------------------------------------------------*/
/** \brief      This type is used for the Stop Watch trigger modes.
             
    \par Allowed Values

    \arg \b     CY_FR_STOP_WATCH_SOFTWARE_TRIGGER               \n \b Value: 0x00 \b Description:
    \arg \b     CY_FR_STOP_WATCH_EXTERNAL_TRIGGER               \n \b Value: 0x02 \b Description:
    \arg \b     CY_FR_STOP_WATCH_INTERRUPT_0_TRIGGER            \n \b Value: 0x04 \b Description:
    \arg \b     CY_FR_STOP_WATCH_INTERRUPT_1_TRIGGER            \n \b Value: 0x08 \b Description:
    \arg \b     CY_FR_STOP_WATCH_ALL_INTERRUPT_TRIGGER          \n \b Value: 0x0C \b Description:
 */
/*---------------------------------------------------------------------------------*/
typedef enum {
  CY_FR_STOP_WATCH_SOFTWARE_TRIGGER      = 0x01,
  CY_FR_STOP_WATCH_EXTERNAL_TRIGGER      = 0x02,
  CY_FR_STOP_WATCH_INTERRUPT_0_TRIGGER   = 0x04,
  CY_FR_STOP_WATCH_INTERRUPT_1_TRIGGER   = 0x08,
  CY_FR_STOP_WATCH_ALL_INTERRUPT_TRIGGER = 0x0C,
} CY_FR_STOP_WATCH_TRIGGER_TYPE;


/*---------------------------------------------------------------------------------*/
/** \brief      valid values for Wakeup Status

    \par Allowed Values

    \arg \b     CY_FR_WAKEUP_STATUS_UNDEFINED          \n \b Value: 0x00 \b Description:
    \arg \b     CY_FR_WAKEUP_STATUS_RECEIVED_HEADER    \n \b Value: 0x01 \b Description:
    \arg \b     CY_FR_WAKEUP_STATUS_RECEIVED_WUP       \n \b Value: 0x02 \b Description:
    \arg \b     CY_FR_WAKEUP_STATUS_COLLISION_HEADER   \n \b Value: 0x03 \b Description:
    \arg \b     CY_FR_WAKEUP_STATUS_COLLISION_WUP      \n \b Value: 0x04 \b Description:
    \arg \b     CY_FR_WAKEUP_STATUS_COLLISION_UNKNOWN  \n \b Value: 0x05 \b Description:
    \arg \b     CY_FR_WAKEUP_STATUS_TRANSMITTED        \n \b Value: 0x06 \b Description:
 */
/*---------------------------------------------------------------------------------*/
typedef enum {
  CY_FR_WAKEUP_STATUS_UNDEFINED         = 0x00,
  CY_FR_WAKEUP_STATUS_RECEIVED_HEADER   = 0x01,
  CY_FR_WAKEUP_STATUS_RECEIVED_WUP      = 0x02,
  CY_FR_WAKEUP_STATUS_COLLISION_HEADER  = 0x03,
  CY_FR_WAKEUP_STATUS_COLLISION_WUP     = 0x04,
  CY_FR_WAKEUP_STATUS_COLLISION_UNKNOWN = 0x05,
  CY_FR_WAKEUP_STATUS_TRANSMITTED       = 0x06,
} CY_FR_WAKEUP_STATUS_TYPE;


/*---------------------------------------------------------------------------------*/
/** \brief      valid values for SLOT MODE

    \par Allowed Values

    \arg \b     CY_FR_SINGLE        \n \b Value: 0x00 \b Description: Slot mode is SINGLE      
    \arg \b     CY_FR_ALL_PENDING   \n \b Value: 0x02 \b Description: Slot mode is ALL_PENDING 
    \arg \b     CY_FR_PENDING       \n \b Value: 0x03 \b Description: Slot mode is PENDING     
 */
/*---------------------------------------------------------------------------------*/
typedef enum {
  CY_FR_SINGLE      = 0x00,
  CY_FR_ALL_PENDING = 0x02,
  CY_FR_PENDING     = 0x03,
} CY_FR_SLOT_MODE_TYPE;


/*---------------------------------------------------------------------------------*/
/** \brief      This Type defines the Even and Odd Communication Cycles.

    \par Allowed Values

    \arg \b     CY_FR_EVEN       \n \b Value: 0x00 \b Description: Even Communication Cycles.   
    \arg \b     CY_FR_ODD        \n \b Value: 0x01 \b Description: Odd Communication Cycles.    
 */
/*---------------------------------------------------------------------------------*/
typedef enum {
  CY_FR_EVEN = 0x00,
  CY_FR_ODD  = 0x01,
} CY_FR_CYCLE_TYPE;

/** \} group_fru_enums */

/**
* \addtogroup group_fru_data_structures
* \{
*/

/*----------------------------------------------------------------------------------
 * Structure Type Definitions
 *---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/** \brief      This structure contains the Output Buffer Header Section Register 
                Read Header Section 1-3 and Message Buffer Status as 32 Bit Values.
                It is used by function cy_fr_api_rx_handler_buffer ( ), 
                cy_fr_api_rx_handler_id ( ) and  cy_fr_api_rx_handler_fifo ( ) as
                output parameter.
    
    \par        Field Documentation 

    \arg \b     uint32_t \b RDHS1;   \n Header Section 1 of the output buffer 
    \arg \b     uint32_t \b RDHS2;   \n Header section 2 of the output buffer 
    \arg \b     uint32_t \b RDHS3;   \n Header section 3 of the output buffer 
    \arg \b     uint32_t \b MBS;     \n Message buffer status of output buffer
 */
/*---------------------------------------------------------------------------------*/
typedef struct cy_fr_rx_buffer_header_struct{
  uint32_t RDHS1;             /* Header Section 1 of the output buffer */
  uint32_t RDHS2;             /* Header section 2 of the output buffer */
  uint32_t RDHS3;             /* Header section 3 of the output buffer */
  uint32_t MBS;               /* Message buffer status of output buffer */
}CY_FR_RX_BUFFER_HEADER_STRUCT;



/*---------------------------------------------------------------------------------*/
/** \brief      This structure contains the stop watch values. It used as output 
                parameter from cy_fr_api_get_stpw ( ).
    
    \par        Field Documentation 

    \arg \b     uint16_t \b MACROTICK;              \n Macrotick value from the Stop Watch Register 1
    \arg \b     uint16_t \b CYCLE_COUNTER;          \n Cycle Counter value form the Stop Watch Register 1
    \arg \b     uint16_t \b SLOT_COUNTER_CHANNEL_A; \n Slot Counter value for Channel A STPW 2
    \arg \b     uint16_t \b SLOT_COUNTER_CHANNEL_B; \n SLot Counter value for Channel B STPW 2

 */
/*---------------------------------------------------------------------------------*/
typedef struct cy_fr_stop_watch_struct{
  uint16_t MACROTICK;               /* Macrotick value from the Stop Watch Register 1 */
  uint16_t CYCLE_COUNTER;           /* Cycle Counter value form the Stop Watch Register 1 */
  uint16_t SLOT_COUNTER_CHANNEL_A;  /* Slot Counter value for Channel A STPW 2 */
  uint16_t SLOT_COUNTER_CHANNEL_B;  /* SLot Counter value for Channel B STPW 2 */
}CY_FR_STOP_WATCH_STRUCT;



/*---------------------------------------------------------------------------------*/
/** \brief      This structure contains aggregated channel status flags for one 
                channel. It is used by function cy_fr_api_aggregated_channel_status ( ) 
                as output parameter.
    
    \par        Field Documentation 

    \arg \b     bool \b VALID_FRAME;              \n Valid Frame received for Channel
    \arg \b     bool \b SYNTAX_ERROR;             \n Syntax Error Detected for Channel
    \arg \b     bool \b CONTENT_ERROR;            \n Content Error Detected for Channel 
    \arg \b     bool \b COMMUNICATION_INDICATOR;  \n Communication Indicator for Channel
    \arg \b     bool \b SLOT_BOUNDARY_VIOLATION;  \n Slot boundary violation for channel

 */
/*---------------------------------------------------------------------------------*/
typedef struct cy_fr_aggregated_channel_status_struct{
  bool VALID_FRAME;             /* Valid Frame received for Channel */
  bool SYNTAX_ERROR;            /* Syntax Error Detected for Channel */
  bool CONTENT_ERROR;           /* Content Error Detected for Channel */
  bool COMMUNICATION_INDICATOR; /* Communication Indicator for Channel */
  bool SLOT_BOUNDARY_VIOLATION; /* Slot boundary violation for channel */
}CY_FR_AGGREGATED_CHANNEL_STATUS_STRUCT;


/*---------------------------------------------------------------------------------*/
/** \brief      This structure contains the E-Ray Core Release Information. It is 
                used by  functioncy_fr_api_get_eray_version ( ) as output parameter.
    
    \par        Field Documentation 

    \arg \b     uint8_t \b DAY;   \n Design Time Stamp, Day
    \arg \b     uint8_t \b MON;   \n Design Time Stamp, Month
    \arg \b     uint8_t \b YEAR;  \n Design Time Stamp, Year 
    \arg \b     uint8_t \b STEP;  \n Step of Core Release
    \arg \b     uint8_t \b REL;   \n Core Release

 */
/*---------------------------------------------------------------------------------*/
typedef struct cy_fr_core_release_struct{
  uint8_t DAY;                 /* Design Time Stamp, Day */
  uint8_t MON;                 /* Design Time Stamp, Month */
  uint8_t YEAR;                /* Design Time Stamp, Year */
  uint8_t STEP;                /* Step of Core Release */
  uint8_t REL;                 /* Core Release */
}CY_FR_CORE_RELEASE_STRUCT;



/*---------------------------------------------------------------------------------*/
/** \brief      This structure contains the parity flags for the message buffer RAM A 
                and B, input buffer RAM and for the output buffer RAM. It is used by
                function cy_fr_api_get_buffer_parity_flags ( ) as output parameter.
           
    \par        Field Documentation 

    \arg \b     bool \b PIBF;    \n Parity Error Input buffer RAM 1,2 
    \arg \b     bool \b POBF;    \n Parity Error Output buffer RAM 1,2
    \arg \b     bool \b PMR;     \n Parity Error Message RAM       
    \arg \b     bool \b PTBF1;   \n Parity Error Transient Buffer A
    \arg \b     bool \b PTBF2;   \n Parity Error Transient Buffer B

 */
/*---------------------------------------------------------------------------------*/
typedef struct cy_fr_buffer_parity_struct{
  bool PIBF;                /* Parity Error Input buffer RAM 1,2 */
  bool POBF;                /* Parity Error Output buffer RAM 1,2*/
  bool PMR;                 /* Parity Error Message RAM */
  bool PTBF1;               /* Parity Error Transient Buffer A */
  bool PTBF2;               /* Parity Error Transient Buffer B */
}CY_FR_BUFFER_PARITY_STRUCT;       


/*---------------------------------------------------------------------------------*/
/** \brief      This structure contains the frame id and the channel of an even or 
                odd sync frame. It is used by function cy_fr_api_get_sync_frame ( ) as 
                output parameter.
    
    \par        Field Documentation 

    \arg \b     CY_FR_FRAME_ID_TYPE \b FRAME_ID;    \n Frame ID of a received Sync Frame.
    \arg \b     CY_FR_CHANNEL_TYPE  \b CHANNEL;     \n Channel(s) of a received Sync Frame. 

 */
/*---------------------------------------------------------------------------------*/
typedef struct cy_fr_sync_frame_struct{
  CY_FR_FRAME_ID_TYPE FRAME_ID;           /* Frame ID of a received Sync Frame */
  CY_FR_CHANNEL_TYPE  CHANNEL;            /* Channel(s) of a received Sync Frame */
}CY_FR_SYNC_FRAME_STRUCT;

/** \} group_fru_data_structures */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_API_GLOBAL_H__ */
/*---------------------------------------------------------------------------------*/


#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */

