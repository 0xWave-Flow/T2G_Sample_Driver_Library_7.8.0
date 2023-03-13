function(create_libcrypto)
    
    set(libBaseName crypto)
    
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

    if(${ARG_CPU_TYPE} STREQUAL cm0plus)
        set(libFlavorName server)
    else()
        set(libFlavorName client)
    endif()

    set(libName ${libBaseName}_${libFlavorName}_${ARG_CPU_TYPE}_${TOOLCHAIN_ID})

    message(STATUS "Creating library: ${libName}")

    add_library(${libName} STATIC EXCLUDE_FROM_ALL)

    # Only route the "Release" config of the libs to the appropriate folder for SDL release
    set(libOutputDirectory ${PATH_DIE_SRC_DRIVERS}/crypto/libs)
    set_target_properties(${libName}
        PROPERTIES
            # ARCHIVE_OUTPUT_DIRECTORY_DEBUG           ${libOutputDirectory}
            ARCHIVE_OUTPUT_DIRECTORY_RELEASE         ${libOutputDirectory}
            # ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO  ${libOutputDirectory}
            # ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL      ${libOutputDirectory}
    )

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
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_core_aes.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_core_chacha.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_core_cmac.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_core_crc.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_core_des.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_core_hmac.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_core_prng.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_core_rsa.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_core_sha.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_core_trng.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_core_util.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_core_vu.c
            ${PATH_COMMON_SRC_DRIVERS}/crypto/src/cy_crypto_server.c
    )

endfunction()
