#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    riscv ROM Patch File
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
# ============================================================================
import struct
import ctypes
import sys
import os
import shutil
import traceback

dir_name = os.path.dirname(os.path.realpath(__file__))
info_item = [
    "Device_Code_Version",
    "Patch_Cpu_Core",
    "Patch_File_Address",
    "Patch_TBL_Address",
    "Patch_TBL_Run_Address",
    "Table_Max_Size",
    "Table_Reg_Size",
    "ROM_Address",
    "ROM_Size",
    "CMP_Bin_File",
    "TBL_Bin_File",
    "RW_Bin_File",
    "TABLE_REG_CONUT",
    ]

# The default value is 4.
# The value will be set based on long jump or short jump for linx131.
CMP_HEAD_LEN = 3
g_cmp_total_len = 131 # 128个比较表 + 3个头部信息
CMP_REG_SIZE = 4
PATCH_COUNT_REG_INDEX = 2
DATA_PATCH_COUNT = 2
pid = str(os.getpid())


# 目录转换
def get_dir(dir_in):
    return os.path.join(dir_name, dir_in)

def remove_bin_file():
    os.remove(get_dir(pid + "cmp.bin")) if os.path.exists(get_dir(pid + "cmp.bin")) else None
    os.remove(get_dir(pid + "tbl.bin")) if os.path.exists(get_dir(pid + "tbl.bin")) else None
    os.remove(get_dir(pid + "rw.bin")) if os.path.exists(get_dir(pid + "rw.bin")) else None

# 转换成bin文件
def copy_bin_file(str_dst, str_src):
    try:
        with open(str_src, "rb")as file_src:
            try:
                with open(str_dst, "wb+")as file_dst:
                    byte = file_src.read(1)
                    while byte:
                        file_dst.write(byte)
                        byte = file_src.read(1)
                    file_dst.close()
            except Exception as e:
                print("Error: %s Can't Open!" % file_dst)
                remove_bin_file()
                sys.exit(1)
            file_src.close()
    except Exception as e:
        print("Error: %s Can't Open!" % str_src)
        remove_bin_file()
        sys.exit(1)


# 生成bin文件
def merge_output_file(files):
    try:
        reg_size = int(files['Table_Reg_Size'])
        max_size = int(files['Table_Max_Size'])
        with open(get_dir(files['RW_Bin_File']), "rb+") as file_rw:
            try:
                with open(get_dir(files['TBL_Bin_File']), "rb+")as file_table:
                    data_num = int(files['TABLE_REG_CONUT']) * reg_size
                    data_table = []
                    for num in range(data_num):
                        data_table.append(0)
                    buff = file_table.read(1)
                    j = 0
                    while buff:
                        data_table[j] = struct.unpack('<B', buff)[0]
                        buff = file_table.read(1)
                        j += 1
                    file_table.close()
                    offset_addr = int(files['Patch_TBL_Address'], 16) - int(files['Patch_File_Address'], 16) +\
                                      DATA_PATCH_COUNT * reg_size
                    file_rw.seek(offset_addr, 0)
                    i = 0
                    while i < len(data_table):
                        byte1 = struct.pack('B', data_table[i])
                        file_rw.write(byte1)
                        i += 1
            except Exception as e:
                print("Error: %s Can't Open!" % file_table)
                remove_bin_file()
                sys.exit(1)
            try:
                with open(get_dir(files['CMP_Bin_File']), "rb+")as file_cmp:
                    data_num = g_cmp_total_len * CMP_REG_SIZE
                    data_cmp = []
                    for num1 in range(data_num):
                        data_cmp.append(0)
                    l = 0
                    buff1 = file_cmp.read(1)
                    while buff1:
                        data_cmp[l] = struct.unpack('<B', buff1)[0]
                        buff1 = file_cmp.read(1)
                        l += 1
                    file_cmp.close()
                    offset_addr = int(files['Patch_TBL_Address'], 16) - int(files['Patch_File_Address'], 16) +\
                                      int(files['TABLE_REG_CONUT']) * max_size
                    file_rw.seek(offset_addr, 0)
                    i = 0
                    while i < len(data_cmp):
                        byte2 = struct.pack('B', data_cmp[i])
                        file_rw.write(byte2)
                        i += 1
                    file_rw.close()
            except Exception as e:
                print("Error: %s Can't Open!" % file_cmp)
                remove_bin_file()
                sys.exit(1)
    except Exception as e:
        print("Error: %s Can't Open!" % file_rw)
        remove_bin_file()
        sys.exit(1)


