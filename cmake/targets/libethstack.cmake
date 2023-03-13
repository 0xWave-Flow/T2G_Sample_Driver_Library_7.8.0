function(create_libethstack)

    set(libBaseName ethstack)
    
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
    if(${ARG_CPU_TYPE} STREQUAL cm0plus)
        message(FATAL_ERROR "This library does not support ${ARG_CPU_TYPE}!")
    endif()

    set(libName ${libBaseName}_${ARG_CPU_TYPE})
    
    message(STATUS "Creating library: ${libName}")

    add_library(${libName} STATIC)

    target_include_directories(${libName}
        PUBLIC
            ${PATH_COMMON_SRC_ETHSTACK}/include
            ${PATH_COMMON_SRC_ETHSTACK}/portable/Compiler/${TOOLCHAIN_ID_UPPERCASE}
            ${PATH_COMMON_SRC_ETHSTACK}/portable/NetworkInterface/include
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

    target_link_libraries(${libName}
        PRIVATE
            rtos_${ARG_CPU_TYPE}
    )

    target_sources(${libName}
        PRIVATE
            ${PATH_COMMON_SRC_ETHSTACK}/FreeRTOS_ARP.c
            ${PATH_COMMON_SRC_ETHSTACK}/FreeRTOS_DHCP.c
            ${PATH_COMMON_SRC_ETHSTACK}/FreeRTOS_DNS.c
            ${PATH_COMMON_SRC_ETHSTACK}/FreeRTOS_IP.c
            ${PATH_COMMON_SRC_ETHSTACK}/FreeRTOS_Sockets.c
            ${PATH_COMMON_SRC_ETHSTACK}/FreeRTOS_Stream_Buffer.c
            ${PATH_COMMON_SRC_ETHSTACK}/FreeRTOS_TCP_IP.c
            ${PATH_COMMON_SRC_ETHSTACK}/FreeRTOS_TCP_WIN.c
            ${PATH_COMMON_SRC_ETHSTACK}/FreeRTOS_UDP_IP.c
            ${PATH_COMMON_SRC_ETHSTACK}/portable/BufferManagement/BufferAllocation_1.c
            # ${PATH_COMMON_SRC_ETHSTACK}/portable/BufferManagement/BufferAllocation_2.c
            ${PATH_COMMON_SRC_ETHSTACK}/portable/NetworkInterface/Common/phyHandling.c
            ${PATH_COMMON_SRC_ETHSTACK}/portable/NetworkInterface/Traveo_T2G/NetworkInterface.c
    )

endfunction()
