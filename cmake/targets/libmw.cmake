function(create_libmw)
    
    set(libBaseName mw)
    
    # Define the supported set of keywords
    set(prefix ARG)
    set(noValueArgs)
    set(singleValueArgs CPU_TYPE)
    set(multiValueArgs)

    cmake_parse_arguments(${prefix} "${noValueArgs}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    printFunctionArgs(${CMAKE_CURRENT_FUNCTION} "${noValueArgs}" "${singleValueArgs}" "${multiValueArgs}")

    # Check presence of mandatory arguments
    if(NOT DEFINED ARG_CPU_TYPE)
        message(FATAL_ERROR "CPU_TYPE is not specified!")
    endif()

    set(libName ${libBaseName}_${ARG_CPU_TYPE})
    
    message(STATUS "Creating library: ${libName}")

    add_library(${libName} STATIC)

    target_include_directories(${libName}
        PRIVATE
            ${INCLUDE_DIRS}
    )

    target_compile_definitions(${libName}
        PRIVATE
            ${COMPILER_DEFINES}
            ${COMPILER_DEFINES_${ARG_CPU_TYPE}}
    )

    target_compile_options(${libName}
        PRIVATE
            $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_OPTIONS}>
            $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_OPTIONS_${ARG_CPU_TYPE}}>
            $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_OPTIONS}>
            $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_OPTIONS_${ARG_CPU_TYPE}}>
            $<$<COMPILE_LANGUAGE:ASM>:${ASSEMBLER_OPTIONS}>        
            $<$<COMPILE_LANGUAGE:ASM>:${ASSEMBLER_OPTIONS_${ARG_CPU_TYPE}}>  
    )

    target_sources(${libName}
        PRIVATE
          #-------------------------------- COMMON FOLDER ---------------------------------------------
            $<$<BOOL:${HAVE_MW_AUDIO_AIC261}>:${PATH_COMMON_SRC_MW}/aic261/cy_aic261.c>
            $<$<BOOL:${HAVE_MW_AUDIO_AIC261}>:${PATH_COMMON_SRC_MW}/aic261/cy_aic261.h>
            ${PATH_COMMON_SRC_MW}/button/cy_button.c
            ${PATH_COMMON_SRC_MW}/button/cy_button.h
            ${PATH_COMMON_SRC_MW}/flash/cy_mw_flash.c
            ${PATH_COMMON_SRC_MW}/flash/cy_mw_flash.h
            ${PATH_COMMON_SRC_MW}/semihosting/cy_semihosting.c
            ${PATH_COMMON_SRC_MW}/semihosting/cy_semihosting.h
            ${PATH_COMMON_SRC_MW}/sw_timer/cy_sw_tmr.c
            ${PATH_COMMON_SRC_MW}/sw_timer/cy_sw_tmr.h
          #-------------------------------- SERIES FOLDER ---------------------------------------------
            $<$<BOOL:${HAVE_MW_AUDIO_CS42448}>:${PATH_DIE_SRC_MW}/audio/cy_cs42448.c>
            $<$<BOOL:${HAVE_MW_AUDIO_CS42448}>:${PATH_DIE_SRC_MW}/audio/cy_cs42448.h>
            $<$<BOOL:${HAVE_MW_ETH_PHY_DP83848Q_Q1}>:${PATH_DIE_SRC_MW}/eth_phy/cy_dp83848q_q1.c>
            $<$<BOOL:${HAVE_MW_ETH_PHY_DP83848Q_Q1}>:${PATH_DIE_SRC_MW}/eth_phy/cy_dp83848q_q1.h>
            $<$<BOOL:${HAVE_MW_ETH_PHY_DP83867}>:${PATH_DIE_SRC_MW}/eth_phy/cy_dp83867.c>
            $<$<BOOL:${HAVE_MW_ETH_PHY_DP83867}>:${PATH_DIE_SRC_MW}/eth_phy/cy_dp83867.h>
            $<$<BOOL:${HAVE_MW_GFX_ENV}>:${PATH_DIE_SRC_MW}/gfx_env/cy_gfx_env.c>
            $<$<BOOL:${HAVE_MW_GFX_ENV}>:${PATH_DIE_SRC_MW}/gfx_env/cy_gfx_env.h>
            $<$<BOOL:${HAVE_MW_MIPI_SENSOR}>:${PATH_DIE_SRC_MW}/mipi_sensor/cy_mipi_sensor.c>
            $<$<BOOL:${HAVE_MW_MIPI_SENSOR}>:${PATH_DIE_SRC_MW}/mipi_sensor/cy_mipi_sensor.h>
            $<$<BOOL:${HAVE_MW_POWER}>:${PATH_DIE_SRC_MW}/power/cy_power.c>
            $<$<BOOL:${HAVE_MW_POWER}>:${PATH_DIE_SRC_MW}/power/cy_power.h>
            $<$<BOOL:${HAVE_MW_SMIF_MEM}>:${PATH_DIE_SRC_MW}/smif_mem/cy_smif_device_common.c>
            $<$<BOOL:${HAVE_MW_SMIF_MEM}>:${PATH_DIE_SRC_MW}/smif_mem/cy_smif_device_common.h>
            $<$<BOOL:${HAVE_MW_SMIF_MEM}>:${PATH_DIE_SRC_MW}/smif_mem/cy_smif_hb_flash.c>
            $<$<BOOL:${HAVE_MW_SMIF_MEM}>:${PATH_DIE_SRC_MW}/smif_mem/cy_smif_hb_flash.h>
            $<$<BOOL:${HAVE_MW_SMIF_MEM}>:${PATH_DIE_SRC_MW}/smif_mem/cy_smif_s25fl.c>
            $<$<BOOL:${HAVE_MW_SMIF_MEM}>:${PATH_DIE_SRC_MW}/smif_mem/cy_smif_s25fl.h>
            $<$<BOOL:${HAVE_MW_SMIF_MEM}>:${PATH_DIE_SRC_MW}/smif_mem/cy_smif_s25fs.c>
            $<$<BOOL:${HAVE_MW_SMIF_MEM}>:${PATH_DIE_SRC_MW}/smif_mem/cy_smif_s25fs.h>
            $<$<BOOL:${HAVE_MW_SMIF_MEM}>:${PATH_DIE_SRC_MW}/smif_mem/cy_smif_semp.c>
            $<$<BOOL:${HAVE_MW_SMIF_MEM}>:${PATH_DIE_SRC_MW}/smif_mem/cy_smif_semp.h>

    )

endfunction()
