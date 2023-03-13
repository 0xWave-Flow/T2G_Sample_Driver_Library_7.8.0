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

#if (CY_USE_PSVP == 1)
 #error "This example is not supposed to work in PSVP environment."
#endif

#define CY_SYSCLK_PATH_NUM (SRSS_NUM_CLKPATH-1u) /* SRSS_NUM_CLKPATH includes CLK_REF_HF */

typedef struct
{
    uint32_t wcoFreq;
    uint32_t refHfClock;
    uint32_t SourceFreqPath[CY_SYSCLK_PATH_NUM];

    uint32_t OutFreqPath[CY_SYSCLK_PATH_NUM];

    uint32_t fllSourceFreq;
    uint32_t pll400M0SourceFreq;
    uint32_t pll400M1SourceFreq;
    uint32_t pll200M0SourceFreq;
    uint32_t pll200M1SourceFreq;

    uint32_t fllFreq;
    uint32_t pll400M0Freq;
    uint32_t pll400M1Freq;
    uint32_t pll200M0Freq;
    uint32_t pll200M1Freq;

    uint32_t ClockFreqHf[CY_SYSCLK_HFCLK_NUM];

    uint32_t WDTClockFreq;
    uint32_t RTCClockFreq;
    uint32_t ECOPrescaleClockFreq;
    uint32_t LfClockFreq;
    uint32_t MTWDTClockFreq;
    uint32_t TimerClockFreq;
    uint32_t Fast0ClockFreq;
    uint32_t Fast1ClockFreq;
    uint32_t PeriClockFreq;
    uint32_t MemClockFreq;
    uint32_t SlowClockFreq;
    uint32_t CoreClockFreq;
    uint32_t SysticClockFreq;

    uint32_t Gr5ClockFreq;
    uint32_t Gr6ClockFreq;
    uint32_t Gr9ClockFreq;

    uint32_t Gr3ClockFreq;
    uint32_t Gr4ClockFreq;
    uint32_t Gr8ClockFreq;

    uint32_t FlashInterfaceFreq;
    uint32_t Ram0FastFreq;
    uint32_t Ram0SlowFreq;
    uint32_t Ram1FastFreq;
    uint32_t Ram1SlowFreq;
    uint32_t Ram2FastFreq;
    uint32_t Ram2SlowFreq;
    uint32_t RomFastFreq;
    uint32_t RomSlowFreq;

} stc_frequency_of_clocks;

/*****************************************/
/**********     User Option    ***********/
/*****************************************/
#define PATH_SOURCE_CLOCK_TEST PATH_SOURCE_CLOCK_ECO

#define FLL_TARGET_FREQ           (100000000ul)
#define PLL400_0_TARGET_FREQ      (340000000ul)
#define PLL400_1_TARGET_FREQ      (196608000ul)
#define PLL200_0_TARGET_FREQ      (160000000ul)
#define PLL200_1_TARGET_FREQ      (80000000ul)
#define ECO_PRESCALER_TARGET_FREQ (1234567ul)

const cy_en_hf_clk_sources_t CLK_SOURCE_PATH_HF[CY_SYSCLK_HFCLK_NUM] =
{
    CY_SYSCLK_HFCLK_IN_CLKPATH3, /* PLL2: PLL200#0 */
    CY_SYSCLK_HFCLK_IN_CLKPATH1, /* PLL0: PLL400#0 */
    CY_SYSCLK_HFCLK_IN_CLKPATH4, /* PLL3: PLL200#1 */
    CY_SYSCLK_HFCLK_IN_CLKPATH5, /* No PLL/FLL (bypass) will be output */
    CY_SYSCLK_HFCLK_IN_CLKPATH3, /* PLL2: PLL200#0 */
    CY_SYSCLK_HFCLK_IN_CLKPATH2, /* PLL1: PLL400#1 */
    CY_SYSCLK_HFCLK_IN_CLKPATH0, /* FLL */
    CY_SYSCLK_HFCLK_IN_CLKPATH5, /* No PLL/FLL (bypass) */
};

const cy_en_hf_clk_dividers_t CLK_PRE_DIVIDER_HF[CY_SYSCLK_HFCLK_NUM] =
{
    CY_SYSCLK_HFCLK_DIVIDE_BY_2,
    CY_SYSCLK_HFCLK_NO_DIVIDE,
    CY_SYSCLK_HFCLK_NO_DIVIDE,
    CY_SYSCLK_HFCLK_NO_DIVIDE,
    CY_SYSCLK_HFCLK_DIVIDE_BY_4,
    CY_SYSCLK_HFCLK_NO_DIVIDE,
    CY_SYSCLK_HFCLK_NO_DIVIDE,
    CY_SYSCLK_HFCLK_NO_DIVIDE,
};

#define FLASH_WAIT_STATE     (1ul) // divided by 2
#define RAM0_FAST_WAIT_STATE (0ul) // divided by 1
#define RAM0_SLOW_WAIT_STATE (1ul) // divided by 2
#define RAM1_FAST_WAIT_STATE (0ul) // divided by 1
#define RAM1_SLOW_WAIT_STATE (1ul) // divided by 2
#define RAM2_FAST_WAIT_STATE (0ul) // divided by 1
#define RAM2_SLOW_WAIT_STATE (1ul) // divided by 2
#define ROM_FAST_WAIT_STATE  (0ul) // divided by 1
#define ROM_SLOW_WAIT_STATE  (1ul) // divided by 2

#define CLK_RTC_SOURCE     (CY_SYSCLK_BAK_IN_ILO)              // cy_en_clkbak_input_sources_t
#define CLK_LF_SOURCE      (CY_SYSCLK_LFCLK_IN_WCO)            // cy_en_lfclk_input_sources_t
#define CLK_TIMER_SOURCE   (CY_SYSCLK_CLKTIMER_IN_HF0_DIV8)    // cy_en_clktimer_input_sources_t
#define CLK_SYSTICK_SOURCE (CY_SYSTICK_CLOCK_SOURCE_CLK_TIMER) // cy_en_systick_clock_source_t

#define CLK_FAST0_DIVIDER (3.5)  // divided by 4.5
#define CLK_FAST1_DIVIDER (4.5)  // divided by 5.5
#define CLK_MEM_DIVIDER   (0ul)  // divided by 2
#define CLK_SLOW_DIVIDER  (2ul)  // divided by 3
#define CLK_PERI_DIVIDER  (6ul)  // divided by 7
#define CLK_GR5_DIVIDER   (7ul)  // divided by 8
#define CLK_GR6_DIVIDER   (8ul)  // divided by 9
#define CLK_GR9_DIVIDER   (9ul)  // divided by 10
#define CLK_GR3_DIVIDER   (10ul) // divided by 11
#define CLK_GR4_DIVIDER   (11ul) // divided by 12
#define CLK_GR8_DIVIDER   (12ul) // divided by 13
#define CLK_TIMER_DIVIDER (4ul)  // divided by 5

