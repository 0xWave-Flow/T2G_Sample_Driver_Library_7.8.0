# Note: Build config dependent options are specified in corresponding toolchain file!
#       (e.g. generation of debug symbols, optimization level, NDEBUG define)


###################################################################################################
# C Compiler
###################################################################################################

set(C_COMPILER_OPTIONS
    -dual_debug
    -thumb
    -thumb_lib
    -C99
    --short_enum
    -align4
    --no_commons
    --no_alternative_tokens
    -asm3g
    -preprocess_assembly_files
    --preprocess_linker_directive_full
    -nostartfiles
    -globalcheck=normal
    -globalcheck_qualifiers
    --prototype_errors
    -Wformat
    -Wimplicit-int
    -Wshadow
    -Wtrigraphs
    -Wundef
    -reject_duplicates
    -list
    --no_wrap_diagnostics
)

set(C_COMPILER_OPTIONS_cm0plus
    -cpu=cortexm0plus
    -fsoft
)

set(C_COMPILER_OPTIONS_cm4
    -cpu=cortexm4f
    -fsingle
)

set(C_COMPILER_OPTIONS_cm7
    -cpu=cortexm7
    -fhard
)


###################################################################################################
# CXX Compiler
###################################################################################################

set(CXX_COMPILER_OPTIONS
    ${C_COMPILER_OPTIONS}
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
    ${C_COMPILER_OPTIONS}
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
    ${C_COMPILER_OPTIONS}
    ${LINKER_FILE}
)
# No CPU specific linker options needed for GHS
set(LINKER_OPTIONS_cm0plus)
set(LINKER_OPTIONS_cm4)
set(LINKER_OPTIONS_cm7)


#--------------------------------------------------------------------------------------------------
# Additional toolchain specific libraries to link
set(TOOLCHAIN_LIBS) # No toolchain libs need to be linked explicitly in case of GHS

# No special options required before/after the libraries to link are passed on the command line
set(LINKER_OPTION_PRE_LINK_LIBS)
set(LINKER_OPTION_POST_LINK_LIBS)

#--------------------------------------------------------------------------------------------------
set(LINKER_FILE
    ${CMAKE_SOURCE_DIR}/${DIE}/tools/ghs/memory_map.ld
    ${CMAKE_SOURCE_DIR}/${DIE}/tools/ghs/linker_directives.ld
)

# Link step of executable will depend on these files
set(LINK_DEPENDENCY_FILES ${LINKER_FILE})

# Returns the linker option specific to the linker file(s). The function implementation and parameters allow
# a target / CPU Instance specific handling of the linker file (e.g. as required by GCC to preprocess the linker file)
# Note: It is recommended to pass linkerDefines as a string, e.g.: "${LIST_OF_LINKER_DEFINES}"
function(get_linker_file_option outVar targetName cpuTypeInstance linkerDefines)
    
    # No special handling needed

    # Return the linker option that specifies the linker file
    set(${outVar} ${LINKER_FILE} PARENT_SCOPE)

endfunction()

