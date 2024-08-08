#===============================================================================
# @brief    cmake component build
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
macro(get_obj_path output obj_root)
    set(ret)
    foreach(src ${REGD_SOURCES})
        # filter full path
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" relpath ${src})
        # make suffix to be .o
        list(APPEND ret ${obj_root}/${relpath}.o)
    endforeach()
    set(${output} ${ret})
endmacro()

macro(register_component)
    set(${COMPONENT_NAME}_MODULE_NAME ${MODULE_NAME} CACHE INTERNAL "" FORCE)
    set(${COMPONENT_NAME}_AUTO_DEF ${AUTO_DEF_FILE_ID} CACHE INTERNAL "" FORCE)
    set(${COMPONENT_NAME}_SOURCES ${SOURCES} CACHE INTERNAL "" FORCE)
endmacro(register_component)

macro(component_on_rom)
    if(${COMPONENT_NAME} IN_LIST RAM_COMPONENT)
        set(COMPONENT_ON_ROM false)
    elseif(${COMPONENT_NAME} IN_LIST ROM_COMPONENT)
        set(COMPONENT_ON_ROM true)
    else()
        unset(COMPONENT_ON_ROM)
    endif()
endmacro()

macro(need_whole_archiv)
    set(WHOLE_ARCHIV false)
    if (COMPONENT_ON_ROM STREQUAL true)
        set(WHOLE_ARCHIV true)
    endif()
    if(DEFINED WHOLE_LINK AND WHOLE_LINK STREQUAL true)
        set(WHOLE_ARCHIV true)
    endif()
    unset(WHOLE_LINK)
endmacro()

macro(replace_lib_output_path)
    install_lib_gen()
    if(DEFINED LIB)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB})
    endif()
    if(DEFINED LIB_OUT_PATH)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUT_PATH})
    endif()
    if(DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY AND ${BUILD_ROM_CALLBACK} AND ${COMPONENT_ON_ROM})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}-prerom)
    endif()
endmacro()

