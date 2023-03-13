set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

set(IAR TRUE)
set(TOOLCHAIN_ASM_EXT   .s) # Primary assembly file extension
set(TOOLCHAIN_ID        iar)
string(TOUPPER ${TOOLCHAIN_ID} TOOLCHAIN_ID_UPPERCASE)

# Set all known assembly file extensions, some SW (e.g. MCAL) may not have toolchain specific assembly files but rather use one file (with ifdefs) for multiple toolchains
set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS arm;s;S;asm)

######################################################################################
# Toolchain paths, archiver settings and include directories
######################################################################################
# Check tool_config.cmake for variables that set the tool's base path

# Set up the CMake variables for compiler and assembler
set(CMAKE_C_COMPILER   "${IAR_COMPILER_ROOT_DIR}/arm/bin/iccarm.exe"   CACHE STRING "")
set(CMAKE_CXX_COMPILER "${IAR_COMPILER_ROOT_DIR}/arm/bin/iccarm.exe"   CACHE STRING "")
set(CMAKE_ASM_COMPILER "${IAR_COMPILER_ROOT_DIR}/arm/bin/iasmarm.exe"  CACHE STRING "")
set(CMAKE_AR           "${IAR_COMPILER_ROOT_DIR}/arm/bin/iarchive.exe" CACHE STRING "")
# IAR specific archiver command rules (builtin rules do not match IAR spec)
# Seems like changing below CMAKE variables is discouraged but there seems to be no other way to make it work...
set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> --create  <TARGET> <OBJECTS>")
set(CMAKE_C_ARCHIVE_APPEND "<CMAKE_AR> --replace <TARGET> <OBJECTS>")
set(CMAKE_C_ARCHIVE_FINISH)
set(CMAKE_CXX_ARCHIVE_CREATE ${CMAKE_C_ARCHIVE_CREATE})
set(CMAKE_CXX_ARCHIVE_APPEND ${CMAKE_C_ARCHIVE_APPEND})
set(CMAKE_CXX_ARCHIVE_FINISH ${CMAKE_C_ARCHIVE_FINISH})
# Update compile rules to ensure that dependency files are generated with a proper name (because naming rule to generate Ninja DEP_FILE entries are hardcoded in CMake)
set(CMAKE_C_COMPILE_OBJECT   "<CMAKE_C_COMPILER>   <DEFINES> <INCLUDES> <FLAGS> --dependencies=n <OBJECT>.d -o <OBJECT> -c <SOURCE>")
set(CMAKE_CXX_COMPILE_OBJECT "<CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> <FLAGS> --dependencies=n <OBJECT>.d -o <OBJECT> -c <SOURCE>")
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <DEFINES> <INCLUDES> <FLAGS> --dependencies=n <OBJECT>.d -o <OBJECT> -c <SOURCE>")

set(TOOLCHAIN_INCLUDE_DIRS
    ${IAR_COMPILER_ROOT_DIR}/arm/inc/c
)

# The tool provided by the toolchain (or any other tool) that is used to convert the ELF file to an S-Record file
set(TOOLCHAIN_ELF_CONVERTER "${IAR_COMPILER_ROOT_DIR}/arm/bin/ielftool.exe" CACHE STRING "")

######################################################################################
# Build config dependent flags
# (for the 4 default CMake build configs: Debug, Release, RelWithDebInfo, MinSizeRel)
######################################################################################

# Note: Using the corresponding ..._INIT variables leads to appending of the flags, but we need to clear the ones implicitly defined by CMake as they are wrong
set(CMAKE_C_FLAGS_DEBUG          "-Ol --debug"                         CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE        "-Ohs --no_size_constraints -DNDEBUG" CACHE STRING "")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELEASE} --debug"    CACHE STRING "")
set(CMAKE_C_FLAGS_MINSIZEREL     "-Ohz -DNDEBUG"                       CACHE STRING "")

set(CMAKE_CXX_FLAGS_DEBUG          ${CMAKE_C_FLAGS_DEBUG}          CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE        ${CMAKE_C_FLAGS_RELEASE}        CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO} CACHE STRING "")
set(CMAKE_CXX_FLAGS_MINSIZEREL     ${CMAKE_C_FLAGS_MINSIZEREL}     CACHE STRING "")

set(CMAKE_ASM_FLAGS_DEBUG          "-r"                            CACHE STRING "")
set(CMAKE_ASM_FLAGS_RELEASE        ""                              CACHE STRING "")
set(CMAKE_ASM_FLAGS_RELWITHDEBINFO "${CMAKE_ASM_FLAGS_RELEASE} -r" CACHE STRING "")
set(CMAKE_ASM_FLAGS_MINSIZEREL     ""                              CACHE STRING "")


######################################################################################
# Toolchain specific helper functions
######################################################################################

# Converts the given "defines" into the toolchain specific format for passing them to the linker and assigns them to the given 'outVar' variable
# Example: get_converted_linker_defines(MY_LINKER_DEFS  THIS_IS_A_DEFINE_WITHOUT_VALUE  THIS_IS_A_DEFINE_WITH_VALUE=1)
function(get_converted_linker_defines  outVar)
    if(ARGN)
        foreach(arg IN LISTS ARGN)
            # IAR does not support linker defines without value, so if the given define does not include a value we assign it a dummy "1" value
            if(${arg} MATCHES "=")
                set(value_suffix "")
            else()
                set(value_suffix "=1")
            endif()
            set(temp ${temp} "SHELL:--config_def ${arg}${value_suffix}") # SHELL keyword is required to avoid later de-duplication of --config_def by CMake when multiple defines are used                
        endforeach()
        set(${outVar} ${temp} PARENT_SCOPE)
    endif()
endfunction()


# Constructs the argument string that is required when calling the toolchain ELF file converter and assigns it to the given 'outVar' variable
# Example: get_elf_converter_args(ELF_CONVERTER_ARGS  <path_to>/<ELF-file>)
function(get_elf_converter_args  outVar elfFile)
    set(${outVar} ${elfFile} ${elfFile}.srec --silent --srec --srec-len 16  PARENT_SCOPE)
endfunction()
