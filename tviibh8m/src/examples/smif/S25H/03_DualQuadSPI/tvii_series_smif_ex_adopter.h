/***************************************************************************//**
* \file tvii_series_smif_ex_adopter.h
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

#if !defined(TVII_SERIES_SMIF_EX_ADOPTER_H)
#define TVII_SERIES_SMIF_EX_ADOPTER_H

/************************************************************/
/***             User modifiable definitions              ***/
/************************************************************/
#define USED_SMIF_CHANNEL 0

/************************************************************/
/***                Checking Availability                 ***/
/************************************************************/
#if (USED_SMIF_CHANNEL == 0)
  // available
#elif (USED_SMIF_CHANNEL == 1)
  #if defined(CY_SMIF_CORE1_EXISTS)
    // available
  #else
    #error "Unsupported channel"
  #endif
#else
    #error "Unsupported channel"
#endif

/************************************************************/
/***          SMIF Channel 0 Related Definitions          ***/
/************************************************************/
#if   (USED_SMIF_CHANNEL == 0)

  #define SMIF_USED               CY_SMIF_DRV_SMIF0_CORE0

  #define SMIF_DEVICE0            CY_SMIF_DRV_SMIF0_CORE0_DEVICE0
  #define SMIF_DEVICE1            CY_SMIF_DRV_SMIF0_CORE0_DEVICE1

  #define SMIF_IRQ_NO             CY_SMIF_DRV_SMIF0_IRQN

  #define CY_SMIF_CLK_PORT        CY_SMIF0_CLK_PORT
  #define CY_SMIF_CLK_PIN         CY_SMIF0_CLK_PIN
  #define CY_SMIF_CLK_PIN_MUX     CY_SMIF0_CLK_PIN_MUX

#ifdef CY_SMIF0_CLK_INV_PORT
  #define CY_SMIF_CLK_INV_PORT    CY_SMIF0_CLK_INV_PORT
  #define CY_SMIF_CLK_INV_PIN     CY_SMIF0_CLK_INV_PIN
  #define CY_SMIF_CLK_INV_PIN_MUX CY_SMIF0_CLK_INV_PIN_MUX
#else
  #define CY_SMIF_CLK_INV_PORT    (NULL)         // dummy
  #define CY_SMIF_CLK_INV_PIN     0              // dummy
  #define CY_SMIF_CLK_INV_PIN_MUX HSIOM_SEL_GPIO // dummy
#endif

  #define CY_SMIF_RWDS_PORT       CY_SMIF0_RWDS_PORT
  #define CY_SMIF_RWDS_PIN        CY_SMIF0_RWDS_PIN
  #define CY_SMIF_RWDS_PIN_MUX    CY_SMIF0_RWDS_PIN_MUX

  #define CY_SMIF_SELECT0_PORT    CY_SMIF0_SELECT0_PORT
  #define CY_SMIF_SELECT0_PIN     CY_SMIF0_SELECT0_PIN
  #define CY_SMIF_SELECT0_PIN_MUX CY_SMIF0_SELECT0_PIN_MUX

  #define CY_SMIF_SELECT1_PORT    CY_SMIF0_SELECT1_PORT
  #define CY_SMIF_SELECT1_PIN     CY_SMIF0_SELECT1_PIN
  #define CY_SMIF_SELECT1_PIN_MUX CY_SMIF0_SELECT1_PIN_MUX

  #define CY_SMIF_DATA0_PORT      CY_SMIF0_DATA0_PORT
  #define CY_SMIF_DATA0_PIN       CY_SMIF0_DATA0_PIN
  #define CY_SMIF_DATA0_PIN_MUX   CY_SMIF0_DATA0_PIN_MUX

  #define CY_SMIF_DATA1_PORT      CY_SMIF0_DATA1_PORT
  #define CY_SMIF_DATA1_PIN       CY_SMIF0_DATA1_PIN
  #define CY_SMIF_DATA1_PIN_MUX   CY_SMIF0_DATA1_PIN_MUX

  #define CY_SMIF_DATA2_PORT      CY_SMIF0_DATA2_PORT
  #define CY_SMIF_DATA2_PIN       CY_SMIF0_DATA2_PIN
  #define CY_SMIF_DATA2_PIN_MUX   CY_SMIF0_DATA2_PIN_MUX

  #define CY_SMIF_DATA3_PORT      CY_SMIF0_DATA3_PORT
  #define CY_SMIF_DATA3_PIN       CY_SMIF0_DATA3_PIN
  #define CY_SMIF_DATA3_PIN_MUX   CY_SMIF0_DATA3_PIN_MUX

  #define CY_SMIF_DATA4_PORT      CY_SMIF0_DATA4_PORT
  #define CY_SMIF_DATA4_PIN       CY_SMIF0_DATA4_PIN
  #define CY_SMIF_DATA4_PIN_MUX   CY_SMIF0_DATA4_PIN_MUX

  #define CY_SMIF_DATA5_PORT      CY_SMIF0_DATA5_PORT
  #define CY_SMIF_DATA5_PIN       CY_SMIF0_DATA5_PIN
  #define CY_SMIF_DATA5_PIN_MUX   CY_SMIF0_DATA5_PIN_MUX

  #define CY_SMIF_DATA6_PORT      CY_SMIF0_DATA6_PORT
  #define CY_SMIF_DATA6_PIN       CY_SMIF0_DATA6_PIN
  #define CY_SMIF_DATA6_PIN_MUX   CY_SMIF0_DATA6_PIN_MUX

  #define CY_SMIF_DATA7_PORT      CY_SMIF0_DATA7_PORT
  #define CY_SMIF_DATA7_PIN       CY_SMIF0_DATA7_PIN
  #define CY_SMIF_DATA7_PIN_MUX   CY_SMIF0_DATA7_PIN_MUX

  #define CY_SMIF_QUAD_MEMORY     CY_SMIF0_QUAD_MEMORY

