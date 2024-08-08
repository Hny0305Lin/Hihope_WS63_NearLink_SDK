#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Build nvbin
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# ============================================================================

import os
import sys

file_dir = os.path.dirname(os.path.realpath(__file__))
g_root = os.path.realpath(os.path.join(file_dir, "..", ".."))
sys.path.append(os.path.join(g_root, 'build', 'script', 'nv'))
from nv_binary import nv_begin

if __name__ == '__main__':
    args = sys.argv[1:]
    if (len(args) != 3) or (not os.path.exists(args[0])):
        print("input args is invalid!\r\n")
    else:
        nv_begin(args[0], args[1], 0, False, args[2])
