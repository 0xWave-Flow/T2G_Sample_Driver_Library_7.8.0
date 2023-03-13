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
#include "crypto_tdes_test_vector.h"

/***********************************************************************************************************************
 * TDES
 ***********************************************************************************************************************/
void Run_TDES(void)
{
    cy_stc_crypto_context_des_t cryptoDesContext;

    /* Encryption with several block(s) */
    for (uint32_t i = 0; i < TDES_TEST_BLOCK_NUM; i++)
    {
        Cy_Crypto_Tdes_Run
        (
            &cryptoCtx,
            &cryptoDesContext,
            CY_CRYPTO_ENCRYPT,
            &Tdes_Test_Vector0_keyPtr,
            &Tdes_Test_Vector0_cipher[i],
            &Tdes_Test_Vector0_plain_ref[i]
        );
        /* Wait for completion */
        Cy_Crypto_Sync(true, &cryptoCtx);

        /* Compare with reference value just for testing. */
        /* You can remove following ASSERT */
        CY_ASSERT(memcmp(&Tdes_Test_Vector0_cipher[i], &Tdes_Test_Vector0_cipher_ref[i], sizeof(Tdes_Test_Vector0_cipher_ref[i])) == 0);
    }
    /* Then you can get the result from Tdes_Test_Vector0_cipher */

    /* Decryption with several block(s) */
    for (uint32_t i = 0; i < TDES_TEST_BLOCK_NUM; i++)
    {
        Cy_Crypto_Tdes_Run
        (
            &cryptoCtx,
            &cryptoDesContext,
            CY_CRYPTO_DECRYPT,
            &Tdes_Test_Vector0_keyPtr,
            &Tdes_Test_Vector0_plain[i],
            &Tdes_Test_Vector0_cipher_ref[i]
        );
        /* Wait for completion */
        Cy_Crypto_Sync(true, &cryptoCtx);

        /* Compare with reference value just for testing. */
        /* You can remove following ASSERT */
        CY_ASSERT(memcmp(&Tdes_Test_Vector0_plain[i], &Tdes_Test_Vector0_plain_ref[i], sizeof(Tdes_Test_Vector0_plain_ref[i])) == 0);
    }
    /* Then you can get the result from Tdes_Test_Vector0_plain */
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

    /* Run TDES */
    Run_TDES();

    for(;;)
    {
    }
}



/* [] END OF FILE */
