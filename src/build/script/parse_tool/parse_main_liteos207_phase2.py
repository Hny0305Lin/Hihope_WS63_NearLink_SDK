#!/usr/bin/env python
# coding:utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.

import sys,string,re,os
from ctypes import *
from parse_elf import *
from parse_liteos import *
from parse_module_diag import *
from auto_class import *
from parse_basic import *
from parse_print_global_var import *
import json
import argparse

if __name__ == '__main__':
    print("Running python at", sys.executable,
          " version:%d.%d.%d " % (sys.version_info[0], sys.version_info[1], sys.version_info[2]))
    argp = argparse.ArgumentParser(description='Parse entry step2:')
    argp.add_argument('--nm_file', type=str, help='nm file path.')
    argp.add_argument('--bin_list', type=str, help='-bin_list:"name1|addr1|size1|name2|addr2|size2".')
    argp.add_argument('--result', type=str, help='memory result file')
    argp.add_argument('--global_file', type=str, help='global file path')
    args = argp.parse_args()
    print("args:")
    print(args)

    bin_list = args.bin_list.split("|")
    key_list = ["name", "addr", "size"]
    bin_dict = {}
    temp_list = []
    for i in range(len(bin_list)):
        bin_dict[key_list[i%3]] = bin_list[i]
        if (i%3 == 2):
            temp_list.append(bin_dict)
            bin_dict = {}
    parse_elf_step_2(args.nm_file)
    for item in temp_list:
        addr = int(item["addr"])
        size = int(item["size"])
        parse_memory_register(item["name"], addr, size)

    with open(args.result,'w+') as log_fp:
        parse_liteos207_info(log_fp)
        try:
            parse_print_global_var(args.global_file, log_fp)
        except:
            print("[EXCEPTION]parse_print_global_var fail.")




