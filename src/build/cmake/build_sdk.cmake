#===============================================================================
# @brief    cmake sdk build
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

# copy file or dir to the dest path in sdk
function(install_dest_sdk src sdk_dest partern)
    if (NOT DEFINED SDK_OUTPUT_PATH)
        return()
    endif()
    string(REPLACE "*" "all" _partern ${partern})
    string(REPLACE "/" "_" _name ${src}_${_partern})
    string(REPLACE "@" "_" _name ${_name})
    if(TARGET COPY${_name})
        return()
    endif()
    string(REPLACE "${ROOT_DIR}" "${SDK_OUTPUT_PATH}" dest ${sdk_dest})
    add_custom_target(COPY${_name} ALL
        COMMENT "--Installing ${src} => ${dest}"
        COMMAND ${Python3_EXECUTABLE} ${BUILD_UTILS} copy "${src}" "${dest}" ".srcrelease" '${partern}'
    )
    add_dependencies(COPY${_name} ${TARGET_NAME})
endfunction()

# copy file or dir to it's sdk path
function(install_sdk src partern)
    if (NOT DEFINED SDK_OUTPUT_PATH)
        return()
    endif()
    string(REPLACE "/" "_" _name ${src})
    string(REPLACE "@" "_" _name ${_name})
    if(TARGET COPY${_name})
        return()
    endif()
    string(REPLACE "${ROOT_DIR}" "${SDK_OUTPUT_PATH}" dest ${src})
    add_custom_target(COPY${_name} ALL
        COMMENT "--Installing ${src} => ${dest}"
        COMMAND ${Python3_EXECUTABLE} ${BUILD_UTILS} copy "${src}" "${dest}" ".srcrelease" '${partern}'
    )
    add_dependencies(COPY${_name} ${TARGET_NAME})
endfunction()

function(install_sdk_by_sh src partern)
    if (NOT DEFINED SDK_OUTPUT_PATH)
        return()
    endif()
    return_if_not_build()
    string(REPLACE "*" "all" _partern ${partern})
    string(REPLACE "/" "_" _name ${src}_${_partern})
    string(REPLACE "@" "_" _name ${_name})
    if(TARGET COPY${_name})
        return()
    endif()
    string(REPLACE "${ROOT_DIR}" "${SDK_OUTPUT_PATH}" dest ${src})
    add_custom_target(COPY${_name} ALL
        COMMENT "--Installing ${src} => ${dest}"
        COMMAND mkdir -p "${dest}"
        COMMAND cp -rf "${src}/${partern}" "${dest}" || :
    )
    add_dependencies(COPY${_name} ${TARGET_NAME})
endfunction()

macro(install_lib_gen)
    if (DEFINED SDK_OUTPUT_PATH AND DEFINED LIB_GEN_NAME AND ${COMPONENT_NAME} IN_LIST SDK_LIBGEN_COMPONENTS)
        set(LIB_OUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_GEN_NAME})
        string(REPLACE "${ROOT_DIR}" "${SDK_OUTPUT_PATH}" LIB_OUT_PATH ${LIB_OUT_PATH})
    endif()
endmacro()

macro(export_component_info)
file(WRITE ${PROJECT_BINARY_DIR}/component_info/${COMPONENT_NAME} "SOURCES: ${SOURCES}\n")
file(APPEND ${PROJECT_BINARY_DIR}/component_info/${COMPONENT_NAME} "HEADER: ${PRIVATE_HEADER}\n")
file(APPEND ${PROJECT_BINARY_DIR}/component_info/${COMPONENT_NAME} "DEFINES: ${PRIVATE_DEFINES}\n")
file(APPEND ${PROJECT_BINARY_DIR}/component_info/${COMPONENT_NAME} "COMPONENT_FLAGS: ${COMPONENT_FLAGS}\n")
endmacro()

macro(export_public_info)
file(WRITE ${PROJECT_BINARY_DIR}/component_info/PUBLIC "PUBLIC_HEADER: ${ALL_PUBLIC_HEADER}\n")
file(APPEND ${PROJECT_BINARY_DIR}/component_info/PUBLIC "PUBLIC_DEFINES: ${ALL_PUBLIC_DEFINES}\n")
endmacro()

macro(add_tooltrain)
set(TOOLCHAIN_FILE ${PROJECT_BINARY_DIR}/toolchains/${TARGET_NAME}.json)
file(WRITE ${TOOLCHAIN_FILE}
"{
    \"CC\": \"${CMAKE_C_COMPILER}\",
    \"CXX\": \"${CMAKE_CXX_COMPILER}\",
    \"AS\": \"${CMAKE_ASM_COMPILER}\",
    \"LD\": \"${CMAKE_LINKER}\",
    \"AR\": \"${CMAKE_AR}\",
    \"RANLIB\": \"${CMAKE_RANLIB}\",
    \"STRIP\": \"${CMAKE_STRIP}\",
    \"NM\": \"${CMAKE_NM}\",
    \"OBJCOPY\": \"${CMAKE_OBJCOPY}\",
    \"READELF\": \"${CMAKE_READELF}\",
    \"OBJDUMP\": \"${CMAKE_OBJDUMP}\"
}")
endmacro(add_tooltrain)

