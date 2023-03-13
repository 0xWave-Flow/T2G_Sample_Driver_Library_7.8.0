cmake_minimum_required(VERSION 3.23)

###################################################################################################
# Some CMake cache variables
###################################################################################################

option(PSVP         "Targets are built for PSVP"            OFF) # PSVP: FPGA based pre-silicon validation platform
option(USE_RTOS     "Include FreeRTOS functionality"        ON)
option(USE_ETHSTACK "Include Ethernet stack (FreeRTOS+TCP)" OFF) # Only supported by tviibh8m, tviibh16m, tviic2d4m, tviic2d6m and tviic2d6mddr, not yet tviibh4m
option(USE_MCAL     "Include MCAL functionality"            OFF)
# option(USE_ETHSTACK "Include Ethernet stack (FreeRTOS+TCP)" ON) # Only supported by tviibh8m, tviibh16m, tviic2d4m, tviic2d6m and tviic2d6mddr, not yet tviibh4m
# option(USE_MCAL     "Include MCAL functionality"            ON)

option(BUILD_CRYPTO_LIBS "Add the crypto libraries as non-default build targets (this option is for Infineon internal use)" OFF)


###################################################################################################
# Main and early includes
###################################################################################################


set(MCU_REV d)
set(DIE tviibh8m)
set(TOOLCHAIN_ID gcc)