/** Wait time definition **/
#define WAIT_FOR_STABILIZATION (10000ul)

/*****************************************/
/**********   Constant #define ***********/
/*****************************************/

#define PATH_SOURCE_CLOCK_ECO      (0ul)
#define PATH_SOURCE_CLOCK_IMO      (1ul)

#define CLK_FREQ_WCO       (   32768ul)
#define CLK_FREQ_ILO0      (   32768ul)
#define CLK_FREQ_ILO1      (   32768ul)
#define CLK_FREQ_ECO       (16000000ul)
#define CLK_FREQ_IMO       ( 8000000ul)

#if PATH_SOURCE_CLOCK_TEST == PATH_SOURCE_CLOCK_ECO
  #define PATH_SOURCE_CLOCK_FREQ CLK_FREQ_ECO
  #define PATH_SOURCE_CLOCK_IN   CY_SYSCLK_CLKPATH_IN_ECO
  #define PATH_SOURCE_REF_CLOCK   CY_SYSCLK_CLKREF_IN_ECO
#elif PATH_SOURCE_CLOCK_TEST == PATH_SOURCE_CLOCK_IMO
  #define PATH_SOURCE_CLOCK_FREQ CLK_FREQ_IMO
  #define PATH_SOURCE_CLOCK_IN   CY_SYSCLK_CLKPATH_IN_IMO
  #define PATH_SOURCE_REF_CLOCK   CY_SYSCLK_CLKREF_IN_IMO
#else
  #error "Option not supported"
#endif

#define FLL_PATH_NO          (0ul)
#define PLL_400M_0_PATH_NO   (1ul)
#define PLL_400M_1_PATH_NO   (2ul)
#define PLL_200M_0_PATH_NO   (3ul)
#define PLL_200M_1_PATH_NO   (4ul)
#define BYPASSED_PATH_NO     (5ul)

/** ECO parameters **/
#define SUM_LOAD_SHUNT_CAP_IN_PF (17ul)

#if CLK_FREQ_ECO   < 16000000ul
#define ESR_IN_OHM            (250ul)
#elif CLK_FREQ_ECO < 20000000ul
#define ESR_IN_OHM            (150ul)
#elif CLK_FREQ_ECO < 30000000ul
#define ESR_IN_OHM            (100ul)
#else
#define ESR_IN_OHM            (70ul)
#endif

#define MIN_NEG_RESISTANCE     (5 * ESR_IN_OHM)
#define MAX_DRIVE_LEVEL_IN_UW  (100ul)

/*****************************************/
/*********** Global Variables ************/
/*****************************************/
/*** Parameters for Clock Configuration ***/
cy_stc_base_clk_freq_t g_baseFreqInfo = 
{
    .clk_imo_freq  = CLK_FREQ_IMO, //CY_CLK_IMO_FREQ_HZ,
    .clk_ext_freq  = 0ul,
    .clk_eco_freq  = CLK_FREQ_ECO,
    .clk_ilo0_freq = CLK_FREQ_ILO0,
    .clk_ilo1_freq = CLK_FREQ_ILO1,
    .clk_wco_freq  = CLK_FREQ_WCO,
};

cy_stc_pll_config_t g_pll200_0_Config = 
{
    .inputFreq  = PATH_SOURCE_CLOCK_FREQ,       // ECO: 16MHz
    .outputFreq = PLL200_0_TARGET_FREQ,         // target PLL output
    .lfMode     = false,                        // VCO frequency is [200MHz, 400MHz]
    .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
};

cy_stc_pll_config_t g_pll200_1_Config = 
{
    .inputFreq  = PATH_SOURCE_CLOCK_FREQ,       // ECO: 16MHz
    .outputFreq = PLL200_1_TARGET_FREQ,         // target PLL output
    .lfMode     = false,                        // VCO frequency is [200MHz, 400MHz]
    .outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
};

cy_stc_pll_400M_config_t g_pll400_0_Config =
{
    .inputFreq    = PATH_SOURCE_CLOCK_FREQ,
    .outputFreq   = PLL400_0_TARGET_FREQ,
    .outputMode   = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
    .fracEn       = false,
    .fracDitherEn = false,
    .sscgEn       = true,
    .sscgDitherEn = true,
    .sscgDepth    = CY_SYSCLK_SSCG_DEPTH_MINUS_2_0,
    .sscgRate     = CY_SYSCLK_SSCG_RATE_DIV_512,
};

cy_stc_pll_400M_config_t g_pll400_1_Config =
{
    .inputFreq    = PATH_SOURCE_CLOCK_FREQ,
    .outputFreq   = PLL400_1_TARGET_FREQ,
    .outputMode   = CY_SYSCLK_FLLPLL_OUTPUT_AUTO,
    .fracEn       = true,
    .fracDitherEn = true,
    .sscgEn       = false,
    .sscgDitherEn = false,
    .sscgDepth    = CY_SYSCLK_SSCG_DEPTH_MINUS_2_0,
    .sscgRate     = CY_SYSCLK_SSCG_RATE_DIV_512,
};

/*** Parameters for Calculation and checking ***/
stc_frequency_of_clocks g_expectedFreq = { 0ul };
stc_frequency_of_clocks g_reportedFreq = { 0ul };

/******   Clock Measurement Utilities   ******/
typedef struct
{
    cy_en_meas_clks_t name;
    uint32_t measuredFreq;
} stc_clock_measure;

stc_clock_measure g_clockMeasuredInfo[] = 
{
    {.name = CY_SYSCLK_MEAS_CLK_ILO0,    .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_WCO,     .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_BAK,     .measuredFreq= 0ul}, // CLK_RTC
    {.name = CY_SYSCLK_MEAS_CLK_LFCLK,   .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_IMO,     .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_ILO1,    .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_ECO,     .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_ECO_PRE, .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_PATH0,   .measuredFreq= 0ul}, // frequency should be same as that of input in PSVP.
    {.name = CY_SYSCLK_MEAS_CLK_PATH1,   .measuredFreq= 0ul}, // frequency should be same as that of input in PSVP.
    {.name = CY_SYSCLK_MEAS_CLK_PATH2,   .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_PATH3,   .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_PATH4,   .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_PATH5,   .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_HFCLK0,  .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_HFCLK1,  .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_HFCLK2,  .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_HFCLK3,  .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_HFCLK4,  .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_HFCLK5,  .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_HFCLK6,  .measuredFreq= 0ul},
    {.name = CY_SYSCLK_MEAS_CLK_HFCLK7,  .measuredFreq= 0ul},
};
#define NUM_OF_MEASURED_CLOCK (sizeof(g_clockMeasuredInfo) / sizeof(stc_clock_measure))

