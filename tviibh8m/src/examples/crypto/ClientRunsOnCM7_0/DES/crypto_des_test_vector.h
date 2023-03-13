/***************************************************************************//**
* \file crypto_des_test_vector.h
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

#ifndef __CRYPTO_DES_TEST_VECTOR__
#define __CRYPTO_DES_TEST_VECTOR__

#ifdef __CRYPTO_ENABLE_TEST_VECTOR__

/* Refer https://www.cosic.esat.kuleuven.be/nessie/testvectors/bc/des/Des-64-64.test-vectors */
/*******************************************************************************
* Set8_Vector0
********************************************************************************/
const uint8_t Des_Set8_Vector0_keyPtr[] = 
{
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
};
const uint8_t Des_Set8_Vector0_plain_ref[] = 
{
    0x41,0xAD,0x06,0x85,0x48,0x80,0x9D,0x02,
};
const uint8_t Des_Set8_Vector0_cipher_ref[] = 
{
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
};
uint8_t Des_Set8_Vector0_cipher[8];
uint8_t Des_Set8_Vector0_plain[8];

/*******************************************************************************
* Set8_Vector1
********************************************************************************/
const uint8_t Des_Set8_Vector1_keyPtr[] = 
{
    0x2B,0xD6,0x45,0x9F,0x82,0xC5,0xB3,0x00,
};
const uint8_t Des_Set8_Vector1_plain_ref[] = 
{
    0xB1,0x0F,0x84,0x30,0x97,0xA0,0xF9,0x32,
};
const uint8_t Des_Set8_Vector1_cipher_ref[] = 
{
    0xEA,0x02,0x47,0x14,0xAD,0x5C,0x4D,0x84,
};
uint8_t Des_Set8_Vector1_cipher[8];
uint8_t Des_Set8_Vector1_plain[8];

#endif

#endif

