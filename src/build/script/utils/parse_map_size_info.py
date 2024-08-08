#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
import os
import re
import sys
from collections import defaultdict
from pathlib import Path


def raise_string_exit(raise_string):
    # raise Exception(raise_string)
    sys.exit(0)


def get_section_state(lines_split_set, name):
    """
    name: 要判断section状态的 段名
    g_state_lst: 需要判断的字符的列表
    state: 二进制位表示, bit 0 表示load bit 1 表示debug
    none    -> 00
    load    -> 01
    debug   -> 10
    laod + debug    -> 11
    """
    state = 0
    default_debug_lst = ['.comment', '.riscv.attributes']  # 默认debug的列表
    # 在debug默认列表中直接返回  10
    if name in default_debug_lst:
        return state | 1 << 1
    lines_split_set = set(lines_split_set)
    for i, st in enumerate(g_state_lst):
        if st in lines_split_set:
            state |= 1 << i
    return state


def conver_to_utf8(input_file, output_file):
    """
    判断文件编码是否是uft-8, 否则将文件转化utf-8重新保存
    """
    try:
        with open(input_file, 'r', encoding='utf-8') as f:
            f.read()
    except:
        try:
            with open(input_file, 'rb') as infile:
                raw_data = infile.read()
                decode_txt = raw_data.decode(errors='replace')
        except Exception as e:
            print(f"无法解析读取的文件或编码: {e}")
        try:
            with open(output_file, 'w', encoding='utf-8') as outfile:
                outfile.write(decode_txt)
        except Exception as e:
            print(f"无法保存文件: {e}")


