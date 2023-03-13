#/******************************************************************************#
#* $Revision: 310426 $
#* $Date:: 2022-02-15 17:21:30 +0530#$
# *****************************************************************************/#
#/* __DISCLAIMER_START__                                                      */#
#/*****************************************************************************/#
#* (c) 2014-2017, Cypress Semiconductor Corporation                             #
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
# \file tvii_debug.py
#
# \brief GHS debugger script
#
# History:
# 2018-05-15 0.01 CHEY  Initial version for TVII-B-H-8M series (based on v0.12 of TVII-B-E-1M tvii_debug.py)
# 2018-05-22 0.02 CHEY  Some changes to support first PSVP bitfile
# 2018-06-07 0.03 CHEY  Further changes to support first PSVP bitfiles, not all functionality is
#                       working now (e.g. "VERIFY" mode), major changes/improvements might be
#                       possible once final ROM boot / flash boot is included
# 2018-06-20 0.04 CHEY  Hack for CMR3 bitfile SRAM build cfg: Vector table moved (here and linker file).
#                       Some preparations for flash build configuration. 
# 2018-07-31 0.05 CHEY  some changes for all execution modes (FLASH, RAM, VERIFY, ERASE)
# 2018-08-16 0.06 CHEY  Probe configuration changed to SWD. Note that matching *.ghsmc files are needed!
#                       'IS_PSVP_WITHOUT_FLASH_BOOT_FW' split into 2 separate configuration settings.
#                       Some other fixes and improvements
# 2018-08-16 0.07 CHEY  Script should now be able to cope with spaces in the path to the project
# 2018-08-27 0.08 CHEY  Added selection of connection type (USB or Ethernet)
# 2018-09-06 0.09 CHEY  Added ID print and debug buttons for resume/halt all cores
# 2018-11-01 0.10 CHEY  Changed vector table base retrieval method in PatchProgramEntryForRamBuild()
# 2018-11-02 0.11 CHEY  Previous change does not work reliably, so using a different method
# 2018-11-06 0.12 CHEY  Major change to debug script:
#                       - CM7 cores are now powered-up by script and let run in ROM boot. This allows
#                         a similar handling for many features as in CM4 based TVII scripts
#                       - Other refactoring and printing of more detailed target information
#                       !!! Ensure to update your custom .ghsmc files if not using the SDL project !!!
# 2018-11-22 0.13 CHEY  Fixed issue that CM0+ does not halt at configured user breakpoint in case of
#                       of Flash build. Added support for routing printf() to debugger "I/O" tab.
# 2018-11-22 0.14 CHEY  Added clearing of PRIMASK before flash programming (may help for some flash issues)
# 2019-02-13 0.15 CHEY  - Changed default settings for silicon with final FlashBoot (-> reset vector catch)
#                       - Improved robustness of flash programming by ensuring certain system settings
#                       - Changed CM7 power mode transitions so that they stay in DBG_RSTOFF (WFI loop in SROM)
#                       - Printing more target information and warning depending on probe firmware/settings
#                       - Adding support for read/write via SysAP (not yet used by script)
#                       - Setting CM7 TCM (RMW) and AHB Slave Port init signals to "enabled"
# 2019-02-26 0.16 CHEY  - Added automatic determination of CM0_RESET_VECTOR_CATCH_IS_WORKING
#                       - Improved functionality that ensures that program stops at user breakpoint
#                       - Commented setting of CM7 TCM (RMW) and AHB Slave Port init signals to "enabled" again
# 2019-03-15 0.17 CHEY  Fixed issue in CheckProbeSettings()
# 2019-03-19 0.18 CHEY  Setting init values for DTCM in CM7_X_CTL, needed when CM7 stacks are linked to DTCM
#                       because GHS checks if stack pointer is valid(?)
# 2019-03-29 0.19 CHEY  Prepare trace setup (not functional yet, may require (beta) Probe firmware 5.8)
# 2019-06-27 0.20 CHEY  SetUserBreakpoints() now considers cache maintenance in case of CM7
# 2019-10-25 0.21 CHEY  Move SRAM base address for GHS flash kernel to support new MCU revisions
#                       with relocated SROM private RAM
# 2019-12-09 0.22 CHEY  - Migrated to 'argparser' Python module and introducing new optional parameter for script
#                         that will skip setting of user breakpoints (used during certain automated tests)
#                       - adding halt of CM0+ core after initial XRES
#                       - removed obsolete "enforced approval mode" workaround and other minor changes
# 2020-01-16 0.23 CHEY  Added API for executing DAP system calls -> added output of current life-cycle state
# 2020-01-31 0.24 CHEY  Changed trace setup and some related parts (see Readme.txt of SDL for version/tool information)
# 2020-02-07 0.25 CHEY  Adding print of SFLASH data revision, split printing of target info in 2 lines now
# 2020-07-10 0.26 CHEY  - Improve IsTpiuTraceProbe() for certain cases
#                       - Added "controlled" reset before PrintTargetInformation (otherwise syscalls might fail)
#                       - Create 'debugger' object if not existing (e.g. if script is called in other environment)
# 2020-10-06 0.27 CHEY  Clear some control bits in DHCSR register before triggering SYSRESET to fix problem
# 2020-10-20 0.28 CHEY  Check for "Could not detect flash device" message and print potential solution
# 2021-09-22 0.29 CHEY  Add support for SFLASH user row programming
# ***********************************************************************************************/#


############################## START OF USER CONFIGURATION ###############################

#-----------------------------------------------------------------------------------------
# Select whether the target is a PSVP (pre-silicon validation platform based on FPGA) or
# the actual final silicon chip
IS_PSVP = False  # Set True or False

#-----------------------------------------------------------------------------------------
# Set the label/address (see GHS manual for supported formats) where the
# debugger will stop the CM0+ process after the connection process. Must be a string.
BREAKPOINT_LABEL_M0 = "main"

# Set the label/address (see GHS manual for supported formats) where the
# debugger will put a breakpoint for the CM7_0/CM7_1 process. Must be a string.
# Note: This breakpoint can typically only be hit after CM7_0/CM7_1 has been enabled by CM0+.
BREAKPOINT_LABEL_M7_0 = "main"
BREAKPOINT_LABEL_M7_1 = "main"

#-----------------------------------------------------------------------------------------
# If the script is executed in "VERIFY" mode, please select if verification shall only be done sparsely (instead of completely)
VERIFY_SPARSELY_ONLY = False  # Set True or False

#-----------------------------------------------------------------------------------------
# Select whether SFLASH user rows shall/can be programmed or not (if executable contains data linked to the corresponding address space)
PERMIT_SFLASH_USER_ROW_PROGRAMMING = False  # Set True or False

#-----------------------------------------------------------------------------------------
# Select the connection method to the debug probe: USB or Ethernet. For the Ethernet connection
# the probe's IP address or host name (by default "ghprobe<serial_number>") can be specified
CONNECTION_TYPE = "-usb"  # Set to "-usb", "<IP address>" (e.g "10.10.10.10") or "<host name>" (e.g. "ghprobe12345")

#-----------------------------------------------------------------------------------------
# Select which CM7 instance(s) shall be configured for trace (note that simultaneous trace is possible)
# Only set to True if your MULTI IDE/Compiler version & patch level, and probe type and firmware support Cortex-M7 trace!
TRACE_CM7_0 = False  # Set True or False
TRACE_CM7_1 = False  # Set True or False

# Select whether off-chip trace via Trace Port Interface Unit (TPIU) shall be used instead
# of on-chip Embedded Trace Buffer (ETB)
# If TPIU shall be used, the GPIO pins which are shared with trace signals are configured for trace
# Please ensure that your trace adapter is 5V capable or external level shifters are used!
USE_TPIU_OFFCHIP_TRACE = False  # Set True or False

# Trace signal routing may be different for the available CPU boards and packages, so the actual target board must be set here
CPU_BOARD_TYPE = 'CPU_BOARD_176_QFP'  # Set 'CPU_BOARD_176_QFP', 'CPU_BOARD_272_BGA' or 'CPU_BOARD_320_BGA'

# This trace clock divider setting will be made if USE_TPIU_OFFCHIP_TRACE is True 
TRACE_CLK_DIV    = 2  # Clock divider (will divide CLK_PERI to get PCLK_CPUSS_CLOCK_TRACE_IN, which is twice the frequency that will be output on the pin)
TRACE_CLK_DIV_NR = 3  # Divider instance number. Index is 0-based, default value is the last "8.0" type divider of this device

