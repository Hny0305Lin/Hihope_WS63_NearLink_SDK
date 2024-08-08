#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.

import os
import re
import sys
import platform
import string
import shutil
import time
import json
import xml.etree.ElementTree as ET


py_version = platform.python_version()

def timestamp():
    return time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(time.time()))

##########################################################################
#
# 日志内容提取函数
#
##########################################################################
# 日志状态机处理流程：
# LOG_ENTRY_FINDING              -- 查找日志接口
# LOG_ENTRY_FOUNDED              -- 日志接口匹配，查找日志内容
# LOG_CONT_ENTRY_FINDING         -- 日志内容入口匹配，以“号开关，以”号结束
# LOG_CONT_END_FOUNING           -- 日志内容查找
# LOG_EXIT_FINDING               -- 日志线束符匹配

# 全局变量
aich_name = ""
log_info_list = []   # 日志信息列表，【文件号， 行号， 日志内容】
g_undefine_id = []  # 保存未定义的FileId
g_fsm_status = "LOG_ENTRY_FINDING"
g_log_key_find = 0  # （状态位）判断是否查找到日志关键字，0否1是
g_log_str = ""
g_log_str_full = ""
g_log_entry = [
    r"LOG_PRINT[0-4](\s*)\(",
    r"log_print[0-4](\s*)\(",
    r"acore_log[0-4](\s*)\(",
    r"[printlog]{3,5}_alter(\s*)\(",
    r"[PRINTLOG]{3,5}_ALTER(\s*)\(",
    r"_LOG[0-4]{1,2}(\s*)\(",
    r"(DBG|INFO|ERR|WARN)_LOG(\s*)\(",
    # BTH
    r"(dbg|info|err|warn)_log(\s*)\(",
    r"(dbg|info|err|warn)_log[0-4](\s*)\(",
    # diag common
    r"diag_(error|warning|info|debug)_log(\d*)(\s*)\(",
    r"PRINT_(DEBUG|INFO|ERR|WARN)(\s*)\(",
    r"print_(debug|info|err|warn)(\s*)\(",
    r"_log_(err|warn|info|debug)(\s*)\(",
    # GPU
    r"(vau|dpu|jpeg|gmmu|vg)_(err|info|dbg|print)(\s*)\(",
    # WIFI
    r"oam_(error|warning|info)_log[0-4](\s*)\(",
    r"alg_ar_dbg_log[0-4](\s*)\(",
    r"common_log_dbg[0-4](\s*)\(",
    # GPU
    r"GRAPHIC_LOG(F|E|W|I|D|P)(\s*)\(",
    r"DISP_LOG(E|W|I|D)(\s*)\(",
    r"LV_LOG(E|W|I)(\s*)\(",
    # Meida
    r"ALOG(V|D|I|W|E)(\s*)\(",
    r"MEDIA_(DEBUG|ERR|WARNING|INFO|FATAL)_LOG(\s*)\(",
    r"HMF_LOG(F|E|W|I)(\s*)\(",
    r"MEDIA_HAL_LOG(V|D|I|W|E)(\s*)\(",
    r"CHECK_NULL_(RETURN|RETURN_NONE)(\s*)\(",
    r"CHECK_FAILED_(RETURN|RETURN_NONE|PRINT|RETURN_WITH_UNLOCK)(\s*)\(",
    r"CHK_NULL_(RETURN|RETURN_NONE|RETURN_WITH_UNLOCK)(\s*)\(",
    r"CHK_FAILED_(RETURN|NO_RETURN)(\s*)\(",
    r"(CHK_COND_RETURN|CHK_FALSE_RETURN|CHECK_STATE_SAME_RETURN)(\s*)\(",
    # OHOS
    r"HILOG_(DEBUG|ERROR|FATAL|INFO|WARN)(\s*)\(",
    r"SAMGR_LOG_(ERROR|FATAL|INFO|WARN)(\s*)\(",
    # app
    r"WEARABLE_LOG(D|I|W|E)(\s*)\(",
    # plat
    r"hcc_printf_err_log(\s*)\(",
    # audio
    r"audio_log[0-4](\s*)\(",
]  # 日志关键字规则

