# Note: Build config dependent options are specified in corresponding toolchain file!
#       (e.g. generation of debug symbols, optimization level, NDEBUG define)

###################################################################################################
# C Compiler
###################################################################################################

set(C_COMPILER_OPTIONS
    -Xdialect-c99
    #-\#
)

set(C_COMPILER_OPTIONS_cm0plus
    -tARMCORTEXM0PLS:simple
)

set(C_COMPILER_OPTIONS_cm4
    -tARMCORTEXM4LF:simple
)

set(C_COMPILER_OPTIONS_cm7
    -tARMCORTEXM7LV:simple
)


###################################################################################################
# CXX Compiler
###################################################################################################

set(CXX_COMPILER_OPTIONS

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

# Assembler will be called through compiler driver, so some options may need to be prepended with -Wa,
set(ASSEMBLER_OPTIONS
    -Xpreprocess-assembly
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
# Linker will be called through compiler driver, so some options may need to be prepended with -Wl
set(LINKER_OPTIONS
    -Xdialect-c99 # somehow needed
    -Wl,-Xpreprocess-lecl
    -Wl,-Xgenerate-copytables    
    -Ws
    -e_start
    #-Wl,-m # Map file option, only on STDOUT?!
)
set(LINKER_OPTIONS_cm0plus
    ${C_COMPILER_OPTIONS_cm0plus}
)
set(LINKER_OPTIONS_cm4
    ${C_COMPILER_OPTIONS_cm4}
)
set(LINKER_OPTIONS_cm7
    ${C_COMPILER_OPTIONS_cm7}
)


#--------------------------------------------------------------------------------------------------
# Additional toolchain specific libraries to link
set(TOOLCHAIN_LIBS) # No toolchain libs need to be linked explicitly in case of DIAB

# No special options required before/after the libraries to link are passed on the command line
set(LINKER_OPTION_PRE_LINK_LIBS)
set(LINKER_OPTION_POST_LINK_LIBS)

#--------------------------------------------------------------------------------------------------
set(LINKER_FILE ${CMAKE_SOURCE_DIR}/${DIE}/tools/diab/linker_directives.dld)

# Link step of executable will depend on these files
set(LINK_DEPENDENCY_FILES ${LINKER_FILE})

# Returns the linker option specific to the linker file(s). The function implementation and parameters allow
# a target / CPU Instance specific handling of the linker file (e.g. as required by GCC to preprocess the linker file)
# Note: It is recommended to pass linkerDefines as a string, e.g.: "${LIST_OF_LINKER_DEFINES}"
function(get_linker_file_option outVar targetName cpuTypeInstance linkerDefines)
    
    # No special handling needed

    # Return the linker option that specifies the linker file
    set(${outVar} -Wm${LINKER_FILE} PARENT_SCOPE)

endfunction()

