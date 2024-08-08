#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "gpu_proc")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/diag_gpu_proc.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PRIVATE_HEADER
	${CMAKE_SOURCE_DIR}/middleware/utils/common_headers/drivers/gpu
	${CMAKE_SOURCE_DIR}/middleware/chips/brandy/dfx/include
	${CMAKE_SOURCE_DIR}/middleware/services/gpu/drv/include
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
