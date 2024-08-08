## global prop define ##
set_property(GLOBAL PROPERTY LITEOS_MODULE_LIBS_PATH_PROP)

## var define ##
set(LITEOS_CMACRO)
set(LITEOS_ASMACRO)
set(LITEOS_CXXMACRO)
set(LITEOS_CXXINCLUDE)
set(LITEOS_DEP_LIBS_EXT)
set(LITEOS_U_INCLUDES)
set(LITEOS_K_INCLUDES)

## opensource path ##
set(LITEOS_OPENSOURCE_PATHNAME open_source)
set(LITEOS_ZLIB_PATH ${LITEOSTOPDIR}/${LITEOS_OPENSOURCE_PATHNAME}/zlib)
set(LITEOS_LIBC_PATH ${LITEOSTOPDIR}/${LITEOS_OPENSOURCE_PATHNAME}/musl)
set(LITEOS_CMSIS_PATH ${LITEOSTOPDIR}/${LITEOS_OPENSOURCE_PATHNAME}/CMSIS)

## LiteOS self-developed code compiler options ##
## please do not arbitrarily add -Wno-xxx suppression here ##
set(LITEOS_NEEDED_COMPILE_OPTS -Wextra -Wno-unused-parameter -Wno-missing-field-initializers)
set(LITEOS_SUGGESTED_COMPILE_OPTS -Wunused -Wswitch-default)
if(NOT LOSCFG_COMPILER_XTENSA_32)
    list(APPEND LITEOS_SUGGESTED_COMPILE_OPTS -Wvla -Wdate-time)
endif()

set(LITEOS_NON_SECURE_LOCAL_OPTS ${LITEOS_NEEDED_COMPILE_OPTS} ${LITEOS_SUGGESTED_COMPILE_OPTS})

############### configuring compiler ###############
# Introduced compiler binaries which might different from gcc compilers.
# Those compilers need to implement like ${CC} ${GPP} like gcc does.
# Meanwhile Introduced compiler includes and lib paths
# Export : ${LITEOS_COMPILER_GCC_INCLUDE}
#          ${LITEOS_CXXINCLUDE}
#          ${LITEOS_COMPILER_GCCLIB_PATH}
#          ${LITEOS_COMPILER_CXXLIB_PATH}

if(LOSCFG_SAFETY_KERNEL)
    set(KERNEL_PATH kernel_s)
    set(ARCH_PATH arch_s)
else()
    set(KERNEL_PATH kernel)
    set(ARCH_PATH arch)
endif()

if(LOSCFG_COMPILER_GCC)
    # Supported GCC Compilers
    # Also support "make CROSS_COMPILE=" to use third party compilers.
    include(${LITEOS_TOP_DIR}/build/cmake/compiler/compiler_gcc_common.cmake)
elseif(LOSCFG_COMPILER_XTENSA_32)
    include(${LITEOSTOPDIR}/build/cmake/compiler/compiler_xcc_common.cmake)
elseif(LOSCFG_COMPILER_CLANG)
    include(${LITEOSTOPDIR}/build/cmake/compiler/compiler_clang_common.cmake)
endif()

## variable define ##
set(CUR_OS liteos)
set(PYTHON python -B)
set(PYTHON3 python3 -B)
set(OUT ${CMAKE_CURRENT_BINARY_DIR})
set(BUILD ${OUT}/obj)
set(MK_PATH ${LITEOSTOPDIR}/build/cmake)
set(LITEOS_SCRIPTPATH ${LITEOSTOPDIR}/build/scripts)
set(LITEOS_LIB_BIGODIR ${OUT}/lib/obj)
set(LOSCFG_ENTRY_SRC ${LITEOSTOPDIR}/${KERNEL_PATH}/init/los_init.c)
set(BOARD_LD_INCULDE_PATH)
set(BOARD_LD_OUTPUT_PATH ${OUT}/Huawei_LiteOS/targets)
set(BOARD_LD_FILE ${BOARD_LD_OUTPUT_PATH}/board.ld)

set(BOARD_LD_S_FILE ${LITEOSTOPDIR}/targets/${LITEOS_PLATFORM}/board.ld.S)

