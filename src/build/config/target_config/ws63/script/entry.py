#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.

import os
import json
import sys
import platform
import subprocess
import shutil
import re

from utils.build_utils import root_path, exec_shell, compare_bin

from typing import Dict, Any


SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))

def do_cmd(target_name: str, hook_name: str, env: Dict[str, Any])->bool:
    python_path = sys.executable
    print("python path: ", python_path)

    if not os.path.isfile(os.path.join(root_path, "output", "ws63", "acore", "boot_bin", "flashboot.bin")) and target_name != "ws63-flashboot" and hook_name == 'build_pre':
        print("flashboot start build .....")
        errcode = exec_shell([python_path, 'build.py', 'ws63-flashboot'], None, True)
        return True

    if target_name == 'ws63-liteos-mfg' and hook_name == 'build_post':
        errcode = exec_shell([python_path, "build.py", "ws63-liteos-app"], None, True)
        return True

    if hook_name != 'build_post':
        return True

    if env.get('gen_mem_bin'):
        script_path = os.path.join(SCRIPT_DIR, 'get_mem_bin.sh')
        print("gen_mem_bin ing...")
        errcode = exec_shell(['bash', script_path, root_path, target_name, env.get('bin_name')], None, True)
        if errcode != 0:
            print("gen_mem_bin failed!")
            return False
        print("gen_mem_bin done!")

    if env.get('generate_efuse_bin'):
        copy_py = os.path.join(SCRIPT_DIR, 'efuse_cfg_gen.py')
        print("generate_efuse_bin ing...")
        errcode = exec_shell([sys.executable, copy_py], None, True)
        if errcode != 0:
            print("generate_efuse_bin failed!")
            return False
        shutil.copy(os.path.join(root_path, 'output/ws63/acore/ws63-liteos-app/efuse_cfg.bin'), os.path.join(root_path, 'output/ws63/acore/boot_bin'))
        print("generate_efuse_bin done!")

    if env.get('copy_files_to_interim'):
        # copy_files_to_interim
        copy_py = os.path.join(SCRIPT_DIR, 'copy_files_to_interim.py')
        print("copy_files_to_interim ing...")
        errcode = exec_shell([sys.executable, copy_py, root_path], None, True)
        if errcode != 0:
            print("copy_files_to_interim failed!")
            return False
        print("copy_files_to_interim done!")

    if env.get('pke_rom_bin'):
        # gen pke_rom_bin
        gen_pke_rom_bin_sh = os.path.join(SCRIPT_DIR, 'pke_rom.sh')
        if os.path.exists(os.path.join(root_path, \
            'drivers/chips/ws63/rom/rom_boot/drivers/drivers/hal/security_unified/hal_cipher/pke/rom_lib.c')):
            print("generate pke_rom_bin ing...")
            errcode = exec_shell(['sh', gen_pke_rom_bin_sh, root_path], None, True)
            if errcode != 0:
                print("generate pke_rom_bin failed!")
                return False
            print("generate pke_rom_bin done!")

            # verify pke rom bin
            if env.get('fixed_pke'):
                bin1 = os.path.join(root_path, "output", env.get('chip'), env.get('core'), 'pke_rom', 'pke_rom.bin')
                bin2 = env.get('fixed_pke_path', '').replace('<root>', root_path)
                if not compare_bin(bin1, bin2):
                    print(f"Verify pke rom bin ERROR! :{bin1} is not same with {bin2}")
                    return False

    if env.get('rom_in_one'):
        # gen rom_in_one
        if "windows" in platform.platform().lower():
            rom_in_one = os.path.join(SCRIPT_DIR, 'rom_in_one.py')
            print("generate rom_in_one ing...")
            errcode = exec_shell([python_path, rom_in_one, root_path, env.get('bin_name')], None, True)
        else:
            rom_in_one = os.path.join(SCRIPT_DIR, 'rom_in_one.sh')
            print("generate rom_in_one ing...")
            errcode = exec_shell(['sh', rom_in_one, root_path, env.get('bin_name')], None, True)

        if errcode != 0:
            print("generate rom_in_one failed!")
            return False
        print("generate rom_in_one done!")

        # verify codepoint bin
        bin1 = os.path.join(root_path, "output", env.get('chip'), env.get('core'), \
                target_name, env.get('bin_name')+'_rompack.bin')
        if env.get('fixed_rom_in_one') and os.path.isfile(bin1):# only rompack bin exists
            bin2 = env.get('fixed_rom_in_one_path', '').replace('<root>', root_path)
            if not compare_bin(bin1, bin2):
                print(f"Verify rom_in_one bin ERROR! :{bin1} is not same with {bin2}")
                return False

    if env.get('fixed_bin_name'):
        bin1 = os.path.join(root_path, "output", env.get('chip'), env.get('core'), \
            target_name, env.get('fixed_bin_name'))
        bin2 = env.get('fixed_bin_path', '').replace('<root>', root_path)
        if not compare_bin(bin1, bin2):
            print(f"Verify bin ERROR! :{bin1} is not same with {bin2}")
            return False
    nv_handle = os.path.join(SCRIPT_DIR, 'nv_handle.py')
    exec_shell([python_path, nv_handle], None, True)
    return True


def config_check(root_path, target_name):
    config_file_path = os.path.join(root_path, "build", "config", "target_config", "ws63", "config.py")
    with open(config_file_path, "r", encoding="utf-8") as f:
        for i in f:
            if target_name in i:
                print(target_name, " in config.py.")
                return True
    return False