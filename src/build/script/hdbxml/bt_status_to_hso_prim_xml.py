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

G_PARAMS = {}
XML_PATH="build/config/target_config/<chip>/hdb_config/database_cfg/mk_hdb_xml.json"

g_fsm_status = ""
enum_dict = {}
stuct_dict = {}

def print_enum_dict(dict_info):
    for name in dict_info:
        print("    %s" % name)
        xx = dict_info[name]
        for key in xx:
            print("        %s" % key)

def print_struct_dict(dict_info):
    for msg_id in dict_info:
        print("    %s" % msg_id)
        msg_dict = dict_info[msg_id]
        for key in msg_dict:
            print("        %s" % key)
            if key != "struct_para_list":
                print("            %s" % msg_dict[key])
            else:
                for para in msg_dict[key]:
                    print("            %s" % para)

def gen_enum_dict_fun(datalines):
    global g_fsm_status
    global enum_dict
    enum_name = ""
    g_fsm_status = "FIND_ENUM_START"
    # 遍历行内容
    for line in datalines:
        line = line.replace('\r\n', '')
        line = line.replace('\t', ' ')
        if len(line) == 0:
            continue
        m = re.search(r'^\/\/', line.strip())
        if m is not None:
            continue
        if g_fsm_status == "FIND_ENUM_START":
            m = re.search(r'\{\s*(.*)\s*START\s*\}', line.strip())
            if m is not None:
                enum_name = m.groups()[0].strip().replace(" ", "_")
                g_fsm_status = "FIND_ENUM_MEMBER"
                if enum_name not in enum_dict:
                    enum_dict[enum_name] = []
        elif g_fsm_status == "FIND_ENUM_MEMBER":
            m = re.search(r'\{\s*PUBLIC\s*END\s*\}', line.strip())
            if m is not None:
                g_fsm_status = "FIND_ENUM_START"
            else:
                member = line.strip().split(" ")
                if len(member) == 2:
                    temp = ("    %s = %s," % (member[1].split(".")[0], member[0]))
                    if enum_dict[enum_name].count(temp) == 0:
                        enum_dict[enum_name].append(temp)
    g_fsm_status = ""
    #print_enum_dict(enum_dict)

def add_enum_dict_fun(msg_id, para_index, enum_list):
    enum_name = ("bt_status_enum_msg_id_%x_%d" % (msg_id, para_index))
    if enum_name not in enum_dict:
        enum_dict[enum_name] = []
    for item in enum_list:
        member = item.strip().split(" ")
        if len(member) == 2:
            temp = ("    %s = %s," % (member[1], member[0]))
            if enum_dict[enum_name].count(temp) == 0:
                enum_dict[enum_name].append(temp)
    return enum_name

def merge_enum_dict_fun(msg_id, para_index, enum_list):
    enum_name = ("bt_status_enum_msg_id_%x_%d_" % (msg_id, para_index))
    for item in enum_list:
        enum_name = ("%s%s_" % (enum_name, item.strip()[2:].replace(" ", "_")))

    if enum_name not in enum_dict:
        enum_dict[enum_name] = []

    for item in enum_list:
        temp = item.strip()[2:].replace(" ", "_")
        enum_dict[enum_name] += enum_dict[temp]
    return enum_name

