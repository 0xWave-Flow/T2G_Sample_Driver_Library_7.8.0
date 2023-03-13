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

typedef struct
{
    uint64_t u64[2];
} uint128_t;

typedef struct
{
    uint32_t att[4];
} stc_ppu_all_attribute_t;

typedef struct
{
    volatile uint32_t* pu32[4];
} stc_ppu_attribute_p_t;

/* User Test Parameters */
static const uint32_t gTestPpuSuffix = 2ul;
static const bool     gTestPpuFixed  = true;
static const bool     gTestPpuMaster = true;
static stc_ppu_all_attribute_t gTestAttValue =
{
    //     {       ATT0,       ATT1,       ATT2,       ATT3, }
    .att = { 0x1F1F1F1F, 0x1F1F1F1F, 0x00000000, 0x00000000, },
};
/* User Test Parameters end */

static stc_ppu_attribute_p_t GetTargetAtt(uint32_t suffix, bool isFixed, bool isMaster);
static uint128_t MakeActualWord(const stc_ppu_all_attribute_t* attValue);
static uint128_t MakeWordForPpuEccWithWoundingBit(const stc_ppu_all_attribute_t* attValue, uint32_t ppuWordAddr);
static uint32_t CalculatePpuWordAddr(uint32_t suffix, bool isFixed, bool isMaster);
static uint8_t mxambatk_ecc_128_120_parity(uint128_t word);
static uint128_t And128bit(uint128_t a, uint128_t b);
static uint8_t ReductionXOR128(uint128_t data);

int main(void)
{
    __enable_irq();

    SystemInit();

    /* Configure Fault report */
    Cy_SysFlt_ClearStatus(FAULT_STRUCT0); // clear status (typically this process is done by boot code)
    Cy_SysFlt_SetMaskByIdx(FAULT_STRUCT0, CY_SYSFLT_PERI_PERI_C_ECC);
    Cy_SysFlt_SetMaskByIdx(FAULT_STRUCT0, CY_SYSFLT_PERI_PERI_NC_ECC);

    /* Set user defined ATT value to the target registers */
    volatile stc_ppu_attribute_p_t att = GetTargetAtt(gTestPpuSuffix, gTestPpuFixed, gTestPpuMaster);
    *att.pu32[0] = gTestAttValue.att[0];
    *att.pu32[1] = gTestAttValue.att[1];
    *att.pu32[2] = gTestAttValue.att[2];
    *att.pu32[3] = gTestAttValue.att[3];

    /* Calculate ADDR[10:0] and set to PERI_ECC_CTL.WORD_ADDR. */
    uint32_t wordAddr = CalculatePpuWordAddr(gTestPpuSuffix, gTestPpuFixed, gTestPpuMaster);
    PERI->unECC_CTL.stcField.u11WORD_ADDR = wordAddr;

    /* Generate the ECC parity and set to PERI_ECC_CTL.PARITY. */
    uint128_t word = MakeWordForPpuEccWithWoundingBit(&gTestAttValue ,wordAddr);
    uint8_t parity = mxambatk_ecc_128_120_parity(word);
    PERI->unECC_CTL.stcField.u8PARITY = parity;

    /* Set the PERI_ECC_CTL.ECC_INJ_EN to "1". */
    PERI->unECC_CTL.stcField.u1ECC_INJ_EN = 1u;

    /*********************************/
    /* Test 1: Reading without error */
    /*********************************/
    /* Read the target PPU structure ATT0-3. */
    (void)*att.pu32[0];
    (void)*att.pu32[1];
    (void)*att.pu32[2];
    (void)*att.pu32[3];

    /* Read the fault error source */
    cy_en_sysflt_source_t fault_source;
    fault_source = Cy_SysFlt_GetErrorSource(FAULT_STRUCT0);

    /* Expectation: no fault */
    CY_ASSERT(fault_source == CY_SYSFLT_NO_FAULT);

    /* Clear the fault status */
    Cy_SysFlt_ClearStatus(FAULT_STRUCT0);

    /********************************************************/
    /* Test 2: Reading with 1-bit error (correctable error) */
    /********************************************************/
    /* Set attribute value with 1-bit being inverted (Correctable error). */
    *att.pu32[0] = gTestAttValue.att[0] ^ 0x00001000ul;

    /* Write the correct value. Note that because the ATT has 128 bits,
       only 32-bits writing initiates read-modify-write.
       Thus, this instruction contains reading which will cause correctable ECC error. */
    *att.pu32[0] = gTestAttValue.att[0];

    /* Read the fault error source */
    do
    {
        fault_source = Cy_SysFlt_GetErrorSource(FAULT_STRUCT0);
    } while(fault_source == CY_SYSFLT_NO_FAULT);

    /* Check the error code. Expectation: correctable ECC error */
    CY_ASSERT(fault_source == CY_SYSFLT_PERI_PERI_C_ECC);

    /* Clear the fault status */
    Cy_SysFlt_ClearStatus(FAULT_STRUCT0);

    /*************************************************************/
    /* Test 3: Reading with 2-bits error (non-correctable error) */
    /*************************************************************/
    /* Set attribute value with 2-bits being inverted (Non-correctable error). */
    *att.pu32[0] = gTestAttValue.att[0] ^ 0x00101000ul;

    /* Write the correct value. Note that because the ATT has 128 bits,
       only 32-bits writing initiates read-modify-write.
       Thus, this instruction contains reading which will cause non-correctable ECC error. */
    *att.pu32[0] = gTestAttValue.att[0];

    /* Read the fault error source */
    do
    {
        fault_source = Cy_SysFlt_GetErrorSource(FAULT_STRUCT0);
    } while(fault_source == CY_SYSFLT_NO_FAULT);

    /* Check the error code. Expectation: non-correctable ECC error */
    CY_ASSERT(fault_source == CY_SYSFLT_PERI_PERI_NC_ECC);

    /* Clear the fault status */
    Cy_SysFlt_ClearStatus(FAULT_STRUCT0);

    for(;;);
}

