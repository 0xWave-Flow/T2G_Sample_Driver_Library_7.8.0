function(create_librtos)

    set(libBaseName rtos)
    
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

    if(${ARG_CPU_TYPE} STREQUAL cm4)
        set(cpuFolderName ARM_CM4F)
    else()
        set(cpuFolderName ARM_CM7/r0p1)
    endif()

    target_include_directories(${libName}
        PUBLIC
            ${PATH_COMMON_SRC_RTOS}/include
            ${PATH_COMMON_SRC_RTOS}/portable/${TOOLCHAIN_ID_UPPERCASE}/${cpuFolderName}
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
            ${PATH_COMMON_SRC_RTOS}/croutine.c
            ${PATH_COMMON_SRC_RTOS}/event_groups.c
            ${PATH_COMMON_SRC_RTOS}/list.c
            ${PATH_COMMON_SRC_RTOS}/queue.c
            ${PATH_COMMON_SRC_RTOS}/stream_buffer.c
            ${PATH_COMMON_SRC_RTOS}/tasks.c
            ${PATH_COMMON_SRC_RTOS}/timers.c
            ${PATH_COMMON_SRC_RTOS}/portable/MemMang/heap_4.c
            ${PATH_COMMON_SRC_RTOS}/portable/${TOOLCHAIN_ID_UPPERCASE}/${cpuFolderName}/port.c
            $<$<NOT:$<BOOL:${GCC}>>:${PATH_COMMON_SRC_RTOS}/portable/${TOOLCHAIN_ID_UPPERCASE}/${cpuFolderName}/portasm${TOOLCHAIN_ASM_EXT}> # GCC does not have an assembly file in its port
    )

endfunction()
