#===============================================================================
# @brief    cmake component build
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

add_custom_target(HSO_DB
    COMMENT "Generating HSO_DB ${TARGET_NAME}"
)
if(DEFINED LOG_CUSTOM_ENABLE)
    if(${LOG_CUSTOM_ENABLE} STREQUAL True)
        include(${CMAKE_SOURCE_DIR}/build/script/hdbxml_custom/MessageXmlGen/messagexmlgen.cmake)
    endif()
endif()

function(create_hso_db)
    if(DEFINED HSO_XML_CHIP)
        set(XML_CHIP ${HSO_XML_CHIP})
    else()
        set(XML_CHIP ${CHIP})
    endif()

    set(XML_FILE ${ROOT_DIR}/build/config/target_config/${XML_CHIP}/hdb_config/database_template/acore/system/hdbcfg/mss_cmd_db.xml)
    add_custom_target(HSO_DB_MKDIR
        COMMAND ${Python3_EXECUTABLE} ${MAK_HSO_XML} mkdir ${ROOT_DIR}/ ${XML_CHIP} ${CORE}
        COMMENT "HSO_DB Makedir"
    )

    if(DEFINED LOG_CUSTOM_ENABLE)
        if(${LOG_CUSTOM_ENABLE} STREQUAL True)
            build_xml()
        endif()
    endif()

    foreach(COMPONENT ${TARGET_COMPONENT})
        if (NOT DEFINED ${COMPONENT}_SOURCES)
            continue()
        endif()

        set(CLOSED_FLAG "False")
        check_if_closed_component(${COMPONENT})
        if (${COMPONENT_IS_CLOSED})
            set(CLOSED_FLAG "True")
        endif()
        list(JOIN LOG_DEF_LIST "," LOG_DEF)
        list(JOIN ${COMPONENT}_SOURCES "," SOURCE)
        # for windows command length limit
        file(WRITE "${PROJECT_BINARY_DIR}/hso_temp/${COMPONENT}_temp.txt" "${LOG_DEF}####${SOURCE}")
        add_custom_command(
            OUTPUT ${PROJECT_BINARY_DIR}/hso_temp/${COMPONENT}.txt
            # for re-run HDB
            COMMAND ${CP} ${PROJECT_BINARY_DIR}/hso_temp/${COMPONENT}_temp.txt ${PROJECT_BINARY_DIR}/hso_temp/${COMPONENT}.txt
            COMMAND ${Python3_EXECUTABLE} ${MAK_HSO_XML} ${ROOT_DIR}/ ${XML_CHIP} ${CORE} ${ARCH} ${${COMPONENT}_AUTO_DEF} ${${COMPONENT}_MODULE_NAME} ${CLOSED_FLAG} ${PROJECT_BINARY_DIR}/hso_temp/${COMPONENT}.txt
            COMMENT "Building HSO_DB_${COMPONENT}"
            DEPENDS  "${${COMPONENT}_SOURCES}" ${LOG_DEF_LIST} HSO_DB_MKDIR
        )
        add_custom_target(HSO_DB_${COMPONENT}
            DEPENDS ${PROJECT_BINARY_DIR}/hso_temp/${COMPONENT}.txt
        )
        add_dependencies(HSO_DB HSO_DB_${COMPONENT})
    endforeach ()
    if(NOT DEFINED HSO_ENABLE_BT)
        set(HSO_ENABLE_BT FALSE)
    endif()
    if(${CORE} STREQUAL "bt_core" OR ${HSO_ENABLE_BT} STREQUAL "True")
        add_custom_target(HSO_DB_BT_STATUS
            COMMAND ${Python3_EXECUTABLE} ${BT_STATUS_HSO_XML} ${ROOT_DIR}/ ${XML_CHIP}
            COMMAND ${Python3_EXECUTABLE} ${OTA_MSG_HSO_XML}   ${ROOT_DIR}/ ${XML_CHIP}
        )
        add_dependencies(HSO_DB HSO_DB_BT_STATUS)
    endif()

    add_custom_command(TARGET HSO_DB POST_BUILD
        COMMENT "Merge HSO_XML & Create HSO_DB"
        COMMAND ${Python3_EXECUTABLE} ${HSO_XML_PRE_PROCESS} ${ROOT_DIR}/ ${XML_CHIP} ${CORE}
        COMMAND ${Python3_EXECUTABLE} ${HSO_XML_MERGE} ${ROOT_DIR}/ ${XML_CHIP} ${CORE} "${HSO_ENABLE_BT}"
        COMMAND ${Python3_EXECUTABLE} ${HSO_XML_DB_CREATE} ${ROOT_DIR}/ ${XML_CHIP}
    )
    if(NOT DEFINED GEN_PARSE_TOOL)
        set(HSO_ENABLE_BT FALSE)
    endif()

    if("${GEN_PARSE_TOOL}" STREQUAL "True")
        set(INFO_FILE ${PROJECT_BINARY_DIR}/${BIN_NAME}.info)
        set(NM_FILE ${PROJECT_BINARY_DIR}/${BIN_NAME}.nm)
        set(PARSE_TOOL_DIR "${PROJECT_BINARY_DIR}/parse_tool")
        add_custom_command(TARGET HSO_DB POST_BUILD
            COMMAND ${Python3_EXECUTABLE} ${HSO_PARSE_MAIN} ${PARSE_TOOL_DIR} ${INFO_FILE} ${NM_FILE} ${XML_FILE}
            COMMAND ${CP_PY} ${HSO_PARSE_DIR}/ ${PARSE_TOOL_DIR} "none" *.py
        )
    endif()
endfunction(create_hso_db)
