**It is recommended to open and read this document with a proper Markdown viewer!**

# Preface

This document describes the CMake based build environment for Traveo T2G Sample Driver Library (SDL).

Feature Overview:
- Different toolchains supported
  - Arm GNU (GCC)
  - GreenHills (GHS)
  - IAR Systems (IAR)
  - Windriver DIAB
- Standard build configurations available
  - Debug
  - Release
  - Release with Debug
  - (Min Size)
- Integration of MCAL possible
- Usage options
  - Commandline based
  - Visual Studio Code (with additional extensions)
- Debugging support:
  - GHS & Probe
  - IAR & iJet Probe
  - Lauterbach Trace32 & Probe
  - Visual Studio Code (with additional extensions) & Infineon MiniProg4/KitProg or SEGGER J-Link
- Utility support/integration (e.g. flash programming, Tresos GUI start)

# Changelog

## Traveo T2G SDL CMake build environment

| T2G SDL Release | Author | Change description                                                                                                                                             |
| --------------- | ------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 7.5.0           | CHEY   | Initial release                                                                                                                                                |
| 7.6.0           | CHEY   | Add Lauterbach/IAR debugger targets, MCAL integration support, ENV var support for tool config, Ethernet stack integration, SREC generation                    |
| 7.7.0           | CHEY   | Improved/Fixed MCAL support (fix cmdline length limit issue, add some default defines)                                                                         |
| 7.8.0           | CHEY   | Add GCC toolchain and VS Code OpenOCD based debug support, IAR debug templates update (v9.30.1), add CYTVII-B-E-1M-SK to presets, add tviibh16m (preliminary)  |

## CMake_Readme.md

| T2G SDL Release | Author | Change description                                                                                                                               |
| --------------- | ------ | ------------------------------------------------------------------------------------------------------------------------------------------------ |
| 7.5.0           | CHEY   | Initial release                                                                                                                                  |
| 7.6.0           | CHEY   | Major updates (Advanced Usage, MCAL, ...)                                                                                                        |
| 7.7.0           | CHEY   | none                                                                                                                                             |
| 7.8.0           | CHEY   | Updates related to GCC, VS Code debugging and tviibh16m (quad CM7) support, some restructuring and minor edits                                   |

# Introduction

## What is CMake?

CMake is a cross-platform, open-source build system generator or meta build system. It produces files for a native build system like *Make*, *Ninja*, *MS Visual Studio* and [many more](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html). The CMake language is used to describe the build process of a SW, its artifacts and build targets in a portable way. It is used by many well known and popular SW projects.

