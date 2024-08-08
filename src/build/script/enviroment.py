#!/usr/bin/env python3
# encoding=utf-8
# =========================================================================
# @brief    Config Target Definitions File
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# =========================================================================
import multiprocessing
import os
import sys
import json
import copy
import importlib
from utils.build_utils import fn_get_subdirs, rm_pyc, target_config_path

all_target = {}
all_group = {}
all_target_templates = {}
chip_group_target = {}
chip_copy_target = {}

from target_config.common_config import CommonConfig
from utils.build_utils import exec_shell, root_path, get_platform_name, output_root

rm_pyc(target_config_path)
for name in fn_get_subdirs(target_config_path):
    load_fmt = "target_config.%s.target_config" %name
    try:
        load_mod = importlib.import_module(load_fmt)
    except:
        print("[WARN] config/target_config/%s/target_config.py is not exist, you can delete it" % name)
        continue
    all_target_templates.update(load_mod.target_template)

    load_fmt = "target_config.%s.config" %name
    load_mod = importlib.import_module(load_fmt)
    if "target_copy" in load_mod.__dict__:
        chip_copy_target[name] = load_mod.target_copy
    all_target.update(load_mod.target)
    all_group.update(load_mod.target_group)
    chip_group_target[name] = list(load_mod.target.keys()) + list(load_mod.target_group.keys())

# 判断key_list中的关键字是否都在string中
def is_list_in_str(key_list, string):
    for item in key_list:
        if item not in string:
            return False
    return True


