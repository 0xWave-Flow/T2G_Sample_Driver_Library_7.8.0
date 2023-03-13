# Note: Build config dependent options are specified in corresponding toolchain file!
#       (e.g. generation of debug symbols, optimization level, NDEBUG define)


###################################################################################################
# C Compiler
###################################################################################################

set(C_COMPILER_OPTIONS
    --endian little
    -e
    --no_wrap_diagnostics
)

set(C_COMPILER_OPTIONS_cm0plus
    --cpu Cortex-M0+
)

set(C_COMPILER_OPTIONS_cm4
    --cpu Cortex-M4F
    --fpu VFPv4_sp
)

set(C_COMPILER_OPTIONS_cm7
    --cpu Cortex-M7
    --fpu VFPv5_d16
)


###################################################################################################
# CXX Compiler
###################################################################################################

set(CXX_COMPILER_OPTIONS
    ${C_COMPILER_OPTIONS}
    --c++
)

set(CXX_COMPILER_OPTIONS_cm0plus
    ${C_COMPILER_OPTIONS_cm0plus}
)
set(CXX_COMPILER_OPTIONS_cm4
    ${C_COMPILER_OPTIONS_cm4}
)
set(CXX_COMPILER_OPTIONS_cm7
    ${C_COMPILER_OPTIONS_cm7}
)


###################################################################################################
# Assembler
###################################################################################################

set(ASSEMBLER_OPTIONS
    --endian little
)

set(ASSEMBLER_OPTIONS_cm0plus
    ${C_COMPILER_OPTIONS_cm0plus}
)
set(ASSEMBLER_OPTIONS_cm4
    ${C_COMPILER_OPTIONS_cm4}
)
set(ASSEMBLER_OPTIONS_cm7
    ${C_COMPILER_OPTIONS_cm7}
)


###################################################################################################
# Linker
###################################################################################################

# Generic and CPU type specific linker options (does not include option to specify the linker file)
set(LINKER_OPTIONS
    --map ${CMAKE_BINARY_DIR}/$<CONFIG>/$<TARGET_PROPERTY:NAME>.map    
    --no_wrap_diagnostics
)
# No CPU specific linker options needed for IAR
set(LINKER_OPTIONS_cm0plus)
set(LINKER_OPTIONS_cm4)
set(LINKER_OPTIONS_cm7)

#--------------------------------------------------------------------------------------------------
# Additional toolchain specific libraries to link
set(TOOLCHAIN_LIBS) # No toolchain libs need to be linked explicitly in case of IAR

# No special options required before/after the libraries to link are passed on the command line
set(LINKER_OPTION_PRE_LINK_LIBS)
set(LINKER_OPTION_POST_LINK_LIBS)

#--------------------------------------------------------------------------------------------------
# Unfortunately, IAR linker file names are not generic or consistent, so the correct one needs to be determined based on several conditions
if((${DIE} STREQUAL tviibe1m) AND (${MCU_REV_STRING} STRGREATER_EQUAL rev_d))
    set(IAR_LINKER_FILENAME  linker_directives_tviibe_rev_d.icf)
elseif(${DIE} STREQUAL tviibe1m)
    set(IAR_LINKER_FILENAME  linker_directives_tviibe.icf)
elseif((${DIE} STREQUAL tviibe2m) AND (${MCU_REV_STRING} STRGREATER_EQUAL rev_c))
    set(IAR_LINKER_FILENAME  linker_directives_tviibe_rev_c.icf)
elseif(${DIE} STREQUAL tviibe2m)
    set(IAR_LINKER_FILENAME  linker_directives_tviibe.icf)
elseif(${DIE} STREQUAL tviibe4m)
    set(IAR_LINKER_FILENAME  linker_directives_${DIE}.icf)
elseif(${DIE} STREQUAL tviibe512k)
    set(IAR_LINKER_FILENAME  linker_directives_${DIE}.icf)
elseif(${DIE} STREQUAL tviibh4m)
    set(IAR_LINKER_FILENAME  linker_directives_tviibh.icf)
elseif((${DIE} STREQUAL tviibh8m) AND (${MCU_REV_STRING} STRGREATER_EQUAL rev_c))
    set(IAR_LINKER_FILENAME  linker_directives_tviibh_rev_c.icf)
elseif(${DIE} STREQUAL tviibh8m)
    set(IAR_LINKER_FILENAME  linker_directives_tviibh.icf)
elseif(${DIE} STREQUAL tviibh16m)
    set(IAR_LINKER_FILENAME  linker_directives.icf)
elseif(${DIE} STREQUAL tviic2d4m)
    set(IAR_LINKER_FILENAME  linker_directives_${DIE}.icf)
elseif((${DIE} STREQUAL tviic2d6m) AND (${MCU_REV_STRING} STRGREATER_EQUAL rev_b))
    set(IAR_LINKER_FILENAME  linker_directives_tviic2d_rev_b.icf)
elseif(${DIE} STREQUAL tviic2d6m)
    set(IAR_LINKER_FILENAME  linker_directives_tviic2d.icf)
elseif(${DIE} STREQUAL tviic2d6mddr)
    set(IAR_LINKER_FILENAME  linker_directives_${DIE}.icf)    
elseif(${DIE} STREQUAL tviice4m)
    set(IAR_LINKER_FILENAME  linker_directives_${DIE}.icf)
else()
    message(FATAL_ERROR "IAR Linker File could not be determined!")
endif()

set(LINKER_FILE ${CMAKE_SOURCE_DIR}/${DIE}/tools/iar/${IAR_LINKER_FILENAME})

message(STATUS "IAR Linker File determined: ${LINKER_FILE}")

# Link step of executable will depend on these files
set(LINK_DEPENDENCY_FILES ${LINKER_FILE})

# Returns the linker option specific to the linker file(s). The function implementation and parameters allow
# a target / CPU Instance specific handling of the linker file (e.g. as required by GCC to preprocess the linker file)
# Note: It is recommended to pass linkerDefines as a string, e.g.: "${LIST_OF_LINKER_DEFINES}"
function(get_linker_file_option outVar targetName cpuTypeInstance linkerDefines)
    
    # No special handling needed

    # Return the linker option that specifies the linker file
    set(${outVar} --config ${LINKER_FILE} PARENT_SCOPE)

endfunction()
