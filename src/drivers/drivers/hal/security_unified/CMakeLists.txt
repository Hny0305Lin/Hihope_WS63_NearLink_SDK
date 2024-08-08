#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_security_unified")

if("romboot" IN_LIST APPLICATION)
    set(CRYPTO_BUILD_ENV "boot")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/cfbb/boot/crypto_osal_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/cfbb/crypto_security.c

        # pke
        # algorithm
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/drv_common_pke.c
        # default for all algorithm.
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_verify_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/ecc_data_ram_addr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/rom_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/hal_pke.c

        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/spacc/hal_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/trng/hal_trng.c
    )
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_VERIFY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_verify_common.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_verify.c")
    endif()
    set(PUBLIC_DEFINES
        # need to change to related to kconfig.
        -DCRYPTO_ERROR_ENV=6    # NOOS
        -DCRYPTO_CONFIG_ROMBOOT_ENV # for romboot
        -DCRYPTO_ROM_ENABLE
    )
elseif (("flashboot" IN_LIST APPLICATION) OR ((${CHIP} STREQUAL "ws63") AND ("loaderboot" IN_LIST APPLICATION)))
    set(CRYPTO_BUILD_ENV "boot")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_km/hal_rkp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_km/hal_keyslot.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_km/hal_klad.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_fapc/hal_fapc.c

        ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/cfbb/boot/crypto_osal_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/cfbb/crypto_security.c

        # pke
        # algorithm
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/drv_common_pke.c
        # default for all algorithm.
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_verify_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/ecc_data_ram_addr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/rom_lib.c
    )
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_VERIFY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_verify_common.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_sign_verify.c")
    endif()
    if(NOT DEFINED CONFIG_FLASH_BOOT_SUPPORT_ROM_API)
        # flash functions.
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/hal_pke.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/spacc/hal_hash.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/trng/hal_trng.c")
    endif()
    set(PUBLIC_DEFINES
        # need to change to related to kconfig.
        -DCRYPTO_ERROR_ENV=6    # NOOS
        -DCRYPTO_CONFIG_ROMBOOT_ENV # for romboot
    )
elseif("BUILD_APPLICATION_STANDARD" IN_LIST DEFINES)
    set(CRYPTO_BUILD_ENV "os")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/cfbb/os/crypto_osal_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/cfbb/crypto_security.c

        # algorithm
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/drv_common_pke.c
        # default for all algorithm.
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_verify_common.c

        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/ecc_data_ram_addr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/rom_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/hal_pke.c

        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/spacc/hal_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/spacc/hal_symc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/trng/hal_trng.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_fapc/hal_fapc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_km/hal_rkp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_km/hal_keyslot.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_km/hal_klad.c
    )
    # algorithm
    if(DEFINED CONFIG_PKE_SUPPORT_CURVE)
        # curve25519 algorithm function realization.
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/curve_data_ram_addr.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/curve_ec_fp.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_EDWARD)
        # ed25519 algorithm function realization.
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/ed_data_ram_addr.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ed_ec_fp.c")
    endif()
    if((DEFINED CONFIG_PKE_SUPPORT_RSA) OR (DEFINED CONFIG_PKE_SUPPORT_BIG_NUMBER))
        # rsa algorithm function realization.
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/rsa_data_ram_addr.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/rsa.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_SIGN)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_sign_verify.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_common.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_VERIFY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_verify_common.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_verify.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_SIGN)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_sign.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_sign_verify.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_common.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_VERIFY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_verify_common.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_sign_verify.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_CRYPTO)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_common.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_crypto.c")
    endif()
    if((DEFINED CONFIG_PKE_SUPPORT_ECC_ECDH) OR
        (DEFINED CONFIG_PKE_SUPPORT_ECC_GEN_KEY) OR
        (DEFINED CONFIG_PKE_SUPPORT_ECC_CAL))
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_common.c")
    endif()

    set(PUBLIC_DEFINES
        # -DCRYPTO_CCM_TRACE_ENABLE  # debug for ccm trace
        # -DCRYPTO_HAL_FUNC_TRACE_ENABLE  # Debug for hal func trace
    )
    if("USE_LITEOS" IN_LIST DEFINES)
        list(APPEND PUBLIC_DEFINES -DCRYPTO_ERROR_ENV=4)    # LiteOS
    elseif("__FREERTOS__" IN_LIST DEFINES)
        list(APPEND PUBLIC_DEFINES -DCRYPTO_ERROR_ENV=7)    # FreeRTOS
    elseif("__ALIOS__" IN_LIST DEFINES)
        list(APPEND PUBLIC_DEFINES -DCRYPTO_ERROR_ENV=8)    # alios
    endif()
