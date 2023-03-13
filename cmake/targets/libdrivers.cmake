function(create_libdrivers)

    set(libBaseName drivers)
    
    # Define the supported set of keywords
    set(prefix ARG)
    set(noValueArgs)
    set(singleValueArgs CPU_TYPE)
    set(multiValueArgs)

    cmake_parse_arguments(${prefix} "${noValueArgs}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    printFunctionArgs(${CMAKE_CURRENT_FUNCTION} "${noValueArgs}" "${singleValueArgs}" "${multiValueArgs}")

    # Check presence of mandatory arguments
    if(NOT DEFINED ARG_CPU_TYPE)
        message(FATAL_ERROR "CPU_TYPE is not specified!")
    endif()

    set(libName ${libBaseName}_${ARG_CPU_TYPE})
    
    message(STATUS "Creating library: ${libName}")

    add_library(${libName} STATIC)

    target_include_directories(${libName}
        PRIVATE
            ${INCLUDE_DIRS}
    )

    target_compile_definitions(${libName}
        PRIVATE
            ${COMPILER_DEFINES}
            ${COMPILER_DEFINES_${ARG_CPU_TYPE}}
    )

    target_compile_options(${libName}
        PRIVATE
            $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_OPTIONS}>
            $<$<COMPILE_LANGUAGE:C>:${C_COMPILER_OPTIONS_${ARG_CPU_TYPE}}>
            $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_OPTIONS}>
            $<$<COMPILE_LANGUAGE:CXX>:${CXX_COMPILER_OPTIONS_${ARG_CPU_TYPE}}>
            $<$<COMPILE_LANGUAGE:ASM>:${ASSEMBLER_OPTIONS}>        
            $<$<COMPILE_LANGUAGE:ASM>:${ASSEMBLER_OPTIONS_${ARG_CPU_TYPE}}>  
    )

    target_sources(${libName}
        PRIVATE
          #-------------------------------- COMMON FOLDER ---------------------------------------------
            ${PATH_COMMON_SRC_DRIVERS}/adc/cy_adc.c
            ${PATH_COMMON_SRC_DRIVERS}/adc/cy_adc.h
            ${PATH_COMMON_SRC_DRIVERS}/canfd/cy_canfd.c
            ${PATH_COMMON_SRC_DRIVERS}/canfd/cy_canfd.h
            ${PATH_COMMON_SRC_DRIVERS}/crypto/cy_crypto.h
            ${PATH_COMMON_SRC_DRIVERS}/crypto/cy_crypto_common.h
            ${PATH_COMMON_SRC_DRIVERS}/crypto/cy_crypto_config.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/cy_crypto_config.h
            ${PATH_COMMON_SRC_DRIVERS}/crypto/cy_crypto_server.h
            $<$<BOOL:${HAVE_IP_CXPI}>:${PATH_COMMON_SRC_DRIVERS}/cxpi/cy_cxpi.c>
            $<$<BOOL:${HAVE_IP_CXPI}>:${PATH_COMMON_SRC_DRIVERS}/cxpi/cy_cxpi.h>
            ${PATH_COMMON_SRC_DRIVERS}/dma/cy_mdma.c
            ${PATH_COMMON_SRC_DRIVERS}/dma/cy_mdma.h
            ${PATH_COMMON_SRC_DRIVERS}/dma/cy_pdma.c
            ${PATH_COMMON_SRC_DRIVERS}/dma/cy_pdma.h
            ${PATH_COMMON_SRC_DRIVERS}/evtgen/cy_evtgen.c
            ${PATH_COMMON_SRC_DRIVERS}/evtgen/cy_evtgen.h
            ${PATH_COMMON_SRC_DRIVERS}/flash/cy_flash.c
            ${PATH_COMMON_SRC_DRIVERS}/flash/cy_flash.h
            ${PATH_COMMON_SRC_DRIVERS}/gpio/cy_gpio.c
            ${PATH_COMMON_SRC_DRIVERS}/gpio/cy_gpio.h
            ${PATH_COMMON_SRC_DRIVERS}/ipc/cy_ipc_config.h
            ${PATH_COMMON_SRC_DRIVERS}/ipc/cy_ipc_drv.c
            ${PATH_COMMON_SRC_DRIVERS}/ipc/cy_ipc_drv.h
            ${PATH_COMMON_SRC_DRIVERS}/ipc/cy_ipc_pipe.c
            ${PATH_COMMON_SRC_DRIVERS}/ipc/cy_ipc_pipe.h
            ${PATH_COMMON_SRC_DRIVERS}/ipc/cy_ipc_sema.c
            ${PATH_COMMON_SRC_DRIVERS}/ipc/cy_ipc_sema.h
            ${PATH_COMMON_SRC_DRIVERS}/lin/cy_lin.c
            ${PATH_COMMON_SRC_DRIVERS}/lin/cy_lin.h
            ${PATH_COMMON_SRC_DRIVERS}/lvd/cy_lvd.c
            ${PATH_COMMON_SRC_DRIVERS}/lvd/cy_lvd.h
            ${PATH_COMMON_SRC_DRIVERS}/mcwdt/cy_mcwdt.c
            ${PATH_COMMON_SRC_DRIVERS}/mcwdt/cy_mcwdt.h
            ${PATH_COMMON_SRC_DRIVERS}/mpu/cy_mpu.c
            ${PATH_COMMON_SRC_DRIVERS}/mpu/cy_mpu.h
            ${PATH_COMMON_SRC_DRIVERS}/prot/cy_prot.c
            ${PATH_COMMON_SRC_DRIVERS}/prot/cy_prot.h
            ${PATH_COMMON_SRC_DRIVERS}/scb/cy_scb_common.c
            ${PATH_COMMON_SRC_DRIVERS}/scb/cy_scb_common.h
            ${PATH_COMMON_SRC_DRIVERS}/scb/cy_scb_ezi2c.c
            ${PATH_COMMON_SRC_DRIVERS}/scb/cy_scb_ezi2c.h
            ${PATH_COMMON_SRC_DRIVERS}/scb/cy_scb_i2c.c
            ${PATH_COMMON_SRC_DRIVERS}/scb/cy_scb_i2c.h
            ${PATH_COMMON_SRC_DRIVERS}/scb/cy_scb_spi.c
            ${PATH_COMMON_SRC_DRIVERS}/scb/cy_scb_spi.h
            ${PATH_COMMON_SRC_DRIVERS}/scb/cy_scb_uart.c
            ${PATH_COMMON_SRC_DRIVERS}/scb/cy_scb_uart.h
            ${PATH_COMMON_SRC_DRIVERS}/smartio/cy_smartio.c
            ${PATH_COMMON_SRC_DRIVERS}/smartio/cy_smartio.h
            $<$<BOOL:${HAVE_IP_SMIF}>:${PATH_COMMON_SRC_DRIVERS}/smif/${SMIF_IP_VERSION}/cy_smif_ver_specific.c>
            $<$<BOOL:${HAVE_IP_SMIF}>:${PATH_COMMON_SRC_DRIVERS}/smif/${SMIF_IP_VERSION}/cy_smif_ver_specific.h>
            $<$<BOOL:${HAVE_IP_SMIF}>:${PATH_COMMON_SRC_DRIVERS}/smif/common/cy_smif.c>
            $<$<BOOL:${HAVE_IP_SMIF}>:${PATH_COMMON_SRC_DRIVERS}/smif/common/cy_smif.h>
            $<$<BOOL:${HAVE_IP_SMIF}>:${PATH_COMMON_SRC_DRIVERS}/smif/common/cy_smif_memslot.c>
            $<$<BOOL:${HAVE_IP_SMIF}>:${PATH_COMMON_SRC_DRIVERS}/smif/common/cy_smif_memslot.h>
            ${PATH_COMMON_SRC_DRIVERS}/srom/cy_srom.c
            ${PATH_COMMON_SRC_DRIVERS}/srom/cy_srom.h
            ${PATH_COMMON_SRC_DRIVERS}/sysflt/cy_sysflt.c
            ${PATH_COMMON_SRC_DRIVERS}/sysflt/cy_sysflt.h
            ${PATH_COMMON_SRC_DRIVERS}/sysint/cy_sysint.c
            ${PATH_COMMON_SRC_DRIVERS}/sysint/cy_sysint.h
            ${PATH_COMMON_SRC_DRIVERS}/syslib/${TOOLCHAIN_ID}/cy_syslib_${TOOLCHAIN_ID}${TOOLCHAIN_ASM_EXT}
            $<$<BOOL:${GCC}>:${PATH_COMMON_SRC_DRIVERS}/syslib/gcc/cy_syslib_newlib_systemcalls.c> # GCC only: systemcall (stub) implementations for 'newlib'
            ${PATH_COMMON_SRC_DRIVERS}/syslib/cy_syslib.c
            ${PATH_COMMON_SRC_DRIVERS}/syslib/cy_syslib.h
            ${PATH_COMMON_SRC_DRIVERS}/syspm/cy_syspm.c
            ${PATH_COMMON_SRC_DRIVERS}/syspm/cy_syspm.h
            ${PATH_COMMON_SRC_DRIVERS}/sysreset/cy_sysreset.c
            ${PATH_COMMON_SRC_DRIVERS}/sysreset/cy_sysreset.h
            ${PATH_COMMON_SRC_DRIVERS}/sysrtc/cy_sysrtc.c
            ${PATH_COMMON_SRC_DRIVERS}/sysrtc/cy_sysrtc.h
            ${PATH_COMMON_SRC_DRIVERS}/systick/cy_systick.c
            ${PATH_COMMON_SRC_DRIVERS}/systick/cy_systick.h
            ${PATH_COMMON_SRC_DRIVERS}/syswdt/cy_syswdt.c
            ${PATH_COMMON_SRC_DRIVERS}/syswdt/cy_syswdt.h
            ${PATH_COMMON_SRC_DRIVERS}/tcpwm/cy_tcpwm.c
            ${PATH_COMMON_SRC_DRIVERS}/tcpwm/cy_tcpwm.h
            ${PATH_COMMON_SRC_DRIVERS}/tcpwm/cy_tcpwm_counter.c
            ${PATH_COMMON_SRC_DRIVERS}/tcpwm/cy_tcpwm_counter.h
            ${PATH_COMMON_SRC_DRIVERS}/tcpwm/cy_tcpwm_pwm.c
            ${PATH_COMMON_SRC_DRIVERS}/tcpwm/cy_tcpwm_pwm.h
            ${PATH_COMMON_SRC_DRIVERS}/tcpwm/cy_tcpwm_quaddec.c
            ${PATH_COMMON_SRC_DRIVERS}/tcpwm/cy_tcpwm_quaddec.h
            ${PATH_COMMON_SRC_DRIVERS}/tcpwm/cy_tcpwm_sr.c
            ${PATH_COMMON_SRC_DRIVERS}/tcpwm/cy_tcpwm_sr.h
            ${PATH_COMMON_SRC_DRIVERS}/trigmux/cy_trigmux.c
            ${PATH_COMMON_SRC_DRIVERS}/trigmux/cy_trigmux.h
          #-------------------------------- SERIES FOLDER ---------------------------------------------
            $<$<BOOL:${HAVE_IP_AUDIOSS}>:${PATH_DIE_SRC_DRIVERS}/audioss/cy_i2s.c>
            $<$<BOOL:${HAVE_IP_AUDIOSS}>:${PATH_DIE_SRC_DRIVERS}/audioss/cy_i2s.h>
            $<$<BOOL:${HAVE_IP_SOUNDSS_DAC}>:${PATH_DIE_SRC_DRIVERS}/audioss/dac/cy_dac.c>
            $<$<BOOL:${HAVE_IP_SOUNDSS_DAC}>:${PATH_DIE_SRC_DRIVERS}/audioss/dac/cy_dac.h>
            $<$<BOOL:${HAVE_IP_SOUNDSS}>:${PATH_DIE_SRC_DRIVERS}/audioss/i2s/cy_i2s.c>
            $<$<BOOL:${HAVE_IP_SOUNDSS}>:${PATH_DIE_SRC_DRIVERS}/audioss/i2s/cy_i2s.h>
            $<$<BOOL:${HAVE_IP_SOUNDSS}>:${PATH_DIE_SRC_DRIVERS}/audioss/mixer/cy_mixer.c>
            $<$<BOOL:${HAVE_IP_SOUNDSS}>:${PATH_DIE_SRC_DRIVERS}/audioss/mixer/cy_mixer.h>
            $<$<BOOL:${HAVE_IP_SOUNDSS}>:${PATH_DIE_SRC_DRIVERS}/audioss/pwm/cy_pwm.c>
            $<$<BOOL:${HAVE_IP_SOUNDSS}>:${PATH_DIE_SRC_DRIVERS}/audioss/pwm/cy_pwm.h>
            $<$<BOOL:${HAVE_IP_SOUNDSS}>:${PATH_DIE_SRC_DRIVERS}/audioss/sg/cy_sg.c>
            $<$<BOOL:${HAVE_IP_SOUNDSS}>:${PATH_DIE_SRC_DRIVERS}/audioss/sg/cy_sg.h>
            $<$<BOOL:${HAVE_IP_SOUNDSS}>:${PATH_DIE_SRC_DRIVERS}/audioss/tdm/cy_tdm.c>
            $<$<BOOL:${HAVE_IP_SOUNDSS}>:${PATH_DIE_SRC_DRIVERS}/audioss/tdm/cy_tdm.h>
            $<$<BOOL:${HAVE_IP_AXIDMA}>:${PATH_DIE_SRC_DRIVERS}/axidma/cy_axidma.c>
            $<$<BOOL:${HAVE_IP_AXIDMA}>:${PATH_DIE_SRC_DRIVERS}/axidma/cy_axidma.h>
            ${PATH_DIE_SRC_DRIVERS}/cpu/cy_cpu.c
            ${PATH_DIE_SRC_DRIVERS}/cpu/cy_cpu.h
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/cdn_errno.h>
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/cdn_stdint.h>
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/cedi.h>
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/cps_v2.h>
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/cy_ethif.c>
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/cy_ethif.h>
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/edd.c>
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/edd_int.h>
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/edd_rx.c>
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/edd_tx.c>
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/emac_regs.h>
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/emac_regs_macro.h>
            $<$<BOOL:${HAVE_IP_ETHERNET}>:${PATH_DIE_SRC_DRIVERS}/ethernet/log.h>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/api/cy_fr_api_control_service.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/api/cy_fr_api_init.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/api/cy_fr_api_interrupt_service.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/api/cy_fr_api_rx_handler.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/api/cy_fr_api_status_service.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/api/cy_fr_api_time_service.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/api/cy_fr_api_timer_service.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/api/cy_fr_api_tx_handler.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/ccal/cy_fr_ccal_control_service.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/ccal/cy_fr_ccal_init.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/ccal/cy_fr_ccal_interrupt_service.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/ccal/cy_fr_ccal_rx_handler.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/ccal/cy_fr_ccal_status_service.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/ccal/cy_fr_ccal_time_service.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/ccal/cy_fr_ccal_timer_service.c>
            $<$<BOOL:${HAVE_IP_FLEXRAY}>:${PATH_DIE_SRC_DRIVERS}/flexray/ccal/cy_fr_ccal_tx_handler.c>
            $<$<BOOL:${HAVE_IP_FPDLINK}>:${PATH_DIE_SRC_DRIVERS}/fpdlink/cy_fpdlink.c>
            $<$<BOOL:${HAVE_IP_FPDLINK}>:${PATH_DIE_SRC_DRIVERS}/fpdlink/cy_fpdlink.h>
            $<$<BOOL:${HAVE_IP_LPDDR4}>:${PATH_DIE_SRC_DRIVERS}/lpddr4/cy_lpddr4.c>
            $<$<BOOL:${HAVE_IP_LPDDR4}>:${PATH_DIE_SRC_DRIVERS}/lpddr4/cy_lpddr4.h>
            $<$<BOOL:${HAVE_IP_MIPICSI2}>:${PATH_DIE_SRC_DRIVERS}/mipicsi2/cy_mipicsi2.c>
            $<$<BOOL:${HAVE_IP_MIPICSI2}>:${PATH_DIE_SRC_DRIVERS}/mipicsi2/cy_mipicsi2.h>
            $<$<BOOL:${HAVE_IP_SEGLCD}>:${PATH_DIE_SRC_DRIVERS}/seglcd/cy_seglcd.c>
            $<$<BOOL:${HAVE_IP_SEGLCD}>:${PATH_DIE_SRC_DRIVERS}/seglcd/cy_seglcd.h>
            ${PATH_DIE_SRC_DRIVERS}/sysclk/cy_sysclk.c
            ${PATH_DIE_SRC_DRIVERS}/sysclk/cy_sysclk.h
            $<$<BOOL:${HAVE_IP_SDHC}>:${PATH_DIE_SRC_DRIVERS}/sd_host/cy_sd_host.c>
            $<$<BOOL:${HAVE_IP_SDHC}>:${PATH_DIE_SRC_DRIVERS}/sd_host/cy_sd_host.h>
            $<$<BOOL:${HAVE_IP_PMIC}>:${PATH_DIE_SRC_DRIVERS}/syspmic/cy_syspmic.c>
            $<$<BOOL:${HAVE_IP_PMIC}>:${PATH_DIE_SRC_DRIVERS}/syspmic/cy_syspmic.h>
            $<$<BOOL:${HAVE_IP_REGHC}>:${PATH_DIE_SRC_DRIVERS}/sysreghc/cy_sysreghc.c>
            $<$<BOOL:${HAVE_IP_REGHC}>:${PATH_DIE_SRC_DRIVERS}/sysreghc/cy_sysreghc.h>
    )

endfunction()
