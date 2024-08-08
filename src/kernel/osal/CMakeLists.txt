#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "osal")

if(NOT DEFINED OS)
    return()
endif()

if(${OS} STREQUAL "non-os")
    set(src_sub nonos)
elseif(${OS} STREQUAL "liteos")
    set(src_sub liteos)
elseif(${OS} STREQUAL "freertos")
    set(src_sub freertos)
elseif(${OS} STREQUAL "alios")
    set(src_sub alios)
endif()

if(${OS} STREQUAL "non-os" AND NOT "OSAL_NO_PAL" IN_LIST DEFINES)
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/adapt/${OS}/pal_non_os_interface.c
        ${CMAKE_CURRENT_SOURCE_DIR}/adapt/${OS}/pal_print.c
    )
    if(${CHIP} STREQUAL "bs25")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/adapt/${OS}/osal_addr.c")
    endif()
elseif(${OS} STREQUAL "liteos")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/adapt/${OS}/osal_debug_adapt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/adapt/${OS}/oal_onetrack.c
        ${CMAKE_CURRENT_SOURCE_DIR}/adapt/${OS}/osal_cpp_adapt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_addr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_atomic.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_barrier.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_cache.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_completion.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_debug.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_drvbox.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_event.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_fileops.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_interrupt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_math.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_msgqueue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_mutex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_semaphore.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_spinlock.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_string.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_task.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_timer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_wait.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_workqueue.c
    )

if("LOSCFG_FS_PROC" IN_LIST DEFINES)
	list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_proc.c)
endif()    
if(NOT "CHIP_WS63=1" IN_LIST DEFINES AND NOT "CHIP_WS53=1" IN_LIST DEFINES AND NOT "LITEOS_208" IN_LIST DEFINES)
	list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_device.c)
endif()

elseif(${OS} STREQUAL "freertos")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_addr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_atomic.c
        # ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_barrier.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_cache.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_completion.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_debug.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_device.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_drvbox.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_event.c
        # ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_fileops.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_interrupt.c
        # ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_math.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_msgqueue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_mutex.c
        # ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_proc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_semaphore.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_spinlock.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_string.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_task.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_timer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_wait.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_workqueue.c
        # ${CMAKE_CURRENT_SOURCE_DIR}/adapt/${src_sub}/osal_debug_adapt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/adapt/${OS}/osal_adapt.c
    )

elseif(${OS} STREQUAL "alios")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_interrupt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_string.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_debug.c
	    ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_addr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_mutex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_semaphore.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_timer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_task.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_msgqueue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_workqueue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_atomic.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_event.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_spinlock.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_wait.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_cache.c
        ${CMAKE_CURRENT_SOURCE_DIR}/adapt/${OS}/osal_adapt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/${src_sub}/osal_test.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_mutex01.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_mutex02.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_task01.c
        # ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_task02.c
        # ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_task03.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_task.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_semaphore01.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_timer01.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_workqueue.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_event.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_atomic01.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_atomic02.c
        #${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_msgqueue.c
        #${CMAKE_CURRENT_SOURCE_DIR}/test/alios/osal_test_irq.c
        #${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_spinlock01.c
        #${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_spinlock03.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_string01.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_string02.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_wait.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_timer02.c
        ${CMAKE_CURRENT_SOURCE_DIR}/test/src/osal_test_atomic.c
)

elseif(${OS} STREQUAL "non-os")
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_atomic.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_debug.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_interrupt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_mutex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_semaphore.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_spinlock.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_wait.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}/osal_task.c
    )
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/atomic
    ${CMAKE_CURRENT_SOURCE_DIR}/include/debug
    ${CMAKE_CURRENT_SOURCE_DIR}/include/device
    ${CMAKE_CURRENT_SOURCE_DIR}/include/drvbox
    ${CMAKE_CURRENT_SOURCE_DIR}/include/event
    ${CMAKE_CURRENT_SOURCE_DIR}/include/fileops
    ${CMAKE_CURRENT_SOURCE_DIR}/include/interrupt
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lock
    ${CMAKE_CURRENT_SOURCE_DIR}/include/math
    ${CMAKE_CURRENT_SOURCE_DIR}/include/memory
    ${CMAKE_CURRENT_SOURCE_DIR}/include/msgqueue
    ${CMAKE_CURRENT_SOURCE_DIR}/include/proc
    ${CMAKE_CURRENT_SOURCE_DIR}/include/schedule
    ${CMAKE_CURRENT_SOURCE_DIR}/include/semaphore
    ${CMAKE_CURRENT_SOURCE_DIR}/include/string
    ${CMAKE_CURRENT_SOURCE_DIR}/include/time
    ${CMAKE_CURRENT_SOURCE_DIR}/adapt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/test/alios
    ${CMAKE_CURRENT_SOURCE_DIR}/test/src
    
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/${src_sub}
)

if(${OS} STREQUAL "alios")
    list(APPEND PRIVATE_HEADER
        ${ROOT_DIR}/kernel/alios/alios_v3.3.0/components/osal_aos/rhino/include
        ${ROOT_DIR}/kernel/alios/alios_v3.3.0/components/osal_aos/include/aos
        ${ROOT_DIR}/kernel/alios/alios_v3.3.0/components/osal_aos/include
    )
endif()

set(PRIVATE_DEFINES
    CONFIG_LIB_SUPPORT_COMPAT_LINUX
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS

    -Wno-unused-parameter
    -Wno-implicit-function-declaration
    -Wno-sign-compare
    -Wno-visibility
)

if(${ARCH} STREQUAL "riscv70")
set(COMPONENT_CCFLAGS
    ${COMPONENT_CCFLAGS}
    -Wno-typedef-redefinition
)
endif()

if (${ARCH} STREQUAL "cortex_m7")
set(COMPONENT_CCFLAGS
    ${COMPONENT_CCFLAGS}
    -Wno-error=cast-function-type
)
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

if (${CHIP} STREQUAL "socmn1")
    add_subdirectory_if_exist(test)
endif()

if(syschannel_host_ko IN_LIST TARGET_COMPONENT)
install_sdk(${CMAKE_SOURCE_DIR}/kernel/osal/src/linux "*") 
endif()