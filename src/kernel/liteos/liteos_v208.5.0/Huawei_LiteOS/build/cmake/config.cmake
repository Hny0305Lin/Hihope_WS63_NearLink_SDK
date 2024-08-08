
############### this is a cmake that you can config it ###############
include(${LITEOS_TOP_DIR}/build/cmake/los_config.cmake)

set(TOP_LD_PATH ${LITEOSTOPDIR})
set(SCRIPTS_PATH ${LITEOS_SCRIPTPATH})
set(LITEOS_MK_PATH ${MK_PATH})
set(LITEOS_SUBDIRS ${LIB_SUBDIRS})
set(LIB_BIGODIR ${LITEOS_LIB_BIGODIR})


##### liteos tables -u ldflags #####
include(${LITEOS_MK_PATH}/liteos_tables_ldflags.cmake)


##### Use global PROPERTY LITEOS_MODULE_LIBS_PATH_PROP to import $(MODULE_NAME).a #####
# functions like "-include $(LIBDEP_FILE)" in makefile project
get_property(LITEOS_MODULE_DEP_LIBS_PATH GLOBAL PROPERTY LITEOS_MODULE_LIBS_PATH_PROP)
FOREACH(LOS_LIBS_PATH ${LITEOS_MODULE_DEP_LIBS_PATH})
    STRING(REGEX REPLACE ".+/lib(.+)\\..*" "\\1" LOS_LIBS_NAME ${LOS_LIBS_PATH})
    list(APPEND LITEOS_DEP_LIBS_INT ${LOS_LIBS_NAME})
if(LOSCFG_SAFETY_KERNEL)
    STRING(REGEX REPLACE "\\.a" ".O" LOS_OBJS_NAME ${LOS_LIBS_PATH})
    list(APPEND LITEOS_DEP_OBJS_INT ${LOS_OBJS_NAME})
endif()
ENDFOREACH(LOS_LIBS_PATH)
set(LITEOS_BASELIB ${LITEOS_EXT_AHEAD_LIBS} ${LITEOS_DEP_LIBS_EXT} ${LITEOS_INT_AHEAD_LIBS} ${LITEOS_DEP_LIBS_INT})


##### defined OS common cc macros and opts #####
# LITEOS_ASMACRO, LITEOS_CMACRO, LITEOS_CXXMACRO defined in los_config.cmake ahead
# no need to set LITEOS_ASFLAGS, LITEOS_CFLAGS, LITEOS_CXXFLAGS
set(LITEOS_COPTS
    ${LITEOS_COPTS_DEBUG} ${LITEOS_COPTS_BASE} ${LITEOS_COPTS_EXTRA}
    ${LITEOS_CORE_COPTS})
set(LITEOS_CXXOPTS ${LITEOS_CXXOPTS_BASE})
set(LITEOS_EXT_API_INC
    ${LITEOS_ARCH_INCLUDE}      ${LITEOS_KERNEL_INCLUDE}   ${LITEOS_EXTKERNEL_INCLUDE}
    ${LITEOS_COMPAT_INCLUDE}    ${LITEOS_FS_INCLUDE}       ${LITEOS_NET_INCLUDE}
    ${LITEOS_LIB_INCLUDE}       ${LITEOS_DRIVERS_INCLUDE}  ${LOSCFG_TOOLS_DEBUG_INCLUDE}
    ${LITEOS_SECURITY_INCLUDE}
)
set(LITEOS_LDFLAGS ${LITEOS_LD_OPTS} ${LITEOS_LD_PATH} ${LITEOS_LD_SCRIPT})


##### LiteOS exported target with header include and cc opts only, no srcs #####
set(LOS_INTF_DEP_TARGETS ${LITEOS_DEP_LIBS_INT})
list(TRANSFORM LOS_INTF_DEP_TARGETS APPEND ${LOS_PUB_INTF_SUFFIX})

