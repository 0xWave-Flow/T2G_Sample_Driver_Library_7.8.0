/* __DISCLAIMER_START__                                                      */
/******************************************************************************
* (c) 2014-2017, Cypress Semiconductor Corporation
* or a subsidiary of Cypress Semiconductor Corporation. All rights
* reserved.
*
* This software, including source code, documentation and related
* materials ( "Software" ), is owned by Cypress Semiconductor
* Corporation or one of its subsidiaries ( "Cypress" ) and is protected by
* and subject to worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ( "EULA" ).
*
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the
* Software source code solely for use in connection with Cypress' s
* integrated circuit products. Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO
* WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING,
* BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE. Cypress reserves the right to make
* changes to the Software without notice. Cypress does not assume any
* liability arising out of the application or use of the Software or any
* product or circuit described in the Software. Cypress does not
* authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ( "High Risk Product" ). By
* including Cypress' s product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
******************************************************************************/
/* __DISCLAIMER_END__                                                        */
/*****************************************************************************/

// Check that necessary definitions are available
#if (!defined(_LINK_flash_)) && (!defined(_LINK_sram_))
  #error "Link location not defined or not supported!"
#endif

#if (!defined(_CORE_cm0plus_)) && (!defined(_CORE_cm7_0_)) && (!defined(_CORE_cm7_1_))
  #error "Target core not defined or not supported!"
#endif

#if defined(_CORE_cm7_0_) || defined(_CORE_cm7_1_)
  #define _CORE_cm7_x_
#endif

//=======================================================================================
// memory_map.ld contains symbol and memory area definitions used by this linker file
//=======================================================================================


//============================================================================================================
// CONFIGURATION
//------------------------------------------------------------------------------------------------------------

// Select whether the CM7 stack and heap are linked to SRAM or DTCM, by uncommenting or commenting the following define.
// Note: Some examples/drivers are not yet supporting DTCM linkage because local variables (stack) will need to
// be accessed by other masters (e.g. DMA) and address conversions for DTCM addresses are missing

#define LINK_CM7_STACK_HEAP_TO_SRAM_NOT_DTCM

//============================================================================================================

