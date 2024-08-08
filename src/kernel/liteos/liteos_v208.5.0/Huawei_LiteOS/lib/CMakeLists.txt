
set(LIBC_DIR_PATH ../../open_source/musl)  # relative path due to makefile
set(MODULE_y)

list(APPEND MODULE_${LOSCFG_LIB_LIBSEC} libsec)

list(APPEND MODULE_${LOSCFG_LIB_LIBC} libc)

list(APPEND MODULE_${LOSCFG_LIB_LIBM} libm)

list(APPEND MODULE_${LOSCFG_LIB_ZLIB} zlib)

if(LOSCFG_ARCH_ARM_V7A OR LOSCFG_ARCH_ARM_V7R OR LOSCFG_ARCH_CORTEX_R52)
    list(APPEND MODULE_${LOSCFG_LIB_LIBC} liteos_libc/arch/${ARCH_LOCAL}/armv7)
elseif(LOSCFG_ARCH_RISCV32)
    list(APPEND MODULE_${LOSCFG_LIB_LIBC} liteos_libc/arch/${ARCH_LOCAL})
elseif(LOSCFG_ARCH_ARM_AARCH64)
    list(APPEND MODULE_${LOSCFG_LIB_LIBC} liteos_libc/arch/${ARCH_LOCAL}/generic)
endif()

FOREACH(CUR_MODULE_y ${MODULE_y})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${CUR_MODULE_y})
ENDFOREACH(CUR_MODULE_y)
