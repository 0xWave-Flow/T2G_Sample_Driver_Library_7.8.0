/***************************************************************************//**
* \file main_cm7_0.c
*
* \version 1.0
*
* \brief Main example file for CM7_0
*
********************************************************************************
* \copyright
* Copyright 2016-2020, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"

/* Include default crypto configuration.
 * You can use other configuration instead follows.
 */
#include "crypto/cy_crypto_config.h"

/* For CRYPTO client that can run on the both CPUs */
cy_stc_crypto_context_t        cryptoCtx;

/* Test vector for checking the behavior */
#define __CRYPTO_ENABLE_TEST_VECTOR__   (1)
#include "crypto_aes_test_vector.h"


/***********************************************************************************************************************
 * AES ECB with 128bit key
 ***********************************************************************************************************************/
uint8_t AesEcb128CypherTxt[4][16];
uint8_t AesEcb128PlainTxt[4][16];
void Run_AES_ECB(void)
{
    cy_stc_crypto_context_aes_t cryptoAesContext;

    /* Encrypt 128bit x 4 plain text */
    for (uint32_t i = 0; i < sizeof(AesEcb128PlainTxtRef) / sizeof(AesEcb128PlainTxtRef[0]); i++)
    {
        Cy_Crypto_Aes_Ecb_Run
        (
            &cryptoCtx,
            &cryptoAesContext,
            CY_CRYPTO_ENCRYPT,
            &AesEcbKey128,
            CY_CRYPTO_KEY_AES_128,
            &AesEcb128CypherTxt[i],
            &AesEcb128PlainTxtRef[i]
        );
        /* Wait for completion */
        Cy_Crypto_Sync(true, &cryptoCtx);

        /* Compare with reference value just for testing. */
        /* You can remove following ASSERT */
        CY_ASSERT(memcmp(&AesEcb128CypherTxt[i], &AesEcb128CypherTxtRef[i], sizeof(AesEcb128CypherTxtRef[i])) == 0u);
    }
    /* Then you can get the result from AesEcb128CypherTxt */

    /* Decrypt 128bit x 4 plain text */
    for (uint32_t i = 0; i < sizeof(AesEcb128CypherTxtRef) / sizeof(AesEcb128CypherTxtRef[0]); i++)
    {
        Cy_Crypto_Aes_Ecb_Run
        (
            &cryptoCtx,
            &cryptoAesContext,
            CY_CRYPTO_DECRYPT,
            &AesEcbKey128,
            CY_CRYPTO_KEY_AES_128,
            &AesEcb128PlainTxt[i],
            &AesEcb128CypherTxtRef[i]
        );
        /* Wait for completion */
        Cy_Crypto_Sync(true, &cryptoCtx);

        /* Compare with reference value just for testing. */
        /* You can remove following ASSERT */
        CY_ASSERT(memcmp(&AesEcb128PlainTxt[i], &AesEcb128PlainTxtRef[i], sizeof(AesEcb128PlainTxtRef[i])) == 0u);
    }
    /* Then you can get the result from AesEcb128PlainTxt */
}


/***********************************************************************************************************************
 * AES CBC with 128bit key
 ***********************************************************************************************************************/
uint8_t AesCbc128CypherTxt[4][16];
uint8_t AesCbc128PlainTxt[4][16];
void Run_AES_CBC(void)
{
    cy_stc_crypto_context_aes_t cryptoAesContext;

    /* Encrypt 128bit x 4 plain text */
    Cy_Crypto_Aes_Cbc_Run
    (
        &cryptoCtx,
        &cryptoAesContext,
        CY_CRYPTO_ENCRYPT,
        &AesCbcKey128,
        CY_CRYPTO_KEY_AES_128,
        sizeof(AesCbc128PlainTxtRef),
        &AesCbcIV,
        &AesCbc128CypherTxt,
        &AesCbc128PlainTxtRef
    );
    /* Wait for completion */
    Cy_Crypto_Sync(true, &cryptoCtx);

    /* Compare with reference value just for testing. */
    /* You can remove following ASSERT */
    CY_ASSERT(memcmp(&AesCbc128CypherTxt, &AesCbc128CypherTxtRef, sizeof(AesCbc128CypherTxtRef)) == 0u);
    /* Then you can get the result from AesCbc128CypherTxt */

    /* Decrypt 128bit x 4 plain text */
    Cy_Crypto_Aes_Cbc_Run
    (
        &cryptoCtx,
        &cryptoAesContext,
        CY_CRYPTO_DECRYPT,
        &AesCbcKey128,
        CY_CRYPTO_KEY_AES_128,
        sizeof(AesCbc128CypherTxtRef),
        &AesCbcIV,
        &AesCbc128PlainTxt,
        &AesCbc128CypherTxtRef
    );
    /* Wait for completion */
    Cy_Crypto_Sync(true, &cryptoCtx);

    /* Compare with reference value just for testing. */
    /* You can remove following ASSERT */
    CY_ASSERT(memcmp(&AesCbc128PlainTxt, &AesCbc128PlainTxtRef, sizeof(AesCbc128PlainTxtRef)) == 0u);
    /* Then you can get the result from AesCbc128PlainTxt */
}


/***********************************************************************************************************************
 * AES CFB with 128bit key
 ***********************************************************************************************************************/
