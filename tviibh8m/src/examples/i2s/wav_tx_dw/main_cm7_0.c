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

/*
    This example transmits .wav file to external audio DAC (AIC261)
    via I2S protocol.
    Please note this example supports only 16 bit length PCM data.
    TVII is master, and the AIC261 is slave. Sampling rate, 
    monaural/stereo will be determined by wav header.
    When internal clock is used, please connect I2S_MCLK to MCLK input
    port of the AIC261. When external clock is used please connect
    external crystal output to I2S_IF of TVII and to MCLK input port
    of the AIC261.
    - Operation Description
    I2S HW will trigger DW transmission. The DW will then transmit
    sound data to I2S FIFO. Any CPU task is not required.
*/

/* Device Specific Definition  */
#define AUDIO_I2S_DW            (CY_AUDIOSS_TX_DW)
#define AUDIO_I2S_TX_DW_LOG_CH  (CY_AUDIOSS_TX_DW_CH)
#define AUDIO_I2S_TX_TO_DW_TRIG (CY_AUDIOSS_TX_DW_TRIG)

/* User setting for clock and  */
//------------------------------------------------------------------------------
#define I2S_INTERNAL_CLOCK          (0ul)
#define I2S_EXTERNAL_CLOCK          (1ul)
#define CY_I2S_USE_CLK              I2S_INTERNAL_CLOCK

#if(CY_I2S_USE_CLK == I2S_INTERNAL_CLOCK)
  #define AUDIO_SOURCE_FREQ_IN_HZ (196608000ul) // Assumed this value has been configured in system initialization.
#else // I2S_EXTERNAL_CLOCK
  #define AUDIO_SOURCE_FREQ_IN_HZ (24576000ul) // Frequency of External Crystal
#endif
#define AUDIO_CONST_DIV_NUM     (8ul)         // Note: this is specified by HW IP
#define PCM_DATA_WIDTH          (16ul)        // this example only support 16 bit PCM
#define I2S_CHANNEL_NUM         (2ul)         // fixed 2 channel in I2S mode

#define AUDIO_MCLK_DIV_CONFIG (CY_I2S_MCLK_DIV_8) // User defined value
#define AUDIO_MCLK_DIV_NUM    (1ul << AUDIO_MCLK_DIV_CONFIG)
#if(CY_I2S_USE_CLK == I2S_INTERNAL_CLOCK)
  #define AUDIO_MCLK_CLOCK_HZ   (AUDIO_SOURCE_FREQ_IN_HZ / AUDIO_MCLK_DIV_NUM)
#else
  #define AUDIO_MCLK_CLOCK_HZ   AUDIO_SOURCE_FREQ_IN_HZ // Crystal clock will go directory MCLK port
#endif

#pragma pack(1)
typedef struct {
    uint32_t riff;         // offset: 0,  size: 4, Indicates type RIFF value = 'RIFF'
    uint32_t size;         // offset: 4,  size: 4, file size - 8
    uint32_t type;         // offset: 8,  size: 4, = 'WAVE'
    uint32_t id;           // offset: 12, size: 4, Format chunk marker. Includes trailing null
    uint32_t chanksize;    // offset: 16, size: 4, chunk data size (16)
    uint16_t format;       // offset: 20, size: 2, Type of format (1 is PCM) - 2 byte integer
    uint16_t channels;     // offset: 22, size: 2, Number of Channels - 2 byte integer
    uint32_t samplerate;   // offset: 24, size: 4, Sample Rate
    uint32_t bytepersec;   // offset: 28, size: 4, (Sample Rate * BitsPerSample * Channels) / 8
    uint16_t blockalign;   // offset: 30, size: 2, data block size
    uint16_t bitswidth;    // offset: 32, size: 2, Bits per sample
    uint32_t data;         // offset: 36, size: 4, "data"
    uint32_t bytesize;     // offset: 40, size: 4, Size of the data section
} stc_wav_header;
#define WAV_HEADER_SIZE (sizeof(stc_wav_header))
#pragma pack()

// Sound Data
const uint8_t g_Wav16Data[] =
{
    #include "wav_16bit_data.inc"
};
#define I2S_SOUND_SIZE_IN_BYTE ((sizeof(g_Wav16Data) / sizeof(g_Wav16Data[0])) - (sizeof(stc_wav_header)))
#define I2S_SOUND_PCM_NUMBER   (I2S_SOUND_SIZE_IN_BYTE / 2ul)
uint16_t*       gp_SoundData = (uint16_t*)&g_Wav16Data[WAV_HEADER_SIZE];
stc_wav_header* gp_WavHeader = (stc_wav_header*)g_Wav16Data;
uint32_t        g_PcmNumber; // will be updated in runtime

