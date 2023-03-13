#ifndef __CRC32_EMULATOR__
#define __CRC32_EMULATOR__

/**********************************/
/****** 32 bit CRC Emulator *******/
/**********************************/

typedef union
{
    uint32_t u32;
    uint8_t  u8[4];
} un_WordChar;

typedef enum
{
    CRC_MSB_FIRST = 0,
    CRC_LSB_FIRST,
} en_first_bit;

typedef struct {
    en_first_bit ROBIT; // Reserve Output Bit Setting (LSB first or MSB first)
    en_first_bit RIBIT; // Reserve Input Bit Setting (LSB first or MSB first)
} stc_CRC_OPTION;

typedef struct {
    const uint8_t* indata;   // input data
    uint32_t       size;     // input data size
    un_WordChar    poly;     // corresponding to POLYNOMIAL register
    uint8_t        data_xor; // corresponding to DATA_XOR register
    un_WordChar    rem_xor;  // corresponding to REM_CTL register
    stc_CRC_OPTION option;
} stc_crc32_emu_config_t;

static uint8_t ExtractBit(uint32_t input, uint8_t pos)
{
    return (uint8_t)((input & (1ul << (uint32_t)pos)) != 0u);
}

static uint32_t Reverse4ByteEndian(uint32_t in)
{
    return ((in & 0xFF000000ul) >> 24ul) |
           ((in & 0x00FF0000ul) >> 8ul)  |
           ((in & 0x0000FF00ul) << 8ul)  |
           ((in & 0x000000FFul) << 24ul);
}

uint8_t Reverse8BitEndian(uint8_t in)
{
    uint8_t result = 0u;

    for(uint8_t i = 0u; i < 8u; i++)
    {
        result |= ((in & (1u << (7u - i))) != 0u) << i;
    }

    return result;
}

uint32_t Reverse32BitEndian(uint32_t in)
{
    uint32_t  result = 0;

    for(uint8_t i = 0u; i < 32u; i++)
    {
        result |= ((in & (1u << (31u - i))) != 0u) << i;
    }

    return result;
}

static uint32_t g_CrcSeed = 0xFFFFFFFFul;
static void SetCrcSeed(uint32_t seed)
{
    g_CrcSeed = seed;
}

static uint32_t CRCEmulate32bit(stc_crc32_emu_config_t * crc_params)
{
    /********** Set initial value of crc *************/
    un_WordChar caluclatedCRC = { .u32 = g_CrcSeed };

    for(uint32_t i_ProcessingBytePos = 0ul; i_ProcessingBytePos < crc_params->size; i_ProcessingBytePos += 4ul)
    {
        uint32_t  dataSizeInProcess_Byte;
        if(i_ProcessingBytePos + 4ul > crc_params->size)
        {
            dataSizeInProcess_Byte = (crc_params->size - i_ProcessingBytePos);
        }
        else
        {
            dataSizeInProcess_Byte = 4ul;
        }

        uint32_t dataSizeInProcess_Bit =  dataSizeInProcess_Byte * 8ul;

        /***********************************************/
        /***** 1. Apply DATA XOR to the input data *****/
        /***********************************************/
        un_WordChar dataInProcess = { .u32 = 0x00000000ul };
        for(uint32_t i = 0ul ; i < dataSizeInProcess_Byte; i++)
        {
            dataInProcess.u8[i] = crc_params->indata[i_ProcessingBytePos + i] ^ crc_params->data_xor;
        }

        /****************************************************/
        /***** 2. Apply RIBYTE effect to the input data *****/
        /****************************************************/
        dataInProcess.u32 = Reverse4ByteEndian(dataInProcess.u32) >> (32ul - dataSizeInProcess_Bit);

        /****************************************************/
        /***** 3. Apply RIBIT effect to the input data ******/
        /****************************************************/
        if(crc_params->option.RIBIT == CRC_LSB_FIRST)
        {
            dataInProcess.u32 = Reverse4ByteEndian(Reverse32BitEndian(dataInProcess.u32));
        }

        /****************************************************/
        /************ 4. Start CRC calculation  *************/
        /****************************************************/
        for(int8_t i_BitPosition = (dataSizeInProcess_Bit-1ul); i_BitPosition >= 0; i_BitPosition--)
        {
            uint8_t CRC_HighestBit;
            uint8_t Data_Bit;

            /* Extract highest bit of current CRC data */
            CRC_HighestBit = ExtractBit(caluclatedCRC.u32, (32u-1u));

            /* Extract the bit of the data */
            Data_Bit = ExtractBit(dataInProcess.u32, i_BitPosition);

            /* Shift "caluclatedCRC" 1 bit left */
            caluclatedCRC.u32 = caluclatedCRC.u32 << 1ul;

            /* If current CRC highest bit XOR the data bit is true, */
            /* execute CRC XOR polarity */
            if(Data_Bit ^ CRC_HighestBit)
            {
                caluclatedCRC.u32 = caluclatedCRC.u32 ^ crc_params->poly.u32;
            }
        }
    }

    /* Save calculated CRC so far as a next CRC seed */
    g_CrcSeed = caluclatedCRC.u32;

    /****************************************************/
    /***** 5. Apply FXOR effect to the output data ******/
    /****************************************************/
    caluclatedCRC.u32 = caluclatedCRC.u32 ^ crc_params->rem_xor.u32;

    /****************************************************/
    /***** 6. Apply ROBIT effect to the output data *****/
    /****************************************************/
    if(crc_params->option.ROBIT == CRC_LSB_FIRST)
    {
        caluclatedCRC.u32 = Reverse32BitEndian(caluclatedCRC.u32);
    }

    return caluclatedCRC.u32;
}

#endif