# DSP日志关键字规则，不要改变顺序，慎重修改
g_audio_log_entry = [
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_(s32|bool)(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_u32(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_h32(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_float(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_void(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_ret(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_fun(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_null_pointer(\s*)\(",
    r"audio_func_enter(\s*)\(",
    r"audio_func_exit(\s*)\(",
    r"sap_(alert|fatal|err|warn|dbg|trace)_log_info(\s*)\(",
    r"audio_log_(alert|fatal|err|warn|dbg|trace)(\s*)\(",
]  # DSP日志关键字规则，不要改变顺序，慎重修改

g_audio_log_key_index = -1
g_audio_log_addition_info = [" = %d",
                             " = %u",
                             " = 0x%08X",
                             " = %f",
                             " = %p",
                             "Error Code: [0x%08X]",
                             " Failed, Error Code: [0x%08X]",
                             " = %p,  Null Pointer!\\n",
                             "enter...",
                             "exit..."]

g_log_full_cont = '\"(.*)\"\s*[,|)]'
# 用于记录日志行数，日志多行时可能是第一行，有可能是最后一行；
# 0 表示第一行  1表示最后一行
g_log_entry_or_end_flag = -1

XML_PATH="build/config/target_config/<chip>/hdb_config/database_cfg/mk_hdb_xml.json"

def get_log_level(str):
    m = re.search(r'LOG_LEVEL_ERROR|ERR_LOG|err_log|(GRAPHIC|DISP|LV)_LOGF|(GRAPHIC|DISP|LV)_LOGE|WEARABLE_LOGE|_ERR|print_err|ERROR|FATAL', str)
    if m is not None:
        return 2,'ERROR'
    m = re.search(r'(CHK|CHECK)_NULL_RETURN|(CHK|CHECK)_NULL_RETURN_NONE|(CHK|CHECK)_FAILED_RETURN|CHECK_FAILED_RETURN_NONE', str)
    if m is not None:
        return 2,'ERROR'
    m = re.search(r'CHECK_FAILED_PRINT|CHECK_FAILED_RETURN_WITH_UNLOCK|CHK_NULL_RETURN_WITH_UNLOCK|CHK_FAILED_NO_RETURN', str)
    if m is not None:
        return 2,'ERROR'
    m = re.search(r'CHK_COND_RETURN|CHK_FALSE_RETURN', str)
    if m is not None:
        return 2,'ERROR'
    m = re.search(r'LOG_LEVEL_WARNING|WARN_LOG|warn_log|warning_log|(GRAPHIC|DISP|LV)_LOGW|WEARABLE_LOGW|_WARN|print_warn', str)
    if m is not None:
        return 3,'WARNING'
    m = re.search(r'alg_ar_dbg|common_log_dbg', str)
    if m is not None:
        return 3,'WARNING'
    m = re.search(r'LOG_LEVEL_INFO|INFO_LOG|info_log|(GRAPHIC|DISP|LV)_LOGI|GRAPHIC_LOGP|WEARABLE_LOGI|_INFO|print_info', str)
    if m is not None:
        return 5,'INFO'
    m = re.search(r'CHECK_STATE_SAME_RETURN', str)
    if m is not None:
        return 5,'INFO'
    m = re.search(r'LOG_LEVEL_DBG|(DBG|DEBUG)_LOG|(dbg|debug)_log|(GRAPHIC|DISP|LV)_LOGD|WEARABLE_LOGD|_DEBUG|print_debug', str)
    if m is not None:
        return 6,'DBG'

    m = re.search(r'_error_log|_log_err|_ERROR_|(vau|dpu|jpeg|gmmu|vg)_err|ALOGE|MEDIA_(ERR|FATAL)_LOG|HMF_LOG(F|E)|MEDIA_HAL_LOGE', str)
    if m is not None:
        return 2,'ERROR'
    m = re.search(r'_warning_log|_log_warn|_WARNING_|ALOGW|HMF_LOGW|MEDIA_HAL_LOGW', str)
    if m is not None:
        return 3,'WARNING'
    m = re.search(r'_info_log|_log_info|_INFO_|(vau|dpu|jpeg|gmmu|vg)_info|ALOGI|MEDIA_INFO_LOG|HMF_LOGI|MEDIA_HAL_LOGI', str)
    if m is not None:
        return 5,'INFO'
    m = re.search(r'_debug_log|_log_debug|(vau|dpu|jpeg|gmmu|vg)_(dbg|print)|ALOG(V|D)|MEDIA_DEBUG_LOG|MEDIA_HAL_LOG(V|D)', str)
    if m is not None:
        return 6,'DBG'

    print("no match for :%s" % str)
    return 7, 'no match'

# DSP日志级别识别，慎重修改
def get_audio_log_level(str):
    m = re.search(r'_alert|_err|_fatal', str)
    if m is not None:
        return 2,'ERROR'
    m = re.search(r'_warn', str)
    if m is not None:
        return 3,'WARNING'
    m = re.search(r'_trace', str)
    if m is not None:
        return 5,'INFO'
    m = re.search(r'_dbg', str)
    if m is not None:
        return 6,'DBG'
    m = re.search(r'_enter|_exit', str)
    if m is not None:
        return 5,'INFO'
    print("audio log no match for :%s" % str)
    return 7, 'no match'

def add_content_to_xml(data, msg_root_element):
    prim_pri  = data["LEVEL_STR"]
    prim_sz =  data["FMT"]
    prim_line = data["LINE_NUM"]
    prim_file = data["FILE_NAME"]
    prim_file_id = data["FILE_ID"]
    args_count = data["ARGS_COUNT"]
    mod_id = data["MOD_ID"]
    level = data["LEVEL_NUM"]

    new_element = ET.Element('')
    new_element.tag = 'MSG'
    new_element.attrib['STRUCTURE'] = "%s%d" % ("diag_log_msg", args_count)
    new_element.attrib['NAME'] = "%s @%s(%d),%s" % (prim_sz, prim_file, prim_line, prim_pri)

    #sdt log_id to hso msg_id
    #log_flg  mod_id   file_id   line_num   level
    #1bit     4bit     10bit     14bit      3bit
    msg_id = (1 << 31) | (mod_id << 27) | (prim_file_id << 17) | (prim_line << 3) | level
    new_element.attrib['ID'] = hex(msg_id)
    new_element.tail = '\n\t\t\t'
    msg_root_element.append(new_element)

# 根据日志状态机流程逐行解析日志内容
def parse_log_from_line(line):
    global g_fsm_status
    global g_log_str
    global g_log_str_full
    global g_log_str_full_flag
    global g_log_entry
    global g_log_full_cont
    global g_log_entry_or_end_flag
    global g_log_key_find
    global g_audio_log_key_index
    findkey = 0
    # 匹配日志接口
    if "LOG_ENTRY_FINDING" == g_fsm_status:
        g_log_entry_or_end_flag = -1
        g_audio_log_key_index = -1
        g_log_str = ""
        g_log_str_full = ""
        for x in g_log_entry:
            m = re.search(x, line.strip())
            if m is not None:
                findkey = 1
                break
        if 1 == findkey:
            g_fsm_status = "LOG_ENTRY_FOUNDED"
            g_log_str_full += line
            g_log_entry_or_end_flag = 0
            g_log_key_find = 1
            parse_log_from_line(line)

        # SAP log
        find_audio_key = 0
        index = 0
        for x in g_audio_log_entry:
            m = re.search(x, line.strip())
            if m is not None:
                find_audio_key = 1
                break
            index = index + 1
        if 1 == find_audio_key:
            g_fsm_status = "LOG_ENTRY_FOUNDED"
            g_log_str_full += line
            g_log_entry_or_end_flag = 0
            g_audio_log_key_index = index
            g_log_key_find = 1
            parse_log_from_line(line)
        else:
            return

    # 日志接口匹配，确认是否完整日志；确认日志是否正常结束
    elif "LOG_ENTRY_FOUNDED" == g_fsm_status:
        if g_audio_log_key_index >= 0 and g_audio_log_key_index <= 4:   # such as "sap_alert_log_u32"
            m1 = re.search("\(\s*(\S+)\s*\)", line)
            if m1 is not None:
                g_log_str = m1.groups()[0] + g_audio_log_addition_info[g_audio_log_key_index] + '\0';
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
        elif g_audio_log_key_index == 5: # such as "sap_fatal_log_err_code"
            m1 = re.search("\(\s*(\w+)\s*\)", line)
            if m1 is not None:
                g_log_str = g_audio_log_addition_info[g_audio_log_key_index] + '\0';
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
        elif g_audio_log_key_index == 6: # such as "sap_err_log_fun_err"
            m1 = re.search("\(\s*(\w+)\s*,", line)
            if m1 is not None:
                g_log_str = "Call " + m1.groups()[0] + g_audio_log_addition_info[g_audio_log_key_index] + '\0';
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
        elif g_audio_log_key_index == 7: # such as "sap_err_log_null_pointer"
            m1 = re.search("\(\s*(\S+)\s*\)", line)
            if m1 is not None:
                g_log_str = m1.groups()[0] + g_audio_log_addition_info[g_audio_log_key_index] + '\0';
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
        elif g_audio_log_key_index == 8 or g_audio_log_key_index == 9: # such as "audio_func_enter"
            m1 = re.search("\(\)", line)
            if m1 is not None:
                g_log_str = g_audio_log_addition_info[g_audio_log_key_index] + '\0';
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
        else:
            m1 = re.search(g_log_full_cont, line)  # 完整日志格式
            m2 = re.search("\"(.+)", line)  # 部分日志格式 ，引号开头
            if m1 is not None:
                g_log_str = m1.groups()[0] + '\0'
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
            elif m2 is not None:
                g_fsm_status = "LOG_CONT_END_FINDING"
                g_log_str = m2.groups()[0]  # 引号后面的内容为日志内容
                m1 = re.search(r'(.+)"\s*[,|)]', line)
                if m1 is not None:
                    g_fsm_status = "LOG_EXIT_FINDING"
            if "LOG_ENTRY_FOUNDED" == g_fsm_status:
                m3 = re.search(r"\)[\s]*\;", line.strip())
                if m3 is not None:
                    g_fsm_status = "LOG_ENTRY_FINDING"
                    g_log_entry_or_end_flag = -1
                    g_audio_log_key_index = -1
            if g_log_str_full.find(line) == -1:
                g_log_str_full += line

    # 匹配部分日志接口
    elif "LOG_CONT_END_FINDING" == g_fsm_status:
        m = re.search(r"[\s]+(.+)\"", line)
        if m is not None:
            temp = m.groups()[0]
            m1 = re.search(r'(.+)"\s*[,|)]', line)  #部分日志可以跨多行
            if m1 is None:
                g_log_str += temp
            else:
                g_log_str += (temp + '\0')
                g_fsm_status = "LOG_EXIT_FINDING"
                parse_log_from_line(line)
        else:
            g_log_str += line.strip()  # 纯日志内容 ，继续找日志
        if g_log_str_full.find(line) == -1:
            g_log_str_full += line

    # 匹配日志结束符
    elif "LOG_EXIT_FINDING" == g_fsm_status:
        m = re.search(r"\)[\s]*\;", line.strip())
        if m is not None:
            g_fsm_status = "LOG_ENTRY_FINDING"  # 继续逐行扫描下一条日志入口
            g_log_entry_or_end_flag = 1
        if g_log_str_full.find(line) == -1:
            g_log_str_full += line


g_file_id_dict = {}  # file_id_str  file_id_num
g_last_file_id_num = 0
g_moudle_id = {
    'wifi' : 0,
    'bt' : 1,
    'gnss' :2,
    'dsp' : 3,
    'pf' : 4,
    'media' : 5,
    'nfc': 6,
    'app' : 7,
    'gpu': 8,
    'gui' : 9,
    'slp' : 10,
    'bth' : 11,
    'ohos' : 12,
}

def parse_single_source_file(cfilepath, auto_def, module_name):
    global g_log_key_find
    global g_log_str
    global g_fsm_status
    global log_info_list
    global g_audio_log_key_index
    curr_line_num = 0      # 当前行行号
    file_id_num = 0      # 本文件号
    line_num_save = 0
    log_info_list = []

    m = re.search(r"\w.[cC|hH]", cfilepath)
    if m is None:
        print("Error!%s is not C or H File" % cfilepath)
        return

    # 增加对文件中换行符的处理
    destfile = cfilepath + "_temp"
    modify_newline_on_linux(cfilepath, destfile)
    # 打开源文件，读取文件内容
    try:
        if py_version.startswith("3"):
            try:
                with open(destfile, 'r', encoding="UTF-8") as fd_src:
                    datalines = [line for line in fd_src]
            except Exception:
                with open(destfile, 'r', encoding="ISO-8859-1") as fd_src:
                    datalines = [line for line in fd_src]
        else:
            with open(destfile, 'r') as fd_src:
                datalines = [line for line in fd_src]
    except Exception:
        print("open file %s failed." % cfilepath)
        return

    os.remove(destfile)
    if module_name not in g_file_id_dict:
        return
    _, filename = os.path.split(cfilepath)
    if auto_def:
        file_id_str = filename.replace('.' , '_').upper()
        if file_id_str not in g_file_id_dict[module_name]:
            return
        g_fsm_status = "LOG_ENTRY_FINDING"
    else:
        m = re.search(r"#define[\s]+THIS_FILE_ID[\s]+([\w]*)", '\n'.join(datalines))
        if m is None:
            return 0
        file_id_str = m.group(1)
        g_fsm_status = "LOG_ENTRY_FINDING"

    # 遍历行内容
    for line in datalines:
        # 行号加1
        curr_line_num += 1

        m = re.search(r'^\/\/', line.strip())
        if m is not None:
            continue
        # 判断是否可识别的普通日志文件
        parse_log_from_line(line)
        if 1 == g_log_key_find:
            try:
                file_id_num = g_file_id_dict[module_name][file_id_str]
                moudle_id_num = g_moudle_id[module_name]
            except KeyError:
                #print(
                #    "warning: Moudle:%s, %s not defined. fileName[%s]." %
                #    (moudle_name, file_id_str, filename))
                g_undefine_id.append((file_id_str, filename))
                break
            g_log_key_find = 0
            line_num_save = curr_line_num
        # 解析日志，判断是否找到，如果找到写入文件
        if 1 == g_log_entry_or_end_flag:
            g_log_str = g_log_str.replace('\\r', "")  # 清除\r，\n，空格
            g_log_str = g_log_str.replace('\\n', "")
            g_log_str = g_log_str.replace('\r', "")  # 清除\r，\n，空格
            g_log_str = g_log_str.replace('\n', "")
            g_log_str = g_log_str.replace("\\", "")
            g_log_str = g_log_str.replace('""', "")
            g_log_str = g_log_str.replace("\000", "")
            g_log_str.strip()
            if (g_audio_log_key_index != -1):
                level_num, level_str = get_audio_log_level(g_log_str_full)
            else:
                level_num, level_str = get_log_level(g_log_str_full)
            data = { }
            data["MOD_ID"] = moudle_id_num
            data["FILE_ID"] = file_id_num
            if (aich_name == "cortex_m7"):
                data["LINE_NUM"] = line_num_save
            else:
                data["LINE_NUM"] = curr_line_num
            data["FMT"] = g_log_str
            data["LEVEL_NUM"] = level_num
            data["LEVEL_STR"] = level_str
            data["ARGS_COUNT"] = g_log_str.count("%")
            data["FILE_NAME"] =  cfilepath.split('/')[-1]
            log_info_list.append(data)
            g_log_key_find = 0
            g_audio_log_key_index = -1
    return 0


# 保存FILD_ID
def save_file_id_dict(line_str, moudle_name):
    global g_file_id_dict
    global g_last_file_id_num

    file_id_str = ''
    file_id_num = 0
    if line_str == '':
        return 0

    m2 = re.search("[^A-Za-z0-9_=]", line_str)
    if m2 is None:
        m1 = line_str.split('=')  # XXXXXXX = dddd,
        if len(m1) == 2:
            file_id_str = m1[0]  # 获取file id字符串
            file_id_num = m1[1]
            g_file_id_dict[moudle_name][file_id_str] = int(file_id_num)
            g_last_file_id_num = int(file_id_num) + 1
        else:
            file_id_str = line_str
            g_file_id_dict[moudle_name][file_id_str] = g_last_file_id_num
            g_last_file_id_num += 1
    else:
        return -1
    return 0


# 打印fileid的映射字典
def print_file_id_dict():
    print(timestamp() + " start to print FileIdDict::")
    for system_name in g_file_id_dict:
        for key in g_file_id_dict[system_name]:
            print(timestamp() + " key:%s value:%d" % (key,\
                  g_file_id_dict[system_name][key]))


# 获取FILE_ID枚举值
def get_file_id_enum_in_file(h_file_path):
    global g_file_id_dict
    global g_last_file_id_num
    g_last_file_id_num = 0

    moudle_name = os.path.basename(h_file_path).split('_')[-1][:-2]
    # btc将file_id头文件迁移后取名为log_def_btc.h 实际moudle_name任是bt
    # bth将file_id头文件迁移后取名为log_def_bth.h 实际moudle_name任是bt
    if moudle_name == 'btc':
        moudle_name = 'bt'
    if chip == 'bs25' and core_name == 'bt_core' and moudle_name == "bth":
        moudle_name = 'bt'
    if moudle_name not in g_file_id_dict:
        g_file_id_dict[moudle_name] = {}
    m = re.search(r'\w+\.h$', h_file_path)
    if m is None:
        print(timestamp() + ' Wrong type file, please input head file path.')
        return -1

    if not os.path.exists(h_file_path):
        print(timestamp() + ' Head file [%s] did not exist.' % (h_file_path))
        return 0
    try:
        if py_version.startswith("3"):
            with open(h_file_path, 'r', encoding="UTF-8") as fd:
                id_file = fd.read()
        else:
            with open(h_file_path, 'r') as fd:
                id_file = fd.read()
    except Exception as e:
        print(e)
        return -1
    id_list = re.search(r"typedef enum[\s]*\{([\d\D]*?)\}", id_file).group(1)
    id_list = id_list.replace(',', '').replace(' ', '').split('\n')
    for line_str in id_list:
        line_str = line_str.strip().split("/")[0]
        save_file_id_dict(line_str, moudle_name)

    return 0

def get_file_id_enum_in_paths(h_file_paths):
    for h_file in h_file_paths:
        if not h_file:
            continue
        if get_file_id_enum_in_file(h_file) != 0:
            return -1
    return 0


# 文件换行符处理
def modify_newline_on_linux(srcfile, destfile):
    with open(srcfile, "rb") as fsrc, open(destfile, "wb") as fdest:
        while True:
            cs = fsrc.read(1)
            if len(cs) > 0:
                if cs == "\r":
                    fdest.write(cs)
                    cs = fsrc.read(1)
                    if cs != "\n":
                        fdest.write("\n")
                        fdest.write(cs)
                    else:
                        fdest.write(cs)
                else:
                    fdest.write(cs)
            else:
                break

#mkdir ${target} ${root} ${chip} ${core}
def generate_db_temp_dir(sys_argv):
    root = sys_argv[2]
    chip = sys_argv[3]
    core_name = sys_argv[4]
    in_path = XML_PATH
    in_path = in_path.replace('<chip>', chip)

    # get global settings
    db_conf = None
    conf = os.path.join(root, in_path)
    with open(conf, 'r') as f:
        db_conf = json.load(f)

    base_file_dir = os.path.join(os.path.join(root, db_conf["HDB_XML_TEMP_BASE_DIR"]), core_name)
    if not os.path.exists(base_file_dir):
        os.makedirs(base_file_dir)

#${target} ${root} ${chip} ${core} ${src_file} ${file_id_dir}
def generate_db_xml(root, core_name, auto_def, module_name, src_file, closed_flag):
    global log_info_list
    if not os.path.isfile(src_file):
        return

    # get global settings
    db_conf = None
    conf = os.path.join(root, in_path)
    with open(conf, 'r') as f:
        db_conf = json.load(f)

    parse_single_source_file(src_file, auto_def, module_name)
    if len(log_info_list) == 0:
        return

    _, filename = os.path.split(src_file)
    filename = filename + ".xml"
    if (closed_flag != True):
        base_file_dir = os.path.join(os.path.join(root, db_conf["HDB_XML_TEMP_BASE_DIR"]), core_name)
    else:
        base_file_dir = os.path.join(os.path.join(root, db_conf["HDB_XML_TEMP_BASE_DIR"]), "closed_comp")
        if not os.path.exists(base_file_dir):
            os.makedirs(base_file_dir)

    dst_xml_full_name = os.path.join(base_file_dir, filename)

    msg_root_element = ET.Element('MSG_LOG')
    for data in log_info_list:
        add_content_to_xml(data, msg_root_element)

    tree = ET.ElementTree(msg_root_element)
    tree.write(dst_xml_full_name)

# main
if __name__ == "__main__":
    if (sys.argv[1] == "mkdir"):
        generate_db_temp_dir(sys.argv)
        sys.exit(0)
    root = sys.argv[1]
    chip = sys.argv[2]
    core_name = sys.argv[3]
    aich_name = sys.argv[4]
    in_path = XML_PATH
    in_path = in_path.replace('<chip>', chip)
    auto_def = sys.argv[5]
    if auto_def == "TRUE" or auto_def == "True":
        auto_def = True
    else:
        auto_def = False
    module_name = sys.argv[6]
    closed_flag = sys.argv[7]
    if closed_flag == "TRUE" or closed_flag == "True":
        closed_flag = True
    else:
        closed_flag = False
    info_file = sys.argv[8]

    try:
        with open(info_file, 'r') as f:
            text = f.read()
            file_id_dir = text.split("####")[0].split(',')
            src_files = text.split("####")[1].split(',')

        if file_id_dir != None and get_file_id_enum_in_paths(file_id_dir) != 0:
            sys.exit(1)

        for src_file in src_files:
            src_file = src_file.strip()
            generate_db_xml(root, core_name, auto_def, module_name, src_file, closed_flag)
    except:
        print("[WARN] open %s exception" % info_file)
