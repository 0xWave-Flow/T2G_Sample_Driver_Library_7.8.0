/***************************************************************************//**
* \file mcal_notifications.h
*
* \brief
* Common file with declarations of MCAL notification functions.
* Must be added to the <MCAL_Driver>IncludeFile containers in Tresos in all drivers
* where notification functions are needed and used.
*
********************************************************************************
* \copyright
* Copyright 2016-2019, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef _MCAL_NOTIFICATIONS_H_
#define _MCAL_NOTIFICATIONS_H_

// Gpt
extern void Timer_16bit_1kHz_Notification(void);
extern void Timer_32bit_1MHz_Notification(void);

// Icu
extern void CpuBoard_Button1_Notification(void);
extern void CpuBoard_Button2_Notification(void);   // only in case of QFP-176
extern void CpuBoard_Button3_Notification(void);   // only in case of QFP-176


#endif /* _MCAL_NOTIFICATIONS_H_ */


/* [] END OF FILE */
