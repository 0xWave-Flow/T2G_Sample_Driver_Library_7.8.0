function(create_vsc_launch_json)

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


    set(LAUNCH_JSON_CONTENT) # explicit init, this string will be used in the end to configure the launch.json template file


    ######################################################################################
    # Template strings for later processing by 'string(CONFIGURE ...)' function
    ######################################################################################

    # The templates below contain "static" variables and "dynamic" variables which are both enclosed by @@ characters
    # to avoid conflicts with JSON syntax.
    # Dynamic variable names start with TEMPLATE_VAR_... and will be updated by the later code based on the currently
    # processed debug configuration. Static variables originate from various places (e.g. tool configuration) and are
    # identical for each debug configuration.


    set(jsonMainCpuTemplateString
[==[
        {
            "name": "@TEMPLATE_VAR_MAIN_CFG_NAME@ (OpenOCD)",
            "type": "cortex-debug",
            "cwd": "${workspaceFolder}",
            "armToolchainPath": "@GCC_COMPILER_ROOT_DIR@/bin",
            "svdFile": "${workspaceFolder}/misc/tools/svd/@DIE@/@MCU_REV_STRING@/@SERIES@.svd",
            "servertype": "openocd",
            "serverpath": "@AFU_OOCD_ROOT_DIR@/bin/openocd.exe",
            "searchDir": [
                "@AFU_OOCD_ROOT_DIR@/scripts"
            ],
            "configFiles": [
                "interface/@AFU_OOCD_INTERFACE_CFG_FILE@",
                "target/@AFU_OOCD_TARGET_CFG_FILE@",
            ],
            "openOCDPreConfigLaunchCommands": [
                "set ENABLE_ACQUIRE 0" // Ensure regular boot flow (prevent entering of "TEST_MODE")
            ],
            "numberOfProcessors": @TEMPLATE_VAR_NR_OF_CPUS@,
            "targetProcessor": 0,            
            "executable": "${command:cmake.getLaunchTargetDirectory}/@ARG_EXE_NAME_CM0PLUS@@CMAKE_EXECUTABLE_SUFFIX@",
            "request": "launch",
            "loadFiles": [
                "${command:cmake.getLaunchTargetDirectory}/@ARG_EXE_NAME_CM0PLUS@@CMAKE_EXECUTABLE_SUFFIX@",
@TEMPLATE_VAR_ADDL_LOAD_FILES@
            ],
            "runToEntryPoint": "main",
@TEMPLATE_VAR_CHAINED_CFG@            
        },
]==])


    set(jsonChainedConfigurationsTemplateString
[==[
            "chainedConfigurations": {
                "enabled": true,
                "waitOnEvent": "postInit",
                "detached": false, // OpenOCD requires false
                "lifecycleManagedByParent": true,
                "launches": [
@TEMPLATE_VAR_LAUNCH_ENTRIES@
                ]
            },
]==])

    set(jsonAppCpuTemplateString
[==[
        {
            "name": "@TEMPLATE_VAR_SUB_CFG_NAME@ (OpenOCD)",
            "type": "cortex-debug",
            "presentation": {
                "hidden": true,
            },
            "cwd": "${workspaceFolder}",
            "servertype": "openocd",
            "targetProcessor": @TEMPLATE_VAR_TARGET_CPU_INDEX@,            
            "executable": "${command:cmake.getLaunchTargetDirectory}/@TEMPLATE_VAR_SUB_CFG_EXE_NAME@@CMAKE_EXECUTABLE_SUFFIX@",
            "request": "launch", // 'attach' does not work properly, therefore use 'launch' and override the launch commands 
            "overrideLaunchCommands": [],
            "runToEntryPoint": "main",
        },
]==])


    set(jsonLoadFileEntryTemplateString
[==[
                "${command:cmake.getLaunchTargetDirectory}/@TEMPLATE_VAR_LOAD_FILE_EXE_NAME@@CMAKE_EXECUTABLE_SUFFIX@",
]==])

    set(jsonLaunchEntryTemplateString
[==[
                    {
                        "name": "@TEMPLATE_VAR_LAUNCH_CFG_NAME@ (OpenOCD)",
                    },
]==])


    ######################################################################################
    # Misc
    ######################################################################################

    # Determinate derivate specific total amount of CPU cores
    if(HAVE_CM7_3)
        set(TEMPLATE_VAR_NR_OF_CPUS 5)
    elseif(HAVE_CM7_1)
        set(TEMPLATE_VAR_NR_OF_CPUS 3)
    else()
        set(TEMPLATE_VAR_NR_OF_CPUS 2)
    endif()


    ######################################################################################
    # Prepare lists required for creating all relevant configurations
    ######################################################################################

    # "CM0+ only" debug config will always be created
    list(APPEND listMainCfgNames cm0plus)
    list(APPEND listNrOfChainedCfgs 0)


    if(HAVE_CM4 AND DEFINED ARG_EXE_NAME_CM4)
        # Lists for creation of main configurations
        list(APPEND listMainCfgNames         cm0plus_cm4)
        list(APPEND listNrOfChainedCfgs      1)
        list(APPEND listLaunchCfgNames       cm4)
        list(APPEND listAddlLoadFileExeNames ${ARG_EXE_NAME_CM4})
        # Lists for creation of sub (chained) configurations
        list(APPEND listSubCfgNames          cm4)
        list(APPEND listSubCfgExeNames       ${ARG_EXE_NAME_CM4})
        list(APPEND listTargetCpuIndex       1)
    endif()

    if(HAVE_CM7_0 AND DEFINED ARG_EXE_NAME_CM7_0)
        # Lists for creation of main configurations
        list(APPEND listMainCfgNames         cm0plus_cm7_0)
        list(APPEND listNrOfChainedCfgs      1)
        list(APPEND listLaunchCfgNames       cm7_0)
        list(APPEND listAddlLoadFileExeNames ${ARG_EXE_NAME_CM7_0})
        # Lists for creation of sub (chained) configurations
        list(APPEND listSubCfgNames          cm7_0)
        list(APPEND listSubCfgExeNames       ${ARG_EXE_NAME_CM7_0})
        list(APPEND listTargetCpuIndex       1)
    endif()

    if(HAVE_CM7_1 AND DEFINED ARG_EXE_NAME_CM7_1)
        # Lists for creation of main configurations
        list(APPEND listMainCfgNames         cm0plus_cm7_1)
        list(APPEND listNrOfChainedCfgs      1)
        list(APPEND listLaunchCfgNames       cm7_1)
        list(APPEND listAddlLoadFileExeNames ${ARG_EXE_NAME_CM7_1})

        if(DEFINED ARG_EXE_NAME_CM7_0)
            list(APPEND listMainCfgNames         cm0plus_cm7_dual)
            list(APPEND listNrOfChainedCfgs      2)
            list(APPEND listLaunchCfgNames       cm7_0 cm7_1)
            list(APPEND listAddlLoadFileExeNames ${ARG_EXE_NAME_CM7_0} ${ARG_EXE_NAME_CM7_1})
        endif()

        # Lists for creation of sub (chained) configurations
        list(APPEND listSubCfgNames          cm7_1)
        list(APPEND listSubCfgExeNames       ${ARG_EXE_NAME_CM7_1})
        list(APPEND listTargetCpuIndex       2)
    endif()

    if(HAVE_CM7_2 AND DEFINED ARG_EXE_NAME_CM7_2)
        # Lists for creation of main configurations
        list(APPEND listMainCfgNames         cm0plus_cm7_2)
        list(APPEND listNrOfChainedCfgs      1)
        list(APPEND listLaunchCfgNames       cm7_2)
        list(APPEND listAddlLoadFileExeNames ${ARG_EXE_NAME_CM7_2})
        # Lists for creation of sub (chained) configurations
        list(APPEND listSubCfgNames          cm7_2)
        list(APPEND listSubCfgExeNames       ${ARG_EXE_NAME_CM7_2})
        list(APPEND listTargetCpuIndex       3)
    endif()

    if(HAVE_CM7_3 AND DEFINED ARG_EXE_NAME_CM7_3)
        # Lists for creation of main configurations
        list(APPEND listMainCfgNames         cm0plus_cm7_3)
        list(APPEND listNrOfChainedCfgs      1)
        list(APPEND listLaunchCfgNames       cm7_3)
        list(APPEND listAddlLoadFileExeNames ${ARG_EXE_NAME_CM7_3})

        if((DEFINED ARG_EXE_NAME_CM7_0) AND (DEFINED ARG_EXE_NAME_CM7_1) AND (DEFINED ARG_EXE_NAME_CM7_2))
            list(APPEND listMainCfgNames         cm0plus_cm7_quad)
            list(APPEND listNrOfChainedCfgs      4)
            list(APPEND listLaunchCfgNames       cm7_0 cm7_1 cm7_2 cm7_3)
            list(APPEND listAddlLoadFileExeNames ${ARG_EXE_NAME_CM7_0} ${ARG_EXE_NAME_CM7_1} ${ARG_EXE_NAME_CM7_2} ${ARG_EXE_NAME_CM7_3})
        endif()

        # Lists for creation of sub (chained) configurations
        list(APPEND listSubCfgNames          cm7_3)
        list(APPEND listSubCfgExeNames       ${ARG_EXE_NAME_CM7_3})
        list(APPEND listTargetCpuIndex       4)
    endif()


    # Loop through and generate all the main debug configurations (combinations of CM0+ and subsets or all application cores)
    foreach(TEMPLATE_VAR_MAIN_CFG_NAME IN LISTS listMainCfgNames)

        # Init/Clear vars/lists used within templates and which need to be cleared for every iteration
        # (because items are appended to them or they are optional)
        set(TEMPLATE_VAR_ADDL_LOAD_FILES)
        set(TEMPLATE_VAR_LAUNCH_ENTRIES)
        set(TEMPLATE_VAR_CHAINED_CFG)

        list(POP_FRONT listNrOfChainedCfgs nrOfChainedCfgs)

        # If the main configuration includes chained subconfigurations the corresponding 
        # launch entries and (additional) load file entries need to be created
        if(${nrOfChainedCfgs})
            foreach(index RANGE 1 ${nrOfChainedCfgs})
                # Additional load files (ELF)
                list(POP_FRONT listAddlLoadFileExeNames TEMPLATE_VAR_LOAD_FILE_EXE_NAME)
                string(CONFIGURE ${jsonLoadFileEntryTemplateString} tempString @ONLY)
                string(APPEND TEMPLATE_VAR_ADDL_LOAD_FILES ${tempString})
                # Chained configurations
                list(POP_FRONT listLaunchCfgNames TEMPLATE_VAR_LAUNCH_CFG_NAME)
                string(CONFIGURE ${jsonLaunchEntryTemplateString} tempString @ONLY)
                string(APPEND TEMPLATE_VAR_LAUNCH_ENTRIES ${tempString})
            endforeach()
            # Since the main configuration contains chained configurations, the respective JSON element shall be prepared:
            string(CONFIGURE ${jsonChainedConfigurationsTemplateString} TEMPLATE_VAR_CHAINED_CFG @ONLY)
        endif()

        # Now that all relevant template vars have there right value, the current main configuration can be added to the var
        # that represents the final file launch.json content.
        string(CONFIGURE ${jsonMainCpuTemplateString} tempString @ONLY)
        string(APPEND LAUNCH_JSON_CONTENT ${tempString})

    endforeach()

    # Loop through and generate all the configurations for the application cores (which are chained by above main configurations)
    foreach(TEMPLATE_VAR_SUB_CFG_NAME IN LISTS listSubCfgNames)
        # Get the corresponding executable file for this application core and the OOCD target CPU index (CoreSight AP index)
        list(POP_FRONT listSubCfgExeNames TEMPLATE_VAR_SUB_CFG_EXE_NAME)
        list(POP_FRONT listTargetCpuIndex TEMPLATE_VAR_TARGET_CPU_INDEX)

        string(CONFIGURE ${jsonAppCpuTemplateString} tempString @ONLY)
        string(APPEND LAUNCH_JSON_CONTENT ${tempString})

    endforeach()

    # message(STATUS "${LAUNCH_JSON_CONTENT}")


    ######################################################################################
    # Create actual launch.json file
    ######################################################################################

    set(configureFileName launch.json)
    set(configureFileSrc  ${CMAKE_SOURCE_DIR}/cmake/template_files/dbg_vsc/${configureFileName})
    set(configureFileDst  ${CMAKE_SOURCE_DIR}/.vscode/${configureFileName})    

    message(STATUS "Creating Visual Studio Code debugger launch configuration")
    message(STATUS "  Writing file ${configureFileDst}")
    configure_file(${configureFileSrc} ${configureFileDst} @ONLY)

endfunction()

