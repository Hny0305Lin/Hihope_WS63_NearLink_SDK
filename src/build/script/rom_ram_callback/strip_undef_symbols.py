#!/usr/bin/env python
# coding:utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.

import sys
import os
import re
import time

undef_rom_symbol_list = []
undef_result_symbols_list = []
# 在其他rom中定义的符号
undef_rom_defined_list = []
rom_defined_list = []

none_path = "null"

argv_list = {
    "undef_raw_file": none_path,
    "rom_symbol_file": none_path,
    "undef_file": none_path
}

input_file_list = {
    "undef_raw_file": "rom_bin_raw.undef",
    "rom_symbol_file": "rom_symbol.list",
    "undef_file": "rom_bin.undef"
}


def check_file_exist(path):
    if not os.path.exists(path):
        err_msg = "path: " + path + " not exist!!"
        print_manual(err_msg)
        sys.exit(1)


def undef_symbol_load():
    global undef_rom_symbol_list
    undef_rom_symbol_list = []
    check_file_exist(argv_list['undef_raw_file'])
    with open(argv_list['undef_raw_file'], "r") as undef_file:
        line = undef_file.readline()
        while line:
            line = line.replace('\r', '')
            line = line.replace('\n', '')
            if line.split().__len__() < 2:
                print("error format for %s file, str=%s" % ('undef_raw_file', line))
                sys.exit(1)
            undef_symbol_name = line.split()[1]
            # print("undef %s duplicate" % undef_symbol_name)
            if undef_rom_symbol_list.count(undef_symbol_name) <= 0:
                # 所有undefine 符号集合， 但是有的undefine符号有可能是在其他rom文件中定义，所以后面还要筛选
                undef_rom_symbol_list.append(undef_symbol_name)
            # else:
                # print("undef %s duplicate" % undef_symbol_name)
            line = undef_file.readline()
        undef_rom_symbol_list.sort()


def rom_defined_symbol_load():
    global rom_defined_list
    check_file_exist(argv_list['rom_symbol_file'])
    with open(argv_list['rom_symbol_file'], "r") as image_elf:
        rom_defined_list = []
        line = image_elf.readline()
        count = 0
        while line:
            line = line.replace('\r', '')
            line = line.replace('\n', '')

            if (line.split().__len__() != 8) or len(line) <= 0 or (line.split()[4] != "GLOBAL"):
                # if line.split().__len__() == 8:
                #     print("image1 elf: %s %s %s" % (line.split()[3], line.split()[4], line.split()[7]))
                line = image_elf.readline()
                continue
            else:
                symbol_name = line.split()[7]
                if (line.split()[3] != "FUNC") and (line.split()[3] != "OBJECT"):
                    line = image_elf.readline()
                    continue
                if rom_defined_list.count(symbol_name) <= 0:
                    # 添加所有定义过的符号
                    rom_defined_list.append(symbol_name)
                    count = count + 1
                    # print("D %s" % symbol_name)
            line = image_elf.readline()
        print("all objs defined %d data & functions" % count)


def gen_rom_undef_target_file():
    global undef_rom_defined_list
    global undef_result_symbols_list
    undef_cnt = 0
    def_cnt = 0
    with open(argv_list['undef_file'], "w+") as rom_undef:
        undef_result_symbols_list = []
        undef_rom_defined_list = []
        for undef_s in undef_rom_symbol_list:
            # 在rom代码中没有定义，是undef符号
            if rom_defined_list.count(undef_s) <= 0:
                # print("undef %s duplicate" % undef_s)
                undef_result_symbols_list.append(undef_s)
                undef_cnt = undef_cnt + 1
            else:
                undef_rom_defined_list.append(undef_s)
                def_cnt = def_cnt + 1

        undef_result_symbols_list.sort()
        undef_rom_defined_list.sort()
        for undef_s in undef_result_symbols_list:
            print("undef %s duplicate" % undef_s)
            print("U %s" % undef_s, file=rom_undef)
        print("all objs had %d undef symbols, and %d defined symbols" % (undef_cnt, def_cnt))

def print_manual(err_msg):
    print("%s " % err_msg)
    print("%s argument=xxx" % sys.argv[0])
    print("arguments: ")
    for key in argv_list:
        print("%s=..." % key)


def dump_all_arguments():
    print("input arguments information: ")
    for key in argv_list:
        print("argument: %s=%s" % (key, argv_list[key]))


def print_unkown_arg(msg):
    err_msg = "unknown argument: " + msg
    print_manual(err_msg)
    sys.exit(1)


def print_input_arg(msg):
    print("%s" % msg)
    for arg in sys.argv[1:]:
        print("%s" % arg)


def parse_args(output):
    # 解析多个参数 *.py file=
    for key, value in argv_list.items():
        file_path = os.path.join(output, input_file_list[key])
        # check_file_exist(file_path)
        argv_list[key] = file_path
    dump_all_arguments()


# input argvs:
# output argvs
def strip_undefined_symbols(output):
    parse_args(output)
    undef_symbol_load()
    rom_defined_symbol_load()
    gen_rom_undef_target_file()