#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(PUBLIC_HEADER
    ${ROOT_DIR}/middleware/utils/common_headers/native
)

set(COMPONENT_NAME nv_config_ws63)
set(BUILD_TARGETS nv_config_ws63)
set(TARGET ${CORE})
set(GEN_TARGET_DIR
        ${ROOT_DIR}/output/${CHIP}/${CORE})
set(TARGET_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${ROOT_DIR}/middleware/utils/common_headers/native)
set(BUILD_GEN_UTILS
        ${ROOT_DIR}/build/script/nv/generate_utils.py)
set(TARGET_DEFINES
    -DCONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM)

if(${COMPONENT_NAME} IN_LIST BUILD_TARGETS)
    set(GEN_TARGET_SRC ${GEN_TARGET_DIR}/${TARGET}.c)
    message("GEN_TARGET_SRC = ${GEN_TARGET_SRC}")
    set(PRECOMPILE_TARGET ${GEN_TARGET_DIR}/${TARGET}.etypes)
    message("PRECOMPILE_TARGET = ${PRECOMPILE_TARGET}")
    set(INCLUDE_FLAGS)
    foreach(item ${TARGET_INCLUDE})
        list(APPEND INCLUDE_FLAGS -I${item})
    endforeach()
    message("TARGET_INCLUDE = ${TARGET_INCLUDE}")
    add_custom_command(
        OUTPUT NV_SRC
        COMMAND ${Python3_EXECUTABLE} ${BUILD_GEN_UTILS} NV include ${GEN_TARGET_SRC}
        COMMAND ${CMAKE_C_COMPILER} -o ${PRECOMPILE_TARGET} ${INCLUDE_FLAGS} ${TARGET_DEFINES} -E ${GEN_TARGET_SRC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    add_custom_target(${COMPONENT_NAME} ALL DEPENDS NV_SRC)

else()
    return()
endif()

install_sdk(${CMAKE_CURRENT_SOURCE_DIR} "*")
