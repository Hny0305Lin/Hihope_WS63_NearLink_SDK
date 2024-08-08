#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hitls")

set(SOURCES
    ${ROOT_DIR}/open_source/HiTLS/crypto/bn/src/bn_basic.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/bn/src/bn_bincal.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/bn/src/bn_operation.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/bn/src/bn_optimizer.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/bn/src/bn_ucal.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/bn/src/bn_utils.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/bn/src/bn_mont.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/bn/src/noasm_bn_mont.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/bn/src/bn_comba.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/eal/src/eal_bn.c
    ${ROOT_DIR}/open_source/HiTLS/bsl/sal/src/sal_mem.c
    ${ROOT_DIR}/open_source/HiTLS/bsl/log/src/log.c
    ${ROOT_DIR}/open_source/HiTLS/bsl/err/src/err.c
    ${ROOT_DIR}/open_source/HiTLS/bsl/sal/src/sal_threadlock.c
    ${ROOT_DIR}/open_source/HiTLS/bsl/err/src/avl.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/sm3/src/sm3_public.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/sha2/src/sha2_256.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/sha2/src/sha2_512.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/sha2/src/noasm_sha256.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/cmac/src/cmac.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/aes/src/crypt_aes.c
)

set(PUBLIC_HEADER
    ${ROOT_DIR}/open_source/HiTLS/crypto/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/bn/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/bn/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/bn/src
    ${ROOT_DIR}/open_source/HiTLS/include/crypto
    ${ROOT_DIR}/open_source/HiTLS/include/bsl
    ${ROOT_DIR}/open_source/HiTLS/include/tls
    ${ROOT_DIR}/open_source/HiTLS/bsl/sal/include
    ${ROOT_DIR}/open_source/HiTLS/bsl/sal/src
    ${ROOT_DIR}/open_source/HiTLS/bsl/log/include
    ${ROOT_DIR}/open_source/HiTLS/bsl/err/include
    ${ROOT_DIR}/open_source/HiTLS/bsl/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/sm3/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/sha1/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/sha2/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/sha3/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/md5/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/md5_sha1/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/cmac/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/aes/include
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    -DHITLS_THIRTY_TWO_BITS
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