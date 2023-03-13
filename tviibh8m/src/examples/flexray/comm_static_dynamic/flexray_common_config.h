/***************************************************************************//**
* \file flexray_common_config.h
*
* \version 1.0
*
* \brief Supporting example file
*
********************************************************************************
* \copyright
* Copyright 2016-2020, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef FLEXRAY_COMMON_CONFIG_H
#define FLEXRAY_COMMON_CONFIG_H

#define STATIC_TX_BUF_AB    0
#define STATIC_RX_BUF_A     1
#define STATIC_RX_BUF_B     2
#define DYNAMIC_TX_BUF_A    3
#define DYNAMIC_RX_BUF_A    4

#define PAYLOAD_STATIC_BYTES    32
#define PAYLOAD_DYNAMIC_BYTES   16

#define BARGRAPH_INCR_MIN         0
#define BARGRAPH_INCR_MAX        15
#define BARGRAPH_INCR_INIT        8 
#define BARGRAPH_INCR_STEP        1

typedef struct
{
  uint8_t bargraph_slider_val;
  uint8_t reserved[31];
} PAYLOAD_STATIC_STRUCT;

typedef struct
{
  uint8_t remote_bargraph_incr_val; // (Note: "hexchar_val" (if connected to sample on MB91465X board)
  uint8_t reserved[15];
} PAYLOAD_DYNAMIC_STRUCT;




#endif
