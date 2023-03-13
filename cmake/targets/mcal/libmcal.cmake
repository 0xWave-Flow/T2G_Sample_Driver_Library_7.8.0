function(create_libmcal)
    
    set(libBaseName mcal)
    
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


    ###############################################################
    # Checking Tresos project preference file

    # The Makefiles included in the MCAL delivery are using some hardcoded paths. Since we want to re-use them we need to ensure
    # that the user's Tresos project setting configures the same path for the generated files.
    set(tresosProjectPrefFile ${MCAL_VAR_TRESOS_PROJECT_DIR}/.prefs/pref_general.xdm)
    if(EXISTS ${tresosProjectPrefFile})
        set(genPathExpected "output\\generated") # This is the expected value (relative from the Tresos project folder)
        file(READ ${tresosProjectPrefFile} tresosProjectPrefFileContent)
        string(REGEX MATCH [[name="GenerationPath" value="([- A-Za-z0-9_.\\/]*)"]] matchResult "${tresosProjectPrefFileContent}")
        if(NOT "${CMAKE_MATCH_1}" STREQUAL "${genPathExpected}")
            message(STATUS "Tresos project GenerationPath must be '${genPathExpected}' but is '${CMAKE_MATCH_1}'")
            if(AUTOFIX_TRESOS_GEN_PATH)
                message(STATUS "Converting GenerationPath in pref_general.xdm to expected value because AUTOFIX_TRESOS_GEN_PATH is on")
                file(COPY_FILE ${tresosProjectPrefFile} ${tresosProjectPrefFile}.bak) # Backup old version
                string(REGEX REPLACE [[name="GenerationPath" value="([- A-Za-z0-9_.\\/]*)"]] [[name="GenerationPath" value="output\\generated"]] tresosProjectPrefFileContentNew "${tresosProjectPrefFileContent}")
                file(WRITE ${tresosProjectPrefFile} "${tresosProjectPrefFileContentNew}")
            else()
                message(FATAL_ERROR " Update GenerationPath manually or set AUTOFIX_TRESOS_GEN_PATH CMake cache variable\n"
                                    " (e.g.add '-D AUTOFIX_TRESOS_GEN_PATH=ON' to the commandline)")
            endif()
        endif()
    else()
        message(FATAL_ERROR " File does not exist: ${tresosProjectPrefFile}\n"
                            " Please check MCAL_VAR_TRESOS_PROJECT_DIR")
    endif()

    ###############################################################
    # MCAL file generation and dependency checking

    set(cancelMessage "Cancelling '${libName}' processing due to errors, but continuing with CMake configuration to enable use of 'util_tresos_gui' target") # Move message to separate var because it is used at 2 places

    set(genCmakeFileBaseName  generated_cmake_file_for_mcal.cmake)
    set(genCmakeFile          ${CMAKE_BINARY_DIR}/${genCmakeFileBaseName})
    set(mcalMakeFile          ${CMAKE_SOURCE_DIR}/cmake/targets/mcal/Makefile.mak) # "Glue logic" Makefile

    if(NOT EXISTS ${genCmakeFile})
        message(STATUS "Generating MCAL files because '${genCmakeFileBaseName}' does not exist")
        generate_mcal_sources_and_cmake_file(cmdSuccess ${genCmakeFile} ${mcalMakeFile})
        if(NOT ${cmdSuccess})
            message(STATUS ${cancelMessage})
            return()
        endif()
    endif()

    # Now we read in the genCmakeFile which is either up t odate (if it did not exist earlier) or might have an unknown state (if it existed but maybe Tresos project config has changed in the meantime)
    # Also in the latter case we can use it as a starting point for upcoming dependency checking because
    # - if an MCAL plugin was added to or removed from the project, the preferences file would change
    # - if the configuration of an MCAL plugin has changed, the corresponding xdm file would change

    include(${genCmakeFile})

    set(mcalGenerationDependencyFiles
        ${tresosProjectPrefFile}               
        ${MCAL_CFG_DEPENDENT_XDM_CFG_FILES}    
        ${mcalMakeFile}
    )
    message(DEBUG "Dependency files: ${mcalGenerationDependencyFiles}")


    # If genCmakeFile already existed it could be possible that any of the dependency files is newer than it and we need to re-generate all files 
    foreach(depFile IN LISTS mcalGenerationDependencyFiles)
        
        if(${depFile} IS_NEWER_THAN ${genCmakeFile})
            get_filename_component(depFileBaseName ${depFile} NAME)
            message(STATUS "Generating MCAL files because '${depFileBaseName}' is newer than '${genCmakeFileBaseName}'")
            generate_mcal_sources_and_cmake_file(cmdSuccess ${genCmakeFile} ${mcalMakeFile})
            if(NOT ${cmdSuccess})
                message(STATUS ${cancelMessage})
                return()
            endif()

            # Include the updated file again to overwrite the variables from previous inclusion
            include(${genCmakeFile})
            
            # Update the dependency file list because MCAL_CFG_DEPENDENT_XDM_CFG_FILES might have changed
            set(mcalGenerationDependencyFiles
                ${tresosProjectPrefFile}
                ${MCAL_CFG_DEPENDENT_XDM_CFG_FILES}
                ${mcalMakeFile}
            )

            message(DEBUG "Dependency files after refresh: ${mcalGenerationDependencyFiles}")
            
            break() # No need to compare further files
        endif()

    endforeach()

    # Now mcalGenerationDependencyFiles is definitely up-to-date

    # Add the dependency files for MCAL generation as dependency files for the CMake configuration step, so that the CMake configuration step can
    # re-generate MCAL files and update the libmcal target if one of the MCAL generation dependencies has changed and a CMake build step was triggered.
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${mcalGenerationDependencyFiles})


    ###############################################################

    add_library(${libName} STATIC)

    set_target_properties(${libName}
        PROPERTIES
            ADDITIONAL_CLEAN_FILES 
                "${MCAL_VAR_TRESOS_PROJECT_DIR}/output;${genCmakeFile}"
    )

    target_include_directories(${libName}
        PUBLIC
            ${MCAL_CFG_DEPENDENT_INCLUDE_DIRS}
        PRIVATE
            ${INCLUDE_DIRS}
    )

    target_compile_definitions(${libName}
        PUBLIC
            ${MCAL_CFG_DEPENDENT_COMPILER_DEFS}
            # Defines to enable the declaration of the plugins' MainFunctions:
            CAN_DECLARATION_MAINFUNCTION
            CORTST_DECLARATION_MAINFUNCTION
            FEE_DECLARATION_MAINFUNCTION
            FLS_DECLARATION_MAINFUNCTION
            FLS_DECLARATION_SPECIAL_FUNCTION_FOR_CYFEE
            FLSTST_DECLARATION_MAINFUNCTION
            RAMTST_DECLARATION_MAINFUNCTION
            SPI_DECLARATION_MAINFUNCTION
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
            ${MCAL_CFG_DEPENDENT_SOURCE_FILES}
    )

    target_link_libraries(${libName}
        PRIVATE
            ${MCAL_CFG_DEPENDENT_ADDL_LIBS}
            mcalstubs_${ARG_CPU_TYPE}
    )

