# Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.

list(APPEND LOCAL_SRCS_y src/${LITEOS_ARCH_RISCV}/runstop.S)

list(REMOVE_ITEM LOCAL_SRCS_y src/trap.S)
list(APPEND LOCAL_SRCS_y src/${LITEOS_ARCH_RISCV}/trap.S)