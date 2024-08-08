#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(porting)
add_subdirectory_if_exist(dyn_mem)
add_subdirectory_if_exist(boot)
add_subdirectory_if_exist(test_rom)

if(USE_WS63_ROM_REPO IN_LIST DEFINES)
add_subdirectory_if_exist(rom)
endif()

set(COMPONENT_NAME "chip_ws63")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/riscv31/interrupt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/riscv31/interrupt_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/riscv31/vectors.s
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/riscv31/flash_patch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/interrupt/interrupt_adapter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vectors/vectors.c
)

if(DEFINED OS AND NOT ${OS} STREQUAL "non-os")
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/interrupt/interrupt_os_adapter.c)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/riscv31/oam_trace.c)
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/include
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/riscv31
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/riscv_common
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vectors
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

set(COMPONENT_NAME "arch_header")

set(SOURCES
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/riscv_common
    ${CMAKE_CURRENT_SOURCE_DIR}/arch/riscv/riscv31
)

set(PRIVATE_HEADER
)

set(WHOLE_LINK
    true
)

build_component()

set(COMPONENT_NAME "rom_callback")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/output/rom_callback.S
)

if(DEFINED ROM_SYM_PATH)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/plat_rom_data.c)
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

if(${BUILD_ROM_CALLBACK} OR rom_callback IN_LIST TARGET_COMPONENT)
    install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/acore.sym "*")
    install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/output/function.lds "*")
    install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/output/data.lds "*")
    install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/output/rom_data.lds "*")
    install_sdk(${ROOT_DIR}/interim_binary/ws63/bin/rom_bin "*.bin")
    install_sdk(${ROOT_DIR}/interim_binary/ws63/bin/boot_bin "*.bin")
    execute_process(
        COMMAND ${CP} ${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/output/function.lds ${CMAKE_BINARY_DIR}
        COMMAND ${CP} ${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/output/data.lds ${CMAKE_BINARY_DIR}
        COMMAND ${CP} ${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/output/rom_data.lds ${CMAKE_BINARY_DIR}
    )
endif()

if(rom_callback IN_LIST TARGET_COMPONENT)
    include(${CMAKE_CURRENT_SOURCE_DIR}/rom_config/${CORE}/output/rom_callback_wrap.cmake)
    file(WRITE ${CMAKE_BINARY_DIR}/rom_cb_flag.srp "${CMAKE_CB_FLGAS}")
    target_link_options(${TARGET_NAME}
        PRIVATE
        @${CMAKE_BINARY_DIR}/rom_cb_flag.srp
    )
endif()

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/rom_info/${TARGET_COMMAND})

build_component()