### include variable
set(MODULE ${MK_PATH}/kmodule.cmake)
set(MODULE_LIBC ${MK_PATH}/module_libc.cmake)
set(UMODULE ${MK_PATH}/umodule.cmake)

if(LOSCFG_COMPILER_HIMIX_32)
    list(APPEND LITEOS_CMACRO __COMPILER_HUAWEILITEOS__)
elseif(LOSCFG_COMPILER_HIMIX210_64)
    list(APPEND LITEOS_CMACRO __COMPILER_HUAWEILITEOS__)
elseif(LOSCFG_COMPILER_HCC_64)
    list(APPEND LITEOS_CMACRO __COMPILER_HUAWEILITEOS__)
elseif(LOSCFG_COMPILER_GCC_64)
    list(APPEND LITEOS_CMACRO __COMPILER_HUAWEILITEOS__)
endif()

list(APPEND LITEOS_CMACRO __LITEOS__ SECUREC_IN_KERNEL=0 _ALL_SOURCE)
if(NOT LOSCFG_DEBUG_VERSION)
list(APPEND LITEOS_CMACRO NDEBUG)
endif()

# workround : the linker may use the math functions in libstdc++. put the libm in the front
#             to make sure the math functions in the libm will be used.
if(LOSCFG_LIB_LIBM)
    list(APPEND LITEOS_EXT_AHEAD_LIBS m)
endif()

if(LOSCFG_COMPILER_CLANG)
    if(LOSCFG_COMPILER_LINGLONG)
        list(APPEND LITEOS_DEP_LIBS_EXT clang_rt.builtins)
    elseif(LOSCFG_COMPILER_ARM_NONE_EABI_CLANG)
        list(APPEND LITEOS_DEP_LIBS_EXT clang_rt.builtins-arm)
    elseif(LOSCFG_COMPILER_RISCV_CLANG_UNKNOWN OR LOSCFG_COMPILER_RISCV_CLANG_MUSL)
        list(APPEND LITEOS_DEP_LIBS_EXT clang_rt.builtins-riscv32)
    endif()
elseif(LOSCFG_USING_GCC_NOPIC_LIB)
    list(APPEND LITEOS_DEP_LIBS_EXT gcc-nopic)
else()
    list(APPEND LITEOS_DEP_LIBS_EXT gcc)
endif()

if(NOT LOSCFG_LIB_LIBC)
    list(APPEND LITEOS_DEP_LIBS_EXT c)
endif()

if(NOT LOSCFG_COMPILER_ARM_NONE_EABI AND
   NOT LOSCFG_COMPILER_RISCV_GCC_UNKNOWN AND
   NOT LOSCFG_COMPILER_XTENSA_32 AND
   NOT LOSCFG_COMPILER_CLANG)
    list(APPEND LITEOS_DEP_LIBS_EXT gcc_eh)
endif()
set(AS_OBJS_LIBC_FLAGS __ASSEMBLY__ CACHE STRING "liteos asm flags" FORCE)

set(WARNING_AS_ERROR -Wall -Werror)
if(LOSCFG_COMPILER_RISCV_CLANG_UNKNOWN OR LOSCFG_COMPILER_RISCV_CLANG_MUSL OR LOSCFG_COMPILER_LINGLONG)
    list(APPEND WARNING_AS_ERROR -Wno-tautological-constant-out-of-range-compare -Wno-parentheses-equality -Wno-string-plus-int)
endif()

set(LIBC_FILTER_OPTS -Wfloat-equal)
## ATTENTION: api_inc.cmake is about external api header files' path, don't add other vars in it ##
if(LOSCFG_KERNEL_CPPSUPPORT)
    # origin: lib/api.mk
    list(APPEND LITEOS_DEP_LIBS_EXT supc++ stdc++)
endif()

set(ALL_INC_FILE)
if(LOSCFG_KERNEL_DYNLOAD)
    list(APPEND ALL_INC_FILE ${OUT}/dynload_dir/build/dynload_ld.cmake)