# Define which port pins shall be used for TPIU trace. These settings will only be applied if USE_TPIU_OFFCHIP_TRACE is True.
# It should be checked that the provided settings are matching the actual CPU board or custom hardware.
# Note: Please also check if any jumpers need to be changed on your CPU board as the trace signals may be shared with other functionality!
TRACE_PIN_CONFIG = {
                        'CPU_BOARD_176_QFP' : {
                                                'CLK'    : { 'PORT' : 22, 'PIN' : 4, 'HSIOM_SEL' : 27 },
                                                'DATA_0' : { 'PORT' : 18, 'PIN' : 4, 'HSIOM_SEL' : 27 },
                                                'DATA_1' : { 'PORT' : 18, 'PIN' : 5, 'HSIOM_SEL' : 27 },
                                                'DATA_2' : { 'PORT' : 18, 'PIN' : 6, 'HSIOM_SEL' : 27 },
                                                'DATA_3' : { 'PORT' : 18, 'PIN' : 7, 'HSIOM_SEL' : 27 },
                                            },
                        'CPU_BOARD_272_BGA' : {
                                                'CLK'    : { 'PORT' : 22, 'PIN' : 4, 'HSIOM_SEL' : 27 },
                                                'DATA_0' : { 'PORT' : 21, 'PIN' : 5, 'HSIOM_SEL' : 27 },
                                                'DATA_1' : { 'PORT' : 22, 'PIN' : 1, 'HSIOM_SEL' : 27 },
                                                'DATA_2' : { 'PORT' : 22, 'PIN' : 2, 'HSIOM_SEL' : 27 },
                                                'DATA_3' : { 'PORT' : 22, 'PIN' : 3, 'HSIOM_SEL' : 27 },
                                            },
                        'CPU_BOARD_320_BGA' : {
                                                'CLK'    : { 'PORT' : 18, 'PIN' : 3, 'HSIOM_SEL' : 27 },
                                                'DATA_0' : { 'PORT' : 18, 'PIN' : 4, 'HSIOM_SEL' : 27 },
                                                'DATA_1' : { 'PORT' : 18, 'PIN' : 5, 'HSIOM_SEL' : 27 },
                                                'DATA_2' : { 'PORT' : 18, 'PIN' : 6, 'HSIOM_SEL' : 27 },
                                                'DATA_3' : { 'PORT' : 18, 'PIN' : 7, 'HSIOM_SEL' : 27 },
                                            }
                    }                        

############################## END OF USER CONFIGURATION #################################


############################## START OF DEFINITIONS ######################################
import math
import os
import sys
import tempfile
import time
import argparse
import re

# Script settings (e.g. to debug the Python script)
ENABLE_DEBUG_COMMAND_OUTPUT = False  # Set True or False

# Use pathname with slash (not backslash) as path separator
SCRIPT_FILE_WITH_PATH     = os.path.normpath(sys.argv[0]).replace("\\", "/")
SCRIPT_FILE_WITH_REL_PATH = "./" + os.path.relpath(SCRIPT_FILE_WITH_PATH).replace("\\", "/")
PATH_TO_DEBUGGING_FOLDER  = os.path.split(SCRIPT_FILE_WITH_PATH)[0] # Assumes that this script is located there

# Script argument parsing
arg_parser = argparse.ArgumentParser()
arg_parser.add_argument('EXECUTION_MODE', help='Script execution mode (e.g. FLASH, RAM, VERIFY, ERASE, RESET)')
arg_parser.add_argument('-s', '--skip_breakpoints', dest='SKIP_BREAKPOINTS', action='store_true', help='Skip setting of user breakpoints (application will run after script is processed)')
args = arg_parser.parse_args()

EXECUTION_MODE   = args.EXECUTION_MODE
SKIP_BREAKPOINTS = args.SKIP_BREAKPOINTS

# In case the script is re-used in another environment and debugger is called with '-nodisplay' command line parameter,
# there would be no 'debugger' Python object and hence it is manually created here
if not debugger or not debugger.IsAlive():
    debugger = GHS_Debugger()

#-----------------------------------------------------------------------------------------
# Register addresses and other definitions used in the script:

TARGET_DIE = "tviibh8m"

# Debug / CoreSight definitions
REG__SCS_DHCSR                     = 0xe000edf0  # Debug Halting Control and Status Register
REG__SCS_DEMCR                     = 0xe000edfc  # Debug Exception and Monitor Control Register
REG__TRC_CSTF_CSTFCTL              = 0xe008c000  # Trace Funnel Control Register of CSTF
REG__TPIU_ITCTRL                   = 0xe008ef00  # TPIU Integration Mode Control register
REG__TPIU_ITATBCTR2                = 0xe008eef0  # TPIU Integration Test ATB Control 2 Register
MASK__SCS_DHCSR_S_HALT             = 0x00020000
MASK__SCS_DEMCR_VC_CORERESET       = 0x00000001
VALUE__SCS_DHCSR__BEFORE_SYSRESET  = 0xa05f0003
VALUE__TRC_CSTF_CSTFCTL__ETM_CM7_0 = 0x00000302  # Enables funnel input for ETM trace of CM7_0 (slave port #1)
VALUE__TRC_CSTF_CSTFCTL__ETM_CM7_1 = 0x00000308  # Enables funnel input for ETM trace of CM7_1 (slave port #3)

# Cortex-M definitions
REG__NVIC_ISER               = 0xe000e100  # NVIC Interrupt Set-Enable Register
REG__NVIC_ICER               = 0xe000e180  # NVIC Interrupt Clear-Enable Register
REG__NVIC_IPR0               = 0xe000e400  # NVIC Interrupt Priority Register 0
VALUE__NVIC_ISER__IRQ0_1     = 0x00000003  # Value for enabling IRQ0 and IRQ1
VALUE__NVIC_ICER__NOT_IRQ0_1 = 0xfffffffc  # Value for disabling all IRQs but IRQ0 and IRQ1
VALUE__NVIC_IPR0__1_HIGHER_0 = 0x00000040  # Assigns higher priority to IRQ1 than IRQ0

# CPUSS / SRSS definitions
REG__SRSS_CLK_ROOT_SELECT1         = 0x40261244
VALUE__SRSS_CLK_ROOT_SELECT        = 0x80000100  # Enable clock and set DIRECT_MUX bit to select ROOT_MUX (not IMO)
REG__CPUSS_CM7_0_PWR_CTL           = 0x40201200
REG__CPUSS_CM7_1_PWR_CTL           = 0x40201210
VALUE__CPUSS_CM7_x_PWR_CTL__RESET  = 0x05fa0001  # Reset state
VALUE__CPUSS_CM7_x_PWR_CTL__ENABLE = 0x05fa0003  # Enable state
REG__CPUSS_CM7_0_CTL               = 0x4020000c
REG__CPUSS_CM7_1_CTL               = 0x4020040c
MASK__CPUSS_CM7_x_CTL_CPU_WAIT     = 0x00000010
REG__CPUSS_PROTECTION              = 0x402020c4
DICT__PROTECTION_STATE             = {  0 : "UNKNOWN",
                                        1 : "VIRGIN",
                                        2 : "NORMAL",
                                        3 : "SECURE",
                                        4 : "DEAD"
                                     }

# DAP IPC definitions
DAP_IPC_STRUCT_NUMBER            = 3
REG__DAP_IPC_STRUCT_ACQUIRE      = 0x40220000 + (DAP_IPC_STRUCT_NUMBER * 0x20) + 0x00
REG__DAP_IPC_STRUCT_NOTIFY       = 0x40220000 + (DAP_IPC_STRUCT_NUMBER * 0x20) + 0x08
REG__DAP_IPC_STRUCT_DATA0        = 0x40220000 + (DAP_IPC_STRUCT_NUMBER * 0x20) + 0x0c
MASK__IPC_STRUCT_ACQUIRE_SUCCESS = 0x80000000
VALUE__IPC_STRUCT_ACQUIRE        = 0x80000000 # SUCCESS==1
VALUE__IPC_STRUCT_NOTIFY         = 0x00000001 # Will notify IPC Interrupt Structure #0 (used for system calls!)

# System Call definitions
MASK__SYSCALL_STATUS           = 0xf0000000 # Mask for the status bits in the system call response data
VALUE__SYSCALL_STATUS_SUCCESS  = 0xa0000000 # Indicates successful system call execution

SYSCALL_CMD__SILICON_ID_TYPE_1 = 0x00000100 # System Call: Op-code=SiliconId, Parameter=ID-Type #1

MASK__LIFECYCLE_STATE          = 0x00f00000 # Mask for the bits denoting the life-cycle state in the response of SYSCALL_CMD__SILICON_ID_TYPE_1
SHIFT__LIFECYCLE_STATE         = 20         # Corresponding shift
DICT__LIFECYCLE_STATE          = {  0 : "VIRGIN",
                                    1 : "NORMAL",
                                    2 : "SECURE_WITH_DEBUG",
                                    3 : "SECURE",
                                    4 : "RMA",
                                    5 : "SORT",
                                    6 : "PROVISIONED",
                                    7 : "NORMAL_PROVISIONED",
                                    8 : "NORMAL_RMA",
                                    9 : "CORRUPTED"
                                  }

