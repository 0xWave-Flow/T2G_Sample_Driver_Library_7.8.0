/***************************************************************************//**
* \file main_cm0plus.c
*
* \version 1.0
*
* \brief Main example file for CM0+
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

/* For CRYPTO server that runs on the CM0+ */
cy_stc_crypto_server_context_t cryptoServerCtx;

/***********************************************************************************************************************
 * main function
 ***********************************************************************************************************************/
int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /* Start crypto server */
    {
        Cy_Crypto_Server_Start(&cryptoConfig, &cryptoServerCtx);

        /* Enable CRYPTO regarding IRQn */
        /* In this example 2 of crypto regarding interrupt uses same number,
            so just one invoking NVIC setting is needed.
                CY_CRYPTO_NOTIFY_INTR_NR    == 
                CY_CRYPTO_ERROR_INTR_NR
        */
        NVIC_SetPriority(CY_CRYPTO_NOTIFY_CPU_INT_IDX, 0);
        NVIC_ClearPendingIRQ(CY_CRYPTO_NOTIFY_CPU_INT_IDX);
        NVIC_EnableIRQ(CY_CRYPTO_NOTIFY_CPU_INT_IDX);
    }

    /* Enable CM7_0. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);

    /* Now CRYPTO server runs */
    for(;;)
    {
    }
}



/* [] END OF FILE */