/************************************************************/
/***          SMIF Channel 1 Related Definitions          ***/
/************************************************************/
#elif (USED_SMIF_CHANNEL == 1)

  #define SMIF_USED               CY_SMIF_DRV_SMIF0_CORE1

  #define SMIF_DEVICE0            CY_SMIF_DRV_SMIF0_CORE1_DEVICE0
  #define SMIF_DEVICE1            CY_SMIF_DRV_SMIF0_CORE1_DEVICE1

  #define SMIF_IRQ_NO             CY_SMIF_DRV_SMIF1_IRQN

  #define CY_SMIF_CLK_PORT        CY_SMIF1_CLK_PORT
  #define CY_SMIF_CLK_PIN         CY_SMIF1_CLK_PIN
  #define CY_SMIF_CLK_PIN_MUX     CY_SMIF1_CLK_PIN_MUX

#ifdef CY_SMIF1_CLK_INV_PORT // inverted clock is not available on PSVP (and hence would not be defined in BSP header)
  #define CY_SMIF_CLK_INV_PORT    CY_SMIF1_CLK_INV_PORT
  #define CY_SMIF_CLK_INV_PIN     CY_SMIF1_CLK_INV_PIN
  #define CY_SMIF_CLK_INV_PIN_MUX CY_SMIF1_CLK_INV_PIN_MUX
#else
  #define CY_SMIF_CLK_INV_PORT    (NULL)         // dummy
  #define CY_SMIF_CLK_INV_PIN     0              // dummy
  #define CY_SMIF_CLK_INV_PIN_MUX HSIOM_SEL_GPIO // dummy