# Port and clock definitions related to trace
REG__PERI_GR0_CLOCK_CTL_0    = 0x40040c00                          # Control register for PCLK_CPUSS_CLOCK_TRACE_IN
REG__PERI_GR0_DIV8_CTL_N     = 0x40041000 + (TRACE_CLK_DIV_NR * 4) # Control register for n-th 8-bit divider in group 0
REG__PERI_GR0_DIV_CMD        = 0x40040000
VALUE__PERI_GR0_CLOCK_CTL_0  = (TRACE_CLK_DIV_NR & 0x1f)           # Selects the user chosen 8-bit divider for PCLK_CPUSS_CLOCK_TRACE_IN
VALUE__PERI_GR0_DIV8_CTL_N   = ((TRACE_CLK_DIV - 1) << 8)          # Set divider value according to user configuration
VALUE__PERI_GR0_DIV_CMD      = 0x83ff0000 | TRACE_CLK_DIV_NR       # Enable the n-th 8-bit divider (no phase alignment to other divider)
GPIO_BASE                    = 0x40310000                          # Base address of GPIO_PRT
GPIO_PORT_INCR               = 0x80                                # Address increment to get from GPIO_PRT[n] to GPIO_PRT[n+1]
GPIO_PRT_CFG_REG_OFFSET      = 0x44                                # The offset of the GPIO_PRT_CFG register within the GPIO_PRT struct
GPIO_PRT_CFG_REG_PIN_SHIFT   = 4                                   # 8 pins per port are configured in each GPIO_PRT_CFG, this value defines the bit shift
GPIO_PRT_CFG_DRIVE_MODE_MASK = 0x7                                 # Mask for the DRIVE_MODE bitfield within the GPIO_PRT_CFG register, shift will be added later depending on pin number
GPIO_DRIVE_MODE_VALUE        = 6                                   # Value for DRIVE_MODE (6 = GPIO_DM_STRONG_IN_OFF)
HSIOM_BASE                   = 0x40300000                          # Base address of HSIOM_PRT
HSIOM_PORT_INCR              = 0x10                                # Address increment to get from HSIOM_PRT[n] to HSIOM_PRT[n+1]
HSIOM_PIN_SHIFT              = 8                                   # 4 pins per port are configured in each HSIOM_PRT_PORT_SELx, this value defines the bit shift
HSIOM_PINS_PER_REG           = 4                                   # 4 pins per port are configured in each HSIOM_PRT_PORT_SELx
HSIOM_REG_INCR               = 4                                   # Address increment to get from HSIOM_PRT_PORT_SEL0 to HSIOM_PRT_PORT_SEL1
HSIOM_IOSEL_MASK             = 0x1f                                # Mask for the IO_SEL bitfield within the HSIOM_PRT_PORT_SELx register, shift will be added later depending on pin number

# Flash related definitions
REG__FLASHC_FM_CTL_ECT_MAIN_FLASH_SAFETY   = 0x4024f400
REG__FLASHC_FM_CTL_ECT_WORK_FLASH_SAFETY   = 0x4024f500

VALUE__FLASHC_FM_CTL_ECT_MAIN_FLASH_SAFETY = 0x00000001
VALUE__FLASHC_FM_CTL_ECT_WORK_FLASH_SAFETY = 0x00000001

CODE_FLASH_BASE_ADDRESS                    = 0x10000000
CODE_FLASH_SIZE_BYTES                      = 8384 * 1024

# SRAM related definitions
SRAM_BASE_ADDRESS                     = 0x28000000
SRAM_BASE_ADDRESS_FOR_GHS_FLASHKERNEL = SRAM_BASE_ADDRESS + (2 * 1024)  # avoid 2KB of non accessible SROM private RAM at start of SRAM (depending on MCU revision)

# Global variables, correct values will be determined later
CM0_INCLUDED_IN_GHSMC_FILE                     = True
CM7_0_INCLUDED_IN_GHSMC_FILE                   = True
CM7_1_INCLUDED_IN_GHSMC_FILE                   = True
CM7_0_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED = None
CM7_1_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED = None

#-----------------------------------------------------------------------------------------
# The following configuration item needs to be set depending on whether the target has a
# preliminary or dummy boot firmware that does not enter CM0+ user application by triggering
# a CM0+ "warm reset". Then the vector catch will not work and the CPU will be halted manually.
# Note: In some situations this may cause issues, if the halting happened too late
CM0_RESET_VECTOR_CATCH_IS_WORKING = None  # Value is determined by first call of IsCm0ResetVectorCatchWorking()

############################## END OF DEFINITIONS ########################################


############################## START OF FUNCTIONS ########################################

# Helper functions

# Convert number to hex string without potential "L"-suffix, optionally the width (i.e. number of digits) can be specified,
# e.g. to ensure a string with 8 hex digits (leading 0s)
def ConvertToHex(value, width=0):
    return hex(value).rstrip("L") if (width == 0) else "{0:#0{1}x}".format(value, width + 2) # width + 2 because '0x' is included in the count

# Executes a debugger command
def ExecDebugCmd(cmd_string, overwrite_print=False):
    debugger.RunCommands(cmd_string, printOutput=(ENABLE_DEBUG_COMMAND_OUTPUT or overwrite_print))

# Executes a debugger command routed via CM0+
def ExecDebugCmdViaCm0(cmd_string, overwrite_print=False):
    ExecDebugCmd("m0 " + cmd_string, overwrite_print)

# Executes a debugger command routed via CM7_0
def ExecDebugCmdViaCm7_0(cmd_string, overwrite_print=False):
    ExecDebugCmd("m7_0 " + cmd_string, overwrite_print)

# Executes a debugger command routed via CM7_1
def ExecDebugCmdViaCm7_1(cmd_string, overwrite_print=False):
    ExecDebugCmd("m7_1 " + cmd_string, overwrite_print)

# Returns the execution output string from the last executed debugger command
def GetDebugCmdOutput():
    return debugger.cmdExecOutput

# Reads memory via SYSAP, returns None on read error
def ReadViaSysAp(address, width_bytes=4):
    read_value = None
    # Uses undocumented feature to read/write using an arbitrary MEM-AP:
    # 'target t 0 mr 4 0xY12345678' --> reads 4 bytes from address 0x12345678 using AP index 'Y' of the DAP configured as core #0 in *.ghpcfg
    ExecDebugCmd("target t 0 mr " + str(width_bytes) + " " + ConvertToHex(address, 8).replace("0x", "0x0")) # SYSAP AP index is '0' ( Y -> 0)
    if GetDebugCmdOutput().lower().find("error") == -1:
        # Extracts the read value from the output message of type e.g. "Core <n>: <AP>:<address>: <value>"
        try:
            read_value = int(GetDebugCmdOutput().split(':')[-1], 0)
        except:
            ExitScriptWithError("ERROR: Could not convert read value string to integer!")
    else:
        ExitScriptWithError("ERROR: Read command not successful! Address: " + ConvertToHex(address))
    return read_value    
    
# Reads memory via the specified core, returns None on read error
def _ReadInternal(address, width_bytes, core):
    read_value = None
    ExecDebugCmd(core + " target mr " + str(width_bytes) + " " + ConvertToHex(address))
    # Check if the output message does not contain the "error" string which would indicate a read error
    if GetDebugCmdOutput().lower().find("error") == -1:
        # Extracts the read value from the output message of type e.g. "<address>: <value>"
        try:
            read_value = int(GetDebugCmdOutput().split(':')[1], 0)
        except:
            ExitScriptWithError("ERROR: Could not convert read value string to integer!")
    else:
        ExitScriptWithError("ERROR: Read command not successful! Address: " + ConvertToHex(address))
    return read_value

# Reads memory via CM0+, returns None on read error
def ReadViaCm0(address, width_bytes=4):
    return _ReadInternal(address, width_bytes, "m0")

# Reads memory via CM7_0, returns None on read error
def ReadViaCm7_0(address, width_bytes=4):
    return _ReadInternal(address, width_bytes, "m7_0")

# Reads memory via CM7_1, returns None on read error
def ReadViaCm7_1(address, width_bytes=4):
    return _ReadInternal(address, width_bytes, "m7_1")

# Writes memory via the SYSAP, returns True on success and False on error (please note: no verification of written data by read-after-write!)
# Note: Seems like GHS never returns an error when using this write method
def WriteViaSysAp(address, value, width_bytes=4):
    # Uses undocumented feature to read/write using an arbitrary MEM-AP:
    # 'target t 0 mw 4 0xY12345678 0xdeadbeef' --> reads 4 bytes 0xdeadbeef to address 0x12345678 using AP index 'Y' of the DAP configured as core #0 in *.ghpcfg
    ExecDebugCmd("target t 0 mw " + str(width_bytes) + " " + ConvertToHex(address, 8).replace("0x", "0x0") + " " + ConvertToHex(value)) # SYSAP AP index is '0' ( Y -> 0)
    # Check if the output message contains the "error" string which would indicate a write error
    return (GetDebugCmdOutput().lower().find("error") == -1)
    
# Writes memory via the specified core, returns True on success and False on error (please note: no verification of written data by read-after-write!)
def _WriteInternal(address, value, width_bytes, core):
    ExecDebugCmd(core + " target mw " + str(width_bytes) + " " + ConvertToHex(address) + " " + ConvertToHex(value))
    # Check if the output message contains the "error" string which would indicate a write error
    return (GetDebugCmdOutput().lower().find("error") == -1)