# Table or Cmp
def creat_bin_file(file_name, contents, type_in):
    cmp_len = len(contents)
    try:
        with open(file_name, "wb")as file_o:
            for i in range(cmp_len):
                byte = struct.pack('I', contents[i])
                file_o.write(byte)
        file_o.close()
    except Exception as e:
        print("Error: %s Can't Open!" % file_name)
        remove_bin_file()
        sys.exit(1)


# 获取函数对应的二进制文件
def get_func_code_in_file(files, func_addr, bt_rom_file_in):
    rom_pack = ['', 0xffffffff]
    temp_rom_begin = int(files['ROM_Address'], 16)
    temp_rom_size = int(files['ROM_Size'], 16)
    if (temp_rom_begin <= func_addr) and (
            func_addr < (temp_rom_begin + temp_rom_size)):
        rom_pack[0] = temp_rom_begin
        rom_pack[1] = bt_rom_file_in
    return rom_pack


# 获取函数对应的二进制
def get_func_code_in_bin(rom_addr, file_name, func_addr, align):
    bin_content = [0, 0, 0, 0]
    func_addr_offset = func_addr - rom_addr - align
    try:
        with open(file_name, "rb") as file_o:
            file_o.seek(func_addr_offset, 0)
            for i in range(4):
                readonce = file_o.read(4)
                bin_content[i] = struct.unpack('<I', readonce)[0]
            file_o.close()
    except Exception as e:
        print("Error: %s Can't Open!" % file_name)
        remove_bin_file()
        sys.exit(1)
    return bin_content


# 获取patch相关信息
def get_patch_info(patchinfo):
    all_name = {}
    try:
        with open(str(patchinfo), "r", encoding="utf-8") as file_o:
            source_in_lines = file_o.readlines()
            for line in source_in_lines:
                if '[Function]' in line:
                    break
                if '=' in line:
                    config_key= line.split('=')[0].strip()
                    config_value= line.split('=')[1].strip()
                    if config_key in info_item:
                        all_name[config_key] = config_value
        # 在bin文件名上加上进程id前缀，防止多进程时出错
        all_name["CMP_Bin_File"] = pid + all_name["CMP_Bin_File"]
        all_name["TBL_Bin_File"] = pid + all_name["TBL_Bin_File"]
        all_name["RW_Bin_File"]  = pid + all_name["RW_Bin_File"]
        return all_name
    except Exception:
        print("Error: %s Can't Open!" % patchinfo)
        remove_bin_file()
        sys.exit(1)

# 获取patch函数名
def get_func_name(func_file_name, index):
    func_names = []
    try:
        with open(func_file_name, "r", encoding='utf-8') as file_o:
            is_func = False
            for line in file_o:
                line = line.strip()   # 去掉每行头尾空白
                if '[Function]' in line:
                    is_func = True
                    continue
                if not len(line) or line.startswith('#') or not is_func:
                    continue
                temp = line.split()
                if len(temp) != 2:
                    print(
                        "Error format file_o:%s,line:%s," %
                        (func_file_name, temp))
                func_names.append(temp[index])
    except UnicodeDecodeError as e:
        print("get_func_name catch UnicodeDecodeError: %s" % func_file_name)
        remove_bin_file()
        sys.exit(1)
    except Exception as e:
        print(traceback.format_exc())
        print("Error: %s Can't Open!" % func_file_name)
        remove_bin_file()
        sys.exit(1)
    return func_names


# gcc编译器nm文件中所有行有效
def get_nm_content(file_name):
    nm_ontent = []
    try:
        with open(file_name, "r", encoding='utf-8') as file_o:
            nm_lines = file_o.readlines()
            for nm_line in nm_lines:
                if nm_line.find(" T ") != -1 or nm_line.find(" t ") != -1 or nm_line.find(" A "):
                    nm_line = nm_line.strip('\n')
                    nm_ontent.append(nm_line)
    except Exception as e:
        print("Error: %s Can't Open!" % file_name)
        remove_bin_file()
        sys.exit(1)
    return nm_ontent