/* I2S-Tx and I2S-Rx interface configuration */
//------------------------------------------------------------------------------
#define I2S_TX_FIFO_TH_NUM   (CY_I2S_TX_FIFO_NUM/2ul)
#define I2S_TX_FIFO_REST_NUM (CY_I2S_TX_FIFO_NUM - I2S_TX_FIFO_TH_NUM)

cy_stc_i2s_config_clk_t g_i2s_clk_config =
{
    .clkDiv  = 0u, // will be modified in runtime
    .extClk  = (bool)CY_I2S_USE_CLK,
    .mclkDiv = AUDIO_MCLK_DIV_CONFIG,
    .mclkEn  = true,
};

cy_stc_i2s_config_tx_t g_i2s_tx_config =
{
    .txEnabled          = true,
    .txDmaTrigger       = true,
    .txMasterMode       = true,
    .txAlignment        = CY_I2S_I2S_MODE,
    .txWsPulseWidth     = CY_I2S_WS_ONE_CHANNEL_LENGTH, // Not be cared in I2S mode
    .txWatchdogEnable   = false,
    .txWatchdogValue    = 0xFFFFFFFFul,
    .txSdoLatchingTime  = false,
    .txSckoInversion    = false,
    .txSckiInversion    = false,
    .txChannels         = 0ul,                          // Not be cared in I2S mode
    .txChannelLength    = CY_I2S_LEN16,
    .txWordLength       = CY_I2S_LEN16,
    .txOverheadValue    = CY_I2S_OVHDATA_ZERO,
    .txFifoTriggerLevel = I2S_TX_FIFO_TH_NUM,
};

cy_stc_gpio_pin_prt_config_t g_i2s_pin_config[] =
{
//  {                    portReg,                    pinNum, outVal,                driveMode,                         hsiom, intEdge, intMask, vtrip, slewRate, driveSel },
    {     CY_AUDIOSS_TX_SCK_PORT,     CY_AUDIOSS_TX_SCK_PIN,    0ul, CY_GPIO_DM_STRONG_IN_OFF,     CY_AUDIOSS_TX_SCK_PIN_MUX,     0ul,     0ul,   0ul,      0ul,      0ul },
    {      CY_AUDIOSS_TX_WS_PORT,      CY_AUDIOSS_TX_WS_PIN,    0ul, CY_GPIO_DM_STRONG_IN_OFF,      CY_AUDIOSS_TX_WS_PIN_MUX,     0ul,     0ul,   0ul,      0ul,      0ul },
    {     CY_AUDIOSS_TX_SDO_PORT,     CY_AUDIOSS_TX_SDO_PIN,    0ul, CY_GPIO_DM_STRONG_IN_OFF,     CY_AUDIOSS_TX_SDO_PIN_MUX,     0ul,     0ul,   0ul,      0ul,      0ul },
#if (CY_I2S_USE_CLK == I2S_INTERNAL_CLOCK)
    {       CY_AUDIOSS_MCLK_PORT,       CY_AUDIOSS_MCLK_PIN,    0ul, CY_GPIO_DM_STRONG_IN_OFF,       CY_AUDIOSS_MCLK_PIN_MUX,     0ul,     0ul,   0ul,      0ul,      0ul },
#else
    { CY_AUDIOSS_CLK_I2S_IF_PORT, CY_AUDIOSS_CLK_I2S_IF_PIN,    0ul,         CY_GPIO_DM_HIGHZ, CY_AUDIOSS_CLK_I2S_IF_PIN_MUX,     0ul,     0ul,   0ul,      0ul,      0ul },
#endif
};
#define I2S_PORT_NUM (sizeof(g_i2s_pin_config)/sizeof(g_i2s_pin_config[0]))

/* I2S dedicated DW */
#define RESERVED_DESCRIPTOR_NUM (512)
static cy_stc_pdma_descr_t g_stcDescr[RESERVED_DESCRIPTOR_NUM];

static cy_stc_pdma_chnl_config_t   chnlConfig =
{
    .PDMA_Descriptor = g_stcDescr, // will be updated in runtime
    .preemptable     = 0ul,
    .priority        = 0ul,
    .enable          = 1ul,  // enabled after initialization
};