endif()
list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/${ARCH_PATH}/api_inc.cmake)
list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/${KERNEL_PATH}/api_inc.cmake)
list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/lib/api_inc.cmake)
list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/drivers/api_inc.cmake)
list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/compat/api_inc.cmake)
list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/fs/api_inc.cmake)
list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/shell/api_inc.cmake)
list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/net/api_inc.cmake)
list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/security/api_inc.cmake)
FOREACH(FILE ${ALL_INC_FILE})
    if(EXISTS ${FILE})
        include(${FILE})
    endif()
ENDFOREACH(FILE)

# the following vars will be used by submodules CMakeLists.txt, so defined here
set(LOS_PUB_INTF_SUFFIX _intf_pub)
set(LOS_PRI_INTF_SUFFIX _intf_pri)
set(LOS_CC_PROP_INTF_PUB "LiteOS${LOS_PUB_INTF_SUFFIX}")
set(VENDOR_CC_PROP_INTF_PUB "Vendor${LOS_PUB_INTF_SUFFIX}")

# Export APIs from these module.
add_subdirectory(${LITEOSTOPDIR}/${ARCH_PATH} Huawei_LiteOS/${ARCH_PATH})
add_subdirectory(${LITEOSTOPDIR}/targets Huawei_LiteOS/targets)

add_subdirectory(${LITEOSTOPDIR}/${KERNEL_PATH} Huawei_LiteOS/${KERNEL_PATH})
add_subdirectory(${LITEOSTOPDIR}/lib Huawei_LiteOS/lib)
add_subdirectory(${LITEOSTOPDIR}/drivers Huawei_LiteOS/drivers)

if(EXISTS ${LITEOSTOPDIR}/shell/CMakeLists.txt)
    add_subdirectory(${LITEOSTOPDIR}/shell Huawei_LiteOS/shell)
endif()

if(NOT LOSCFG_SAFETY_KERNEL)
if(EXISTS ${LITEOSTOPDIR}/compat/CMakeLists.txt)
    add_subdirectory(${LITEOSTOPDIR}/compat Huawei_LiteOS/compat)
endif()

if(EXISTS ${LITEOSTOPDIR}/fs/CMakeLists.txt)
    add_subdirectory(${LITEOSTOPDIR}/fs Huawei_LiteOS/fs)
endif()

if(EXISTS ${LITEOSTOPDIR}/net/CMakeLists.txt)
    add_subdirectory(${LITEOSTOPDIR}/net Huawei_LiteOS/net)
endif()

if(EXISTS ${LITEOSTOPDIR}/security/CMakeLists.txt)
    add_subdirectory(${LITEOSTOPDIR}/security Huawei_LiteOS/security)
endif()

if(EXISTS ${LITEOSTOPDIR}/components/CMakeLists.txt)
    add_subdirectory(${LITEOSTOPDIR}/components Huawei_LiteOS/components)
endif()


endif()



############################# Tools && Debug Option Begin ##############################
if(LOSCFG_3RDPARTY)
    add_subdirectory(${LITEOSTOPDIR}/3rdParty Huawei_LiteOS/3rdParty)
endif()

if(LOSCFG_TEST)
    include(${LITEOSTOPDIR}/test/test.cmake)
    add_subdirectory(${LITEOSTOPDIR}/test Huawei_LiteOS/test)
endif()

if(LOSCFG_NULL_ADDRESS_PROTECT)
    list(APPEND LITEOS_CMACRO LOSCFG_NULL_ADDRESS_PROTECT)
endif()
############################# Tools && Debug Option End #################################


## Optimize Options
if(LOSCFG_COMPILER_OPTIMIZE_NONE)
    set(LITEOS_COPTS_OPTIMIZE -O0)
elseif(LOSCFG_COMPILER_OPTIMIZE_SPEED)
    set(LITEOS_COPTS_OPTIMIZE -O2)
elseif(LOSCFG_COMPILER_OPTIMIZE_SIZE)
    set(LITEOS_COPTS_OPTIMIZE -Os)
endif()

if(LOSCFG_COMPILE_DEBUG)
    set(LITEOS_COPTS_OPTION -g -gdwarf-2)
endif()