/*******************************************************************************
* Function Name: GetTargetAtt
****************************************************************************//**
*
* This function returns a structure which contains pointers to PPU ATT registers
* which are indicated by input parameters.
*
* \param suffix PPU ATT register suffix.
*
* \param isFixed input true for fixed PPU, false for programmable PPU
*
* \param isMaster input true for master PPU, false for slave PPU
*
* \return \ref stc_ppu_attribute_p_t
*
*******************************************************************************/
static stc_ppu_attribute_p_t GetTargetAtt(uint32_t suffix, bool isFixed, bool isMaster)
{
    stc_ppu_attribute_p_t att = {NULL};
    if(isFixed== true)
    {
        if(isMaster == true)
        {
            att.pu32[0] = &PERI_MS->PPU_FX[suffix].unMS_ATT0.u32Register;
            att.pu32[1] = &PERI_MS->PPU_FX[suffix].unMS_ATT1.u32Register;
            att.pu32[2] = &PERI_MS->PPU_FX[suffix].unMS_ATT2.u32Register;
            att.pu32[3] = &PERI_MS->PPU_FX[suffix].unMS_ATT3.u32Register;
        }
        else
        {
            att.pu32[0] = &PERI_MS->PPU_FX[suffix].unSL_ATT0.u32Register;
            att.pu32[1] = &PERI_MS->PPU_FX[suffix].unSL_ATT1.u32Register;
            att.pu32[2] = &PERI_MS->PPU_FX[suffix].unSL_ATT2.u32Register;
            att.pu32[3] = &PERI_MS->PPU_FX[suffix].unSL_ATT3.u32Register;
        }
    }
    else
    {
        if(isMaster == true)
        {
            att.pu32[0] = &PERI_MS->PPU_PR[suffix].unMS_ATT0.u32Register;
            att.pu32[1] = &PERI_MS->PPU_PR[suffix].unMS_ATT1.u32Register;
            att.pu32[2] = &PERI_MS->PPU_PR[suffix].unMS_ATT2.u32Register;
            att.pu32[3] = &PERI_MS->PPU_PR[suffix].unMS_ATT3.u32Register;
        }
        else
        {
            att.pu32[0] = &PERI_MS->PPU_PR[suffix].unSL_ATT0.u32Register;
            att.pu32[1] = &PERI_MS->PPU_PR[suffix].unSL_ATT1.u32Register;
            att.pu32[2] = &PERI_MS->PPU_PR[suffix].unSL_ATT2.u32Register;
            att.pu32[3] = &PERI_MS->PPU_PR[suffix].unSL_ATT3.u32Register;
        }
    }

    return att;
}


