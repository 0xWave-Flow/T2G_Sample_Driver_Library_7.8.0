/***************************************************************************//**
* \file main_cm7_0.c
*
* \brief
* Main file for CM7 #0
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
#include <stdio.h>


int main(void)
{
    __enable_irq();

    SystemInit();

    Cy_SysInt_SetSystemIrqVector((cy_en_intr_t)MCAL_PROJECT_TIMER_16BIT_1KHZ_GPT_IRQ_NUMBER, MCAL_PROJECT_TIMER_16BIT_1KHZ_GPT_IRQ_HANDLER);
    Cy_SysInt_SetSystemIrqVector((cy_en_intr_t)MCAL_PROJECT_TIMER_32BIT_1MHZ_GPT_IRQ_NUMBER, MCAL_PROJECT_TIMER_32BIT_1MHZ_GPT_IRQ_HANDLER);
    Cy_SysInt_SetSystemIrqVector((cy_en_intr_t)MCAL_PROJECT_CB_BUTTON_1_ICU_IRQ_NUMBER,      MCAL_PROJECT_CB_BUTTON_1_ICU_IRQ_HANDLER);
    #if defined( CYT4BF8CDE )
    Cy_SysInt_SetSystemIrqVector((cy_en_intr_t)MCAL_PROJECT_CB_BUTTON_2_ICU_IRQ_NUMBER,      MCAL_PROJECT_CB_BUTTON_2_ICU_IRQ_HANDLER);
    Cy_SysInt_SetSystemIrqVector((cy_en_intr_t)MCAL_PROJECT_CB_BUTTON_3_ICU_IRQ_NUMBER,      MCAL_PROJECT_CB_BUTTON_3_ICU_IRQ_HANDLER);
    #endif
    
    Cy_SysInt_InitIRQ( &(cy_stc_sysint_irq_t) { .sysIntSrc = (cy_en_intr_t)MCAL_PROJECT_TIMER_16BIT_1KHZ_GPT_IRQ_NUMBER, .intIdx = CPUIntIdx2_IRQn, .isEnabled = true } );
    Cy_SysInt_InitIRQ( &(cy_stc_sysint_irq_t) { .sysIntSrc = (cy_en_intr_t)MCAL_PROJECT_TIMER_32BIT_1MHZ_GPT_IRQ_NUMBER, .intIdx = CPUIntIdx2_IRQn, .isEnabled = true } );
    Cy_SysInt_InitIRQ( &(cy_stc_sysint_irq_t) { .sysIntSrc = (cy_en_intr_t)MCAL_PROJECT_CB_BUTTON_1_ICU_IRQ_NUMBER,      .intIdx = CPUIntIdx3_IRQn, .isEnabled = true } );
    #if defined( CYT4BF8CDE )
    Cy_SysInt_InitIRQ( &(cy_stc_sysint_irq_t) { .sysIntSrc = (cy_en_intr_t)MCAL_PROJECT_CB_BUTTON_2_ICU_IRQ_NUMBER,      .intIdx = CPUIntIdx3_IRQn, .isEnabled = true } );
    Cy_SysInt_InitIRQ( &(cy_stc_sysint_irq_t) { .sysIntSrc = (cy_en_intr_t)MCAL_PROJECT_CB_BUTTON_3_ICU_IRQ_NUMBER,      .intIdx = CPUIntIdx3_IRQn, .isEnabled = true } );
    #endif
    
    NVIC_EnableIRQ(CPUIntIdx2_IRQn);
    NVIC_EnableIRQ(CPUIntIdx3_IRQn);

    Mcu_Init(&McuConf_McuModuleConfiguration_McuModuleConfiguration_0);
    Mcu_SetMode(McuConf_McuModeSettingConf_McuModeSetting_ApplCore0_Active);
    Mcu_InitClock(McuConf_McuClockSettingConfig_McuClockSettingConfig_Active);
    while(Mcu_GetPllStatus() != MCU_PLL_LOCKED)
    {
    }
    Mcu_DistributePllClock();

    Port_Init(&PortConf_PortConfigSet_PortConfigSet_0);
    
    Dio_FlipChannel(DioConf_DioChannel_CB_LED_1);
    Dio_FlipChannel(DioConf_DioChannel_CB_LED_2);    

    Gpt_Init(&GptConf_GptChannelConfigSet_GptChannelConfigSet_0);
    Gpt_StartTimer(GptConf_GptChannelConfiguration_Timer_16bit_1kHz, 500 - 1); // Start timer with 500 ms interval
    Gpt_EnableNotification(GptConf_GptChannelConfiguration_Timer_16bit_1kHz);
    Gpt_StartTimer(GptConf_GptChannelConfiguration_Timer_32bit_1MHz, 1000 - 1); // Start timer with 1 ms interval
    Gpt_EnableNotification(GptConf_GptChannelConfiguration_Timer_32bit_1MHz);

    Icu_Init(&IcuConf_IcuConfigSet_IcuConfigSet_0);
    Icu_EnableEdgeDetection(IcuConf_IcuChannel_CB_BUTTON_1);
    Icu_EnableNotification(IcuConf_IcuChannel_CB_BUTTON_1);
    #if defined( CYT4BF8CDE )
    Icu_EnableEdgeDetection(IcuConf_IcuChannel_CB_BUTTON_2);
    Icu_EnableEdgeDetection(IcuConf_IcuChannel_CB_BUTTON_3);
    Icu_EnableNotification(IcuConf_IcuChannel_CB_BUTTON_2);
    Icu_EnableNotification(IcuConf_IcuChannel_CB_BUTTON_3);
    #endif

    #if defined( CYT4BF8CDE )
    Cy_Semihosting_Init(SCB4, 115200UL, NULL, false); // CY_USB_SCB_UART_TYPE from BSP header not correct for Rev 3.0 board in SDL 6.4.0
    #else
    Cy_Semihosting_Init(CY_USB_SCB_UART_TYPE, 115200UL, NULL, false);
    #endif
    printf("Cypress Traveo II MCAL SW Framework!\n");

    for(;;)
    {
    }
}


void Timer_16bit_1kHz_Notification(void)
{
    Dio_FlipChannel(DioConf_DioChannel_CB_LED_1);
}

void Timer_32bit_1MHz_Notification(void)
{
    __ASM("nop");
}

void CpuBoard_Button1_Notification(void)
{
    static bool bTimerNotificationIsEnabled = true;
    if(bTimerNotificationIsEnabled == false)
    {
        Gpt_EnableNotification(GptConf_GptChannelConfiguration_Timer_16bit_1kHz); // Start LED toggling
    }
    else
    {
        Gpt_DisableNotification(GptConf_GptChannelConfiguration_Timer_16bit_1kHz); // Stop LED toggling
    }
    bTimerNotificationIsEnabled = (bTimerNotificationIsEnabled == false) ? true : false; // toggle state
}

#if defined( CYT4BF8CDE )

void CpuBoard_Button2_Notification(void)
{
    __ASM("nop");
}

void CpuBoard_Button3_Notification(void)
{
    __ASM("nop");
}

#endif



/* [] END OF FILE */
