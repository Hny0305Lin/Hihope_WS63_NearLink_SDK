# Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.

## ATTENTION: api_inc.cmake is about external api header file's path. ##
## DON'T ADD OTHER VARS IN IT ##

if(LOSCFG_SHELL)
    set(LITEOS_SHELL_INCLUDE ${LITEOSTOPDIR}/shell/include)
endif()