//
// Program layout
//
SECTIONS
{

    //============================================================================================================
    // ROM type sections
    //------------------------------------------------------------------------------------------------------------
    
  #if   defined(_LINK_flash_)
    #define MEMORY_FOR_ROM_TYPE_SECTIONS     CODE_FLASH
  #elif defined(_LINK_sram_)
    #define MEMORY_FOR_ROM_TYPE_SECTIONS     SRAM
  #endif  
    

    // Note: .intvec must be the first section in ROM in order for __cmx_vector_base_linker_symbol to be correctly calculated!

    .intvec          ALIGN(intvec_alignment)                     : > MEMORY_FOR_ROM_TYPE_SECTIONS
    .picbase                                                     : > .
    .text                                                        : > .
    .intercall                                                   : > .
    .interfunc                                                   : > .
    .fixaddr                                                     : > .
    .fixtype                                                     : > .
    .secinfo                                                     : > .
    .robase                                                      : > .
    .rodata                                                      : > .
    .ROM.syscall     ROM(.syscall)                               : > .
    .ROM.data        ROM(.data)                                  : > .
    
  #if defined(_LINK_sram_)
    // Cypress (ROM and Flash) boot firmware needs an area in SRAM (e.g. for stack). This may cause issues in case of
    // RAM build configurations when code/const sections are loaded there by the debugger, but may get overwritten
    // if reset is triggered by the debug script later on. So in case of RAM build configurations the following
    // statement checks if the area of ROM type sections overlaps the boot area.
    // Comment the statement if the sections shall be linked there anyway.
    //
    // A simple check is sufficient because the boot area is from SRAM_MAX-6KB to SRAM_MAX-2KB and it is unlikely that the ROM sections would only
    // start in the last 2KB (which are anyway not accessible in early MCU revisions because of private SROM RAM area)
    //
    // !!! This statement should be the last entry in the ROM type sections !!! (Note: The symbol will not be used, it is just used to implement the error check)
    
    __rom_type_sections_end_address = (. < sram_used_by_boot_base) ? . : error("ROM type sections are overlapping the area used by Cypress boot firmware. Check comment in linker file!");    
  #endif

    //============================================================================================================
    // RAM type sections
    //------------------------------------------------------------------------------------------------------------
    
    // >>> SRAM <<<
    
    .sram_base       ALIGN(ecc_init_width)                       : > SRAM
    .pidbase         ALIGN(8)                                    : > .
    .data            ALIGN(8)                                    : > .
    .bss             ALIGN(8)                            CLEAR   : > . // can be changed to NOCLEAR when RAM ECC initialization is enabled in startup_customize.h to improve startup time
    .intvec_ram      ALIGN(intvec_alignment)             NOCLEAR : > .
    .syscall         ALIGN(8)                                    : > .
  #if defined(_CORE_cm0plus_) || defined(LINK_CM7_STACK_HEAP_TO_SRAM_NOT_DTCM)
    .heap            ALIGN(8)                 PAD(heap_reserve)  : > .
    .stack           ALIGN(ecc_init_width)    PAD(stack_reserve) : > .
  #endif

    
  #ifdef _CORE_cm7_x_
  
    // >>> DTCM <<<
    
    .dtcm_base                                                   : > SELF_DTCM
    // Placeholder: add/move sections for DTCM address space here
   #if (!defined(LINK_CM7_STACK_HEAP_TO_SRAM_NOT_DTCM))
    .heap            ALIGN(8)                 PAD(heap_reserve)  : > .
    .stack           ALIGN(ecc_init_width)    PAD(stack_reserve) : > .
   #endif
    
    // >>> ITCM <<<
    
    .itcm_base                                                   : > SELF_ITCM
    // Placeholder: add/move sections for ITCM address space here
    // (Note: corresponding .ROM.xyz sections are needed as well to copy section contents on startup)
    
  #endif


    //============================================================================================================
    // These special symbols mark the bounds of RAM and ROM memory.
    // They are used by the MULTI debugger.
    //------------------------------------------------------------------------------------------------------------
    __ghs_romstart = MEMADDR(CODE_FLASH);
    __ghs_romend   = MEMENDADDR(SFLASH_ALT);
    __ghs_ramstart = MEMADDR(SRAM);
    __ghs_ramend   = MEMENDADDR(SRAM);

    //============================================================================================================
    // These special symbols help GHS to identify the run type of this build: RAM, ROM, ROM-to-RAM
    //------------------------------------------------------------------------------------------------------------
  #if   defined(_LINK_flash_)
    __ghs_rombootcodestart = MEMADDR(CODE_FLASH);
    __ghs_rombootcodeend   = MEMENDADDR(CODE_FLASH);
    __ghs_rambootcodestart = 0;
    __ghs_rambootcodeend   = 0;
  #elif defined(_LINK_sram_)
    __ghs_rombootcodestart = 0;
    __ghs_rombootcodeend   = 0;
    __ghs_rambootcodestart = 0;
    __ghs_rambootcodeend   = 0;
  #endif

    //============================================================================================================
    // The start of the main CPU(s) vector table is required by CM0+ application to correctly
    // set CPUSS->CMx_VECTOR_TABLE_BASE register before releasing the main CPU(s) from reset
    // Following statements also ensure that the created symbol has the correct alignment
    //------------------------------------------------------------------------------------------------------------
  #ifdef _CORE_cm0plus_
      #if   defined(_LINK_flash_)
        __cm7_0_rom_start = MEMADDR(CODE_FLASH) - cm0plus_code_flash_offset + cm7_0_code_flash_offset;
        __cm7_1_rom_start = MEMADDR(CODE_FLASH) - cm0plus_code_flash_offset + cm7_1_code_flash_offset;
      #elif defined(_LINK_sram_)
        __cm7_0_rom_start = MEMADDR(SRAM)       - cm0plus_sram_offset       + cm7_0_sram_offset;
        __cm7_1_rom_start = MEMADDR(SRAM)       - cm0plus_sram_offset       + cm7_1_sram_offset;      
      #endif
    __cm7_0_vector_base_linker_symbol = (__cm7_0_rom_start + cm7_intvec_alignment - 1) & ~(cm7_intvec_alignment - 1);
    __cm7_1_vector_base_linker_symbol = (__cm7_1_rom_start + cm7_intvec_alignment - 1) & ~(cm7_intvec_alignment - 1);
  #endif

    //============================================================================================================
    // These symbols will be used by startup.c / startup_customize.h for RAM ECC initialisation
    //------------------------------------------------------------------------------------------------------------
  #if defined(_LINK_sram_)
    // If "ROM type" sections are linked to SRAM they shall not be cleared by the startup code
    __ecc_init_sram_start_address = ADDR(.sram_base); // The empty .sram_base section is a marker for the start of "RAM type" sections in SRAM
  #else
    __ecc_init_sram_start_address = MEMADDR(SRAM);
  #endif
    __ecc_init_sram_end_address   = MEMENDADDR(SRAM) - 1;
    
  #ifdef _CORE_cm7_x_
    __ecc_init_dtcm_start_address = MEMADDR(SELF_DTCM);
    __ecc_init_dtcm_end_address   = MEMENDADDR(SELF_DTCM) - 1;
    __ecc_init_itcm_start_address = MEMADDR(SELF_ITCM);
    __ecc_init_itcm_end_address   = MEMENDADDR(SELF_ITCM) - 1;
  #endif  
    

}