#endif

  #define CY_SMIF_RWDS_PORT       CY_SMIF1_RWDS_PORT
  #define CY_SMIF_RWDS_PIN        CY_SMIF1_RWDS_PIN
  #define CY_SMIF_RWDS_PIN_MUX    CY_SMIF1_RWDS_PIN_MUX

  #define CY_SMIF_SELECT0_PORT    CY_SMIF1_SELECT0_PORT
  #define CY_SMIF_SELECT0_PIN     CY_SMIF1_SELECT0_PIN
  #define CY_SMIF_SELECT0_PIN_MUX CY_SMIF1_SELECT0_PIN_MUX

  #define CY_SMIF_SELECT1_PORT    CY_SMIF1_SELECT1_PORT
  #define CY_SMIF_SELECT1_PIN     CY_SMIF1_SELECT1_PIN
  #define CY_SMIF_SELECT1_PIN_MUX CY_SMIF1_SELECT1_PIN_MUX

  #define CY_SMIF_DATA0_PORT      CY_SMIF1_DATA0_PORT
  #define CY_SMIF_DATA0_PIN       CY_SMIF1_DATA0_PIN
  #define CY_SMIF_DATA0_PIN_MUX   CY_SMIF1_DATA0_PIN_MUX

  #define CY_SMIF_DATA1_PORT      CY_SMIF1_DATA1_PORT
  #define CY_SMIF_DATA1_PIN       CY_SMIF1_DATA1_PIN
  #define CY_SMIF_DATA1_PIN_MUX   CY_SMIF1_DATA1_PIN_MUX

  #define CY_SMIF_DATA2_PORT      CY_SMIF1_DATA2_PORT
  #define CY_SMIF_DATA2_PIN       CY_SMIF1_DATA2_PIN
  #define CY_SMIF_DATA2_PIN_MUX   CY_SMIF1_DATA2_PIN_MUX

  #define CY_SMIF_DATA3_PORT      CY_SMIF1_DATA3_PORT
  #define CY_SMIF_DATA3_PIN       CY_SMIF1_DATA3_PIN
  #define CY_SMIF_DATA3_PIN_MUX   CY_SMIF1_DATA3_PIN_MUX

  #define CY_SMIF_DATA4_PORT      CY_SMIF1_DATA4_PORT
  #define CY_SMIF_DATA4_PIN       CY_SMIF1_DATA4_PIN
  #define CY_SMIF_DATA4_PIN_MUX   CY_SMIF1_DATA4_PIN_MUX

  #define CY_SMIF_DATA5_PORT      CY_SMIF1_DATA5_PORT
  #define CY_SMIF_DATA5_PIN       CY_SMIF1_DATA5_PIN
  #define CY_SMIF_DATA5_PIN_MUX   CY_SMIF1_DATA5_PIN_MUX

  #define CY_SMIF_DATA6_PORT      CY_SMIF1_DATA6_PORT
  #define CY_SMIF_DATA6_PIN       CY_SMIF1_DATA6_PIN
  #define CY_SMIF_DATA6_PIN_MUX   CY_SMIF1_DATA6_PIN_MUX

  #define CY_SMIF_DATA7_PORT      CY_SMIF1_DATA7_PORT
  #define CY_SMIF_DATA7_PIN       CY_SMIF1_DATA7_PIN
  #define CY_SMIF_DATA7_PIN_MUX   CY_SMIF1_DATA7_PIN_MUX

  #define CY_SMIF_QUAD_MEMORY     CY_SMIF1_QUAD_MEMORY

#endif

/************************************************************/
/***      TVII Series and Channel Related Definitions     ***/
/************************************************************/
#define TVII_B_H_4M   ( defined(CYT4BBBCEE) || defined(CYT4BBBCES) || defined(CYT4BB8CEE) || defined(CYT4BB8CES) || defined(CYT4BB7CEE) || defined(CYT4BB7CES) || defined(CYT4BB5CEE) || defined(CYT4BB5CES))
#define TVII_B_H_8M   ( defined(CYT4BFCCHE) || defined(CYT4BFCCHS) || defined(CYT4BFCCJE) || defined(CYT4BFCCJS) || defined(CYT4BF8CES) || defined(CYT4BF8CEE) || defined(CYT4BF8CDS) || defined(CYT4BF8CDE) || defined(CYT4A0100S) )
#define TVII_B_H_2D6M defined(CYT4DNDBHS)

