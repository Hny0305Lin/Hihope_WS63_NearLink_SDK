#===============================================================================
# @brief    cmake deal with link script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

if("${LINK_SCRIPT}" STREQUAL "")
    MESSAGE(FATAL_ERROR "linkscript is not found!")
endif()

function(auto_gen_lds macro_name lib_list)
    file(APPEND ${CMAKE_BINARY_DIR}/auto_gen_libfunc.lds "#define ${macro_name}(func) \\\n")
    set(LIB_FUNC "${${lib_list}}")
    list(TRANSFORM LIB_FUNC PREPEND "    func(*lib")
    list(TRANSFORM LIB_FUNC APPEND ".a:*.o) \\\n")
    file(APPEND ${CMAKE_BINARY_DIR}/auto_gen_libfunc.lds ${LIB_FUNC} "\n")
endfunction()

# 添加用于处理链接脚本用的编译宏
set(LDS_DEFINES ${DEFINES})
list(TRANSFORM LDS_DEFINES PREPEND "-D")

# 添加用于处理链接脚本用的编译头文件
if(DEFINED LDS_HEADER)
    list(REMOVE_ITEM LDS_HEADER "")
    list(TRANSFORM LDS_HEADER PREPEND "-I")
else()
    set(LDS_HEADER ${ALL_PUBLIC_HEADER})
    list(REMOVE_DUPLICATES LDS_HEADER)
    list(REMOVE_ITEM LDS_HEADER "")
    list(TRANSFORM LDS_HEADER PREPEND "-I")
endif()
list(APPEND LDS_HEADER -I${CMAKE_BINARY_DIR})
if(DEFINED ROM_RAM_CHECK)
    list(APPEND LDSDEFINES -DROM_BIN_CHECK)
endif()
list(TRANSFORM LDS_HEADER PREPEND "\"")
list(TRANSFORM LDS_HEADER APPEND "\"")
string(JOIN " " LDS_HEADER_STR ${LDS_HEADER})
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/linker_header.srp "${LDS_HEADER_STR}")
auto_gen_lds("LIB_RAM_FUNC" RAM_COMPONENT)
auto_gen_lds("LIB_ROM_FUNC" ROM_COMPONENT)
auto_gen_lds("LIB_LOS_FUNC" LOS_LIB)

if(DEFINED SDK_OUTPUT_PATH)
    string(REPLACE "${PROJECT_SOURCE_DIR}" "${SDK_OUTPUT_PATH}" dest ${LINK_SCRIPT})
    get_filename_component(dest ${dest} DIRECTORY)
    install(FILES ${LINK_SCRIPT} DESTINATION ${dest})
    install_sdk(${LINK_SCRIPT} "*.prelds")
endif()

if(NOT DEFINED ROM_CHECK)
    set(ROM_CHECK False)
endif()

if(${ROM_CHECK})
    list(APPEND LDS_DEFINES "-DROM_CHECK")
endif()

add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
    COMMAND ${CMAKE_C_COMPILER} -P -xc -E -o linker.lds @${CMAKE_CURRENT_BINARY_DIR}/linker_header.srp ${LDS_DEFINES} ${LINK_SCRIPT}
    COMMENT "Generating ${LINK_SCRIPT} -> ${CMAKE_BINARY_DIR}/linker.lds"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM
)

target_link_directories(${TARGET_NAME}
    PRIVATE
    ${LIB_GCC}
    ${LIB_C}
    ${LIB_CXX}
)

target_link_libraries(${TARGET_NAME}
PRIVATE
    -Wl,--start-group "${STD_LIBS}" -Wl,--end-group
)

target_link_options(${TARGET_NAME}
    PRIVATE
    ${LINKFLAGS}
    -Wl,-T${CMAKE_BINARY_DIR}/linker.lds -nostartfiles
    -Wl,--gc-sections -Wl,-Map=${CMAKE_BINARY_DIR}/${BIN_NAME}.map
    -Wl,--print-memory-usage
)
