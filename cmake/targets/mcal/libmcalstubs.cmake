function(create_libmcalstubs)
    
    set(libBaseName mcalstubs)
    
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
        PUBLIC
            ${PATH_COMMON_SRC_MCALSTUBS}
        PRIVATE
            ${INCLUDE_DIRS}
    )

    target_compile_definitions(${libName}
        PRIVATE
            ${COMPILER_DEFINES}
            ${COMPILER_DEFINES_${ARG_CPU_TYPE}}
            # -- Uncomment below defines to enable printf in some stubs
            # CY_MCAL_STUBS_CANIF_CBK_TX_ENABLE_PRINTF
            # CY_MCAL_STUBS_CANIF_CBK_RX_ENABLE_PRINTF
            # CY_MCAL_STUBS_CANIF_CBK_MODE_STATE_MISC_ENABLE_PRINTF
            # CY_MCAL_STUBS_DEM_FAILED_ENABLE_PRINTF
            # CY_MCAL_STUBS_DEM_PASSED_ENABLE_PRINTF
            # CY_MCAL_STUBS_DET_ENABLE_PRINTF
            # CY_MCAL_STUBS_ECUM_WAKEUP_ENABLE_PRINTF
            # CY_MCAL_STUBS_ERROR_CALLOUT_ENABLE_PRINTF
            # CY_MCAL_STUBS_LINIF_CBK_ENABLE_PRINTF
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
            ${PATH_COMMON_SRC_MCALSTUBS}/CanIf_Cbk.c
            ${PATH_COMMON_SRC_MCALSTUBS}/CanIf_Cbk.h
            ${PATH_COMMON_SRC_MCALSTUBS}/CanIf_MemMap.h
            ${PATH_COMMON_SRC_MCALSTUBS}/Dem.h
            ${PATH_COMMON_SRC_MCALSTUBS}/Dem_MemMap.h
            ${PATH_COMMON_SRC_MCALSTUBS}/Dem_ReportErrorStatus.c
            ${PATH_COMMON_SRC_MCALSTUBS}/Det.h
            ${PATH_COMMON_SRC_MCALSTUBS}/Det_MemMap.h
            ${PATH_COMMON_SRC_MCALSTUBS}/Det_ReportError.c
            ${PATH_COMMON_SRC_MCALSTUBS}/Det_ReportRuntimeError.c
            ${PATH_COMMON_SRC_MCALSTUBS}/EcuM.h
            ${PATH_COMMON_SRC_MCALSTUBS}/EcuM_Cbk.c
            ${PATH_COMMON_SRC_MCALSTUBS}/EcuM_Cbk.h
            ${PATH_COMMON_SRC_MCALSTUBS}/ErrorCalloutHandler.c
            ${PATH_COMMON_SRC_MCALSTUBS}/ErrorCalloutHandler.h
            ${PATH_COMMON_SRC_MCALSTUBS}/Interrupts_DisableRestore.h
            ${PATH_COMMON_SRC_MCALSTUBS}/LinIf_Cbk.c
            ${PATH_COMMON_SRC_MCALSTUBS}/LinIf_Cbk.h
            ${PATH_COMMON_SRC_MCALSTUBS}/MemIf_Types.h
            ${PATH_COMMON_SRC_MCALSTUBS}/Os.c
            ${PATH_COMMON_SRC_MCALSTUBS}/Os.h
            ${PATH_COMMON_SRC_MCALSTUBS}/Os_MemMap.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Adc.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Adc.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Can.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Can.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_CorTst.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_CorTst.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Dio.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Dio.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Fee.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Fee.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Fls.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Fls.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_FlsTst.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_FlsTst.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Gpt.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Gpt.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Icu.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Icu.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Lin.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Lin.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Mcu.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Mcu.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_MemMap.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Ocu.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Ocu.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Port.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Port.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Pwm.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Pwm.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_RamTst.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_RamTst.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Spi.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Spi.h
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Wdg_66_IA.c
            ${PATH_COMMON_SRC_MCALSTUBS}/SchM_Wdg_66_IA.h
            ${PATH_COMMON_SRC_MCALSTUBS}/WdgIf_Types.h
    )

    target_link_libraries(${libName}
        PRIVATE
            mcal_${ARG_CPU_TYPE}
    )

endfunction()