add_library(${LOS_CC_PROP_INTF_PUB} INTERFACE)
add_library(${VENDOR_CC_PROP_INTF_PUB} INTERFACE) # for vendor compile options

target_compile_options(${LOS_CC_PROP_INTF_PUB} INTERFACE
    # auto added the header file LITEOS_PLATFORM_MENUCONFIG_H in all files
    -include ${LITEOS_PLATFORM_MENUCONFIG_H}
    $<$<COMPILE_LANGUAGE:C>:${LITEOS_COPTS}>
    $<$<COMPILE_LANGUAGE:ASM>:${LITEOS_ASOPTS}>
    $<$<COMPILE_LANGUAGE:CXX>:${LITEOS_CXXOPTS}>
)

target_compile_definitions(${LOS_CC_PROP_INTF_PUB} INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${LITEOS_CMACRO}>
    $<$<COMPILE_LANGUAGE:ASM>:${LITEOS_ASMACRO}>
    $<$<COMPILE_LANGUAGE:CXX>:${LITEOS_CMACRO} ${LITEOS_CXXMACRO}>
)

target_include_directories(${LOS_CC_PROP_INTF_PUB} INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${LITEOS_EXT_API_INC}>
    $<$<COMPILE_LANGUAGE:ASM>:${LITEOS_EXT_API_INC}>
    $<$<COMPILE_LANGUAGE:CXX>:${LITEOS_CXXINCLUDE}>
)

target_link_libraries(${LOS_CC_PROP_INTF_PUB} INTERFACE
    $<BUILD_INTERFACE:${LOS_INTF_DEP_TARGETS}>
)


##### cc board.ld.S file #####
if(EXISTS ${BOARD_LD_S_FILE})
    string(REGEX REPLACE "/include/menuconfig.h" "" PLAT_MENUCONFIG_PATH ${LITEOS_PLATFORM_MENUCONFIG_H})
    list(APPEND BOARD_LD_INCULDE_PATH -I${PLAT_MENUCONFIG_PATH} -I${LITEOSTOPDIR}/kernel/base/include)
    add_custom_target(
        los_taget_pre_build ALL
        DEPENDS ${BOARD_LD_FILE}
    )

    add_custom_command(
        OUTPUT ${BOARD_LD_FILE}
        COMMAND ${CMAKE_C_COMPILER} -E ${BOARD_LD_S_FILE} ${BOARD_LD_INCULDE_PATH} -o ${BOARD_LD_FILE} -P -D__COMPILER_HUAWEILITEOS__
    )
    add_dependencies(${LOS_CC_PROP_INTF_PUB} los_taget_pre_build)

    # check kconfig is install or not
    execute_process(
        COMMAND python3 ${SCRIPTS_PATH}/check_kconfiglib.py
        RESULT_VARIABLE PYTHON_RESULT
        OUTPUT_VARIABLE PYTHON_OUTPUT
        WORKING_DIRECTORY ${LITEOS_TOP_DIR}
    )
    if(${PYTHON_OUTPUT} MATCHES "True")
        get_target_property(TARGET_IS_EXISTS savemenuconfig STATUS)
        message("TARGET_IS_EXISTS:${TARGET_IS_EXISTS}")
        if(${TARGET_IS_EXISTS} MATCHES AVAILABLE)
            add_dependencies(los_taget_pre_build savemenuconfig)
        endif()
    endif()

endif()

##### output projects sys info #####
message("OUT:${OUT}")
message("LITEOS_MODULE_DEP_LIBS_PATH:${LITEOS_MODULE_DEP_LIBS_PATH}")
message("LITEOS_DEP_LIBS_INT:${LITEOS_DEP_LIBS_INT}")
message("LITEOS_DEP_LIBS_EXT:${LITEOS_DEP_LIBS_EXT}")
message("LITEOS_BASELIB:${LITEOS_BASELIB}")
message("LOS_INTF_DEP_TARGETS:${LOS_INTF_DEP_TARGETS}")
