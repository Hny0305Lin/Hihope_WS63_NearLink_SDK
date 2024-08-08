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
from xml.etree.ElementTree import ElementTree

XML_PATH="build/config/target_config/<chip>/hdb_config/database_cfg/mk_hdb_xml.json"

def get_msg_root(subsystem):
    msg_root_element = ET.Element('')
    for child in subsystem:
        if child.tag == 'MSG_LOG':
            msg_root_element = child
            break
    if  msg_root_element.tag == 'MSG_LOG':
        temp_attrib = msg_root_element.attrib
        temp_text = msg_root_element.text
        temp_tail = msg_root_element.tail
        msg_root_element.attrib = temp_attrib
        msg_root_element.text = temp_text
        msg_root_element.tail = temp_tail
    return msg_root_element

def get_subsystem_by_name(tree, name):
    root = tree.getroot()
    for child in root:
        if child.attrib["NAME"] == name:
            return child
    return None

def merge_db_xml(root, chip, core_name):

    xml_src_file = G_PARAMS['HDB_PRIM_XML_SRC_FILE'].strip()
    dst_xml_file = G_PARAMS['HDB_PRIM_XML_DST_FILE'].strip()
    base_dir  = G_PARAMS['HDB_XML_TEMP_BASE_DIR']

    mod_list = []
    if core_name == 'bt_core':
        mod_list = ['bt_core', 'bt_status', 'ota_msg'] #bt_status.xml、ota_msg.xml目前都是在bt编译时生成
    else:
        mod_list = [core_name]

    dst_xml_dir = os.path.abspath(os.path.join(dst_xml_file, ".."))
    if not os.path.isdir(dst_xml_dir):
        os.makedirs(dst_xml_dir)

    if not os.path.exists(dst_xml_file):
        shutil.copy(xml_src_file, dst_xml_file)

    for mod in mod_list:
        if not os.path.isdir(os.path.join(base_dir, mod)):
            continue
        tree_dst = ET.parse(dst_xml_file)
        subsystem = get_subsystem_by_name(tree_dst, mod)
        if subsystem is None:
            print("Error: subsystem %s is not in %s" %(mod, dst_xml_file))
            continue

        msg_root_element = get_msg_root(subsystem)
        msg_root_element.clear()

        filelist = os.listdir(os.path.join(base_dir, mod))
        for filename in filelist:
            tmp_xml_file = os.path.join(os.path.join(base_dir, mod), filename)
            tree = ElementTree()
            tree.parse(tmp_xml_file)
            root = tree.getroot()
            if root.tag == 'MSG_LOG':
                for child in root:
                    msg_root_element.append(child)
            else:
                subsystem_src = get_subsystem_by_name(tree, mod)
                msg_element_src = get_msg_root(subsystem_src)
                for child in msg_element_src:
                    msg_root_element.append(child)
        tree_dst.write(dst_xml_file)

# main
if __name__ == "__main__":
    global G_PARAMS

    root = sys.argv[1]
    chip = sys.argv[2]
    core_name = sys.argv[3]
    if len(sys.argv) == 5:
        hso_en_bt = sys.argv[4]
    else:
        hso_en_bt = ""

    in_path = XML_PATH
    in_path = in_path.replace('<chip>', chip)

    db_conf = None
    conf = os.path.join(root, in_path)
    with open(conf, 'r') as f:
        db_conf = json.load(f)

    G_PARAMS = {}
    G_PARAMS['HDB_XML_TEMP_BASE_DIR'] = os.path.join(root, db_conf["HDB_XML_TEMP_BASE_DIR"])
    G_PARAMS['HDB_PRIM_XML_DST_FILE'] = os.path.join(root, db_conf["HDB_PRIM_XML_DST_FILE"])
    G_PARAMS['HDB_PRIM_XML_SRC_FILE'] = os.path.join(root, db_conf["HDB_PRIM_XML_SRC_FILE"])

    merge_db_xml(root, chip, core_name)
    if hso_en_bt == "True":
        merge_db_xml(root, chip, 'bt_core')

    if ("HDB_XML_PRE_GENERATED_CORE" in db_conf):
        G_PARAMS['HDB_XML_PRE_GENERATED_CORE'] = db_conf["HDB_XML_PRE_GENERATED_CORE"]
        core_list = []
        core_list = G_PARAMS['HDB_XML_PRE_GENERATED_CORE']

        for core in core_list:
            merge_db_xml(root, chip, core)
