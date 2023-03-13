#############################################################################################
# User Configuration
#############################################################################################
set(DEVICE CYT2B78CAE CACHE STRING "Device part number, typically CYT2B78CAE (for 176-QFP)")

set(MCAL_VAR_TRESOS_PROJECT_NAME TVII-B-E-1M_QFP-176 CACHE STRING "MCAL Tresos project name")

#############################################################################################
# Static Configuration
#############################################################################################
set(SERIES cyt2b7)

set(HAVE_CM4                    ON)      # Device has a Cortex-M4
set(HAVE_CM7_0                  OFF)     # Device has a Cortex-M7 (instance #0)
set(HAVE_CM7_1                  OFF)     # Device has a Cortex-M7 (instance #1)
set(HAVE_CM7_2                  OFF)     # Device has a Cortex-M7 (instance #2)
set(HAVE_CM7_3                  OFF)     # Device has a Cortex-M7 (instance #3)
set(HAVE_IP_CXPI                OFF)     # Device supports CXPI
set(HAVE_IP_SMIF                OFF)     # Device supports SMIF
set(HAVE_IP_ETHERNET            OFF)     # Device supports Ethernet
set(HAVE_IP_FLEXRAY             OFF)     # Device supports FlexRay
set(HAVE_IP_AUDIOSS             OFF)     # Device supports Audio Sub-System
set(HAVE_IP_SOUNDSS             OFF)     # Device supports Sound Sub-System
set(HAVE_IP_SOUNDSS_DAC         OFF)     # Device supports DAC in Sound Sub-System
set(HAVE_IP_SDHC                OFF)     # Device supports SDHC Host Controller
set(HAVE_IP_REGHC               OFF)     # Device supports REGHC IP for external power control
set(HAVE_IP_PMIC                OFF)     # Device supports PMIC IP for external power control
set(HAVE_IP_MIPICSI2            OFF)     # Device supports MIPI-CSI2 interface
set(HAVE_IP_FPDLINK             OFF)     # Device supports FPD-Link interface
set(HAVE_IP_VIDEOSS             OFF)     # Device supports Video Sub-System
set(HAVE_IP_AXIDMA              OFF)     # Device supports AXI-DMA Controller
set(HAVE_IP_LPDDR4              OFF)     # Device supports LPDDR4 Controller
set(HAVE_IP_SEGLCD              OFF)     # Device supports Segment LCD Controller

set(HAVE_MW_AUDIO_AIC261        OFF)     # Middleware for AIC261 Audio IC
set(HAVE_MW_AUDIO_CS42448       OFF)     # Middleware for CS42448 Audio IC
set(HAVE_MW_ETH_PHY_DP83848Q_Q1 OFF)     # Middleware for DP83848Q-Q1 Ethernet PHY
set(HAVE_MW_ETH_PHY_DP83867     OFF)     # Middleware for DP83867 Ethernet PHY
set(HAVE_MW_GFX_ENV             OFF)     # Middleware for "Graphics environment"
set(HAVE_MW_MIPI_SENSOR         OFF)     # Middleware for "MIPI sensor"
set(HAVE_MW_POWER               OFF)     # Middleware for "power"
set(HAVE_MW_SMIF_MEM            OFF)     # Middleware for "SMIF memories"

######################################################################################
# Infineon Auto Flash Utility / OpenOCD Configuration
######################################################################################
set(AFU_OOCD_TARGET_CFG_FILE traveo2_1m_a0.cfg)   # Config name to be used for Infineon Auto Flash Utility / OpenOCD

######################################################################################
# Lauterbach Configuration
######################################################################################
set(T32_DEVICE_NAME CYT2B78CA)   # Device name used by Lauterbach (without the CPU suffix)

######################################################################################
# IAR Debug Configuration
######################################################################################
set(IAR_DEVICE_NAME CYT2B7)   # Device name used by IAR (without the CPU suffix)
