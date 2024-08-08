# set syscalls.h include path for kernel space
list(APPEND MODULE_INCLUDE_PRI ${LITEOS_K_INCLUDES})
list(APPEND MODULE_CXXINCLUDE_PRI ${LITEOS_K_INCLUDES})

include(${MK_PATH}/module.cmake)