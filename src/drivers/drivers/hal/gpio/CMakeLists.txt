#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_gpio_v100")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_gpio_v100.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v100/hal_gpio_v100_regs_op.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/v100/
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

set(COMPONENT_NAME "hal_gpio_v150")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/hal_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_gpio_v150.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_gpio_v150_regs_op.c
    ${CMAKE_CURRENT_SOURCE_DIR}/v150/hal_gpio_v150_comm.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/v150/
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