/******   Clock Output Utilities   ******/
cy_stc_gpio_pin_config_t clkOutPortConfig =
{
    .outVal    = 0ul,
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,
    .hsiom     = CY_HF3_CLK_OUT_PIN_MUX,
    .intEdge   = 0ul,
    .intMask   = 0ul,
    .vtrip     = 0ul,
    .slewRate  = 0ul,
    .driveSel  = 0ul,
};

static void AllClockConfiguration(void);
static void MeasureClockFrequency(void);
static void CompareExpectedAndMeasured(void);
static void RecalucClockFrequencyValues(void);
static void CompareExpectedAndCaluclated(void);
static void PrepareExpectedClockFrequency(void);

int main(void)
{
 
    /* disable watchdog timer */
    Cy_WDT_Disable();

    /* Calculate Expected Clock Frequency at first */
    PrepareExpectedClockFrequency();

    /* Set clk_hf0 source IMO (8MHz) */
    CY_ASSERT(Cy_SysClk_ClkPathSetSource(BYPASSED_PATH_NO, CY_SYSCLK_CLKPATH_IN_IMO) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_HfClkEnable(CY_SYSCLK_HFCLK_0) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_HfClockSetSource(CY_SYSCLK_HFCLK_0, (cy_en_hf_clk_sources_t)BYPASSED_PATH_NO) == CY_SYSCLK_SUCCESS);

    /* Disable Fll */
    CY_ASSERT(Cy_SysClk_FllDisableSequence(WAIT_FOR_STABILIZATION) == CY_SYSCLK_SUCCESS);

    /* Disable Pll */
    CY_ASSERT(Cy_SysClk_Pll400MDisable(PLL_400M_0_PATH_NO) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_Pll400MDisable(PLL_400M_1_PATH_NO) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_PllDisable(PLL_200M_0_PATH_NO) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_PllDisable(PLL_200M_1_PATH_NO) == CY_SYSCLK_SUCCESS);

    /* Enable interrupt */
    __enable_irq();

    /* Set Clock Configuring registers */
    AllClockConfiguration();

    /* Measure clock frequencies using ECO and check */
    MeasureClockFrequency();
    CompareExpectedAndMeasured();

    /* Read register value and re-calculate the frequency and check */
    RecalucClockFrequencyValues();
    CompareExpectedAndCaluclated();

    /* Start output internal clock */
    Cy_GPIO_Pin_Init(CY_HF3_CLK_OUT_PORT, CY_HF3_CLK_OUT_PIN, &clkOutPortConfig);

    /* Please ensure output clock frequency using oscilloscope */
    for(;;);
}