macro(sdk_export_component type)
if (DEFINED SDK_OUTPUT_PATH)
    set(COMPONENT_INFO ${PROJECT_BINARY_DIR}/component_info/info.json)
    if(${type} STREQUAL "start")
        set(START_SYMBOL "" CACHE INTERNAL "" FORCE)
        file(WRITE ${COMPONENT_INFO} "{\n")
        file(APPEND ${COMPONENT_INFO} 
        "\"COMPONENTS\":{")
    elseif(${type} STREQUAL "end")
        set(PUBLIC_INCLUDES)
        foreach(pub_header ${ALL_PUBLIC_HEADER})
            get_filename_component(header ${pub_header} ABSOLUTE)
            if (NOT ${header} IN_LIST PUBLIC_INCLUDES)
                set(PUBLIC_INCLUDES ${PUBLIC_INCLUDES} ${header})
            endif()
        endforeach()
        list(REMOVE_DUPLICATES ALL_PUBLIC_DEFINES)
        list(REMOVE_DUPLICATES ALL_PUBLIC_CCFLAGS)
        list(REMOVE_DUPLICATES LOG_DEF_LIST)
        set(STD_LIB_DIR ${LIB_GCC} ${LIB_C} ${LIB_CXX})
        file(APPEND ${COMPONENT_INFO} "},")
        file(APPEND ${COMPONENT_INFO} "\"PUBLIC_INCLUDES\": \"${PUBLIC_INCLUDES}\",\n")
        file(APPEND ${COMPONENT_INFO} "\"PUBLIC_DEFINES\": \"${ALL_PUBLIC_DEFINES}\",\n")
        file(APPEND ${COMPONENT_INFO} "\"PUBLIC_CCFLAGS\": \"${ALL_PUBLIC_CCFLAGS}\",\n")
        file(APPEND ${COMPONENT_INFO} "\"BIN_NAME\": \"${BIN_NAME}\",\n")
        file(APPEND ${COMPONENT_INFO} "\"LINKFLAGS\": \"${LINKFLAGS}\",\n")
        file(APPEND ${COMPONENT_INFO} "\"STD_LIB_DIR\": \"${STD_LIB_DIR}\",\n")
        file(APPEND ${COMPONENT_INFO} "\"STD_LIBS\": \"${STD_LIBS}\",\n")
        file(APPEND ${COMPONENT_INFO} "\"ARCH_FAMILY\": \"${ARCH_FAMILY}\",\n")
        file(APPEND ${COMPONENT_INFO} "\"LINK_SCRIPT\": \"${LINK_SCRIPT}\",\n")
        file(APPEND ${COMPONENT_INFO} "\"KERNEL\": \"${OS}\",\n")
        file(APPEND ${COMPONENT_INFO} "\"LOS_ROOT\": \"${LOS_ROOT}\",\n")
        file(APPEND ${COMPONENT_INFO} "\"LOS_PUB_CCFLAGS\": \"${LOS_PUB_CCFLAGS}\",\n")
        file(APPEND ${COMPONENT_INFO} "\"LOG_DEF_LIST\": \"${LOG_DEF_LIST}\"\n")
        file(APPEND ${COMPONENT_INFO} "}")
    elseif(${type} STREQUAL "third_party")
        if(DEFINED LIB_DIR)
            set(LINK_LIB_DIR ${LIB_DIR})
        else()
            set(LINK_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR})
        endif()
        file(APPEND ${COMPONENT_INFO}
            "${START_SYMBOL}
            \"${COMPONENT_NAME}\":{
            \"SOURCES\": \"${SOURCES}\",
            \"THIRD_PARTY\": \"true\",
            \"WHOLE_LINK\": \"${WHOLE_ARCHIV}\",
            \"COMPONENT_CUSTOM_INCLUDE\": \"${COMPONENT_CUSTOM_INCLUDE}\",
            \"LIB_DIR\":\"${LINK_LIB_DIR}\",
            \"COMPONENT_CUSTOM_CCFLAGS\":\"${COMPONENT_CUSTOM_CCFLAGS}\",
            \"COMPONENT_CUSTOM_DEFINES\":\"${COMPONENT_CUSTOM_DEFINES}\",
            \"LIBS\":\"${LIBS}\"
            }
            ")
        set(START_SYMBOL "," CACHE INTERNAL "" FORCE)
    else()
        if(DEFINED LIB_DIR)
            set(LINK_LIB_DIR ${LIB_DIR})
        else()
            check_if_closed_component(${COMPONENT_NAME})
            if (${COMPONENT_IS_CLOSED})
                set(LINK_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND})
            else()
                set(LINK_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR})
            endif()
        endif()

        check_if_closed_component(${COMPONENT_NAME})
        if (${COMPONENT_IS_CLOSED})
            file(APPEND ${COMPONENT_INFO}
            "${START_SYMBOL}
            \"${COMPONENT_NAME}\":{
            \"SOURCES\": \"\",
            \"PRIVATE_INCLUDE\": \"\",
            \"PRIVATE_DEFINES\": \"\",
            \"PRIVATE_CCFLAGS\": \"\",
            \"WHOLE_LINK\": \"${WHOLE_ARCHIV}\",
            \"LIB_DIR\":\"${LINK_LIB_DIR}\",
            \"LIBS\":\"${LIBS}\",
            \"MODULE_NAME\":\"${${COMPONENT_NAME}_MODULE_NAME}\",
            \"AUTO_DEF\":\"${${COMPONENT_NAME}_AUTO_DEF}\"
            }
            ")
        else()
            file(APPEND ${COMPONENT_INFO}
            "${START_SYMBOL}
            \"${COMPONENT_NAME}\":{
            \"SOURCES\": \"${SOURCES}\",
            \"PRIVATE_INCLUDE\": \"${PRIVATE_HEADER}\",
            \"PRIVATE_DEFINES\": \"${PRIVATE_DEFINES}\",
            \"PRIVATE_CCFLAGS\": \"${COMPONENT_CCFLAGS}\",
            \"WHOLE_LINK\": \"${WHOLE_ARCHIV}\",
            \"LIB_DIR\":\"${LINK_LIB_DIR}\",
	        \"LIBS\":\"${LIBS}\",
            \"MODULE_NAME\":\"${${COMPONENT_NAME}_MODULE_NAME}\",
            \"AUTO_DEF\":\"${${COMPONENT_NAME}_AUTO_DEF}\"
            }
            ")
        endif()
        set(START_SYMBOL "," CACHE INTERNAL "" FORCE)
    endif()
