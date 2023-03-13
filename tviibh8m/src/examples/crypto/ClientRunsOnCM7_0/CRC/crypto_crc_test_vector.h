/***************************************************************************//**
* \file crypto_crc_test_vector.h
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

#ifndef __CRYPTO_CRC_TEST_VECTOR__
#define __CRYPTO_CRC_TEST_VECTOR__

#ifdef __CRYPTO_ENABLE_TEST_VECTOR__

/*******************************************************************************
* CRC32
********************************************************************************/
const uint8_t  CRC32_src[]          = {0x12, 0x34, 0x56, 0x78, 0x9a};
const uint32_t CRC32_polynomial     = 0x04c11db7uL;
const uint32_t CRC32_dataReverse    = 1uL;
const uint32_t CRC32_dataXor        = 0uL;
const uint32_t CRC32_lfsrInitState  = 0xffffffffuL;
const uint32_t CRC32_remReverse     = 1uL;
const uint32_t CRC32_remXor         = 0xffffffffuL;
const uint32_t CRC32_result_ref     = 0x3c4687af;
uint32_t CRC32_dataSize = sizeof(CRC32_src) / sizeof(CRC32_src[0]);
uint32_t CRC32_result;

/*******************************************************************************
* CRC16CCITT
********************************************************************************/
const uint8_t  CRC16CCITT_src[]          = {0x12, 0x34, 0x56, 0x78, 0x9a};
const uint32_t CRC16CCITT_polynomial     = 0x10210000;
const uint32_t CRC16CCITT_dataReverse    = 0;
const uint32_t CRC16CCITT_dataXor        = 0x00;
const uint32_t CRC16CCITT_lfsrInitState  = 0xffff0000;
const uint32_t CRC16CCITT_remReverse     = 0;
const uint32_t CRC16CCITT_remXor         = 0x00000000;
const uint32_t CRC16CCITT_result_ref     = 0xf8a00000;
uint32_t CRC16CCITT_dataSize = sizeof(CRC16CCITT_src) / sizeof(CRC16CCITT_src[0]);
uint32_t CRC16CCITT_result;

/*******************************************************************************
* CRC16
********************************************************************************/
const uint8_t  CRC16_src[]          = {0x12, 0x34, 0x56, 0x78, 0x9a};
const uint32_t CRC16_polynomial     = 0x80050000;
const uint32_t CRC16_dataReverse    = 1;
const uint32_t CRC16_dataXor        = 0x00;
const uint32_t CRC16_lfsrInitState  = 0xffff0000;
const uint32_t CRC16_remReverse     = 1;
const uint32_t CRC16_remXor         = 0x00000000;
const uint32_t CRC16_result_ref     = 0x000048d0;
uint32_t CRC16_dataSize = sizeof(CRC16_src) / sizeof(CRC16_src[0]);
uint32_t CRC16_result;

#endif

#endif