def gen_struct_dict_fun(datalines):
    global g_fsm_status
    g_fsm_status = "FIND_STRUCT_START"
    para_count = 0
    msg_id = 0
    msg_fmt = ""

    for line in datalines:
        line = line.replace('\r\n', '')
        line = line.replace('\t', ' ')
        line = line.replace('\\r', "")
        line = line.replace('\\n', "")
        line = line.replace('\r', "")
        line = line.replace('\n', "")
        line = line.replace("\\", "")
        if len(line) == 0:
            continue
        m = re.search(r'^\/\/(.*)', line.strip())
        if m is not None:
            continue
        if g_fsm_status == "FIND_STRUCT_START":
            m = re.search(r'(MsgID=\S*)\s*;\s*(Parameter Number=[0-9]*)\s*;\s*(.*)\]', line.strip())
            if m is not None:
                para_count = int(m.groups()[1].strip().split("=")[1], 10)
                msg_id = int(m.groups()[0].strip().split("=")[1], 16)
                msg_fmt = m.groups()[2].strip()
                if msg_id not in stuct_dict:
                    temp = { }
                    temp["full_str"] = line
                    temp["para_count"] = para_count
                    temp["msg_id"] = msg_id
                    temp["msg_fmt"] = msg_fmt
                    temp["struct_para_list"] = []
                    stuct_dict[msg_id] = temp
                g_fsm_status = "FIND_STRUCT_MEM"
        elif g_fsm_status == "FIND_STRUCT_MEM":
            temp = ("Parameter%d_CommandList\s*End" % (para_count))
            m = re.search(temp, line.strip())
            if m is not None:
                g_fsm_status = "FIND_STRUCT_START"
            stuct_dict[msg_id]["struct_para_list"].append(line)
    #print_struct_dict(stuct_dict)

def parse_struct_para_list(msg_id, struct_para_list):
    global g_fsm_status
    g_fsm_status = "FIND_PARA_START"
    index = 0
    para_mem_dict = {}
    struct_str = "typedef struct {\n"
    while len(struct_para_list) > 0:
        para_str = struct_para_list.pop(0)
        if g_fsm_status == "FIND_PARA_START":
            m = re.search(r'CommandList\s*Start', para_str)
            if m is not None:
                g_fsm_status = "FIND_PARA_MEM"
                index += 1
                if index not in para_mem_dict:
                    para_mem_dict[index] = []
        elif g_fsm_status == "FIND_PARA_MEM":
            m = re.search(r'CommandList\s*End', para_str)
            if m is not None:
                g_fsm_status = "FIND_PARA_START"
            else:
                m = re.search(r'Index\s*Show', para_str)
                if m is None:
                    para_mem_dict[index].append(para_str)

    for item in para_mem_dict:
        need_enum_ref = 0
        if len(para_mem_dict[item]) == 0:
            struct_str += ("    td_u32 para_%s;\n" % (item))
        else:
            for str in para_mem_dict[item]:
                if str.find("$$") != -1:
                    need_enum_ref = 1
                    break
            if need_enum_ref == 0:
                enum_name = add_enum_dict_fun(msg_id, item, para_mem_dict[item])
                struct_str += ("    %s para_%s;\n" % (enum_name, item))
            else:
                if len(para_mem_dict[item]) > 1:
                    enum_name = merge_enum_dict_fun(msg_id, item, para_mem_dict[item])
                    struct_str += ("    %s para_%s;\n" % (enum_name, item))
                else:
                    enum_name = para_mem_dict[item][0].strip()[2:].replace(" ", "_")
                    struct_str += ("    %s para_%s;\n" % (enum_name, item))


    struct_str += ("} bt_status_msg_id_%x;\n\n" % (msg_id))
    return struct_str

def get_struct_def_string():
    struct_str = ""
    for msg_id in stuct_dict:
        msg_dict = stuct_dict[msg_id]
        for key in msg_dict:
            if key == "struct_para_list":
                struct_str += parse_struct_para_list(msg_id, list(msg_dict[key]))
    return struct_str

def get_enum_def_string():
    enum_str = ""
    for item in enum_dict:
        enum_str = "%stypedef enum {\n" % enum_str
        for i in enum_dict[item]:
            enum_str = ("%s%s\n" % (enum_str, i))
        enum_str = ("%s} %s;\n\n" % (enum_str, item))
    return enum_str

def write_bt_status_def_file():
    global G_PARAMS
    txt_dst_dir = G_PARAMS["HDB_TXT_DST_DIR"]

    struct_str = get_struct_def_string()
    enum_str = get_enum_def_string()

    dst_file = os.path.join(txt_dst_dir, "bt_status_hso_msg_struct_def.txt")

    with open(dst_file, 'w') as dst_f:
        dst_f.write('#include "base_datatype_def.txt"' + '\n')
        dst_f.write(enum_str)
        dst_f.write(struct_str)

