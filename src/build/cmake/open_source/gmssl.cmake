set(GMSSL_PATH ${CMAKE_DIR}/../../open_source/GmSSL3.0)

set(COMPONENT_NAME "gmssl_hmac_sm3")

set(SOURCES
    ${GMSSL_PATH}/src/sm3.c
    ${GMSSL_PATH}/src/sm3_hmac.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${GMSSL_PATH}/include/
    ${GMSSL_PATH}/include/gmssl/
    ${GMSSL_PATH}/src/
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    "-Wno-sign-compare"  "-Wno-missing-declarations" "-Wno-missing-prototypes"
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

if(${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    install_sdk(${GMSSL_PATH} "*")
endif()

build_component()

set(COMPONENT_NAME "gmssl_sm4_ccm")

set(SOURCES
    ${GMSSL_PATH}/src/sm4_enc.c
    ${GMSSL_PATH}/src/sm4_common.c
    ${GMSSL_PATH}/src/sm4_setkey.c
)

set(PRIVATE_HEADER
    ${GMSSL_PATH}/include/
    ${GMSSL_PATH}/include/gmssl/
    ${GMSSL_PATH}/src/
)

if(${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    install_sdk(${GMSSL_PATH} "*")
endif()

build_component()

set(COMPONENT_NAME "gmssl_hmac_sha256")

set(PUBLIC_HEADER
    ${CMAKE_DIR}/open_source/ # 改private 开源软件需要调用stdio, 芯片无法提供, 强制插入替代定义
)

set(PUBLIC_DEFINES
    "GMSSL_ERROR_H" # 改private 开源软件需要调用stdio, 芯片无法提供, cmake定义开源软件的保护宏强制跳过
)

set(COMPONENT_PUBLIC_CCFLAGS
    "--includegmssl_stdio_adapter.h" # 改private 开源软件需要调用stdio, 芯片无法提供, 强制插入替代定义
)

set(SOURCES
    ${GMSSL_PATH}/src/digest.c
    ${GMSSL_PATH}/src/hmac.c
    ${GMSSL_PATH}/src/sha256.c
    ${GMSSL_PATH}/src/sha1.c
    ${GMSSL_PATH}/src/sha512.c
)

set(PRIVATE_HEADER
    ${GMSSL_PATH}/include/
    ${GMSSL_PATH}/include/gmssl/
    ${GMSSL_PATH}/src/
)

if(${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
    install_sdk(${GMSSL_PATH} "*")
endif()

build_component()
