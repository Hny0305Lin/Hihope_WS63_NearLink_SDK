
ExternalProject_Add(liteos_hi1980b_imu
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1980b/imu
                -D NAMESPACE_SUFFIX=Imu
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/hi1980b_imu.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1980b_imu_debug
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1980b/imu_debug
                -D NAMESPACE_SUFFIX=Imu
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/debug/hi1980b_imu.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1980b_imu_mmu
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1980b/imu_mmu
                -D NAMESPACE_SUFFIX=Imu
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/hi1980b_imu_mmu.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1980b_imu_debug_mmu
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1980b/imu_debug_mmu
                -D NAMESPACE_SUFFIX=Imu
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/debug/hi1980b_imu_mmu.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1980_imu
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1980/imu
                -D NAMESPACE_SUFFIX=Imu
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/hi1980_imu.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1980_imu_mmu
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1980/imu_mmu
                -D NAMESPACE_SUFFIX=Imu
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/hi1980_imu_mmu.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1980_imu_debug
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1980/imu_debug
                -D NAMESPACE_SUFFIX=Imu
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/debug/hi1980_imu.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1980_imu_debug_mmu
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1980/imu_debug_mmu
                -D NAMESPACE_SUFFIX=Imu
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/debug/hi1980_imu_mmu.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1980b_ffts
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1980b/ffts
                -D NAMESPACE_SUFFIX=Ffts
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/hi1980b_ffts.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1980b_tsch
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1980b/tsch
                -D NAMESPACE_SUFFIX=Tsch
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/hi1980b_tsch.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1910b_tsch
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1910b/tsch
                -D NAMESPACE_SUFFIX=Tsch
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/hi1910b_tsch.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1980b_vdec
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
               -D INSTALL_LIB_CMAKE=ON
               -D INSTALL_RELATIVE_PATH=hi1980b/vdec
               -D NAMESPACE_SUFFIX=Vdec
               -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
               -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/hi1980b_vdec.config
    BUILD_COMMAND ${MAKE}
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1980_tsch
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1980/tsch
                -D NAMESPACE_SUFFIX=Tsch
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/hi1980_tsch.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1910_tsch
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1910/tsch
                -D NAMESPACE_SUFFIX=Tsch
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/hi1910_tsch.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)

ExternalProject_Add(liteos_hi1910p_tsch
    SOURCE_DIR ${LITEOS_SOURCE_DIR}
    CMAKE_ARGS  -D CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/liteos/ascend
                -D INSTALL_LIB_CMAKE=ON
                -D INSTALL_RELATIVE_PATH=hi1910p/tsch
                -D NAMESPACE_SUFFIX=Tsch
                -D LITEOS_TARGETS_INCLUDE=${LITEOS_SOURCE_DIR}/targets/ascend/common/include
                -D LOSCFG_CONFIG_PATH=${LITEOS_SOURCE_DIR}/tools/build/config/hi1910p_tsch.config
    BUILD_COMMAND $(MAKE)
    BUILD_ALWAYS TRUE
    EXCLUDE_FROM_ALL TRUE
)