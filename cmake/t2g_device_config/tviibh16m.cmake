#############################################################################################
# User Configuration
#############################################################################################
set(DEVICE CYT6BJCDHE CACHE STRING "Device part number, typically CYT6BJ8DDE (for 176-QFP) or CYT6BJCDHE (for 320-BGA)")

if(${DEVICE} STREQUAL CYT6BJ8DDE)
    set(tresos_project_name TVII-B-H-16M_QFP-176)
elseif(${DEVICE} STREQUAL CYT6BJCDHE)
    set(tresos_project_name TVII-B-H-16M_BGA-320)
endif()
set(MCAL_VAR_TRESOS_PROJECT_NAME ${tresos_project_name} CACHE STRING "MCAL Tresos project name")

#############################################################################################
# Static Configuration
#############################################################################################
set(SERIES cyt6bj)

set(HAVE_CM4                    OFF)     # Device has a Cortex-M4
set(HAVE_CM7_0                  ON)      # Device has a Cortex-M7 (instance #0)
set(HAVE_CM7_1                  ON)      # Device has a Cortex-M7 (instance #1)
set(HAVE_CM7_2                  ON)      # Device has a Cortex-M7 (instance #2)
set(HAVE_CM7_3                  ON)      # Device has a Cortex-M7 (instance #3)
set(HAVE_IP_CXPI                OFF)     # Device supports CXPI
set(HAVE_IP_SMIF                ON)      # Device supports SMIF
set(HAVE_IP_ETHERNET            ON)      # Device supports Ethernet
set(HAVE_IP_FLEXRAY             ON)      # Device supports FlexRay
set(HAVE_IP_AUDIOSS             ON)      # Device supports Audio Sub-System
set(HAVE_IP_SOUNDSS             OFF)     # Device supports Sound Sub-System
set(HAVE_IP_SOUNDSS_DAC         OFF)     # Device supports DAC in Sound Sub-System
set(HAVE_IP_SDHC                ON)      # Device supports SDHC Host Controller
set(HAVE_IP_REGHC               ON)      # Device supports REGHC IP for external power control
set(HAVE_IP_PMIC                OFF)     # Device supports PMIC IP for external power control
set(HAVE_IP_MIPICSI2            OFF)     # Device supports MIPI-CSI2 interface
set(HAVE_IP_FPDLINK             OFF)     # Device supports FPD-Link interface
set(HAVE_IP_VIDEOSS             OFF)     # Device supports Video Sub-System
set(HAVE_IP_AXIDMA              OFF)     # Device supports AXI-DMA Controller
set(HAVE_IP_LPDDR4              OFF)     # Device supports LPDDR4 Controller
set(HAVE_IP_SEGLCD              OFF)     # Device supports Segment LCD Controller

set(HAVE_MW_AUDIO_AIC261        ON)      # Middleware for AIC261 Audio IC
set(HAVE_MW_AUDIO_CS42448       OFF)     # Middleware for CS42448 Audio IC
set(HAVE_MW_ETH_PHY_DP83848Q_Q1 OFF)     # Middleware for DP83848Q-Q1 Ethernet PHY
set(HAVE_MW_ETH_PHY_DP83867     ON)      # Middleware for DP83867 Ethernet PHY
set(HAVE_MW_GFX_ENV             OFF)     # Middleware for "Graphics environment"
set(HAVE_MW_MIPI_SENSOR         OFF)     # Middleware for "MIPI sensor"
set(HAVE_MW_POWER               ON)      # Middleware for "power"
set(HAVE_MW_SMIF_MEM            ON)      # Middleware for "SMIF memories"

set(SMIF_IP_VERSION             ver2)    # Uses SMIF2.0

######################################################################################
# Infineon Auto Flash Utility / OpenOCD Configuration
######################################################################################
set(AFU_OOCD_TARGET_CFG_FILE traveo2_16m.cfg)   # Config name to be used for Infineon Auto Flash Utility / OpenOCD

######################################################################################
# Lauterbach Configuration
######################################################################################
if(${DEVICE} STREQUAL CYT6BJ8DDE)
    set(T32_DEVICE_NAME CYT6BJ8)   # Device name used by Lauterbach (without the CPU suffix)
elseif(${DEVICE} STREQUAL CYT6BJCDHE)
    set(T32_DEVICE_NAME CYT6BJC)   # Device name used by Lauterbach (without the CPU suffix)
endif()

######################################################################################
# IAR Debug Configuration
######################################################################################
set(IAR_DEVICE_NAME CYT6BJ)   # Device name used by IAR (without the CPU suffix)