def add_content_to_xml(data, msg_root_element):
    new_element = ET.Element('')
    new_element.tag = 'MSG'
    new_element.attrib['STRUCTURE'] = ("bt_status_msg_id_%x" % (data["msg_id"]))
    new_element.attrib['NAME'] = data["msg_fmt"]
    new_element.attrib['ID'] = hex((31 << 16) | (0xffff & data["msg_id"])) # OM_MSG_TYPE_STATUS = 31
    new_element.tail = '\n\t\t\t'
    msg_root_element.append(new_element)

def write_bt_status_prim_xml_file():
    global G_PARAMS

    base_file_dir = os.path.join(G_PARAMS['HDB_XML_TEMP_BASE_DIR'], "bt_status")
    if not os.path.isdir(base_file_dir):
        os.makedirs(base_file_dir)

    dst_xml_full_name = os.path.join(base_file_dir, "bt_status.xml")

    msg_root_element = ET.Element('MSG_LOG')
    for msg_id in stuct_dict:
        add_content_to_xml(stuct_dict[msg_id], msg_root_element)

    tree = ET.ElementTree(msg_root_element)
    tree.write(dst_xml_full_name)

def generate_db_file():
    global G_PARAMS
    bt_status_dir = G_PARAMS["BT_STATUS_DIR"]

    bt_status_file = os.path.join(bt_status_dir, "STATUS_CommandList.txt")
    try:
        if py_version.startswith("3"):
            try:
                with open(bt_status_file, 'r', encoding="UTF-8") as fd_src:
                    datalines = [line for line in fd_src]
            except Exception:
                with open(bt_status_file, 'r', encoding="ISO-8859-1") as fd_src:
                    datalines = [line for line in fd_src]
        else:
            with open(bt_status_file, 'r') as fd_src:
                datalines = [line for line in fd_src]
    except Exception:
        print("open file %s failed." % cfilepath)
        return

    gen_enum_dict_fun(datalines)
    gen_struct_dict_fun(datalines)

    write_bt_status_def_file()
    write_bt_status_prim_xml_file()

def generate_db():
    global G_PARAMS
    root = sys.argv[1]
    chip = sys.argv[2]
    in_path = XML_PATH
    in_path = in_path.replace('<chip>', chip)

    db_conf = None
    conf = os.path.join(root, in_path)
    with open(conf, 'r') as f:
        db_conf = json.load(f)

    G_PARAMS = {}
    G_PARAMS['HDB_XML_TEMP_BASE_DIR'] = os.path.join(root, db_conf["HDB_XML_TEMP_BASE_DIR"])
    G_PARAMS["HDB_TXT_DST_DIR"] = os.path.join(root, db_conf["HDB_TXT_DST_DIR"])
    G_PARAMS["HDB_TXT_BASE_DATATYPE"] = os.path.join(root, db_conf["HDB_TXT_BASE_DATATYPE"])
    G_PARAMS["HDB_TXT_SRC_DIR"] = os.path.join(root, db_conf["HDB_TXT_SRC_DIR"])
    G_PARAMS["BT_STATUS_DIR"] = os.path.join(root, db_conf["BT_STATUS_DIR"])

    txt_src_dir = G_PARAMS["HDB_TXT_SRC_DIR"]
    txt_dst_dir = G_PARAMS["HDB_TXT_DST_DIR"]
    base_datatype_def_txt = G_PARAMS["HDB_TXT_BASE_DATATYPE"]
    bt_status_dir = G_PARAMS["BT_STATUS_DIR"]

    if not os.path.exists(txt_src_dir):
        return
    if not os.path.exists(base_datatype_def_txt):
        return
    if not os.path.exists(bt_status_dir):
        return
    if not os.path.exists(txt_dst_dir):
        os.makedirs(txt_dst_dir)

    generate_db_file()

# main
if __name__ == "__main__":
    generate_db()
