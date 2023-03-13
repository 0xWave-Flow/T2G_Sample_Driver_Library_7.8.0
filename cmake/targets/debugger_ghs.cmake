function(create_ghs_debugger_targets)

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

    set(configureFileSrcDir ${CMAKE_SOURCE_DIR}/cmake/template_files/dbg_ghs)
    set(configureFileDstDir ${CMAKE_BINARY_DIR}/_dbg_ghs_copy_src)    
    set(copyDstDir          ${CMAKE_BINARY_DIR}/$<CONFIG>/dbg_ghs)

    message(STATUS "Creating GHS debugger targets:")

    set(targetNamePrefix dbg_ghs)

    foreach(target IN LISTS debugTargetList)

        configure_file(${configureFileSrcDir}/${target}.ghsmc ${configureFileDstDir}/${target}.ghsmc)

        set(targetName ${targetNamePrefix}_${target})
        
        message(STATUS "  ${targetName}")

        add_custom_target(${targetName}
            COMMAND            ${CMAKE_COMMAND} -E copy_directory ${configureFileDstDir} ${copyDstDir}
            COMMAND            start ${GHS_MULTI_ROOT_DIR}/multi.exe ${copyDstDir}/${target}.ghsmc
            WORKING_DIRECTORY  ${CMAKE_SOURCE_DIR}/${DIE}/tools/ghs
        )

    endforeach()

endfunction()

