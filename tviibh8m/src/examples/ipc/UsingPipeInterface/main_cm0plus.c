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
#include "example_config.h"

#if (EXAMPLE_CONFIG_SELECTED == EXAMPLE_CONFIG__CM0P_TO_CM7_0) || (EXAMPLE_CONFIG_SELECTED == EXAMPLE_CONFIG__CM0P_TO_CM7_1)

typedef struct
{
    uint32_t clientId;  /* PIPE I/F internal use area. Must be first element of the structure. */
    uint32_t LedData;
} custom_data_t;

custom_data_t pipeData = 
{
    .clientId = 0x01u,
    .LedData  = 0x00u
};

bool released;

cy_en_ipc_pipe_status_t status;

cy_stc_ipc_pipe_config_t pipeConfig = 
{
    .epIndexForThisCpu = EP_INDEX_THIS_CPU,
    .epConfigData      = CY_IPC_PIPE_ENDPOINTS_DEFAULT_CONFIG,
};

/* Call when the other end point release the handle corresponding this end point */
void ReleaseCallback(void)
{
    /* Notified core already got the data. */
    /* Update send data for next transmission. */
    released = true;
}

#endif

int main(void)
{
    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

  #if (EXAMPLE_CONFIG_SELECTED == EXAMPLE_CONFIG__CM0P_TO_CM7_0) || (EXAMPLE_CONFIG_SELECTED == EXAMPLE_CONFIG__CM0P_TO_CM7_1)
      
    /* Initialize PIPE */
    Cy_IPC_Pipe_Init(&pipeConfig);

    /* Already registered the handler to the system interrup structure.
     * So just enable corresponding IRQ channel.
     */
    NVIC_ClearPendingIRQ(pipeConfig.epConfigData[EP_INDEX_THIS_CPU].ipcCpuIntIdx);
    NVIC_EnableIRQ(pipeConfig.epConfigData[EP_INDEX_THIS_CPU].ipcCpuIntIdx);
    
  #endif

    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);

    for(;;)
    {
      #if (EXAMPLE_CONFIG_SELECTED == EXAMPLE_CONFIG__CM0P_TO_CM7_0) || (EXAMPLE_CONFIG_SELECTED == EXAMPLE_CONFIG__CM0P_TO_CM7_1)
        /* Send data */
        released = false;
        status = Cy_IPC_Pipe_SendMessage
        (
            EP_INDEX_OTHER_CPU,
            &pipeData,
            ReleaseCallback
        );
        
        /* When the data successfully into the pipe */
        if (status == CY_IPC_PIPE_SUCCESS)
        {
            while(!released)
            {
                /* wait until the data has been received by the other core */
            }
            /* Update the data to be sent */
            pipeData.LedData = (pipeData.LedData + 1) % 5;
            // Wait 0.5 [s]
            Cy_SysTick_DelayInUs(500000);
        }
      #endif
    }
}



/* [] END OF FILE */