set(LITEOS_COPTS_DEBUG ${LITEOS_COPTS_OPTION} ${LITEOS_COPTS_OPTIMIZE})
set(LITEOS_CXXOPTS_BASE ${LITEOS_COPTS_OPTION} ${LITEOS_COPTS_OPTIMIZE})
set(LITEOS_ASOPTS ${LITEOS_COPTS_OPTION})

## Security Options
if(LOSCFG_CC_STACKPROTECTOR)
    set(LITEOS_SSP -fstack-protector --param ssp-buffer-size=4)
elseif(LOSCFG_CC_STACKPROTECTOR_STRONG)
    set(LITEOS_SSP -fstack-protector-strong)
elseif(LOSCFG_CC_STACKPROTECTOR_ALL)
    set(LITEOS_SSP -fstack-protector-all)
endif()

# ASLR Options
if(LOSCFG_LASLR_GOT)
    set(LITEOS_ASLR_OPTS -fpie)
    set(LITEOS_ASLR_LD_OPTS -pie -z now)
elseif(LOSCFG_LASLR_GSPR)
    set(LITEOS_ASLR_LD_OPTS --emit-relocs)
endif()


set(LITEOS_COMMON_OPTS)
if(NOT LOSCFG_LASLR)
    list(APPEND LITEOS_COMMON_OPTS -fno-pic)
endif()

list(APPEND LITEOS_COMMON_OPTS
            -fno-builtin -funsigned-char
            -ffunction-sections -fdata-sections
            ${WARNING_AS_ERROR} ${LITEOS_SSP} ${LITEOS_ASLR_OPTS}
)

if(LOSCFG_ARCH_XTENSA_XEA3)
    # xea2 don't support shadow due to libc in compiler
    list(APPEND LITEOS_COMMON_OPTS -Wshadow)
endif()

list(APPEND LITEOS_COMMON_OPTS -Wformat=2)
if((NOT LOSCFG_COMPILER_CLANG) AND (NOT LOSCFG_COMPILER_XTENSA_32))
    list(APPEND LITEOS_COMMON_OPTS -freg-struct-return -Wtrampolines)
endif()

if(LOSCFG_LIB_LIBC)
    list(APPEND LITEOS_COMMON_OPTS -nostdinc -nostdlib)
endif()

set(LITEOS_COPTS_BASE ${LITEOS_COMMON_OPTS})

# Anonymous structs and unions are supported on c11; while gcc supports those features as extension
# which is turn on for default. As for Clang, if choose c99, -std=gnu99 should be used.
if(LOSCFG_COMPILER_CLANG)
    if (LOSCFG_COMPILER_LINGLONG)
        list(APPEND LITEOS_COPTS_BASE -std=c99)
    else()
        list(APPEND LITEOS_COPTS_BASE -std=c11)
    endif()
elseif(LOSCFG_COMPILER_XTENSA_32)
    list(APPEND LITEOS_COPTS_BASE -std=gnu99)
else()
    list(APPEND LITEOS_COPTS_BASE -std=c99)
endif()


# -Wpointer-arith will treat the size of a void or of a function as 1.
# -Wstrict-prototypes will warn if a function is defined without specifying the argument types.
# -pipe will use pipes, save compilation time
list(APPEND LITEOS_COPTS_BASE -Wpointer-arith -Wstrict-prototypes -fno-exceptions -pipe -fno-common)
if(LOSCFG_COMPILER_GCC)
    list(APPEND LITEOS_COPTS_BASE -fno-aggressive-loop-optimizations)
endif()

# clang support -fno-omit-frame-pointer
if(LOSCFG_BACKTRACE)
if(NOT LOSCFG_ARCH_ARM_CORTEX_M AND NOT LOSCFG_ARCH_XTENSA)
    list(APPEND LITEOS_COPTS_BASE -fno-omit-frame-pointer)
    list(APPEND LITEOS_CXXOPTS_BASE -fno-omit-frame-pointer)
endif()
endif()

if(NOT LOSCFG_COMPILER_XTENSA_32)
    list(APPEND LITEOS_COPTS_BASE -Winvalid-pch)
endif()

if((NOT LOSCFG_ARCH_RISCV32) AND (NOT LOSCFG_COMPILER_ARM_NONE_EABI))
    list(APPEND LITEOS_COPTS_BASE -fno-short-enums)