def parse_lst_file(lst_file):
    conver_to_utf8(lst_file, lst_file)
    record_flag = 0
    section_name_dict = {}
    record_line = ['Idx', 'Name', 'Size', 'VMA', 'LMA', 'File', 'off', 'Algn']
    with open(lst_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    i = 0
    while i < len(lines):
        # 字符串以 "Sections:" 开始 且 下一行等于 record_line 则开始记录
        if lines[i].startswith("Sections:"):
            if lines[i + 1].split()[:3] == record_line[:3]:
                record_flag = 1
                i += 2
                continue
            else:
                raise_string_exit(f"Not found {record_line}")
        if record_flag:
            name = lines[i].split()[1]
            section_state = get_section_state(lines[i + 1].split(), name)
            section_name_dict[name] = section_state
            i += 2
            # 不是以空格 开头 则Sections 解析完毕 跳出
            if not lines[i].startswith(' '):
                break
        else:
            i += 1
    return section_name_dict


def get_mem_address_len_lst_append_idx(name):
    # ram, flash, rom 关键字特性列表
    ram_feat_lst = [
        'CT_SRAM', 'PRESERVE_SHARE_MEM', 'EM_MUX_RAM', 'ITCM', 'DTCM', 'SRAM', 'APP_ITCM', 'APP_DTCM',
        'CPUTRACE_RAM'
    ]
    flash_feat_lst = ['CT_FLASH', 'PROGRAM_STARTUP', 'PROGRAM', 'FLASH_STARTUP', 'FLASH_PROGRAM']
    rom_feat_lst = ['CT_ROM', 'ROM', 'ACORE_ROM']
    # 返回 get_mem_address_len_lst 中的行索引
    if any(feat in name for feat in ram_feat_lst):
        return 0
    elif any(feat in name for feat in flash_feat_lst):
        return 1
    elif any(feat in name for feat in rom_feat_lst):
        return 2
    else:
        raise_string_exit(f"Unknown not in ['RAM', 'FLASH', 'ROM'] -- {name}")


def get_space_type_state(section_name, start_address, mem_address_range_lst):  #### 函数名 根据地址查找占用空间类型
    """
    start_address: 段名 开始地址
    state:
    none -> 000
    ram   -> 001
    flash   -> 010
    rom -> 100
    """
    state = 0
    for j, lst in enumerate(mem_address_range_lst):
        for start, end, _ in lst:
            if start <= start_address < end:
                state |= 1 << j
                # state & 1 表示是否在ram中,  且 含有 LOAD 则添加 flash状态
                if (state & 1) and (lst_info_dict[section_name] >> g_state_lst.index('LOAD,')) & 1:
                    state |= 1 << 1
    return state


def state2space_type(state):
    """
    占用空间类型状态码 转 字符描述
    """
    space_type = []
    space_type_lst = ['RAM', 'FLASH', 'ROM']  # 占用空间类型列表
    for i, x in enumerate(space_type_lst):
        if state >> i & 1:
            space_type.append(x)
    return ' + '.join(space_type)


def parse_section_info(lines, i, debug_flag, mem_address_range_lst):
    # 解析段名行的信息
    line_info_lst = lines[i].split()
    if len(line_info_lst) == 1:
        # 段名下为空的跳出 不解析
        if not lines[i + 1].strip():
            return "continue", debug_flag
        # 存在段名太长 信息换行的情况
        line_info_lst += lines[i + 1].split()
        # 段名下不包含长度信息 不解析
        if len(line_info_lst) < 3:
            return "continue", debug_flag
    # 段名
    section_name = line_info_lst[0]
    if section_name in lst_info_dict:
        # 如果段名是debugging 跳过不处理
        if (lst_info_dict[section_name] >> g_state_lst.index('DEBUGGING,')) & 1:
            debug_flag = 1
            return "continue", debug_flag
        else:
            debug_flag = 0
    else:
        # 不在 lst_info_dict 里 且 段名长度为 0, 不处理
        if int(line_info_lst[2], 16) == 0:
            debug_flag = 1
            return "continue", debug_flag
        else:
            raise_string_exit(f"Not found {section_name} in lst_info_dict -- {i}")
    # 起始地址
    start_address = int(line_info_lst[1], 16)
    # 段长度
    section_len = int(line_info_lst[2], 16)
    state = get_space_type_state(section_name, start_address, mem_address_range_lst)
    if state & 1 and (state >> 1) & 1:
        # 用于判断 ram+flash情况下 flash地址与ram地址的差值
        flash_start_address = int(line_info_lst[-1], 16)
        diff_address = flash_start_address - start_address
    else:
        diff_address = 0
    # 键名
    map_info_key = (section_name, start_address, section_len, state)
    return map_info_key, diff_address, debug_flag


def parse_symb_info(lines, i, map_info_key, diff_address, symb_info):
    """
    解析符号信息
    """
    pattern1 = r'\((.*?)\)'  # 寻找库名(o_name)正则匹配
    pattern2 = r'(\w+\.a)'  # 寻找文件名(a_name)正则匹配
    o_name_lst = ('S.obj)', 's.obj)', '.o)', 'c.obj)')  # 库名的后缀匹配
    special_fill_threshold = 24  # fill地址内存的阈值，小于等于24字节的字段被认为是自动对齐的填充字段，不需要解释
    symbol_type_lst = ('.text', '.rodata', '.data', '.bss')  # 符号类型的元组，不在里面则为 .other
    line_info_lst = lines[i].split()
    if len(line_info_lst) == 1:
        if lines[i + 1].strip().endswith(o_name_lst):
            line_info_lst += lines[i + 1].split()
    if line_info_lst[0] == '*fill*':
        symb_name = line_info_lst[0]
        symb_start_address = int(line_info_lst[1], 16)
        symb_len = int(line_info_lst[2], 16)
        symb_end_address = symb_start_address + symb_len
        # fill 大小大于 special_fill_threshold 赋值段名
        if symb_len > special_fill_threshold:
            a_name = o_name = map_info_key[0]
        else:
            a_name, o_name = symb_info[-2], symb_info[-1]
        symb_info = [symb_start_address, symb_end_address, symb_len, symb_name, '.fill', a_name, o_name]
    elif not line_info_lst[-1].endswith(o_name_lst):
        raise_string_exit(f"Not found {o_name_lst} -- {i}")
    else:
        symb_name = line_info_lst[0]
        symb_start_address = int(line_info_lst[1], 16)
        symb_len = int(line_info_lst[2], 16)
        symb_end_address = symb_start_address + symb_len
        symb_type = '.other'
        for st in symbol_type_lst:
            if symb_name.startswith(st) or symb_name.endswith(st):
                symb_type = st
                break
        o_name = re.findall(pattern1, line_info_lst[-1])[-1]
        a_name = re.findall(pattern2, line_info_lst[-1])[0]
        symb_info = [symb_start_address, symb_end_address, symb_len, symb_name, symb_type, a_name, o_name]
    # 根据 state 把 symb_info 加入ram_dict, g_flash_dict, g_rom_dict
    if map_info_key[-1] & 1:
        if symb_info[2] != 0:
            g_ram_dict[map_info_key].append(symb_info)
    if (map_info_key[-1] >> 1) & 1:
        # 加入flash_dict 的起始地址需要加 diff_address
        # ram+flash情况下 diff_address!=0
        # 单flash 情况下 diff_address=0
        map_info_key_lst = list(map_info_key)
        map_info_key_lst[1] += diff_address
        tmp_key = tuple(map_info_key_lst)
        tmp_symb_info = symb_info.copy()
        tmp_symb_info[0] += diff_address
        tmp_symb_info[1] += diff_address
        if tmp_symb_info[2] != 0:
            g_flash_dict[tmp_key].append(tmp_symb_info)
    if (map_info_key[-1] >> 2) & 1:
        if symb_info[2] != 0:
            g_rom_dict[map_info_key].append(symb_info)
    return symb_info


def sort_data_dict(data_dict):
    """
    data_dict 按键值的 起始地址大小 从小到大排序
    """
    data_dict = dict(sorted(data_dict.items(), key=lambda x: x[0][1]))
    return data_dict


def check_is_overlap(data_dict):
    """
    检查同一个段名下的 地址是否重叠  首地址 a 尾地址 b
    a-b<0 -> 调整
    a=b -> 跳过
    a-b>0 -> 报错
    """
    for k, lst in data_dict.items():
        for i in range(1, len(lst)):
            a, b = lst[i][0], lst[i - 1][1]
            if a - b < 0:
                lst[i][0] = b
                lst[i][2] = lst[i][1] - b
                lst[i].append('调整!')
            elif a - b > 0:
                raise_string_exit("There is a gap between the start address and the end address.")
    return data_dict


def get_tab_len_lst(data_dict):
    """
    获得 段与段之间的 段长度差值
    """
    tab_len_lst = []
    for i, k in enumerate(data_dict.keys()):
        if i == 0:
            tab_len = [k[0], k[1], k[2], 0]
        else:
            tab_len = [k[0], k[1], k[2], k[1] - end_address]
        end_address = k[1] + k[2]
        tab_len_lst.append(tab_len)
    return tab_len_lst


def save_dict_csv(data_dict, file_name):
    """
    data_dict: 要保存的数据, 保存为csv
    file_name: 保存csv的文件名  file_name.csv
    """
    # data_dict 按键值的 起始地址大小 从小到大排序
    data_dict = sort_data_dict(data_dict)
    # 检查同一个段名下的 地址是否重叠
    data_dict = check_is_overlap(data_dict)
    with open(OUTPUT_DIR / f'{file_name}.csv', 'w', encoding='utf_8_sig') as f:
        # data_dict的表头名
        header = ['起始地址', '结束地址', '符号大小', '符号名', '符号类型', '库名', '文件名', '段名', '段起始地址', '段大小', '占用空间类型', '']
        f.write(','.join(header) + '\n')
        for i, (k, info_lst) in enumerate(data_dict.items()):
            k = list(k)
            k[-1] = state2space_type(k[-1])
            k = ','.join(map(str, k))
            for info in info_lst:
                if info[-1] == '调整!':
                    f.write(f"{','.join(map(str, info[:-1]))},{k},调整!\n")
                else:
                    f.write(f"{','.join(map(str, info))},{k},\n")
    # 获得 段与段之间的 段长度差值
    tab_len_lst = get_tab_len_lst(data_dict)
    with open(OUTPUT_DIR / f'{file_name}_tab_len.csv', 'w', encoding='utf_8_sig') as f:
        header = ['段名', '段起始地址', '段大小', 'Tab_Len']
        f.write(','.join(header) + '\n')
        for lst in tab_len_lst:
            f.write(','.join(map(str, lst)) + '\n')


def parse_map_file(map_file):
    with open(map_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    # [symb_start_address, symb_end_address, symb_len, symb_name, symb_type, a_name, o_name]
    symb_info = []
    # 初始化 flag
    mem_record_flag = link_flag = debug_flag = link_if_flag = 0
    ## mem_if_flag 是否 判断 字符串以Memory Configuration开头
    mem_if_flag = 1
    mem_conf_flag_lst = ['Name', 'Origin', 'Length', 'Attributes']
    # 记录地址范围的列表，用于判断是否 ram rom flash, 下标0: ram, 1: flash, 2: rom
    mem_address_range_lst = [[] for _ in range(3)]
    i = 0
    while i < len(lines):
        if not lines[i].strip():
            i += 1
            continue
        # 开始解析 Memory Configuration
        if mem_if_flag and lines[i].startswith("Memory Configuration"):
            if lines[i + 2].split() == mem_conf_flag_lst:
                mem_record_flag = 1
                mem_if_flag = 0
                i += 3
                continue
            else:
                raise_string_exit(f"Not found {mem_conf_flag_lst} -- {i}")
        if mem_record_flag:
            # 不记录 *default*
            if lines[i].startswith('*default*'):
                link_if_flag = 1
                mem_record_flag = 0
                i += 2
                continue
            elif lines[i].startswith('Linker'):
                raise_string_exit("Not found *default*")
            mem_lst = lines[i].split()
            # mem_info = (起始地址, 结束地址, 地址长度)
            mem_info = (int(mem_lst[1], 16), int(mem_lst[1], 16) + int(mem_lst[2], 16), int(mem_lst[2], 16))
            idx = get_mem_address_len_lst_append_idx(mem_lst[0])
            mem_address_range_lst[idx].append(mem_info)
        # 开始解析 Linker script and memory map 下的信息
        if link_if_flag:
            if lines[i].startswith("Linker script and memory map"):
                link_flag = 1
                link_if_flag = 0
                i += 1
                continue
            else:
                raise_string_exit("Not found Linker script and memory map")
        if link_flag:
            # 解析段名行的信息
            if any(lines[i].startswith(key) for key in lst_info_dict.keys()) or (lines[i].startswith('.') and
                                                                                 lines[i][1] != '.'):
                tmp_info = parse_section_info(lines, i, debug_flag, mem_address_range_lst)
                if tmp_info[0] == "continue":
                    debug_flag = tmp_info[1]
                    i += 1
                    continue
                else:
                    map_info_key, diff_address, debug_flag = tmp_info
            # 解析段名下的符号信息
            elif debug_flag == 0 and lines[i].startswith((' .', ' *fill')):
                # 解析符号信息
                symb_info = parse_symb_info(lines, i, map_info_key, diff_address, symb_info)
        i += 1
    save_dict_csv(g_ram_dict, f"{map_file.name}_ram_dict")
    save_dict_csv(g_flash_dict, f"{map_file.name}_flash_dict")
    save_dict_csv(g_rom_dict, f"{map_file.name}_rom_dict")


def get_group_owner(data_dict, save_name):
    a_name2resp_group_dict = dict()  # {a_name: PLAT  WIFI  BGLE}
    with open(OWNER_DIR / 'library_owner.csv', 'r', encoding='utf-8') as f:
        header = f.readline()
        lines = f.readlines()
    for line in lines:
        a_name, group = line.strip().split(',')
        a_name2resp_group_dict[a_name] = group

    c_name2wifi_group_dict = dict()  # {c_name: 应用组  算法组  协议组 前端组 系统组}
    with open(OWNER_DIR / 'wifi_owner.csv', 'r', encoding='utf-8') as f:
        header = f.readline()
        lines = f.readlines()
    for line in lines:
        c_name, group = line.strip().split(',')
        c_name2wifi_group_dict[c_name] = group
    # a_name : [sum_symb_len, group_type]   ---group_type: PLAT  WIFI  BGLE UNKNOWN
    file_stat_dict = defaultdict(lambda: [0, 'UNKNOWN'])
    # {c_name : {a_name : [sum_symb_len, group_type, wifi_group_type]}}
    wifi_stat_dict = defaultdict(lambda: defaultdict(lambda: [0, 'UNKNOWN', '']))
    for symb_info_list in data_dict.values():
        for symb_info in symb_info_list:
            symb_len, a_name, c_name = symb_info[2], symb_info[5], symb_info[6]
            # 去除后缀 .obj 但 .heap .stack .shere_mem 跳过
            if not c_name.startswith('.'):
                c_name = c_name.split('.')[0] + '.c'  #
            if a_name in a_name2resp_group_dict:
                file_stat_dict[a_name][1] = a_name2resp_group_dict[a_name]
                wifi_stat_dict[c_name][a_name][1] = a_name2resp_group_dict[a_name]
                if a_name2resp_group_dict[a_name] == 'WIFI':
                    if c_name in c_name2wifi_group_dict:
                        wifi_stat_dict[c_name][a_name][2] = c_name2wifi_group_dict[c_name]
                    else:
                        wifi_stat_dict[c_name][a_name][2] = 'WIFI_UNKNOWN'
            file_stat_dict[a_name][0] += symb_len
            wifi_stat_dict[c_name][a_name][0] += symb_len
    # PLAT  WIFI  BGLE 按 库名 角度统计 大小
    with open(OUTPUT_DIR / f"{save_name}_library.csv", 'w', encoding='utf_8_sig') as f:
        header = ['库名', '大小', '负责组']
        f.write(f"{','.join(header)}\n")
        group_dict = defaultdict(int)  # 用于下面的 负责组的角度统计 地址大小
        for a_name, info in file_stat_dict.items():
            f.write(f"{a_name},{info[0]},{info[1]}\n")
            group_dict[info[1]] += info[0]

    # 从负责组的角度统计 地址大小
    with open(OUTPUT_DIR / f"{save_name}_library_summary.csv", 'w', encoding='utf_8_sig') as f:
        header = ['Group_Owner', 'Size', 'Limit']
        f.write(f"{','.join(header)}\n")
       # 各组规格限制 PLAT WIFI BTC BTH UNKNOWN  STACK PKTRAM
        lim_all = {'ws63-liteos-app.map_ram': {'PLAT' : 35.50, 'WIFI':46.5, 'RADAR':49, 'BTC': 20, 'BTH': 10, 'UNKNOWN': 0.5, 'STACK':7,'PKTRAM':50},
            'control_ws53.map_ram':{'PLAT' : 16.68, 'WIFI':18.64, 'BTC': 25, 'BTH': 10, 'UNKNOWN': 25, 'STACK':6,'HEAP':27},
            'control_ws53.map_flash':{'PLAT' : 15, 'WIFI':46.5, 'BTC': 109, 'BTH': 6, 'UNKNOWN': 35},
            'liteos_ws53_light.map_flash':{'PLAT' : 191, 'WIFI':670, 'BTC': 1, 'BTH': 242.5, 'UNKNOWN': 1},
            'ws63-liteos-app.map_flash':{'PLAT' : 197, 'WIFI':622.1, 'RADAR':37.5, 'BTC': 143.5, 'BTH': 228, 'UNKNOWN': 0.5}}
        lim_i = 0
        for owner, size in group_dict.items():
            size_t = size/BIT_SIZE
            if save_name in lim_all:
                lim = lim_all[save_name]
                f.write(f"{owner},{size_t},{lim[owner]}\n")
                if size_t > lim[owner]:
                    print("Error " + owner + " Exceeded " + save_name + " memory size limit!! Current size is %.3f limit is %.3f" %(size_t, lim[owner]))
                    sys.exit(1)
                lim_i += 1
            else:
                f.write(f"{owner},{size_t},0\n")

    # 应用组  算法组  协议组 前端组 系统组 按 文件名 库名 角度统计 地址大小
    with open(OUTPUT_DIR / f"{save_name}_file.csv", 'w', encoding='utf_8_sig') as f:
        header = ['文件名', '库名', '文件大小', '负责组', 'WIFI_责任组']
        f.write(f"{','.join(header)}\n")
        wifi_group_dict = defaultdict(int)  # 用于下面的 WIFI负责组的角度统计 地址大小
        for c_name, info_dict in wifi_stat_dict.items():
            for a_name, info in info_dict.items():
                f.write(f"{c_name},{a_name},{info[0]},{info[1]},{info[2]}\n")
                if info[1] == 'WIFI':
                    wifi_group_dict[info[2]] += info[0]

    # 从WIFI负责组的角度统计 地址大小
    with open(OUTPUT_DIR / f"{save_name}_file_summary.csv", 'w', encoding='utf_8_sig') as f:
        header = ['Group_Owner', 'Size', 'Limit']
        f.write(f"{','.join(header)}\n")
        lim_all = {'control_ws53.map_flash':{'系统组' : 8.0, '协议组':9.5, '应用组': 2, '前端组': 27.1, '算法组': 0.5,'WIFI_UNKNOWN': 5},
            'liteos_ws53_light.map_flash':{'系统组' : 36, '协议组':212.5, '应用组': 356, '前端组': 15, '算法组': 39,'WIFI_UNKNOWN': 0.5},
            'ws63-liteos-app.map_flash':{'系统组' : 35.5, '协议组':196.1, '应用组': 314.5, '前端组': 37.1, '算法组': 39.3,'WIFI_UNKNOWN': 0.5}}
        lim_i = 0
        for owner, size in wifi_group_dict.items():
            size_t = size/BIT_SIZE
            if save_name in lim_all:
                lim = lim_all[save_name]
                f.write(f"{owner},{size_t},{lim[owner]}\n")
                if size_t > lim[owner]:
                    print("Error " + owner + " Exceeded " + save_name + " memory size limit!! Current size is %.3f limit is %.3f" %(size_t, lim[owner]))
                    sys.exit(1)
                lim_i += 1
            else:
                f.write(f"{owner},{size_t},0\n")


def usage():
    print("usage:")
    print("    python3 script target_lst_file target_map_file")
    print("")


if __name__ == "__main__":
    if len(sys.argv[1:]) != 2:
        usage()
        sys.exit(0)
    root_path = os.path.join(os.path.split(os.path.realpath(__file__))[0], '..', '..', '..')
    root_path = os.path.abspath(root_path)
    BIT_SIZE = 1024
    OWNER_DIR = Path(root_path) / 'protocol/wifi/build/smaller'
    if not OWNER_DIR.exists():
        sys.exit(0)
    DATA_DIR = Path('./')
    OUTPUT_DIR = Path('./smaller')  # 输出的文件夹下 存放表格数据
    if not OUTPUT_DIR.exists():
        OUTPUT_DIR.mkdir()
    g_state_lst = ['LOAD,', 'DEBUGGING,']  # 状态列表
    g_ram_dict, g_flash_dict, g_rom_dict = defaultdict(list), defaultdict(list), defaultdict(list)
    try:
        lst_info_dict = parse_lst_file(DATA_DIR / sys.argv[1])
        parse_map_file(DATA_DIR / sys.argv[2])
    except:
        sys.exit(0)
    get_group_owner(g_ram_dict, f"{sys.argv[2]}_ram")
    get_group_owner(g_flash_dict, f"{sys.argv[2]}_flash")
    get_group_owner(g_rom_dict, f"{sys.argv[2]}_rom")