message(">>> CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
message(">>> MCU_REV = ${MCU_REV}")
message(">>> DIE = ${DIE}")
message(">>> TOOLCHAIN_ID = ${TOOLCHAIN_ID}")

include(${CMAKE_SOURCE_DIR}/cmake/misc/helper_functions.cmake)

# Create MCU_REV_STRING before including other cmake files, as the revision information might be needed there
if(${MCU_REV} STREQUAL latest)
    get_latest_mcu_revision(MCU_REV) # Find actual revision if only the "latest" pseudo revision has been set
endif()
string(TOLOWER rev_${MCU_REV} MCU_REV_STRING) # Create a string like "rev_d" from an MCU revision string "D" because this is needed for various purposes (e.g. subfolder names, define...)

include(${CMAKE_SOURCE_DIR}/cmake/t2g_device_config/${DIE}.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/tool_config.cmake)


###################################################################################################
# PROJECT call / Print important build info / ...
###################################################################################################

# Note: Toolchain files are only processed after project() call, no access to vars that are defined in them is possible before that point
project(T2G-SDL-CMake-${DIE}  DESCRIPTION "CMake variant of T2G SDL"  LANGUAGES C ASM)

# Now after toolchain processing explicitly set this CMake internal variable because its default value varies between the toolchains
# (due to CMake internal hardcoding, e.g. IAR -> ".elf", others -> "") and would not allow a generic handling in our CMake scripts
set(CMAKE_EXECUTABLE_SUFFIX .elf)

# Now toolchain is known so the corresponding config file can be included
include(${CMAKE_SOURCE_DIR}/cmake/toolchains/t2g_toolchain_config_${TOOLCHAIN_ID}.cmake)

print_info()

# Create a generic variable that holds the CPU type of the application/main CPU(s)
if(HAVE_CM4)
    set(APP_CPU_TYPE cm4)
else()
    set(APP_CPU_TYPE cm7)
endif()


###################################################################################################
# Common variables needed to build all/most of the target libraries/executables
###################################################################################################

# Frequently used paths
set(PATH_COMMON_SRC                  ${CMAKE_SOURCE_DIR}/common/src)
set(PATH_COMMON_SRC_DRIVERS          ${CMAKE_SOURCE_DIR}/common/src/drivers)
set(PATH_COMMON_SRC_MW               ${CMAKE_SOURCE_DIR}/common/src/mw)
set(PATH_DIE_SRC                     ${CMAKE_SOURCE_DIR}/${DIE}/src)
set(PATH_DIE_SRC_DRIVERS             ${CMAKE_SOURCE_DIR}/${DIE}/src/drivers)
set(PATH_DIE_SRC_MW                  ${CMAKE_SOURCE_DIR}/${DIE}/src/mw)
if(USE_RTOS)
    set(PATH_COMMON_SRC_RTOS         ${CMAKE_SOURCE_DIR}/common/src/rtos)
endif()
if(USE_ETHSTACK)
    set(PATH_COMMON_SRC_ETHSTACK     ${CMAKE_SOURCE_DIR}/common/src/ethstack)
endif()
if(USE_MCAL)
    set(PATH_COMMON_SRC_MCALSTUBS    ${CMAKE_SOURCE_DIR}/common/src/mcalstubs)
endif()

# Generic compiler/assember include directories (check CMake files that create executable or library targets for further specific include dirs)
set(INCLUDE_DIRS
    ${TOOLCHAIN_INCLUDE_DIRS} # Explicitly adding the toolchain include dirs may help code indexers in 3rd party apps like VS Code
    ${DIE}/hdr/${MCU_REV_STRING}
    ${DIE}/hdr/${MCU_REV_STRING}/ip
    ${DIE}/hdr/${MCU_REV_STRING}/mcureg
    ${DIE}/src
    ${DIE}/src/drivers
    ${DIE}/src/mw
    ${DIE}/src/interrupts/${MCU_REV_STRING}
    ${DIE}/src/system/${MCU_REV_STRING}
    common/hdr
    common/hdr/cmsis/include
    common/src/drivers
    common/src/mw
)

# Generic compiler/assember defines (check CMake files that create executable or library targets for further specific defines)
set(COMPILER_DEFINES
    ${DIE}
    ${SERIES}
    ${DEVICE}
    CY_MCU_${MCU_REV_STRING}
    CY_USE_PSVP=$<BOOL:${PSVP}>
    ${BOARD}
    ${BOARD_REV}
    _LINK_${LINK}_
    $<$<BOOL:${USE_RTOS}>:USE_RTOS>
    $<$<BOOL:${USE_ETHSTACK}>:USE_ETHSTACK>
    $<$<BOOL:${USE_MCAL}>:USE_MCAL>
)

# CPU specific defines (check CMake files that create executable or library targets for further specific defines)
set(COMPILER_DEFINES_cm0plus)

set(COMPILER_DEFINES_cm4)

set(COMPILER_DEFINES_cm7
    __ICACHE_PRESENT=1
    __DCACHE_PRESENT=1
)

# Generic linker defines (check CMake files that create executable targets for further specific defines)
set(LINKER_DEFINES
    CY_MCU_${MCU_REV_STRING}
    _LINK_${LINK}_
    CY_USE_PSVP=$<BOOL:${PSVP}>
)


###################################################################################################
# Add all necessary libraries for all the CPU types
###################################################################################################

include(${CMAKE_SOURCE_DIR}/cmake/targets/libdrivers.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/targets/libmw.cmake)

# Standard libraries for drivers and middleware, will always be linked by the executables
create_libdrivers(CPU_TYPE cm0plus)
create_libmw(CPU_TYPE cm0plus)

create_libdrivers(CPU_TYPE ${APP_CPU_TYPE})
create_libmw(CPU_TYPE ${APP_CPU_TYPE})

# Optional libraries depending on user configuration, all enabled/created libs will be collected in list ADDITIONAL_LIBS_TO_LINK_${APP_CPU_TYPE}
if(USE_RTOS)
    include(${CMAKE_SOURCE_DIR}/cmake/targets/librtos.cmake)
    create_librtos(CPU_TYPE ${APP_CPU_TYPE})
    list(APPEND ADDITIONAL_LIBS_TO_LINK_${APP_CPU_TYPE} rtos_${APP_CPU_TYPE})
endif()

if(USE_ETHSTACK)
    include(${CMAKE_SOURCE_DIR}/cmake/targets/libethstack.cmake)
    create_libethstack(CPU_TYPE ${APP_CPU_TYPE})
    list(APPEND ADDITIONAL_LIBS_TO_LINK_${APP_CPU_TYPE} ethstack_${APP_CPU_TYPE})
endif()

if(USE_MCAL)
    include(${CMAKE_SOURCE_DIR}/cmake/targets/mcal/libmcalstubs.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/targets/mcal/libmcal.cmake)

    create_libmcalstubs(CPU_TYPE ${APP_CPU_TYPE})
    create_libmcal(CPU_TYPE ${APP_CPU_TYPE})

    list(APPEND ADDITIONAL_LIBS_TO_LINK_${APP_CPU_TYPE} mcalstubs_${APP_CPU_TYPE})
    list(APPEND ADDITIONAL_LIBS_TO_LINK_${APP_CPU_TYPE} mcal_${APP_CPU_TYPE})

endif()

# Infineon internal use
if(BUILD_CRYPTO_LIBS)
    include(${CMAKE_SOURCE_DIR}/cmake/targets/libcrypto.cmake)
    create_libcrypto(CPU_TYPE cm0plus)
    create_libcrypto(CPU_TYPE ${APP_CPU_TYPE})
endif()


###################################################################################################
# Add the executables for all the CPUs
###################################################################################################

include(${CMAKE_SOURCE_DIR}/cmake/targets/executable.cmake)

# Create variables for the names of the generated executables because they might be needed at multiple places
set(EXE_NAME_CM0PLUS cm0plus)
set(EXE_NAME_CM4     cm4)
set(EXE_NAME_CM7_0   cm7_0)
set(EXE_NAME_CM7_1   cm7_1)
set(EXE_NAME_CM7_2   cm7_2)
set(EXE_NAME_CM7_3   cm7_3)


create_executable(
    CPU_TYPE cm0plus
    EXE_NAME ${EXE_NAME_CM0PLUS}
    ADDL_SOURCES
        ${PATH_DIE_SRC}/main_cm0plus.c
)

if(HAVE_CM4)
    create_executable(
        CPU_TYPE ${APP_CPU_TYPE}
        EXE_NAME ${EXE_NAME_CM4}
        ADDL_SOURCES
            ${PATH_DIE_SRC}/main_cm4.c
        ADDL_LIBS
            ${ADDITIONAL_LIBS_TO_LINK_${APP_CPU_TYPE}}
    )
endif()

if(HAVE_CM7_0)
    create_executable(
        CPU_TYPE ${APP_CPU_TYPE}
        CM7_INSTANCE 0
        EXE_NAME ${EXE_NAME_CM7_0}
        ADDL_SOURCES
            ${PATH_DIE_SRC}/main_cm7_0.c
        ADDL_LIBS
            ${ADDITIONAL_LIBS_TO_LINK_${APP_CPU_TYPE}}
    )
endif()

if(HAVE_CM7_1)
    create_executable(
        CPU_TYPE ${APP_CPU_TYPE}
        CM7_INSTANCE 1
        EXE_NAME ${EXE_NAME_CM7_1}
        ADDL_SOURCES
            ${PATH_DIE_SRC}/main_cm7_1.c
        ADDL_LIBS
            ${ADDITIONAL_LIBS_TO_LINK_${APP_CPU_TYPE}}
    )
endif()

if(HAVE_CM7_2)
    create_executable(
        CPU_TYPE ${APP_CPU_TYPE}
        CM7_INSTANCE 2
        EXE_NAME ${EXE_NAME_CM7_2}
        ADDL_SOURCES
            ${PATH_DIE_SRC}/main_cm7_2.c
        ADDL_LIBS
            ${ADDITIONAL_LIBS_TO_LINK_${APP_CPU_TYPE}}
    )
endif()

if(HAVE_CM7_3)
    create_executable(
        CPU_TYPE ${APP_CPU_TYPE}
        CM7_INSTANCE 3
        EXE_NAME ${EXE_NAME_CM7_3}
        ADDL_SOURCES
            ${PATH_DIE_SRC}/main_cm7_3.c
        ADDL_LIBS
            ${ADDITIONAL_LIBS_TO_LINK_${APP_CPU_TYPE}}
    )
endif()


###################################################################################################
# Debugging (add custom targets for invoking a debugger, create VS Code files, ...)
###################################################################################################

include(${CMAKE_SOURCE_DIR}/cmake/targets/debugger_ghs.cmake)
create_ghs_debugger_targets(
    EXE_NAME_CM0PLUS ${EXE_NAME_CM0PLUS}
    EXE_NAME_CM4     ${EXE_NAME_CM4}
    EXE_NAME_CM7_0   ${EXE_NAME_CM7_0}
    EXE_NAME_CM7_1   ${EXE_NAME_CM7_1}
    EXE_NAME_CM7_2   ${EXE_NAME_CM7_2}
    EXE_NAME_CM7_3   ${EXE_NAME_CM7_3}
)

include(${CMAKE_SOURCE_DIR}/cmake/targets/debugger_iar.cmake)
create_iar_debugger_targets(
    EXE_NAME_CM0PLUS ${EXE_NAME_CM0PLUS}
    EXE_NAME_CM4     ${EXE_NAME_CM4}
    EXE_NAME_CM7_0   ${EXE_NAME_CM7_0}
    EXE_NAME_CM7_1   ${EXE_NAME_CM7_1}
    EXE_NAME_CM7_2   ${EXE_NAME_CM7_2}
    EXE_NAME_CM7_3   ${EXE_NAME_CM7_3}
)

include(${CMAKE_SOURCE_DIR}/cmake/targets/debugger_t32.cmake)
create_t32_debugger_targets(
    EXE_NAME_CM0PLUS ${EXE_NAME_CM0PLUS}
    EXE_NAME_CM4     ${EXE_NAME_CM4}
    EXE_NAME_CM7_0   ${EXE_NAME_CM7_0}
    EXE_NAME_CM7_1   ${EXE_NAME_CM7_1}
    EXE_NAME_CM7_2   ${EXE_NAME_CM7_2}
    EXE_NAME_CM7_3   ${EXE_NAME_CM7_3}
)

include(${CMAKE_SOURCE_DIR}/cmake/targets/debugger_vsc.cmake)
create_vsc_launch_json(
    EXE_NAME_CM0PLUS ${EXE_NAME_CM0PLUS}
    EXE_NAME_CM4     ${EXE_NAME_CM4}
    EXE_NAME_CM7_0   ${EXE_NAME_CM7_0}
    EXE_NAME_CM7_1   ${EXE_NAME_CM7_1}
    EXE_NAME_CM7_2   ${EXE_NAME_CM7_2}
    EXE_NAME_CM7_3   ${EXE_NAME_CM7_3}
)

###################################################################################################
# Miscellaneous (add custom targets for invoking other utilities, ...)
###################################################################################################

include(${CMAKE_SOURCE_DIR}/cmake/targets/util_afu_oocd.cmake)
create_afu_oocd_targets(
    EXE_NAME_CM0PLUS ${EXE_NAME_CM0PLUS}
    EXE_NAME_CM4     ${EXE_NAME_CM4}
    EXE_NAME_CM7_0   ${EXE_NAME_CM7_0}
    EXE_NAME_CM7_1   ${EXE_NAME_CM7_1}
    EXE_NAME_CM7_2   ${EXE_NAME_CM7_2}
    EXE_NAME_CM7_3   ${EXE_NAME_CM7_3}
)

if(USE_MCAL)
    include(${CMAKE_SOURCE_DIR}/cmake/targets/util_tresos.cmake)
    create_tresos_utility_targets()
endif()