static cy_stc_pdma_descr_config_t  stcDmaDescrConfig =
{
    .deact          = 0ul,
    .intrType       = CY_PDMA_INTR_DESCR_CMPLT,
    .trigoutType    = CY_PDMA_TRIGOUT_DESCR_CMPLT,
    .chStateAtCmplt = CY_PDMA_CH_ENABLED,
    .triginType     = CY_PDMA_TRIGIN_1ELEMENT,
    .dataSize       = CY_PDMA_HALFWORD,
    .srcTxfrSize    = 0ul,
    .destTxfrSize   = 1ul, // 32bit
    .descrType      = CY_PDMA_2D_TRANSFER,
    .srcAddr        = (uint32_t *)&g_Wav16Data[WAV_HEADER_SIZE],
    .destAddr       = (uint32_t *)&CY_AUDIOSS_TYPE->unTX_FIFO_WR.u32Register,
    .srcXincr       = 0ul, // will be updated in runtime
    .destXincr      = 0ul,
    .xCount         = 0ul, // will be updated in runtime
    .srcYincr       = 0ul, // will be updated in runtime
    .destYincr      = 0ul,
    .yCount         = 0ul, // will be updated in runtime
    .descrNext      = NULL, // will be updated in runtime
};

static void Initialize_AIC261(void);
static void AudioDataIntegrityCheck(void);
static void PrepareDwDescriptorMono(void);
static void PrepareDwDescriptorStereo(void);

int main(void)
{
    SystemInit();

    SCB_DisableDCache(); // Disables D cache because DMA also reads descriptor in the SRAM.
    __enable_irq();

    // Check wav file data integrity
    AudioDataIntegrityCheck();

    /********************************************/
    /*    Initialize TI Audio IC (AIC 261)      */
    /********************************************/
    Cy_AIC261_InitSPI();
    Initialize_AIC261();

    /**************************************/
    /* Port setting for I2S communication */
    /**************************************/
    Cy_GPIO_Multi_Pin_Init(g_i2s_pin_config, I2S_PORT_NUM);

    /*******************************************/
    /*    Initialization DW dedicating to I2S  */
    /*******************************************/
    if(gp_WavHeader->channels == 2)
    {
        PrepareDwDescriptorStereo();
    }
    else
    {
        PrepareDwDescriptorMono();
    }

    Cy_PDMA_Disable(AUDIO_I2S_DW);
    Cy_PDMA_Chnl_DeInit(AUDIO_I2S_DW, AUDIO_I2S_TX_DW_LOG_CH);
    Cy_PDMA_Chnl_Init(AUDIO_I2S_DW, AUDIO_I2S_TX_DW_LOG_CH, &chnlConfig);
    Cy_PDMA_Chnl_Enable(AUDIO_I2S_DW, AUDIO_I2S_TX_DW_LOG_CH);
    Cy_PDMA_Chnl_SetInterruptMask(DW1, AUDIO_I2S_TX_DW_LOG_CH);
    Cy_PDMA_Enable(AUDIO_I2S_DW);
    /*    Setting Trigger Mux  */
    Cy_TrigMux_Connect1To1(AUDIO_I2S_TX_TO_DW_TRIG, 0ul, TRIGGER_TYPE_LEVEL, 0ul);

    /********************************************/
    /*          initialization for I2S          */
    /********************************************/
    // De-Initialize I2S
    Cy_I2S_DeInit(CY_AUDIOSS_TYPE);

    // Flush FIFO of I2S 
    Cy_I2S_ClearTxFifo(CY_AUDIOSS_TYPE);
    Cy_I2S_ClearRxFifo(CY_AUDIOSS_TYPE);

    // Calculate clock divider number as per sound data format
    g_i2s_clk_config.clkDiv = AUDIO_SOURCE_FREQ_IN_HZ / (AUDIO_CONST_DIV_NUM * gp_WavHeader->samplerate * PCM_DATA_WIDTH * I2S_CHANNEL_NUM);
    CY_ASSERT(AUDIO_SOURCE_FREQ_IN_HZ % (AUDIO_CONST_DIV_NUM * gp_WavHeader->samplerate * PCM_DATA_WIDTH) == 0ul);
    // Initialize I2S Clock settings
    CY_ASSERT(Cy_I2S_InitClock(CY_AUDIOSS_TYPE, &g_i2s_clk_config) == CY_I2S_SUCCESS);

    // Initialize I2S Tx settings
    CY_ASSERT(Cy_I2S_InitTx(CY_AUDIOSS_TYPE, &g_i2s_tx_config) == CY_I2S_SUCCESS);

    /* Start I2S Tx */
    Cy_I2S_EnableTx(CY_AUDIOSS_TYPE);

    for(;;);
}

