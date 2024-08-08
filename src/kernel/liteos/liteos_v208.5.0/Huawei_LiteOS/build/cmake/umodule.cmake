# set syscalls.h include path for user space
list(APPEND MODULE_INCLUDE_PRI ${LITEOS_U_INCLUDES})
list(APPEND MODULE_CXXINCLUDE_PRI ${LITEOS_U_INCLUDES})

include(${MK_PATH}/module.cmake)