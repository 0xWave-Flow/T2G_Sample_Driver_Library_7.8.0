import csv
import os
from datetime import datetime

# File name paths
TVII_CHIP_LIST_CSV_FILE_WITH_REL_PATH = "tvii_chip_list.csv"
ODB_TEMPLATE_FILE_WITH_REL_PATH       = "flash_chips_fcr4tcm.odb.template"
ODB_OUTPUT_FILE_WITH_REL_PATH         = "..\\flash_chips_fcr4tcm.odb"


# Enables/Disables the output of some debug messages (e.g. the calculated flash addresses/gaps)
PRINT_DEBUG_MESSAGES = False  # Set True or False

# Chip definitions will be added at this line in the ODB template file
ODB_FILE_TEMPLATE_INSERTION_MARKER = "<<<SCRIPT_ADDS_TRAVEO_II_ENTRIES_HERE>>>"

# Each device will be visually separated in the ODB file by using following string
ODB_FILE_ENTRY_SEPARATOR           = 120 * "~"

# A template for a TVII single flash macro device entry, with marker strings for the variable content
ENTRY_TEMPLATE_SINGLE_FLASH_MACRO = """\
  { cpu_name="<<<DESCRIPTION>>>";
    tvii_famid=<<<FAMILY_ID>>>; tvii_silid=<<<SILICON_ID>>>; dual_bank=false;
    geometry {
      { count=<<<CF0_SECTOR_NUMBER_LARGE>>>; size="32 KB" }
      { count=<<<CF0_SECTOR_NUMBER_SMALL>>>; size="8 KB" }
      { count=1; size="<<<GAP_CF0_END_TO_SFLASH_START_KB>>> KB"; valid=false }
      { count=64; size="512" }
      { count=1; size="8160 KB"; valid=false }
      { count=64; size="512" }
    }
  }
  { cpu_name="<<<DESCRIPTION>>> | DualBank";
    tvii_famid=<<<FAMILY_ID>>>; tvii_silid=<<<SILICON_ID>>>; dual_bank=true;
    geometry {
      { count=<<<CF0_SECTOR_NUMBER_LARGE_DB>>>; size="32 KB" }
      { count=<<<CF0_SECTOR_NUMBER_SMALL_DB>>>; size="8 KB" }
      { count=1; size="<<<GAP_CF0_LOWER_BANK_END_TO_UPPER_BANK_START_KB>>> KB"; valid=false }
      { count=<<<CF0_SECTOR_NUMBER_LARGE_DB>>>; size="32 KB" }
      { count=<<<CF0_SECTOR_NUMBER_SMALL_DB>>>; size="8 KB" }
      { count=1; size="<<<GAP_CF0_UPPER_BANK_END_TO_SFLASH_START_KB>>> KB"; valid=false }
      { count=64; size="512" }
      { count=1; size="8160 KB"; valid=false }
      { count=64; size="512" }
    }
  }
  { cpu_name="<<<DESCRIPTION>>> | WorkFlash";
    tvii_famid=<<<FAMILY_ID>>>; tvii_silid=<<<SILICON_ID>>>; wflash=true; row_bytes=4; dual_bank=false;
    geometry {
      { count=<<<WF0_SECTOR_NUMBER_LARGE>>>; size="2 KB" }
      { count=<<<WF0_SECTOR_NUMBER_SMALL>>>; size="128" }
    }
  }
  { cpu_name="<<<DESCRIPTION>>> | WorkFlash | DualBank";
    tvii_famid=<<<FAMILY_ID>>>; tvii_silid=<<<SILICON_ID>>>; wflash=true; row_bytes=4; dual_bank=true;
    geometry {
      { count=<<<WF0_SECTOR_NUMBER_LARGE_DB>>>; size="2 KB" }
      { count=<<<WF0_SECTOR_NUMBER_SMALL_DB>>>; size="128" }
      { count=1; size="<<<GAP_WF0_LOWER_BANK_END_TO_UPPER_BANK_START_KB>>> KB"; valid=false }
      { count=<<<WF0_SECTOR_NUMBER_LARGE_DB>>>; size="2 KB" }
      { count=<<<WF0_SECTOR_NUMBER_SMALL_DB>>>; size="128" }
    }
  }
"""