CMake offers many more features (e.g. SW testing and deployment) than what is planned to be used in the scope of Traveo T2G SDL. For further information please refer to the [CMake website](https://cmake.org/) or check the *References* chapter.

When searching the web you will often come across the term *modern* CMake which refers to a major conceptual change in how targets and their sources, properties, interfaces and dependencies are modeled and which is the recommended way going forward. Likewise, you will often find examples/solutions that are not following the modern CMake approach and you should be aware of that and try to avoid it when customizing the provided CMake solution.

## CMake basic workflow

When using CMake to build a SW project, the whole process is split into two phases:

### Configuration phase

During this phase the user settings, command line parameters, etc. are evaluated and the files for the native build system are generated (e.g. *Makefiles* if *Make* has been chosen as the generator).

CMake also creates the so called CMake cache file, where some of the settings will be preserved across subsequent configurations. The cache file can also be edited manually or using GUI tools (like `cmake-gui.exe` as part of the CMake delivery) to change build settings after the initial configuration.

### Build phase

During this phase the SW will actually be built by the native build system. CMake will be used as a wrapper and will handle the calling of the native build system (e.g. calling *Make* with the appropriate *Makefile* and parameters).

A CMake project can also describe/have custom build targets that can be "built" (i.e. executed) in this phase. This can be used to e.g. start the debugger for the created executable or call some other external tool like a flash programmer and so on.

## CMake files

The main CMake file is called `CMakeLists.txt` and is located in the root of the Traveo T2G SDL folder. This file can then include further files, which may be helpful for modularizing the description of the build.

Recent CMake versions also support so called preset files:

- `CMakePresets.json` (supposed to be checked into version control)
- `CMakeUserPresets.json` (not supposed to be checked into version control, for developer's/user's own purpose)

These preset files are an easy way to specify and share the most common configuration and build scenarios for the given project and hence simplify the amount of command line parameters needed for calling CMake. Of course the usage of the defined presets is purely optional.

# Prerequisites

## CMake

Download CMake from the [CMake website](https://cmake.org/download/) and either extract or install it (depending on the variant you have downloaded).

The first line in `CMakeLists.txt` states which CMake version will be at least needed and this is checked during the CMake configuration phase. At the time of writing the expected version is CMake 3.23.0 or later, but it may be updated in future releases of the SDL to enable the use of new CMake features.

After installation/extraction please ensure that the folder containing `cmake.exe` is available in your *PATH* environment variable.

## Ninja build system

The default native build system (or CMake generator) for the CMake support in Traveo T2G SDL will be the *Ninja* build system. It is preferred over *Make* because of its performance/speed.

Visit the Ninja [website](https://ninja-build.org/), you should find a link to GitHub where the releases will be available for download.

After extracting Ninja at the location of your choice please ensure that the folder containing `ninja.exe` is available in your *PATH* environment variable.

## Tool configuration (paths, ...)

System/User specific tool (compiler/debugger/etc.) paths and corresponding settings are configured as CMake cache variables in this file:

`./cmake/tool_config.cmake`

Please ensure that they match your environment. If they do not match there are two options on how to specify the correct value:

- Edit the value in the file directly
- Set up an environment variable and assign it the right value. The environment variable needs to have the same name like the CMake variable but prefixed with `SDL_CMAKE_`  (e.g. `GHS_COMPILER_ROOT_DIR` --> `SDL_CMAKE_GHS_COMPILER_ROOT_DIR`). The script checks whether such environment variables exist and if they do, they will take precedence over the values specified in the file

# Basic usage / Quick guide

This section will focus on the use of the presets defined in `CMakePresets.json`. See the *Advanced Usage* section for a list and description of required cache variables to be passed via the command line if presets are not used.

**All commands are supposed to be executed in a command shell in the root directory of T2G SDL.**

## Configuration

### Available presets

A list of the available configuration presets can be obtained by the following command:

`cmake --list-presets`

The output should look as follows:

```
C:\T2G_Sample_Driver_Library>cmake --list-presets
Available configure presets:

  "tviibe1m-gcc"     - T2G-B-E-1M (GCC)
  "tviibe1m-sk-gcc"  - CYTVII-B-E-1M-SK (GCC)
  "tviibe2m-gcc"     - T2G-B-E-2M (GCC)
  "tviibe4m-gcc"     - T2G-B-E-4M (GCC)
  "tviibe512k-gcc"   - T2G-B-E-512K (GCC)
  "tviibh4m-gcc"     - T2G-B-H-4M (GCC)
  "tviibh8m-gcc"     - T2G-B-H-8M (GCC)
  "tviibh16m-gcc"    - T2G-B-H-16M (GCC)
  "tviic2d4m-gcc"    - T2G-C-2D-4M (GCC)
  "tviic2d6m-gcc"    - T2G-C-2D-6M (GCC)
  "tviic2d6mddr-gcc" - T2G-C-2D-6M-DDR (GCC)
  "tviice4m-gcc"     - T2G-C-E-4M (GCC)
  "tviibe1m-ghs"     - T2G-B-E-1M (GHS)
  "tviibe1m-sk-ghs"  - CYTVII-B-E-1M-SK (GHS)
  "tviibe2m-ghs"     - T2G-B-E-2M (GHS)
  "tviibe4m-ghs"     - T2G-B-E-4M (GHS)
  "tviibe512k-ghs"   - T2G-B-E-512K (GHS)
  "tviibh4m-ghs"     - T2G-B-H-4M (GHS)
  "tviibh8m-ghs"     - T2G-B-H-8M (GHS)
  "tviibh16m-ghs"    - T2G-B-H-16M (GHS)
  "tviic2d4m-ghs"    - T2G-C-2D-4M (GHS)
  "tviic2d6m-ghs"    - T2G-C-2D-6M (GHS)
  "tviic2d6mddr-ghs" - T2G-C-2D-6M-DDR (GHS)
  "tviice4m-ghs"     - T2G-C-E-4M (GHS)
  "tviibe1m-iar"     - T2G-B-E-1M (IAR)
  "tviibe1m-sk-iar"  - CYTVII-B-E-1M-SK (IAR)
  "tviibe2m-iar"     - T2G-B-E-2M (IAR)
  "tviibe4m-iar"     - T2G-B-E-4M (IAR)
  "tviibe512k-iar"   - T2G-B-E-512K (IAR)
  "tviibh4m-iar"     - T2G-B-H-4M (IAR)
  "tviibh8m-iar"     - T2G-B-H-8M (IAR)
  "tviibh16m-iar"    - T2G-B-H-16M (IAR)
  "tviic2d4m-iar"    - T2G-C-2D-4M (IAR)
  "tviic2d6m-iar"    - T2G-C-2D-6M (IAR)
  "tviic2d6mddr-iar" - T2G-C-2D-6M-DDR (IAR)
  "tviice4m-iar"     - T2G-C-E-4M (IAR)
  "tviibe1m-diab"    - T2G-B-E-1M (DIAB)
  "tviibe1m-sk-diab" - CYTVII-B-E-1M-SK (DIAB)
```

The only choices available are

- Toolchain 
- T2G derivate  
  *Note:* There are special "derivates" available that target special boards, e.g. `tviibe1m-sk-<toolchain>` supports the T2G-B-E-1M 100-pin starter kit (sk) with the part number [*CYTVII-B-E-1M-SK*](https://www.infineon.com/cms/en/product/evaluation-boards/cytvii-b-e-1m-sk/)


Fixed options (as defined by the presets) are:

- Latest MCU revision is targeted
- Application will be linked to *Flash* (not *SRAM*)
- *Ninja Multi-Config* will be used as build-system generator
- The build output directory will be `./build/<cfg-preset-name>`
- Some T2G derivate specific options (e.g. CPU board / board revision)
- PSVP build for devices without silicon yet

### Configuring the project

The CMake configure step is initiated with the following command (*tviibe1m-ghs* configure preset will be used as an example)

`cmake --preset tviibe1m-ghs`

This should result in the following output:

```
C:\T2G_Sample_Driver_Library>cmake --preset tviibe1m-ghs
Preset CMake variables:

  BOARD_REV="CPU_BOARD_REVC"
  CMAKE_TOOLCHAIN_FILE:FILEPATH="C:/T2G_Sample_Driver_Library/cmake/toolchains/toolchain-ghs.cmake"
  DIE="tviibe1m"
  LINK="flash"
  MCU_REV="latest"

-- Latest MCU revision detected: D
-- The C compiler identification is GHS 2017.1.4
-- The ASM compiler identification is unknown
-- Found assembler: C:/ghs/comp_201714/ccthumb.exe
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/ghs/comp_201714/ccthumb.exe - skipped
-- Warning: Did not find file Compiler/-ASM
-- ============================================================
-- T2G SDL - CMake Configuration
-- ------------------------------------------------------------
-- Toolchain         = ghs
-- DIE               = tviibe1m
-- MCU revision      = D
-- Device            = CYT2B78CAE
-- Link location     = flash
-- PSVP              = OFF
-- Board             =
-- Board revision    = CPU_BOARD_REVC
-- Use RTOS          = ON
-- Use ETH Stack     = OFF
-- Use MCAL          = OFF
-- ============================================================
-- Creating library: drivers_cm0plus
-- Creating library: mw_cm0plus
-- Creating library: drivers_cm4
-- Creating library: mw_cm4
-- Creating library: rtos_cm4
-- Creating executable: cm0plus
-- Creating executable: cm4
-- Creating GHS debugger targets:
--   dbg_ghs_cm0plus
--   dbg_ghs_cm0plus_cm4
-- Creating IAR debugger targets:
--   dbg_iar_cm0plus
--   dbg_iar_cm0plus_cm4
-- Creating Lauterbach T32 debugger targets:
--   dbg_t32_cm0plus
--   dbg_t32_cm0plus_cm4
-- Creating Visual Studio Code debugger launch configuration
--   Writing file C:/T2G_Sample_Driver_Library/.vscode/launch.json
-- Creating Auto Flash Utility / OpenOCD targets:
--   util_afu_oocd_prog_cm0plus
--   util_afu_oocd_prog_cm4
--   util_afu_oocd_prog_all
--   util_afu_oocd_erase_all
-- Configuring done
-- Generating done
-- Build files have been written to: C:/T2G_Sample_Driver_Library/build/tviibe1m-ghs
```

*Note: Warnings during the tool detection process can typically be ignored (as long as the actual build later on works fine).*  


## Building

### Build configurations

The project supports multiple build configurations. Since during configuration stage we had selected a so called *multi-config* capable generator (i.e. Ninja Multi-Config), the choice of the actual build configuration will be postponed to the build stage. (For single configuration generators, one needs to select the build configuration already during configuration stage).

The available build configurations for the Traveo T2G SDL match the official ones that are already part of CMake by default. (Of course the configurations could be modified, extended, reduced, etc...)

| Build Configuration Name | Description                                                                                                                                                              |
| ------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| Debug                    | Project is built without optimizations (or only those that do not affect debuggability). Debug information/symbols is/are of course included in the generated artifacts. |
| Release                  | Project is built with speed optimizations. Chosen options match the ones used by Infineon commercial SW products for Traveo T2G.                                         |
| RelWithDebInfo           | Same as *Release* but artifacts include debug information/sysmbols. Debuggability may be negatively affected though (e.g. re-ordered or removed code).                   |

Note: There is a 4th build configuration called *MinSizeRel* which is a release configuration with size optimizations and the toolchain options are prepared for it, but it is not enabled by CMake by default. 

The build artifacts for the different build configurations of a preset will end up in an identically named subfolder of the build output folder (`./build/<cfg-preset-name>`)

### Available presets

A list of the available configuration presets can be obtained by the following command:

`cmake --build --list-presets`

The output should look as follows:

```
C:\T2G_Sample_Driver_Library>cmake --build --list-presets
Available build presets:

  "tviibe1m-gcc"                    - T2G-B-E-1M (GCC), 'Debug' Cfg
  "tviibe1m-sk-gcc"                 - CYTVII-B-E-1M-SK (GCC), 'Debug' Cfg
  "tviibe2m-gcc"                    - T2G-B-E-2M (GCC), 'Debug' Cfg
  "tviibe4m-gcc"                    - T2G-B-E-4M (GCC), 'Debug' Cfg
  "tviibe512k-gcc"                  - T2G-B-E-512K (GCC), 'Debug' Cfg
  "tviibh4m-gcc"                    - T2G-B-H-4M (GCC), 'Debug' Cfg
  "tviibh8m-gcc"                    - T2G-B-H-8M (GCC), 'Debug' Cfg
  "tviibh16m-gcc"                   - T2G-B-H-16M (GCC), 'Debug' Cfg
  "tviic2d4m-gcc"                   - T2G-C-2D-4M (GCC), 'Debug' Cfg
  "tviic2d6m-gcc"                   - T2G-C-2D-6M (GCC), 'Debug' Cfg
  "tviic2d6mddr-gcc"                - T2G-C-2D-6M-DDR (GCC), 'Debug' Cfg
  "tviice4m-gcc"                    - T2G-C-E-4M (GCC), 'Debug' Cfg
  "tviibe1m-ghs"                    - T2G-B-E-1M (GHS), 'Debug' Cfg
  "tviibe1m-sk-ghs"                 - CYTVII-B-E-1M-SK (GHS), 'Debug' Cfg
  "tviibe2m-ghs"                    - T2G-B-E-2M (GHS), 'Debug' Cfg
  "tviibe4m-ghs"                    - T2G-B-E-4M (GHS), 'Debug' Cfg
  "tviibe512k-ghs"                  - T2G-B-E-512K (GHS), 'Debug' Cfg
  "tviibh4m-ghs"                    - T2G-B-H-4M (GHS), 'Debug' Cfg
  "tviibh8m-ghs"                    - T2G-B-H-8M (GHS), 'Debug' Cfg
  "tviibh16m-ghs"                   - T2G-B-H-16M (GHS), 'Debug' Cfg
  "tviic2d4m-ghs"                   - T2G-C-2D-4M (GHS), 'Debug' Cfg
  "tviic2d6m-ghs"                   - T2G-C-2D-6M (GHS), 'Debug' Cfg
  "tviic2d6mddr-ghs"                - T2G-C-2D-6M-DDR (GHS), 'Debug' Cfg
  "tviice4m-ghs"                    - T2G-C-E-4M (GHS), 'Debug' Cfg
  "tviibe1m-iar"                    - T2G-B-E-1M (IAR), 'Debug' Cfg
  "tviibe1m-sk-iar"                 - CYTVII-B-E-1M-SK (IAR), 'Debug' Cfg
  "tviibe2m-iar"                    - T2G-B-E-2M (IAR), 'Debug' Cfg
  "tviibe4m-iar"                    - T2G-B-E-4M (IAR), 'Debug' Cfg
  "tviibe512k-iar"                  - T2G-B-E-512K (IAR), 'Debug' Cfg
  "tviibh4m-iar"                    - T2G-B-H-4M (IAR), 'Debug' Cfg
  "tviibh8m-iar"                    - T2G-B-H-8M (IAR), 'Debug' Cfg
  "tviibh16m-iar"                   - T2G-B-H-16M (IAR), 'Debug' Cfg
  "tviic2d4m-iar"                   - T2G-C-2D-4M (IAR), 'Debug' Cfg
  "tviic2d6m-iar"                   - T2G-C-2D-6M (IAR), 'Debug' Cfg
  "tviic2d6mddr-iar"                - T2G-C-2D-6M-DDR (IAR), 'Debug' Cfg
  "tviice4m-iar"                    - T2G-C-E-4M (IAR), 'Debug' Cfg
  "tviibe1m-diab"                   - T2G-B-E-1M (DIAB), 'Debug' Cfg
  "tviibe1m-sk-diab"                - CYTVII-B-E-1M-SK (DIAB), 'Debug' Cfg
  "tviibe1m-gcc-release"            - T2G-B-E-1M (GCC), 'Release' Cfg
  "tviibe1m-sk-gcc-release"         - CYTVII-B-E-1M-SK (GCC), 'Release' Cfg
  "tviibe2m-gcc-release"            - T2G-B-E-2M (GCC), 'Release' Cfg
  "tviibe4m-gcc-release"            - T2G-B-E-4M (GCC), 'Release' Cfg
  "tviibe512k-gcc-release"          - T2G-B-E-512K (GCC), 'Release' Cfg
  "tviibh4m-gcc-release"            - T2G-B-H-4M (GCC), 'Release' Cfg
  "tviibh8m-gcc-release"            - T2G-B-H-8M (GCC), 'Release' Cfg
  "tviibh16m-gcc-release"           - T2G-B-H-16M (GCC), 'Release' Cfg
  "tviic2d4m-gcc-release"           - T2G-C-2D-4M (GCC), 'Release' Cfg
  "tviic2d6m-gcc-release"           - T2G-C-2D-6M (GCC), 'Release' Cfg
  "tviic2d6mddr-gcc-release"        - T2G-C-2D-6M-DDR (GCC), 'Release' Cfg
  "tviice4m-gcc-release"            - T2G-C-E-4M (GCC), 'Release' Cfg
  "tviibe1m-ghs-release"            - T2G-B-E-1M (GHS), 'Release' Cfg
  "tviibe1m-sk-ghs-release"         - CYTVII-B-E-1M-SK (GHS), 'Release' Cfg
  "tviibe2m-ghs-release"            - T2G-B-E-2M (GHS), 'Release' Cfg
  "tviibe4m-ghs-release"            - T2G-B-E-4M (GHS), 'Release' Cfg
  "tviibe512k-ghs-release"          - T2G-B-E-512K (GHS), 'Release' Cfg
  "tviibh4m-ghs-release"            - T2G-B-H-4M (GHS), 'Release' Cfg
  "tviibh8m-ghs-release"            - T2G-B-H-8M (GHS), 'Release' Cfg
  "tviibh16m-ghs-release"           - T2G-B-H-16M (GHS), 'Release' Cfg
  "tviic2d4m-ghs-release"           - T2G-C-2D-4M (GHS), 'Release' Cfg
  "tviic2d6m-ghs-release"           - T2G-C-2D-6M (GHS), 'Release' Cfg
  "tviic2d6mddr-ghs-release"        - T2G-C-2D-6M-DDR (GHS), 'Release' Cfg
  "tviice4m-ghs-release"            - T2G-C-E-4M (GHS), 'Release' Cfg
  "tviibe1m-iar-release"            - T2G-B-E-1M (IAR), 'Release' Cfg
  "tviibe1m-sk-iar-release"         - CYTVII-B-E-1M-SK (IAR), 'Release' Cfg
  "tviibe2m-iar-release"            - T2G-B-E-2M (IAR), 'Release' Cfg
  "tviibe4m-iar-release"            - T2G-B-E-4M (IAR), 'Release' Cfg
  "tviibe512k-iar-release"          - T2G-B-E-512K (IAR), 'Release' Cfg
  "tviibh4m-iar-release"            - T2G-B-H-4M (IAR), 'Release' Cfg
  "tviibh8m-iar-release"            - T2G-B-H-8M (IAR), 'Release' Cfg
  "tviibh16m-iar-release"           - T2G-B-H-16M (IAR), 'Release' Cfg
  "tviic2d4m-iar-release"           - T2G-C-2D-4M (IAR), 'Release' Cfg
  "tviic2d6m-iar-release"           - T2G-C-2D-6M (IAR), 'Release' Cfg
  "tviic2d6mddr-iar-release"        - T2G-C-2D-6M-DDR (IAR), 'Release' Cfg
  "tviice4m-iar-release"            - T2G-C-E-4M (IAR), 'Release' Cfg
  "tviibe1m-diab-release"           - T2G-B-E-1M (DIAB), 'Release' Cfg
  "tviibe1m-sk-diab-release"        - CYTVII-B-E-1M-SK (DIAB), 'Release' Cfg
  "tviibe1m-gcc-relwithdebinfo"     - T2G-B-E-1M (GCC), 'Release w/ Debug Info' Cfg
  "tviibe1m-sk-gcc-relwithdebinfo"  - CYTVII-B-E-1M-SK (GCC), 'Release w/ Debug Info' Cfg
  "tviibe2m-gcc-relwithdebinfo"     - T2G-B-E-2M (GCC), 'Release w/ Debug Info' Cfg
  "tviibe4m-gcc-relwithdebinfo"     - T2G-B-E-4M (GCC), 'Release w/ Debug Info' Cfg
  "tviibe512k-gcc-relwithdebinfo"   - T2G-B-E-512K (GCC), 'Release w/ Debug Info' Cfg
  "tviibh4m-gcc-relwithdebinfo"     - T2G-B-H-4M (GCC), 'Release w/ Debug Info' Cfg
  "tviibh8m-gcc-relwithdebinfo"     - T2G-B-H-8M (GCC), 'Release w/ Debug Info' Cfg
  "tviibh16m-gcc-relwithdebinfo"    - T2G-B-H-16M (GCC), 'Release w/ Debug Info' Cfg
  "tviic2d4m-gcc-relwithdebinfo"    - T2G-C-2D-4M (GCC), 'Release w/ Debug Info' Cfg
  "tviic2d6m-gcc-relwithdebinfo"    - T2G-C-2D-6M (GCC), 'Release w/ Debug Info' Cfg
  "tviic2d6mddr-gcc-relwithdebinfo" - T2G-C-2D-6M-DDR (GCC), 'Release w/ Debug Info' Cfg
  "tviice4m-gcc-relwithdebinfo"     - T2G-C-E-4M (GCC), 'Release w/ Debug Info' Cfg
  "tviibe1m-ghs-relwithdebinfo"     - T2G-B-E-1M (GHS), 'Release w/ Debug Info' Cfg
  "tviibe1m-sk-ghs-relwithdebinfo"  - CYTVII-B-E-1M-SK (GHS), 'Release w/ Debug Info' Cfg
  "tviibe2m-ghs-relwithdebinfo"     - T2G-B-E-2M (GHS), 'Release w/ Debug Info' Cfg
  "tviibe4m-ghs-relwithdebinfo"     - T2G-B-E-4M (GHS), 'Release w/ Debug Info' Cfg
  "tviibe512k-ghs-relwithdebinfo"   - T2G-B-E-512K (GHS), 'Release w/ Debug Info' Cfg
  "tviibh4m-ghs-relwithdebinfo"     - T2G-B-H-4M (GHS), 'Release w/ Debug Info' Cfg
  "tviibh8m-ghs-relwithdebinfo"     - T2G-B-H-8M (GHS), 'Release w/ Debug Info' Cfg
  "tviibh16m-ghs-relwithdebinfo"    - T2G-B-H-16M (GHS), 'Release w/ Debug Info' Cfg
  "tviic2d4m-ghs-relwithdebinfo"    - T2G-C-2D-4M (GHS), 'Release w/ Debug Info' Cfg
  "tviic2d6m-ghs-relwithdebinfo"    - T2G-C-2D-6M (GHS), 'Release w/ Debug Info' Cfg
  "tviic2d6mddr-ghs-relwithdebinfo" - T2G-C-2D-6M-DDR (GHS), 'Release w/ Debug Info' Cfg
  "tviice4m-ghs-relwithdebinfo"     - T2G-C-E-4M (GHS), 'Release w/ Debug Info' Cfg
  "tviibe1m-iar-relwithdebinfo"     - T2G-B-E-1M (IAR), 'Release w/ Debug Info' Cfg
  "tviibe1m-sk-iar-relwithdebinfo"  - CYTVII-B-E-1M-SK (IAR), 'Release w/ Debug Info' Cfg
  "tviibe2m-iar-relwithdebinfo"     - T2G-B-E-2M (IAR), 'Release w/ Debug Info' Cfg
  "tviibe4m-iar-relwithdebinfo"     - T2G-B-E-4M (IAR), 'Release w/ Debug Info' Cfg
  "tviibe512k-iar-relwithdebinfo"   - T2G-B-E-512K (IAR), 'Release w/ Debug Info' Cfg
  "tviibh4m-iar-relwithdebinfo"     - T2G-B-H-4M (IAR), 'Release w/ Debug Info' Cfg
  "tviibh8m-iar-relwithdebinfo"     - T2G-B-H-8M (IAR), 'Release w/ Debug Info' Cfg
  "tviibh16m-iar-relwithdebinfo"    - T2G-B-H-16M (IAR), 'Release w/ Debug Info' Cfg
  "tviic2d4m-iar-relwithdebinfo"    - T2G-C-2D-4M (IAR), 'Release w/ Debug Info' Cfg
  "tviic2d6m-iar-relwithdebinfo"    - T2G-C-2D-6M (IAR), 'Release w/ Debug Info' Cfg
  "tviic2d6mddr-iar-relwithdebinfo" - T2G-C-2D-6M-DDR (IAR), 'Release w/ Debug Info' Cfg
  "tviice4m-iar-relwithdebinfo"     - T2G-C-E-4M (IAR), 'Release w/ Debug Info' Cfg
  "tviibe1m-diab-relwithdebinfo"    - T2G-B-E-1M (DIAB), 'Release w/ Debug Info' Cfg
  "tviibe1m-sk-diab-relwithdebinfo" - CYTVII-B-E-1M-SK (DIAB), 'Release w/ Debug Info' Cfg
```

So in summary you should see the same preset options available like during the configuration stage but each of them appearing in 3 flavors supporting the 3 build configurations:

| Build Preset Naming          | Build Configuration                                                                                                                                                |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| *preset_name*                | *Debug* build configuration. Build preset name does not have a suffix because this is assumed to be the most commonly used build configuration for Traveo T2G SDL. |
| *preset_name*-release        | *Release* build configuration                                                                                                                                      |
| *preset_name*-relwithdebinfo | *RelWithDebInfo* build configuration                                                                                                                               |

Of course, only presets can be chosen for building for which the configuration stage had been run already.

### Building the project

The CMake build step is initiated with the following command (*tviibe1m-ghs* build preset will be used as an example)

`cmake --build --preset tviibe1m-ghs`

This should result in the following output (note that the progress is constantly updated in the command shell in a single line, only the final message is shown below):

```
C:\T2G_Sample_Driver_Library>cmake --build --preset tviibe1m-ghs
[115/115] Linking C executable Debug\cm0plus.elf
```

## Debugging

Like described earlier, CMake also supports custom targets that can be used to trigger any command. The Traveo T2G CMake files will define custom targets to start a supported debugger. 

These custom targets will end up as build targets of the selected build system (*Ninja* in our case). This means that the already known build command syntax can be used, but the name of the custom target needs to be appended, e.g.:

`cmake --build --preset tviibe1m-ghs --target dbg_ghs_cm0plus_cm4`

This example command will start the GHS MULTI debugger with a configuration that includes the executables for Cortex-M0+ and Cortex-M4 CPU cores within T2G-B-E-1M. 

At the moment, it is assumed that the project has been built already (future releases may add proper dependencies and built the executables before debugging).

**For debugging using Visual Studio Code and OpenOCD please refer the *Visual Studio Code* chapter.**

### Available debug targets

The following table lists all debug targets that are available, the <tool> placeholder needs to be replaced with the identifier of a supported toolchain, at the moment following tools are supported:

- `ghs` -> GreenHills MULTI
- `iar` -> IAR
- `t32` -> Lauterbach Trace32

| Target Name                   | Applicable T2G Devices                 | Description                                                                          |
| ----------------------------- | -------------------------------------- | ------------------------------------------------------------------------------------ |
| `dbg_<tool>_cm0plus`          | all                                    | Configuration that only includes the CM0+ executable                                 |
| `dbg_<tool>_cm0plus_cm4`      | all with Cortex-M4 CPU core            | Configuration that includes the CM0+ and the CM4 executable.                         |
| `dbg_<tool>_cm0plus_cm7_0`    | all with Cortex-M7 CPU core            | Configuration that includes the CM0+ and the CM7 instance #0 executable.             |
| `dbg_<tool>_cm0plus_cm7_1`    | all with dual/quad Cortex-M7 CPU cores | Configuration that includes the CM0+ and the CM7 instance #1 executable.             |
| `dbg_<tool>_cm0plus_cm7_dual` | all with dual/quad Cortex-M7 CPU cores | Configuration that includes the CM0+ and CM7 instance #0 and #1 executables.         |
| `dbg_<tool>_cm0plus_cm7_2`    | all with quad Cortex-M7 CPU cores      | Configuration that includes the CM0+ and the CM7 instance #2 executable.             |
| `dbg_<tool>_cm0plus_cm7_3`    | all with quad Cortex-M7 CPU cores      | Configuration that includes the CM0+ and the CM7 instance #3 executable.             |
| `dbg_<tool>_cm0plus_cm7_quad` | all with quad Cortex-M7 CPU cores      | Configuration that includes the CM0+ and CM7 instance #0, #1, #2 and #3 executables. |

Some combinations of toolchain and target type may not be possible. If in doubt please check the CMake output during the configuration phase, it will list all created debugger targets.

**Note**: At the moment only the `flash` link location is supported by the included Lauterbach Trace32 and IAR script/project files.

## Utilities

### Infineon Auto Flash Utility (AFU) / OpenOCD (OOCD)

The Infineon Auto Flash Utility (AFU) / OpenOCD (OOCD) can be used to program or erase the MCU flash memory or for many other purposes (I/O reg read/write, verification, dumping, ...).

Like for debugging, custom targets will be created that can be used to trigger certain AFU commands. 

An example command line to trigger one of the commands could look as follows:

`cmake --build --preset tviibe1m-ghs --target util_afu_oocd_erase_all`

This example command will start the Infineon Auto Flash Utility / OpenOCD and erase the complete Code Flash memory within T2G-B-E-1M.

At the moment, it is assumed that the project has been built already (future releases may add proper dependencies and built the executables before triggering certain AFU commands).

Below is a list of the currently prepared custom targets

| Target Name                  | Applicable T2G Devices                 | Description                                 |
| ---------------------------- | -------------------------------------- | ------------------------------------------- |
| `util_afu_oocd_prog_cm0plus` | all                                    | Program the CM0+ executable only            |
| `util_afu_oocd_prog_cm4`     | all with Cortex-M4 CPU core            | Program the CM4 executable only             |
| `util_afu_oocd_prog_cm7_0`   | all with Cortex-M7 CPU core            | Program the CM7 instance #0 executable only |
| `util_afu_oocd_prog_cm7_1`   | all with dual/quad Cortex-M7 CPU cores | Program the CM7 instance #1 executable only |
| `util_afu_oocd_prog_cm7_2`   | all with quad Cortex-M7 CPU cores      | Program the CM7 instance #2 executable only |
| `util_afu_oocd_prog_cm7_3`   | all with quad Cortex-M7 CPU cores      | Program the CM7 instance #3 executable only |
| `util_afu_oocd_prog_all`     | all                                    | Program all executables                     |
| `util_afu_oocd_erase_all`    | all                                    | Erase all Code Flash memory                 |

### MCAL utilities (e.g. Tresos GUI)

Refer to the corresponding sub-chapter in the *MCAL* chapter.

# Visual Studio Code

The very popular *Visual Studio Code* (VS Code) editor/IDE can be used in combination with the Traveo T2G CMake support to offer a GUI based user experience. 

The syntax highlighting and other language features benefit from CMake automatically providing all include paths, compile definitions, etc. for the currently selected project. E.g. this means that code that will not be part of the build can be greyed out correctly by VSC because it knows all commandline pre-processor defines.

## Prequisites

Download VS Code from the [website](https://code.visualstudio.com/download). 

Following list of VS Code extensions are recommended or even mandatory:

| Extension Name                                                                           | Description                                                                                                                                |
| ---------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------ |
| [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)          | Adds C/C++ language support to VS Code                                                                                                     |
| [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) | Adds support for a full-featured, convenient and powerful workflow for CMake based projects                                                |
| [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake)                  | Provides CMake syntax highlighting, completions, help, etc.(Optional, recommended if you are intending to modify the provided CMake files) |
| [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) | Supports debugging Cortex-M MCUs using different GDB server backends (e.g. OpenOCD), requires Arm GNU toolchain (GDB and other tools)      |

## Usage

This is only a short/quick guide. Please refer the respective project websites of the extensions for more exhaustive explanations (e.g. extension settings, usage, etc.)

### Overview

1. Open the T2G SDL root folder in VS Code
2. The CMake Tools extension should process and evaluate the CMakePresets.json
3. In the status bar at the bottom, you should be able to   
   - select the active configuration preset   
   - select the active build preset (CMake Tools automatically restricts the choice to the presets that are applicable for the currently selected configuration preset)   
   - initiate a build (which will first trigger the configuration phase)
4. Alternatively, you can also initiate commands via the VS Code command palette (Ctrl-Shift-P), e.g.:   
   - *CMake: Select Configure Preset*   
   - *CMake: Select Build Preset*   
   - *CMake: Configure*   
   - *CMake: Build*
5. The CMake project outline together with the custom targets (e.g. debugger targets) are shown in the CMake section of the VS Code side-bar (click the icon *CMake* to show it).
   From there you can run the custom targets (e.g. GHS debugger). Again, you can also "build" a custom target using the VS Code command palette (*CMake: Build Target*)
6. Alternatively/Additionally, for debugging within VS Code refer the following sub-chapter

### Debugging  
  
For debugging with Visual Studio Code the following prerequisites have to be fulfilled:  
- *C/C++*, *CMake Tools* and *Cortex-Debug* extensions need to be installed
- Arm GNU toolchain needs to be installed (also when debugging executables produced by e.g. GHS, IAR, DIAB), [Download Page](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads), tested with **arm-gnu-toolchain-11.3.rel1-mingw-w64-i686-arm-none-eabi**
- *Infineon Auto Flash Utility* or *Infineon Open OCD* fork needs to be installed
- Supported debuggers are: *Infineon MiniProg4*, *SEGGER J-Link* or an onboard *Infineon Kitprog* chip (e.g. as on *CYTVII-B-E-1M-SK* evaluation board)
- All relevant items in `./cmake/tool_config.cmake` must be correct, e.g. Arm GNU toolchain and Infineon Auto Flash Utility (AFU) / OpenOCD (OOCD) sections

Once (and every time) the CMake configuration phase is executed, it will create a `launch.json` file for the current workspace (`/.vscode/launch.json`). This file contains the debug configurations (e.g. `cm0plus`, `cm0plus_cm4`, ...) with all relevant paths, etc. for the currently selected CMake configuration preset. This means you can click the *Run and Debug* side-bar icon, select the desired debug configuration in the drop-down list at the top and launch the debugging session with the green *play* button.
*Notes:* 
- On the first attempt VS Code will ask you which target to launch. Any of the available targets (e.g. `cm0plus.elf`) can be selected as this is only used within `launch.json` to determine the proper folder path to the *ELF* files, the actual target to be debugged is selected by debug configuration drop-down as described above.
- If you switch between configuration presets in the same workspace, always ensure that the CMake configuration phase for the newly selected preset has been executed (either manually or maybe laready done automatically), so that the `launch.json` content is getting refreshed.
- When manually modifying `launch.json` always consider that the file can get overwritten, therefore adjusting the template in `/cmake/targets/debugger_vsc.cmake` might be the better solution for permanent changes

# Advanced usage

## CMake cache / variables

The CMake cache is a kind of configuration file whose values persist across subsequent runs of CMake for a given project. It is a file called `CMakeCache.txt` and is generated by CMake on the first configuration run for a project and stored in the build output folder.

The cache contains variables that
- have been passed to CMake via the `-D` option on the command line:
   `-D <var>[:<type>]=<value>`
- have been designated to be cache variables by the user in their CMake scripts using this form of the `set` command: `set(<variable> <value>... CACHE <type> <docstring> [FORCE])` or the `option` command
- have been designated by the CMake developers to be cache variables

Once a cache variable has been set and is present, its value can only be changed in the following way:
- Pass the new/updated value via the `-D` command line option
- Edit the `CMakeCache.txt` file directly in a text editor, or preferably use a CMake  GUI tool like[ `cmake-gui.exe` ]([cmake-gui(1) &mdash; CMake 3.23.2 Documentation](https://cmake.org/cmake/help/latest/manual/cmake-gui.1.html#manual:cmake-gui(1)))which is part of the official CMake binaries
- Use the `FORCE` option of the `set` command when setting the cache variable: `set(<variable> <value>... CACHE <type> <docstring> FORCE)`

Of course the CMake cache file can also be deleted which causes CMake to re-generate it on the next configuration run.

Further useful and more detailed information about the CMake cache can be found [here]([CMake Cache &#8212; Mastering CMake](https://cmake.org/cmake/help/book/mastering-cmake/chapter/CMake%20Cache.html)).

### Potential pitfalls

As has been described above, once a cache variable is set its value can only be changed in certain ways. This is often not considered by beginners and often forgotten by experienced users when dealing with CMake.

**Example:**

The `CMakeLists.txt` included with the SDL package sets the `USE_RTOS`cache variable:

```
[...]
option(USE_RTOS "Include FreeRTOS functionality" ON)
[...]
```

Let us assume you have run the configuration already, but then you decide that you do not need the RTOS functionality after all. You edit the `CMakeLists.txt` and change the setting to OFF:

```
[...]
option(USE_RTOS "Include FreeRTOS functionality" OFF)
[...]
```

If you now re-run the configuration, *nothing will change* because the variable in the cache is not being updated.

Following solutions exist:

1. Delete the `CMakeCache.txt` and re-run the configuration
2. Edit the variable value in `CMakeCache.txt` via an editor or GUI tool
3. Pass the new value on the command line when configuring, e.g. 
   `cmake --preset tviibe1m-ghs -D USE_RTOS=OFF`

Keep in mind that values of cache variables could depend on the values of other cache variables. A change of a cache variable will not propagate to another cache variable that depends on its value, use one of the solutions described above.

## CMake without presets

CMake presets are just a user-friendly way to pass all required variables and parameters (e.g. toolchain file, generator type, source and build directories) to CMake. Instead of passing a preset, the user can also specify all parameters manually to have better control about them. It is also possible to use a preset but in addition append or change variables on the command line. Please note that at the time of writing the mixed usage of CMake presets and additional variables on the command line is not possible with the VS Code *CMake Tools* plugin (e.g. refer this [issue](https://github.com/microsoft/vscode-cmake-tools/issues/1836)).

### Configuration

Instead of using e.g. this configuration preset

`cmake --preset tviibe1m-ghs`

the following invocation of CMake is equivalent:

`cmake -B ./build/tviibe1m-ghs -G "Ninja Multi-Config" --toolchain ./cmake/toolchains/toolchain-ghs.cmake -D MCU_REV=latest -D LINK=flash -D DIE=tviibe1m -D BOARD_REV=CPU_BOARD_REVC`

The build output directory, the generator type, the toolchain file and all necessary cache variables need to be specified.

Following table lists all cache variables that need to be passed via command line because they do not get assigned a value within the CMake scripts:

| Variable Name | Possible Values                                                                                                                                                                                | Remarks   |
| ------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------- |
| `DIE`         | `tviibe1m`, `tviibe2m`, ...                                                                                                                                                                    | Mandatory |
| `MCU_REV`     | Either a specific revision letter like `A`, `B`, `C`, ... or the *pseudo* revision `latest` (the CMake script will then find out the latest revision by checking the presence of some folders) | Mandatory |
| `LINK`        | SDL linker files by default support two locations for linking ROM-type sections: `flash` or `sram`                                                                                             | Mandatory |
| `BOARD_REV`   | Some derivates expect that the revision of the used evaluation board is specified, e.g. `CPU_BOARD_REVC`                                                                                       | Optional  |
| `BOARD`       | Some derivates expect that used evaluation board type is specified, e.g. `CY_176LQFP_EVK_REV_A` (the type typically already includes the revision so that `BOARD_REV` can be omitted)          | Optional  |

Following non-exhaustive list of variables do have a default value assigned by the CMake scripts but often need to be overwritten by passing the correct value via command line:

| Variable Name  | Possible Values                                                                                                                                                                                                                                                                                                                                                                                              | Remarks                                                                                                                  |
| -------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------ |
| `DEVICE`       | One of the available device part numbers for a given derivate/die, e.g. check `cy_device_headers.h` inside the `./<die>/hdr/rev_?/` folder and the datasheet. By default the device is set to the one that is used on the eval board of the selected derivate. If a derivate offers multiple eval boards (e.g. with different package sizes), the one with the largest package has been selected by default. | This variable needs to be changed if you use a different eval board than the one considered as default or a custom board |
| `PSVP`         | `OFF` (default) or `ON` or other corresponding boolean values supported by CMake                                                                                                                                                                                                                                                                                                                             | If for certain derivates or revisions there is no silicon yet                                                            |
| `USE_ETHSTACK` | `OFF` (default) or `ON`or other corresponding boolean values supported by CMake                                                                                                                                                                                                                                                                                                                              | Enables the building of the Ethernet stack, only supported by some DIE                                                   |
| `USE_MCAL`     | `OFF` (default) or `ON`or other corresponding boolean values supported by CMake                                                                                                                                                                                                                                                                                                                              | Enables integration of MCAL                                                                                              |

**Example**:

The default device for `tviibh8m` is `CYT4BFCCHE` which is the 320-pin BGA package. As there are also evaluation boards available with a 176-pin QFP package, the `DEVICE` variable needs to be changed to `CYT4BF8CDE` if you target such a board. This can be done e.g. by passing the new value on the command line when configuring the project:

`cmake --preset tviibh8m-ghs -D DEVICE=CYT4BF8CDE`

### Building

Instead of using e.g. this build preset

`cmake --build --preset tviibe1m-ghs-release`

the following invocation of CMake is equivalent:

`cmake --build ./build/tviibe1m-ghs --config Release`

The build output directory and the build configuration need to be specified.

## Project modifications

### Adding source files

There are many options how to add your own additional source files to the project. The function `create_executable()` which is called in `CMakeLists.txt` has several optional arguments, one of them is `ADDL_SOURCES` which is currently also used to pass the `main_*.c`  files. You can add your sources there. If the corresponding header files are not in one of the standard include search directories, you could add the necessary directory through the `ADDL_INCLUDE_DIRS` argument. Alternatively, you can also add it the variable `INCLUDE_DIRS` which is used by all components (libraries and executables). The same holds true for compiler defines which could use `ADDL_COMPILER_DEFS` argument or the `COMPILER_DEFINES` variable.

Of course it is also possible to use functions provided by CMake like `target_sources()`, `target_include_directories()`, `target_compile_definitions()` etc to extend the executables after they have been created.

# MCAL

## Prerequisites

### Tresos/MCAL installation

Due to license reasons SDL does not come with the actual Tresos and MCAL package. It is assumed that you have properly installed the Tresos, MCAL and related packages (e.g. Self-Test Libraries, or Flash EEPROM Emulation driver) and that the Tresos license has been activated successfully.

### MCAL examples

Starting from SDL version 7.6.0 the `examples` folders of the derivates now also contain MCAL examples (e.g. subfolders in `./tviibe1m/src/examples/mcal`). Like with any other SDL example, it is expected that you copy all the files/folders of an example to the derivates `src` folder, thereby overwring existing files (you may want to backup them prior to that...)

**Example**:
Copy everything in `./tviibe1m/src/examples/mcal/Dio_Gpt_Icu_Mcu_Port` to `./tviibe1m/src`

### Tool configuration

All relevant variables in `./cmake/tool_config.cmake` or the corresponding environment variables need to be properly setup. Refer the corresponding chapter in this Readme for more information about this file and chapter *CMake variables related to MCAL* for more descriptions about the variables

## CMake configuration phase

The same commands like described in the other sections of this Readme apply to the MCAL case as well. Since by default the USE_MCAL variable is OFF it needs to be set to ON, e.g. by appending it to the command line as follows:

`cmake --preset tviibe1m-ghs -D USE_MCAL=ON`

This should result in the following output:

```
C:\T2G_Sample_Driver_Library>cmake --preset tviibe1m-ghs -D USE_MCAL=ON
Preset CMake variables:

  BOARD_REV="CPU_BOARD_REVC"
  CMAKE_TOOLCHAIN_FILE:FILEPATH="C:/T2G_Sample_Driver_Library/cmake/toolchains/toolchain-ghs.cmake"
  DIE="tviibe1m"
  LINK="flash"
  MCU_REV="latest"

-- Latest MCU revision detected: D
-- The C compiler identification is GHS 2017.1.4
-- The ASM compiler identification is unknown
-- Found assembler: C:/ghs/comp_201714/ccthumb.exe
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/ghs/comp_201714/ccthumb.exe - skipped
-- Warning: Did not find file Compiler/-ASM
-- ============================================================
-- T2G SDL - CMake Configuration
-- ------------------------------------------------------------
-- Toolchain         = ghs
-- DIE               = tviibe1m
-- MCU revision      = D
-- Device            = CYT2B78CAE
-- Link location     = flash
-- PSVP              = OFF
-- Board             =
-- Board revision    = CPU_BOARD_REVC
-- Use RTOS          = ON
-- Use ETH Stack     = OFF
-- Use MCAL          = ON
-- Tresos            = C:/INFINEON_ESDB/Tresos26_2_0
-- Tresos workspace  = C:/T2G_Sample_Driver_Library/tresos_workspace
-- Tresos project    = C:/T2G_Sample_Driver_Library/tviibe1m/src/TVII-B-E-1M_QFP-176
-- ============================================================
-- Creating library: drivers_cm0plus
-- Creating library: mw_cm0plus
-- Creating library: drivers_cm4
-- Creating library: mw_cm4
-- Creating library: rtos_cm4
-- Creating library: mcalstubs_cm4
-- Creating library: mcal_cm4
-- Generating MCAL files because 'generated_cmake_file_for_mcal.cmake' does not exist
****************************************
Generate files:
~~~~~~~~~~~~~~~
call C:/INFINEON_ESDB/Tresos26_2_0/bin/tresos_cmd.bat -Dinfo=false -data C:/T2G_Sample_Driver_Library/tresos_workspace make generate TVII-B-E-1M_QFP-176
EB tresos Studio 26.2.0 b191017-0938
Copyright 2003-2019 Elektrobit Automotive GmbH
WARNING 22-06-09,16:29:35 (APPC_2) The following message severities are filtered and will not be logged: INFO

Current workspace: C:\T2G_Sample_Driver_Library\tresos_workspace

Errors "0" Warnings "0"
****************************************
-- Creating executable: cm0plus
-- Creating executable: cm4
-- Creating GHS debugger targets:
--   dbg_ghs_cm0plus
--   dbg_ghs_cm0plus_cm4
-- Creating Lauterbach T32 debugger targets:
--   dbg_t32_cm0plus
--   dbg_t32_cm0plus_cm4
-- Creating Visual Studio Code debugger launch configuration
--   Writing file C:/T2G_Sample_Driver_Library/.vscode/launch.json
-- Creating Auto Flash Utility / OpenOCD targets:
--   util_afu_oocd_prog_cm0plus
--   util_afu_oocd_prog_cm4
--   util_afu_oocd_prog_all
--   util_afu_oocd_erase_all
-- Creating Tresos Utility targets:
--   util_tresos_gui
-- Configuring done
-- Generating done
-- Build files have been written to: C:/T2G_Sample_Driver_Library/build/tviibe1m-ghs
```

As can be the seen in the example log output above, the CMake scripts will invoke the Tresos code generator to generate the configuration dependent MCAL source/header files (and other files). Since this step is rather time consuming, it will only be executed if the scripts detect that this step is necessary, a corresponding dependency checking has been implemented and the log output indicates the reason for running the generator.

## Building / Debugging

There is no difference for building or debugging the projects compared to the non-MCAL case.

## Utilities

When MCAL usage is enabled, following additional custom targets will be created that can be used to invoke utilities:

| Target Name       | Description                                                                                                           |
| ----------------- | --------------------------------------------------------------------------------------------------------------------- |
| `util_tresos_gui` | Start the Tresos GUI with the current Tresos project, e.g. to make changes to the configuration or fix issues with it |

Please note that the CMake scripts will try to gracefully complete the CMake configuration step even in the presence of errors during Tresos code generation. Of course in this state a successful build of the project will not be possible but it ensures that the utility targets had been created. Hence, the `util_tresos_gui` can be invoked to open Tresos GUI and fix the issues in the Tresos project configuration that led to the failure when trying to generate the project.

## CMake variables related to MCAL

Following table summarizes and describes all CMake cache variables that are related to MCAL usage.

| Variable Name                   | Description                                                                                                                                                                                                                                                                             |
| ------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `USE_MCAL`                      | Needs to be `ON` (or other corresponding boolean values supported by CMake) to enable MCAL support                                                                                                                                                                                      |
| `MCAL_VAR_TRESOS_DIR`           | Path to your Tresos folder that includes the Tresos binaries, MCAL plugins, etc. The default value is the installation path that is proposed by the Infineon Tresos/MCAL installers. It is likely that you may want to change this especially when working with multiple MCAL releases. |
| `MCAL_VAR_TRESOS_WORKSPACE_DIR` | As Tresos is an Eclipse based tool it requires a workspace folder. The default workspace is a folder in the SDL root which will be created automatically. It should not be necessary to change this variable.                                                                           |
| `MCAL_VAR_TRESOS_PROJECT_DIR`   | Path to the Tresos project folder to be used. The default value is suitable for MCAL/Tresos example projects delivered with SDL, assuming that you have copied them to the derivate's `src` folder (like you would do for any other SDL example)                                        |
| `AUTOFIX_TRESOS_GEN_PATH`       | OFF by default. Can be set to `ON` (or other corresponding boolean values supported by CMake) to let the CMake script update the output generation path in your Tresos project. MCAL/Tresos example projects delivered with SDL do not require this as they have the correct setting.   |

# Troubleshooting

## Verbose output

The log output level during the configuration stage can be controlled by following command line parameter:

`--log-level=<ERROR|WARNING|NOTICE|STATUS|VERBOSE|DEBUG|TRACE>`

It can just be appended like in the following example:

`cmake --preset tviibe1m-ghs --log-level=DEBUG`

The default log level is STATUS, more verbose information may be available when choosing DEBUG or TRACE.

## Delete CMake cache

In case of strange problems (especially after modifying CMake files) it is recommended to try again after deleting the CMake cache. The most easiest way to do this is to just delete the complete build output folder `./build/<cfg-preset-name>`(default name).

## Clean the build outputs

Instead of deleting the whole build folder and hence the output from the CMake configuration stage, it might be sufficient to clean the build output through the "clean" target, e.g.:

`cmake --build --preset tviibe1m-ghs --target clean`

This might help e.g. when dependencies are not working correctly.

## DIAB compiler identification fails during CMake configure

CMake tries to detect and test a toolchain by compiling a test file during the first invocation of a CMake configuration phase. With the DIAB compiler this might fail if you have not configured a proper default DIAB target configuration because in this step CMake will not pass the correct target flag (`-t...`) to the compiler.

You can set a valid DIAB default target with the `dctrl -t` command to e.g. the following values:

```
C:\WindRiver\compilers\diab-5.9.8.1\WIN32\bin>dctrl -t
Possible architectures:
  1) ARM
  2) ARMv8
  3) TriCore
  4) MCS

Select default architecture? (1-4)[1] 1

Possible devices:
   1) ARM             2) ARMT            3) ARMT2           4) ARMX
   5) ARMXT           6) ARMV5E          7) ARM968E-S       8) ARMV5ET
   9) ARM968E-ST     10) ARMV6          11) ARMV6K         12) ARMV6MT
  13) ARMV7          14) ARMV7A         15) ARMV7R         16) ARMV7AT2
  17) ARMV7RT2       18) ARMV7MT2       19) ARMCORTEXA5    20) ARMCORTEXA5T2
  21) ARMCORTEXA7    22) ARMCORTEXA7T2  23) ARMCORTEXA8    24) ARMCORTEXA8T2
  25) ARMCORTEXA9    26) ARMCORTEXA9T2  27) ARMCORTEXA15   28) ARMCORTEXA15T2
  29) ARMCORTEXR4    30) ARMCORTEXR4T2  31) ARMCORTEXR5    32) ARMCORTEXR5T2
  33) ARMCORTEXR7    34) ARMCORTEXR7T2  35) ARMCORTEXM0    36) ARMCORTEXM0P
  37) ARMCORTEXM1    38) ARMCORTEXM3    39) ARMCORTEXM4    40) ARMCORTEXM7

Select device or core? (1-40)[1] 36

Possible object formats:
  1) ARM(XSCALE)/Thumb ELF Object Format
  2) ARM(XSCALE)/Thumb ELF little-endian Object Format
  3) ARM(XSCALE)/Thumb AAELF Object Format
  4) ARM(XSCALE)/Thumb AAELF little-endian Object Format

Select object format? (1-4)[1] 2

Possible floating point modes:
  1) Software Floating Point
  2) No Floating Point

Floating point mode "None" can be used to reduce the library footprint
of programs that do not use floating point.

Select float point mode? (1-2)[1] 1

Possible environments:
   1) windiss   - Instruction Set Simulator
   2) cross     - Use Ram Disk for I/O
   3) simple    - Only character I/O
   4) other     - Enter text string

The environment controls which libraries are linked by default.
It also specifies an {environ}.conf referred to by dtools.conf.

Select environment? (1-4)[1] 3

Selected default compilation mode is: -tARMCORTEXM0PLS:simple
```

# References

| Link                                                                              | Description                                                                                            |
| --------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------ |
| [CMake](https://cmake.org/)                                                       | CMake website                                                                                          |
| [CMake Reference Documentation](https://cmake.org/cmake/help/latest/)             | Reference Documentation for CMake (pointing to latest CMake revision)                                  |
| [Professional CMake: A practical guide](https://crascit.com/professional-cmake/)  | Book from one of the CMake co-maintainers, covering basic as well as advanced aspects of modern CMake. |
| [Ninja build system](https://ninja-build.org/)                                    | Build system focusing on speed/performance                                                             |
| [VS Code](https://code.visualstudio.com/)                                         | Visual Studio Code                                                                                     |
| [VS Code: CMake Tools extension](https://github.com/microsoft/vscode-cmake-tools) | Very useful CMake extension for VS Code                                                                |

# Known issues

This is a non-exhaustive list of known issues:

- Low test coverage considering the huge combinatorial possibilities
- Utilities (e.g. GHS debugger) are started in command shell blocking mode, once the utility has started you can cancel the process by pressing Ctrl-C which frees the command shell for further commands, but the utility should stay open. 
  Example: After starting the GHS MULTI debugger you want to use the same command shell for further command input, e.g. when going through further code->build->debug iterations. 
- Applicable for targets with dual CM7: Only one common RTOS library (rtos_cm7) is built. In case the CM7s shall have a different RTOS configuration, separate libs should be built with separate FreeRTOSConfig.h files and corresponding include paths. (Note: this is also applicable for non-CMake builds using the tool's projects)
- Using ELF files produced by GCC with other tools has the following known problems:  
  - IAR does not understand DWARF version 5 which is the default for GCC v11.3. Own/SDL code could be compiled with `-gdwarf-3` flag for example, but GCC libraries would still be DWARF v5 so they would have to be compiled from source as well...  
  - GNU linker (LD) sets the ELF program header member *p_paddr* to the LMA of the sections (e.g. the flash address of the initializers for the *.data* section), some tools may have a problem with this non-standard compliant behavior and hence will not be able to flash the ELF file correctly; e.g.:    
    - GHS does not support it --> as a workaround one could program the S-Record files instead of the ELF files and then connect with "Verify only" option

# Notes

The provided CMake support tries to fit into the existing T2G SDL structure which causes some less optimal architecture/modularization than it would be possible if everything were tailored and structured for CMake from scratch.