# Writes memory via CM0+, returns True on success and False on error (please note: no verification of written data by read-after-write!)
def WriteViaCm0(address, value, width_bytes=4):
    return _WriteInternal(address, value, width_bytes, "m0")

# Writes memory via CM7_0, returns True on success and False on error (please note: no verification of written data by read-after-write!)
def WriteViaCm7_0(address, value, width_bytes=4):
    return _WriteInternal(address, value, width_bytes, "m7_0")

# Writes memory via CM7_1, returns True on success and False on error (please note: no verification of written data by read-after-write!)
def WriteViaCm7_1(address, value, width_bytes=4):
    return _WriteInternal(address, value, width_bytes, "m7_1")

# Exits the script with an error return value and optionally prints a message
def ExitScriptWithError(message=""):
    print(message)
    sys.exit(1)

#-----------------------------------------------------------------------------------------
# Traveo II debug & connection functions

# Simple check whether application is linked to Flash
# Returns True if symbol address of "main" is within CODE_FLASH region, otherwise False
def IsApplicationLinkedForFlash():
    main_address = debugger.GetSymbolAddress("main")
    if (main_address >= CODE_FLASH_BASE_ADDRESS) and (main_address < (CODE_FLASH_BASE_ADDRESS + CODE_FLASH_SIZE_BYTES)):
        return True
    else:
        return False

# Returns a string indicating the current MCU Protection State
def GetProtectionStateString():
    cpuss_prot = ReadViaCm0(REG__CPUSS_PROTECTION)
    return DICT__PROTECTION_STATE[cpuss_prot]

# Returns a string indicating the current MCU Life-cycle State
def GetLifecycleStateString():
    syscall_result = ExecuteDapSystemCall(SYSCALL_CMD__SILICON_ID_TYPE_1)
    lifecycle = (syscall_result & MASK__LIFECYCLE_STATE) >> SHIFT__LIFECYCLE_STATE
    return DICT__LIFECYCLE_STATE[lifecycle]

# Returns True if the MCU Protection State is "VIRGIN", otherwise False
def IsVirginState():
    if GetProtectionStateString() == DICT__PROTECTION_STATE[1]:
        return True
    else:
        return False

# Returns the vector table base address of CM0+ (assumes that section name is ".intvec")
def GetCm0VectorTableBaseAddress():
    ExecDebugCmdViaCm0(".intvec")
    return int(GetDebugCmdOutput(), 0)

# Check which cores are included in the .ghsmc file used to start this debug session
def EvaluateAliasesInGhsmcFile():
    global CM0_INCLUDED_IN_GHSMC_FILE
    global CM7_0_INCLUDED_IN_GHSMC_FILE
    global CM7_1_INCLUDED_IN_GHSMC_FILE
    
    # If an alias is used in a route command but it has not been defined by .ghsmc file, the debugger will print following string
    # "No components matched the supplied description."
    ExecDebugCmd("m0 echo")
    if (GetDebugCmdOutput().lower().find("components") != -1):
        CM0_INCLUDED_IN_GHSMC_FILE = False
    ExecDebugCmd("m7_0 echo")
    if (GetDebugCmdOutput().lower().find("components") != -1):
        CM7_0_INCLUDED_IN_GHSMC_FILE = False
    ExecDebugCmd("m7_1 echo")
    if (GetDebugCmdOutput().lower().find("components") != -1):
        CM7_1_INCLUDED_IN_GHSMC_FILE = False
    # CM0+ alias is mandatory. Check that it was defined in .ghsmc file
    if CM0_INCLUDED_IN_GHSMC_FILE is False:
        ExitScriptWithError("ERROR: Debugger must be started by *.ghsmc file and CM0+ core (and 'cm0plus' alias) needs to be defined in *.ghsmc file!")

# Modify/Add debugger toolbar buttons 
def ModifyToolbarButtons():
    # Change the functionality of the standard reset button in the debugger toolbar
    command = "python -b -f " + SCRIPT_FILE_WITH_REL_PATH + " RESET"
    ExecDebugCmd("debugbutton Reset c=\"" + command +  "\" i=\"reset\" t=\"Traveo II: Reset\"")
     # Add separator to toolbar and then buttons to resume/halt all cores, and trigger XRES
    ExecDebugCmd("debugbutton -")
    ExecDebugCmd("debugbutton tvii_go_all   c=\"target gc\"   i=\"go_all16\" t=\"Traveo II: Resume all cores\"")
    ExecDebugCmd("debugbutton tvii_halt_all c=\"target gh\"   i=\"pause\"    t=\"Traveo II: Halt all cores\"")
    ExecDebugCmd("debugbutton tvii_xres     c=\"target tr r\" i=\"target16\" t=\"Traveo II: Trigger XRES\"")

# Connect to the GreenHills Debug Probe / Super Trace Probe (and optionally set custom clock frequency)
def ConnectToProbe():
    debugger.Disconnect(printOutput=ENABLE_DEBUG_COMMAND_OUTPUT)
    debugger.ConnectToTarget("mpserv", moreOpts=CONNECTION_TYPE + " -no_trace_registers -cfgload \"" + PATH_TO_DEBUGGING_FOLDER + "/" + TARGET_DIE +".ghpcfg\"")
    # Check whether a "target die" specific or generic environment variable exists that defines a custom JTAG/SWD frequency,
    # see 'target set clock' GHS help for the supported formats (e.g. "4.5 MHz")
    custom_clock = os.getenv('GHS_TVII_CUSTOM_DEBUG_CLOCK_' + TARGET_DIE) # TVII die-specific custom clock
    if custom_clock is None:
        custom_clock = os.getenv('GHS_TVII_CUSTOM_DEBUG_CLOCK') # TVII generic custom clock
    if custom_clock is not None:
        print("Setting custom clock: " + custom_clock)
        ExecDebugCmd("target set clock " + custom_clock)    
    # Setup trace paths here and immediately reconnect if necessary to ensure that a (potentially) changed configuration is properly reflected within MULTI Debugger
    if ConfigureCoreSightTracePaths() is True:
        print("Reconnect because GHS CoreSight trace path configuration may have changed.")
        debugger.Disconnect(printOutput=ENABLE_DEBUG_COMMAND_OUTPUT)    
        debugger.ConnectToTarget("mpserv", moreOpts=CONNECTION_TYPE + " -no_trace_registers")

# Returns probe firmware version information
def GetProbeFwVersion():
    major = None
    minor = None
    patch_build = None
    ExecDebugCmd("target info")
    version_info = GetDebugCmdOutput()
    search_result_official = re.search('v(\d).(\d).(\d)',   version_info) # e.g. Green Hills Probe v6.0.2
    search_result_beta     = re.search('v(\d).(\d)\D*(\d+)', version_info) # e.g. Green Hills Probe v5.7  build 642347
    if (search_result_official is None) and (search_result_beta is None):
        print("WARNING: Could not parse probe firmware version information!")
    else:
        search_result = search_result_official if search_result_official is not None else search_result_beta
        major       = int(search_result.group(1))
        minor       = int(search_result.group(2))
        patch_build = int(search_result.group(3))
    return major, minor, patch_build

# Checks probe firmware version and other settings (e.g. xswitches) and prints a warning if they are not as expected
def CheckProbeSettings():
    major, minor, patch_build = GetProbeFwVersion()
    if (major is not None) and (minor is not None) and (patch_build is not None):
        print("Probe firmware version: {0}.{1}.{2}".format(major, minor, patch_build))
        if (major == 5) and ((minor >= 7) or ((minor == 6) and (patch_build >= 5))):
            ExecDebugCmd("target xswitch swd.strict_ack_handling")
            xswitch_info = GetDebugCmdOutput()
            # Check if the xswitch is available at all
            if xswitch_info.find("No such XSwitch") == -1:
                if xswitch_info.startswith("+") is False:
                    print("WARNING: Setting 'target xswitch +swd.strict_ack_handling' is recommended!")
        elif (major <= 5):
            print("WARNING: Probe firmware upgrade is recommended!")

# Prepares CM0+ settings to allow successful system call execution
def PrepareSystemCallInfrastructure():
    # Ensure that system configuration is as needed for executing system call
    # Note: Unknown settings may exist if vector catch from a prior reset by script has not worked
    # and CM0+ already executed (parts of) the user application

                                                               # Ensure that:
    ExecDebugCmdViaCm0("target rw nvic_aircr 0x05fa0002")      # - all active state information for fixed and configurable exceptions is cleared
    ExecDebugCmdViaCm0("target rw vtor 0")                     # - Vector Table Base is pointing to ROM table
    ExecDebugCmdViaCm0("target rw primask 0")                  # - IRQs are not masked
    ExecDebugCmdViaCm0("target rw syst_csr 0")                 # - SysTick is disabled
    ExecDebugCmdViaCm0("target rw mpu_ctrl 0")                 # - MPU is disabled
    WriteViaCm0(REG__NVIC_ISER, VALUE__NVIC_ISER__IRQ0_1)      # - IRQ0 and IRQ1 are enabled
    WriteViaCm0(REG__NVIC_ICER, VALUE__NVIC_ICER__NOT_IRQ0_1)  # - all other IRQs are disabled
    WriteViaCm0(REG__NVIC_IPR0, VALUE__NVIC_IPR0__1_HIGHER_0)  # - IRQ1 has higher priority than IRQ0

