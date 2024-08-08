#===============================================================================
# @brief libcoap cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "coap")

set(CMAKE_COAP_SOURCE_DIR
    ${CMAKE_DIR}/../../open_source/libcoap/libcoap)

set(SOURCES
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_block.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_hashkey.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_io_lwip.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_session.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_encode.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_net.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_option.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_pdu.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_resource.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_str.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_subscribe.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_uri.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_debug.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_io.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_address.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_cache.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_netif.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_dtls.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_tcp.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_ws.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_layers.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_async.c
    ${CMAKE_COAP_SOURCE_DIR}/src/coap_notls.c
)

set(PUBLIC_HEADER
    ${CMAKE_COAP_SOURCE_DIR}/include/
    ${CMAKE_COAP_SOURCE_DIR}/
    ${CMAKE_COAP_SOURCE_DIR}/examples/lwip/config/
)

set(PRIVATE_HEADER
    ${CMAKE_COAP_SOURCE_DIR}/include/
    ${CMAKE_COAP_SOURCE_DIR}/
    ${CMAKE_COAP_SOURCE_DIR}/examples/lwip/config/
)

set(PRIVATE_DEFINES
    LIB_CONFIGURABLE
    WITH_LWIP
    MEMP_USE_CUSTOM_POOLS=1
    LWIP_NOASSERT
    LOG_WARNING=4
    HASH_NONFATAL_OOM=1
    COAP_MAX_LOGGING_LEVEL=0
    COAP_CONSTRAINED_STACK=0
)

set(PUBLIC_DEFINES
    #MEMP_USE_CUSTOM_POOLS=1
)
if(${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    set(FILE_TO_CHECK ${CMAKE_COAP_SOURCE_DIR}/include/coap3/coap.h)
    if (NOT EXISTS ${FILE_TO_CHECK})
        set(SOURCE_FILE ${CMAKE_COAP_SOURCE_DIR}/include/coap3/coap.h.windows)
        set(DESTINATION_FILE ${CMAKE_COAP_SOURCE_DIR}/include/coap3/coap.h)
        file(RENAME ${SOURCE_FILE} ${DESTINATION_FILE})
    endif()
endif()

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-missing-prototypes
    -Wno-unused-parameter
    -Wno-strict-prototypes
    -Wno-implicit-function-declaration
    -Wno-missing-declarations
    -Wno-sign-compare
    -Wno-int-conversion
    -Wno-unused-label
    -Wno-unused-function
    -Wno-error=logical-op
    -Wno-error=return-type
    -Wno-error=jump-misses-init
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)
if(${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    build_component()
endif()