endif()

list(APPEND LITEOS_CXXOPTS_BASE
    -std=c++11 -nostdinc++ -fexceptions -fpermissive -fno-use-cxa-atexit -frtti
    ${LITEOS_COMMON_OPTS}
)

if(LOSCFG_COMPILER_ARM_NONE_EABI)
    list(APPEND LITEOS_CXXOPTS_BASE -Wno-deprecated)
endif()

if(NOT LOSCFG_COMPILER_XTENSA_32)
    list(APPEND LITEOS_CXXOPTS_BASE -Winvalid-pch)
endif()

if(LOSCFG_LLTREPORT)
    # -fprofile-arcs may introduce false alarm on 'maybe-uninitialized'
    set(LITEOS_GCOV_OPTS -fprofile-arcs -ftest-coverage -Wno-maybe-uninitialized)
    list(APPEND LITEOS_DEP_LIBS_EXT gcov)
endif()

list(APPEND LITEOS_LD_OPTS -static --gc-sections ${LITEOS_ASLR_LD_OPTS})
if(LOSCFG_TRUSTZONE AND LOSCFG_SECURE_REGION)
    list(APPEND LITEOS_LD_OPTS --cmse-implib --out-implib=${CMAKE_CURRENT_BINARY_DIR}/CMSE_importLib.o)
elseif(LOSCFG_TRUSTZONE AND (NOT LOSCFG_SECURE_REGION))
    list(APPEND LITEOS_LD_OPTS ${CMAKE_CURRENT_BINARY_DIR}/../secure/CMSE_importLib.o)
endif()

if(LOSCFG_COMPILER_XTENSA_32)
    include(${MK_PATH}/compiler/compiler_xcc_cpu.cmake)
elseif(LOSCFG_COMPILER_GCC)
    include(${MK_PATH}/compiler/compiler_gcc_cpu.cmake)
endif()

list(APPEND LITEOS_LD_OPTS ${LITEOS_DYNLOADOPTS})
set(LITEOS_LD_PATH
    -L${LITEOS_SCRIPTPATH}/ld -L${BOARD_LD_OUTPUT_PATH}
    -L${OUT}/lib -L${LITEOS_LIB_BIGODIR} -L${LITEOSTOPDIR}/tools/build
    -L${LITEOSTOPDIR}/targets/${LITEOS_PLATFORM}
)
FOREACH(CUR_DIR ${LITEOS_COMPILER_GCCLIB_PATH} ${LITEOS_COMPILER_CXXLIB_PATH})
    list(APPEND LITEOS_LD_PATH -L${CUR_DIR})
ENDFOREACH(CUR_DIR)

if(LOSCFG_VENDOR)
    list(APPEND LITEOS_LD_PATH -L${OUT}/lib/rdk -L${OUT}/lib/sdk -L${OUT}/lib/main_server)
endif()

if(LOSCFG_USING_BOARD_LD)
if(LOSCFG_BOX)
    set(LITEOS_LD_SCRIPT -T${LITEOSTOPDIR}/targets/${LITEOS_PLATFORM}/box/liteos.ld)
else()
    set(LITEOS_LD_SCRIPT -T${LITEOSTOPDIR}/targets/${LITEOS_PLATFORM}/liteos.ld)
endif()
else()
    set(LITEOS_LD_SCRIPT -T${LITEOSTOPDIR}/liteos.ld)
endif()

list(APPEND LITEOS_CXXINCLUDE
    ${LITEOS_COMPILER_GCC_INCLUDE}
    ${LITEOS_ARCH_INCLUDE}
    ${LITEOS_NET_INCLUDE}
    ${LITEOS_SECURITY_INCLUDE}
    ${LITEOS_KERNEL_INCLUDE}
    ${LITEOS_FS_INCLUDE}
    ${LITEOS_LIBC_INCLUDE}
    ${LITEOS_DRIVERS_INCLUDE}
)
set(LOSCFG_TOOLS_DEBUG_INCLUDE ${LITEOS_SHELL_INCLUDE} ${LITEOS_UART_INCLUDE})
