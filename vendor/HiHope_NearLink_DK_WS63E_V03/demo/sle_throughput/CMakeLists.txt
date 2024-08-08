#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_SERVER_SPEED)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/sle_server_speed/src/sle_server_adv.c" "${CMAKE_CURRENT_SOURCE_DIR}/sle_server_speed/src/sle_uuid_server.c" "${CMAKE_CURRENT_SOURCE_DIR}/sle_server_speed/inc" PARENT_SCOPE)
elseif(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_CLIENT_SPEED)
    set(SOURCES "${SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/sle_client_speed/src/sle_uuid_client.c" "${CMAKE_CURRENT_SOURCE_DIR}/sle_client_speed/inc" PARENT_SCOPE)
endif()