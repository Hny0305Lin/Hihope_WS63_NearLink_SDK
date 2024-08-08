#===============================================================================
# @brief mqtt cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "mqtt")

set(CMAKE_MQTT_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/paho.mqtt.c/)

set(SOURCES
        ${CMAKE_MQTT_SOURCE_DIR}/src/Base64.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/Clients.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/Heap.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/LinkedList.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/Log.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/Messages.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/MQTTClient.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/MQTTPacket.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/MQTTPacketOut.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/MQTTPersistence.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/MQTTPersistenceDefault.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/MQTTProperties.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/MQTTProtocolClient.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/MQTTProtocolOut.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/MQTTReasonCodes.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/OsWrapper.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/Proxy.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/SHA1.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/Socket.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/SocketBuffer.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/SSLSocket.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/SSLSocket_mbedtls.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/StackTrace.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/Thread.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/Tree.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/utf-8.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/WebSocket.c
        ${CMAKE_MQTT_SOURCE_DIR}/src/MQTTTime.c
)

set(PUBLIC_HEADER
    ${CMAKE_MQTT_SOURCE_DIR}/src
)

set(PRIVATE_HEADER
    ${CMAKE_MQTT_SOURCE_DIR}/src
)

set(PRIVATE_DEFINES
    IOT_LITEOS_ADAPT
    NO_PERSISTENCE
    COMPAT_CMSIS
    NOSTACKTRACE
    MBEDTLS
    MBEDTLS_USE_CRT
    HIGH_PERFORMANCE
)

set(PUBLIC_DEFINES
)

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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()