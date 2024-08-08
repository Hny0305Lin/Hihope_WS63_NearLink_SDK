# Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.

## ATTENTION: api_inc.cmake is about external api header file's path. ##
## DON'T ADD OTHER VARS IN IT ##

set(LITEOS_LIBC_INCLUDE ${LITEOSTOPDIR}/platform/libsec/include)

# Exporting musl libc APIs
#
# Attention:
# './lib/libc/src/include' and 'lib/libc/src/internal' should not exported,
# while os would compile failed if those are not included which should be fix.
if(LOSCFG_LIB_LIBC)
    if(LOSCFG_ARCH_ARM_AARCH32)
        set(ARCH_LOCAL arm)
    elseif(LOSCFG_ARCH_ARM_AARCH64)
        set(ARCH_LOCAL aarch64)
    elseif(LOSCFG_ARCH_RISCV32)
        set(ARCH_LOCAL riscv32)
    endif()
    list(APPEND LITEOS_LIBC_INCLUDE
        ${LITEOS_LIBC_PATH}/arch/${ARCH_LOCAL}
        ${LITEOS_LIBC_PATH}/arch/generic
        ${LITEOSTOPDIR}/lib/liteos_libc/include
        ${LITEOS_LIBC_PATH}/include
    )
endif()

if(LOSCFG_LIB_ZLIB)
    set(LITEOS_ZLIB_INCLUDE ${LITEOS_ZLIB_PATH})
endif()

set(LITEOS_LIB_INCLUDE
    ${LITEOS_LIBC_INCLUDE} ${LITEOS_ZLIB_INCLUDE} ${LITEOS_COMPILER_GCC_INCLUDE}
)
