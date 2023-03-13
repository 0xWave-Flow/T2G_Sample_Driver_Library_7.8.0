/***************************************************************************//**
* \file cy_bargraph_definitions.h
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

#ifndef CY_BARGRAPH_DEFINITIONS_H
#define CY_BARGRAPH_DEFINITIONS_H

#include "cy_device_headers.h"

#define NUMBER_OF_BARGRAPH_LEDS    10

#define GET_NR_OF_ACTIVE_LEDS_FROM_PERCENT(percent)      ((percent) / (100UL / NUMBER_OF_BARGRAPH_LEDS))

#define SET_BARGRAPH_PERCENT(percent)   do {                                                                                            \
                                            uint8_t u8NrOfActiveLeds = GET_NR_OF_ACTIVE_LEDS_FROM_PERCENT(percent);                     \
                                            Cy_GPIO_Write(BARGRAPH_LED_A_PORT, BARGRAPH_LED_A_PIN, (u8NrOfActiveLeds >=  1) ? 1 : 0);   \
                                            Cy_GPIO_Write(BARGRAPH_LED_B_PORT, BARGRAPH_LED_B_PIN, (u8NrOfActiveLeds >=  2) ? 1 : 0);   \
                                            Cy_GPIO_Write(BARGRAPH_LED_C_PORT, BARGRAPH_LED_C_PIN, (u8NrOfActiveLeds >=  3) ? 1 : 0);   \
                                            Cy_GPIO_Write(BARGRAPH_LED_D_PORT, BARGRAPH_LED_D_PIN, (u8NrOfActiveLeds >=  4) ? 1 : 0);   \
                                            Cy_GPIO_Write(BARGRAPH_LED_E_PORT, BARGRAPH_LED_E_PIN, (u8NrOfActiveLeds >=  5) ? 1 : 0);   \
                                            Cy_GPIO_Write(BARGRAPH_LED_F_PORT, BARGRAPH_LED_F_PIN, (u8NrOfActiveLeds >=  6) ? 1 : 0);   \
                                            Cy_GPIO_Write(BARGRAPH_LED_G_PORT, BARGRAPH_LED_G_PIN, (u8NrOfActiveLeds >=  7) ? 1 : 0);   \
                                            Cy_GPIO_Write(BARGRAPH_LED_H_PORT, BARGRAPH_LED_H_PIN, (u8NrOfActiveLeds >=  8) ? 1 : 0);   \
                                            Cy_GPIO_Write(BARGRAPH_LED_I_PORT, BARGRAPH_LED_I_PIN, (u8NrOfActiveLeds >=  9) ? 1 : 0);   \
                                            Cy_GPIO_Write(BARGRAPH_LED_J_PORT, BARGRAPH_LED_J_PIN, (u8NrOfActiveLeds >= 10) ? 1 : 0);   \
                                        } while(0)


// The LEDs on the base board shall serve as a bargraph consisting of 10 LEDs
// Since the LED numeration does not math their physical order on the PCB,
// the definitions are remapped from the unsorted 0-9 to the sorted A-J

#define BARGRAPH_LED_A_PORT           CY_LED3_PORT
#define BARGRAPH_LED_A_PIN            CY_LED3_PIN
#define BARGRAPH_LED_A_PIN_MUX        CY_LED3_PIN_MUX

#define BARGRAPH_LED_B_PORT           CY_LED2_PORT
#define BARGRAPH_LED_B_PIN            CY_LED2_PIN
#define BARGRAPH_LED_B_PIN_MUX        CY_LED2_PIN_MUX

#define BARGRAPH_LED_C_PORT           CY_LED1_PORT
#define BARGRAPH_LED_C_PIN            CY_LED1_PIN
#define BARGRAPH_LED_C_PIN_MUX        CY_LED1_PIN_MUX

#define BARGRAPH_LED_D_PORT           CY_LED0_PORT
#define BARGRAPH_LED_D_PIN            CY_LED0_PIN
#define BARGRAPH_LED_D_PIN_MUX        CY_LED0_PIN_MUX

#define BARGRAPH_LED_E_PORT           CY_LED7_PORT
#define BARGRAPH_LED_E_PIN            CY_LED7_PIN
#define BARGRAPH_LED_E_PIN_MUX        CY_LED7_PIN_MUX

#define BARGRAPH_LED_F_PORT           CY_LED6_PORT
#define BARGRAPH_LED_F_PIN            CY_LED6_PIN
#define BARGRAPH_LED_F_PIN_MUX        CY_LED6_PIN_MUX

#define BARGRAPH_LED_G_PORT           CY_LED5_PORT
#define BARGRAPH_LED_G_PIN            CY_LED5_PIN
#define BARGRAPH_LED_G_PIN_MUX        CY_LED5_PIN_MUX

#define BARGRAPH_LED_H_PORT           CY_LED4_PORT
#define BARGRAPH_LED_H_PIN            CY_LED4_PIN
#define BARGRAPH_LED_H_PIN_MUX        CY_LED4_PIN_MUX

#define BARGRAPH_LED_I_PORT           CY_LED9_PORT
#define BARGRAPH_LED_I_PIN            CY_LED9_PIN
#define BARGRAPH_LED_I_PIN_MUX        CY_LED9_PIN_MUX

#define BARGRAPH_LED_J_PORT           CY_LED8_PORT
#define BARGRAPH_LED_J_PIN            CY_LED8_PIN
#define BARGRAPH_LED_J_PIN_MUX        CY_LED8_PIN_MUX




#endif // CY_BARGRAPH_DEFINITIONS_H


#ifdef ENABLE_VARIABLE_DEFINITION

cy_stc_gpio_pin_config_t bargraph_led_a_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = BARGRAPH_LED_A_PIN_MUX,                           
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


cy_stc_gpio_pin_config_t bargraph_led_b_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = BARGRAPH_LED_B_PIN_MUX,                           
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


cy_stc_gpio_pin_config_t bargraph_led_c_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = BARGRAPH_LED_C_PIN_MUX,                           
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


cy_stc_gpio_pin_config_t bargraph_led_d_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = BARGRAPH_LED_D_PIN_MUX,                           
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


cy_stc_gpio_pin_config_t bargraph_led_e_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = BARGRAPH_LED_E_PIN_MUX,                           
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


cy_stc_gpio_pin_config_t bargraph_led_f_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = BARGRAPH_LED_F_PIN_MUX,                           
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


cy_stc_gpio_pin_config_t bargraph_led_g_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = BARGRAPH_LED_G_PIN_MUX,                           
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


cy_stc_gpio_pin_config_t bargraph_led_h_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = BARGRAPH_LED_H_PIN_MUX,                           
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


cy_stc_gpio_pin_config_t bargraph_led_i_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = BARGRAPH_LED_I_PIN_MUX,                           
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


cy_stc_gpio_pin_config_t bargraph_led_j_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = BARGRAPH_LED_J_PIN_MUX,                           
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




#endif // ENABLE_VARIABLE_DEFINITION


