#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.

import os
import re
import sys
import platform
import string
import shutil
import json

XML_PATH="build/config/target_config/<chip>/hdb_config/database_cfg/mk_hdb_xml.json"

def recursive_copy_overwrite(src, dest, ignore=None):
    if os.path.isdir(src):
        if not os.path.isdir(dest):
            os.makedirs(dest)
        files = os.listdir(src)
        if ignore is not None:
            ignored = ignore(src, files)
        else:
            ignored = set()
        for f in files:
            if f not in ignored:
                recursive_copy_overwrite(os.path.join(src, f), os.path.join(dest, f), ignore)
    else:
        shutil.copyfile(src, dest)

def xml_ignore(path, names):
    ignored_names = []
    for name in names:
        if name.endswith('mss_prim_db.xml'):
            ignored_names.append(name)
    return set(ignored_names)

def process_pre_generated_db_xml():
    global G_PARAMS
    root = sys.argv[1]
    chip = sys.argv[2]
    in_path = XML_PATH
    in_path = in_path.replace('<chip>', chip)

    db_conf = None
    conf = os.path.join(root, in_path)
    with open(conf, 'r') as f:
        db_conf = json.load(f)

    if ("HDB_XML_PRE_GENERATED_DIR" not in db_conf):
        print("HDB_XML_PRE_GENERATED_DIR NOT in json")
        return

    G_PARAMS = {}
    G_PARAMS['BUILD_TEMP_PATH'] = os.path.join(root, db_conf["BUILD_TEMP_PATH"])
    G_PARAMS['HDB_XML_PRE_GENERATED_DIR'] = db_conf["HDB_XML_PRE_GENERATED_DIR"]

    src_dir = []
    src_dir = G_PARAMS['HDB_XML_PRE_GENERATED_DIR']
    dst_dir = G_PARAMS['BUILD_TEMP_PATH'].strip()

    for dir in src_dir:
        full_dir = os.path.join(root, dir)
        if(os.path.exists(full_dir)):
            recursive_copy_overwrite(full_dir, dst_dir, xml_ignore)

# main
if __name__ == "__main__":
    process_pre_generated_db_xml()