#define DW_DEST_COUNT_BIT_NUM (8ul)
#define DW_DEST_COUNT_MAX     (1ul << DW_DEST_COUNT_BIT_NUM)
#define DW_DEST_COUNT_XY_MAX  (1ul << (DW_DEST_COUNT_BIT_NUM*2))
#define DW_DEST_COUNT_MASK    (DW_DEST_COUNT_MAX - 1ul)
#define DW_DEST_COUNT_XY_MASK (DW_DEST_COUNT_XY_MAX - 1ul)
static void PrepareDwDescriptorStereo(void)
{
    stcDmaDescrConfig.srcXincr = 1ul;

    uint32_t restDataSize = I2S_SOUND_PCM_NUMBER;
    uint32_t counter = 0ul;
    while(restDataSize > 0)
    {
        uint32_t startPoint = I2S_SOUND_PCM_NUMBER - restDataSize;
        stcDmaDescrConfig.srcAddr = &gp_SoundData[startPoint];
        if(restDataSize > DW_DEST_COUNT_XY_MAX)
        {
            stcDmaDescrConfig.xCount = DW_DEST_COUNT_MAX;
            stcDmaDescrConfig.yCount = DW_DEST_COUNT_MAX;
            stcDmaDescrConfig.srcYincr = DW_DEST_COUNT_MAX;
            stcDmaDescrConfig.descrNext = &g_stcDescr[counter+1];
            restDataSize -= DW_DEST_COUNT_XY_MAX;
        }
        else if(restDataSize > DW_DEST_COUNT_MAX)
        {
            stcDmaDescrConfig.xCount = DW_DEST_COUNT_MAX;
            stcDmaDescrConfig.yCount = restDataSize >> DW_DEST_COUNT_BIT_NUM;
            stcDmaDescrConfig.srcYincr = DW_DEST_COUNT_MAX;
            stcDmaDescrConfig.descrNext = &g_stcDescr[counter+1];
            restDataSize -= DW_DEST_COUNT_MAX * stcDmaDescrConfig.yCount;
        }
        else
        {
            stcDmaDescrConfig.xCount = restDataSize;
            stcDmaDescrConfig.yCount = 1;
            stcDmaDescrConfig.srcYincr = restDataSize;
            stcDmaDescrConfig.descrNext = &g_stcDescr[0];
            restDataSize = 0ul;
        }

        Cy_PDMA_Descr_Init(&g_stcDescr[counter], &stcDmaDescrConfig);
        counter++;
    }
}

static void PrepareDwDescriptorMono(void)
{
    uint32_t descNum = (I2S_SOUND_PCM_NUMBER >> DW_DEST_COUNT_BIT_NUM)  + 1ul;
    CY_ASSERT(descNum < RESERVED_DESCRIPTOR_NUM);
    uint32_t restPcmNum = (I2S_SOUND_PCM_NUMBER & DW_DEST_COUNT_MASK);

    stcDmaDescrConfig.srcXincr = 0ul;
    stcDmaDescrConfig.xCount   = 2ul; // mono L and R
    stcDmaDescrConfig.srcYincr = 1ul;

    for(uint32_t i = 0ul; i < descNum; i++)
    {
        stcDmaDescrConfig.srcAddr = &gp_SoundData[DW_DEST_COUNT_MAX*i];
        if(i == (descNum - 1ul)) // last descriptor
        {
            stcDmaDescrConfig.yCount = restPcmNum;
            stcDmaDescrConfig.descrNext = &g_stcDescr[0];
        }
        else
        {
            stcDmaDescrConfig.yCount = DW_DEST_COUNT_MAX;
            stcDmaDescrConfig.descrNext = &g_stcDescr[i+1];
        }
        Cy_PDMA_Descr_Init(&g_stcDescr[i],&stcDmaDescrConfig);
    }
}