endfunction()


function(generate_mcal_sources_and_cmake_file successOutVar pathToGenCmakeFile pathToMakefile)

    set(${successOutVar} TRUE PARENT_SCOPE) # Preset return value to indicate success

    set(makeExe      ${MCAL_VAR_TRESOS_DIR}/bin/make.exe)
    set(makeFile     ${pathToMakefile})
    set(makeTargets  generate_mcal_files generate_cmake_file)

    list(APPEND makeArgs -f)
    list(APPEND makeArgs ${makeFile})
    list(APPEND makeArgs TRESOS_DIR=${MCAL_VAR_TRESOS_DIR})
    list(APPEND makeArgs TRESOS_WORKSPACE_DIR=${MCAL_VAR_TRESOS_WORKSPACE_DIR})
    list(APPEND makeArgs TRESOS_PROJECT_DIR=${MCAL_VAR_TRESOS_PROJECT_DIR})
    list(APPEND makeArgs TRESOS_PROJECT_NAME=${MCAL_VAR_TRESOS_PROJECT_NAME})
    list(APPEND makeArgs TOOLCHAIN=${TOOLCHAIN_ID_UPPERCASE})
    list(APPEND makeArgs GENERATED_CMAKE_FILE=${pathToGenCmakeFile})

    foreach(target IN LISTS makeTargets)

        execute_process(
            COMMAND ${makeExe} ${target} ${makeArgs}
            ECHO_OUTPUT_VARIABLE
            ECHO_ERROR_VARIABLE
            RESULT_VARIABLE resultVar
            OUTPUT_VARIABLE outputVar
            ERROR_VARIABLE errorVar
            TIMEOUT 120
        )

        if(resultVar) # Return code is not '0', indicating an error
            if(${target} STREQUAL generate_mcal_files)
                # Don't raise a FATAL_ERROR if the generation of MCAL files failed. This may be due to some configuration errors in the Tresos project and therefore we should
                # try to complete the CMake configuration phase so that the 'util_tresos_gui' target can then be used for launching Tresos to fix the issues in the configuration
                message(WARNING "Generation of MCAL files failed! Return code: ${resultVar}")
                set(${successOutVar} FALSE PARENT_SCOPE) # Indicate failure through return value
                return() # Abort further processing
            else()
                message(FATAL_ERROR "Generation of CMake file from MCAL files failed! Return code: ${resultVar}")
            endif()
        endif()

    endforeach()

endfunction()


