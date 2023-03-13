function(create_tresos_utility_targets)

    # Define the supported set of keywords
    set(prefix ARG)
    set(noValueArgs)
    set(singleValueArgs)
    set(multiValueArgs)

    cmake_parse_arguments(${prefix} "${noValueArgs}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    printFunctionArgs(${CMAKE_CURRENT_FUNCTION} "${noValueArgs}" "${singleValueArgs}" "${multiValueArgs}")

    ########################################################################################################

    message(STATUS "Creating Tresos Utility targets:")

    # Prior importing (or check whether project is imported already) is not necessary as this would have been ensured by create_libmcal() in libmcal.cmake already

    set(targetName util_tresos_gui)

    message(STATUS "  ${targetName}")

    add_custom_target(${targetName}
        COMMAND            start ${MCAL_VAR_TRESOS_DIR}/bin/tresos_gui.exe -data ${MCAL_VAR_TRESOS_WORKSPACE_DIR}
        WORKING_DIRECTORY  ${CMAKE_SOURCE_DIR}
    )

endfunction()