#if (TVII_B_H_4M)

  #define SMIF_MPU_REG_NO  MPU_REG_SMIF0_DEV

  #define SMIF_HF_CLOCK  CY_SYSCLK_HFCLK_6 // have to confirm (YOTS)

  #define PLL_200M_0_PATH_NO              (3u)
  static void ChangePLLFrequency(uint32_t outputFreq)
  {
    #if (CY_USE_PSVP == 0u)
      cy_stc_pll_config_t pll200_0_Config = 
      {
          .inputFreq     = 16000000ul,
          .outputFreq    = outputFreq,    // target PLL output
          .lfMode        = 0u,            // VCO frequency is [200MHz, 400MHz]
          .outputMode    = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
      };

      CY_ASSERT(Cy_SysClk_HfClockSetSource(CY_SYSCLK_HFCLK_0, CY_SYSCLK_HFCLK_IN_CLKPATH5) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_PllDisable(PLL_200M_0_PATH_NO) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_ClkPathSetSource(PLL_200M_0_PATH_NO, CY_SYSCLK_CLKPATH_IN_ECO) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_PllConfigure(PLL_200M_0_PATH_NO , &pll200_0_Config) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_PllEnable(PLL_200M_0_PATH_NO, 10000ul) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_HfClockSetSource(CY_SYSCLK_HFCLK_0, CY_SYSCLK_HFCLK_IN_CLKPATH3) == CY_SYSCLK_SUCCESS);
    #endif
      return;
  }

#elif (TVII_B_H_8M)

  #define SMIF_MPU_REG_NO  MPU_REG_SMIF0_DEV

  #define SMIF_HF_CLOCK CY_SYSCLK_HFCLK_6

  #define PLL_200M_0_PATH_NO              (3u)
  static void ChangePLLFrequency(uint32_t outputFreq)
  {
    #if (CY_USE_PSVP == 0u)
      cy_stc_pll_config_t pll200_0_Config = 
      {
          .inputFreq     = 16000000ul,
          .outputFreq    = outputFreq,    // target PLL output
          .lfMode        = 0u,            // VCO frequency is [200MHz, 400MHz]
          .outputMode    = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
      };

      CY_ASSERT(Cy_SysClk_HfClockSetSource(CY_SYSCLK_HFCLK_0, CY_SYSCLK_HFCLK_IN_CLKPATH5) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_PllDisable(PLL_200M_0_PATH_NO) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_ClkPathSetSource(PLL_200M_0_PATH_NO, CY_SYSCLK_CLKPATH_IN_ECO) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_PllConfigure(PLL_200M_0_PATH_NO , &pll200_0_Config) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_PllEnable(PLL_200M_0_PATH_NO, 10000ul) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_HfClockSetSource(CY_SYSCLK_HFCLK_0, CY_SYSCLK_HFCLK_IN_CLKPATH3) == CY_SYSCLK_SUCCESS);
    #endif
      return;
  }

#else // TVII_B_H_2D6M

  #if (USED_SMIF_CHANNEL == 0)

    #define SMIF_MPU_REG_NO  MPU_REG_SMIF0_DEV

    #define SMIF_HF_CLOCK CY_SYSCLK_HFCLK_8

  #else // (USED_SMIF_CHANNEL == 1)

    #define SMIF_MPU_REG_NO  MPU_REG_SMIF1_DEV

    #define SMIF_HF_CLOCK CY_SYSCLK_HFCLK_9

  #endif

  #define PLL_400M_2_PATH_NO              (3u)
  static void ChangePLLFrequency(uint32_t outputFreq)
  {
    #if (CY_USE_PSVP == 0u)
      cy_stc_pll_400M_config_t pll400_0_Config = 
      {
          .inputFreq     = 16000000ul,
          .outputFreq    = outputFreq,    // target PLL output
          .outputMode    = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
          .fracEn        = false,
          .fracDitherEn  = false,
          .sscgEn        = true,
          .sscgDitherEn  = true,
          .sscgDepth     = CY_SYSCLK_SSCG_DEPTH_MINUS_2_0,
          .sscgRate      = CY_SYSCLK_SSCG_RATE_DIV_512,
      };

      CY_ASSERT(Cy_SysClk_Pll400MDisable(PLL_400M_2_PATH_NO) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_ClkPathSetSource(PLL_400M_2_PATH_NO, CY_SYSCLK_CLKPATH_IN_ECO) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_Pll400MConfigure(PLL_400M_2_PATH_NO , &pll400_0_Config) == CY_SYSCLK_SUCCESS);
      CY_ASSERT(Cy_SysClk_Pll400MEnable(PLL_400M_2_PATH_NO, 10000ul) == CY_SYSCLK_SUCCESS);
    #endif
      return;
  }

#endif


#endif // TVII_SERIES_SMIF_EX_ADOPTER_H