# Executes a system call via DAP and returns the result
# If CM0+ is accessible, CM0+ state will prepared for system call, otherwise (e.g. because chip is in SECURE and
# CM0+ access port is disabled) all accesses will be limited to System AP and it is assumed that the running CM0+
# application allows DAP system calls
def ExecuteDapSystemCall(syscall_cmd_data, syscall_timeout_s=0.500, cm0_accessible=True):
    # Before the system call, ensure that CM0+ is able to process the system call
    if cm0_accessible is True:
        HaltCpuCm0()    
        PrepareSystemCallInfrastructure()
        # Now prepare an "endless loop" instruction in SRAM, set CM0+ program counter to it
        # and let it run, so that it can process the system call IRQ later on
        sram_backup = ReadViaCm0(SRAM_BASE_ADDRESS_FOR_GHS_FLASHKERNEL)
        WriteViaCm0(SRAM_BASE_ADDRESS_FOR_GHS_FLASHKERNEL, 0x0000e7fe) # instruction encoding for an endless loop
        ExecDebugCmdViaCm0("target rr pc")    
        pc_backup = int(GetDebugCmdOutput().split('=')[1], 0)
        ExecDebugCmdViaCm0("target rw pc " + ConvertToHex(SRAM_BASE_ADDRESS_FOR_GHS_FLASHKERNEL))    
        ResumeCpuCm0()
    
    # Acquire DAP IPC struct
    WriteViaSysAp(REG__DAP_IPC_STRUCT_ACQUIRE, 0) # DAP needs to write dummy value to acquire (read does not acquire!)
    lock_status = ReadViaSysAp(REG__DAP_IPC_STRUCT_ACQUIRE)
    if ((lock_status & MASK__IPC_STRUCT_ACQUIRE_SUCCESS) != VALUE__IPC_STRUCT_ACQUIRE):
        ExitScriptWithError("ERROR: IPC_STRUCT2 could not be successfully acquired!")
        
    # Set command and trigger system call
    WriteViaSysAp(REG__DAP_IPC_STRUCT_DATA0, syscall_cmd_data | 1) # Since request is made by DAP, bit 0 must be set!
    WriteViaSysAp(REG__DAP_IPC_STRUCT_NOTIFY, VALUE__IPC_STRUCT_NOTIFY)
    
    # Poll for system call completion (with optional user specified time-out)
    check_interval_s = 0.010
    while True:
        time.sleep(check_interval_s)
        syscall_timeout_s -= check_interval_s
        # CM0+ will release the DAP IPC struct at the end of system call processing,
        # so we just check whether the IPC struct is still acquired
        lock_status = ReadViaSysAp(REG__DAP_IPC_STRUCT_ACQUIRE)
        if ((lock_status & MASK__IPC_STRUCT_ACQUIRE_SUCCESS) != VALUE__IPC_STRUCT_ACQUIRE):
            break
        if(syscall_timeout_s <= 0.0):
            ExitScriptWithError("ERROR: DAP system call execution has not completed in time!")        
    
    # System call result/status is placed in IPC DATA0 register in case of DAP system call
    result = ReadViaSysAp(REG__DAP_IPC_STRUCT_DATA0)
    if (result & MASK__SYSCALL_STATUS) != VALUE__SYSCALL_STATUS_SUCCESS:
        ExitScriptWithError("ERROR: System call failed! Status = " + ConvertToHex(result))
    
    if cm0_accessible is True:
        # At the end of system call, halt CM0+ and restore original SRAM content and program counter
        HaltCpuCm0()
        ExecDebugCmdViaCm0("target rw pc " + ConvertToHex(pc_backup))
        WriteViaCm0(SRAM_BASE_ADDRESS_FOR_GHS_FLASHKERNEL, sram_backup)
        
    return result

    
# Resets the MCU via its XRES pin
def ResetMcuViaXresPin(msg="Reset target via XRES"):
    if msg is not None:
        print(msg)
    ExecDebugCmd("target tr r")

# Resets the MCU via CM0+ SCB_AIRCR_SYSRESETREQ register bit
def ResetMcuViaCm0AircrSysResetReq(msg="Reset target via CM0+ SCB_AIRCR_SYSRESETREQ"):
    if msg is not None:
        print(msg)
    # Ensure C_STEP/C_MASKINTS control bits are cleared before SYSRESET to prevent a potential problem
    WriteViaCm0(REG__SCS_DHCSR, VALUE__SCS_DHCSR__BEFORE_SYSRESET)
    ExecDebugCmdViaCm0("target rw nvic_aircr 0x05fa0004")

# Resets the MCU via CM0+ SCB_AIRCR_SYSRESETREQ register bit and ensures that CM0+ is halted (manually or using reset vector catch)
def ResetMcuViaCm0AircrSysResetReqAndHalt(msg="Reset target via CM0+ SCB_AIRCR_SYSRESETREQ and halt CM0+"):
    if IsCm0ResetVectorCatchWorking() is False:
        # If there is no Flash Boot firmware, there will be no CPU reset to transition to CM0+ application
        # hence no vector catch and we can halt the CPU manually and immediately
        ResetMcuViaCm0AircrSysResetReq(msg)
        ExecDebugCmdViaCm0("target jr")
        HaltCpuCm0()
    else:
        timeout_s = 0.250
        check_interval_s = 0.010
        ChangeResetVectorCatch("m0", True)
        ResetMcuViaCm0AircrSysResetReq(msg)
        # CM0+ now first executes ROM/Flash Boot including listen window before CM0+ "warm" reset is triggered by Flash Boot and only this
        # warm reset can be caught by the vector catch function because during the first reset, debugging is not yet enabled by boot SW.
        # Depending on flash contents (TOC2, code flash base address contents) the boot SW will not be left (e.g. bootloader is entered),
        # then the timeout will apply and the CPU is halted manually.
        while IsCpuCm0Halted() is False:
            time.sleep(check_interval_s)
            timeout_s -= check_interval_s
            if(timeout_s <= 0.0):
                print("CM0+ has not halted by reset vector catch. Halting it manually!")
                HaltCpuCm0()
                break
        ChangeResetVectorCatch("m0", False)
    # Enable CM7 access for debugger
    EnableCm7Access()

# Enables the reset vector catch for the specified core
def ChangeResetVectorCatch(core, enabled):
    # Sometimes the high-level 'target set catch_reset' state does not match the actual register bit value (SCB_DEMCR bit 0)
    # So this code ensures that both values match
    ExecDebugCmd(core + " target tset catch_reset " + ("on" if enabled else "off"))
    masked_read_val = _ReadInternal(REG__SCS_DEMCR, 4, core) & (~MASK__SCS_DEMCR_VC_CORERESET)
    _WriteInternal(REG__SCS_DEMCR, masked_read_val | (MASK__SCS_DEMCR_VC_CORERESET if enabled else 0), 4, core)
    
# Returns a FlashBoot version string or "dummy" if it is only a FlashBoot replacement
def GetFlashBootVersion():
    fb_header_1 = ReadViaCm0(0x17002004)
    fb_header_6 = ReadViaCm0(0x17002018)
    versioning_scheme = (fb_header_1 >> 28)
    major             = (fb_header_1 >> 24) & 0xf
    minor             = (fb_header_1 >> 16) & 0xff
    
    if (versioning_scheme < 2):
        return "dummy"
    else:
        patch = fb_header_6 >> 24
        build = fb_header_6 & 0xffff
        return "{}.{}.{}.{}".format(major, minor, patch, build)
    
# Returns the MCU major and minor revision
def GetMcuRev():
    major = (ReadViaCm0(0xf0000fe8) >> 4) & 0xf
    minor = (ReadViaCm0(0xf0000fec) >> 4) & 0xf
    return major, minor
    
# Prints information about the current target
def PrintTargetInformation():
    family_id            = ConvertToHex((ReadViaCm0(0xf0000fe0) & 0xff) + ((ReadViaCm0(0xf0000fe4) & 0xf) << 8))
    mcu_major, mcu_minor = GetMcuRev()
    mcu_rev              = str(mcu_major) + "." + str(mcu_minor) + " (" + chr(65 + (mcu_major - 1)) + str(mcu_minor - 1) + ")"
    silicon_id           = ConvertToHex(ReadViaCm0(0x17000000) >> 16, 4)
    flashboot_rev        = GetFlashBootVersion()
    sflash_rev           = str(ReadViaCm0(0x170000a8))
    toc2_flags           = ConvertToHex(ReadViaCm0(0x17007df8), 4)    
    protection_state     = GetProtectionStateString()
    lifecycle_state      = GetLifecycleStateString()
    print("Family ID: "        + family_id        + ", " +
          "Silicon ID: "       + silicon_id       + ", " +
          "Major.Minor Rev.: " + mcu_rev          + ", " +
          "FlashBoot Rev.: "   + flashboot_rev    + ", " +
          "SFLASH Rev.: "      + sflash_rev       + ", " +
          "TOC2_FLAGS: "       + toc2_flags
          )
    print("Protection: "       + protection_state + ", " +
          "Life-cycle: "       + lifecycle_state          
          )
    
