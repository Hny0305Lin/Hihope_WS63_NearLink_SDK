STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}  )
set(MODULE_NAME ${CURRENT_FOLDER})

# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_COPTS_PUB)
set(MODULE_ASOPTS_PUB)
set(MODULE_CXXOPTS_PUB)
set(MODULE_CMACRO_PUB)
set(MODULE_ASMACRO_PUB)
set(MODULE_CXXMACRO_PUB)

# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI)
if(LOSCFG_SHELL)
    list(APPEND MODULE_INCLUDE_PRI
        ${LITEOSTOPDIR}/fs/nfs
        ${LITEOSTOPDIR}/fs/ramfs
        ${LITEOSTOPDIR}/fs/fat
        ${LITEOSTOPDIR}/kernel/extended/include)
else()
    list(APPEND MODULE_INCLUDE_PRI ${LITEOSTOPDIR}/shell/include)
endif()
if(LOSCFG_TEST)
    list(APPEND MODULE_INCLUDE_PRI ${LITEOSTOPDIR}/test/include)
endif()

set(MODULE_COPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_ASOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CXXOPTS_PRI ${LITEOS_NON_SECURE_LOCAL_OPTS})
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

# add srcs to private target #
set(LOCAL_SRCS_y)
set(LOCAL_DIRS_y)

list(APPEND LOCAL_DIRS_${LOSCFG_ENABLE_MAGICKEY} src/magickey)

if(LOSCFG_SHELL)
    # add dir
    list(APPEND LOCAL_DIRS_y src/base)
    list(APPEND LOCAL_DIRS_${LOSCFG_SHELL_EXTENDED_CMDS} src/cmds)
    # add port src
    if(LOSCFG_SHELL_UART)
        list(APPEND LOCAL_SRCS_${LOSCFG_DRIVERS_UART_LINUX} src/base/porting/port_uart_linux.c)
        list(APPEND LOCAL_SRCS_${LOSCFG_DRIVERS_UART_LITEOS} src/base/porting/port_uart.c)
    endif()
    list(APPEND LOCAL_SRCS_${LOSCFG_SHELL_CONSOLE} src/base/porting/port_console.c)
endif()

if(LOSCFG_SHELL)
    if(LOSCFG_DYNAMIC_BOX)
        if(LOSCFG_PLATFORM_STM32F769IDISCOVERY)
            # add special cflag for dynamic box module
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffixed-r9")
        endif()
    endif()
endif()
FOREACH(CUR_DIR ${LOCAL_DIRS_y})
    aux_source_directory(${CUR_DIR} LOCAL_SRCS_y)
ENDFOREACH(CUR_DIR)


set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files
if(NOT "${LOCAL_SRCS}" STREQUAL "")
if(LOSCFG_SHELL_USER_TEXT)
    include(${UMODULE})
else()
    include(${MODULE})
endif()
endif()
