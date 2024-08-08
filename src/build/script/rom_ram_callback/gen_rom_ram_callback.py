#!/usr/bin/env python
# coding:utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2030. All rights reserved.

import sys
import os
import re
import time

# .o中得到的undef symbols
undef_rom_symbol_list = []
# 待处理的undef symbls, 全局函数和全局变量，再通过elf目标区分符号的类型
undef_result_symbols_list = []
# 需要处理的rom-ram 函数符号列表
rom_func_list = []
# 需要处理的rom-ram 数据区符号列表
rom_data_list = []
# 特殊undef symbols过滤列表
undef_list_filter = []
# 包含所有全局链接符号的字典
dict_all_symbols_glb = dict()
# 被优化或者特殊ROM-RAM call符号列表
rom_ram_call_leacy_list = []

# 长跳中转函数后缀名
romcallback_suffix_str = "_veneer_rom"
romcallback_prefix_str = "__wrap_"
romcallback_real_prefix = "__real_"

# filenames from outside
# undef symbols from elf
# nm -u ~/wifi_rom.o |awk '{print $NF}' > ~/wifi_rom.undef
undef_file_str = "undef_file"

# undef symbols filter list, remove special symbols from undef_file
undef_filter_str = "undef_filter"

# relocation information file, include all rel info for undef symbols, file maybe huge large
# readelf -W -r  ~/wifi_rom.o > ~/wifi_rom.rel
rel_file_str = "rel_file"

# all compile symbols from elf file
# readelf -W -s ~/asic.elf > ~/image_symbols.list
image_elf_str = "image_elf"

# link script section config file, rw section config list
region_file_str = "region_file"

# result files
# source code auto gen
# veneer long jump assemable code
rom_file_str = "rom_file"
# redefine macros for undef function symbols
auto_redefine_macro_file_str = "auto_redefine_macro_file"
# undef symbols function & data report list
rom_ram_call_func_file_str = "rom_ram_call_func_file"
rom_ram_call_data_file_str = "rom_ram_call_data_file"
rom_ram_call_leacy_file_str = "rom_ram_call_leacy_file"
rom_ram_data_leacy_file_str = "data_leacy_file"
# rom-ram function & data fix section define link scripts
lds_data_file_str = "lds_data_file"
func_lds_file_str = "func_lds_file"

none_path = "null"

argv_list = {
    undef_file_str: none_path,
    undef_filter_str: none_path,
    rel_file_str: none_path,
    image_elf_str: none_path,
    region_file_str: none_path,
    rom_file_str: none_path,
    auto_redefine_macro_file_str: none_path,
    rom_ram_call_func_file_str: none_path,
    rom_ram_call_data_file_str: none_path,
    rom_ram_call_leacy_file_str: none_path,
    lds_data_file_str: none_path,
    func_lds_file_str: none_path,
    rom_ram_data_leacy_file_str: none_path,
    # cpu target(arm32, riscv32.)
    "target": "arm32"
}

input_file_list = {
    undef_file_str: "rom_bin.undef",
    undef_filter_str: "undef_rom_filter.list",
    rel_file_str: "rom_bin.rel",
    image_elf_str: "image_symbol.list",
    region_file_str: "region.list",
    rom_file_str: "rom_callback.S",
    auto_redefine_macro_file_str: "rom_callback_wrap.cmake",
    rom_ram_call_func_file_str: "rom_ram_func.list",
    rom_ram_call_data_file_str: "rom_ram_call_data.list",
    rom_ram_call_leacy_file_str: "rom_ram_call_leacy.list",
    lds_data_file_str: "data.lds",
    func_lds_file_str: "function.lds",
    rom_ram_data_leacy_file_str: "data_leacy.list"
}


def check_file_exist(path):
    if not os.path.exists(path):
        err_msg = "path: " + path + " not exist!!"
        print_manual(err_msg)
        sys.exit(1)


