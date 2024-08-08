include(../libc/libc_src.cmake)

set(MODULE_NAME posix)

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
    ${LITEOSTOPDIR}/lib/liteos_libc/pthread
    ${LITEOSTOPDIR}/lib/liteos_libc/errno
    ${LITEOSTOPDIR}/lib/liteos_libc/tzdst
    ${LITEOSTOPDIR}/lib/liteos_libc/time
    ${LITEOSTOPDIR}/lib/liteos_libc/lock
    ${LITEOS_LIBC_PATH}/src/include
    ${LITEOS_LIBC_PATH}/src/internal
    ${LITEOS_LIBC_PATH}/src/time
)
set(MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)


# add srcs to private target #
set(LOCAL_SRCS_y)
set(LOCAL_DIRS_y)

# liteos_libc path
set(LOCAL_DIRS_LITEOS_LIBC pthread mq sched misc ldso env)

# musl_libc path
set(LOCAL_SRCS_LIBC
    ${LIBC_PRNG_SRC} ${LIBC_LINUX_SRC}
    ${LIBC_THREAD_SRC}
)

# posix configured module path
list(APPEND LOCAL_DIRS_${LOSCFG_COMPAT_POSIX} ${LOCAL_DIRS_LITEOS_LIBC})
list(APPEND LOCAL_SRCS_${LOSCFG_COMPAT_POSIX} time/timer.c)
list(APPEND LOCAL_SRCS_${LOSCFG_COMPAT_POSIX} ${LOCAL_SRCS_LIBC})

FOREACH(CUR_DIR ${LOCAL_DIRS_y})
    aux_source_directory(${CUR_DIR} LOCAL_SRCS_y)
ENDFOREACH(CUR_DIR)

list(REMOVE_ITEM LOCAL_SRCS_y misc/getentropy.c)

if(LOSCFG_COMPILER_CLANG)
list(APPEND MODULE_COPTS_PRI -Wno-parentheses -Wno-strict-prototypes -Wno-array-parameter)
else()
list(APPEND MODULE_COPTS_PRI -Wno-parentheses -Wno-strict-prototypes)
endif()

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
include(${MODULE_LIBC})