/*******************************************************************************
* Function Name: CalculatePpuWordAddr
****************************************************************************//**
*
* This function calculate a WORD_ADDR for the ECC injection
* E.g. To get the WORD_ADDR for PERI_MS_PPU_FX2_MS_ATT[0:3] then, input
* suffix = 2,  isFixed = true,  isMaster = true
* The ADDR can be calculated as follows.
* PERI_MS_PPU_PRx_SL_ATT0-3: x * 2
* PERI_MS_PPU_PRx_MS_ATT0-3: x * 2 + 1
* PERI_MS_PPU_FXx_SL_ATT0-3: x * 2 + 64
* PERI_MS_PPU_FXx_MS_ATT0-3: x * 2 + 65
*
* \param suffix PPU ATT register suffix.
*
* \param isFixed input true for fixed PPU, false for programmable PPU
*
* \param isMaster input true for master PPU, false for slave PPU
*
* \return calculated PPU word address
*
*******************************************************************************/
static uint32_t CalculatePpuWordAddr(uint32_t suffix, bool isFixed, bool isMaster)
{
    uint32_t offset = 0ul;
    offset += (isFixed  ? 64 : 0);
    offset += (isMaster ? 1  : 0);

    return (suffix * 2ul) + offset;
}

/*******************************************************************************
* Function Name: MakeActualWord
****************************************************************************//**
*
* This function calculates ACTUAL_WORD which is used for ECC generation
* for PPU register.
* E.g.
* ACTUAL_WORD[4:0] = PC1
* ACTUAL_WORD[9:5] = PC2
* ...
* ACTUAL_WORD[74:40] = PC15
* Note the PC[0] isn't used for the ECC calculation.
*
* \param stc_ppu_all_attribute_t* pointer to PPU ATT value.
*
* \param isFixed input true for fixed PPU, false for programmable PPU
*
* \param isMaster input true for master PPU, false for slave PPU
*
* \return calculated PPU word address
*
*******************************************************************************/
static uint128_t MakeActualWord(const stc_ppu_all_attribute_t* attValue)
{
    /* Make PC[1] ~ PC[15] */
    uint8_t pc[16];
    for(uint32_t pcIdx = 1; pcIdx < 16; pcIdx += 1)
    {
        uint32_t bitPos = (pcIdx*8) % 32;
        uint32_t attSuffix = pcIdx / 4;
        pc[pcIdx] = (attValue->att[attSuffix] >> bitPos) & 0x1F;
    }

    /* Make ACTUAL_WORD form the PC0~PC15 */
    /* Note PC[0] is not used */
    uint128_t acturalWord = {0ul};
    acturalWord.u64[0] =  (uint64_t)pc[1]           |
                         ((uint64_t)pc[2]  << 5ul)  |
                         ((uint64_t)pc[3]  << 10ul) |
                         ((uint64_t)pc[4]  << 15ul) |
                         ((uint64_t)pc[5]  << 20ul) |
                         ((uint64_t)pc[6]  << 25ul) |
                         ((uint64_t)pc[7]  << 30ul) |
                         ((uint64_t)pc[8]  << 35ul) |
                         ((uint64_t)pc[9]  << 40ul) |
                         ((uint64_t)pc[10] << 45ul) |
                         ((uint64_t)pc[11] << 50ul) |
                         ((uint64_t)pc[12] << 55ul) |
                         ((uint64_t)pc[13] << 60ul);
    acturalWord.u64[1] = ((uint64_t)pc[13] >> 4ul) |
                         ((uint64_t)pc[14] << 1ul) |
                         ((uint64_t)pc[15] << 6ul);

    return acturalWord;
}

/*******************************************************************************
* Function Name: MakeWordForPpuEccWithWoundingBit
****************************************************************************//**
*
* This function calculate a word for the ECC generation.
* word[74:0]   = ACTUAL_WORD[74:0]
* word[75]     = 0 (wounding bit);
* word[86:76]  = ADDR[10:0]
* word[127:77] = 0
*
* \param stc_ppu_all_attribute_t PPU ATT register suffix.
*
* \param isFixed input true for fixed PPU, false for programmable PPU
*
* \param isMaster input true for master PPU, false for slave PPU
*
* \return calculated PPU word address
*
*******************************************************************************/
static uint128_t MakeWordForPpuEccWithWoundingBit(const stc_ppu_all_attribute_t* attValue, uint32_t ppuWordAddr)
{
    /* CODEWORD_SW [74:0] = ACTUALWORD [74:0]; */
    /* Other bits = 0 */
    uint128_t word = MakeActualWord(attValue);

    /* CODEWORD_SW [75] = 0 (wounding bit); */
    // nothing to do

    /* CODEWORD_SW [86: 76] = ADDR [10:0]; */ 
    word.u64[1] |= (ppuWordAddr & 0x000003FFul) << (76ul - 64ul);

    return word;
}

