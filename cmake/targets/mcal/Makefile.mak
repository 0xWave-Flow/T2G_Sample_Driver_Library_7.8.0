#################################################################
# Remap some variables and generate new ones that are neeeded
#################################################################
TRESOS2_WORKSPACE_DIR   := $(TRESOS_WORKSPACE_DIR)
TRESOS2_PROJECT_NAME    := $(TRESOS_PROJECT_NAME)
TRESOS2_USER_OPTIONS    += -Dinfo=false
TRESOS2_USER_OPTIONS    += -data $(TRESOS2_WORKSPACE_DIR)
TRESOS2_BASE            := $(TRESOS_DIR)
SSC_ROOT                := $(TRESOS2_BASE)/plugins
TRESOS_CMD              := tresos_cmd.bat

PROJECT_ROOT            := $(TRESOS_PROJECT_DIR)
GENERATED_MAKE_CFG_FILE := $(PROJECT_ROOT)/output/generated/make/Make_cfg.mak

# Check the variable SSC_ROOT. Blanks are not allowed.
ifneq ($(word 2,$(SSC_ROOT)),)
$(error The variable SSC_ROOT=$(SSC_ROOT) is not valid. Blanks are not allowed in this variable!)
endif

#################################################################
# Enable the generic compiler plugin
#################################################################
COMPILER += compiler

#################################################################
# Generated (project dependent) configuration of Make plugin
#################################################################
-include $(GENERATED_MAKE_CFG_FILE)

#################################################################
# "CPU" type --> will be MXS40 (from Make_cfg.mak)
#################################################################
CPU ?= $(TARGET)

#################################################################
# Include global makefile
##################################################################
-include $(SSC_ROOT)/Make_$(Make_VARIANT)/make/global.mak

#################################################################
# Helper Functions / Variables
#################################################################
# Convert to uppercase, Usage: $(call uppercase,$(VAR))
uppercase = $(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst p,P,$(subst q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst w,W,$(subst x,X,$(subst y,Y,$(subst z,Z,$1))))))))))))))))))))))))))

# These are mandatory plugins or those without code/headers, the filter is used gefore generating defines CY_MCAL_MODULE_ENABLED_<Plugin>
PLUGINS_FILTER_LIST_FOR_COMPILER_DEFS := compiler tresos2 Base Make Platforms Resource

# These are plugins that do not have a corresponding XDM config file in the config subfolder
PLUGINS_FILTER_LIST_FOR_XDM_CFG_FILES := compiler tresos2 Base Make Platforms

# These are the toolchain specific include directories appended by the MCAL make files to CC_INCLUDE_PATH (ensure that TOOLPATH_COMPILER is not set)
# They will be filtered out because they are not needed
TOOLCHAIN_INCLUDE_FOLDERS_FILTER_LIST = \lib \ansi \inc\c \include\arm
CC_INCLUDE_PATH_WITHOUT_TOOLCHAIN = $(filter-out $(TOOLCHAIN_INCLUDE_FOLDERS_FILTER_LIST),$(CC_INCLUDE_PATH))

#################################################################
# Variables for all things that will be required by CMake
##################################################################

# MemMap plugin is a stub and therefore its include dir is not part of the "default" makefiles.
MEMMAP_INCLUDE_DIR              := $(TRESOS_DIR)/plugins/MemMap_TS_T40D13M0I0R0/include
# The regular include dirs of the Platforms plugin are part of the "default" makefiles, but Compiler_Cfg.h from the 'templates' folder is needed in addition
PLATFORMS_TEMPLATES_INCLUDE_DIR := $(TRESOS_DIR)/plugins/Platforms_$(Platforms_VARIANT)/templates


