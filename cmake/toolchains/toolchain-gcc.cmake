set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

set(GCC TRUE)
set(TOOLCHAIN_ASM_EXT   .S) # Primary assembly file extension
set(TOOLCHAIN_ID        gcc)
string(TOUPPER ${TOOLCHAIN_ID} TOOLCHAIN_ID_UPPERCASE)

# Set all known assembly file extensions, some SW (e.g. MCAL) may not have toolchain specific assembly files but rather use one file (with ifdefs) for multiple toolchains
set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS arm;s;S;asm)

######################################################################################
# Toolchain paths, archiver settings and include directories
######################################################################################
# Check tool_config.cmake for variables that set the tool's base path

# Set up the CMake variables for compiler and assembler
set(CMAKE_C_COMPILER   "${GCC_COMPILER_ROOT_DIR}/bin/arm-none-eabi-gcc.exe"        CACHE STRING "")
set(CMAKE_CXX_COMPILER "${GCC_COMPILER_ROOT_DIR}/bin/arm-none-eabi-g++.exe"        CACHE STRING "")
set(CMAKE_ASM_COMPILER "${GCC_COMPILER_ROOT_DIR}/bin/arm-none-eabi-gcc.exe"        CACHE STRING "")
set(CMAKE_AR           "${GCC_COMPILER_ROOT_DIR}/bin/arm-none-eabi-gcc-ar.exe"     CACHE STRING "")
set(CMAKE_NM           "${GCC_COMPILER_ROOT_DIR}/bin/arm-none-eabi-gcc-nm.exe"     CACHE STRING "")
set(CMAKE_RANLIB       "${GCC_COMPILER_ROOT_DIR}/bin/arm-none-eabi-gcc-ranlib.exe" CACHE STRING "")

set(TOOLCHAIN_INCLUDE_DIRS
    ${GCC_COMPILER_ROOT_DIR}/arm-none-eabi/include
)

# The tool provided by the toolchain (or any other tool) that is used to convert the ELF file to an S-Record file
set(TOOLCHAIN_ELF_CONVERTER "${GCC_COMPILER_ROOT_DIR}/bin/arm-none-eabi-objcopy.exe" CACHE STRING "")

######################################################################################
# Build config dependent flags
# (for the 4 default CMake build configs: Debug, Release, RelWithDebInfo, MinSizeRel)
######################################################################################

# Note: Using the corresponding ..._INIT variables would lead to appending of the flags, but we want to explicitly define all flags here
set(CMAKE_C_FLAGS_DEBUG          "-Og -g3"                      CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE        "-O3 -flto -DNDEBUG"           CACHE STRING "")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELEASE} -g3" CACHE STRING "")
set(CMAKE_C_FLAGS_MINSIZEREL     "-Os -flto -DNDEBUG"           CACHE STRING "")

set(CMAKE_CXX_FLAGS_DEBUG          ${CMAKE_C_FLAGS_DEBUG}          CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE        ${CMAKE_C_FLAGS_RELEASE}        CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO} CACHE STRING "")
set(CMAKE_CXX_FLAGS_MINSIZEREL     ${CMAKE_C_FLAGS_MINSIZEREL}     CACHE STRING "")

set(CMAKE_ASM_FLAGS_DEBUG          ${CMAKE_C_FLAGS_DEBUG}          CACHE STRING "")
set(CMAKE_ASM_FLAGS_RELEASE        ${CMAKE_C_FLAGS_RELEASE}        CACHE STRING "")
set(CMAKE_ASM_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO} CACHE STRING "")
set(CMAKE_ASM_FLAGS_MINSIZEREL     ${CMAKE_C_FLAGS_MINSIZEREL}     CACHE STRING "")


######################################################################################
# Toolchain specific helper functions
######################################################################################

# Converts the given "defines" into the toolchain specific format for passing them to the linker and assigns them to the given 'outVar' variable
# Example: get_converted_linker_defines(MY_LINKER_DEFS  THIS_IS_A_DEFINE_WITHOUT_VALUE  THIS_IS_A_DEFINE_WITH_VALUE=1)
function(get_converted_linker_defines  outVar)
    if(ARGN)
        foreach(arg IN LISTS ARGN)
            set(temp ${temp} "-D${arg}")
        endforeach()
        set(${outVar} ${temp} PARENT_SCOPE)
    endif()
endfunction()


# Constructs the argument string that is required when calling the toolchain ELF file converter and assigns it to the given 'outVar' variable
# Example: get_elf_converter_args(ELF_CONVERTER_ARGS  <path_to>/<ELF-file>)
function(get_elf_converter_args  outVar elfFile)
    set(${outVar} -O srec ${elfFile} ${elfFile}.srec  PARENT_SCOPE)
endfunction()