# 根据数据符号名查找地址判断符号是zi还是rw符号，根据配置文件定义生成链接脚本定义，在lds中自己调用
def rom_parse_region_file_list():
    rom_data_list_dict = dict()
    for var in rom_data_list:
        rom_data_list_dict[var] = dict_all_symbols_glb.get(var)
    check_file_exist(argv_list[region_file_str])
    with open(argv_list[region_file_str], "r") as region_file, open(argv_list[lds_data_file_str], "w+") as lds_data_file:
        line = region_file.readline()
        while line:
            line = line.replace('\r', '')
            line = line.replace('\n', '')
            if line.find('#', 0, 1) >= 0:
                line = region_file.readline()
                continue
            if line.split(",").__len__() != 4:
                print("invalid region item:%s, must be 4 cols" % line)
                sys.exit(1)
            region_len = line.split(",").__len__()
            region_type = line.split(",")[0]
            region_name = line.split(",")[1]
            region_begin = line.split(",")[2]
            region_end = line.split(",")[3]
            if not dict_all_symbols_glb.get(region_begin):
                # 找不到_SXXXX 或者_EXXXX 直接报错
                print("can't found %s in image symbols list, break" % region_begin)
                sys.exit(1)
            if not dict_all_symbols_glb.get(region_end):
                # 找不到_SXXXX 或者_EXXXX 直接报错
                print("can't found %s in image symbols list, break" % region_end)
                sys.exit(1)
            # 处理数据符号
            region_begin_value = dict_all_symbols_glb[region_begin]
            region_end_value = dict_all_symbols_glb[region_end]
            # print("region %s:%s [0x%x:0x%x]" % (region_begin, region_end, region_begin_value, region_end_value))
            if region_end_value == region_begin_value:
                # 区间相等，为空段
                # print("empty region, skip")
                print("#define %s" % region_name, file=lds_data_file)
            elif region_end_value < region_begin_value:
                print("invalid region range %s:%s [0x%x:0x%x]" % (
                    region_begin, region_end, region_begin_value, region_end_value))
                sys.exit(1)
            else:
                # 遍历所有的数据符号，加入对应的分区
                # 遍历所有的需要处理的变量符号（包括data & bss）
                lds_str = ""
                for var in rom_data_list:
                    if not dict_all_symbols_glb.get(var):
                        # fatal err
                        print("can't found %s in dict" % var)
                    if not rom_data_list_dict.get(var):
                        # 当前符号已经处理过
                        print("%s been dealt with in dict" % var)
                        continue
                    var_address = dict_all_symbols_glb[var]
                    if (var_address >= region_begin_value) and (var_address < region_end_value):
                        # 符号地址落在该region范围内
                        # print("%s in region %s" % (var, line))
                        lds_str = ("%sKEEP(*.o*(.%s.%s));" % (lds_str, region_type, var))
                        del rom_data_list_dict[var]
                print("#ifdef _PRE_FEATURE_VENEER_ROM", file=lds_data_file)
                print("#define %s _S%s = .;%s; _E%s = .;" % (region_name, region_name, lds_str, region_name), file=lds_data_file)
                print("#else", file=lds_data_file)
                print("#define %s _S%s = .;%s; _E%s = .;" % (region_name, region_name, "", region_name), file=lds_data_file)
                print("#endif", file=lds_data_file)
            line = region_file.readline()

    # 保存没有被处理的变量符号，这些符号不在region list 范围内，可能是指定地址存放，不需要脚本处理
    with open(argv_list[rom_ram_data_leacy_file_str], "w+") as rom_ram_data_leacy_file:
        for key in rom_data_list_dict:
            print("0x%x    %s" % (rom_data_list_dict[key], key), file=rom_ram_data_leacy_file)
    return True


# 生成函数段定义链接脚本宏文件
def rom_function_lds_gen():
    with open(argv_list[func_lds_file_str], "w+") as func_lds_file:
        lds_str = ""
        for var in rom_func_list:
            # 中转函数按照8字节对齐，2条4字节指令，riscv短指令只有4字节，ARM统一8字节, 一律按照8字节对齐
            lds_str = ("%s . = ALIGN(8); KEEP(*.o*(.%s.romcall.text));" % (lds_str, var))
        print("#ifdef _PRE_FEATURE_VENEER_ROM", file=func_lds_file)
        #  跳转函数，不区分模块
        print("#define VENEER_TEXT _SVENEER_TEXT = .;%s_EVENEER_TEXT = .;. = ALIGN(8);" % lds_str, file=func_lds_file)
        print("#else", file=func_lds_file)
        print("#define VENEER_TEXT", file=func_lds_file)
        print("#endif", file=func_lds_file)