def get_func_addr(func_names, m_contents, compiler_name):
    index = 0
    func_addrs = []
    while index < len(func_names):
        index_temp = 0
        for m_content in m_contents:
            temp = m_content.split('|')
            if compiler_name == "GCC" and func_names[index] == temp[0].strip():
                func_addrs.append(temp[1].strip())
                if int(temp[1], 16) < 4:
                    print(
                        "Error: %s Length is %s , it can not be less than 4 "
                        "bytes!" % (temp[3], temp[1]))
                    sys.exit(1)
                elif int(temp[1], 16) == 4:
                    print(
                        "Warning: %s Length is %s bytes, it may be unsafe!" %
                        (temp[3], temp[1]))
                break
            index_temp += 1
            if index_temp == len(m_contents):
                print(
                    "Error: %s Function Can't Find in Map File !" %
                    func_names[index])
                sys.exit(1)
        index += 1
    return func_addrs


# Ctrl寄存器 +  Remap寄存器 + CMP数量 + CMP表
def get_cmp_content(func_addrs, patch_tbl_addr, version):
    cmp_content = []
    if version == "Version1":
        cmp_content.append(0)
        cmp_content.append(int(patch_tbl_addr, 16))
        cmp_content.append(0)
    index = 0

    while index < len(func_addrs):
        func_addr = int(func_addrs[index], 16)
        func_addr = func_addr & (~0x01)  # 地址最后一位为指令标记位,清除
        if version == "Version1":
            func_addr |= 0x1
            cmp_content.append(func_addr)
            index += 1
    cmp_count = len(cmp_content) - CMP_HEAD_LEN
    if version == "Version1" or version == "Version2":
        if version == "Version1":
            cmp_content[PATCH_COUNT_REG_INDEX] = cmp_count
        if len(cmp_content) > g_cmp_total_len:  # 128个比较表 + 头部信息
            print("Error: CMP Packet is larger than CMP Reg Capacitance")
        while cmp_count < g_cmp_total_len - 3:
            cmp_content.append(0)
            cmp_count += 1

    return cmp_content


def get_table_content_for_short_jump(files, func_addrs, func_patch_addrs, version, bt_rom_file_in):
    table_content = []
    bit0_to6 = 0x6F
    bit7_to11 = 0x5 << 7

    func_num = len(func_addrs)
    index = 0
    while index < func_num:
        func_addr = int(func_addrs[index], 16)
        func_patch_addr = int(func_patch_addrs[index], 16)

        off_addr = func_patch_addr - func_addr

        off_bit1_to10 = (off_addr & 0x7fe) >> 1
        off_bit12_to19 = (off_addr & 0xff000) >> 12
        off_bit11 = (off_addr & 0x800) >> 11
        off_bit20 = (off_addr & 0x100000) >> 20

        bit_code = bit0_to6 + bit7_to11 + (off_bit12_to19 << 12) + (off_bit11 << 20) + (off_bit1_to10 << 21) + (off_bit20 << 31)
        table_content.append(bit_code)
        index += 1

    table_count = len(table_content)
    if table_count > int(files['TABLE_REG_CONUT']):  # 128个比较表
        print("Error: TABLE Packet is larger than CMP Reg Capacitance")
        sys.exit(1)
    while table_count < int(files['TABLE_REG_CONUT']):
        table_content.append(0)
        table_count += 1

    return table_content

def get_table_content_for_long_jump(files, func_addrs, func_patch_addrs, version, bt_rom_file_in):
    table_content = []
    auipc_opt_bits = 0x17
    jalr_opt_bits = 0x67
    base_addr_bits = 0x6 # x6
    jalr_bit12_to14 = 0x0 << 12
    jalr_bit7_to11 = 0x0 << 7

    func_num = len(func_addrs)
    index = 0
    while index < func_num:
        func_addr = int(func_addrs[index], 16)
        func_patch_addr = int(func_patch_addrs[index], 16)

        off_addr = func_patch_addr - func_addr
        off_bit12_to31 = off_addr & 0xfffff000
        off_bit0_to11 = off_addr & 0xfff
        if off_bit0_to11 > 0x7FF:
            off_bit12_to31 = off_bit12_to31 + 0x1000
            off_bit0_to11 = 0x1000 - off_bit0_to11
            off_bit0_to11 = (~off_bit0_to11 + 1) & 0xfff

        auipc_bit_code = auipc_opt_bits + (base_addr_bits << 7) + off_bit12_to31
        table_content.append(auipc_bit_code)

        jalr_bit_code = jalr_opt_bits + jalr_bit7_to11 + jalr_bit12_to14 + (base_addr_bits << 15) + (off_bit0_to11 << 20)
        table_content.append(jalr_bit_code)

        index += 1

    table_count = len(table_content)
    if table_count > 2 * int(files['TABLE_REG_CONUT']):  # 128个比较表
        print("Error: TABLE Packet is larger than CMP Reg Capacitance")
        sys.exit(1)
    while table_count < 2 * int(files['TABLE_REG_CONUT']):
        table_content.append(0)
        table_content.append(0)
        table_count += 2

    return table_content

