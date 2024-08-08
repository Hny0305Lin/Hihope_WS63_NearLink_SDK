# Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.

## ATTENTION: api_inc.cmake is about external api header files' path. ##
## DON'T ADD OTHER VARS IN IT ##

if(LOSCFG_ARCH_ARM_AARCH32)
    if(LOSCFG_ARCH_ARM_CORTEX_M)
        set(LITEOS_ARCH_INCLUDE ${LITEOSTOPDIR}/arch/arm/cortex_m/include)
        if(LOSCFG_TRUSTZONE)
            list(APPEND LITEOS_ARCH_INCLUDE ${LITEOSTOPDIR}/arch/arm/cortex_m/include/secure)
        endif()
    else()
        set(LITEOS_ARCH_INCLUDE ${LITEOSTOPDIR}/arch/arm/cortex_a_r/include)
    endif()
endif()

if(LOSCFG_ARCH_ARM_AARCH64)
    set(LITEOS_ARCH_INCLUDE ${LITEOSTOPDIR}/arch/arm64/include)
endif()

if(LOSCFG_ARCH_MMU_ENABLE)
    list(APPEND LITEOS_ARCH_INCLUDE ${LITEOSTOPDIR}/arch/common/mmu/include)
endif()

if(LOSCFG_ARCH_XTENSA)
    if(LOSCFG_ARCH_XTENSA_XEA2)
        set(LITEOS_ARCH_INCLUDE ${LITEOSTOPDIR}/arch/xtensa/xea2/include)
    elseif(LOSCFG_ARCH_XTENSA_XEA3)
        set(LITEOS_ARCH_INCLUDE ${LITEOSTOPDIR}/arch/xtensa/xea3/include)
    endif()
endif()

if(LOSCFG_ARCH_RISCV32)
    set(LITEOS_ARCH_INCLUDE ${LITEOSTOPDIR}/arch/riscv/include)
endif()

if(LOSCFG_ARCH_LINGLONG)
    set(LITEOS_ARCH_INCLUDE ${LITEOSTOPDIR}/arch/linglong/include)
endif()