rom_rel_ro_list = ['Relocation section \'.rel.text',
                   'Relocation section \'.rel.rodata',
                   'Relocation section \'.rela.text',
                   'Relocation section \'.rela.rodata']


def rom_rel_check_ro_section(line):
    for ro in rom_rel_ro_list:
        if line.find(ro) >= 0:
            return True
    return False


# 解析rom.o中的代码段范围内undef符号列表
def rom_rel_load():
    # global undef_rom_symbol_list
    check_file_exist(argv_list[rel_file_str])
    with open(argv_list[rel_file_str], "r") as rel_file:
        line = rel_file.readline()
        while line:
            # line = rel_file.readline()
            if rom_rel_check_ro_section(line):
                # print("%s" % line)
                # skip Offset row
                line = rel_file.readline()
                line = rel_file.readline()
                while line:
                    if line.find('0', 0, 1) < 0:
                        break
                    if line.split().__len__() < 5:
                        line = rel_file.readline()
                        continue
                    # print("%s" % line)
                    # print ("%s %s" % (line.split()[2],line.split()[4]))
                    rel_type = line.split()[2]
                    # rel 文件 第5列表示符号名称， 如果解析错误，说明readelf 版本可能不匹配
                    rel_symbol_name = line.split()[4]
                    # print("%s %s" % (rel_type, rel_symbol_name))
                    if undef_rom_symbol_list.count(rel_symbol_name) <= 0:
                        line = rel_file.readline()
                        continue
                    if undef_result_symbols_list.count(rel_symbol_name) <= 0:
                        undef_result_symbols_list.append(rel_symbol_name)
                    # alg_autorate_init_user_legacy_rate_set_capability_extend
                    # print("%s" % rel_symbol_name)
                    line = rel_file.readline()
            else:
                line = rel_file.readline()


# 根据elf信息给undef symbols分类，函数还是数据类型
def undef_symbols_type_process():
    global undef_result_symbols_list
    func_list = []
    data_list = []
    check_file_exist(argv_list[image_elf_str])
    with open(argv_list[image_elf_str], "r") as image_elf:
        line = image_elf.readline()
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
                symbol_address = int(line.split()[1], 16)
                if dict_all_symbols_glb.get(symbol_name):
                    print("symbol name:%s repeat" % symbol_name)
                else:
                    dict_all_symbols_glb[symbol_name] = symbol_address
                    # print("add dict %s => %s" % (symbol_name, symbol_address))
                # print("image2 elf: %s %s %s" % (line.split()[3], line.split()[4], line.split()[7]))
                if line.split()[3] == "FUNC":
                    # print("FUNC: %s" % line.split()[7])
                    func_list.append(line.split()[7])
                if line.split()[3] == "OBJECT":
                    # print("DATA: %s" % line.split()[7])
                    data_list.append(line.split()[7])
            line = image_elf.readline()

    for symbol_i in undef_result_symbols_list:
        # print("symbol_i: %s" % symbol_i)
        if func_list.count(symbol_i) > 0:
            # 类型是函数,去重
            # print("symbol_i: %s" % symbol_i)
            if rom_func_list.count(symbol_i) <= 0:
                rom_func_list.append(symbol_i)
        elif data_list.count(symbol_i) > 0:
            # 类型是数据,去重
            # print("symbol_i: %s" % symbol_i)
            if rom_data_list.count(symbol_i) <= 0:
                rom_data_list.append(symbol_i)
        else:
            # 未识别符号，可能被编译器优化或者是特殊定义符号
            print("unkown undef symbol type: %s, not function or data or removed when ld" % symbol_i)
            rom_ram_call_leacy_list.append(symbol_i)
    rom_func_list.sort()
    rom_data_list.sort()
    rom_ram_call_leacy_list.sort()