def create_patch(patch_info, nm_file_in, rom_bin_file):
    global g_cmp_total_len
    file_all = get_patch_info(patch_info)
    g_cmp_total_len = int(file_all['TABLE_REG_CONUT']) + 3
    core = file_all['Patch_Cpu_Core']
    funs = get_func_name(patch_info, 0)
    funs_patch = get_func_name(patch_info, 1)
    nm_contents = get_nm_content(nm_file_in)
    func_addrs = get_func_addr(funs, nm_contents, "GCC")
    func_patch_addrs = get_func_addr(funs_patch, nm_contents, "GCC")
    cmp_contents = get_cmp_content(func_addrs, file_all['Patch_TBL_Run_Address'], file_all['Device_Code_Version'])
    reg_size = int(file_all['Table_Reg_Size'])
    if reg_size == 4:
        table_contents = get_table_content_for_short_jump(file_all, func_addrs, func_patch_addrs, file_all['Device_Code_Version'], rom_bin_file)
    elif reg_size == 8:
        table_contents = get_table_content_for_long_jump(file_all, func_addrs, func_patch_addrs, file_all['Device_Code_Version'], rom_bin_file)
    else:
        print("ErrorCore %s for rom patch" % core)
    creat_bin_file(get_dir(file_all['CMP_Bin_File']), cmp_contents, file_all['CMP_Bin_File'])
    creat_bin_file(get_dir(file_all['TBL_Bin_File']), table_contents, file_all['TBL_Bin_File'])

def output_bin_file(file_all, output_dir_in, ram_bin_file):
    if os.path.exists(os.path.join(output_dir_in, ram_bin_file)):
        shutil.move(os.path.join(output_dir_in, ram_bin_file), os.path.join(output_dir_in, "unpatch.bin"))
    if os.path.exists(os.path.join(output_dir_in, "rw.bin")):
        os.remove(os.path.join(output_dir_in, "rw.bin"))
    shutil.move(get_dir(file_all['RW_Bin_File']), os.path.join(output_dir_in, ram_bin_file))
    os.remove(get_dir(file_all['CMP_Bin_File']))
    os.remove(get_dir(file_all['TBL_Bin_File']))
    print("Generating %s..." % ram_bin_file)


def get_patch_addr(patch_info, ram_bin_file, output_dir_in):
    file_all = get_patch_info(patch_info)
    funs = get_func_name(patch_info, 0)
    funs_patch = get_func_name(patch_info, 1)
    copy_bin_file(get_dir(file_all['RW_Bin_File']), ram_bin_file)
    merge_output_file(file_all)
    output_bin_file(file_all, output_dir_in, ram_bin_file)

if __name__ == "__main__":
    if(len(sys.argv) == 8):
        ram_bin_file = sys.argv[1]
        rom_bin_file = sys.argv[2]
        nm_file = sys.argv[3]
        partch_config_dir = sys.argv[4]
        core = sys.argv[5]
        target_name = sys.argv[6]
        output_dir = sys.argv[7]

        if os.path.exists(os.path.join(partch_config_dir, f'{target_name}.cfg')):
            patch_info = os.path.join(partch_config_dir, f'{target_name}.cfg')
        else:
            patch_info = os.path.join(partch_config_dir, f'{core}.cfg')

        create_patch(patch_info, nm_file, rom_bin_file)
        get_patch_addr(patch_info, ram_bin_file, output_dir)

    else:
        print(
            "Usage: %s <xx.bin> <xx_rom.bin> <xx.nm> <patch_confi_dir> <core> <target_name>"
            "<output_dir>" % os.path.basename(sys.argv[0]))
        sys.exit(1)