static void Initialize_AIC261(void)
{
    /* PAGE1 Reset All Registers */
    Cy_AIC261_WriteCtlReg(CY_PAGE1_RESET, 0xBB00);

    //PAGE1 REGISTER 00H: ADC Control 
    {
        /* read the register */
        cy_un_AIC261_ADCControl ADC_Control;
        ADC_Control.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE1_ADC_CTL);

        /* modify the register */
        ADC_Control.bitField.ADCR       = 0x2;   //Conversion Rate: 2MHz

        /* write the register */
        Cy_AIC261_WriteCtlReg(CY_PAGE1_ADC_CTL, ADC_Control.u16);

        /* verify the register */
        cy_un_AIC261_ADCControl ADC_Control_Verify;
        ADC_Control_Verify.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE1_ADC_CTL);
        CY_ASSERT(ADC_Control.bitField.ADCR == ADC_Control_Verify.bitField.ADCR);
    }

    // PAGE1 Reference Control
    {
        /* read the register */
        cy_un_AIC261_RefferenceControl Ref_Control;
        Ref_Control.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE1_REFERENCE);

        /* modify the register */
        Ref_Control.bitField.IREFV      = 0x1;   // VREF = 2.50V

        /* write the register */
        Cy_AIC261_WriteCtlReg(CY_PAGE1_REFERENCE, Ref_Control.u16);

        /* verify the register */
        cy_un_AIC261_RefferenceControl Ref_Control_Verify;
        Ref_Control_Verify.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE1_REFERENCE);
        CY_ASSERT(Ref_Control.bitField.IREFV == Ref_Control_Verify.bitField.IREFV);
    }

    //REGISTER 01H: Audio control 1
    {
        /* read the register */
        cy_un_AIC261_AudioControl_1 AudioControl;
        AudioControl.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE2_AUDIO_CONTROL_1);

        /* modify the register */
        AudioControl.bitField.ADCHPF = 0; // High Pass filter disabled
        AudioControl.bitField.ADCIN  = 0; // ADC Input = Single-ended input MIC
        AudioControl.bitField.WLEN   = 0; // Word length 16 bit
        AudioControl.bitField.DATFM  = 0; // I2S mode
        AudioControl.bitField.DACFS  = 0; // DAC FS = Fsref / 1
        AudioControl.bitField.ADCFS  = 0; // ADC FS = Fsref /

        /* write the register */
        Cy_AIC261_WriteCtlReg(CY_PAGE2_AUDIO_CONTROL_1, AudioControl.u16);

        /* verify the register */
        cy_un_AIC261_AudioControl_1 AudioControl_Verify;
        AudioControl_Verify.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE2_AUDIO_CONTROL_1);
        CY_ASSERT(AudioControl_Verify.u16 == AudioControl.u16);
    }

    //REGISTER 01H: CODEC ADC Gain Control
    {
        /* read the register */
        cy_un_AIC261_ADCGainControl ADCGainControl;
        ADCGainControl.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE2_CODEC_ADC_GAIN);

        /* modify the register */
        ADCGainControl.bitField.ADMUT = 0x0;   // ADC channel not muted
        ADCGainControl.bitField.ADPGA = 0x20; // +16dB
        ADCGainControl.bitField.AGCEN = 0x0;

        /* write the register */
        Cy_AIC261_WriteCtlReg(CY_PAGE2_CODEC_ADC_GAIN, ADCGainControl.u16);

        /* verify the register */
        cy_un_AIC261_ADCGainControl ADCGainControl_Verify;
        ADCGainControl_Verify.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE2_CODEC_ADC_GAIN);
        CY_ASSERT(ADCGainControl.bitField.ADMUT == ADCGainControl_Verify.bitField.ADMUT);
        CY_ASSERT(ADCGainControl.bitField.ADPGA == ADCGainControl_Verify.bitField.ADPGA);
        CY_ASSERT(ADCGainControl.bitField.AGCEN == ADCGainControl_Verify.bitField.AGCEN);
    }

    //REGISTER 02H: CODEC DAC Gain Control (microphone vol control )
    {
        /* read the register */
        cy_un_AIC261_DACGainControl DACGainControl;
        DACGainControl.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE2_CODEC_DAC_GAIN);

        /* modify the register */
        DACGainControl.bitField.DALMU = 0x0; // DAC left channel not muted
        DACGainControl.bitField.DALVL = 0x0; // 0 dB
        DACGainControl.bitField.DARMU = 0x0; // DAC right channel not muted
        DACGainControl.bitField.DARVL = 0x0; // 0 dB

        /* write the register */
        Cy_AIC261_WriteCtlReg(CY_PAGE2_CODEC_DAC_GAIN, DACGainControl.u16);

        /* verify the register */
        cy_un_AIC261_DACGainControl DACGainControl_Verify;
        DACGainControl_Verify.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE2_CODEC_DAC_GAIN);
        CY_ASSERT(DACGainControl.bitField.DALMU == DACGainControl_Verify.bitField.DALMU);
        CY_ASSERT(DACGainControl.bitField.DALVL == DACGainControl_Verify.bitField.DALVL);
        CY_ASSERT(DACGainControl.bitField.DARMU == DACGainControl_Verify.bitField.DARMU);
        CY_ASSERT(DACGainControl.bitField.DARVL == DACGainControl_Verify.bitField.DARVL);
    }

    //REGISTER 05H: CODEC Power Control
    {
        /* read the register */
        cy_un_AIC261_DACPowerControl CODECPowerControl;
        CODECPowerControl.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE2_CODEC_POWER_CONTROL);

        /* modify the register */
        CODECPowerControl.bitField.PWDNC    = 0x0;  //Codec Power-up 
        CODECPowerControl.bitField.DAODRC   = 0x1;  //audio output drive in high power mode
        CODECPowerControl.bitField.DAPWDN   = 0x0;  //DAC power up
        CODECPowerControl.bitField.ADPWDN   = 0x0;  //ADC power up
        CODECPowerControl.bitField.VBIAS    = 0x1;  //VBIAS = 2.0V

        /* write the register */
        Cy_AIC261_WriteCtlReg(CY_PAGE2_CODEC_POWER_CONTROL, CODECPowerControl.u16);

        /* verify the register */
        cy_un_AIC261_DACPowerControl CODECPowerControl_Verify;
        CODECPowerControl_Verify.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE2_CODEC_POWER_CONTROL);
        CY_ASSERT(CODECPowerControl.bitField.PWDNC == CODECPowerControl_Verify.bitField.PWDNC);
        CY_ASSERT(CODECPowerControl.bitField.DAODRC == CODECPowerControl_Verify.bitField.DAODRC);
        CY_ASSERT(CODECPowerControl.bitField.DAPWDN == CODECPowerControl_Verify.bitField.DAPWDN);
        CY_ASSERT(CODECPowerControl.bitField.ADPWDN == CODECPowerControl_Verify.bitField.ADPWDN);
        CY_ASSERT(CODECPowerControl.bitField.VBIAS == CODECPowerControl_Verify.bitField.VBIAS);
    }

    //REGISTER 1BH: PLL Programmability
    {
        /* read the register */
        cy_un_AIC261_PLL_Programmability PLL_1BH;
        PLL_1BH.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE2_PLL_PROGRAMMABILITY_0);

        /* modify the register */
        PLL_1BH.bitField.PLLSEL     = 0x0; // PLL Disable.(MCLK=24.576MHz)
        PLL_1BH.bitField.QVAL       = AUDIO_MCLK_CLOCK_HZ / (AIC261_MCLK_CONST_DIV_NUM * gp_WavHeader->samplerate); // Valid only if PLL is disabled.
        CY_ASSERT(AUDIO_MCLK_CLOCK_HZ % (AIC261_MCLK_CONST_DIV_NUM * gp_WavHeader->samplerate) == 0ul);

         /* write the register */
        Cy_AIC261_WriteCtlReg(CY_PAGE2_PLL_PROGRAMMABILITY_0, PLL_1BH.u16);

        /* verify the register */
        cy_un_AIC261_PLL_Programmability PLL_1BH_Verify;
        PLL_1BH_Verify.u16 = Cy_AIC261_ReadCtlReg(CY_PAGE2_PLL_PROGRAMMABILITY_0);
        CY_ASSERT(PLL_1BH.bitField.PLLSEL == PLL_1BH_Verify.bitField.PLLSEL);
        CY_ASSERT(PLL_1BH.bitField.QVAL == PLL_1BH_Verify.bitField.QVAL);
    }
}

static void AudioDataIntegrityCheck(void)
{
    CY_ASSERT(memcmp((void*)&gp_WavHeader->riff, "RIFF", 4) == 0);
    CY_ASSERT(memcmp((void*)&gp_WavHeader->type, "WAVE", 4) == 0);
    CY_ASSERT(gp_WavHeader->format == 1/*PCM*/);
    CY_ASSERT((gp_WavHeader->channels == 1) || (gp_WavHeader->channels == 2));
    CY_ASSERT(gp_WavHeader->bitswidth == PCM_DATA_WIDTH);
}
/* [] END OF FILE */
