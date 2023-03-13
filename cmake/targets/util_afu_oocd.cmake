function(create_afu_oocd_targets)

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

    ########################################################################################################

    if(DEFINED ARG_EXE_NAME_CM0PLUS)
        list(APPEND cpuList cm0plus)
        set(afuOocdProgFileName_cm0plus ${ARG_EXE_NAME_CM0PLUS}${CMAKE_EXECUTABLE_SUFFIX})
    endif()

    if(HAVE_CM4 AND DEFINED ARG_EXE_NAME_CM4)
        list(APPEND cpuList cm4)
        set(afuOocdProgFileName_cm4 ${ARG_EXE_NAME_CM4}${CMAKE_EXECUTABLE_SUFFIX})
    endif()

    if(HAVE_CM7_0 AND DEFINED ARG_EXE_NAME_CM7_0)
        list(APPEND cpuList cm7_0)
        set(afuOocdProgFileName_cm7_0 ${ARG_EXE_NAME_CM7_0}${CMAKE_EXECUTABLE_SUFFIX})
    endif()

    if(HAVE_CM7_1 AND DEFINED ARG_EXE_NAME_CM7_1)
        list(APPEND cpuList cm7_1)
        set(afuOocdProgFileName_cm7_1 ${ARG_EXE_NAME_CM7_1}${CMAKE_EXECUTABLE_SUFFIX})
    endif()

    if(HAVE_CM7_2 AND DEFINED ARG_EXE_NAME_CM7_2)
        list(APPEND cpuList cm7_2)
        set(afuOocdProgFileName_cm7_2 ${ARG_EXE_NAME_CM7_2}${CMAKE_EXECUTABLE_SUFFIX})
    endif()

    if(HAVE_CM7_3 AND DEFINED ARG_EXE_NAME_CM7_3)
        list(APPEND cpuList cm7_3)
        set(afuOocdProgFileName_cm7_3 ${ARG_EXE_NAME_CM7_3}${CMAKE_EXECUTABLE_SUFFIX})
    endif()

    message(STATUS "Creating Auto Flash Utility / OpenOCD targets:")
    
    set(targetNamePrefix util_afu_oocd)

    set(afuOocdCallCmd  "${AFU_OOCD_ROOT_DIR}/bin/openocd.exe" -s "${AFU_OOCD_ROOT_DIR}/scripts" -f interface/${AFU_OOCD_INTERFACE_CFG_FILE} -c "transport select swd" -f target/${AFU_OOCD_TARGET_CFG_FILE})

    # Note: The semicolons in the AFU/OOCD commands need to be escaped because they would be interpreted by CMake as list item separators otherwise

    ######################################################################################
    # Program single executable
    ######################################################################################
    set(progAllCmd "") # Used to collect the commands for the "program all" target while going through the list of single executables

    foreach(cpu IN LISTS cpuList)

        set(targetName ${targetNamePrefix}_prog_${cpu})

        message(STATUS "  ${targetName}")

        set(progCmd "program ${CMAKE_BINARY_DIR}/$<CONFIG>/${afuOocdProgFileName_${cpu}}")
        set(customCmds "init\; reset init\; ${progCmd}\; shutdown")

        set(progAllCmd "${progAllCmd} ${progCmd}\;") # Build-up/Prepare command string for "program all" target

        add_custom_target(${targetName}
            COMMAND            ${afuOocdCallCmd} -c ${customCmds}
            WORKING_DIRECTORY  ${CMAKE_SOURCE_DIR}
        )

    endforeach()

    ######################################################################################
    # Program all
    ######################################################################################

    set(targetName ${targetNamePrefix}_prog_all)

    message(STATUS "  ${targetName}")
     
    set(customCmds "init\; reset init\; ${progAllCmd}\; shutdown")

    add_custom_target(${targetName}
        COMMAND            ${afuOocdCallCmd} -c ${customCmds}
        WORKING_DIRECTORY  ${CMAKE_SOURCE_DIR}
    )

    ######################################################################################
    # Erase all
    ######################################################################################

    set(targetName ${targetNamePrefix}_erase_all)

    message(STATUS "  ${targetName}")

    set(customCmds "init\; reset init\; flash erase_sector 0 0 last\; shutdown")    

    add_custom_target(${targetName}
        COMMAND            ${afuOocdCallCmd} -c ${customCmds}
        WORKING_DIRECTORY  ${CMAKE_SOURCE_DIR}
    )

endfunction()