# Returns True if the target supports the CM0+ reset vector catch (because the Flash Boot enters user application via CM0+ "warm reset", or False otherwise
def IsCm0ResetVectorCatchWorking():
    global CM0_RESET_VECTOR_CATCH_IS_WORKING
    if CM0_RESET_VECTOR_CATCH_IS_WORKING is None:
        if GetFlashBootVersion() == "dummy":
            CM0_RESET_VECTOR_CATCH_IS_WORKING = False
        else:
            CM0_RESET_VECTOR_CATCH_IS_WORKING = True
    return CM0_RESET_VECTOR_CATCH_IS_WORKING

# Halts the CM0+
def HaltCpuCm0():
    ExecDebugCmdViaCm0("target th")

# Returns 'True' if CM0+ is halted, otherwise 'False'. Uses low-level register information instead of debugger state.
def IsCpuCm0Halted():
    dhcsr_masked = ReadViaCm0(REG__SCS_DHCSR) & MASK__SCS_DHCSR_S_HALT
    return False if (dhcsr_masked == 0) else True

# Resumes the CM0+
def ResumeCpuCm0():
    ExecDebugCmdViaCm0("c")

# Sets the user configured breakpoints for CM0+ and CM7_0/CM7_1 either as SW or HW breakpoints depending on build type
def SetUserBreakpoints():
    if SKIP_BREAKPOINTS is False:
        # The breakpoint hit command will notify the debugger that the ROM-to-RAM copy for .syscall section is finished.
        # (It is assumed that user breakpoint is set at main() or later in the program flow.)
        # Debugger will install a SW breakpoint in the .syscall section which is a prerequisite for routing printf() etc.
        # to the GHS debugger "I/O" tab (the other one is "target syscalls on") 
        # In case of CM7 the D-Cache is cleaned and I-Cache is invalidated because caches might already be enabled at this point
        # in time and it shall be ensured that the SW breakpoint instruction is really fetched and not stuck in D-Cache
        bp_hit_cmd_cm0plus = " { target just_did_rom_to_ram_copy }"
        bp_hit_cmd_cm7     = " { target just_did_rom_to_ram_copy; target clop flush d * *; target clop invalidate i * * }"
        
        if IsApplicationLinkedForFlash() is False:
            bp_cmd = "b "                # Use SW breakpoints in case of RAM build configuration        
        else:
            bp_cmd = "hardbrk rolling "  # Use HW breakpoints in case of Flash build configuration        
        
        ExecDebugCmdViaCm0(bp_cmd + BREAKPOINT_LABEL_M0 + bp_hit_cmd_cm0plus)
        if CM7_0_INCLUDED_IN_GHSMC_FILE is True:
            ExecDebugCmdViaCm7_0(bp_cmd + BREAKPOINT_LABEL_M7_0 + bp_hit_cmd_cm7)
        if CM7_1_INCLUDED_IN_GHSMC_FILE is True:
            ExecDebugCmdViaCm7_1(bp_cmd + BREAKPOINT_LABEL_M7_1 + bp_hit_cmd_cm7)

# The ROM boot/Flash boot code does not branch to RAM, so for RAM builds we need to patch some registers
# Note: Expects that CM0+ application has been downloaded already
def PatchProgramEntryForRamBuild():
    cm0_vector_table_base = GetCm0VectorTableBaseAddress()
    print("Patching PC and MSP because of RAM build (CM0+ vector table base: " + ConvertToHex(cm0_vector_table_base) + ")")
    ExecDebugCmdViaCm0("$MSP = *((unsigned long*)" + ConvertToHex(cm0_vector_table_base + 0) + ")")
    ExecDebugCmdViaCm0("$PC  = *((unsigned long*)" + ConvertToHex(cm0_vector_table_base + 4) + ")")
 
# Ensure that CM0+ is halted at the configured user breakpoint after the controlled reset.
def EnsureTargetIsStoppedAtUserBreakpoint():
    halted_due_to_breakpoint = False
    ExecDebugCmdViaCm0("target ti")
    # Outputs messages like
    # - "Core 1: little-endian ARM Cortex-M0 hardware breakpoint hit at 0x1000015e (bp 0)"
    # - "Core 1: little-endian ARM Cortex-M0 halted at 0x10000194"
    if (GetDebugCmdOutput().find("breakpoint") != -1):
        halted_due_to_breakpoint = True
    
    # In case of a RAM build, PC would have been patched to user reset handler earlier in this function, 
    # so we need to resume execution to hit the breakpoint
    # In case of a Flash build, we have to resume
    # - if the CM0+ reset vector catch is working because then we would have stopped at CM0+ reset handler,
    # - if the halting of CPU by the script happened earlier than hitting the user breakpoint (which may depend on listen window, debugger clock, etc.)
    if ( (IsApplicationLinkedForFlash()  is False)  or
         (IsCm0ResetVectorCatchWorking() is True)   or
         (halted_due_to_breakpoint       is False)      ):
        ResumeCpuCm0()
    
# Checks whether a TPIU capable probe is connected (GHS Super Trace Probe or Probe V4 with the right target adapter)
def IsTpiuTraceProbe():
    ExecDebugCmdViaCm0("target support")
    if GetDebugCmdOutput().find("*** Trace Status ***") != -1:
        ExecDebugCmdViaCm0("target set adapter")
        adapter_string = GetDebugCmdOutput()
        if ((adapter_string.find("TEPOD:ARM-CS20")   != -1) or
            (adapter_string.find("TEPOD:ETM-MICTOR") != -1)      ):
            return True
    return False
        
# Configures the GHS CoreSight trace paths and returns True if a reconnection may be necessary, otherwise False
def ConfigureCoreSightTracePaths():
    # Check for availability of the "coresight_path" command
    ExecDebugCmd("target coresight_path")
    # If not supported, output should be "Command 'coresight_path' failed."
    if GetDebugCmdOutput().find("failed") != -1:
        return False        
    # Clear all CoreSight trace paths by default
    ExecDebugCmd("target coresight_path del *")        
    if ((CM7_0_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED is False) and 
        (CM7_1_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED is False)):
        return False        
    # Setup all necessary paths as per the user configuration
    if CM7_0_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED:
        ExecDebugCmd("target coresight_path add 2 [CM7_0] -> 6.1 [CSTF: ETM of CM7_0] -> " + ("7 [ETB]" if USE_TPIU_OFFCHIP_TRACE is False else "8 [TPIU]"))
    if CM7_1_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED:
        ExecDebugCmd("target coresight_path add 3 [CM7_1] -> 6.3 [CSTF: ETM of CM7_1] -> " + ("7 [ETB]" if USE_TPIU_OFFCHIP_TRACE is False else "8 [TPIU]"))
    return True
    
