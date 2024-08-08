## ATTENTION: api_inc.cmake is about external api header files' path, don't add other vars in it ##
set(LITEOS_KERNEL_INCLUDE
    ${LITEOSTOPDIR}/kernel/include
    ${LITEOSTOPDIR}/kernel/base/include # too many errors if deleted
)

set(LITEOS_EXTKERNEL_INCLUDE)
if (LOSCFG_KERNEL_EXTKERNEL)
    list(APPEND LITEOS_EXTKERNEL_INCLUDE
        ${LITEOSTOPDIR}/kernel/extended/include
    )
endif()
