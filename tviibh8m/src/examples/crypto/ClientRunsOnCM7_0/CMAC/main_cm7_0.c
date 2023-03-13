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
#include "crypto_cmac_test_vector.h"

/***********************************************************************************************************************
 * CMAC
 ***********************************************************************************************************************/
void Run_CMAC(void)
{
    cy_stc_crypto_context_aes_t cryptoAesContext;

    /* Calculate CMACs */
    for (uint32_t i = 0; i < Cmac_128_TestNum; i++)
    {
        Cy_Crypto_Aes_Cmac_Run
        (
            &cryptoCtx,
            &cryptoAesContext,
            Cmac_128_MessagePtr[i],
            Cmac_128_MessageLen[i],
            &Cmac_128_Key,
            CY_CRYPTO_KEY_AES_128,
            &Cmac_128_Result[i]
        );
        /* Wait for completion */
        Cy_Crypto_Sync(true, &cryptoCtx);

        /* Compare with reference value just for testing. */
        /* You can remove following ASSERT */
        CY_ASSERT(memcmp(&Cmac_128_Result[i], &Cmac_128_Result_Ref[i], sizeof(Cmac_128_Result_Ref[i])) == 0u);
    }
    /* Then you can get the result from Cmac_128_Result */
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

    /* Run CMAC */
    Run_CMAC();

    for(;;)
    {
    }
}



/* [] END OF FILE */