static void AllClockConfiguration(void)
{
    /*********** Setting wait state for ROM **********/
    CPUSS->unROM_CTL.stcField.u2SLOW_WS = ROM_SLOW_WAIT_STATE;
    CPUSS->unROM_CTL.stcField.u2FAST_WS = ROM_FAST_WAIT_STATE;

    /*********** Setting wait state for RAM **********/
    CPUSS->unRAM0_CTL0.stcField.u2SLOW_WS = RAM0_SLOW_WAIT_STATE;
    CPUSS->unRAM0_CTL0.stcField.u2FAST_WS = RAM0_FAST_WAIT_STATE;

    CPUSS->unRAM1_CTL0.stcField.u2SLOW_WS = RAM1_SLOW_WAIT_STATE;
    CPUSS->unRAM1_CTL0.stcField.u2FAST_WS = RAM1_FAST_WAIT_STATE;

    CPUSS->unRAM2_CTL0.stcField.u2SLOW_WS = RAM2_SLOW_WAIT_STATE;
    CPUSS->unRAM2_CTL0.stcField.u2FAST_WS = RAM2_FAST_WAIT_STATE;
    /*********** Setting wait state for FLASH **********/
    FLASHC->unFLASH_CTL.stcField.u4WS = FLASH_WAIT_STATE;

    /*****  ECO setting ******/
    {
        cy_en_sysclk_status_t ecoStatus;
        ecoStatus = Cy_SysClk_EcoConfigureWithMinRneg(
                           CLK_FREQ_ECO,
                           SUM_LOAD_SHUNT_CAP_IN_PF,
                           ESR_IN_OHM,
                           MAX_DRIVE_LEVEL_IN_UW,
                           MIN_NEG_RESISTANCE
                           );
        CY_ASSERT(ecoStatus == CY_SYSCLK_SUCCESS);

        ecoStatus = Cy_SysClk_EcoEnable(WAIT_FOR_STABILIZATION);
        CY_ASSERT(ecoStatus == CY_SYSCLK_SUCCESS);
    }

    /***** ILO0 setting ******/
    {
        cy_en_sysclk_status_t ilo0Status;

        Cy_WDT_Unlock();
        ilo0Status = Cy_SysClk_Ilo0Enable();
        Cy_WDT_Lock();
        CY_ASSERT(ilo0Status == CY_SYSCLK_SUCCESS);
    }

    /***** ILO1 setting ******/
    {
        Cy_SysClk_Ilo1Enable();
    }

    /***** WCO setting ******/
    {
        cy_en_sysclk_status_t wcoStatus;
        wcoStatus = Cy_SysClk_WcoEnable(WAIT_FOR_STABILIZATION*10ul);
        CY_ASSERT(wcoStatus == CY_SYSCLK_SUCCESS);
    }

    /***** ECO prescaler setting ******/
    {
        cy_en_sysclk_status_t ecoPreStatus;

        ecoPreStatus = Cy_SysClk_SetEcoPrescale(CLK_FREQ_ECO, ECO_PRESCALER_TARGET_FREQ);
        CY_ASSERT(ecoPreStatus == CY_SYSCLK_SUCCESS);

        ecoPreStatus = Cy_SysClk_EcoPrescaleEnable(WAIT_FOR_STABILIZATION);
        CY_ASSERT(ecoPreStatus == CY_SYSCLK_SUCCESS);
    }

    /***** FLL(PATH0) source setting ******/
    {
        cy_en_sysclk_status_t fllStatus;

        fllStatus = Cy_SysClk_ClkPathSetSource(FLL_PATH_NO, PATH_SOURCE_CLOCK_IN);
        CY_ASSERT(fllStatus == CY_SYSCLK_SUCCESS);

        fllStatus = Cy_SysClk_FllConfigureStandard(PATH_SOURCE_CLOCK_FREQ, FLL_TARGET_FREQ, CY_SYSCLK_FLLPLL_OUTPUT_AUTO);
        CY_ASSERT(fllStatus == CY_SYSCLK_SUCCESS);

        fllStatus = Cy_SysClk_FllEnable(WAIT_FOR_STABILIZATION);
        CY_ASSERT((fllStatus == CY_SYSCLK_SUCCESS) || (fllStatus == CY_SYSCLK_TIMEOUT));

        SRSS->unCLK_FLL_STATUS.stcField.u1UNLOCK_OCCURRED = 1ul;
        /* Set the FLL bypass mode to 3 */
        SRSS->unCLK_FLL_CONFIG3.stcField.u2BYPASS_SEL = (uint32_t)CY_SYSCLK_FLLPLL_OUTPUT_OUTPUT;

        static cy_stc_fll_manual_config_t monitorFllConfig;
        Cy_SysClk_FllGetConfiguration(&monitorFllConfig);
    }

    /***** PLL400M#0(PATH1) source setting ******/
    {
        cy_en_sysclk_status_t status;
        status = Cy_SysClk_ClkPathSetSource(PLL_400M_0_PATH_NO, PATH_SOURCE_CLOCK_IN);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        status = Cy_SysClk_Pll400MConfigure(PLL_400M_0_PATH_NO, &g_pll400_0_Config);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        status = Cy_SysClk_Pll400MEnable(PLL_400M_0_PATH_NO, WAIT_FOR_STABILIZATION);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        static cy_stc_pll_400M_manual_config_t monitor_pll400_0_Config;
        status = Cy_SysClk_Pll400MGetConfiguration(PLL_400M_0_PATH_NO, &monitor_pll400_0_Config);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);
    }

    /***** PLL400M#1(PATH2) source setting ******/
    {
        cy_en_sysclk_status_t status;
        status = Cy_SysClk_ClkPathSetSource(PLL_400M_1_PATH_NO, PATH_SOURCE_CLOCK_IN);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        status = Cy_SysClk_Pll400MConfigure(PLL_400M_1_PATH_NO, &g_pll400_1_Config);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        status = Cy_SysClk_Pll400MEnable(PLL_400M_1_PATH_NO, WAIT_FOR_STABILIZATION);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        static cy_stc_pll_400M_manual_config_t monitor_pll400_1_Config;
        status = Cy_SysClk_Pll400MGetConfiguration(PLL_400M_1_PATH_NO, &monitor_pll400_1_Config);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);
    }


    /***** PLL200M#0(PATH3) source setting ******/
    {
        cy_en_sysclk_status_t status;
        status = Cy_SysClk_ClkPathSetSource(PLL_200M_0_PATH_NO, PATH_SOURCE_CLOCK_IN);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        status = Cy_SysClk_PllConfigure(PLL_200M_0_PATH_NO , &g_pll200_0_Config);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        status = Cy_SysClk_PllEnable(PLL_200M_0_PATH_NO, WAIT_FOR_STABILIZATION);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        static cy_stc_pll_manual_config_t monitor_pll200_0_Config;
        status = Cy_SysClk_PllGetConfiguration(PLL_200M_0_PATH_NO, &monitor_pll200_0_Config);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);
    }

    /***** PLL200M#1(PATH4) source setting ******/
    {
        cy_en_sysclk_status_t status;
        status = Cy_SysClk_ClkPathSetSource(PLL_200M_1_PATH_NO, PATH_SOURCE_CLOCK_IN);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        status = Cy_SysClk_PllConfigure(PLL_200M_1_PATH_NO , &g_pll200_1_Config);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        status = Cy_SysClk_PllEnable(PLL_200M_1_PATH_NO, WAIT_FOR_STABILIZATION);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        static cy_stc_pll_manual_config_t monitor_pll200_1_Config;
        status = Cy_SysClk_PllGetConfiguration(PLL_200M_1_PATH_NO, &monitor_pll200_1_Config);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);
    }

    /***** PATH5 source setting ******/
    {
        cy_en_sysclk_status_t path2Status;
        path2Status = Cy_SysClk_ClkPathSetSource(BYPASSED_PATH_NO, PATH_SOURCE_CLOCK_IN);
        CY_ASSERT(path2Status == CY_SYSCLK_SUCCESS);
    }

    /***** RTC source setting ******/
    {
        Cy_SysClk_ClkBakSetSource(CLK_RTC_SOURCE);
    }

    /***** CLK LF source setting ******/
    {
        Cy_WDT_Unlock();
        Cy_SysClk_LfClkSetSource(CLK_LF_SOURCE);
        Cy_WDT_Lock();
    }

    /***** CLK FAST 0 source setting ******/
    {
        // divided by (CLK_FAST0_DIVIDER + 1.0)
        uint32_t FixedPointDivider8_5 = (uint32_t)(CLK_FAST0_DIVIDER * 32.0);
        Cy_SysClk_Fast0ClkSetDivider(((FixedPointDivider8_5&0x00001FE0ul) >> 5ul), (FixedPointDivider8_5&0x0000001Ful)); // divided by 3.5
    }

    /***** CLK FAST 1 source setting ******/
    {
        // divided by (CLK_FAST1_DIVIDER + 1.0)
        uint32_t FixedPointDivider8_5 = (uint32_t)(CLK_FAST1_DIVIDER * 32.0);
        Cy_SysClk_Fast1ClkSetDivider(((FixedPointDivider8_5&0x00001FE0ul) >> 5ul), (FixedPointDivider8_5&0x0000001Ful)); // divided by 3.5
    }

    /***** CLK Peri source setting ******/
    {
        Cy_SysClk_PeriClkSetDivider(CLK_PERI_DIVIDER); // divided by (CLK_PERI_DIVIDER + 1)
    }

    /***** CLK MEM source setting ******/
    {
        Cy_SysClk_MemClkSetDivider(CLK_MEM_DIVIDER);
    }

    /***** CLK SLOW source setting ******/
    {
        Cy_SysClk_SlowClkSetDivider(CLK_SLOW_DIVIDER); // divided by (CLK_SLOW_DIVIDER + 1)
    }

    /*** Setting for all clk_hf        ***/
    for(cy_en_hfclk_t i_clkHfNo = CY_SYSCLK_HFCLK_0; i_clkHfNo < CY_SYSCLK_HFCLK_NUM; i_clkHfNo++)
    {
        cy_en_sysclk_status_t clkHf0status;
        clkHf0status = Cy_SysClk_HfClockSetDivider(i_clkHfNo, CLK_PRE_DIVIDER_HF[i_clkHfNo]);
        CY_ASSERT(clkHf0status == CY_SYSCLK_SUCCESS);

        clkHf0status = Cy_SysClk_HfClockSetSource(i_clkHfNo, CLK_SOURCE_PATH_HF[i_clkHfNo]);
        CY_ASSERT(clkHf0status == CY_SYSCLK_SUCCESS);

        clkHf0status = Cy_SysClk_HfClkEnable(i_clkHfNo);
        CY_ASSERT(clkHf0status == CY_SYSCLK_SUCCESS);
    }

    /***  Setting for clock timer        ***/
    Cy_SysClk_ClkTimerSetSource(CLK_TIMER_SOURCE);
    Cy_SysClk_ClkTimerSetDivider(CLK_TIMER_DIVIDER); // divided by (CLK_TIMER_DIVIDER + 1)
    Cy_SysClk_ClkTimerEnable();

    /***  Setting for Systick source clock        ***/
    Cy_SysTick_SetClockSource(CLK_SYSTICK_SOURCE);

    /***  CLK_REF_HF setting        ***/
    Cy_SysClk_ClkRefSetSource(PATH_SOURCE_REF_CLOCK);

    /***  Clock Group setting        ***/
    Cy_SysClk_SetClkGrDiv(5ul, CLK_GR5_DIVIDER);
    Cy_SysClk_SetClkGrDiv(6ul, CLK_GR6_DIVIDER);
    Cy_SysClk_SetClkGrDiv(9ul, CLK_GR9_DIVIDER);
    Cy_SysClk_SetClkGrDiv(3ul, CLK_GR3_DIVIDER);
    Cy_SysClk_SetClkGrDiv(4ul, CLK_GR4_DIVIDER);
    Cy_SysClk_SetClkGrDiv(8ul, CLK_GR8_DIVIDER);

    return;
}