# Configures trace related settings
def SetupTrace():
    # GHS does not support the Micro Trace Buffer (MTB) of CM0+, so there is only something to do if at least
    # one CM7 shall be debugged  and tracing for the respective CM7 is enabled
    if ((CM7_0_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED is False) and 
        (CM7_1_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED is False)):
        return
        
    # TPIU trace is only possible if the current probe is capable for that
    if USE_TPIU_OFFCHIP_TRACE is True and IsTpiuTraceProbe() is False:
        print("WARNING: Skipping TPIU trace configuration because the current probe does not seem to support it!")
        return
        
    # --------------------------------------- All checks passed -------------------------------------------------------

    print("Setting up trace (also check GHS trace options!). This may take a while...")
            
    # Disable trace and abort trace retrieval during the configuration
    if CM7_0_INCLUDED_IN_GHSMC_FILE is True:
        ExecDebugCmdViaCm7_0("trace disable; trace abort")
    if CM7_1_INCLUDED_IN_GHSMC_FILE is True:
        ExecDebugCmdViaCm7_1("trace disable; trace abort")
        
    # Indicate to GHS that trace related registers are accessible now
    ExecDebugCmd("target trace_registers on")

    # Check whether TPIU trace shall be configured
    if USE_TPIU_OFFCHIP_TRACE is True:
        # Loop through all port pins that need to be configured for trace
        for signal_name, signal_cfg in TRACE_PIN_CONFIG[CPU_BOARD_TYPE].items():
            # Set drive mode                    
            prt_cfg_addr = GPIO_BASE + (signal_cfg['PORT'] * GPIO_PORT_INCR) + GPIO_PRT_CFG_REG_OFFSET
            shift = signal_cfg['PIN'] * GPIO_PRT_CFG_REG_PIN_SHIFT
            old_val = ReadViaCm0(prt_cfg_addr)
            prt_cfg_val = (old_val & (~(GPIO_PRT_CFG_DRIVE_MODE_MASK << shift))) | (GPIO_DRIVE_MODE_VALUE << shift)
            WriteViaCm0(prt_cfg_addr, prt_cfg_val)
            # Set I/O mux in HSIOM
            hsiom_cfg_addr = HSIOM_BASE + (signal_cfg['PORT'] * HSIOM_PORT_INCR)
            if signal_cfg['PIN'] >= HSIOM_PINS_PER_REG:
                hsiom_cfg_addr = hsiom_cfg_addr + HSIOM_REG_INCR
                shift = (signal_cfg['PIN'] - HSIOM_PINS_PER_REG) * HSIOM_PIN_SHIFT
            else:
                shift = signal_cfg['PIN'] * HSIOM_PIN_SHIFT                        
            old_val = ReadViaCm0(hsiom_cfg_addr)
            hsiom_cfg_val = (old_val & (~(HSIOM_IOSEL_MASK << shift))) | (signal_cfg['HSIOM_SEL'] << shift)
            WriteViaCm0(hsiom_cfg_addr, hsiom_cfg_val)
        
        # Enable CPUSS_CLOCK_TRACE_IN peripheral clock with user selected divider
        WriteViaCm0(REG__PERI_GR0_DIV8_CTL_N,  VALUE__PERI_GR0_DIV8_CTL_N)
        WriteViaCm0(REG__PERI_GR0_CLOCK_CTL_0, VALUE__PERI_GR0_CLOCK_CTL_0)
        WriteViaCm0(REG__PERI_GR0_DIV_CMD,     VALUE__PERI_GR0_DIV_CMD)

    
    # Note: Some CoreSight components are only accessible through CM7_0 core. The usual Exec/Read/Write functions are not working if CM7_0 is
    #       not included in .ghsmc file (cm0plus_with_cm7_1.ghsmc). Therefore, at some places in the code below, the more generic low level
    #       'ExecDebugCmd("target t 2 ... ")' command is used, that routes the command through core index 2 as specified in .ghpcfg file (i.e. CM7_0)

    # In case of ETB trace...
    if USE_TPIU_OFFCHIP_TRACE is False:
        # Enable TPIU integration test mode and set ATREADYS output signal of TPIU so that the ATB replicator is not stalled by the unused TPIU
        ExecDebugCmd("target t 2 mw 4 " + ConvertToHex(REG__TPIU_ITCTRL)    + " 0x00000001") # Enable Integration Mode    # Only accessible by CM7_0
        ExecDebugCmd("target t 2 mw 4 " + ConvertToHex(REG__TPIU_ITATBCTR2) + " 0x00000001") # Set ATREADYS               # Only accessible by CM7_0
        
    # Enable CoreSight Trace Funnel slave ports for the ETMs of the trace enabled cores, ITM not supported by GHS(?)
    cstf_ctl_value = (VALUE__TRC_CSTF_CSTFCTL__ETM_CM7_0 if CM7_0_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED is True else 0) | \
                     (VALUE__TRC_CSTF_CSTFCTL__ETM_CM7_1 if CM7_1_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED is True else 0)        
    ExecDebugCmd("target t 2 mw 4 " + ConvertToHex(REG__TRC_CSTF_CSTFCTL) + " " + ConvertToHex(cstf_ctl_value)) # Only accessible by CM7_0
    
    # (Re-)enable trace
    if CM7_0_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED is True:
        ExecDebugCmdViaCm7_0("trace enable")
    if CM7_1_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED is True:
        ExecDebugCmdViaCm7_1("trace enable")

# Check whether Flash writing/erasing was successful and print a potential solution otherwise
def CheckFlashSuccess(cmd_output):
    # Check if GHS reported that it could not detect the flash device which in the vast majority of cases indicates an outdated
    # flash_chips_fcr4tcm.odb file. Another possibility is missing TVII flash support in general which requires a patch...
    if cmd_output.find("Could not detect a flash device at") != -1:
        ExecDebugCmd("$_TOOLS_DIR")
        odb_file = GetDebugCmdOutput().split("= ")[1].replace("\\\\", "/").strip('"') + "/defaults/flash/flash_chips_fcr4tcm.odb"
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        print("GHS flasher could not detect the MCU. Typically, this is solved by updating this file:")
        print(odb_file)
        print("Latest ODB file is available e.g. in Traveo II Sample Driver Library (SDL) package:")
        print("<SDL_Root>/misc/tools/ghs/ghs_comp_xxxxxx_defaults_flash/flash_chips_fcr4tcm.odb")
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        ExitScriptWithError("Stopping further script processing")

# Enable Flash writing
def EnableFlashWriting():
    WriteViaCm0(REG__FLASHC_FM_CTL_ECT_MAIN_FLASH_SAFETY, VALUE__FLASHC_FM_CTL_ECT_MAIN_FLASH_SAFETY)
    WriteViaCm0(REG__FLASHC_FM_CTL_ECT_WORK_FLASH_SAFETY, VALUE__FLASHC_FM_CTL_ECT_WORK_FLASH_SAFETY)

# Common steps needed for any "flash burn" operation
def ResetAndPrepareFlashOperation():
    ResetMcuViaCm0AircrSysResetReqAndHalt()
    EnableFlashWriting()    
    # System calls are needed by the GHS flash kernel, that will run from RAM and 
    # trigger SROM API calls for ID retrieval, flash programming and erasing.
    PrepareSystemCallInfrastructure()

# Erase code flash from given offset and with given size (default is whole code flash)
def EraseCodeFlash(offset=0, size=CODE_FLASH_SIZE_BYTES):
    # Creating a temporary file of given size because GHS flash tool also needs a file for erasing
    with tempfile.NamedTemporaryFile(delete = False) as temp_file_for_erase:
        try:
            temp_file_for_erase.seek(size - 1)
            temp_file_for_erase.write(b"\0")
            temp_file_for_erase.close()

            ResetAndPrepareFlashOperation()
            print("Erasing CodeFlash from offset " + ConvertToHex(offset) + " (" + str(offset/1024) + " KB)" + " with size " + ConvertToHex(size) + " (" + str(size/1024) + " KB). This may take a while...")
            # GHS flash kernel must be run on CM0+ at the moment
            ExecDebugCmdViaCm0("flash burn -baseaddress="  + ConvertToHex(CODE_FLASH_BASE_ADDRESS) +
                                         " -rambase="      + ConvertToHex(SRAM_BASE_ADDRESS_FOR_GHS_FLASHKERNEL) +
                                         " -offset="       + ConvertToHex(offset) +
                                         " -memrequire=64" +
                                         " -type=raw"      +
                                         " -executable="   + temp_file_for_erase.name +
                                         " -eraseonly=yes", True)
            CheckFlashSuccess(GetDebugCmdOutput())
        finally:
            os.remove(temp_file_for_erase.name)

# Program internal code flash
def ProgramCodeFlash():
    if PERMIT_SFLASH_USER_ROW_PROGRAMMING:
        sflash_permission_string = "&write_sflash=1"
    else:
        sflash_permission_string = ""

    if CM0_INCLUDED_IN_GHSMC_FILE is True:
        ResetAndPrepareFlashOperation()
        ExecDebugCmdViaCm0("map -modules") # Will print full path to CM0+ .elf file
        ELF_PATH = "\"" + GetDebugCmdOutput() + "\""
        print("Programming CM0+ application image")
        # GHS flash kernel must be run on CM0+ at the moment
        ExecDebugCmdViaCm0("flash burn -baseaddress=" + ConvertToHex(CODE_FLASH_BASE_ADDRESS) + sflash_permission_string +
                                     " -rambase="     + ConvertToHex(SRAM_BASE_ADDRESS_FOR_GHS_FLASHKERNEL) +
                                     " -memrequire=64" +
                                     " -type=elf" +
                                     " -executable=" + ELF_PATH +
                                     " -verify=yes", True)
        CheckFlashSuccess(GetDebugCmdOutput())

    if CM7_0_INCLUDED_IN_GHSMC_FILE is True:
        ResetAndPrepareFlashOperation()
        ExecDebugCmdViaCm7_0("map -modules") # Will print full path to CM7_0 .elf file
        ELF_PATH = "\"" + GetDebugCmdOutput() + "\""
        print("Programming CM7_0 application image")
        # GHS flash kernel must be run on CM0+ at the moment
        ExecDebugCmdViaCm0("flash burn -baseaddress=" + ConvertToHex(CODE_FLASH_BASE_ADDRESS) + sflash_permission_string +
                                     " -rambase="     + ConvertToHex(SRAM_BASE_ADDRESS_FOR_GHS_FLASHKERNEL) +
                                     " -memrequire=64" +
                                     " -type=elf" +
                                     " -executable=" + ELF_PATH +
                                     " -verify=yes", True)
        CheckFlashSuccess(GetDebugCmdOutput())

    if CM7_1_INCLUDED_IN_GHSMC_FILE is True:
        ResetAndPrepareFlashOperation()
        ExecDebugCmdViaCm7_1("map -modules") # Will print full path to CM7_1 .elf file
        ELF_PATH = "\"" + GetDebugCmdOutput() + "\""
        print("Programming CM7_1 application image")
        # GHS flash kernel must be run on CM0+ at the moment
        ExecDebugCmdViaCm0("flash burn -baseaddress=" + ConvertToHex(CODE_FLASH_BASE_ADDRESS) + sflash_permission_string +
                                     " -rambase="     + ConvertToHex(SRAM_BASE_ADDRESS_FOR_GHS_FLASHKERNEL) +
                                     " -memrequire=64" +
                                     " -type=elf" +
                                     " -executable=" + ELF_PATH +
                                     " -verify=yes", True)
        CheckFlashSuccess(GetDebugCmdOutput())

