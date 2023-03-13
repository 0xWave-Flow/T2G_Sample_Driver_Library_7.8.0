######################################################################################
# print_info
######################################################################################

function(print_info)

    message(STATUS "============================================================")
    message(STATUS "T2G SDL - CMake Configuration")
    message(STATUS "------------------------------------------------------------")
    message(STATUS "Toolchain         = ${TOOLCHAIN_ID}")
    message(STATUS "DIE               = ${DIE}")
    message(STATUS "MCU revision      = ${MCU_REV}")
    message(STATUS "Device            = ${DEVICE}")
    message(STATUS "Link location     = ${LINK}")
    message(STATUS "PSVP              = ${PSVP}")
    message(STATUS "Board             = ${BOARD}")
    message(STATUS "Board revision    = ${BOARD_REV}")
    message(STATUS "Use RTOS          = ${USE_RTOS}")
    message(STATUS "Use ETH Stack     = ${USE_ETHSTACK}")
    message(STATUS "Use MCAL          = ${USE_MCAL}")
    if(USE_MCAL)
        message(STATUS "Tresos            = ${MCAL_VAR_TRESOS_DIR}")
        message(STATUS "Tresos workspace  = ${MCAL_VAR_TRESOS_WORKSPACE_DIR}")
        message(STATUS "Tresos project    = ${MCAL_VAR_TRESOS_PROJECT_DIR}")
    endif()
    message(STATUS "============================================================")
    message(DEBUG  "Other variables for debugging CMake")
    message(DEBUG  "------------------------------------------------------------")
    message(DEBUG  "CMAKE_TOOLCHAIN_FILE         = ${CMAKE_TOOLCHAIN_FILE}")
    message(DEBUG  "CMAKE_C_FLAGS_DEBUG          = ${CMAKE_C_FLAGS_DEBUG}")
    message(DEBUG  "CMAKE_C_FLAGS_RELEASE        = ${CMAKE_C_FLAGS_RELEASE}")
    message(DEBUG  "CMAKE_C_FLAGS_RELWITHDEBINFO = ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
    message(DEBUG  "CMAKE_C_FLAGS_MINSIZEREL     = ${CMAKE_C_FLAGS_MINSIZEREL}")
    message(DEBUG  "============================================================")

endfunction()


######################################################################################
# get_latest_mcu_revision
######################################################################################

function(get_latest_mcu_revision outVar)

    file(GLOB revFolderPaths "${CMAKE_CURRENT_SOURCE_DIR}/${DIE}/hdr/rev_*")
    message(DEBUG "revFolderPaths = ${revFolderPaths}")

    list(SORT revFolderPaths)

    list(POP_BACK revFolderPaths latestRevFolderPath)
    message(DEBUG "latestRevFolderPath = ${latestRevFolderPath}")

    string(LENGTH ${latestRevFolderPath} length)
    message(DEBUG "length = ${length}")
    
    math(EXPR lastCharIndex "${length} - 1")
    message(DEBUG "lastCharIndex = ${lastCharIndex}")

    string(SUBSTRING ${latestRevFolderPath} ${lastCharIndex} 1 latestRev)
    string(TOUPPER ${latestRev} latestRev)

    message(STATUS "Latest MCU revision detected: ${latestRev}")

    set(${outVar} ${latestRev} PARENT_SCOPE)

endfunction()


######################################################################################
# printFunctionArgs
######################################################################################

function(printFunctionArgs functionName noValueArgs singleValueArgs multiValueArgs)
    
    # Log details for each supported keyword
    message(DEBUG "Argument summary for function '${functionName}':")
    foreach(arg IN LISTS noValueArgs)
        if(${${prefix}_${arg}})
            message(DEBUG "  ${arg} enabled")
        else()
            message(DEBUG "  ${arg} disabled")
        endif()
    endforeach()
    foreach(arg IN LISTS singleValueArgs multiValueArgs)
        # Single argument values will print as a simple string
        # Multiple argument values will print as a list
        message(DEBUG "  ${arg} = ${${prefix}_${arg}}")
    endforeach()

endfunction()