# A template for a TVII dual flash macro device entry, with marker strings for the variable content
ENTRY_TEMPLATE_DUAL_FLASH_MACRO = """\
  { cpu_name="<<<DESCRIPTION>>>";
    tvii_famid=<<<FAMILY_ID>>>; tvii_silid=<<<SILICON_ID>>>; dual_bank=false;
    geometry {
      { count=<<<CF0_SECTOR_NUMBER_LARGE>>>; size="32 KB" }
      { count=<<<CF0_SECTOR_NUMBER_SMALL>>>; size="8 KB" }
      { count=1; size="<<<GAP_CF0_END_TO_SFLASH_START_KB>>> KB"; valid=false }
      { count=64; size="512" }
      { count=1; size="8160 KB"; valid=false }
      { count=64; size="512" }
      { count=1; size="8160 KB"; valid=false }
      { count=<<<CF1_SECTOR_NUMBER_LARGE>>>; size="32 KB" }
      { count=<<<CF1_SECTOR_NUMBER_SMALL>>>; size="8 KB" }
      { count=1; size="<<<GAP_CF1_END_TO_CF1_EXT_START_KB>>> KB"; valid=false }
      { count=1; size="32 KB" }
      { count=1; size="8160 KB"; valid=false }
      { count=1; size="32 KB" }
      
    }
  }
  { cpu_name="<<<DESCRIPTION>>> | DualBank";
    tvii_famid=<<<FAMILY_ID>>>; tvii_silid=<<<SILICON_ID>>>; dual_bank=true;
    geometry {
      { count=<<<CF0_SECTOR_NUMBER_LARGE_DB>>>; size="32 KB" }
      { count=<<<CF0_SECTOR_NUMBER_SMALL_DB>>>; size="8 KB" }
      { count=1; size="<<<GAP_CF0_LOWER_BANK_END_TO_UPPER_BANK_START_KB>>> KB"; valid=false }
      { count=<<<CF0_SECTOR_NUMBER_LARGE_DB>>>; size="32 KB" }
      { count=<<<CF0_SECTOR_NUMBER_SMALL_DB>>>; size="8 KB" }
      { count=1; size="<<<GAP_CF0_UPPER_BANK_END_TO_SFLASH_START_KB>>> KB"; valid=false }
      { count=64; size="512" }
      { count=1; size="8160 KB"; valid=false }
      { count=64; size="512" }
      { count=1; size="8160 KB"; valid=false }
      { count=<<<CF1_SECTOR_NUMBER_LARGE_DB>>>; size="32 KB" }
      { count=<<<CF1_SECTOR_NUMBER_SMALL_DB>>>; size="8 KB" }
      { count=1; size="<<<GAP_CF1_LOWER_BANK_END_TO_UPPER_BANK_START_KB>>> KB"; valid=false }
      { count=<<<CF1_SECTOR_NUMBER_LARGE_DB>>>; size="32 KB" }
      { count=<<<CF1_SECTOR_NUMBER_SMALL_DB>>>; size="8 KB" }
      { count=1; size="<<<GAP_CF1_UPPER_BANK_END_TO_CF1_EXT_START_KB>>> KB"; valid=false }
      { count=1; size="32 KB" }
      { count=1; size="8160 KB"; valid=false }
      { count=1; size="32 KB" }
    }
  }
  { cpu_name="<<<DESCRIPTION>>> | WorkFlash";
    tvii_famid=<<<FAMILY_ID>>>; tvii_silid=<<<SILICON_ID>>>; wflash=true; row_bytes=4; dual_bank=false;
    geometry {
      { count=<<<WF0_SECTOR_NUMBER_LARGE>>>; size="2 KB" }
      { count=<<<WF0_SECTOR_NUMBER_SMALL>>>; size="128" }
      { count=1; size="130816 KB"; valid=false }
      { count=<<<WF1_SECTOR_NUMBER_LARGE>>>; size="2 KB" }
      { count=<<<WF1_SECTOR_NUMBER_SMALL>>>; size="128" }
    }
  }
  { cpu_name="<<<DESCRIPTION>>> | WorkFlash | DualBank";
    tvii_famid=<<<FAMILY_ID>>>; tvii_silid=<<<SILICON_ID>>>; wflash=true; row_bytes=4; dual_bank=true;
    geometry {
      { count=<<<WF0_SECTOR_NUMBER_LARGE_DB>>>; size="2 KB" }
      { count=<<<WF0_SECTOR_NUMBER_SMALL_DB>>>; size="128" }
      { count=1; size="<<<GAP_WF0_LOWER_BANK_END_TO_UPPER_BANK_START_KB>>> KB"; valid=false }
      { count=<<<WF0_SECTOR_NUMBER_LARGE_DB>>>; size="2 KB" }
      { count=<<<WF0_SECTOR_NUMBER_SMALL_DB>>>; size="128" }
      { count=1; size="<<<GAP_WF0_UPPER_BANK_END_TO_WF1_LOWER_BANK_START_KB>>> KB"; valid=false }
      { count=<<<WF1_SECTOR_NUMBER_LARGE_DB>>>; size="2 KB" }
      { count=<<<WF1_SECTOR_NUMBER_SMALL_DB>>>; size="128" }
      { count=1; size="<<<GAP_WF1_LOWER_BANK_END_TO_UPPER_BANK_START_KB>>> KB"; valid=false }
      { count=<<<WF1_SECTOR_NUMBER_LARGE_DB>>>; size="2 KB" }
      { count=<<<WF1_SECTOR_NUMBER_SMALL_DB>>>; size="128" }
    }
  }
"""

