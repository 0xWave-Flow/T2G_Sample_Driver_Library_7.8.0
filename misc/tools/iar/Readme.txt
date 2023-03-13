******************************************************************************************************************************************************
This section applies for IAR flashloader patch.

1. Copy files of .\misc\tools\iar\IAR_EWARM_8xxx_FlashLoader_Patch_TraveoII.7z to IAR install folder
   (Do not copy anything at folder level)

   Install folder (dst):
    For example, 
	v8.42.1: C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.4\arm
	v8.22.2: C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.0\arm

   Install files (src):
    IAR_EWARM_8xxx_FlashLoader_Patch_TraveoII.7z

	src: \IAR_EWARM_8xxx_FlashLoader_Patch_TraveoII\config\debugger\Infineon
    dst: \config\debugger\Infineon
    
    src: \IAR_EWARM_8xxx_FlashLoader_Patch_TraveoII\config\devices\Infineon\Traveo-II
    dst: \config\devices\Infineon\Traveo-II

    src: \IAR_EWARM_8xxx_FlashLoader_Patch_TraveoII\config\flashloader\Infineon
    dst: \config\flashloader\Infineon

    src: \IAR_EWARM_8xxx_FlashLoader_Patch_TraveoII\config\linker\Infineon
    dst: \config\linker\Infineon

2. Re-start IAR EWARM

3. Rebuild All

4. Download & Debug

******************************************************************************************************************************************************
The above steps need to be repeated for IAR EWARM 8.22.2 using IAR_EWARM_8222_FlashLoader_Patch_TraveoII.7z and the respective IAR installer.
******************************************************************************************************************************************************

Note:
1. IAR workspaces have been updated with New device names (Infineon, instead of Cypress)
2. Unzip IAR patch, you will find Infineon directory, copy into that instead of to the Cypress folder, delete the \arm\config\devices\Cypress\Traveo-II folder.
3. Re-start IAR IDE if is already open.