elseif("UT_TEST" IN_LIST DEFINES)
    set(CRYPTO_BUILD_ENV "ut")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/cfbb/ut/crypto_osal_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/cfbb/crypto_security.c

        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/curve_ec_fp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/drv_common_pke.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_verify_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_verify_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_verify.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ed_ec_fp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/rsa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_crypto.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_sign_verify.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_sign.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/curve_data_ram_addr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/ecc_data_ram_addr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/ed_data_ram_addr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/rom_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/rsa_data_ram_addr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/hal_pke.c

        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/spacc/hal_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/spacc/hal_symc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/trng/hal_trng.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_fapc/hal_fapc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_km/hal_rkp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_km/hal_keyslot.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_km/hal_klad.c
    )
    set(PUBLIC_DEFINES
        # -DCRYPTO_CCM_TRACE_ENABLE  # debug for ccm trace
        # -DCRYPTO_HAL_FUNC_TRACE_ENABLE  # Debug for hal func trace
    )

    list(APPEND PUBLIC_DEFINES -DCRYPTO_ERROR_ENV=1) #Linux
elseif("FUZZ_TEST" IN_LIST DEFINES)
    set(CRYPTO_BUILD_ENV "fuzz")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/cfbb/fuzz/crypto_osal_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/cfbb/crypto_security.c

        # algorithm
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/drv_common_pke.c
        # default for all algorithm.
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_verify_common.c

        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/ecc_data_ram_addr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/rom_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/hal_pke.c

        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/spacc/hal_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/spacc/hal_symc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/trng/hal_trng.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_fapc/hal_fapc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_km/hal_rkp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_km/hal_keyslot.c
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_km/hal_klad.c
    )
    # algorithm
    if(DEFINED CONFIG_PKE_SUPPORT_CURVE)
        # curve25519 algorithm function realization.
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/curve_data_ram_addr.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/curve_ec_fp.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_EDWARD)
        # ed25519 algorithm function realization.
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/ed_data_ram_addr.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ed_ec_fp.c")
    endif()
    if((DEFINED CONFIG_PKE_SUPPORT_RSA) OR (DEFINED CONFIG_PKE_SUPPORT_BIG_NUMBER))
        # rsa algorithm function realization.
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/arch/rsa_data_ram_addr.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/rsa.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_SIGN)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_sign_verify.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_common.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_VERIFY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_verify_common.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_verify.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_SIGN)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_sign.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_sign_verify.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_common.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_VERIFY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_verify_common.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_sign_verify.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_CRYPTO)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_common.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/sm2_ec_fp_crypto.c")
    endif()
    if((DEFINED CONFIG_PKE_SUPPORT_ECC_ECDH) OR
        (DEFINED CONFIG_PKE_SUPPORT_ECC_GEN_KEY) OR
        (DEFINED CONFIG_PKE_SUPPORT_ECC_CAL))
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg/ecc_ec_fp_sign_common.c")
    endif()
    set(PUBLIC_DEFINES
        # -DCRYPTO_CCM_TRACE_ENABLE  # debug for ccm trace
        # -DCRYPTO_HAL_FUNC_TRACE_ENABLE  # Debug for hal func trace
    )

    list(APPEND PUBLIC_DEFINES -DCRYPTO_ERROR_ENV=1) #Linux
endif()

set(PRIVATE_HEADER
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/cfbb/${CRYPTO_BUILD_ENV}
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/include
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/include/common_include
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/include/hal_include
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke/alg
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/pke
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/spacc
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/trng
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_cipher/km
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_fapc
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_km
)

set(PRIVATE_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