# TVII flash constants
CODEFLASH0_BASE              = 0x10000000
CODEFLASH0_LOWER_BANK_BASE   = CODEFLASH0_BASE
CODEFLASH0_UPPER_BANK_BASE   = 0x12000000
WORKFLASH0_BASE              = 0x14000000
WORKFLASH0_LOWER_BANK_BASE   = WORKFLASH0_BASE
WORKFLASH0_UPPER_BANK_BASE   = 0x15000000
SFLASH_BASE                  = 0x17000000
CODEFLASH1_BASE              = 0x18000000
CODEFLASH1_LOWER_BANK_BASE   = CODEFLASH1_BASE
CODEFLASH1_UPPER_BANK_BASE   = 0x1a000000
WORKFLASH1_BASE              = 0x1c000000
WORKFLASH1_LOWER_BANK_BASE   = WORKFLASH1_BASE
WORKFLASH1_UPPER_BANK_BASE   = 0x1d000000
CODEFLASH1_EXT_BASE          = 0x1f000000
CODEFLASH_SECTOR_SIZE_LARGE  = 32 * 1024
CODEFLASH_SECTOR_SIZE_SMALL  =  8 * 1024
WORKFLASH_SECTOR_SIZE_LARGE  =  2 * 1024
WORKFLASH_SECTOR_SIZE_SMALL  = 128


def print_debug(message):
    """Print message if enabled, add a prefix string to indicate that it is a debug message."""
    if PRINT_DEBUG_MESSAGES is True:
        print("DEBUG> " + message)
    

def get_gap_cf0_end_to_sflash_start(secnum_large, secnum_small):
    """Calculate the single bank mode gap in KB from the end of CodeFlash #0 to the start of the SFLASH."""
    cf0_end = CODEFLASH0_BASE + (CODEFLASH_SECTOR_SIZE_LARGE * secnum_large) + (CODEFLASH_SECTOR_SIZE_SMALL * secnum_small)
    gap_in_kb = (SFLASH_BASE - cf0_end) // 1024
    print_debug("CODEFLASH0_END:                                              {0:#010x}".format(cf0_end))
    print_debug("CODEFLASH0_END to SFLASH_START Gap:                          {0:#010x}".format(gap_in_kb * 1024))
    return gap_in_kb    
    
