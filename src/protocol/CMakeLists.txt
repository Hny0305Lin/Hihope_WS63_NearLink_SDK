#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(_CHIP "${CHIP}")
if(${CHIP} STREQUAL "socmn1")
    set(_CHIP "melody")
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/wifi/CMakeLists.txt)
	add_subdirectory(wifi)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/wifi.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/wifi.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cat1/CMakeLists.txt)
    add_subdirectory(cat1)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/cat1.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/cat1.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bt/controller/CMakeLists.txt)
    add_subdirectory(bt/controller btc)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/btc.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/btc.cmake)
elseif(EXISTS ${BIN_DIR}/${_CHIP}/libs/bluetooth/btc/CMakeLists.txt)
    add_subdirectory(${BIN_DIR}/${_CHIP}/libs/bluetooth/btc btc)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bt/host/CMakeLists.txt)
    add_subdirectory(bt/host bth)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/bth.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/bth.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bt/host/audio/bth_audio/CMakeLists.txt)
    add_subdirectory(bt/host/audio/bth_audio)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bt/host/bt/bts/CMakeLists.txt)
    add_subdirectory(bt/host/bt/bts)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/bts.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/bts.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bt/host/audio/bth_audio_service/CMakeLists.txt)
    add_subdirectory(bt/host/audio/bth_audio_service)
elseif(EXISTS ${BIN_DIR}/${PROJECT_CHIP_DIR}/melody/libs/bt/CMakeLists.txt)
    add_subdirectory(${BIN_DIR}/melody/libs/bt bt)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/nfc/CMakeLists.txt)
    add_subdirectory(nfc)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/nfc.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/nfc.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bt/algorithm/posalg/CMakeLists.txt)
    add_subdirectory(bt/algorithm/posalg)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/radar/alg_ai/CMakeLists.txt)
    add_subdirectory(radar/alg_ai)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/radar/plat/CMakeLists.txt)
    add_subdirectory(radar/plat)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/radar.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/radar.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/glp/hiex_v300/CMakeLists.txt)
    add_subdirectory(glp/hiex_v300)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/slp_client.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/slp_client.cmake)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bt/controller/chba/CMakeLists.txt)
    add_subdirectory(bt/controller/chba chba)
elseif(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/chba.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/chba.cmake)
endif()

if(EXISTS ${ROOT_DIR}/libs_url/${_CHIP}/cmake/ohos.cmake)
    include(${ROOT_DIR}/libs_url/${_CHIP}/cmake/ohos.cmake)
endif()
