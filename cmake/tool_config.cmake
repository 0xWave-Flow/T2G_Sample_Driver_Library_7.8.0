##############################################################################################################################
# This file is used to configure paths and certain options for tools and add-on SW (compilers, debuggers, programmers, Tresos,
# MCAL, etc.) which may differ from user to user.
# 
# Two options exist for adjusting them:
#
# 1) Adjust paths and option values directly in this file
# 2) A mechanism is implemented that will check for the presence of corresponding environment variables which will then take
#    precedence over the configured settings in this file. The environment variables need to have the same name like the CMake
#    variable but prefixed with 'SDL_CMAKE_'.
#    Example:
#             CMake variable name:                 GHS_COMPILER_ROOT_DIR
#             Environment variable name: SDL_CMAKE_GHS_COMPILER_ROOT_DIR
#
##############################################################################################################################



######################################################################################
# GreenHills
######################################################################################
if(DEFINED ENV{SDL_CMAKE_GHS_COMPILER_ROOT_DIR})
    set(defaultGhsCompilerRootDir $ENV{SDL_CMAKE_GHS_COMPILER_ROOT_DIR})
else()
    set(defaultGhsCompilerRootDir "C:/ghs/comp_201714")
endif()
set(GHS_COMPILER_ROOT_DIR ${defaultGhsCompilerRootDir} CACHE PATH "Path to GHS compiler folder")


if(DEFINED ENV{SDL_CMAKE_GHS_MULTI_ROOT_DIR})
    set(defaultGhsMultiRootDir $ENV{SDL_CMAKE_GHS_MULTI_ROOT_DIR})
else()
    set(defaultGhsMultiRootDir "C:/ghs/multi_716d")
endif()
set(GHS_MULTI_ROOT_DIR ${defaultGhsMultiRootDir}  CACHE PATH "Path to GHS MULTI folder")


######################################################################################
# IAR Systems
######################################################################################
if(DEFINED ENV{SDL_CMAKE_IAR_COMPILER_ROOT_DIR})
    set(defaultIarCompilerRootDir $ENV{SDL_CMAKE_IAR_COMPILER_ROOT_DIR})
else()
    set(defaultIarCompilerRootDir "C:/Program Files/IAR Systems/Embedded Workbench 9.1")
endif()
set(IAR_COMPILER_ROOT_DIR ${defaultIarCompilerRootDir} CACHE PATH "Path to IAR root folder used for compiling")


if(DEFINED ENV{SDL_CMAKE_IAR_DEBUG_ROOT_DIR})
    set(defaultIarDebugRootDir $ENV{SDL_CMAKE_IAR_DEBUG_ROOT_DIR})
else()
    set(defaultIarDebugRootDir "C:/Program Files (x86)/IAR Systems/Embedded Workbench 9.1")
endif()
set(IAR_DEBUG_ROOT_DIR ${defaultIarDebugRootDir} CACHE PATH "Path to IAR root folder used for debugging")


######################################################################################
# Wind River
######################################################################################
if(DEFINED ENV{SDL_CMAKE_DIAB_COMPILER_ROOT_DIR})
    set(defaultDiabCompilerRootDir $ENV{SDL_CMAKE_DIAB_COMPILER_ROOT_DIR})
else()
    set(defaultDiabCompilerRootDir "C:/WindRiver/compilers/diab-5.9.8.1")
endif()
set(DIAB_COMPILER_ROOT_DIR ${defaultDiabCompilerRootDir} CACHE PATH "Path to Windriver root folder used for compiling")


######################################################################################
# ARM GNU Toolchain (e.g. GCC for building, GDB for debugging with VS Code, ...)
######################################################################################
if(DEFINED ENV{SDL_CMAKE_GCC_COMPILER_ROOT_DIR})
    set(defaultGccCompilerRootDir $ENV{SDL_CMAKE_GCC_COMPILER_ROOT_DIR})
else()
    set(defaultGccCompilerRootDir "C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/11.3 rel1")
endif()
set(GCC_COMPILER_ROOT_DIR ${defaultGccCompilerRootDir} CACHE PATH "Path to GNU C Compiler root folder used for compiling/debugging")


######################################################################################
# Lauterbach
######################################################################################
if(DEFINED ENV{SDL_CMAKE_T32_EXE_DIR})
    set(defaultT32ExeDir $ENV{SDL_CMAKE_T32_EXE_DIR})
else()
    set(defaultT32ExeDir "C:/T32/bin/windows64")