def get_gap_cf1_end_to_cf1_ext_start(secnum_large, secnum_small):
    """Calculate the single bank mode gap in KB from the end of CodeFlash #1 to the start of the CodeFlash #1 extended region."""
    cf1_end = CODEFLASH1_BASE + (CODEFLASH_SECTOR_SIZE_LARGE * secnum_large) + (CODEFLASH_SECTOR_SIZE_SMALL * secnum_small)
    gap_in_kb = (CODEFLASH1_EXT_BASE - cf1_end) // 1024
    print_debug("CODEFLASH1_END:                                              {0:#010x}".format(cf1_end))
    print_debug("CODEFLASH1_END to CODEFLASH1_EXT_START Gap:                  {0:#010x}".format(gap_in_kb * 1024))
    return gap_in_kb

    

def get_gap_cf0_lower_bank_end_to_upper_bank_start(secnum_large, secnum_small):
    """Calculate the dual bank mode gap in KB from the end of the lower CodeFlash #0 bank to the start of the upper CodeFlash #0 bank."""
    cf0_lower_bank_end = CODEFLASH0_LOWER_BANK_BASE + (CODEFLASH_SECTOR_SIZE_LARGE * secnum_large // 2) + (CODEFLASH_SECTOR_SIZE_SMALL * secnum_small // 2)
    gap_in_kb = (CODEFLASH0_UPPER_BANK_BASE - cf0_lower_bank_end) // 1024
    print_debug("CODEFLASH0_LOWER_BANK_END:                                    {0:#010x}".format(cf0_lower_bank_end))
    print_debug("CODEFLASH0_LOWER_BANK_END to CODEFLASH0_UPPER_BANK_START Gap: {0:#010x}".format(gap_in_kb * 1024))
    return gap_in_kb
    
def get_gap_cf1_lower_bank_end_to_upper_bank_start(secnum_large, secnum_small):
    """Calculate the dual bank mode gap in KB from the end of the lower CodeFlash #1 bank to the start of the upper CodeFlash #1 bank."""
    cf1_lower_bank_end = CODEFLASH1_LOWER_BANK_BASE + (CODEFLASH_SECTOR_SIZE_LARGE * secnum_large // 2) + (CODEFLASH_SECTOR_SIZE_SMALL * secnum_small // 2)
    gap_in_kb = (CODEFLASH1_UPPER_BANK_BASE - cf1_lower_bank_end) // 1024
    print_debug("CODEFLASH1_LOWER_BANK_END:                                    {0:#010x}".format(cf1_lower_bank_end))
    print_debug("CODEFLASH1_LOWER_BANK_END to CODEFLASH1_UPPER_BANK_START Gap: {0:#010x}".format(gap_in_kb * 1024))
    return gap_in_kb
    


def get_gap_cf0_upper_bank_end_to_sflash_start(secnum_large, secnum_small):
    """Calculate the dual bank mode gap in KB from the end of the upper CodeFlash #0 bank to the start of the SFLASH."""
    cf0_upper_bank_end = CODEFLASH0_UPPER_BANK_BASE + (CODEFLASH_SECTOR_SIZE_LARGE * secnum_large // 2) + (CODEFLASH_SECTOR_SIZE_SMALL * secnum_small // 2)
    gap_in_kb = (SFLASH_BASE - cf0_upper_bank_end) // 1024
    print_debug("CODEFLASH0_UPPER_BANK_END:                                   {0:#010x}".format(cf0_upper_bank_end))
    print_debug("CODEFLASH0_UPPER_BANK_END to SFLASH_START Gap:               {0:#010x}".format(gap_in_kb * 1024))
    return gap_in_kb

def get_gap_cf1_upper_bank_end_to_cf1_ext_start(secnum_large, secnum_small):
    """Calculate the dual bank mode gap in KB from the end of the upper CodeFlash #1 bank to the start of the CodeFlash #1 extended region."""
    cf1_upper_bank_end = CODEFLASH1_UPPER_BANK_BASE + (CODEFLASH_SECTOR_SIZE_LARGE * secnum_large // 2) + (CODEFLASH_SECTOR_SIZE_SMALL * secnum_small // 2)
    gap_in_kb = (CODEFLASH1_EXT_BASE - cf1_upper_bank_end) // 1024
    print_debug("CODEFLASH1_UPPER_BANK_END:                                   {0:#010x}".format(cf1_upper_bank_end))
    print_debug("CODEFLASH1_UPPER_BANK_END to CODEFLASH1_EXT_START Gap:       {0:#010x}".format(gap_in_kb * 1024))
    return gap_in_kb



def get_gap_wf0_lower_bank_end_to_upper_bank_start(secnum_large, secnum_small):
    """Calculate the dual bank mode gap in KB from the end of the lower WorkFlash #0 bank to the start of the upper WorkFlash #0 bank."""
    wf0_lower_bank_end = WORKFLASH0_LOWER_BANK_BASE + (WORKFLASH_SECTOR_SIZE_LARGE * secnum_large // 2) + (WORKFLASH_SECTOR_SIZE_SMALL * secnum_small // 2)
    gap_in_kb = (WORKFLASH0_UPPER_BANK_BASE - wf0_lower_bank_end) // 1024
    print_debug("WORKFLASH0_LOWER_BANK_END:                                    {0:#010x}".format(wf0_lower_bank_end))
    print_debug("WORKFLASH0_LOWER_BANK_END to WORKFLASH0_UPPER_BANK_START Gap: {0:#010x}".format(gap_in_kb * 1024))
    return gap_in_kb

def get_gap_wf1_lower_bank_end_to_upper_bank_start(secnum_large, secnum_small):
    """Calculate the dual bank mode gap in KB from the end of the lower WorkFlash #1 bank to the start of the upper WorkFlash #1 bank."""
    wf1_lower_bank_end = WORKFLASH1_LOWER_BANK_BASE + (WORKFLASH_SECTOR_SIZE_LARGE * secnum_large // 2) + (WORKFLASH_SECTOR_SIZE_SMALL * secnum_small // 2)
    gap_in_kb = (WORKFLASH1_UPPER_BANK_BASE - wf1_lower_bank_end) // 1024
    print_debug("WORKFLASH1_LOWER_BANK_END:                                    {0:#010x}".format(wf1_lower_bank_end))
    print_debug("WORKFLASH1_LOWER_BANK_END to WORKFLASH1_UPPER_BANK_START Gap: {0:#010x}".format(gap_in_kb * 1024))
    return gap_in_kb



def get_gap_wf0_upper_bank_end_to_wf1_lower_bank_start(secnum_large, secnum_small):
    """Calculate the dual bank mode gap in KB from the end of the upper WorkFlash #0 bank to the start of the lower WorkFlash #1 bank."""
    wf0_upper_bank_end = WORKFLASH0_UPPER_BANK_BASE + (WORKFLASH_SECTOR_SIZE_LARGE * secnum_large // 2) + (WORKFLASH_SECTOR_SIZE_SMALL * secnum_small // 2)
    gap_in_kb = (WORKFLASH1_LOWER_BANK_BASE - wf0_upper_bank_end) // 1024
    print_debug("WORKFLASH0_UPPER_BANK_END:                                    {0:#010x}".format(wf0_upper_bank_end))
    print_debug("WORKFLASH0_UPPER_BANK_END to WORKFLASH1_LOWER_BANK_START Gap: {0:#010x}".format(gap_in_kb * 1024))
    return gap_in_kb




def add_traveo_ii_entries(odb_file):
    """Generate the proper chip definitions for all the entries in the CSV file and add it to the ODB file."""
    with open(TVII_CHIP_LIST_CSV_FILE_WITH_REL_PATH, "r", newline='') as chip_list_file:
        # Define dict keys for the CSV columns and create DictReader object
        keys = ['desc', 'family_id', 'silicon_id', 'cf0_secnum_large', 'cf0_secnum_small', 'wf0_secnum_large', 'wf0_secnum_small', 'cf1_secnum_large', 'cf1_secnum_small', 'wf1_secnum_large', 'wf1_secnum_small']        
        chip_list_reader = csv.DictReader(chip_list_file, fieldnames=keys)
        
        # Go through each line of the CSV file
        for row in chip_list_reader:
            # Should not happen for DictReader
            if not row:
                continue
            # CSV file comment, just skip
            elif row['desc'].lstrip().startswith("//") is True:
                continue
            # Comment line for the ODB file, so just write it to the ODB file, prefixed with the proper comment character
            elif row['desc'].lstrip().startswith("#") is True:
                odb_file.write(row['desc'] + "\n")
                continue
            # Generate an empty line in the ODB
            elif row['desc'].lstrip().startswith("@") is True:
                odb_file.write("\n")
                continue
            
            # Use this optional column to decide whether the current entry contains definitions for a dual flash macro device (e.g. TVII-B-H-16M)
            if row['cf1_secnum_large'] is None:
                is_dual_flash_macro = False
                CURRENT_ENTRY = ENTRY_TEMPLATE_SINGLE_FLASH_MACRO
            else:
                is_dual_flash_macro = True
                CURRENT_ENTRY = ENTRY_TEMPLATE_DUAL_FLASH_MACRO

            # Output the currently processed entry on the console
            print(100 * "=")
            print("Processing: " + row['desc'])            
            
            # Calculate entry dependent flash related parameters
            cf0_secnum_large = int(row['cf0_secnum_large'].strip(), 0)
            cf0_secnum_small = int(row['cf0_secnum_small'].strip(), 0)
            wf0_secnum_large = int(row['wf0_secnum_large'].strip(), 0)
            wf0_secnum_small = int(row['wf0_secnum_small'].strip(), 0)            
            gap_cf0_end_to_sflash_start                = get_gap_cf0_end_to_sflash_start               (cf0_secnum_large, cf0_secnum_small)
            gap_cf0_lower_bank_end_to_upper_bank_start = get_gap_cf0_lower_bank_end_to_upper_bank_start(cf0_secnum_large, cf0_secnum_small)
            gap_cf0_upper_bank_end_to_sflash_start     = get_gap_cf0_upper_bank_end_to_sflash_start    (cf0_secnum_large, cf0_secnum_small)
            gap_wf0_lower_bank_end_to_upper_bank_start = get_gap_wf0_lower_bank_end_to_upper_bank_start(wf0_secnum_large, wf0_secnum_small)
            if is_dual_flash_macro:
                cf1_secnum_large = int(row['cf1_secnum_large'].strip(), 0)
                cf1_secnum_small = int(row['cf1_secnum_small'].strip(), 0)
                wf1_secnum_large = int(row['wf1_secnum_large'].strip(), 0)
                wf1_secnum_small = int(row['wf1_secnum_small'].strip(), 0)            
                gap_cf1_end_to_cf1_ext_start                   = get_gap_cf1_end_to_cf1_ext_start                  (cf1_secnum_large, cf1_secnum_small)
                gap_cf1_lower_bank_end_to_upper_bank_start     = get_gap_cf1_lower_bank_end_to_upper_bank_start    (cf1_secnum_large, cf1_secnum_small)
                gap_cf1_upper_bank_end_to_cf1_ext_start        = get_gap_cf1_upper_bank_end_to_cf1_ext_start       (cf1_secnum_large, cf1_secnum_small)
                gap_wf0_upper_bank_end_to_wf1_lower_bank_start = get_gap_wf0_upper_bank_end_to_wf1_lower_bank_start(wf0_secnum_large, wf0_secnum_small)
                gap_wf1_lower_bank_end_to_upper_bank_start     = get_gap_wf1_lower_bank_end_to_upper_bank_start    (wf1_secnum_large, wf1_secnum_small)
                
            
            # Update the template string with entry specific data
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<DESCRIPTION>>>",                                   row['desc'])
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<FAMILY_ID>>>",                                     row['family_id'].strip())
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<SILICON_ID>>>",                                    row['silicon_id'].strip())
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<CF0_SECTOR_NUMBER_LARGE>>>",                       str(cf0_secnum_large     ))
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<CF0_SECTOR_NUMBER_SMALL>>>",                       str(cf0_secnum_small     ))
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<CF0_SECTOR_NUMBER_LARGE_DB>>>",                    str(cf0_secnum_large // 2))
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<CF0_SECTOR_NUMBER_SMALL_DB>>>",                    str(cf0_secnum_small // 2))
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<WF0_SECTOR_NUMBER_LARGE>>>",                       str(wf0_secnum_large     ))
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<WF0_SECTOR_NUMBER_SMALL>>>",                       str(wf0_secnum_small     ))
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<WF0_SECTOR_NUMBER_LARGE_DB>>>",                    str(wf0_secnum_large // 2))
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<WF0_SECTOR_NUMBER_SMALL_DB>>>",                    str(wf0_secnum_small // 2))
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<GAP_CF0_END_TO_SFLASH_START_KB>>>",                str(gap_cf0_end_to_sflash_start)               )
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<GAP_CF0_LOWER_BANK_END_TO_UPPER_BANK_START_KB>>>", str(gap_cf0_lower_bank_end_to_upper_bank_start))
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<GAP_CF0_UPPER_BANK_END_TO_SFLASH_START_KB>>>",     str(gap_cf0_upper_bank_end_to_sflash_start)    )
            CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<GAP_WF0_LOWER_BANK_END_TO_UPPER_BANK_START_KB>>>", str(gap_wf0_lower_bank_end_to_upper_bank_start))
            if is_dual_flash_macro:
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<CF1_SECTOR_NUMBER_LARGE>>>",                           str(cf1_secnum_large     ))
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<CF1_SECTOR_NUMBER_SMALL>>>",                           str(cf1_secnum_small     ))
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<CF1_SECTOR_NUMBER_LARGE_DB>>>",                        str(cf1_secnum_large // 2))
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<CF1_SECTOR_NUMBER_SMALL_DB>>>",                        str(cf1_secnum_small // 2))
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<WF1_SECTOR_NUMBER_LARGE>>>",                           str(wf1_secnum_large     ))
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<WF1_SECTOR_NUMBER_SMALL>>>",                           str(wf1_secnum_small     ))
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<WF1_SECTOR_NUMBER_LARGE_DB>>>",                        str(wf1_secnum_large // 2))
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<WF1_SECTOR_NUMBER_SMALL_DB>>>",                        str(wf1_secnum_small // 2))
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<GAP_CF1_END_TO_CF1_EXT_START_KB>>>",                   str(gap_cf1_end_to_cf1_ext_start)                  )
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<GAP_CF1_LOWER_BANK_END_TO_UPPER_BANK_START_KB>>>",     str(gap_cf1_lower_bank_end_to_upper_bank_start)    )
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<GAP_CF1_UPPER_BANK_END_TO_CF1_EXT_START_KB>>>",        str(gap_cf1_upper_bank_end_to_cf1_ext_start)       )
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<GAP_WF0_UPPER_BANK_END_TO_WF1_LOWER_BANK_START_KB>>>", str(gap_wf0_upper_bank_end_to_wf1_lower_bank_start))
                CURRENT_ENTRY = CURRENT_ENTRY.replace("<<<GAP_WF1_LOWER_BANK_END_TO_UPPER_BANK_START_KB>>>",     str(gap_wf1_lower_bank_end_to_upper_bank_start)    )

            # Write entry to ODB file and add visual separator
            odb_file.write(CURRENT_ENTRY + "\n")
            odb_file.write("#" + ODB_FILE_ENTRY_SEPARATOR + "\n\n")


print("\nGenerating GreenHills ODB file, based on template file and CSV file with chip definitions:\n")

# Open template file and output file
with     open(ODB_TEMPLATE_FILE_WITH_REL_PATH, "r") as template_file:
    with open(ODB_OUTPUT_FILE_WITH_REL_PATH,   "w") as odb_file:
        
        # Go through each line of the template file and search for the insertion marker
        for line in template_file:
            # Just copy the line from the template to the output if it is not the insertion marker line
            if line.find(ODB_FILE_TEMPLATE_INSERTION_MARKER) == -1:
                odb_file.write(line)
            # Insertion marker line found. Replace it with the generated entries from the CSV file
            else:
                odb_file.write(120 * "#" + "\n")
                odb_file.write("# Traveo II entries generated by Cypress ODB file generator script on " + datetime.now().ctime() + "\n")
                odb_file.write(120 * "#" + "\n\n")
                
                add_traveo_ii_entries(odb_file)
                
                odb_file.write(120 * "#" + "\n")
                odb_file.write("# End of generated content.\n")
                odb_file.write(120 * "#" + "\n\n")
    
print("\n\nDone.")


