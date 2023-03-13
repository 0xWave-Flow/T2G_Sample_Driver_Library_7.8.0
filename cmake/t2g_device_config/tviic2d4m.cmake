#############################################################################################
# User Configuration
#############################################################################################
set(DEVICE CYT3DLABHS CACHE STRING "Device part number, typically CYT3DLABHS (for 216-QFP)")

set(MCAL_VAR_TRESOS_PROJECT_NAME TVII-C-2D-4M_QFP-216 CACHE STRING "MCAL Tresos project name")

#############################################################################################
# Static Configuration
#############################################################################################
set(SERIES cyt3dl)

set(HAVE_CM4                    OFF)     # Device has a Cortex-M4
set(HAVE_CM7_0                  ON)      # Device has a Cortex-M7 (instance #0)
set(HAVE_CM7_1                  OFF)     # Device has a Cortex-M7 (instance #1)
set(HAVE_CM7_2                  OFF)     # Device has a Cortex-M7 (instance #2)
set(HAVE_CM7_3                  OFF)     # Device has a Cortex-M7 (instance #3)
set(HAVE_IP_CXPI                ON)      # Device supports CXPI
set(HAVE_IP_SMIF                ON)      # Device supports SMIF
set(HAVE_IP_ETHERNET            ON)      # Device supports Ethernet
set(HAVE_IP_FLEXRAY             OFF)     # Device supports FlexRay
set(HAVE_IP_AUDIOSS             OFF)     # Device supports Audio Sub-System
set(HAVE_IP_SOUNDSS             ON)      # Device supports Sound Sub-System
set(HAVE_IP_SOUNDSS_DAC         ON)      # Device supports DAC in Sound Sub-System
set(HAVE_IP_SDHC                OFF)     # Device supports SDHC Host Controller
set(HAVE_IP_REGHC               OFF)     # Device supports REGHC IP for external power control
set(HAVE_IP_PMIC                ON)      # Device supports PMIC IP for external power control
set(HAVE_IP_MIPICSI2            ON)      # Device supports MIPI-CSI2 interface
set(HAVE_IP_FPDLINK             ON)      # Device supports FPD-Link interface
set(HAVE_IP_VIDEOSS             ON)      # Device supports Video Sub-System
set(HAVE_IP_AXIDMA              ON)      # Device supports AXI-DMA Controller
set(HAVE_IP_LPDDR4              OFF)     # Device supports LPDDR4 Controller
set(HAVE_IP_SEGLCD              OFF)     # Device supports Segment LCD Controller

set(HAVE_MW_AUDIO_AIC261        OFF)     # Middleware for AIC261 Audio IC
set(HAVE_MW_AUDIO_CS42448       ON)      # Middleware for CS42448 Audio IC
set(HAVE_MW_ETH_PHY_DP83848Q_Q1 ON)      # Middleware for DP83848Q-Q1 Ethernet PHY
set(HAVE_MW_ETH_PHY_DP83867     OFF)     # Middleware for DP83867 Ethernet PHY
set(HAVE_MW_GFX_ENV             ON)      # Middleware for "Graphics environment"
set(HAVE_MW_MIPI_SENSOR         ON)      # Middleware for "MIPI sensor"
set(HAVE_MW_POWER               ON)      # Middleware for "power"
set(HAVE_MW_SMIF_MEM            ON)      # Middleware for "SMIF memories"

set(SMIF_IP_VERSION             ver3_1)  # Uses SMIF.3_1

######################################################################################
# Infineon Auto Flash Utility / OpenOCD Configuration
######################################################################################
set(AFU_OOCD_TARGET_CFG_FILE traveo2_c2d_4m.cfg)  # Config name to be used for Infineon Auto Flash Utility / OpenOCD

######################################################################################
# Lauterbach Configuration
######################################################################################
set(T32_DEVICE_NAME CYT3DLA)   # Device name used by Lauterbach (without the CPU suffix)

######################################################################################
# IAR Debug Configuration
######################################################################################
set(IAR_DEVICE_NAME CYT3DL)   # Device name used by IAR (without the CPU suffix)
