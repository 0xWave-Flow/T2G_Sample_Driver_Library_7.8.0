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
#include "crypto_des_test_vector.h"

/***********************************************************************************************************************
 * DES
 ***********************************************************************************************************************/
void Run_DES(void)
{
    cy_stc_crypto_context_des_t cryptoDesContext;

    /* Encryption */
    Cy_Crypto_Des_Run
    (
        &cryptoCtx,
        &cryptoDesContext,
        CY_CRYPTO_ENCRYPT,
        &Des_Set8_Vector0_keyPtr,
        &Des_Set8_Vector0_cipher,
        &Des_Set8_Vector0_plain_ref
    );
    /* Wait for completion */
    Cy_Crypto_Sync(true, &cryptoCtx);

    /* Compare with reference value just for testing. */
    /* You can remove following ASSERT */
    CY_ASSERT(memcmp(&Des_Set8_Vector0_cipher, &Des_Set8_Vector0_cipher_ref, sizeof(Des_Set8_Vector0_cipher_ref)) == 0);
    /* Then you can get the result from Des_Set8_Vector0_cipher */

    /* Decryption */
    Cy_Crypto_Des_Run
    (
        &cryptoCtx,
        &cryptoDesContext,
        CY_CRYPTO_DECRYPT,
        &Des_Set8_Vector0_keyPtr,
        &Des_Set8_Vector0_plain,
        &Des_Set8_Vector0_cipher_ref
    );
    /* Wait for completion */
    Cy_Crypto_Sync(true, &cryptoCtx);

    /* Compare with reference value just for testing. */
    /* You can remove following ASSERT */
    CY_ASSERT(memcmp(&Des_Set8_Vector0_plain, &Des_Set8_Vector0_plain_ref, sizeof(Des_Set8_Vector0_plain_ref)) == 0);
    /* Then you can get the result from Des_Set8_Vector0_plain */
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

    /* Run DES */
    Run_DES();

    for(;;)
    {
    }
}



/* [] END OF FILE */
