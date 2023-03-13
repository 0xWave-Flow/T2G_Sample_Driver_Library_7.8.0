function(create_executable)

    # Define the supported set of keywords
    set(prefix ARG)
    set(noValueArgs)
    set(singleValueArgs
        CPU_TYPE
        CM7_INSTANCE
        EXE_NAME)
    set(multiValueArgs
        ADDL_SOURCES
        ADDL_INCLUDE_DIRS
        ADDL_COMPILER_DEFS
        ADDL_COMPILER_OPTS
        ADDL_LINKER_DEFS
        ADDL_LINKER_OPTS
        ADDL_LIBS
    )

    cmake_parse_arguments(${prefix} "${noValueArgs}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    printFunctionArgs(${CMAKE_CURRENT_FUNCTION} "${noValueArgs}" "${singleValueArgs}" "${multiValueArgs}")

    # Check presence of mandatory arguments
    if(NOT DEFINED ARG_CPU_TYPE)
        message(FATAL_ERROR "CPU_TYPE is not specified!")
    endif()
    if(NOT DEFINED ARG_EXE_NAME)
        message(FATAL_ERROR "EXE_NAME is not specified!")
    endif()
    if((${ARG_CPU_TYPE} STREQUAL cm7) AND (NOT DEFINED ARG_CM7_INSTANCE))
        message(FATAL_ERROR "CPU_TYPE is cm7 but no CM7_INSTANCE has been passed!")
    endif()


    if(${ARG_CPU_TYPE} STREQUAL cm7)
        set(cpuInstanceSuffix _${ARG_CM7_INSTANCE})
    endif()

    message(STATUS "Creating executable: ${ARG_EXE_NAME}")

    add_executable(${ARG_EXE_NAME})

    ######################################################################################
    # Compilation
    ######################################################################################

    target_include_directories(${ARG_EXE_NAME}
        PRIVATE
            ${INCLUDE_DIRS}
            ${ARG_ADDL_INCLUDE_DIRS}
    )

    target_compile_definitions(${ARG_EXE_NAME}
        PRIVATE
            ${COMPILER_DEFINES}
            ${COMPILER_DEFINES_${ARG_CPU_TYPE}}
            $<$<STREQUAL:${ARG_CPU_TYPE},cm7>:CY_CORE_CM7${cpuInstanceSuffix}>
            ${ARG_ADDL_COMPILER_DEFS}
    )

    target_compile_options(${ARG_EXE_NAME}
        PRIVATE
            $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_OPTIONS}>
            $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_OPTIONS_${ARG_CPU_TYPE}}>
            $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_OPTIONS}>
            $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_OPTIONS_${ARG_CPU_TYPE}}>
            $<$<COMPILE_LANGUAGE:ASM>:${ASSEMBLER_OPTIONS}>
            $<$<COMPILE_LANGUAGE:ASM>:${ASSEMBLER_OPTIONS_${ARG_CPU_TYPE}}>
            ${ARG_ADDL_COMPILER_OPTS}
    )

    target_sources(${ARG_EXE_NAME}
        PRIVATE
            ${PATH_COMMON_SRC}/startup/startup.c
            ${PATH_COMMON_SRC}/startup/startup_customize.h
            ${PATH_COMMON_SRC}/startup/${TOOLCHAIN_ID}/startup_${ARG_CPU_TYPE}${TOOLCHAIN_ASM_EXT}
            ${PATH_DIE_SRC}/system/${MCU_REV_STRING}/system_${DIE}_${ARG_CPU_TYPE}.c
            ${PATH_DIE_SRC}/interrupts/${MCU_REV_STRING}/cy_interrupt_map.c
            ${ARG_ADDL_SOURCES}
    )


    ######################################################################################
    # Linking
    ######################################################################################

    # All preprocessor defines that may be required by the linker script
    set(ALL_LINKER_DEFINES
        _CORE_${ARG_CPU_TYPE}${cpuInstanceSuffix}_
        ${LINKER_DEFINES}
        ${ARG_ADDL_LINKER_DEFS}
    )

    # Get linker file option (call toolchain specific function that allows target / CPU instance specific handling)
    get_linker_file_option(LINKER_FILE_OPTION ${ARG_EXE_NAME} ${ARG_CPU_TYPE}${cpuInstanceSuffix} "${ALL_LINKER_DEFINES}")

    # Get linker defines converted to the toolchain specific linker option syntax
    get_converted_linker_defines(ALL_LINKER_DEFINES_CONVERTED ${ALL_LINKER_DEFINES})

    target_link_options(${ARG_EXE_NAME}
        PRIVATE
            ${LINKER_OPTIONS}
            ${LINKER_OPTIONS_${ARG_CPU_TYPE}}
            ${ARG_ADDL_LINKER_OPTS}
            ${ALL_LINKER_DEFINES_CONVERTED}
            ${LINKER_FILE_OPTION}            
    )

    # Establish additional dependencies for the link step (typically linker file will become a dependency)
    set_target_properties(${ARG_EXE_NAME}
        PROPERTIES
            LINK_DEPENDS "${LINK_DEPENDENCY_FILES}"
    )


    if(${ARG_CPU_TYPE} STREQUAL cm0plus)
        set(libcryptoType server)
    else()
        set(libcryptoType client)
    endif()

    # Libraries to link (Note: 'target_link_libraries' also accepts linker options, not only library names)
    target_link_libraries(${ARG_EXE_NAME} # 
        PRIVATE
            ${LINKER_OPTION_PRE_LINK_LIBS} # Defined by toolchain (usually empty, in GCC case --start-group option is used to solve circular library dependencies because GCC only scans libs once
            #-----------------------------
            drivers_${ARG_CPU_TYPE}
            mw_${ARG_CPU_TYPE}
            ${PATH_DIE_SRC_DRIVERS}/crypto/libs/libcrypto_${libcryptoType}_${ARG_CPU_TYPE}$<$<BOOL:${IAR}>:${cpuInstanceSuffix}>_${TOOLCHAIN_ID}.a
            ${ARG_ADDL_LIBS}
            ${TOOLCHAIN_LIBS}
            #-----------------------------
            ${LINKER_OPTION_POST_LINK_LIBS} # Defined by toolchain (usually empty, in GCC case --end-group option is used to solve circular library dependencies because GCC only scans libs once
    )


    ######################################################################################
    # Miscellaneous (other build steps, clean files, ...)
    ######################################################################################

    # Run an ELF converter if it is defined by the toolchain (will generate a Motorola S-Record file)
    if(DEFINED TOOLCHAIN_ELF_CONVERTER)
        get_elf_converter_args(elfConverterArgs $<TARGET_FILE:${ARG_EXE_NAME}>)
        add_custom_command(
            TARGET      ${ARG_EXE_NAME}
            POST_BUILD
            COMMAND     ${TOOLCHAIN_ELF_CONVERTER}
            ARGS        ${elfConverterArgs}
            BYPRODUCTS  ${CMAKE_BINARY_DIR}/$<CONFIG>/${ARG_EXE_NAME}${CMAKE_EXECUTABLE_SUFFIX}.srec  # Does not (yet?) support Target-dependent generator expressions
        )

    endif()


    # set_property(
    #     TARGET
    #         ${ARG_EXE_NAME}
    #     APPEND
    #     PROPERTY
    #         ADDITIONAL_CLEAN_FILES
    #             "$<TARGET_FILE:${ARG_EXE_NAME}>.map"
    # )



endfunction()