uint8_t AesCfb128CypherTxt[4][16];
uint8_t AesCfb128PlainTxt[4][16];
void Run_AES_CFB(void)
{
    cy_stc_crypto_context_aes_t cryptoAesContext;

    /* Encrypt 128bit x 4 plain text */
    Cy_Crypto_Aes_Cfb_Run
    (
        &cryptoCtx,
        &cryptoAesContext,
        CY_CRYPTO_ENCRYPT,
        &AesCfbKey128,
        CY_CRYPTO_KEY_AES_128,
        sizeof(AesCfb128PlainTxtRef),
        &AesCfbIV,
        &AesCfb128CypherTxt,
        &AesCfb128PlainTxtRef
    );
    /* Wait for completion */
    Cy_Crypto_Sync(true, &cryptoCtx);

    /* Compare with reference value just for testing. */
    /* You can remove following ASSERT */
    CY_ASSERT(memcmp(&AesCfb128CypherTxt, &AesCfb128CypherTxtRef, sizeof(AesCfb128CypherTxtRef)) == 0u);
    /* Then you can get the result from AesCfb128CypherTxt */

    /* Decrypt 128bit x 4 plain text */
    Cy_Crypto_Aes_Cfb_Run
    (
        &cryptoCtx,
        &cryptoAesContext,
        CY_CRYPTO_DECRYPT,
        &AesCfbKey128,
        CY_CRYPTO_KEY_AES_128,
        sizeof(AesCfb128CypherTxtRef),
        &AesCfbIV,
        &AesCfb128PlainTxt,
        &AesCfb128CypherTxtRef
    );
    /* Wait for completion */
    Cy_Crypto_Sync(true, &cryptoCtx);

    /* Compare with reference value just for testing. */
    /* You can remove following ASSERT */
    CY_ASSERT(memcmp(&AesCfb128PlainTxt, &AesCfb128PlainTxtRef, sizeof(AesCfb128PlainTxtRef)) == 0u);
    /* Then you can get the result from AesCfb128PlainTxt */
}

/***********************************************************************************************************************
 * AES CTR with 128bit key
 ***********************************************************************************************************************/
uint8_t AesCtr128CypherTxt[4][16];
uint8_t AesCtr128PlainTxt[4][16];
void Run_AES_CTR(void)
{
    cy_stc_crypto_context_aes_t cryptoAesContext;
    uint32_t srcOffset;

    /* In CTR mode, IV will be updated as a counter. */
    /* So if original value is needed copy it */
    for (uint8_t i = 0; i < 16; i++)
    {
        AesCtrIV_copied[i] = AesCtrIV[i];
    }

    srcOffset = 0;
    /* Encrypt 128bit x 4 plain text */
    Cy_Crypto_Aes_Ctr_Run
    (
        &cryptoCtx,
        &cryptoAesContext,
        CY_CRYPTO_ENCRYPT,
        &AesCtrKey128,
        CY_CRYPTO_KEY_AES_128,
        sizeof(AesCtr128PlainTxtRef),
        &srcOffset,
        &AesCtrIV_copied,
        &AesCtr128CypherTxt,
        &AesCtr128PlainTxtRef
    );
    /* Wait for completion */
    Cy_Crypto_Sync(true, &cryptoCtx);

    /* Compare with reference value just for testing. */
    /* You can remove following ASSERT */
    CY_ASSERT(memcmp(&AesCtr128CypherTxt, &AesCtr128CypherTxtRef, sizeof(AesCtr128CypherTxtRef)) == 0u);
    /* Then you can get the result from AesCtr128CypherTxt */

    for (uint8_t i = 0; i < 16; i++)
    {
        AesCtrIV_copied[i] = AesCtrIV[i];
    }
    srcOffset = 0;
    /* Decrypt 128bit x 4 plain text */
    Cy_Crypto_Aes_Ctr_Run
    (
        &cryptoCtx,
        &cryptoAesContext,
        CY_CRYPTO_DECRYPT,
        &AesCtrKey128,
        CY_CRYPTO_KEY_AES_128,
        sizeof(AesCtr128CypherTxtRef),
        &srcOffset,
        &AesCtrIV_copied,
        &AesCtr128PlainTxt,
        &AesCtr128CypherTxtRef
    );
    /* Wait for completion */
    Cy_Crypto_Sync(true, &cryptoCtx);

    /* Compare with reference value just for testing. */
    /* You can remove following ASSERT */
    CY_ASSERT(memcmp(&AesCtr128PlainTxt, &AesCtr128PlainTxtRef, sizeof(AesCtr128PlainTxtRef)) == 0u);
    /* Then you can get the result from AesCtr128PlainTxt */
}

/***********************************************************************************************************************
 * main function
 ***********************************************************************************************************************/
int main(void)
{
    SystemInit();

    // Example had been originally tested with "cache off", so ensure that caches are turned off (may have been enabled by new startup.c module)
    SCB_DisableICache(); // Disables and invalidates instruction cache
    SCB_DisableDCache(); // Disables, cleans and invalidates data cache

    __enable_irq(); /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    /* Initialize and enable CRYPTO */
    {
        cy_en_crypto_status_t status;
        do
        {
            status = Cy_Crypto_Init(&cryptoConfig, &cryptoCtx);
        } while(status != CY_CRYPTO_SUCCESS);
        Cy_Crypto_Enable(&cryptoCtx);
    }

    /* Run AES ECB mode */
    Run_AES_ECB();
    /* Run AES CBC mode */
    Run_AES_CBC();
    /* Run AES CFB mode */
    Run_AES_CFB();
    /* Run AES CTR mode */
    Run_AES_CTR();

    for(;;)
    {
    }
}



/* [] END OF FILE */
