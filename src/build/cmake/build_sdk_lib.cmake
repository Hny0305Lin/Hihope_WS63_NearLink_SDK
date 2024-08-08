#===============================================================================
# @brief    cmake sdk build
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================


if ("${GEN_SEC_BIN}" STREQUAL "True")
    if (NOT EXISTS ${COMPILER_ROOT}/lib/sec)
        file(MAKE_DIRECTORY ${COMPILER_ROOT}/lib/sec)
    endif()
    if (NOT EXISTS ${COMPILER_ROOT}_win/lib/sec)
        file(MAKE_DIRECTORY ${COMPILER_ROOT}_win/lib/sec)
    endif()
    # tools libs
    execute_process(COMMAND ${CP} ${ROOT_DIR}/tools/bin/sec_tool/lib/sec/${IMAGE_VERSION}/libsec_image.so ${ROOT_DIR}/tools/bin/sec_tool/lib/sec/libsec_image.so)
    execute_process(COMMAND ${CP} ${ROOT_DIR}/tools/bin/sec_tool/lib/sec/${IMAGE_VERSION}/libsec_image.pyd ${ROOT_DIR}/tools/bin/sec_tool/lib/sec/libsec_image.pyd)
    execute_process(COMMAND ${CP} ${ROOT_DIR}/tools/bin/sec_tool/lib/sec/${IMAGE_VERSION}/libsec_image.so ${COMPILER_ROOT}/lib/sec/libsec_image.so)
    execute_process(COMMAND ${CP} ${ROOT_DIR}/tools/bin/sec_tool/lib/sec/${IMAGE_VERSION}/libsec_image.pyd ${COMPILER_ROOT}_win/lib/sec/libsec_image.pyd)
    # linux
    execute_process(COMMAND ${CP} ${ROOT_DIR}/tools/bin/sec_tool/linux/riscv32-linux-musl-objcopy ${COMPILER_ROOT}/bin/riscv32-linux-musl-objcopy)
    execute_process(COMMAND ${CP} ${ROOT_DIR}/tools/bin/sec_tool/linux/riscv32-linux-secmain ${COMPILER_ROOT}/bin/riscv32-linux-secmain)

    if(${CORE} STREQUAL "acore")
        # win: only acore will build win.
        execute_process(COMMAND ${CP} ${ROOT_DIR}/tools/bin/sec_tool/win/riscv32-linux-musl-objcopy.exe ${COMPILER_ROOT}_win/bin/riscv32-linux-musl-objcopy.exe)
        execute_process(COMMAND ${CP} ${ROOT_DIR}/tools/bin/sec_tool/win/riscv32-linux-secmain.exe ${COMPILER_ROOT}_win/bin/riscv32-linux-secmain.exe)
        # image info
        if (EXISTS ${ROOT_DIR}/middleware/chips/${CHIP}/image_info/${IMAGE_VERSION}/libapp_info_nofp.a)
            execute_process(COMMAND ${CP} ${ROOT_DIR}/middleware/chips/${CHIP}/image_info/${IMAGE_VERSION}/libapp_info_nofp.a ${ROOT_DIR}/middleware/chips/${CHIP}/image_info/libapp_info_nofp.a)
        endif()

        if (EXISTS ${ROOT_DIR}/middleware/chips/${CHIP}/image_info/${IMAGE_VERSION}/libimage_info.a)
            execute_process(COMMAND ${CP} ${ROOT_DIR}/middleware/chips/${CHIP}/image_info/${IMAGE_VERSION}/libimage_info.a ${ROOT_DIR}/middleware/chips/${CHIP}/image_info/libimage_info.a)
        endif()

    elseif(${CORE} STREQUAL "bt_core")
        if (EXISTS ${ROOT_DIR}/application/${CHIP}/${CHIP}_bgle/common/lib/image_info/${IMAGE_VERSION}/libimage_info.a)
            execute_process(COMMAND ${CP} ${ROOT_DIR}/application/${CHIP}/${CHIP}_bgle/common/lib/image_info/${IMAGE_VERSION}/libimage_info.a ${ROOT_DIR}/application/${CHIP}/${CHIP}_bgle/common/lib/image_info/libimage_info.a)
        endif()
    endif()
endif()