# Verifies if application is already present in target memory
def VerifyApplication():
    verify_cmd = "verify -all"
    if VERIFY_SPARSELY_ONLY is True:
        verify_cmd = verify_cmd + " -sparse"
    ExecDebugCmdViaCm0(verify_cmd, True)
    if CM7_0_INCLUDED_IN_GHSMC_FILE is True:
        ExecDebugCmdViaCm7_0(verify_cmd, True)
    if CM7_1_INCLUDED_IN_GHSMC_FILE is True:
        ExecDebugCmdViaCm7_1(verify_cmd, True)

# Executes a reset which is build specific (patching of program entry / initial stack for RAM builds)
def DoReset():
    ResetMcuViaCm0AircrSysResetReqAndHalt()
    if IsApplicationLinkedForFlash() is False:
        PatchProgramEntryForRamBuild()
    if CM7_0_INCLUDED_IN_GHSMC_FILE is True:
        pass # ChangeResetVectorCatch("m7_0", False)
    if CM7_1_INCLUDED_IN_GHSMC_FILE is True:
        pass # ChangeResetVectorCatch("m7_1", False)
    SetupTrace()
    EnsureTargetIsStoppedAtUserBreakpoint()

def EnableCm7Access():
    WriteViaCm0(REG__SRSS_CLK_ROOT_SELECT1, VALUE__SRSS_CLK_ROOT_SELECT) # enable CLK_HF1 (CM7_0, CM7_1)
    
    # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    # DebugPowerUpDown API call may need to be added here on later version / silicon!
    # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        
    WriteViaCm0(REG__CPUSS_CM7_0_CTL, ReadViaCm0(REG__CPUSS_CM7_0_CTL) | 0x00800a00) # Set init values for "DTCM Enable and RMW", enable DTCM AHB Slave Port
    WriteViaCm0(REG__CPUSS_CM7_1_CTL, ReadViaCm0(REG__CPUSS_CM7_1_CTL) | 0x00800a00) # Set init values for "DTCM Enable and RMW", enable DTCM AHB Slave Port
    
    WriteViaCm0(REG__CPUSS_CM7_0_PWR_CTL, VALUE__CPUSS_CM7_x_PWR_CTL__ENABLE)
    WriteViaCm0(REG__CPUSS_CM7_1_PWR_CTL, VALUE__CPUSS_CM7_x_PWR_CTL__ENABLE)
    time.sleep(0.001) # wait some time, so that CM7 Power FSM can reach new state ("ENABLED")
    WriteViaCm0(REG__CPUSS_CM7_0_PWR_CTL, VALUE__CPUSS_CM7_x_PWR_CTL__RESET)
    WriteViaCm0(REG__CPUSS_CM7_1_PWR_CTL, VALUE__CPUSS_CM7_x_PWR_CTL__RESET)
    time.sleep(0.001) # wait some time, so that CM7 Power FSM can reach new state ("DBG_RSTOFF" -> CM7 is in a special state like in M4 based TVII, full return to "RESET" is prevented because of CSYSPWRUPREQ)
        
    WriteViaCm0(REG__CPUSS_CM7_0_CTL, ReadViaCm0(REG__CPUSS_CM7_0_CTL) & (~(MASK__CPUSS_CM7_x_CTL_CPU_WAIT))) # Clear CPU_WAIT, let CM7 execute the endless "WFI" loop in SROM
    WriteViaCm0(REG__CPUSS_CM7_1_CTL, ReadViaCm0(REG__CPUSS_CM7_1_CTL) & (~(MASK__CPUSS_CM7_x_CTL_CPU_WAIT))) # Clear CPU_WAIT, let CM7 execute the endless "WFI" loop in SROM

    # "reset_detection" method in *.ghpcfg file is set to "manual" for CM7_x -> indicate to the debugger that the CPUs are available now
    ExecDebugCmdViaCm7_0("target rw ghs_usable 1")
    ExecDebugCmdViaCm7_1("target rw ghs_usable 1")

############################## END OF FUNCTIONS ##########################################


############################## START OF SCRIPT ###########################################


print("-----------------------------")
print("Script execution mode: " + EXECUTION_MODE)

# Create even shorter alias names for routing commands to one of the cores only
ExecDebugCmd("alias m0 route cm0plus")
ExecDebugCmd("alias m7_0 route cm7_0")
ExecDebugCmd("alias m7_1 route cm7_1")

if   (EXECUTION_MODE == "RAM")   and (IsApplicationLinkedForFlash() is True ):
    ExitScriptWithError("ERROR: Application seems to be linked to Flash, but a RAM build operation was requested")
elif (EXECUTION_MODE == "FLASH") and (IsApplicationLinkedForFlash() is False):
    ExitScriptWithError("ERROR: Application seems to be linked to RAM, but a Flash build operation was requested")
    
# Check that a *.ghsmc file was used to start the debug session and which alias names are defined
EvaluateAliasesInGhsmcFile()

# Now that EvaluateAliasesInGhsmcFile() has been called, determine the correct values for these variables
CM7_0_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED = True if ((CM7_0_INCLUDED_IN_GHSMC_FILE is True) and (TRACE_CM7_0 is True)) else False
CM7_1_INCLUDED_IN_GHSMC_FILE_AND_TRACE_ENABLED = True if ((CM7_1_INCLUDED_IN_GHSMC_FILE is True) and (TRACE_CM7_1 is True)) else False

# Make CM0+ the active/selected component to have a defined initial state
ExecDebugCmd("switch -component cm0plus")

#-----------------------------------------------------------------------------------------
# Common operations for multiple execution modes
if EXECUTION_MODE in ["RAM", "FLASH", "VERIFY", "ERASE"]:
    ModifyToolbarButtons()
    ConnectToProbe()
    CheckProbeSettings()
    # Make logical association of the executables with the cores in the target (otherwise routing of "target ..." commands does not work)
    ExecDebugCmd("prepare_target -nosave -verify=none")
    ResetMcuViaXresPin(None)
    ExecDebugCmdViaCm0("target jr")
    HaltCpuCm0()
    ResetMcuViaCm0AircrSysResetReqAndHalt()
    PrintTargetInformation()

#-----------------------------------------------------------------------------------------
if EXECUTION_MODE == "RAM":
    if IsCm0ResetVectorCatchWorking() is False:
        # If an application is already present in Code Flash, the dummy/preliminary boot firmware
        # may have decided to enter CM0+ user code depending on content at CODE_FLASH_BASE_ADDRESS
        # This may cause issues with RAM build configurations. To ensure a clean reset is possible
        # the first 32KB sector will be erased here.
        code_flash_base_content = ReadViaCm0(CODE_FLASH_BASE_ADDRESS)
        if (code_flash_base_content != 0) and (code_flash_base_content != 0xffffffff):
            print("Code Flash base address is non-empty -> Prevent issues with RAM build configuration by erasing it first")
            EraseCodeFlash(0, 32*1024)
    ResetMcuViaCm0AircrSysResetReqAndHalt()
    print("Downloading application to RAM")
    ExecDebugCmd("prepare_target -nosave -load")
    SetUserBreakpoints()
    DoReset()

#-----------------------------------------------------------------------------------------
if EXECUTION_MODE == "FLASH":
    ProgramCodeFlash()
    ResetMcuViaCm0AircrSysResetReqAndHalt() # needed to get user breakpoints on CM7 effective
    SetUserBreakpoints()
    DoReset()

#-----------------------------------------------------------------------------------------
if EXECUTION_MODE == "VERIFY":
    ResetMcuViaCm0AircrSysResetReqAndHalt()
    VerifyApplication()
    SetUserBreakpoints()
    DoReset()

#-----------------------------------------------------------------------------------------
if EXECUTION_MODE == "ERASE":
    if IS_PSVP is True:
        # only 2MB of non-contiguous flash memory in PSVP
        EraseCodeFlash(0, 1024*1024)
        EraseCodeFlash(0x00400000, 1024*1024)
    else:
        EraseCodeFlash() 
    ResetMcuViaCm0AircrSysResetReqAndHalt()

#-----------------------------------------------------------------------------------------
if EXECUTION_MODE == "RESET":
    DoReset()


############################## END OF SCRIPT #############################################
