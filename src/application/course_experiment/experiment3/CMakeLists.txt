#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/io_expander.c" 
    "${CMAKE_CURRENT_SOURCE_DIR}/ssd1306.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ssd1306_fonts.c"
 PARENT_SCOPE)
# set(COMPONENT_NAME "work3")
#
# set(SOURCES
#     ${CMAKE_CURRENT_SOURCE_DIR}/main.c
#     ${CMAKE_CURRENT_SOURCE_DIR}/io_expander.c
#     ${CMAKE_CURRENT_SOURCE_DIR}/ssd1306.c
#     ${CMAKE_CURRENT_SOURCE_DIR}/ssd1306_fonts.c
# )
#
# set(PUBLIC_HEADER
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )
#
# set(PRIVATE_HEADER
# )
#
# set(PRIVATE_DEFINES
# )
#
# set(PUBLIC_DEFINES
# )
#
# # use this when you want to add ccflags like -include xxx
# set(COMPONENT_PUBLIC_CCFLAGS
# )
#
# set(COMPONENT_CCFLAGS
# )
#
# set(WHOLE_LINK
#     true
# )
#
# set(MAIN_COMPONENT
#     true
# )
#
# install_sdk("${CMAKE_CURRENT_SOURCE_DIR}/" "*")
# build_component()
