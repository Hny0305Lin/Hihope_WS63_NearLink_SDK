ExternalProject_Add(ivp_bbk
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/bbk
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=ivp
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ivp/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/ivp_bbk.config
                -D IVP_LITEOS_INTERACTED=ON
                -D CMAKE_TOOLCHAIN_FILE=${LITEOS_SOURCE_DIR}/../../../hisi/ivp/third_party/liteos_onetrack/toolchain_env_vq6.cmake
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
)

ExternalProject_Add(lpm_bbk
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/bbk
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=lpm
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/lpm_v200/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/lpm_bbk.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
)