def classify_undef_symbols():
    with open(argv_list[rom_ram_call_func_file_str], "w+") as rom_ram_call_func_file, open(argv_list[rom_ram_call_data_file_str], "w+") as rom_ram_call_data_file, open(argv_list[rom_ram_call_leacy_file_str], "w+") as rom_ram_call_leacy_file:
        for func_i in rom_func_list:
            # 重定向汇编中引用的外部函数符号需要处理
            print("%s" % func_i, file=rom_ram_call_func_file)
        for data_i in rom_data_list:
            # 重定向汇编中引用的外部数据符号需要处理
            print("%s" % data_i, file=rom_ram_call_data_file)
        for undef_i in undef_rom_symbol_list:
            # 重定向汇编中未使用的外部数据和函数符号认为不影响ROM码点，可以不处理
            if (rom_func_list.count(undef_i) <= 0) and (rom_data_list.count(undef_i) <= 0):
                print("%s" % undef_i, file=rom_ram_call_leacy_file)
        for sym in rom_ram_call_leacy_list:
            print("%s" % sym, file=rom_ram_call_leacy_file)


def undef_symbol_filter_load():
    global undef_filter
    check_file_exist(argv_list[undef_filter_str])
    with open(argv_list[undef_filter_str], "r") as undef_filter:
        line = undef_filter.readline()
        while line:
            line = line.replace('\r', '')
            line = line.replace('\n', '')
            if (len(line) > 0) and (line.find('#', 0, 1) < 0):
                undef_list_filter.append(line)
            line = undef_filter.readline()


def undef_symbol_load():
    check_file_exist(argv_list[undef_file_str])
    with open(argv_list[undef_file_str], "r") as undef_file:
        white_list = ["OsIdleTask"]
        line = undef_file.readline()
        # global  undef_rom_symbol_list
        global undef_list_filter
        global undef_result_symbols_list
        while line:
            line = line.replace('\r', '')
            line = line.replace('\n', '')
            if line.split().__len__() < 2:
                print("error format for %s file, str=%s" % (argv_list[undef_file_str], line))
                sys.exit(1)
            if line.split()[0] != 'U':
                continue
            line = line.split()[1]
            if undef_list_filter.count(line) <= 0:
                # undef_result_symbols_list.append(line)
                undef_rom_symbol_list.append(line)
            else:
                print("filter undef symbol %s" % line)
            line = undef_file.readline()
        undef_rom_symbol_list.extend(white_list)
        undef_rom_symbol_list.sort()

# 宏替换改成链接替换 --wrap=symbol
def rom_gen_arm32_assemfile():
    with open(argv_list[rom_file_str], "w+") as rom_file:
        print("/*", file=rom_file)
        print(" * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2030. All rights reserved.", file=rom_file)
        print(" * Description: Rom Callback Functions by Assemble(arm32), Auto Generate by Python.", file=rom_file)
        print(" * Author: Platform", file=rom_file)
        # print(" * Create: %s" % time.strftime("%Y-%m-%d", time.localtime()), file=rom_file)
        print(" */", file=rom_file)
        print("", file=rom_file)
        print("#ifdef _PRE_FEATURE_VENEER_ROM", file=rom_file)
        # print(".code 32", file=rom_file)
        print(".thumb", file=rom_file)
        print(".syntax unified", file=rom_file)
        # print("#include \"rom_callback.h\"", file=rom_file)
        print("", file=rom_file)
        veneer_rom_cnt = 0
        for list_line in rom_func_list:
            print(".section .%s.romcall.text, \"ax\"" % list_line, file=rom_file)
            print(".globl %s%s" % (romcallback_prefix_str, list_line), file=rom_file)
            print(".type %s%s, %%function" % (romcallback_prefix_str, list_line), file=rom_file)
            print("%s%s:" % (romcallback_prefix_str, list_line), file=rom_file)
            print("    LDR    PC, =%s%s" % (romcallback_real_prefix, list_line), file=rom_file)
            print("", file=rom_file)
            veneer_rom_cnt = veneer_rom_cnt + 1
        print(".end", file=rom_file)
        # print("// %d veneer functions" % veneer_rom_cnt, file=rom_file)
        print("#endif // _PRE_FEATURE_VENEER_ROM", file=rom_file)

