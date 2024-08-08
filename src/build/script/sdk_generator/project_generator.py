#!/usr/bin/env python3
# encoding=utf-8
# =========================================================================
# @brief    Project genarator, parse compiler info & generate IAR/Makefile... project file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# =========================================================================

import copy
import os
import json
import re
import shutil
import sys

script_dir = os.path.split(os.path.normpath(__file__))[0]
makefile_template = os.path.join(script_dir, "sdk_template", "makefile")

def load_json(json_file):
    with open(json_file, 'r') as f:
        temp = f.read()
        return json.loads(temp)

class CompileCommandParser:
    def __init__(self, cc_file):
        temp = load_json(cc_file)
        self.compile_info = {}
        for item in temp:
            self.compile_info[item['file']] = {
                'command': item['command'],
                'directory': item['directory'],
            }

    def get_src_compile_info(self, src_name):
        command = self.compile_info[src_name]['command']
        defines = re.findall(r"-D([^ ]*)", command)
        includes = re.findall(r"-I([^ ]*)", command)

        ccflags = list(set(re.findall(r" (-[^ID][^ ]*)", command)))
        ccflags.remove('-c')
        ccflags.remove('-o')
        pre_include = []
        if '-include' in ccflags:
            ccflags.remove('-include')
            pre_include = list(set(re.findall(r" -include ([^ ]*)", command)))
            ccflags.extend(['-include ' + x for x in pre_include])
        return {
            "DEFINES": list(set(defines)),
            "INCLUDES": list(set(includes)),
            "CCFLAGS": ccflags,
            "PRE_INCLUDE": pre_include,
        }

class ComponentParser(CompileCommandParser):
    def __init__(self, component_info_file, compile_command_file):
        super(ComponentParser, self).__init__(compile_command_file)
        with open(component_info_file, 'r') as fp:
            data = fp.read()
        data = data.replace('="', r'=\\\"')
        data = data.replace('";', r'\\\";')
        data = data.replace(r' \\\";', r' ";')
        info = json.loads(data)
        self.component_info = info.pop("COMPONENTS")
        self.public_info = copy.deepcopy(info)
        self.public_info["COMPONENT_LIST"] = list(self.component_info.keys())
        self.public_info["CHIP"] = chip
        self.public_info["CORE"] = core
        self.public_info["ARCH"] = arch
        self.public_info["TARGET_NAME"] = application_name
        self.public_info["PKG_TARGET_NAME"] = pkg_target_name
        for item, value in self.public_info.items():
            if isinstance(value, str):
                self.public_info[item] = [i for i in value.split(";") if i != '']
        for component, value in self.component_info.items():
            if not value["SOURCES"]:
                continue

            for component_items in value:
                if isinstance(value[component_items], str):
                    value[component_items] = [i for i in value[component_items].split(";") if i != '']
            src = value["SOURCES"]

class MakefileGenerator(ComponentParser):
    def __init__(self, component_info_file, compile_command_file, project_file_dir, project_folder):
        super(MakefileGenerator, self).__init__(component_info_file, compile_command_file)
        self.replace_map = {}
        # self.replace_map.update(toolchain)
        self.project_file_dir = project_file_dir
        self.project_folder = project_folder
        self.make_dir = os.path.join(project_file_dir, project_folder, 'makefile')
        if not os .path.exists(self.make_dir):
            os.makedirs(self.make_dir)
        self.gen_toolchain_file()
        for component_name in self.component_info:
            self.gen_component_file(component_name)
        self.gen_root_file()
        self.gen_makefile_cmd()

    def gen_makefile_cmd(self):
        with open(os.path.join(sdk_output_dir, f'make.cmd'), 'a+') as f:
            f.writelines(self.make_dir + '\n')

    def gen_toolchain_file(self):
        with open(os.path.join(makefile_template, 'toolchain.make'), 'r') as f:
            lines = f.readlines()
        toolchain = load_json(toolchain_file)
        for i, line in enumerate(lines):
            lines[i] = self.deal_replace(line, toolchain)
        with open(os.path.join(self.make_dir, f'toolchains.make'), 'w') as f:
            f.writelines(lines)

    def gen_component_file(self, component_name):
        compile_info = self.component_info[component_name]
        template = os.path.join(makefile_template, 'component_template.make')
        out_path = 'components'
        if "THIRD_PARTY" in compile_info:
            print(compile_info["SOURCES"])
        if "THIRD_PARTY" in compile_info and compile_info["THIRD_PARTY"][0] == "true":
            template = os.path.join(makefile_template, 'third_party_template.make')
            index = compile_info['COMPONENT_CUSTOM_CCFLAGS'].index("-include")
            if index >= 0:
                compile_info['COMPONENT_CUSTOM_CCFLAGS'][index] = "-include$(LITEOS_PLATFORM_MENUCONFIG_H)"
                compile_info['COMPONENT_CUSTOM_CCFLAGS'].pop(index+1)
        compile_info["COMPONENT_NAME"] = component_name
        with open(template, 'r') as f:
            lines = f.readlines()
        for i, line in enumerate(lines):
            lines[i] = self.deal_replace(line, compile_info)
        if not os.path.exists(os.path.join(self.make_dir, out_path)):
            os.makedirs(os.path.join(self.make_dir, out_path))
        with open(os.path.join(self.make_dir, out_path, f'{component_name}.make'), 'w') as f:
            f.writelines(lines)

    def gen_root_file(self):
        with open(os.path.join(makefile_template, 'template.mk'), 'r') as f:
            lines = f.readlines()
        for i, line in enumerate(lines):
            lines[i] = self.deal_replace(line, self.public_info)
        # if not os.path.exists(os.path.join(self.make_dir, 'components')):
        #     os.makedirs(os.path.join(self.make_dir, 'components'))
        with open(os.path.join(self.make_dir, 'Makefile'), 'w') as f:
            f.writelines(lines)

    def join_by_clum_num(self, lst, word, clum_num):
        res = []
        if not lst:
            return res
        c_word = lst[0]
        for item in lst:
            if item == "":
                continue
            if c_word == 0:
                c_word = item
                continue
            if len(c_word) + len(word) + len(item) <= clum_num:
                c_word = f"{c_word}{word}{item}"
                continue
            res.append(c_word)
            c_word = item
        return res

    def deal_replace(self, line, replace_map):
        l = re.findall("REPLACE_(\w*)", line)
        for item in l:
            if isinstance(replace_map[item], list):
                replace_str = " \\\n    ".join(replace_map[item])
                if replace_str:
                    replace_str = " \\\n    %s" % (replace_str.strip())
                replace_str = replace_str.replace(root_dir, "$(SDK_ROOT)")
            elif isinstance(replace_map[item], str):
                replace_str = replace_map[item].replace(root_dir, "$(SDK_ROOT)")
            else:
                print(replace_map[item])
            line = line.replace(f"REPLACE_{item}", replace_str)
        return line

if __name__ == "__main__":
    project_type = sys.argv[1]
    application_name = sys.argv[2]
    cc_json = sys.argv[3]
    project_file_dir = sys.argv[4]
    root_dir = sys.argv[5]
    sdk_output_dir = sys.argv[6]
    chip, core, board, arch, os_kernel, pkg_target_name = sys.argv[7].split(",")
    toolchain_file = sys.argv[8]
    component_info_file = sys.argv[9]
    project_folder = f"{chip}_{core}_{board}_{os_kernel}_{pkg_target_name}"
    project_type_list = project_type.split(",")
    if 'makefile' in project_type_list:
        MakefileGenerator(component_info_file, cc_json, project_file_dir, project_folder)
