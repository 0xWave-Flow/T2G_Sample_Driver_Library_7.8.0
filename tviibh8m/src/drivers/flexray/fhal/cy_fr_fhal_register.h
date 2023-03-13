/***************************************************************************//**
* \file cy_fr_fhal_register.h
* \version 1.0
*
* \brief
* Defines the register addresses
*
* In this file all register addresses and Bit masks defined.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif


#ifndef __CY_FR_FHAL_REGISTER_H__
#define __CY_FR_FHAL_REGISTER_H__

/**
* \addtogroup group_fru_macros
* \{
*/


/*      Register Name            Address */
#define CY_FR_REG_CTL            (0x0000)
#define CY_FR_REG_DMA_CTL        (0x0004)
#define CY_FR_REG_TEST1          (0x0010)
#define CY_FR_REG_TEST2          (0x0014)
#define CY_FR_REG_LCK            (0x001C)
#define CY_FR_REG_EIR            (0x0020)
#define CY_FR_REG_SIR            (0x0024)
#define CY_FR_REG_EILS           (0x0028)
#define CY_FR_REG_SILS           (0x002C)
#define CY_FR_REG_EIES           (0x0030)
#define CY_FR_REG_EIER           (0x0034)
#define CY_FR_REG_SIES           (0x0038)
#define CY_FR_REG_SIER           (0x003C)
#define CY_FR_REG_ILE            (0x0040)
#define CY_FR_REG_T0C            (0x0044)
#define CY_FR_REG_T1C            (0x0048)
#define CY_FR_REG_STPW1          (0x004C)
#define CY_FR_REG_STPW2          (0x0050)
#define CY_FR_REG_SUCC1          (0x0080)
#define CY_FR_REG_SUCC2          (0x0084)
#define CY_FR_REG_SUCC3          (0x0088)
#define CY_FR_REG_NEMC           (0x008C)
#define CY_FR_REG_PRTC1          (0x0090)
#define CY_FR_REG_PRTC2          (0x0094)
#define CY_FR_REG_MHDC           (0x0098)
#define CY_FR_REG_GTUC1          (0x00A0)
#define CY_FR_REG_GTUC2          (0x00A4)
#define CY_FR_REG_GTUC3          (0x00A8)
#define CY_FR_REG_GTUC4          (0x00AC)
#define CY_FR_REG_GTUC5          (0x00B0)
#define CY_FR_REG_GTUC6          (0x00B4)
#define CY_FR_REG_GTUC7          (0x00B8)
#define CY_FR_REG_GTUC8          (0x00BC)
#define CY_FR_REG_GTUC9          (0x00C0)
#define CY_FR_REG_GTUC10         (0x00C4)
#define CY_FR_REG_GTUC11         (0x00C8)
#define CY_FR_REG_CCSV           (0x0100)
#define CY_FR_REG_CCEV           (0x0104)
#define CY_FR_REG_SCV            (0x0110)
#define CY_FR_REG_MTCCV          (0x0114)
#define CY_FR_REG_RCV            (0x0118)
#define CY_FR_REG_OCV            (0x011C)
#define CY_FR_REG_SFS            (0x0120)
#define CY_FR_REG_SWNIT          (0x0124)
#define CY_FR_REG_ACS            (0x0128)
#define CY_FR_REG_ESID           (0x0130)
#define CY_FR_REG_OSID           (0x0170)
#define CY_FR_REG_NMV1           (0x01B0)
#define CY_FR_REG_NMV2           (0x01B4)
#define CY_FR_REG_NMV3           (0x01B8)
#define CY_FR_REG_MRC            (0x0300)
#define CY_FR_REG_FRF            (0x0304)
#define CY_FR_REG_FRFM           (0x0308)
#define CY_FR_REG_FCL            (0x030C)
#define CY_FR_REG_MHDS           (0x0310)
#define CY_FR_REG_LDTS           (0x0314)
#define CY_FR_REG_FSR            (0x0318)
#define CY_FR_REG_MHDF           (0x031C)
#define CY_FR_REG_TXRQ1          (0x0320)
#define CY_FR_REG_TXRQ2          (0x0324)
#define CY_FR_REG_TXRQ3          (0x0328)
#define CY_FR_REG_TXRQ4          (0x032C)
#define CY_FR_REG_NDAT1          (0x0330)
#define CY_FR_REG_NDAT2          (0x0334)
#define CY_FR_REG_NDAT3          (0x0338)
#define CY_FR_REG_NDAT4          (0x033C)
#define CY_FR_REG_MBSC1          (0x0340)
#define CY_FR_REG_MBSC2          (0x0344)
#define CY_FR_REG_MBSC3          (0x0348)
#define CY_FR_REG_MBSC4          (0x034C)
#define CY_FR_REG_CREL           (0x03F0)
#define CY_FR_REG_ENDN           (0x03F4)
#define CY_FR_REG_WRDS           (0x0400)
#define CY_FR_REG_WRHS1          (0x0500)
#define CY_FR_REG_WRHS2          (0x0504)
#define CY_FR_REG_WRHS3          (0x0508)
#define CY_FR_REG_IBCM           (0x0510)
#define CY_FR_REG_IBCR           (0x0514)
#define CY_FR_REG_RDDS           (0x0600)
#define CY_FR_REG_RDHS1          (0x0700)
#define CY_FR_REG_RDHS2          (0x0704)
#define CY_FR_REG_RDHS3          (0x0708)
#define CY_FR_REG_MBS            (0x070C)
#define CY_FR_REG_OBCM           (0x0710)
#define CY_FR_REG_OBCR           (0x0714)
#define CY_FR_REG_WRHS1_MIR2     (0x0BF0)
#define CY_FR_REG_WRHS2_MIR2     (0x0BF4)
#define CY_FR_REG_WRHS3_MIR2     (0x0BF8)
#define CY_FR_REG_WRDS_MIR       (0x0C00)
#define CY_FR_REG_WRHS1_MIR      (0x0D00)
#define CY_FR_REG_WRHS2_MIR      (0x0D04)
#define CY_FR_REG_WRHS3_MIR      (0x0D08)
#define CY_FR_REG_IBCM_MIR       (0x0D10)
#define CY_FR_REG_IBCR_MIR       (0x0D14)
#define CY_FR_REG_RDHS1_MIR2     (0x0DF0)
#define CY_FR_REG_RDHS2_MIR2     (0x0DF4)
#define CY_FR_REG_RDHS3_MIR2     (0x0DF8)
#define CY_FR_REG_MBS_MIR2       (0x0DFC)
#define CY_FR_REG_RDDS_MIR       (0x0E00)
#define CY_FR_REG_RDHS1_MIR      (0x0F00)
#define CY_FR_REG_RDHS2_MIR      (0x0F04)
#define CY_FR_REG_RDHS3_MIR      (0x0F08)
#define CY_FR_REG_MBS_MIR        (0x0F0C)
#define CY_FR_REG_OBCM_MIR       (0x0F10)
#define CY_FR_REG_OBCR_MIR       (0x0F14)

/** \} group_fru_macros */

/*---------------------------------------------------------------------------------*/
#endif  /* #ifndef __CY_FR_FHAL_REGISTER_H__ */
/*---------------------------------------------------------------------------------*/

#if defined(__cplusplus)
}
#endif

/* [] END OF FILE */


