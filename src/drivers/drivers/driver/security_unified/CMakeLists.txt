#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "drv_security_unified")

if("romboot" IN_LIST APPLICATION)
    set(CRYPTO_BUILD_ENV "boot")
    set(SOURCES
        # ROM API
        ${CMAKE_CURRENT_SOURCE_DIR}/romable/hash_romable.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_rom_cipher.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/crypto_drv_common.c

        ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb/crypto_curve_param.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/ecc/drv_pke_ecc_verify.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecdsa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_pke_weak_func.c
    )
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_VERIFY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/ecc/drv_pke_ecc_verify.c")
    endif()

    set(PUBLIC_DEFINES
        # need to change to related to kconfig.
        -DCRYPTO_ERROR_ENV=6    # NOOS
        -DCRYPTO_ROM_ENABLE
    )
elseif (("flashboot" IN_LIST APPLICATION) OR ((${CHIP} STREQUAL "ws63") AND ("loaderboot" IN_LIST APPLICATION)))
    set(CRYPTO_BUILD_ENV "boot")
    set(SOURCES
        # flashboot API
        ${CMAKE_CURRENT_SOURCE_DIR}/romable/hash_romable.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_flashboot_cipher.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_flashboot_symc.c
        # ${CMAKE_CURRENT_SOURCE_DIR}/drv_flashboot_sample.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/crypto_drv_common.c

        ${CMAKE_CURRENT_SOURCE_DIR}/drv_km/drv_keyslot.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_km/drv_klad.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_fapc/drv_fapc.c

        ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb/crypto_curve_param.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_verify.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecdsa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_pke_weak_func.c
    )
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_VERIFY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_verify.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_dsa_hash.c")
    endif()
    # ROM API
    if(DEFINED CONFIG_FLASH_BOOT_SUPPORT_ROM_API)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/rom_drv_api/hal_rom_table.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/rom_drv_api/drv_rom_table.c")
        # this MICRO will only be defined in ws63 & ws53
        if(DEFINED CONFIG_ROM_EXPEND_SUPPORT_NOT_ALIGNED)
            list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_rom_cipher.c")
        endif()
        
    else()
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_rom_cipher.c")
    endif()

    set(PUBLIC_DEFINES
        # need to change to related to kconfig.
        -DCRYPTO_ERROR_ENV=6    # NOOS
        -DPKE_SM2_SUPPORT
    )
elseif("BUILD_APPLICATION_STANDARD" IN_LIST DEFINES)
    set(CRYPTO_BUILD_ENV "os")
    set(SOURCES
        # SYMC
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc_mac_hard.c

        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/crypto_drv_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_pbkdf2_hard.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_trng.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_fapc/drv_fapc.c

        ${CMAKE_CURRENT_SOURCE_DIR}/drv_km/drv_keyslot.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_km/drv_klad.c

        ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb/crypto_curve_param.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecdsa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_pke_weak_func.c

        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_hash_simple.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_init.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_pke.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_symc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_trng.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_km.c

        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sl_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/trng.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/cipher.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/pke.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/km.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/security_init.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/security_sha256.c

        ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt/mbedtls_harden_adapt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt/ecp_harden_impl.c

        ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb/os/crypto_drv_init.c
        ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb/os/crypto_drv_irq.c

        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecc_cal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_pke_ecc_cal.c
    )
    if(DEFINED CONFIG_PKE_SUPPORT_EDWARD)
        # ed25519 algorithm function realization.
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_eddsa.c")
    endif()
    if(${CHIP} STREQUAL "ws53")
        # for deep sleep feature.
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt/hash_harden_impl_short_term.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt/aes_harden_impl_short_term.c")
    else()
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt/hash_harden_impl_long_term.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt/aes_harden_impl_long_term.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_RSA)
        # rsa algorithm function realization.
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_cal.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_rsa.c")
    endif()
    # outter API
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_SIGN)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/ecc/drv_pke_ecc_sign.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_VERIFY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/ecc/drv_pke_ecc_verify.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_SIGN)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_sign.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_dsa_hash.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_VERIFY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_verify.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_dsa_hash.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_CRYPTO)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_crypto.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_ECDH)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecc_ecdh.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_GEN_KEY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecc_gen_key.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_BIG_NUMBER)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_cal.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_pke_cal.c")
    endif()

    set(PUBLIC_DEFINES
        -DSM4_CFB_OFB_SUPPORT
        -DCRYPTO_CTR_NON_ALIGN_SUPPORT
        -DCRYPTO_SYMC_ADDR_NOT_ALIGN_SUPPORT
        # -DCRYPTO_CCM_TRACE_ENABLE   # Debug for ccm trace
        # -DCRYPTO_DRV_FUNC_TRACE_ENABLE  # Debug for drv func trace
        # -DCONFIG_CRYPTO_PERF_STATISTICS
    )
    if("__LITEOS__" IN_LIST DEFINES)
        list(APPEND PUBLIC_DEFINES -DCRYPTO_ERROR_ENV=4)    # LiteOS
    elseif("__FREERTOS__" IN_LIST DEFINES)
        list(APPEND PUBLIC_DEFINES -DCRYPTO_ERROR_ENV=7)    # FreeRTOS
    elseif("__ALIOS__" IN_LIST DEFINES)
        list(APPEND PUBLIC_DEFINES -DCRYPTO_ERROR_ENV=8)    # alios
    endif()

