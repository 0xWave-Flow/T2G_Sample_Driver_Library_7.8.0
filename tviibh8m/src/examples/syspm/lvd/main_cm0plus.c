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


/**********************************************************************
 * 1. Please connect the Bench power supply to VDDD signal.
 *      For 176 CPU Board   :   J6_1
 *      For 320 CPU Board   :   J17_2
 * 2. Make sure provide current controlled voltage around 5V and 20mA.
 * 3. Slowly decrease the VDDD voltage
 *      VDDD below 4V       : USER LED1 will glow
 *      VDDD below 3.6V     : USER LED2 will glow
 *********************************************************************/


#define USER_LED1_PORT    CY_CB_USER_LED1_PORT
#define USER_LED1_PIN     CY_CB_USER_LED1_PIN
#define USER_LED1_PIN_MUX CY_CB_USER_LED1_PIN_MUX

#define USER_LED2_PORT    CY_CB_USER_LED2_PORT
#define USER_LED2_PIN     CY_CB_USER_LED2_PIN
#define USER_LED2_PIN_MUX CY_CB_USER_LED2_PIN_MUX


cy_stc_gpio_pin_config_t user_led_port_pin_cfg =
{
    .outVal    = 0u,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = USER_LED1_PIN_MUX,
    .intEdge   = 0u,
    .intMask   = 0u,
    .vtrip     = 0u,
    .slewRate  = 0u,
    .driveSel  = 0u,
};


int main(void)
{
    cy_en_lvd_status_t status;

    SystemInit();

    __enable_irq(); /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Cy_GPIO_Pin_Init(USER_LED1_PORT, USER_LED1_PIN, &user_led_port_pin_cfg);
    user_led_port_pin_cfg.hsiom = USER_LED2_PIN_MUX;
    Cy_GPIO_Pin_Init(USER_LED2_PORT, USER_LED2_PIN, &user_led_port_pin_cfg); 

    /*********************************************************************/
    /*****                OVD settings                               *****/
    /*********************************************************************/

    // LVD Action Select
    Cy_LVD_SetActionSelect(CY_LVD_TYPE_1, CY_LVD_ACTION_FAULT);
    // Set the VDDD threshold select for OVD
    Cy_LVD_SetThreshold(CY_LVD_TYPE_1, CY_LVD_THRESHOLD_4_0_V);
    // Enable LVD on VDDD
    Cy_LVD_Enable(CY_LVD_TYPE_1);

    // LVD Action Select
    Cy_LVD_SetActionSelect(CY_LVD_TYPE_2, CY_LVD_ACTION_FAULT);
    // Set the VDDD threshold select for OVD
    Cy_LVD_SetThreshold(CY_LVD_TYPE_2, CY_LVD_THRESHOLD_3_6_V);
    // Enable LVD on VDDD
    Cy_LVD_Enable(CY_LVD_TYPE_2);

    /*********************************************************************/
    /*****                OVD status Check                           *****/
    /*********************************************************************/
    for(;;)
    {
        status = Cy_LVD_GetStatus(CY_LVD_TYPE_1);
        if(status == CY_LVD_STATUS_BELOW)
        {
            Cy_GPIO_Write(USER_LED1_PORT, USER_LED1_PIN, 1ul);
        }
        else
        {
            Cy_GPIO_Write(USER_LED1_PORT, USER_LED1_PIN, 0ul);
        }

        status = Cy_LVD_GetStatus(CY_LVD_TYPE_2);
        if(status == CY_LVD_STATUS_BELOW)
        {
            Cy_GPIO_Write(USER_LED2_PORT, USER_LED2_PIN, 1ul);
        }
        else
        {
            Cy_GPIO_Write(USER_LED2_PORT, USER_LED2_PIN, 0ul);
        }
    }
}


/* [] END OF FILE */