endif()
endmacro()

function(generate_project_file)
    if (NOT DEFINED SDK_OUTPUT_PATH)
        return()
    endif()
    set(COMPONENT_INFO ${PROJECT_BINARY_DIR}/component_info/info.json)
    add_tooltrain()
    set(CC_JSON  "${PROJECT_BINARY_DIR}/compile_commands.json")
    if("${SDK_PROJECT_FILE_DIR}" STREQUAL "" )
    set(SDK_PROJECT_FILE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    string(REPLACE "${PROJECT_SOURCE_DIR}" "${SDK_OUTPUT_PATH}" SDK_CURRENT_DIR ${SDK_PROJECT_FILE_DIR})
    string(REPLACE "${PROJECT_SOURCE_DIR}" "${SDK_OUTPUT_PATH}" SDK_CURRENT_COMPILER_DIR ${COMPILER_ROOT})
    list(JOIN SDK_TYPE "," SDK_TYPE_LIST)

    add_custom_target(GEN_PROJECT ALL
        COMMAND ${Python3_EXECUTABLE} ${PRO_GEN} "${SDK_TYPE_LIST}" "${MAIN_COMPONENT}" "${CC_JSON}" "${SDK_CURRENT_DIR}" "${PROJECT_SOURCE_DIR}" "${SDK_OUTPUT_PATH}" "${CHIP},${CORE},${BOARD},${ARCH},${OS},${PKG_TARGET_NAME}" "${TOOLCHAIN_FILE}" "${COMPONENT_INFO}"
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMENT "Generating project file..."
    )

    add_dependencies(GEN_PROJECT "${TARGET_NAME}")
    if(NOT EXISTS ${SDK_CURRENT_COMPILER_DIR})
        get_filename_component(SDK_CURRENT_COMPILER_DIR_ABS  "${SDK_CURRENT_COMPILER_DIR}/.." ABSOLUTE)
        add_custom_target(GEN_COMPILER_LN ALL
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            COMMAND mkdir -p ${SDK_CURRENT_COMPILER_DIR_ABS}
            COMMAND ln -s ${COMPILER_ROOT} ${SDK_CURRENT_COMPILER_DIR}
        )
        if(DEFINED COMPILER_ROOT_WIN AND EXISTS ${COMPILER_ROOT_WIN})
            add_custom_target(GEN_COMPILER_WIN_LN ALL
                WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                COMMAND ln -s ${COMPILER_ROOT_WIN} ${SDK_CURRENT_COMPILER_DIR_ABS}
                DEPENDS GEN_COMPILER_LN
            )
        endif()

    endif()
endfunction(generate_project_file)