# 宏替换改成链接替换 --wrap=symbol
def rom_gen_riscv32_assemfile():
    with open(argv_list[rom_file_str], "w+") as rom_file:
        print("/*", file=rom_file)
        print(" * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2030. All rights reserved.", file=rom_file)
        print(" * Description: Rom Callback Functions by Assemble(riscv32), Auto Generate by Python.", file=rom_file)
        print(" * Author: Platform", file=rom_file)
        # print(" * Create: %s" % time.strftime("%Y-%m-%d", time.localtime()), file=rom_file)
        print(" */", file=rom_file)
        print("", file=rom_file)
        print("#ifdef _PRE_FEATURE_VENEER_ROM", file=rom_file)
        # print(".code 32", file=rom_file)
        # print("#include \"rom_callback.h\"", file=rom_file)
        print("", file=rom_file)
        veneer_rom_cnt = 0
        for list_line in rom_func_list:
            # 这里对齐不顶用，在link script脚本里按照8字节对齐，因为riscv的tail指令在长跳和短跳时链接后的汇编长度会不一样，统一对齐到8字节
            # print(".align 8", file=rom_file)
            print(".section .%s.romcall.text, \"ax\"" % list_line, file=rom_file)
            print(".globl %s%s" % (romcallback_prefix_str, list_line), file=rom_file)
            print(".type %s%s, %%function" % (romcallback_prefix_str, list_line), file=rom_file)
            print("%s%s:" % (romcallback_prefix_str, list_line), file=rom_file)
            print("    tail %s%s" % (romcallback_real_prefix, list_line), file=rom_file)
            print("", file=rom_file)
            veneer_rom_cnt = veneer_rom_cnt + 1
        # print(".end", file=rom_file)
        # print("// %d veneer functions" % veneer_rom_cnt, file=rom_file)
        print("#endif // _PRE_FEATURE_VENEER_ROM", file=rom_file)

# 宏替换改成链接替换！！
def rom_gen_macros_redefine_file():
    with open(argv_list[auto_redefine_macro_file_str], "w+") as auto_redefine_macro_file:
        # auto file header gen
        print("# rom callback wrap function config, auto gen by py!", file=auto_redefine_macro_file)
        print("set(CMAKE_CB_FLGAS ${CMAKE_CB_FLGAS} -Wl", end="", file=auto_redefine_macro_file)
        for list_line in rom_func_list:
            # output redefine macros to rom sections
            print(",--wrap=%s" % list_line, end="", file=auto_redefine_macro_file)
        print(")", file=auto_redefine_macro_file)

def rom_func_source_gen():
    rom_gen_macros_redefine_file()
    target_str = argv_list["target"]
    if target_str == "arm32":
        rom_gen_arm32_assemfile()
    elif target_str == "riscv32":
        rom_gen_riscv32_assemfile()
    else:
        print("unsupport target: %s" % target_str)
        sys.exit(1)


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


def parse_args(target, output):
    for key, value in argv_list.items():
        if key == "target":
            argv_list["target"] = target
        else:
            file_path = os.path.join(output, input_file_list[key])
            # check_file_exist(file_path)
            argv_list[key] = file_path
    dump_all_arguments()


def init_val():
    global undef_rom_symbol_list
    global undef_result_symbols_list
    global rom_func_list
    global rom_data_list
    global undef_list_filter
    global dict_all_symbols_glb
    global rom_ram_call_leacy_list
    undef_rom_symbol_list = []
    undef_result_symbols_list = []
    rom_func_list = []
    rom_data_list = []
    undef_list_filter = []
    dict_all_symbols_glb = dict()
    rom_ram_call_leacy_list = []

# input argvs:
# output argvs
def gen_rom_ram_callback(target, output):
    parse_args(target, output)
    init_val()
    undef_symbol_filter_load()
    undef_symbol_load()
    rom_rel_load()
    undef_symbols_type_process()
    classify_undef_symbols()
    rom_func_source_gen()
    rom_parse_region_file_list()
    rom_function_lds_gen()
