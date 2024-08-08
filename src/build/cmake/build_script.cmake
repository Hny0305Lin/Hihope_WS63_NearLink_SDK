#===============================================================================
# @brief    cmake script path
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(SCRIPT_DIR "${ROOT_DIR}/build/script")

set(PRO_GEN ${SCRIPT_DIR}/sdk_generator/project_generator.py)
set(BUILD_UTILS ${SCRIPT_DIR}/utils/build_utils.py)
set(ELF_TO_DU ${SCRIPT_DIR}/utils/elftodu.py)
set(MEM_STATE ${SCRIPT_DIR}/utils/mem_stats.py)
set(PARSE_MAP_SIZE_INFO ${SCRIPT_DIR}/utils/parse_map_size_info.py)
set(GEN_HEX ${SCRIPT_DIR}/utils/create_hex.py)
set(GEN_PATCH ${SCRIPT_DIR}/patch/patch_${ARCH_FAMILY}.py)

# HSO script
set(HDB_SCRIPT_DIR ${SCRIPT_DIR}/hdbxml)
set(MAK_HSO_XML ${HDB_SCRIPT_DIR}/mk_hso_prim_xml.py)
set(BT_STATUS_HSO_XML ${HDB_SCRIPT_DIR}/bt_status_to_hso_prim_xml.py)
set(OTA_MSG_HSO_XML ${HDB_SCRIPT_DIR}/ota_msg_to_hso_prim_xml.py)
set(HSO_XML_MERGE ${HDB_SCRIPT_DIR}/hso_prim_xml_merge.py)
set(HSO_XML_DB_CREATE ${HDB_SCRIPT_DIR}/database_create.py)
set(HSO_XML_PRE_PROCESS ${HDB_SCRIPT_DIR}/process_pregenerated_xml.py)

# PARSE script
set(OUTPUT_DIR "${ROOT_DIR}/output")
set(HSO_PARSE_DIR ${SCRIPT_DIR}/parse_tool)
set(HSO_PARSE_MAIN ${HSO_PARSE_DIR}/parse_main_phase1.py)

# sign tool
set(SIGN_TOOL ${ROOT_DIR}/tools/bin/sign_tool/sign_tool_pltuni)

# concat tool
set(CONCAT_TOOL ${ROOT_DIR}/build/script/concat_bin.py)

# build nv tool
set(BUILD_NV_TOOL ${ROOT_DIR}/build/script/build_nvbin.py)
set(BUILD_NV_GEN_UTILS ${ROOT_DIR}/build/script/nv/generate_utils.py)

# sec tool
if(${BUILD_PLATFORM} MATCHES "linux")
set(SEC_OBJCPY_TOOL ${ROOT_DIR}/tools/bin/sec_tool/linux/riscv32-linux-musl-objcopy)
set(SEC_TOOL_DIR ${ROOT_DIR}/tools/bin/sec_tool/linux)
elseif(${BUILD_PLATFORM} MATCHES "windows")
set(SEC_OBJCPY_TOOL ${ROOT_DIR}/tools/bin/sec_tool/win/riscv32-linux-musl-objcopy.exe)
set(SEC_TOOL_DIR ${ROOT_DIR}/tools/bin/sec_tool/win)
endif()

# rom info gen script
set(GEN_ROM_INFO_TOOL ${ROOT_DIR}/tools/bin/remote_lib_tool/remote_lib_sym2addr.py)

# IMAGE ANALYSIS script
set(IMAGE_ANALSIS_TOOL ${SCRIPT_DIR}/image_analysis/build_data_analyzer_allinone.py)

# CODESIZE_STATISTIC script
set(CODESIZE_STATISTIC_TOOL ${SCRIPT_DIR}/codesize_statistic.py)