# General string modifications: Replace backshlashes with slashes, strip redundant whitespace, sort alphabetically
# For CMAKE_SOURCE_FILES we remove non *.c/*.s files and single slash entries which might have caused by wrong Makefiles from plugins that used a backslash as line separator but with
# subsequent whitespace which results in Make treating it as a regular string, e.g. refer $(Icu_OUTPUT_PATH)\src\Icu_PBCfg.c \ in Icu_TS_T40D13M1I0R0\make\Icu_rules.mak
# For CMAKE_COMPILER_DEFS we remove plugins that dont produce code or are mandatory anyway

CMAKE_XDM_CFG_FILES = $(sort $(strip $(subst \,/,$(foreach plugin,$(filter-out $(PLUGINS_FILTER_LIST_FOR_XDM_CFG_FILES),$(PLUGINS)), $(TRESOS_PROJECT_DIR)/config/$(plugin).xdm))))
CMAKE_COMPILER_DEFS = $(sort $(foreach plugin,$(filter-out $(PLUGINS_FILTER_LIST_FOR_COMPILER_DEFS),$(PLUGINS)), CY_MCAL_MODULE_ENABLED_$(call uppercase,$(plugin))))
CMAKE_ADDL_LIBS     = $(sort $(strip $(subst \,/,$(LIBRARIES_LINK_ONLY))))
CMAKE_INCLUDE_DIRS  = $(sort $(MEMMAP_INCLUDE_DIR) $(PLATFORMS_TEMPLATES_INCLUDE_DIR) $(strip $(subst \,/,$(CC_INCLUDE_PATH_WITHOUT_TOOLCHAIN))))
CMAKE_SOURCE_FILES  = $(sort $(strip $(filter %.c %.s,$(filter-out /,$(subst \,/,$(foreach lib,$(LIBRARIES_TO_BUILD),$($(lib)_FILES)))))))

# Only files that are generated
CMAKE_SOURCE_FILES_GENERATED = $(sort $(strip $(filter %.c %.s,$(filter-out /,$(subst \,/,$(foreach plugin,$(PLUGINS),$($(plugin)_GEN_FILES)))))))
# Only static files from the plugins folder
CMAKE_SOURCE_FILES_STATIC = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/,$(file)),$(file))))

# Split static source files into files of the individual plugins because echoing CMAKE_SOURCE_FILES later could otherwise exceed the Windows command line limit (8KB) if many plugins are enabled
CMAKE_SOURCE_FILES_STATIC_ADC    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Adc,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_CAN    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Can,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_CORTST = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/CorTst,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_DIO    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Dio,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_FEE    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Fee,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_FLS    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Fls,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_FLSTST = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/FlsTst,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_GPT    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Gpt,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_ICU    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Icu,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_LIN    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Lin,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_MCU    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Mcu,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_OCU    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Ocu,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_PORT   = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Port,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_PWM    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Pwm,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_RAMTST = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/RamTst,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_SPI    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Spi,$(file)),$(file))))
CMAKE_SOURCE_FILES_STATIC_WDG    = $(strip $(foreach file,$(CMAKE_SOURCE_FILES),$(if $(findstring /plugins/Wdg,$(file)),$(file))))
# Fallback case for unknown modules at the time of writing this Makefile
CMAKE_SOURCE_FILES_STATIC_UNKNOWN = $(strip $(filter-out $(CMAKE_SOURCE_FILES_STATIC_ADC) \
														 $(CMAKE_SOURCE_FILES_STATIC_CAN) \
														 $(CMAKE_SOURCE_FILES_STATIC_CORTST) \
														 $(CMAKE_SOURCE_FILES_STATIC_DIO) \
														 $(CMAKE_SOURCE_FILES_STATIC_FEE) \
														 $(CMAKE_SOURCE_FILES_STATIC_FLS) \
														 $(CMAKE_SOURCE_FILES_STATIC_FLSTST) \
														 $(CMAKE_SOURCE_FILES_STATIC_GPT) \
														 $(CMAKE_SOURCE_FILES_STATIC_ICU) \
														 $(CMAKE_SOURCE_FILES_STATIC_LIN) \
														 $(CMAKE_SOURCE_FILES_STATIC_MCU) \
														 $(CMAKE_SOURCE_FILES_STATIC_OCU) \
														 $(CMAKE_SOURCE_FILES_STATIC_PORT) \
														 $(CMAKE_SOURCE_FILES_STATIC_PWM) \
														 $(CMAKE_SOURCE_FILES_STATIC_RAMTST) \
														 $(CMAKE_SOURCE_FILES_STATIC_SPI) \
														 $(CMAKE_SOURCE_FILES_STATIC_WDG),$(CMAKE_SOURCE_FILES_STATIC)))

