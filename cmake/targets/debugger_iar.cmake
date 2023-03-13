function(create_iar_debugger_targets)

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

    message(STATUS "Creating IAR debugger targets:")

    set(targetNamePrefix dbg_iar)

    foreach(target IN LISTS debugTargetList)

        set(configureFileSrcDir ${CMAKE_SOURCE_DIR}/cmake/template_files/dbg_iar/${target})
        set(configureFileDstDir ${CMAKE_BINARY_DIR}/_dbg_iar_copy_src/${target})
        set(copyDstDir          ${CMAKE_BINARY_DIR}/$<CONFIG>/dbg_iar/${target})
        
        # Files that need to be configured depend on the current 'target' (globbing is not used because it would add the input files as CMake Configure dependencies)
        set(configureFileList ) # Clear list var
        list(APPEND configureFileList cm0plus.ewd cm0plus.ewp cm0plus.ewt cm0plus.eww)
        if(target STREQUAL cm0plus_cm4)
            list(APPEND configureFileList cm4.ewd cm4.ewp cm4.ewt cm4.eww multicore_session_cfg.xml)
        elseif(target STREQUAL cm0plus_cm7_0)
            list(APPEND configureFileList cm7_0.ewd cm7_0.ewp cm7_0.ewt cm7_0.eww multicore_session_cfg.xml)
        elseif(target STREQUAL cm0plus_cm7_1)
            list(APPEND configureFileList cm7_1.ewd cm7_1.ewp cm7_1.ewt cm7_1.eww multicore_session_cfg.xml)
        elseif(target STREQUAL cm0plus_cm7_dual)
            list(APPEND configureFileList cm7_0.ewd cm7_0.ewp cm7_0.ewt cm7_0.eww cm7_1.ewd cm7_1.ewp cm7_1.ewt cm7_1.eww multicore_session_cfg.xml)
        elseif(target STREQUAL cm0plus_cm7_2)
            list(APPEND configureFileList cm7_2.ewd cm7_2.ewp cm7_2.ewt cm7_2.eww multicore_session_cfg.xml)
        elseif(target STREQUAL cm0plus_cm7_3)
            list(APPEND configureFileList cm7_3.ewd cm7_3.ewp cm7_3.ewt cm7_3.eww multicore_session_cfg.xml)
        elseif(target STREQUAL cm0plus_cm7_quad)
            list(APPEND configureFileList cm7_0.ewd cm7_0.ewp cm7_0.ewt cm7_0.eww cm7_1.ewd cm7_1.ewp cm7_1.ewt cm7_1.eww cm7_2.ewd cm7_2.ewp cm7_2.ewt cm7_2.eww cm7_3.ewd cm7_3.ewp cm7_3.ewt cm7_3.eww multicore_session_cfg.xml)            
        endif()

        foreach(fileName IN LISTS configureFileList)
            configure_file(${configureFileSrcDir}/${fileName} ${configureFileDstDir}/${fileName})
        endforeach()

        set(targetName ${targetNamePrefix}_${target})
        
        message(STATUS "  ${targetName}")

        add_custom_target(${targetName}
            COMMAND            ${CMAKE_COMMAND} -E copy_directory ${configureFileDstDir} ${copyDstDir}
            COMMAND            start ${IAR_DEBUG_ROOT_DIR}/common/bin/IarIdePm.exe ${copyDstDir}/cm0plus.eww
            WORKING_DIRECTORY  ${CMAKE_BINARY_DIR}/$<CONFIG>
        )

    endforeach()

endfunction()

