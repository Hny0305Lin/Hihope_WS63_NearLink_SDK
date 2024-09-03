# Install script for directory: C:/Users/lmj-m/Documents/fbb_ws63/src/drivers/chips/ws63/porting

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
  set(CMAKE_OBJDUMP "C:/Users/lmj-m/Documents/fbb_ws63/src/tools/bin/compiler/riscv/cc_riscv32_musl_100/cc_riscv32_musl_win/bin/riscv32-linux-musl-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/adc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/arch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/gpio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/liteos/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/pwm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/uart/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/timer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/systick/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/tcxo/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/watchdog/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/pinctrl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/pmp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/i2c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/dma/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/spi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/sfc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/tsensor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/efuse/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/rtc_unified/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/security_unified/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/mem/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/sio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/reboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/version/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/patch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/pm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/lmj-m/Documents/fbb_ws63/src/output/ws63/acore/ws63-flashboot/drivers/chips/ws63/porting/xo_trim/cmake_install.cmake")
endif()