endif()
set(T32_EXE_DIR ${defaultT32ExeDir} CACHE PATH "Path to Lauterbach Trace32 folder with the 't32start', 't32marm' and other executables")


######################################################################################
# Infineon Auto Flash Utility (AFU) / OpenOCD (OOCD)
######################################################################################
if(DEFINED ENV{SDL_CMAKE_AFU_OOCD_ROOT_DIR})
    set(defaultAfuOocdRootDir $ENV{SDL_CMAKE_AFU_OOCD_ROOT_DIR})
else()
    set(defaultAfuOocdRootDir "C:/Program Files (x86)/Infineon/Auto Flash Utility 1.4")
endif()
set(AFU_OOCD_ROOT_DIR ${defaultAfuOocdRootDir} CACHE PATH "Path to Infineon Auto Flash Utility / OpenOCD")


# Specify the hardware interface to be used with AFU/OOCD, typically 'kitprog3.cfg' for Cypress/Infineon MiniProg4/KitProg3 or 'jlink.cfg' for SEGGER J-Link
if(DEFINED ENV{SDL_CMAKE_AFU_OOCD_INTERFACE_CFG_FILE})
    set(defaultAfuOocdInterfaceCfgFile $ENV{SDL_CMAKE_AFU_OOCD_INTERFACE_CFG_FILE})
else()
    set(defaultAfuOocdInterfaceCfgFile "kitprog3.cfg")
endif()
set(AFU_OOCD_INTERFACE_CFG_FILE ${defaultAfuOocdInterfaceCfgFile} CACHE STRING "Infineon Auto Flash Utility / OpenOCD interface configuration file name (typically kitprog3.cfg or jlink.cfg)")


######################################################################################
# MCAL / Tresos (only relevant if USE_MCAL is true)
######################################################################################
# Tresos/MCAL directory (having 'bin', 'plugins', etc. subfolders)
if(DEFINED ENV{SDL_CMAKE_MCAL_VAR_TRESOS_DIR})
    set(defaultMcalVarTresosDir $ENV{SDL_CMAKE_MCAL_VAR_TRESOS_DIR})
else()
    set(defaultMcalVarTresosDir "C:/INFINEON_ESDB/Tresos26_2_0")
endif()
set(MCAL_VAR_TRESOS_DIR ${defaultMcalVarTresosDir} CACHE PATH "Path to Tresos root folder")


# Tresos workspace can be a common folder for multiple projects, if it does not exist, it will be created automatically (Note: it will not be cleaned by the "clean" target)
if(DEFINED ENV{SDL_CMAKE_MCAL_VAR_TRESOS_WORKSPACE_DIR})
    set(defaultMcalVarTresosWorkspaceDir $ENV{SDL_CMAKE_MCAL_VAR_TRESOS_WORKSPACE_DIR})
else()
    set(defaultMcalVarTresosWorkspaceDir "${CMAKE_SOURCE_DIR}/tresos_workspace")
endif()
set(MCAL_VAR_TRESOS_WORKSPACE_DIR ${defaultMcalVarTresosWorkspaceDir} CACHE PATH "Path to Tresos workspace folder")


# Tresos project directory (can be placed anywhere, does not need to be inside the workspace). The default value assumes that you copy the MCAL example folder contents to the DIE's source directory.
if(DEFINED ENV{SDL_CMAKE_MCAL_VAR_TRESOS_PROJECT_DIR})
    set(defaultMcalVarTresosProjectDir $ENV{SDL_CMAKE_MCAL_VAR_TRESOS_PROJECT_DIR})
else()
    set(defaultMcalVarTresosProjectDir "${CMAKE_SOURCE_DIR}/${DIE}/src/${MCAL_VAR_TRESOS_PROJECT_NAME}")
endif()
set(MCAL_VAR_TRESOS_PROJECT_DIR ${defaultMcalVarTresosProjectDir} CACHE PATH "Path to Tresos project folder")


# If Tresos projects not delivered with SDL are used, this option can be helpful if those projects do not adhere to the expected output generation path
if(DEFINED ENV{SDL_CMAKE_AUTOFIX_TRESOS_GEN_PATH})
    set(defaultAutoFixTresosGenPath $ENV{SDL_CMAKE_AUTOFIX_TRESOS_GEN_PATH})
else()
    set(defaultAutoFixTresosGenPath OFF)
endif()
option(AUTOFIX_TRESOS_GEN_PATH "Updates the GenerationPath in pref_general.xdm to expected value (output\\generated)" ${defaultAutoFixTresosGenPath})