class TargetEnvironment:
    def __init__(self, target_name, extra_defines=None):

        self.config = {}
        self.is_implement_target = False
        if extra_defines is None:
            extra_defines = []
        org_target = copy.deepcopy(all_target[target_name])
        # sdk target不需要和template合并
        if org_target.get('build_type', None) == 'SDK':
            self.config = copy.deepcopy(org_target)
            if extra_defines:
                self.config['defines'] = extra_defines
            return

        self.merge_target_config(target_name, extra_defines)

        com_config = CommonConfig(self.config['arch'])

        self.merge_common_config(com_config)
        self.merge_component_set(com_config)
        self.merge_defines_set(com_config)

        if self.is_enable_hso():
            self.config["defines"].append("HSO_SUPPORT")

        for key, value in self.config.items():
            if isinstance(value, list):
                self.deal_with_delete_symbol(key)

        self.config["build_platform"] = get_platform_name()
        self.config['target_command'] = target_name
        if org_target.get('product_type'):
            self.config['product_type'] = org_target.get('product_type')

    def merge_target_config(self, target_name, extra_defines):
        """ 将target_name对应的diff配置同diff配置中base_target_name对应的template配置合并
            Args:
                target_name: diff配置的key
                extra_defines: 从命令-def=XXX,YYY,ZZZ=x中接收的额外宏
        """
        org_target = copy.deepcopy(all_target[target_name])
        base_target_name = org_target.pop('base_target_name')
        if base_target_name in all_target:
            temp_target = copy.deepcopy(all_target[base_target_name])
            self.is_implement_target = True
            org_target = self.merge_dict(temp_target, org_target)
            base_target_name = temp_target['base_target_name']

        self.config = copy.deepcopy(all_target_templates[base_target_name])

        for key, value in org_target.items():
            if not isinstance(value, list):
                self.config[key] = value
                continue
            if key == 'defines':
                self.merge_defines(value)
                continue
            self.extend(key, value)
        self.merge_defines(extra_defines)

    def merge_dict(self, dict1, dict2):
        res = copy.deepcopy(dict2)
        for key, value in dict1.items():
            if not isinstance(value, list):
                res[key] = value
                continue
            if key not in res:
                res[key] = []
            res[key].extend(value)
        return res

    def merge_common_config(self, com_config):
        """ 将公共配置项合并到config中
            Args:
                com_config: 公共配置实例
        """
        self.pretend('ccflags', com_config.get_ram_ccflags())
        self.extend('rom_ccflags', com_config.get_rom_ccflags())
        self.extend('linkflags', com_config.get_linkflags())
        self.extend('std_libs', com_config.get_std_libs())
        self.add('arch_family', com_config.get_arch_family())

    def merge_defines(self, defines, extend=True):
        """
        将参数defines合并到config的defines项中, 对于XXX=y这种宏会被替换
            例：
                config的defines项中有:XXX=y
                参数defines中有:XXX=z
                则合并后config的defines项中:XXX=y => XXX=z
        """
        if extend:
            self.extend('defines', defines)
        special_define = {}
        for define in defines:
            if '=' in define:
                key = define.split('=')[0]
                val = define.split('=')[1]
                special_define[key] = val
        for idx, define in enumerate(self.config['defines']):
            if '=' not in define:
                continue
            key = define.split('=')[0]
            if key in special_define:
                self.config['defines'][idx] = '%s=%s' % (key, special_define[key])
        if extend:
            self.config['defines'].extend(defines)
        self.config['defines'] = list({}.fromkeys(self.config['defines']).keys())

    def merge_component_set(self, com_config):
        """
        处理ram/rom_component_set项中的删除标识, 并将其value合并到ram/rom_component中
        """
        self.deal_with_delete_symbol('ram_component_set')
        self.deal_with_delete_symbol('rom_component_set')

        for ram_set in self.config['ram_component_set']:
            self.extend('ram_component', com_config.get_component_set(ram_set))

        for rom_set in self.config['rom_component_set']:
            self.extend('rom_component', com_config.get_component_set(rom_set))

    def merge_defines_set(self, com_config):
        """
        处理ram/rom_component_set项中的删除标识, 并将其value合并到ram/rom_component中
        """
        self.deal_with_delete_symbol('defines_set')

        for defines_set in self.config['defines_set']:
            self.extend('defines', com_config.get_definse(defines_set))

    def deal_with_delete_symbol(self, key):
        """
        处理self.config中key项中的删除标识
        """
        
        if key not in self.config:
            self.config[key] = []
            return
        lst = self.config[key]
        normal_lst = []
        reserve_lst = []
        delete_lst = []
        define_replace = []
        for item in lst:
            if item.startswith("[r]"):
                reserve_lst.append(item[3:])
                continue
            if not item.startswith("-:"):
                normal_lst.append(item)
                continue
            if key == 'defines' and "=" in item:
                define_replace.append(item[2:])
                continue
            delete_lst.append(item[2:])

        normal_lst = list({}.fromkeys(normal_lst).keys())
        for del_item in delete_lst:
            if del_item in normal_lst:
                normal_lst.remove(del_item)
        self.config[key] = normal_lst
        self.merge_defines(define_replace, False)
        self.config[key].extend(reserve_lst)

    def add_component_defines(self):
        self.config["target_component_defines"] = []
        if 'ram_component' in self.config:
            self.config["target_component_defines"].extend(
                ["SUPPORT_" + x.upper() for x in self.config['ram_component']]
            )
        if 'rom_component' in self.config:
            self.config["target_component_defines"].extend(
                ["SUPPORT_" + x.upper() for x in self.config['rom_component']]
            )
    def is_enable_hso(self):
        return self.get("hso_enable")

    def get_tool_chain(self):
        toolchain_dir = os.path.join(root_path, 'build', 'toolchains')
        toolchain_file = "%s.cmake" % (self.get('tool_chain'))
        return os.path.join(toolchain_dir, toolchain_file)

    def get(self, key, cmake_type=True):
        """
        返回self.config中item项的值, 
        若此值为list类型且cmake_type为True则用";"连接为字符串再返回（cmake中列表中元素用;隔开）
        """
        if key not in self.config:
            return None
        if isinstance(self.config[key], list) and cmake_type:
            return ";".join(self.config[key])
        return self.config[key]

    def set(self, key, val):
        if key not in self.config:
            raise f'{key} not in ' + self.config['target_command']
        if not isinstance(self.config[key], type(val)):
            raise 'type of value is not same with ' + self.config['target_command'] + 'config'
        self.config[key] = val

    def get_output_path(self):
        chip = self.get('chip')
        core = self.get('core')
        target_name = self.get('target_command')
        return os.path.join(output_root, chip, core, target_name)
    
    def get_target_template(self):
        if self.is_implement_target:
            temp = all_target[self.config['target_command']]['base_target_name']
            res = all_target[temp]['base_target_name']
        else:
            res = all_target[self.config['target_command']]['base_target_name']
        return res

    def add(self, key, value):
        if key in self.config:
            raise
        self.config[key] = value

    def extend(self, key, value):
        if not isinstance(value, list):
            return
        if key not in self.config:
            self.config[key] = []
        self.config[key].extend(value)
        
    def pretend(self, key, value):
        if not isinstance(value, list):
            return
        if key not in self.config:
            self.config[key] = []
        value.extend(self.config[key])
        self.config[key] = copy.deepcopy(value)
    
    def append(self, key, value):
        if key not in self.config:
            self.config[key] = []
        if not isinstance(self.config[key], list):
            return
        if value in self.config[key]:
            return
        self.config[key].append(value)

    def remove(self, key, value):
        if key not in self.config:
            return
        if not isinstance(self.config[key], list):
            return
        if value not in self.config[key]:
            return
        self.config[key].remove(value)

    def dump(self, dump_file=False):
        info = json.dumps(self.config, indent=4, ensure_ascii=False)
        if not dump_file:
            print(info)
            return info
        with open(os.path.join(self.get_output_path(), "env_config.json"), "w") as f:
            f.write(info)
        return info

    def is_config_refresh(self):
        config_file = os.path.join(self.get_output_path(), "env_config.json")
        if not os.path.exists(config_file):
            return True
        with open(config_file, "r") as f:
            config = json.load(f)
        return config != self.config

