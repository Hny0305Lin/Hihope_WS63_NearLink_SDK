# Install script for directory: C:/Users/lmj-m/Documents/fbb_ws63/src/middleware/utils/dfx/diag_dfx_cmd

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/ws63_CFBB")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Users/lmj-m/Documents/fbb_ws63/src/tools/bin/compiler/riscv/cc_riscv32_musl_100/cc_riscv32_musl_fp_win/bin/riscv32-linux-musl-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-liteos-app/utils/dfx/diag_dfx_cmd/diag_audio_proc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-liteos-app/utils/dfx/diag_dfx_cmd/diag_audio_dump/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-liteos-app/utils/dfx/diag_dfx_cmd/diag_audio_probe/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-liteos-app/utils/dfx/diag_dfx_cmd/diag_nv/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-liteos-app/utils/dfx/diag_dfx_cmd/diag_update/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-liteos-app/utils/dfx/diag_dfx_cmd/diag_charger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-liteos-app/utils/dfx/diag_dfx_cmd/diag_logfile/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-liteos-app/utils/dfx/diag_dfx_cmd/diag_gpu_proc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-liteos-app/utils/dfx/diag_dfx_cmd/diag_audio_sample_data/cmake_install.cmake")
endif()

