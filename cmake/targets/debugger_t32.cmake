function(create_t32_debugger_targets)

    # Define the supported set of keywords
    set(prefix ARG)
    set(noValueArgs)
    set(singleValueArgs
        EXE_NAME_CM0PLUS
        EXE_NAME_CM4
        EXE_NAME_CM7_0
        EXE_NAME_CM7_1
        EXE_NAME_CM7_2
        EXE_NAME_CM7_3
    )
    set(multiValueArgs)

    cmake_parse_arguments(${prefix} "${noValueArgs}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    printFunctionArgs(${CMAKE_CURRENT_FUNCTION} "${noValueArgs}" "${singleValueArgs}" "${multiValueArgs}")

    # Check presence of mandatory arguments
    if(NOT DEFINED ARG_EXE_NAME_CM0PLUS)
        message(FATAL_ERROR "EXE_NAME_CM0PLUS is not specified and is required for debugger targets at the moment!")
    endif()

    ########################################################################################################

    if(NOT ${LINK} STREQUAL flash)
        message(STATUS "Skipping creation of debugger targets because only 'flash' builds are supported by this debugger target at the moment.")
        return()
    endif()
    
    set(debugTargetList cm0plus)

    if(HAVE_CM4 AND DEFINED ARG_EXE_NAME_CM4)
        list(APPEND debugTargetList cm0plus_cm4)
    endif()

    if(HAVE_CM7_0 AND DEFINED ARG_EXE_NAME_CM7_0)
        list(APPEND debugTargetList cm0plus_cm7_0)
    endif()

    if(HAVE_CM7_1 AND DEFINED ARG_EXE_NAME_CM7_1)
        list(APPEND debugTargetList cm0plus_cm7_1)
        if(DEFINED ARG_EXE_NAME_CM7_0)
            list(APPEND debugTargetList cm0plus_cm7_dual)
        endif()
    endif()

    if(HAVE_CM7_2 AND DEFINED ARG_EXE_NAME_CM7_2)
        list(APPEND debugTargetList cm0plus_cm7_2)
    endif()

    if(HAVE_CM7_3 AND DEFINED ARG_EXE_NAME_CM7_3)
        list(APPEND debugTargetList cm0plus_cm7_3)
        if((DEFINED ARG_EXE_NAME_CM7_0) AND (DEFINED ARG_EXE_NAME_CM7_1) AND (DEFINED ARG_EXE_NAME_CM7_2))
            list(APPEND debugTargetList cm0plus_cm7_quad)
        endif()
    endif()    

    set(configureFileSrcDir ${CMAKE_SOURCE_DIR}/cmake/template_files/dbg_t32)
    set(configureFileDstDir ${CMAKE_BINARY_DIR}/_dbg_t32_copy_src)
    set(copyDstDir          ${CMAKE_BINARY_DIR}/$<CONFIG>/dbg_t32)

    message(STATUS "Creating Lauterbach T32 debugger targets:")

    set(targetNamePrefix dbg_t32)

    foreach(target IN LISTS debugTargetList)

        configure_file(${configureFileSrcDir}/${target}.cmm ${configureFileDstDir}/${target}.cmm)

        set(targetName ${targetNamePrefix}_${target})
        
        message(STATUS "  ${targetName}")

        add_custom_target(${targetName}
            COMMAND            ${CMAKE_COMMAND} -E copy_directory ${configureFileDstDir} ${copyDstDir}
            COMMAND            start ${T32_EXE_DIR}/t32marm.exe -s ${copyDstDir}/${target}.cmm
            WORKING_DIRECTORY  ${CMAKE_BINARY_DIR}/$<CONFIG>
        )

    endforeach()

endfunction()

