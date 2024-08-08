set(MODULE_NAME m)


# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)
set(MODULE_COPTS_PUB)
set(MODULE_ASOPTS_PUB)
set(MODULE_CXXOPTS_PUB)
set(MODULE_CMACRO_PUB)
set(MODULE_ASMACRO_PUB)
set(MODULE_CXXMACRO_PUB)


# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI
    ${LITEOS_LIBC_PATH}/src/include
    ${LITEOS_LIBC_PATH}/src/internal
)
set(MODULE_COPTS_PRI)
set(MODULE_ASOPTS_PRI)
set(MODULE_CXXOPTS_PRI)
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

list(APPEND MODULE_COPTS_PRI -Wno-parentheses -Wno-strict-prototypes)
if(LOSCFG_COMPILER_CLANG)
    list(APPEND MODULE_COPTS_PRI -Wno-uninitialized -Wno-ignored-pragmas -Wno-unused-but-set-variable)
else()
    list(APPEND MODULE_COPTS_PRI
         -Wno-maybe-uninitialized
         -Wno-unknown-pragmas
         -Wno-unused-but-set-variable
    )
endif()

# add srcs to private target #
aux_source_directory(${LIBC_DIR_PATH}/src/math LOCAL_SRCS_y)

# C++ library symbols in the compiler conflict with those in the LiteOS libm #
if(LOSCFG_COMPILER_ARM_NONE_EABI AND LOSCFG_KERNEL_CPPSUPPORT)
    list(REMOVE_ITEM LOCAL_SRCS_y ${LIBC_DIR_PATH}/src/math/frexpl.c ${LIBC_DIR_PATH}/src/math/ceill.c)
endif()

set(LOCAL_SRCS ${LOCAL_SRCS_y})
include(${MODULE_LIBC})