elseif("UT_TEST" IN_LIST DEFINES)
    set(CRYPTO_BUILD_ENV "ut")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/crypto_drv_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_pbkdf2_hard.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc_mac_hard.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_trng.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_fapc/drv_fapc.c

        ${CMAKE_CURRENT_SOURCE_DIR}/drv_km/drv_keyslot.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_km/drv_klad.c

        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/ecc/drv_pke_ecc_sign.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/ecc/drv_pke_ecc_verify.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_sign.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_verify.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_dsa_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_crypto.c

        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_cal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecc_cal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecc_ecdh.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecc_gen_key.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecdsa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_eddsa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_rsa.c

        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_hash_simple.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_init.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_pke.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_pke_cal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_pke_ecc_cal.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_symc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_trng.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_km.c

        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sl_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/trng.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/cipher.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/pke.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/km.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/security_init.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/security_sha256.c

        ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb/ut/crypto_drv_init.c
        ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb/crypto_curve_param.c
    )

    set(PUBLIC_DEFINES
        -DSM4_CFB_OFB_SUPPORT
        # -DCRYPTO_SYMC_ADDR_NOT_ALIGN_SUPPORT
        # -DCRYPTO_CCM_TRACE_ENABLE   # Debug for ccm trace
        # -DCRYPTO_DRV_FUNC_TRACE_ENABLE  # Debug for drv func trace
        # -DCONFIG_CRYPTO_PERF_STATISTICS
    )

    list(APPEND PUBLIC_DEFINES -DCRYPTO_ERROR_ENV=1)    # Linux
elseif("FUZZ_TEST" IN_LIST DEFINES)
    set(CRYPTO_BUILD_ENV "ut")
    set(SOURCES
        # SYMC
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_symc_mac_hard.c

        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/crypto_drv_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_pbkdf2_hard.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/drv_trng.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_fapc/drv_fapc.c

        ${CMAKE_CURRENT_SOURCE_DIR}/drv_km/drv_keyslot.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_km/drv_klad.c

        ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb/crypto_curve_param.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecdsa.c
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_pke_weak_func.c

        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_hash.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_hash_simple.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_init.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_pke.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_symc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_trng.c
        ${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_km.c

        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/sl_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/trng.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/cipher.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/pke.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/km.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/security_init.c
        ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/security_sha256.c

        ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt/mbedtls_harden_adapt.c
        # ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt/hash_harden_impl_short_term.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt/hash_harden_impl_long_term.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt/aes_harden_impl_long_term.c
        # ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt/aes_harden_impl_short_term.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt/ecp_harden_impl.c

        ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb/ut/crypto_drv_init.c
    )
    if(DEFINED CONFIG_PKE_SUPPORT_EDWARD)
    # ed25519 algorithm function realization.
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_eddsa.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_RSA)
    # rsa algorithm function realization.
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_rsa.c")
    endif()
    # outter API
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_SIGN)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/ecc/drv_pke_ecc_sign.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_VERIFY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/ecc/drv_pke_ecc_verify.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_SIGN)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_sign.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_dsa_hash.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_VERIFY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_verify.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_dsa_hash.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_SM2_CRYPTO)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/sm2/drv_pke_sm2_crypto.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_ECDH)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecc_ecdh.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_GEN_KEY)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecc_gen_key.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_ECC_CAL)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_ecc_cal.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_pke_ecc_cal.c")
    endif()
    if(DEFINED CONFIG_PKE_SUPPORT_BIG_NUMBER)
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/drv_pke_cal.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/kapi/source/kapi_pke_cal.c")
    endif()

    set(PUBLIC_DEFINES
        -DSM4_CFB_OFB_SUPPORT
        -DCRYPTO_CTR_NON_ALIGN_SUPPORT
        -DCRYPTO_SYMC_ADDR_NOT_ALIGN_SUPPORT
    )

    list(APPEND PUBLIC_DEFINES -DCRYPTO_ERROR_ENV=1)    # Linux
endif()

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code/fmea
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_km/include
    ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb
    ${CMAKE_CURRENT_SOURCE_DIR}/rom_drv_api
    ${CMAKE_CURRENT_SOURCE_DIR}/romable
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_pke_v5/common
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_code
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_cipher/drv_include
    ${CMAKE_CURRENT_SOURCE_DIR}/kapi/include
    ${CMAKE_CURRENT_SOURCE_DIR}/init/cfbb/${CRYPTO_BUILD_ENV}
    ${CMAKE_CURRENT_SOURCE_DIR}/mbedtls_harden_adapt
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer
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