/*******************************************************************************
* Function Name: mxambatk_ecc_128_120_parity
****************************************************************************//**
*
* This function returns a parity made by mxambatk_ecc_128_120_parity
* algorithm for input 128 bit word.
* mxambatk_ecc_128_120_parity algorithm is shown in below.
*
* CW = input word
* ECC_P0 = 0x01bf_bb75_be3a_72dc_4484_4a88_952a_ad5b
* ECC_P1 = 0x02df_76f9_dd99_b971_1108_9311_26b3_366d
* ECC_P2 = 0x04ef_cf9f_9ad5_ce97_0611_1c22_38c3_c78e
* ECC_P3 = 0x08f7_ecf6_ed67_4e6c_9821_e043_c0fc_07f0
* ECC_P4 = 0x10fb_7baf_6ba6_b5a6_e03e_007c_00ff_f800
* ECC_P5 = 0x20fd_b7ce_f36c_ab5b_ffc0_007f_ff00_0000
* ECC_P6 = 0x40fe_dd7b_74db_55ab_ffff_ff80_0000_0000
* ECC_P7 = 0x807f_0000_07ff_ffff_d442_2584_4ba6_5cb7
* parity[0] = ^ (CW & ECC_P0)
* parity[1] = ^ (CW & ECC_P1)
* ...
* parity[7] = ^ (CW & ECC_P7)
* Note: “^” represents reduction XOR
*
* \param word data value to be calculated of its ECC 
*
* \return calculated ECC (parity) value
*
*******************************************************************************/
static uint8_t mxambatk_ecc_128_120_parity(uint128_t word)
{
    static uint128_t ECC_P[8] =
    {
        {0x44844a88952aad5b, 0x01bfbb75be3a72dc},
        {0x1108931126b3366d, 0x02df76f9dd99b971},
        {0x06111c2238c3c78e, 0x04efcf9f9ad5ce97},
        {0x9821e043c0fc07f0, 0x08f7ecf6ed674e6c},
        {0xe03e007c00fff800, 0x10fb7baf6ba6b5a6},
        {0xffc0007fff000000, 0x20fdb7cef36cab5b},
        {0xffffff8000000000, 0x40fedd7b74db55ab},
        {0xd44225844ba65cb7, 0x807f000007ffffff},
    };

    uint8_t ecc = 0;
    for(uint32_t i=0; i < 8; i++)
    {
        ecc |= (ReductionXOR128(And128bit(word, ECC_P[i])) << i);
    }

    return ecc;
}


/*******************************************************************************
* Function Name: And128bit
****************************************************************************//**
*
* This function takes logical AND for input uint128_t value.
*
* \param a value to be taken AND with another input value
*
* \param b value to be taken AND with another input value
*
* \return calculated AND value.
*
*******************************************************************************/
static uint128_t And128bit(uint128_t a, uint128_t b)
{
    uint128_t result;
    result.u64[0] = a.u64[0] & b.u64[0];
    result.u64[1] = a.u64[1] & b.u64[1];
    return result;
}


/*******************************************************************************
* Function Name: ReductionXOR128
****************************************************************************//**
*
* Reduction XOR: Taking XOR for all bit input
* e.g. ReductionXOR(4'b0011) = 0 ^ 0 ^ 1 ^ 1 = 0
*
* \param data reduction XOR of this value will be calculated
*
* \return calculated reduction XOR.
*
*******************************************************************************/
static uint8_t ReductionXOR128(uint128_t data)
{
    uint64_t parity = 0;
    uint64_t bit    = 0;
    for(uint64_t i_pos = 0; i_pos < 64; i_pos++)
    {
        bit = (data.u64[0] & (1ull << i_pos)) >> i_pos;
        parity ^= bit;
    }

    for(uint64_t i_pos = 0; i_pos < 64; i_pos++)
    {
        bit = (data.u64[1] & (1ull << i_pos)) >> i_pos;
        parity ^= bit;
    }

    return (uint8_t)parity;
}

/* [] END OF FILE */
