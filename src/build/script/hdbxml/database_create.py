#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.

import os
import sys
import json
import shutil

XML_PATH="build/config/target_config/<chip>/hdb_config/database_cfg/mk_hdb_xml.json"
G_PARAMS = {}

# 生成database
def database_create():
    database_base = G_PARAMS["DATABASE_BASE"] # 获取database目录路径
    
    # 判断对应版本，用于生成out下database目录后缀
    target_cus = G_PARAMS.get("DATABASE_VERSION", "").split()
    if not target_cus:
        target_cus = [""]
    for cus in set(target_cus):
        if cus:
            database_target_name = '_'.join(['database', cus])
        else:
            database_target_name = 'database'
        database_target = os.path.join(G_PARAMS["OUT_DIR"], database_target_name)
        # 生成database
        shutil.rmtree(database_target, ignore_errors=True, onerror=None)
        shutil.copytree(database_base, database_target)
        mss_prim_path = G_PARAMS["DATABASE_MSS_PRIM_PATH"]
        diag_dir_path = G_PARAMS["DATABASE_DIAG_DIR_PATH"]
        cores = G_PARAMS["DATABASE_VERSION_CORE"].split()
        for core in cores:
            if os.path.exists(mss_prim_path):
                shutil.copy(mss_prim_path, os.path.join(database_target, core, 'system', 'hdbcfg'))
            if os.path.exists(diag_dir_path):
                filelist = os.listdir(diag_dir_path)
                for f in filelist:
                    shutil.copy(os.path.join(diag_dir_path, f), os.path.join(database_target, core, 'system', 'diag'))
            # 暂时修改out目录下生成的database的目录为cco
            if core == 'acore':
                try:
                    os.rename(os.path.join(database_target, core), os.path.join(database_target, 'cco'))
                except:
                    print("[WARN] rename %s exception" % os.path.join(database_target, core))
                    continue

def create():
    global G_PARAMS
    root = sys.argv[1]
    chip = sys.argv[2]
    in_path = XML_PATH
    in_path = in_path.replace('<chip>', chip)

    db_conf = None
    conf = os.path.join(root, in_path)
    with open(conf, 'r') as f:
        db_conf = json.load(f)

    G_PARAMS["DATABASE_BASE"] = os.path.join(root, db_conf["DATABASE_BASE"])
    G_PARAMS["DATABASE_MSS_PRIM_PATH"] = os.path.join(root, db_conf["DATABASE_MSS_PRIM_PATH"])
    G_PARAMS["DATABASE_DIAG_DIR_PATH"] = os.path.join(root, db_conf["DATABASE_DIAG_DIR_PATH"])
    G_PARAMS["OUT_DIR"] = os.path.join(root, db_conf["OUT_DIR"])
    G_PARAMS["DATABASE_VERSION"] = db_conf["DATABASE_VERSION"]
    G_PARAMS["DATABASE_VERSION_CORE"] = db_conf["DATABASE_VERSION_CORE"]

    database_create()

if __name__ == "__main__":
    create()