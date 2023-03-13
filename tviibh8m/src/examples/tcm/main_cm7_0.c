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


#if (CY_USE_PSVP == 1)
    #define USER_LED_PORT           CY_LED8_PORT
    #define USER_LED_PIN            CY_LED8_PIN
    #define USER_LED_PIN_MUX        CY_LED8_PIN_MUX
#else
    #define USER_LED_PORT           CY_CB_USER_LED2_PORT
    #define USER_LED_PIN            CY_CB_USER_LED2_PIN
    #define USER_LED_PIN_MUX        CY_CB_USER_LED2_PIN_MUX 
#endif

// CM7 DTCM Region
#define DTCM_SELF_RAM_ADDR          (0x20000000UL)

// DTCM Access Type
#define DTCM_RAM_ACCESS_BYTE        0
#define DTCM_RAM_ACCESS_WORD        1
#define DTCM_RAM_ACCESS             DTCM_RAM_ACCESS_WORD

// DTCM Access Size
#if (DTCM_RAM_ACCESS == DTCM_RAM_ACCESS_BYTE)
    #define DTCM_RAM_ACCESS_SIZE    16384
#else // (DTCM_RAM_ACCESS == DTCM_RAM_ACCESS_WORD)
    #define DTCM_RAM_ACCESS_SIZE    4096
#endif

cy_stc_gpio_pin_config_t user_led_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = USER_LED_PIN_MUX,                           
    .intEdge = 0,                                  
    .intMask = 0,                                  
    .vtrip = 0,                                    
    .slewRate = 0,                                 
    .driveSel = 0,                                 
    .vregEn = 0,                                   
    .ibufMode = 0,                                 
    .vtripSel = 0,                                 
    .vrefSel = 0,                                  
    .vohSel = 0,                                   
};

#if (DTCM_RAM_ACCESS == DTCM_RAM_ACCESS_BYTE)
    uint8_t testRead = 0;
    uint8_t writeData[2] = {0x55, 0xAA}; 
    volatile uint8_t * ptrTCM = NULL; 
#else // (DTCM_RAM_ACCESS == DTCM_RAM_ACCESS_WORD)
    uint32_t testRead = 0;
    uint32_t writeData[2] = {0x55AA55AAul, 0xAA55AA55ul};
    volatile uint32_t * ptrTCM = NULL;    
#endif

int main(void)
{
    uint16_t i = 0, temp; 
    
    SystemInit();

    // Example had been originally tested with "cache off", so ensure that caches are turned off (may have been enabled by new startup.c module)
    SCB_DisableICache(); // Disables and invalidates instruction cache
    SCB_DisableDCache(); // Disables, cleans and invalidates data cache
    
    __enable_irq();
    
    // Enable (unlock) access to ITCM and DTCM Registers
    temp = CPUSS->unCM7_0_CTL.stcField.u4PPB_LOCK;
    temp = temp & 0xC;
    CPUSS->unCM7_0_CTL.stcField.u4PPB_LOCK = temp;
    
    // Enable DTCM
    Cy_Cpu_TcmEnable(CY_CPU_CM7_0, CY_CPU_DTCM);
    // Enable DTCM RMW
    Cy_Cpu_TcmReadModifyWriteEnable(CY_CPU_CM7_0, CY_CPU_DTCM);  
    
    // DTCMCR EN/RMW/RETEN enabled to access DTCM 
    CY_SET_REG32(0xE000EF94, 0x2F);

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);
    Cy_GPIO_Write(USER_LED_PORT, USER_LED_PIN, 0);

    for(;;)
    {        
        // Only for measurements
        Cy_GPIO_Write(USER_LED_PORT, USER_LED_PIN, 1);
        
        // Set the DTCM pointer address
        #if (DTCM_RAM_ACCESS == DTCM_RAM_ACCESS_BYTE)
            ptrTCM = (volatile uint8_t *)DTCM_SELF_RAM_ADDR; 
        #else // (DTCM_RAM_ACCESS == DTCM_RAM_ACCESS_WORD)
            ptrTCM = (volatile uint32_t *)DTCM_SELF_RAM_ADDR;    
        #endif       
            
        // Write bytes/words
        for(i = 0; i < DTCM_RAM_ACCESS_SIZE; i++)
        {
            #if (DTCM_RAM_ACCESS == DTCM_RAM_ACCESS_BYTE)
                CY_SET_REG8(ptrTCM++, writeData[i%2]); 
            #else // (DTCM_RAM_ACCESS == DTCM_RAM_ACCESS_WORD)
                CY_SET_REG32(ptrTCM++, writeData[i%2]);    
            #endif 
        }       
        
        // Only for measurements
        Cy_GPIO_Write(USER_LED_PORT, USER_LED_PIN, 0);
        
        // Resetting the DTCM pointer address
        #if (DTCM_RAM_ACCESS == DTCM_RAM_ACCESS_BYTE)
            ptrTCM = (volatile uint8_t *)DTCM_SELF_RAM_ADDR; 
        #else // (DTCM_RAM_ACCESS == DTCM_RAM_ACCESS_WORD)
            ptrTCM = (volatile uint32_t *)DTCM_SELF_RAM_ADDR;    
        #endif        
        
        // Read back those bytes/words written previously
        for(i = 0; i < DTCM_RAM_ACCESS_SIZE; i++)
        {
            #if (DTCM_RAM_ACCESS == DTCM_RAM_ACCESS_BYTE)
                testRead = CY_GET_REG8(ptrTCM++); 
            #else // (DTCM_RAM_ACCESS == DTCM_RAM_ACCESS_WORD)
                testRead = CY_GET_REG32(ptrTCM++);    
            #endif
        }
    }
}



/* [] END OF FILE */
