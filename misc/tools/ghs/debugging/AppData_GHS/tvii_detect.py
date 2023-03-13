#/******************************************************************************#
#* $Revision: 137899 $
#* $Date:: 2019-03-25 17:41:36 +0530#$
# *****************************************************************************/#
#/* __DISCLAIMER_START__                                                      */#
#/*****************************************************************************/#
#* (c) 2014-2018, Cypress Semiconductor Corporation                             #
#* or a subsidiary of Cypress Semiconductor Corporation. All rights             #
#* reserved.                                                                    #
#*                                                                              #
#* This software, including source code, documentation and related              #
#* materials ( "Software" ), is owned by Cypress Semiconductor                  #
#* Corporation or one of its subsidiaries ( "Cypress" ) and is protected by     #
#* and subject to worldwide patent protection (United States and foreign),      #
#* United States copyright laws and international treaty provisions.            #
#* Therefore, you may use this Software only as provided in the license         #
#* agreement accompanying the software package from which you                   #
#* obtained this Software ( "EULA" ).                                           #
#*                                                                              #
#* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,      #
#* non-transferable license to copy, modify, and compile the                    #
#* Software source code solely for use in connection with Cypress' s            #
#* integrated circuit products. Any reproduction, modification, translation,    #
#* compilation, or representation of this Software except as specified          #
#* above is prohibited without the express written permission of Cypress.       #
#*                                                                              #
#* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO                         #
#* WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING,                         #
#* BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED                                 #
#* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A                              #
#* PARTICULAR PURPOSE. Cypress reserves the right to make                       #
#* changes to the Software without notice. Cypress does not assume any          #
#* liability arising out of the application or use of the Software or any       #
#* product or circuit described in the Software. Cypress does not               #
#* authorize its products for use in any products where a malfunction or        #
#* failure of the Cypress product may reasonably be expected to result in       #
#* significant property damage, injury or death ( "High Risk Product" ). By     #
#* including Cypress' s product in a High Risk Product, the manufacturer        #
#* of such system or application assumes all risk of such use and in doing      #
#* so agrees to indemnify Cypress against all liability.                        #
#******************************************************************************/#
#/* __DISCLAIMER_END__                                                        */#
#/*****************************************************************************/#
# ***********************************************************************************************/#
# \file tvii_detect.py
#
# \brief GHS debugger script
#
# History:
# 2018-04-20 0.01 CHEY  Initial version for Traveo II family
# 2018-05-22 0.02 CHEY  New "Flash build" detection because of different SRAM location in TVII-B-H-8M
# 2019-03-25 0.03 CHEY  Enhanced detection mechanism to also support use cases where the current
#                       working directory of MULTI is not within the actual project (e.g. if MULTI
#                       Project Manager has been started from within C:\ghs\multiXYZ)
#                       Checks the detected path if it contains a "space" and exits the script then
# ***********************************************************************************************/#



# ############################## START OF USER CONFIGURATION ###############################

#-----------------------------------------------------------------------------------------

# 1st method:
# List all relative paths from the current working directory (CWD) of MULTI to the tvii_debug.py
# folder from all your possible project structures. This script will try to search and pick the
# right one for the current project from the list below.
# Note: This method may only work if the CWD points into the current project structure, e.g.
#       because top/root .gpj is associated with MULTI Project Manager and was used to open MULTI
REL_SCRIPT_SEARCH_PATHS_FROM_CUR_WORK_DIR = [
                                                "./debugging",                          # e.g. from <my_project>/ghs to <my_project>/ghs/debugging
                                                "../../common-CYT2/debugger_scripts",   # e.g. from <all_projects>/<my_project>/util to <all_projects>/common-CYT2/debugger_scripts
                                            ]

# 2nd method (if 1st fails)
# List all relative paths from the directory of the .ghsmc file to the tvii_debug.py
# folder from all your possible project structures. This script will try to search and pick the
# right one for the current project from the list below.
# Note: This method is a fallback solution if the CWD does not point into the current project structure, e.g.
#       because MULTI Project Manager has been opened directly from within C:\ghs\multiXYZ
REL_SCRIPT_SEARCH_PATHS_FROM_GHSMC_DIR =    [
                                                "../debugging",                         # e.g. from <my_project>/ghs/flash/cm0plus_only.ghsmc to <my_project>/ghs/debugging
                                                "../../common-CYT2/debugger_scripts",   # e.g. from <all_projects>/<my_project>/util/debug.ghsmc to <all_projects>/common-CYT2/debugger_scripts
                                            ]



# ############################## END OF USER CONFIGURATION #################################


# ############################## START OF DEFINITIONS ######################################
import os
import sys
import time

# Script settings (e.g. to debug the Python script)
ENABLE_DEBUG_COMMAND_OUTPUT = False  # Set True or False


#-----------------------------------------------------------------------------------------
# Register addresses and other definitions used in the script:

# These defines are only used for the distinction of Flash and RAM builds, so a Traveo II
# "superset" code flash size is just fine
CODE_FLASH_BASE_ADDRESS = 0x10000000
CODE_FLASH_SIZE_BYTES   = 8384 * 1024


# ############################## END OF DEFINITIONS ########################################


############################## START OF FUNCTIONS ########################################

# Helper functions

# Convert number to hex string without potential "L"-suffix, optionally the width (i.e. number of digits) can be specified,
# e.g. to ensure a string with 8 hex digits (leading 0s)
def ConvertToHex(value, width=0):
    return hex(value).rstrip("L") if (width == 0) else "{0:#0{1}x}".format(value, width + 2) # width + 2 because '0x' is included in the count

