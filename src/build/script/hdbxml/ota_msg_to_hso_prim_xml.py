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
ota_msg_list = []

def gen_ota_msg_dict_fun(datalines):
    global ota_msg_list
    # 遍历行内容
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
        m = re.search(r'^\/\/', line.strip())
        if m is not None:
            continue
        m = re.search(r'\[.*(START|END)\]', line.strip())
        if m is not None:
            continue

        member = line.strip().split(" ")
        if len(member) == 2:
            data = {}
            data["NAME"] = member[1]
            data["ID"] = member[0]
            ota_msg_list.append(data)

def add_content_to_xml(data, msg_root_element):
    new_element = ET.Element('')
    new_element.tag = 'MSG'
    new_element.attrib['NAME'] = data["NAME"]
    new_element.attrib['ID'] = hex((2 << 16) | (0xffff & int(data["ID"], 16))) # OM_MSG_TYPE_OTA = 2
    new_element.tail = '\n\t\t\t'
    msg_root_element.append(new_element)

def write_ota_msg_prim_xml_file():
    global G_PARAMS

    base_file_dir = os.path.join(G_PARAMS['HDB_XML_TEMP_BASE_DIR'], "ota_msg")
    if not os.path.isdir(base_file_dir):
        os.makedirs(base_file_dir)

    dst_xml_full_name = os.path.join(base_file_dir, "ota_msg.xml")

    msg_root_element = ET.Element('MSG_LOG')
    for ota_msg in ota_msg_list:
        add_content_to_xml(ota_msg, msg_root_element)

    tree = ET.ElementTree(msg_root_element)
    tree.write(dst_xml_full_name)

def generate_db_file():
    global G_PARAMS
    ota_msg_dir = G_PARAMS["OTA_MSG_DIR"]

    ota_msg_file = os.path.join(ota_msg_dir, "OTA_MSG_List.txt")
    try:
        if py_version.startswith("3"):
            try:
                with open(ota_msg_file, 'r', encoding="UTF-8") as fd_src:
                    datalines = [line for line in fd_src]
            except Exception:
                with open(ota_msg_file, 'r', encoding="ISO-8859-1") as fd_src:
                    datalines = [line for line in fd_src]
        else:
            with open(ota_msg_file, 'r') as fd_src:
                datalines = [line for line in fd_src]
    except Exception:
        print("open file %s failed." % cfilepath)
        return

    gen_ota_msg_dict_fun(datalines)
    write_ota_msg_prim_xml_file()

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
    G_PARAMS["OTA_MSG_DIR"] = os.path.join(root, db_conf["OTA_MSG_DIR"])

    generate_db_file()

# main
if __name__ == "__main__":
    generate_db()
