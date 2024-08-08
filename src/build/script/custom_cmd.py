#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.

import os
import sys
import json
import importlib
import copy
import traceback

from enviroment import TargetEnvironment
from utils.build_utils import target_config_path, exec_shell

def run_custom_cmd(env: TargetEnvironment, target_name: str, hook_name: str)->bool:
    chip = env.get('chip')
    core = env.get('core')
    if chip not in ('ws63', 'ws53'):# only ws63, ws53
        return True
    script_entry_path = os.path.join(target_config_path, chip, 'script', 'entry.py')
    if os.path.isfile(script_entry_path):
        try:
            sys.path.insert(0, os.path.dirname(script_entry_path))
            custom_module = importlib.import_module('entry')
            importlib.reload(custom_module)
            sys.path.pop(0)
            if hasattr(custom_module, 'do_cmd'):
                if not custom_module.do_cmd(target_name, hook_name, copy.deepcopy(env.config)):
                    print(f"[{chip}][{core}] run custom cmd failed!")
                    return False
                print(f"[{chip}][{core}] run custom cmd success!")
        except:
            traceback.print_exc()
            print(f"[{chip}][{core}] run custom cmd failed!")
            return False
        
    return True