##################################################################
# Targets for SDL CMake integation
##################################################################

.PHONY: generate_mcal_files generate_cmake_file print_cmake_vars

generate_mcal_files:
	@echo ****************************************
ifeq ($(wildcard $(TRESOS2_WORKSPACE_DIR)/.metadata/.plugins/org.eclipse.core.resources/.projects/$(TRESOS2_PROJECT_NAME)/.),) # Skip import if already existing
	@echo Import project into workspace:
	@echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	-call $(TRESOS2_BASE)/bin/$(TRESOS_CMD) $(TRESOS2_USER_OPTIONS) importProject $(TRESOS_PROJECT_DIR)
	@echo:
	@echo:
endif
	@echo Generate files:
	@echo ~~~~~~~~~~~~~~~
	call $(TRESOS2_BASE)/bin/$(TRESOS_CMD) $(TRESOS2_USER_OPTIONS) make generate $(TRESOS2_PROJECT_NAME)
	@echo ****************************************


# generate_cmake_file:
# 	@echo:> $(GENERATED_CMAKE_FILE)
# 	@echo set(MCAL_CFG_DEPENDENT_XDM_CFG_FILES $(CMAKE_XDM_CFG_FILES))  >> $(GENERATED_CMAKE_FILE)
# 	@echo:>> $(GENERATED_CMAKE_FILE)
# 	@echo set(MCAL_CFG_DEPENDENT_COMPILER_DEFS $(CMAKE_COMPILER_DEFS))  >> $(GENERATED_CMAKE_FILE)
# 	@echo:>> $(GENERATED_CMAKE_FILE)
# 	@echo set(MCAL_CFG_DEPENDENT_INCLUDE_DIRS  $(CMAKE_INCLUDE_DIRS))   >> $(GENERATED_CMAKE_FILE)
# 	@echo:>> $(GENERATED_CMAKE_FILE)
# 	@echo set(MCAL_CFG_DEPENDENT_ADDL_LIBS     $(CMAKE_ADDL_LIBS))      >> $(GENERATED_CMAKE_FILE)
# 	@echo:>> $(GENERATED_CMAKE_FILE)
# 	@echo set(MCAL_CFG_DEPENDENT_SOURCE_FILES  $(CMAKE_SOURCE_FILES_GENERATED) $(CMAKE_SOURCE_FILES_STATIC))   >> $(GENERATED_CMAKE_FILE)
# 	@echo:>> $(GENERATED_CMAKE_FILE)

