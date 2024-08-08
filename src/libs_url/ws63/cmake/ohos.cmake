#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_LIST "bootstrap" "broadcast" "samgr" "native_file" "hilog_lite_static" "hiview_lite_static" "hievent_lite_static"
    "samgr_source" "samgr_adapter" "hal_file_static" "hal_sysparam" "param_client_lite" "inithook" "init_utils" "begetutil"
    "hctest" "init_log" "hilog_static" "hota" "hal_update_static" "huks_3.0_sdk" "huks_test_common"
    "udidcomm" "devattest_core" "devattest_sdk" "cjson_static" "hal_token_static"
    "module_ActsParameterTest" "module_ActsBootstrapTest" "module_ActsSamgrTest" "module_ActsDfxFuncTest"
    "module_ActsHieventLiteTest" "module_ActsUtilsFileTest" "module_ActsUpdaterFuncTest" "module_ActsHuksHalFunctionTest")

foreach(COMPONENT ${COMPONENT_LIST})
    set(COMPONENT_NAME ${COMPONENT})
    if (NOT ${COMPONENT_NAME} IN_LIST TARGET_COMPONENT)
        continue()
    endif()
    set(LIBS ${BIN_DIR}/${CHIP}/libs/ohos/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    set(WHOLE_LINK true)
    build_component()
    install_sdk(${BIN_DIR}/${CHIP}/libs/ohos/${TARGET_COMMAND} "*.a")
endforeach()
