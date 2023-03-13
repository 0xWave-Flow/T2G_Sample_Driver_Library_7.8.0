/***************************************************************************//**
* \file crypto_tdes_test_vector.h
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

#ifndef __CRYPTO_TDES_TEST_VECTOR__
#define __CRYPTO_TDES_TEST_VECTOR__

#ifdef __CRYPTO_ENABLE_TEST_VECTOR__

/* http://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-67r1.pdf */
/*******************************************************************************
* TDES
********************************************************************************/
#define TDES_TEST_BLOCK_NUM (3)
const uint8_t Tdes_Test_Vector0_keyPtr[] = 
{
    0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,
    0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,
    0x45,0x67,0x89,0xAB,0xCD,0xEF,0x01,0x23,
};
const uint8_t Tdes_Test_Vector0_plain_ref[TDES_TEST_BLOCK_NUM][8] = 
{
    {0x54,0x68,0x65,0x20,0x71,0x75,0x66,0x63},
    {0x6B,0x20,0x62,0x72,0x6F,0x77,0x6E,0x20},
    {0x66,0x6F,0x78,0x20,0x6A,0x75,0x6D,0x70},
};
const uint8_t Tdes_Test_Vector0_cipher_ref[TDES_TEST_BLOCK_NUM][8] = 
{
    {0xA8,0x26,0xFD,0x8C,0xE5,0x3B,0x85,0x5F},
    {0xCC,0xE2,0x1C,0x81,0x12,0x25,0x6F,0xE6},
    {0x68,0xD5,0xC0,0x5D,0xD9,0xB6,0xB9,0x00},
};
uint8_t Tdes_Test_Vector0_cipher[TDES_TEST_BLOCK_NUM][8];
uint8_t Tdes_Test_Vector0_plain[TDES_TEST_BLOCK_NUM][8];

#endif

#endif