static bool LooseCompare(uint32_t a, uint32_t b)
{
    double tolerate = 0.03;
    if( ((double)(a * (1.0 + tolerate)) >= (double)b) &&
        ((double)b >= (double)(a * (1.0 - tolerate))))
    {
        return true;
    }
    else
    {
        return false;
    }
}

static void MeasureClockFrequency(void)
{
    uint32_t counter1 = 40000ul; /* should take 0.0025s for measuring */

    if((SRSS->unCLK_ECO_STATUS.stcField.u1ECO_OK == 0ul)    ||
      (SRSS->unCLK_ECO_STATUS.stcField.u1ECO_READY == 0ul))
    {
        return; // ECO need to be ready.
    }

    for(uint32_t mesuredClock_i = 0ul; mesuredClock_i < NUM_OF_MEASURED_CLOCK; mesuredClock_i++)
    {
        cy_en_sysclk_status_t status;
        status = Cy_SysClk_StartClkMeasurementCounters(CY_SYSCLK_MEAS_CLK_ECO,      /* reference clock */
                                                       counter1,
                                                       g_clockMeasuredInfo[mesuredClock_i].name);  /* clock to be measured */
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);

        while(Cy_SysClk_ClkMeasurementCountersDone() == false);

        status = Cy_SysClk_ClkMeasurementCountersGetFreq(&g_clockMeasuredInfo[mesuredClock_i].measuredFreq, CLK_FREQ_ECO);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);
 
        /* clear measuring counter */
        status = Cy_SysClk_StartClkMeasurementCounters(CY_SYSCLK_MEAS_CLK_ECO, 
                                                       3ul,
                                                       CY_SYSCLK_MEAS_CLK_ECO);
        CY_ASSERT(status == CY_SYSCLK_SUCCESS);
        while(Cy_SysClk_ClkMeasurementCountersDone() == false);
    }

    return;
}

static void CompareExpectedAndMeasured(void)
{
    for(uint32_t i = 0ul; i < NUM_OF_MEASURED_CLOCK; i++)
    {
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_ILO0)
        {
            CY_ASSERT(LooseCompare(g_baseFreqInfo.clk_ilo0_freq, g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_WCO)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.wcoFreq, g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_BAK)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.RTCClockFreq, g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_LFCLK)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.LfClockFreq, g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_IMO)
        {
            CY_ASSERT(LooseCompare(g_baseFreqInfo.clk_imo_freq, g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_ILO1)
        {
            CY_ASSERT(LooseCompare(g_baseFreqInfo.clk_ilo1_freq, g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_ECO)
        {
            CY_ASSERT(LooseCompare(g_baseFreqInfo.clk_eco_freq, g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_ECO_PRE)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.ECOPrescaleClockFreq, g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_PATH0)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.OutFreqPath[0], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_PATH1)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.OutFreqPath[1], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_PATH2)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.OutFreqPath[2], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_PATH3)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.OutFreqPath[3], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_PATH4)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.OutFreqPath[4], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_PATH5)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.OutFreqPath[5], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_HFCLK0)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.ClockFreqHf[0], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_HFCLK1)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.ClockFreqHf[1], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_HFCLK2)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.ClockFreqHf[2], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_HFCLK3)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.ClockFreqHf[3], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_HFCLK4)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.ClockFreqHf[4], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_HFCLK5)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.ClockFreqHf[5], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_HFCLK6)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.ClockFreqHf[6], g_clockMeasuredInfo[i].measuredFreq));
        }
        if(g_clockMeasuredInfo[i].name == CY_SYSCLK_MEAS_CLK_HFCLK7)
        {
            CY_ASSERT(LooseCompare(g_expectedFreq.ClockFreqHf[7], g_clockMeasuredInfo[i].measuredFreq));
        }
    }
}