macro(build_library)
    set(lib_type STATIC)
    if(DEFINED BUILD_AS_OBJ AND BUILD_AS_OBJ STREQUAL true)
        set(lib_type OBJECT)
    endif()

    list(GET SOURCES 0 SOURCES0)
    get_filename_component(SOURCES0  "${SOURCES0}" ABSOLUTE)
    if(EXISTS ${SOURCES0})
        replace_lib_output_path()
        add_library(${COMPONENT_NAME}
            ${lib_type}
            "${SOURCES}"
        )

        target_compile_definitions(${COMPONENT_NAME}
            PRIVATE
            ${DEFINES}
            ${PRIVATE_DEFINES}
            ${PUBLIC_DEFINES}
        )

        target_include_directories(${COMPONENT_NAME}
            PRIVATE
            ${PRIVATE_HEADER}
            ${PUBLIC_HEADER}
	        $<$<COMPILE_LANGUAGE:CXX>:${CXX_HEADERS}>
        )

        target_compile_options(${COMPONENT_NAME}
            PRIVATE
            ${COMPILE_CCFLAGS}
            $<$<BOOL:${USE_KCONFIG}>:-include${PROJECT_BINARY_DIR}/mconfig.h>
        )

        set(LINK_LIB_${COMPONENT_NAME} ${COMPONENT_NAME})
        register_component()
        target_link_libraries(${COMPONENT_NAME} PRIVATE ${TARGETS_INTERFACES})
    else()
        if(DEFINED LIB)
        else()
            message(STATUS "${SOURCES0} is not found, finding lib${COMPONENT_NAME}.a in ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}")
            find_library(LINK_LIB_${COMPONENT_NAME} "lib${COMPONENT_NAME}.a" ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND})
            if(NOT LINK_LIB_${COMPONENT_NAME})
                message(FATAL_ERROR "${SOURCES0} is not found && lib${COMPONENT_NAME}.a not find")
            endif()
            install_sdk(${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a "*")
        endif()

    endif()

    if(WHOLE_ARCHIV STREQUAL false)
        # treat as a non-rom component
        target_link_libraries(${TARGET_NAME}
            PRIVATE
            ${LINK_LIB_${COMPONENT_NAME}}
        )
    else()
        # one on rom.
        target_link_libraries(${TARGET_NAME}
            PRIVATE
            -Wl,--whole-archive ${LINK_LIB_${COMPONENT_NAME}} -Wl,--no-whole-archive
        )
    endif()
endmacro()

macro(link_libs)
    if(DEFINED LIBS)
        if(WHOLE_ARCHIV STREQUAL false)
            target_link_libraries(${TARGET_NAME}
                PRIVATE
                ${LIBS}
            )
        else()
            target_link_libraries(${TARGET_NAME}
                PRIVATE
                -Wl,--whole-archive ${LIBS} -Wl,--no-whole-archive
            )
        endif()
    endif()
endmacro(link_libs)

macro(main_component)
    if(DEFINED MAIN_COMPONENT AND MAIN_COMPONENT STREQUAL true)
        set(SDK_PROJECT_FILE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "" FORCE)
        set(MAIN_COMPONENT "${COMPONENT_NAME}" CACHE INTERNAL "" FORCE)
    endif()
endmacro()

macro(add_to_logdef)
    if(DEFINED LOG_DEF)
        set(LOG_DEF_LIST  "${LOG_DEF_LIST}" "${LOG_DEF}" CACHE INTERNAL "" FORCE)
    endif()
endmacro(add_to_logdef)

macro(install_file)
    if (DEFINED SDK_OUTPUT_PATH)
        check_if_closed_component(${COMPONENT_NAME})
        if (${COMPONENT_IS_CLOSED})
            if (TARGET ${COMPONENT_NAME})
                get_property(LIB_PATH TARGET ${COMPONENT_NAME} PROPERTY BINARY_DIR)
            endif()

            if(DEFINED LIB)
                install_sdk(${LIB}/ "*")
            elseif(DEFINED LIB_OUT_PATH)
                install_dest_sdk(${LIB_OUT_PATH}/lib${COMPONENT_NAME}.a ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a "*.a")
            elseif(LIB_PATH)
                install_dest_sdk(${LIB_PATH}/lib${COMPONENT_NAME}.a ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a "*.a")
            endif()
        else()
            foreach(src ${SOURCES})
                install_sdk(${src} "*")
            endforeach()
            foreach(header ${PRIVATE_HEADER} ${PUBLIC_HEADER})
                install_sdk(${header} "*.h")
            endforeach()
        endif()

        foreach(lib ${LIBS})
            install_sdk(${lib} "*")
        endforeach()
    endif()
endmacro(install_file)

macro(define_this_module_and_file_id)
    foreach(filename ${SOURCES})
        if(filename STREQUAL  "")
            continue()
        endif()
        if(${AUTO_DEF_FILE_ID})
            string(STRIP ${filename} filename)
            get_filename_component(barename ${filename} NAME)
            string(REPLACE "." "_" barename ${barename})
            string(TOUPPER ${barename} barename)
            string(STRIP ${barename} barename)
            set_property(
                SOURCE ${filename}
                APPEND
                PROPERTY COMPILE_DEFINITIONS
                THIS_FILE_ID=${barename})
        endif()

        if(${AUTO_DEF_MODULE_ID})
            set(prefix "LOG_")
            set(suffix "MODULE")
            string(TOUPPER ${MODULE_NAME} mod_name)
            string(JOIN ${mod_name} mod_name ${prefix} ${suffix})
            set_property(
                SOURCE ${filename}
                APPEND
                PROPERTY COMPILE_DEFINITIONS
                THIS_MOD_ID=${mod_name})
        endif()
    endforeach()
endmacro(define_this_module_and_file_id)

macro(unset_var)
    UNSET(COMPONENT_NAME)
    UNSET(SOURCES)
    UNSET(PRIVATE_HEADER)
    UNSET(PUBLIC_HEADER)
    UNSET(PRIVATE_DEFINES)
    UNSET(PUBLIC_DEFINES)
    UNSET(COMPONENT_CCFLAGS)
    UNSET(COMPONENT_PUBLIC_CCFLAGS)
    UNSET(BUILD_AS_OBJ)
    UNSET(LOG_DEF)
    UNSET(WHOLE_LINK)
    UNSET(LINK_LIB_${COMPONENT_NAME})
    UNSET(LIB_OUT_PATH)
    UNSET(LIB_DIR)
    UNSET(LIBS)
    UNSET(LIB)
    UNSET(REWRITE_CCFLAGS)
    UNSET(COMPONENT_IS_CLOSED)
endmacro(unset_var)


macro(build_component)
    # check this component whether on the rom.
    # set(one_value ONROM)
    # set(multi_value SOURCES PRIVATE_HEADER PUBLIC_HEADER PRIVATE_DEFINES PUBLIC_DEFINES CCFLAGS)
    # cmake_parse_arguments(REGD "${option}" "${one_value}" "${multi_value}" ${ARGN})
    component_on_rom()
    if(DEFINED ROM_SYM_PATH AND COMPONENT_ON_ROM)
        add_library(${COMPONENT_NAME}_interface INTERFACE)
        target_include_directories(${COMPONENT_NAME}_interface INTERFACE ${PUBLIC_HEADER})
        target_compile_definitions(${COMPONENT_NAME}_interface INTERFACE ${PUBLIC_DEFINES})
        list(GET SOURCES 0 SOURCES0)
        get_filename_component(SOURCES0  "${SOURCES0}" ABSOLUTE)
        if(EXISTS ${SOURCES0})
            register_component()
        endif()
    elseif(DEFINED COMPONENT_ON_ROM)
        need_whole_archiv()
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/private)
            set(PRIVATE_HEADER ${PRIVATE_HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/private)
        endif()
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/public)
            set(PUBLIC_HEADER ${PUBLIC_HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/public)
        endif()
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include)
            set(PUBLIC_HEADER ${PUBLIC_HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/include)
        endif()
        if(COMPONENT_ON_ROM STREQUAL false)
            if(REWRITE_CCFLAGS STREQUAL true)
                set(COMPILE_CCFLAGS ${COMPONENT_CCFLAGS})
            else()
                # treat as a non-rom component
                set(COMPILE_CCFLAGS ${CCFLAGS} ${COMPONENT_CCFLAGS})
            endif()
        else()
            # one on rom.

            set(COMPILE_CCFLAGS ${ROM_CCFLAGS})
        endif()

        check_if_closed_component(${COMPONENT_NAME})
        if (RELEASE OR ${COMPONENT_IS_CLOSED})
            list(REMOVE_ITEM COMPILE_CCFLAGS "-g")
        endif()

        add_library(${COMPONENT_NAME}_interface INTERFACE)
        target_include_directories(${COMPONENT_NAME}_interface INTERFACE ${PUBLIC_HEADER})
        target_compile_definitions(${COMPONENT_NAME}_interface INTERFACE ${PUBLIC_DEFINES})
        target_compile_options(${COMPONENT_NAME}_interface INTERFACE ${COMPONENT_PUBLIC_CCFLAGS})
        set(TARGETS_INTERFACES ${RAM_COMPONENT} ${ROM_COMPONENT})
        list(TRANSFORM TARGETS_INTERFACES APPEND "_interface")

        list(LENGTH PUBLIC_HEADER PUBLIC_HEADER_NUM)
        if(NOT ${PUBLIC_HEADER_NUM} EQUAL 0)
            set(ALL_PUBLIC_HEADER "${ALL_PUBLIC_HEADER}" "${PUBLIC_HEADER}" CACHE INTERNAL "" FORCE)
        endif()

        if(DEFINED LOG_CUSTOM_ENABLE)
            if(${LOG_CUSTOM_ENABLE} STREQUAL True)
                if(NOT ${PUBLIC_HEADER_NUM} EQUAL 0)
                    set(ALL_HEADER_DIRS "${ALL_HEADER_DIRS}" "${PUBLIC_HEADER}" CACHE INTERNAL "" FORCE)
                endif()

                list(LENGTH PRIVATE_HEADER PRIVATE_HEADER_NUM)
                if(NOT ${PRIVATE_HEADER_NUM} EQUAL 0)
                    set(ALL_HEADER_DIRS "${ALL_HEADER_DIRS}" "${PRIVATE_HEADER}" CACHE INTERNAL "" FORCE)
                endif()

                list(LENGTH PUBLIC_DEFINES PUBLIC_DEFINES_NUM)
                if(NOT ${PUBLIC_DEFINES_NUM} EQUAL 0)
                    set(ALL_DEFINES "${ALL_DEFINES}" "${PUBLIC_DEFINES}" CACHE INTERNAL "" FORCE)
                endif()

                list(LENGTH PRIVATE_DEFINES PRIVATE_DEFINES_NUM)
                if(NOT ${PRIVATE_DEFINES_NUM} EQUAL 0)
                    set(ALL_DEFINES "${ALL_DEFINES}" "${PRIVATE_DEFINES}" CACHE INTERNAL "" FORCE)
                endif()

                list(LENGTH DEFINES DEFINES_NUM)
                if(NOT ${DEFINES_NUM} EQUAL 0)
                    set(ALL_DEFINES "${ALL_DEFINES}" "${DEFINES}" CACHE INTERNAL "" FORCE)
                endif()
            endif()
        endif()

        set(ALL_PUBLIC_DEFINES "${ALL_PUBLIC_DEFINES}" "${PUBLIC_DEFINES}" CACHE INTERNAL "" FORCE)
        set(ALL_PUBLIC_CCFLAGS "${ALL_PUBLIC_CCFLAGS}" "${COMPONENT_PUBLIC_CCFLAGS}" CACHE INTERNAL "" FORCE)
        link_libs()
        list(LENGTH SOURCES SOURCES_NUM)
        if(NOT ${SOURCES_NUM} EQUAL 0)
            build_library()
        endif()
        if(${BUILD_ROM_CALLBACK} AND ${COMPONENT_ON_ROM})
            reg_rom_callback()
        endif()
        define_this_module_and_file_id()
        sdk_export_component("component")
        install_file()
        add_to_logdef()
        main_component()
    endif()
    unset_var()
endmacro()
