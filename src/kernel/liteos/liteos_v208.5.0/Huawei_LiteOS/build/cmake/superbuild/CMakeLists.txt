cmake_minimum_required(VERSION 3.16.5)
project(integrate)
string(REGEX MATCH "(^.*)/build/cmake/superbuild$" LITEOS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LITEOS_SOURCE_DIR ${CMAKE_MATCH_1})
message("${LITEOS_SOURCE_DIR}")

include(ExternalProject)

message("PATH = $ENV{PATH}")
MESSAGE(STATUS CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}")
message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

if (CMAKE_INSTALL_PREFIX STREQUAL /usr/local)
    set(CMAKE_INSTALL_PREFIX ${LITEOS_SOURCE_DIR}/output CACHE STRING "path for install()" FORCE)
    message(STATUS "No install prefix selected, default to ${CMAKE_INSTALL_PREFIX}.")
endif()

set(SUPERBUILD_PATH ${LITEOS_SOURCE_DIR}/build/cmake/superbuild)
include(${SUPERBUILD_PATH}/ascend.cmake)
include(${SUPERBUILD_PATH}/kirin.cmake)