# Executes a debugger command
def ExecDebugCmd(cmd_string, overwrite_print=False):
    debugger.RunCommands(cmd_string, printOutput=(ENABLE_DEBUG_COMMAND_OUTPUT or overwrite_print))

# Returns the execution output string from the last executed debugger command
def GetDebugCmdOutput():
    return debugger.cmdExecOutput

# Exits the script with an error return value and optionally prints a message
def ExitScriptWithError(message=""):
    print(message)
    sys.exit(1)

#-----------------------------------------------------------------------------------------
# Traveo II functions

# Simple check whether application is linked to Flash
# Returns True if symbol address of "main" is within CODE_FLASH region, otherwise False
def IsApplicationLinkedForFlash():
    main_address = debugger.GetSymbolAddress("main")
    if (main_address >= CODE_FLASH_BASE_ADDRESS) and (main_address < (CODE_FLASH_BASE_ADDRESS + CODE_FLASH_SIZE_BYTES)):
        return True
    else:
        return False



############################## END OF FUNCTIONS ##########################################


############################## START OF SCRIPT ###########################################


#-----------------------------------------------------------------------------------------
# Get path strings to some important files/folders

CURRENT_WORKING_DIR = os.path.normpath(os.getcwd()).replace("\\", "/")
print("Current working directory:   " + CURRENT_WORKING_DIR)

ExecDebugCmd("$_MC_CONFIG_FILE") # Path to current .ghsmc file
try:
    PATH_TO_GHSMC_FILE     = os.path.normpath(GetDebugCmdOutput().split("= ")[1].strip("\"")).replace("\\", "/")
    PATH_TO_GHSMC_FILE_DIR = os.path.dirname(PATH_TO_GHSMC_FILE)
except:
    ExitScriptWithError("ERROR: Could not detect path to .ghsmc file!")

#-----------------------------------------------------------------------------------------

# Search/identify the path to the tvii_debug.py script
PATH_TO_TVII_DEBUG_SCRIPT = None

# First try to find tvii_debug.py with relative path from current working directory...
for cur_dir in REL_SCRIPT_SEARCH_PATHS_FROM_CUR_WORK_DIR:
    temp_rel_path = cur_dir + "/tvii_debug.py"
    if os.path.isfile(CURRENT_WORKING_DIR + "/" + temp_rel_path) is True:
        # Use relative path because current working directory seems to be within the current project
        PATH_TO_TVII_DEBUG_SCRIPT = temp_rel_path
        abs_path_for_print_only = os.path.normpath(CURRENT_WORKING_DIR + "/" + PATH_TO_TVII_DEBUG_SCRIPT).replace("\\", "/")
        print("TVII debug script found at:  " + abs_path_for_print_only + "  (using relative path from current working directory)")
        break

# ... if that fails try to find tvii_debug.py with relative path from .ghsmc file directory and use absolute path then.
if PATH_TO_TVII_DEBUG_SCRIPT is None:
    for cur_dir in REL_SCRIPT_SEARCH_PATHS_FROM_GHSMC_DIR:
        temp_rel_path = cur_dir + "/tvii_debug.py"
        if os.path.isfile(PATH_TO_GHSMC_FILE_DIR + "/" + temp_rel_path) is True:
            print("Current .ghsmc file:         " + PATH_TO_GHSMC_FILE)
            # Use absolute path because current working directory seems to be unrelated to the current project
            PATH_TO_TVII_DEBUG_SCRIPT = os.path.normpath(PATH_TO_GHSMC_FILE_DIR + "/" + temp_rel_path).replace("\\", "/")
            print("TVII debug script found at:  " + PATH_TO_TVII_DEBUG_SCRIPT + "  (using absolute path)")
            break

if PATH_TO_TVII_DEBUG_SCRIPT is None:
    ExitScriptWithError("ERROR: Could not find location of tvii_debug.py!")

if PATH_TO_TVII_DEBUG_SCRIPT.find(" ") != -1:
    # Nesting of strings within 'debugbutton' command further below does not work, but would be needed in case of spaces in the path name
    ExitScriptWithError("ERROR: Spaces in path to debug script are not supported!")


#-----------------------------------------------------------------------------------------
# Create the debug action buttons and remove the "detect" button again
        
command_verify        = "python -b -f " + PATH_TO_TVII_DEBUG_SCRIPT + " VERIFY"
command_ram_download  = "python -b -f " + PATH_TO_TVII_DEBUG_SCRIPT + " RAM"
command_flash_program = "python -b -f " + PATH_TO_TVII_DEBUG_SCRIPT + " FLASH"
command_erase         = "python -b -f " + PATH_TO_TVII_DEBUG_SCRIPT + " ERASE"


ExecDebugCmd("debugbutton traveo_ii_detect") # Delete the "D" (Detect) button again, no longer needed. (Comment this line for easier debugging of this script)

if IsApplicationLinkedForFlash() is False:
    ExecDebugCmd("debugbutton traveo_ii_ram    c=\"" + command_ram_download  + "\" i=\"letter_r16\" t=\"Traveo II: Debug and load RAM\"")
else:
    ExecDebugCmd("debugbutton traveo_ii_flash  c=\"" + command_flash_program + "\" i=\"letter_f16\" t=\"Traveo II: Debug and flash\"")
    
# Following buttons are available for any build configuration
ExecDebugCmd("debugbutton traveo_ii_verify c=\"" + command_verify + "\" i=\"letter_v16\" t=\"Traveo II: Debug and verify only\"")
ExecDebugCmd("debugbutton traveo_ii_erase  c=\"" + command_erase  + "\" i=\"letter_e16\" t=\"Traveo II: Debug and erase\"")


############################## END OF SCRIPT #############################################
