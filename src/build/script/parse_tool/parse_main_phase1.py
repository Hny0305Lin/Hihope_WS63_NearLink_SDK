#!/usr/bin/env python
# coding:utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.

import sys,string,re,os
sys.path.append(os.path.split(os.path.realpath(__file__))[0])
from ctypes import *
from parse_elf import *
import json

if __name__ == '__main__':
    print("Running parse step1 at", sys.executable,
          " version:%d.%d.%d " % (sys.version_info[0], sys.version_info[1], sys.version_info[2]))
    out_dir = sys.argv[1]
    debug_file = sys.argv[2]
    nm_file = sys.argv[3]
    xml_file = sys.argv[4]

    if not os.path.isdir(out_dir):
        os.makedirs(out_dir)
    parse_elf_step_1(debug_file,
        os.path.join(out_dir, 'auto_class.py'),
        os.path.join(out_dir, 'auto_struct.txt'),
        nm_file,
        os.path.join(out_dir, 'global.txt'),
        out_dir,
        xml_file
    )
    print("Build parse tool success.")
