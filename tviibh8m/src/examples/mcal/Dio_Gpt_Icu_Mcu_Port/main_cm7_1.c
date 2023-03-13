/***************************************************************************//**
* \file main_cm7_1.c
*
* \brief
* Main file for CM7 #1
*
********************************************************************************
* \copyright
* Copyright 2016-2019, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"

#include "mcal_project.h"


int main(void)
{
    __enable_irq();

    SystemInit();


    for(;;)
    {
    }
}


/* [] END OF FILE */