class BuildEnvironment:
    """ command line param handler
        receive param and parse it
    """

    def __init__(self, param_list):
        self.python_path = sys.executable
        param_list = param_list[1:]
        self.extr_defines = []
        self.thread = multiprocessing.cpu_count()
        self.need_clean = False
        self.no_symbol_link = False
        self.component = []
        self.build_level = 'normal'
        self.build_as_lib = False
        self.build_as_lib_output_file = ''
        self.target_names = []
        self.group_names = []
        self.generator = 'Unix Makefiles'
        self.no_hso = False
        self.open_kconfig = False
        if get_platform_name() == "windows":
            self.generator = "Ninja"
        self.dump = False
        self.build_time = ''
        self.parse_cmd(param_list)

    def parse_cmd(self, param_list):
        """ parse param
        """
        keys_of_target_name = []
        for param in param_list:
            if param.startswith('-j'):
                self.thread = int(param[2:])
            elif param.startswith('-def='):
                self.extr_defines.extend(param[5:].split(','))
            elif param.startswith('-build_time='):
                self.build_time = param[12:].split(',')[0]
            elif param.startswith('-component='):
                self.component.extend(param[11:].split(','))
            elif param.startswith('-out_libs='):
                self.build_as_lib = True
                self.build_as_lib_output_file = param[10:]
            elif param == '-c':
                self.need_clean = True
            elif param == '-ninja':
                self.generator = 'Ninja'
            elif param == '-release' or param == '-debug' or param == '-normal':
                self.build_level = param[1:]
            elif param == '-dump':
                self.dump = True
            elif param == '-nhso':
                self.no_hso = True
            elif param == '-nsymlink':
                self.no_symbol_link = True
            elif param == 'menuconfig' or param == 'defconfig' or param == 'allyesconfig' or param == 'allnoconfig':
                self.open_kconfig = True
                self.kconfig_param = param
            else:
                keys_of_target_name.append(param)
        self.match_target_names(keys_of_target_name)

    def get_target_names_by_group_name(self, group_name):
        targets = []
        items = all_group[group_name]
        return items

    def is_group(self, item):
        return item in all_group

    def is_target(self, item):
        return item in all_target

    def is_copy_target(self, item):
        for chip in chip_copy_target:
            if item in chip_copy_target[chip]:
                return True
        return False

    def get_chip_name(self, item):
        for chip, targets in chip_group_target.items():
            if item in targets:
                return chip
        return None

    def add_target_names(self, target_name):
        if target_name in all_group:
            self.group_names.append(target_name)
        if target_name in all_target:
            self.target_names.append(target_name)

    def match_target_names(self, keys_of_target_name):
        """ 根据匹配关键字在all_target和all_group中匹配target_name以供选择
        """
        if len(keys_of_target_name) == 1:
            temp_name = keys_of_target_name[0]
            if temp_name in all_group or temp_name in all_target:
                self.add_target_names(temp_name)
                return

        normal_match_list = []
        group_match_list = []
        for key in all_target:
            if is_list_in_str(keys_of_target_name, key):
                normal_match_list.append(key)
        for key in all_group:
            if is_list_in_str(keys_of_target_name, key):
                group_match_list.append(key)

        all_match_list = normal_match_list + group_match_list
        # all_match_list为空, 报错
        if not all_match_list:
            print("Target_name invalid and No matching target_name exists!")
            for item in all_target.keys():
                print(item)
            sys.exit(1)
        # 匹配项只有一个, 无需选择直接返回
        if len(all_match_list) == 1:
            self.add_target_names(all_match_list[0])
            return
        # 按序号打印匹配的target_name
        print("Target_name invalid")
        print("Here are the matching target_names")
        cur_index = 0
        for index in range(len(normal_match_list)):
            print("%-3d : %s" % (cur_index, normal_match_list[index]))
            cur_index += 1
        print("\nHere are the matching aliases target_name")
        for index in range(len(group_match_list)):
            print("%-3d : %s" % (cur_index, group_match_list[index]))
            cur_index += 1
        print("\nDo you want to build them?")
        print("Input index numbers for build or q for quit(num/q):", end='')

        # 输入序号或q/Q
        select_index = input()
        if select_index.lower() == 'q':
            sys.exit(0)
        try:
            select_indexes = [int(x) for x in select_index.split()]
            for index in select_indexes:
                self.add_target_names(all_match_list[index])
        except:
            print("ERROR INPUT!!")
            sys.exit(1)
