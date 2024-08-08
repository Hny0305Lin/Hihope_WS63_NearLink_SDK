## ATTENTION: api_inc.cmake is about external api header files' path, don't add other vars in it ##
if(LOSCFG_COMPAT_CMSIS)
    set(LITEOS_CMSIS_INCLUDE
        ${LITEOSTOPDIR}/compat/cmsis
        ${LITEOS_CMSIS_PATH}/CMSIS/RTOS/LiteOS/INC
        ${LITEOS_CMSIS_PATH}/CMSIS/RTOS2/Include
        ${LITEOS_CMSIS_PATH}/CMSIS/Core/Include
    )
endif()

if(LOSCFG_COMPAT_LINUX)
    set(LITEOS_LINUX_INCLUDE
        ${LITEOSTOPDIR}/compat/linux/include
        ${LITEOSTOPDIR}/${LITEOS_OPENSOURCE_PATHNAME}/freebsd/liteos/compat/linux/include
    )
endif()

set(LITEOS_NUTTX_INCLUDE
    ${LITEOSTOPDIR}/compat/nuttx/include
)

set(LITEOS_COMPAT_INCLUDE ${LITEOS_CMSIS_INCLUDE} ${LITEOS_LINUX_INCLUDE} ${LITEOS_NUTTX_INCLUDE})