static void RecalucClockFrequencyValues(void)
{
    CY_ASSERT(Cy_SysClk_InitGetFreqParams(&g_baseFreqInfo) == CY_SYSCLK_SUCCESS);

    CY_ASSERT(Cy_SysClk_GetWcoFrequency(&g_reportedFreq.wcoFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetRefHfFrequency(&g_reportedFreq.refHfClock) == CY_SYSCLK_SUCCESS);
    for(uint32_t i = 0ul; i < CY_SYSCLK_PATH_NUM; i++)
    {
        CY_ASSERT(Cy_SysClk_GetPathSourceFrequency(i, &g_reportedFreq.SourceFreqPath[i]) == CY_SYSCLK_SUCCESS);
        CY_ASSERT(Cy_SysClk_GetPathFrequency(i, &g_reportedFreq.OutFreqPath[i]) == CY_SYSCLK_SUCCESS);
    }


    CY_ASSERT(Cy_SysClk_GetFllInputFrequency(&g_reportedFreq.fllSourceFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetPllInputFrequency(0ul, &g_reportedFreq.pll400M0SourceFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetPllInputFrequency(1ul, &g_reportedFreq.pll400M1SourceFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetPllInputFrequency(2ul, &g_reportedFreq.pll200M0SourceFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetPllInputFrequency(3ul, &g_reportedFreq.pll200M1SourceFreq) == CY_SYSCLK_SUCCESS);

    CY_ASSERT(Cy_SysClk_GetFllOutputFrequency(&g_reportedFreq.fllFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetPllOutputFrequency(0ul, &g_reportedFreq.pll400M0Freq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetPllOutputFrequency(1ul, &g_reportedFreq.pll400M1Freq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetPllOutputFrequency(2ul, &g_reportedFreq.pll200M0Freq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetPllOutputFrequency(3ul, &g_reportedFreq.pll200M1Freq) == CY_SYSCLK_SUCCESS);

    for(cy_en_hfclk_t i = CY_SYSCLK_HFCLK_0; i < CY_SYSCLK_HFCLK_NUM; i++)
    {
        CY_ASSERT(Cy_SysClk_GetHfClkFrequency(i, &g_reportedFreq.ClockFreqHf[(uint32_t)i]) == CY_SYSCLK_SUCCESS);
    }

    CY_ASSERT(Cy_SysClk_GetWDTClkFrequency(&g_reportedFreq.WDTClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetRTCClkFrequency(&g_reportedFreq.RTCClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetEcoPrescaleFrequency(&g_reportedFreq.ECOPrescaleClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetLfFrequency(&g_reportedFreq.LfClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetMTWDTFrequency(&g_reportedFreq.MTWDTClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetTimerFrequency(&g_reportedFreq.TimerClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetFast0Frequency(&g_reportedFreq.Fast0ClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetFast1Frequency(&g_reportedFreq.Fast1ClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetClkPeriFrequency(&g_reportedFreq.PeriClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetClkMemFrequency(&g_reportedFreq.MemClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetClkSlowFrequency(&g_reportedFreq.SlowClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetCoreFrequency(&g_reportedFreq.CoreClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetSystickFrequency(&g_reportedFreq.SysticClockFreq) == CY_SYSCLK_SUCCESS);

    CY_ASSERT(Cy_SysClk_GetGroupFrequency(5ul, &g_reportedFreq.Gr5ClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetGroupFrequency(6ul, &g_reportedFreq.Gr6ClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetGroupFrequency(9ul, &g_reportedFreq.Gr9ClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetGroupFrequency(3ul, &g_reportedFreq.Gr3ClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetGroupFrequency(4ul, &g_reportedFreq.Gr4ClockFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetGroupFrequency(8ul, &g_reportedFreq.Gr8ClockFreq) == CY_SYSCLK_SUCCESS);

    CY_ASSERT(Cy_SysClk_GetFlashInterfaceFrequency(&g_reportedFreq.FlashInterfaceFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetRamFastFrequency(0ul, &g_reportedFreq.Ram0FastFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetRamFastFrequency(1ul, &g_reportedFreq.Ram1FastFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetRamFastFrequency(2ul, &g_reportedFreq.Ram2FastFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetRamSlowFrequency(0ul, &g_reportedFreq.Ram0SlowFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetRamSlowFrequency(1ul, &g_reportedFreq.Ram1SlowFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetRamSlowFrequency(2ul, &g_reportedFreq.Ram2SlowFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetRomFastFrequency(&g_reportedFreq.RomFastFreq) == CY_SYSCLK_SUCCESS);
    CY_ASSERT(Cy_SysClk_GetRomSlowFrequency(&g_reportedFreq.RomSlowFreq) == CY_SYSCLK_SUCCESS);

}

static void CompareExpectedAndCaluclated(void)
{
    CY_ASSERT(LooseCompare(g_reportedFreq.wcoFreq, g_expectedFreq.wcoFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.refHfClock, g_expectedFreq.refHfClock));
    for(uint32_t i = 0ul; i < CY_SYSCLK_PATH_NUM; i++)
    {
        CY_ASSERT(LooseCompare(g_reportedFreq.SourceFreqPath[i], g_expectedFreq.SourceFreqPath[i]));
        CY_ASSERT(LooseCompare(g_reportedFreq.OutFreqPath[i], g_expectedFreq.OutFreqPath[i]));
    }
    CY_ASSERT(LooseCompare(g_reportedFreq.fllSourceFreq, g_expectedFreq.fllSourceFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.pll400M0SourceFreq, g_expectedFreq.pll400M0SourceFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.pll400M1SourceFreq, g_expectedFreq.pll400M1SourceFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.pll200M0SourceFreq, g_expectedFreq.pll200M0SourceFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.pll200M1SourceFreq, g_expectedFreq.pll200M1SourceFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.fllFreq, g_expectedFreq.fllFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.pll400M0Freq, g_expectedFreq.pll400M0Freq));
    CY_ASSERT(LooseCompare(g_reportedFreq.pll400M1Freq, g_expectedFreq.pll400M1Freq));
    CY_ASSERT(LooseCompare(g_reportedFreq.pll200M0Freq, g_expectedFreq.pll200M0Freq));
    CY_ASSERT(LooseCompare(g_reportedFreq.pll200M1Freq, g_expectedFreq.pll200M1Freq));
    for(uint32_t i = 0ul; i < CY_SYSCLK_HFCLK_NUM; i++)
    {
        CY_ASSERT(LooseCompare(g_reportedFreq.ClockFreqHf[i], g_expectedFreq.ClockFreqHf[i]));
    }
    CY_ASSERT(LooseCompare(g_reportedFreq.WDTClockFreq, g_expectedFreq.WDTClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.ECOPrescaleClockFreq, g_expectedFreq.ECOPrescaleClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.LfClockFreq, g_expectedFreq.LfClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.MTWDTClockFreq, g_expectedFreq.MTWDTClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.TimerClockFreq, g_expectedFreq.TimerClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Fast0ClockFreq, g_expectedFreq.Fast0ClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Fast1ClockFreq, g_expectedFreq.Fast1ClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.PeriClockFreq, g_expectedFreq.PeriClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.MemClockFreq, g_expectedFreq.MemClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.SlowClockFreq, g_expectedFreq.SlowClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.CoreClockFreq, g_expectedFreq.CoreClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.SysticClockFreq, g_expectedFreq.SysticClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Gr5ClockFreq, g_expectedFreq.Gr5ClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Gr6ClockFreq, g_expectedFreq.Gr6ClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Gr9ClockFreq, g_expectedFreq.Gr9ClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Gr3ClockFreq, g_expectedFreq.Gr3ClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Gr4ClockFreq, g_expectedFreq.Gr4ClockFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Gr8ClockFreq, g_expectedFreq.Gr8ClockFreq));

    CY_ASSERT(LooseCompare(g_reportedFreq.FlashInterfaceFreq, g_expectedFreq.FlashInterfaceFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Ram0FastFreq, g_expectedFreq.Ram0FastFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Ram0SlowFreq, g_expectedFreq.Ram0SlowFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Ram1FastFreq, g_expectedFreq.Ram1FastFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Ram2FastFreq, g_expectedFreq.Ram2FastFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.Ram2SlowFreq, g_expectedFreq.Ram2SlowFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.RomFastFreq, g_expectedFreq.RomFastFreq));
    CY_ASSERT(LooseCompare(g_reportedFreq.RomSlowFreq, g_expectedFreq.RomSlowFreq));
}

static void PrepareExpectedClockFrequency(void)
{
    g_expectedFreq.wcoFreq            = CLK_FREQ_WCO;
    g_expectedFreq.refHfClock         = PATH_SOURCE_CLOCK_FREQ;
    for(uint32_t i = 0ul; i < CY_SYSCLK_PATH_NUM; i++)
    {
        g_expectedFreq.SourceFreqPath[i] = PATH_SOURCE_CLOCK_FREQ;
    }

    g_expectedFreq.OutFreqPath[0] = FLL_TARGET_FREQ;
    g_expectedFreq.OutFreqPath[1] = PLL400_0_TARGET_FREQ;
    g_expectedFreq.OutFreqPath[2] = PLL400_1_TARGET_FREQ;
    g_expectedFreq.OutFreqPath[3] = PLL200_0_TARGET_FREQ;
    g_expectedFreq.OutFreqPath[4] = PLL200_1_TARGET_FREQ;
    g_expectedFreq.OutFreqPath[5] = PATH_SOURCE_CLOCK_FREQ;

    g_expectedFreq.fllSourceFreq      = PATH_SOURCE_CLOCK_FREQ;
    g_expectedFreq.pll400M0SourceFreq = PATH_SOURCE_CLOCK_FREQ;
    g_expectedFreq.pll400M1SourceFreq = PATH_SOURCE_CLOCK_FREQ;
    g_expectedFreq.pll200M0SourceFreq = PATH_SOURCE_CLOCK_FREQ;
    g_expectedFreq.pll200M1SourceFreq = PATH_SOURCE_CLOCK_FREQ;

    g_expectedFreq.fllFreq            = FLL_TARGET_FREQ;
    g_expectedFreq.pll400M0Freq       = PLL400_0_TARGET_FREQ;
    g_expectedFreq.pll400M1Freq       = PLL400_1_TARGET_FREQ;
    g_expectedFreq.pll200M0Freq       = PLL200_0_TARGET_FREQ;
    g_expectedFreq.pll200M1Freq       = PLL200_1_TARGET_FREQ;

    for(uint32_t i = 0ul; i < CY_SYSCLK_HFCLK_NUM; i++)
    {
        g_expectedFreq.ClockFreqHf[i] = g_expectedFreq.OutFreqPath[CLK_SOURCE_PATH_HF[i]];
        if(CLK_PRE_DIVIDER_HF[i] == CY_SYSCLK_HFCLK_DIVIDE_BY_2)
        {
            g_expectedFreq.ClockFreqHf[i] >>= 1ul;
        }
        else if(CLK_PRE_DIVIDER_HF[i] == CY_SYSCLK_HFCLK_DIVIDE_BY_4)
        {
            g_expectedFreq.ClockFreqHf[i] >>= 2ul;
        }
        else if(CLK_PRE_DIVIDER_HF[i] == CY_SYSCLK_HFCLK_DIVIDE_BY_8)
        {
            g_expectedFreq.ClockFreqHf[i] >>= 3ul;
        }
    }

    if(CLK_LF_SOURCE == CY_SYSCLK_LFCLK_IN_ILO0)
    {
        g_expectedFreq.WDTClockFreq       = CLK_FREQ_ILO0;
        g_expectedFreq.LfClockFreq        = CLK_FREQ_ILO0;
        g_expectedFreq.MTWDTClockFreq     = CLK_FREQ_ILO0;
    }
    else if(CLK_LF_SOURCE == CY_SYSCLK_LFCLK_IN_WCO)
    {
        g_expectedFreq.WDTClockFreq       = CLK_FREQ_WCO;
        g_expectedFreq.LfClockFreq        = CLK_FREQ_WCO;
        g_expectedFreq.MTWDTClockFreq     = CLK_FREQ_WCO;
    }
    else if(CLK_LF_SOURCE == CY_SYSCLK_LFCLK_IN_ILO1)
    {
        g_expectedFreq.WDTClockFreq       = CLK_FREQ_ILO1;
        g_expectedFreq.LfClockFreq        = CLK_FREQ_ILO1;
        g_expectedFreq.MTWDTClockFreq     = CLK_FREQ_ILO1;
    }
    else // (CLK_LF_SOURCE == CY_SYSCLK_LFCLK_IN_ECO_PRESCALE)
    {
        g_expectedFreq.WDTClockFreq       = ECO_PRESCALER_TARGET_FREQ;
        g_expectedFreq.LfClockFreq        = ECO_PRESCALER_TARGET_FREQ;
        g_expectedFreq.MTWDTClockFreq     = ECO_PRESCALER_TARGET_FREQ;
    }

    if(CLK_RTC_SOURCE == CY_SYSCLK_BAK_IN_WCO)
    {
        g_expectedFreq.RTCClockFreq       = CLK_FREQ_WCO;
    }
    else if(CLK_RTC_SOURCE == CY_SYSCLK_BAK_IN_ILO)
    {
        g_expectedFreq.RTCClockFreq       = CLK_FREQ_ILO0;
    }
    else
    {
        CY_ASSERT(false); // Not supported
    }

    g_expectedFreq.ECOPrescaleClockFreq = ECO_PRESCALER_TARGET_FREQ;

    if(CLK_TIMER_SOURCE == CY_SYSCLK_CLKTIMER_IN_IMO)
    {
        g_expectedFreq.TimerClockFreq     = CLK_FREQ_IMO;
    }
    else if(CLK_TIMER_SOURCE == CY_SYSCLK_CLKTIMER_IN_HF0_NODIV)
    {
        g_expectedFreq.TimerClockFreq     = g_expectedFreq.ClockFreqHf[0];
    }
    else if(CLK_TIMER_SOURCE == CY_SYSCLK_CLKTIMER_IN_HF0_DIV2)
    {
        g_expectedFreq.TimerClockFreq     = g_expectedFreq.ClockFreqHf[0] >> 1ul;
    }
    else if(CLK_TIMER_SOURCE == CY_SYSCLK_CLKTIMER_IN_HF0_DIV4)
    {
        g_expectedFreq.TimerClockFreq     = g_expectedFreq.ClockFreqHf[0] >> 2ul;
    }
    else if(CLK_TIMER_SOURCE == CY_SYSCLK_CLKTIMER_IN_HF0_DIV8)
    {
        g_expectedFreq.TimerClockFreq     = g_expectedFreq.ClockFreqHf[0] >> 3ul;
    }
    g_expectedFreq.TimerClockFreq /= (CLK_TIMER_DIVIDER + 1ul);

    g_expectedFreq.Fast0ClockFreq     = (uint32_t)((double)g_expectedFreq.ClockFreqHf[1] / (CLK_FAST0_DIVIDER + 1.0));
    g_expectedFreq.Fast1ClockFreq     = (uint32_t)((double)g_expectedFreq.ClockFreqHf[1] / (CLK_FAST1_DIVIDER + 1.0));
    g_expectedFreq.PeriClockFreq      = g_expectedFreq.ClockFreqHf[0] / (CLK_PERI_DIVIDER + 1ul);
    g_expectedFreq.MemClockFreq       = g_expectedFreq.ClockFreqHf[0] / (CLK_MEM_DIVIDER + 1ul);
    g_expectedFreq.SlowClockFreq      = g_expectedFreq.MemClockFreq / (CLK_SLOW_DIVIDER + 1ul);
    g_expectedFreq.CoreClockFreq      = g_expectedFreq.SlowClockFreq;

    if(CLK_SYSTICK_SOURCE == CY_SYSTICK_CLOCK_SOURCE_CLK_LF)
    {
        g_expectedFreq.SysticClockFreq = g_expectedFreq.LfClockFreq;
    }
    else if(CLK_SYSTICK_SOURCE == CY_SYSTICK_CLOCK_SOURCE_CLK_IMO)
    {
        g_expectedFreq.SysticClockFreq = CLK_FREQ_IMO;
    }
    else if(CLK_SYSTICK_SOURCE == CY_SYSTICK_CLOCK_SOURCE_CLK_ECO)
    {
        g_expectedFreq.SysticClockFreq = CLK_FREQ_ECO;
    }
    else if(CLK_SYSTICK_SOURCE == CY_SYSTICK_CLOCK_SOURCE_CLK_TIMER)
    {
        g_expectedFreq.SysticClockFreq = g_expectedFreq.TimerClockFreq;
    }
    else //(CLK_SYSTICK_SOURCE == CY_SYSTICK_CLOCK_SOURCE_CLK_CPU)
    {
        g_expectedFreq.SysticClockFreq = g_expectedFreq.CoreClockFreq;
    }


    g_expectedFreq.Gr5ClockFreq    = g_expectedFreq.ClockFreqHf[2] / (CLK_GR5_DIVIDER + 1ul);
    g_expectedFreq.Gr6ClockFreq    = g_expectedFreq.ClockFreqHf[2] / (CLK_GR6_DIVIDER + 1ul);
    g_expectedFreq.Gr9ClockFreq    = g_expectedFreq.ClockFreqHf[2] / (CLK_GR9_DIVIDER + 1ul);

    g_expectedFreq.Gr3ClockFreq    = g_expectedFreq.PeriClockFreq / (CLK_GR3_DIVIDER + 1ul);
    g_expectedFreq.Gr4ClockFreq    = g_expectedFreq.PeriClockFreq / (CLK_GR4_DIVIDER + 1ul);
    g_expectedFreq.Gr8ClockFreq    = g_expectedFreq.PeriClockFreq / (CLK_GR8_DIVIDER + 1ul);

    g_expectedFreq.FlashInterfaceFreq = g_expectedFreq.ClockFreqHf[0] / (FLASH_WAIT_STATE + 1ul);
    g_expectedFreq.Ram0FastFreq = g_expectedFreq.ClockFreqHf[0] / (RAM0_FAST_WAIT_STATE + 1ul);
    g_expectedFreq.Ram0SlowFreq = g_expectedFreq.ClockFreqHf[0] / (RAM0_SLOW_WAIT_STATE + 1ul);
    g_expectedFreq.Ram1FastFreq = g_expectedFreq.ClockFreqHf[0] / (RAM1_FAST_WAIT_STATE + 1ul);
    g_expectedFreq.Ram1SlowFreq = g_expectedFreq.ClockFreqHf[0] / (RAM1_SLOW_WAIT_STATE + 1ul);
    g_expectedFreq.Ram2FastFreq = g_expectedFreq.ClockFreqHf[0] / (RAM2_FAST_WAIT_STATE + 1ul);
    g_expectedFreq.Ram2SlowFreq = g_expectedFreq.ClockFreqHf[0] / (RAM2_SLOW_WAIT_STATE + 1ul);
    g_expectedFreq.RomFastFreq = g_expectedFreq.ClockFreqHf[0] / (ROM_FAST_WAIT_STATE + 1ul);
    g_expectedFreq.RomSlowFreq = g_expectedFreq.ClockFreqHf[0] / (ROM_SLOW_WAIT_STATE + 1ul);

    /* Limitation Check */
    CY_ASSERT(g_expectedFreq.SysticClockFreq <= g_expectedFreq.CoreClockFreq); // From experience

}



/* [] END OF FILE */