generate_cmake_file:
	@echo:> $(GENERATED_CMAKE_FILE)
	@echo set(MCAL_CFG_DEPENDENT_XDM_CFG_FILES $(CMAKE_XDM_CFG_FILES))  >> $(GENERATED_CMAKE_FILE)
	@echo:>> $(GENERATED_CMAKE_FILE)
	@echo set(MCAL_CFG_DEPENDENT_COMPILER_DEFS $(CMAKE_COMPILER_DEFS))  >> $(GENERATED_CMAKE_FILE)
	@echo:>> $(GENERATED_CMAKE_FILE)
	@echo set(MCAL_CFG_DEPENDENT_ADDL_LIBS     $(CMAKE_ADDL_LIBS))      >> $(GENERATED_CMAKE_FILE)
	@echo:>> $(GENERATED_CMAKE_FILE)
	@echo set(MCAL_CFG_DEPENDENT_INCLUDE_DIRS  $(CMAKE_INCLUDE_DIRS))   >> $(GENERATED_CMAKE_FILE)
	@echo:>> $(GENERATED_CMAKE_FILE)
	@echo set(MCAL_CFG_DEPENDENT_SOURCE_FILES >> $(GENERATED_CMAKE_FILE)
	@echo     $(CMAKE_SOURCE_FILES_GENERATED) >> $(GENERATED_CMAKE_FILE)
	$(if $(CMAKE_SOURCE_FILES_STATIC_ADC),    @echo     $(CMAKE_SOURCE_FILES_STATIC_ADC) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_CAN),    @echo     $(CMAKE_SOURCE_FILES_STATIC_CAN) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_CORTST), @echo     $(CMAKE_SOURCE_FILES_STATIC_CORTST) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_DIO),    @echo     $(CMAKE_SOURCE_FILES_STATIC_DIO) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_CAN),    @echo     $(CMAKE_SOURCE_FILES_STATIC_CAN) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_FEE),    @echo     $(CMAKE_SOURCE_FILES_STATIC_FEE) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_FLS),    @echo     $(CMAKE_SOURCE_FILES_STATIC_FLS) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_FLSTST), @echo     $(CMAKE_SOURCE_FILES_STATIC_FLSTST) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_GPT),    @echo     $(CMAKE_SOURCE_FILES_STATIC_GPT) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_ICU),    @echo     $(CMAKE_SOURCE_FILES_STATIC_ICU) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_LIN),    @echo     $(CMAKE_SOURCE_FILES_STATIC_LIN) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_MCU),    @echo     $(CMAKE_SOURCE_FILES_STATIC_MCU) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_OCU),    @echo     $(CMAKE_SOURCE_FILES_STATIC_OCU) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_PORT),   @echo     $(CMAKE_SOURCE_FILES_STATIC_PORT) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_PWM),    @echo     $(CMAKE_SOURCE_FILES_STATIC_PWM) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_RAMTST), @echo     $(CMAKE_SOURCE_FILES_STATIC_RAMTST) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_SPI),    @echo     $(CMAKE_SOURCE_FILES_STATIC_SPI) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_WDG),    @echo     $(CMAKE_SOURCE_FILES_STATIC_WDG) >> $(GENERATED_CMAKE_FILE))
	$(if $(CMAKE_SOURCE_FILES_STATIC_UNKNOWN),@echo     $(CMAKE_SOURCE_FILES_STATIC_UNKNOWN) >> $(GENERATED_CMAKE_FILE))
	@echo ) >> $(GENERATED_CMAKE_FILE)
	@echo:>> $(GENERATED_CMAKE_FILE)




print_cmake_vars:
	@echo ##########################################################################
	@echo Plugins enabled: $(sort $(strip $(PLUGINS)))
	@echo ##########################################################################
	@echo Resulting "libraries" to build: $(sort $(strip $(LIBRARIES_TO_BUILD)))
	@echo ##########################################################################
	@echo Additional libraries to link: $(CMAKE_ADDL_LIBS)
	@echo ################## XDM Cfg files #########################################
	@echo $(CMAKE_XDM_CFG_FILES)
	@echo ################## Compiler defines ######################################
	@echo $(CMAKE_COMPILER_DEFS)
	@echo ################## Include directories ###################################
	@echo $(CMAKE_INCLUDE_DIRS)
	@echo ################## All source files ######################################
	@echo $(CMAKE_SOURCE_FILES)
	@echo ################## All generated files ###################################
	@echo $(CMAKE_SOURCE_FILES_GENERATED)
	@echo ################## All static files ######################################
	@echo $(CMAKE_SOURCE_FILES_STATIC)
	@echo ##########################################################################


