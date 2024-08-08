#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.

import json
import os
import sys
import time
import re

from utils.build_utils import root_path, script_path, target_config_path, output_root
from utils.build_utils import pkg_tools_path, jlink_tools_path, lzma_tools_path, sign_tools_path, derived_tools_path
from utils.build_utils import CopyModule, exec_shell, cmp_file, rm_pyc, rm_all, fn_get_subdirs
from enviroment import TargetEnvironment
from sdk_generator.target_config_genarator import genarate_reserve_config

sdk_copy_common_files = [
    os.path.join(script_path),
    os.path.join(root_path, 'build.py'),
    os.path.join(target_config_path),
    os.path.join(pkg_tools_path),
    os.path.join(jlink_tools_path),
    os.path.join(lzma_tools_path),
    os.path.join(sign_tools_path),
    os.path.join(derived_tools_path),
]

sdk_close_components = [
]

def compare_path_bin(path1, path2):
    for file in os.listdir(path1):
        if not file.endswith('.bin'):
            continue
        if file.endswith('sign.bin'):
            continue
        f1 = os.path.join(path1, file)
        f2 = os.path.join(path2, file)
        print("Comparing:")
        print(f1)
        print(f2)
        if not cmp_file(f1, f2):
            print("DIFF")
            return False
        print("SAME")
    return True


class SdkGenerator:
    def __init__(self, env: TargetEnvironment, sdk_root_path: str):
        self.env = env
        self.sdk_root_path = sdk_root_path
        replace_suffix = ['.srcrelease']
        if self.env.get('replace_suffix', False):
            replace_suffix = self.env.get('replace_suffix', False)
        print(replace_suffix)
        self.copy_module = CopyModule(replace_root=sdk_root_path, replace_suffix=replace_suffix, copy_header = False)
        self.org_target_output_path = []
        self.rm_line_map = {}
        self.sub_cmake = {}
        self.all_cmake = {}
        chip = self.env.get("chip", False)
        if not isinstance(chip, list):
            self.chip = [chip]
        else:
            self.chip = chip
        self.sdk_copy_module_mask_add()
        self.copy_module.append_mask('.srcrelease')

    def parse_depend_cmake_files(self, cmake_trace_file):
        with open(cmake_trace_file, 'r') as fp:
            lines = fp.readlines()

        sdk_cmake_dict = {}
        depend_cmake_files = []

        for line in lines:
            dict_ = json.loads(line)
            if 'file' not in dict_:
                continue
            f = dict_['file']
            if not f.startswith(root_path) or f.startswith(output_root):
                continue
            if 'cmd' in dict_ and dict_['cmd'] == 'add_subdirectory':
                if f not in self.sub_cmake:
                    self.sub_cmake[f] = set()
                file_path = os.path.dirname(f)
                if "$" in dict_['args'][0]:
                    continue
                depend_cmake = os.path.join(file_path, dict_['args'][0], "CMakeLists.txt")
                self.sub_cmake[f].add((int(dict_['line']), depend_cmake))
            if f in sdk_cmake_dict:
                continue
            sdk_cmake_dict[f] = None
            self.all_cmake[f] = None
            tmp = f.replace(root_path,  self.sdk_root_path)
            if self.is_close_component(tmp) == False:
                if tmp.endswith("CMakeLists.txt"):
                    if os.path.exists(os.path.dirname(tmp)):
                        depend_cmake_files.append(f)
                else:
                    depend_cmake_files.append(f)
        return depend_cmake_files

    def copy_menuconfig(self):
        for chip in self.chip:
            if os.path.exists(os.path.join(root_path, 'build', 'config', 'target_config', chip, 'menuconfig')):
                self.copy_srcs([os.path.join(root_path, 'build', 'config', 'target_config', chip, 'menuconfig')])
        self.copy_srcs([os.path.join(root_path, 'config.in')])

    def copy_kconfig(self, cmake_trace_file):
        depend_cmake_files = self.parse_depend_cmake_files(cmake_trace_file)
        kconfig_fils = []
        for f in depend_cmake_files:
            tmp = f.replace("CMakeLists.txt", 'Kconfig')
            if os.path.exists(tmp):
                kconfig_fils.append(tmp)
        self.copy_srcs(kconfig_fils)

    def copy_depends(self, cmake_trace_file):
        depend_cmake_files = self.parse_depend_cmake_files(cmake_trace_file)
        cmake_dest = self.copy_srcs(depend_cmake_files)
        for path in sdk_copy_common_files:
            if not os.path.exists(path):
                sdk_copy_common_files.remove(path)
        self.copy_srcs(sdk_copy_common_files)
        if self.env.get('use_memuconfig') != False:
            self.copy_menuconfig()
            self.copy_kconfig(cmake_trace_file)
        if self.env.get('auto_gen_config'):
            self.genarate_sdk_target_config(self.env.get('pkg_target_name', False))

    def genarate_efuse_cfg_bin(self, chip_name):
        if self.env.get('generate_efuse_bin') == True and chip_name in ["ws63", "ws53"]:
            print("source build efuse_cfg.bin...")
            exec_shell([sys.executable, os.path.join(target_config_path, chip_name, "script", "efuse_cfg_gen.py")])

    def genarate_sdk_target_config(self, targets):
        reserve = {}
        for target in targets:
            env = TargetEnvironment(target)
            chip = env.get('chip')
            if chip not in reserve:
                reserve[chip] = {'target': [], 'template': []}
            reserve[chip]['target'].append(target)
            reserve[chip]['template'].append(env.get_target_template())

        print(reserve)
        for chip in reserve:
            path = os.path.join(self.sdk_root_path, 'build', 'config', 'target_config', chip)
            config_path = os.path.join(path, 'config.py')
            template_path = os.path.join(path, 'target_config.py')
            genarate_reserve_config(reserve[chip]['target'], config_path)
            genarate_reserve_config(reserve[chip]['template'], template_path)


    def rm_lines_in_file(self, _file, lines):
        with open(_file, 'r') as fp_read:
            text = fp_read.readlines()
            lines = sorted(list(set(lines)), reverse=True)
            for idx in lines:
                text.pop(idx - 1)

        with open(_file, 'w') as fp_write:
            fp_write.write("".join(text))

    def copy_srcs(self, file_list):
        dest_src = []
        for file in file_list:
            if not os.path.exists(file):
                print("SDK GENERATE ERROR!!")
                print("FILE: %s is not exists!!" % file)
                raise
            dest = self.copy_module.copy(file)
            if dest is not None:
                dest_src.append(dest)
        return dest_src

    def register_org_target_path(self, path):
        self.org_target_output_path.append(path)

    def rm_cmake_lines(self):
        rm_lines = {}
        for f, line_dep in self.sub_cmake.items():
            for line, dep_cmake in line_dep:
                if dep_cmake in self.all_cmake:
                    continue
                if f not in rm_lines:
                    rm_lines[f] = []
                rm_lines[f].append(line)
        for f, lines in rm_lines.items():
            sdk_file = f.replace(root_path, self.sdk_root_path)
            self.rm_lines_in_file(sdk_file, lines)

    def sdk_copy_module_mask_add(self):
        chip_mask = []
        path_list = [target_config_path]
        for path_mask in path_list:
            for name in fn_get_subdirs(path_mask):
                if name not in self.chip and name not in chip_mask:
                    chip_mask.append(name)
        self.copy_module.append_mask(chip_mask)

    def is_closed_component(self, component_name):
        closed_components = self.env.get('closed_components', cmake_type=False)
        open_components = self.env.get('open_components', cmake_type=False)
        if isinstance(closed_components, list) and component_name in closed_components:
            return True
        elif isinstance(open_components, list) and component_name not in open_components:
            return True
        return False

    def sdk_delete_tmp_files(self):
        delete_files = []
        delete_files.append(os.path.join(output_root, 'sdk', 'output'))
        delete_files.append(os.path.join(output_root, 'sdk', 'make.cmd'))
        for dir_path, dir_names, file_names in os.walk(os.path.join(output_root, 'sdk', 'interim_binary', self.env.get('chip'), 'libs'), topdown=False):
            for name in file_names:
                if name.endswith('.a'):
                    if not self.is_closed_component(name[3:-2]):
                        delete_files.append(os.path.join(dir_path, name))
        for dir_path, dir_names, file_names in os.walk(os.path.join(output_root, 'sdk', 'build', 'config', 'target_config', self.env.get('chip')), topdown=False):
            for name in file_names:
                if name.endswith('.srcrelease'):
                    delete_files.append(os.path.join(dir_path, name))
        rm_all(delete_files)
        rm_pyc(os.path.join(output_root, 'sdk', 'build'))
        rm_pyc(os.path.join(output_root, 'sdk', 'tools', 'pkg'))

    def sdk_build(self, build_time, nhso, build_level):
        org_pwd = os.getcwd()
        os.chdir(self.sdk_root_path)

        self.close_sdk_mem_limit()
        self.change_sdk_version()

        build_targets = self.env.get('pkg_target_name', cmake_type=False)
        print(build_targets)
        sdk_type_list = self.env.get('sdk_type').split(';')
        for idx, target in enumerate(build_targets):
            sdk_build_cmd = ['./build.py', target]
            if build_time != '':
                sdk_build_cmd.append("-build_time=%s" %build_time)
            if nhso == True:
                sdk_build_cmd.append("-nhso")
            if build_level == 'release':
                sdk_build_cmd.append("-release")

            ret_code = exec_shell(sdk_build_cmd)
            if ret_code:
                sys.exit(1)
            org_output_path = self.org_target_output_path[idx]
            sdk_output_path = org_output_path.replace(root_path, self.sdk_root_path)
            if not compare_path_bin(sdk_output_path, org_output_path):
                print("sdk build failed")
                sys.exit(1)
        os.chdir(org_pwd)
        if "makefile" in sdk_type_list:
            self.sdk_makefile_build(nhso, build_level)
        self.sdk_delete_tmp_files()

    def close_sdk_mem_limit(self):
        if "SDK_NOT_MEM_LIMIT" not in self.env.config.get("defines", []):
            return
        chip_config_path = os.path.join('build', 'config', 'target_config', self.env.get('chip'), 'target_config.py')
        with open(chip_config_path, "r") as f:
            config_content = f.read()
        print("new defines: SDK_NOT_MEM_LIMI, do not limit the memory size of the target in sdk")
        with open(chip_config_path, "w") as f:
            config_content = re.sub(r"'defines'\s?:\s?\[", r"'defines': [" + "'SDK_NOT_MEM_LIMIT', ", config_content)
            f.write(config_content)
        return

    def change_sdk_version(self):
        sdk_version_cmd_define = ""
        for build_def in self.env.config.get("defines", []):
            if "SDK_VERSION=" not in build_def:
                continue
            sdk_version_cmd_define = build_def
            break
        if not sdk_version_cmd_define:
            return
        chip_config_path = os.path.join('build', 'config', 'target_config', self.env.get('chip'), 'target_config.py')
        with open(chip_config_path, "r") as f:
            config_content = f.read()
            print(config_content)
        print(f"change defines: {sdk_version_cmd_define}, change the version of sdk")
        with open(chip_config_path, "w") as f:
            sdk_version_cmd_define = sdk_version_cmd_define.replace("\"", "\\\"")
            config_content = re.sub("SDK_VERSION" + r'''=.*?",''', sdk_version_cmd_define + '''",''', config_content)
            print(config_content)
            f.write(config_content)
        return

    def is_close_component(self, path):
        for f in sdk_close_components:
            tmp = f.replace(root_path,  self.sdk_root_path)
            if path.startswith(tmp):
                return True
        return False

    def sdk_makefile_build(self, nhso, build_level):
        with open(os.path.join(self.sdk_root_path, 'make.cmd'), 'r') as fp:
            lines = fp.readlines()
        idx = 0
        for line in lines:
            org_pwd = os.getcwd()
            start_time = time.time()
            line = line.replace('\n', '')
            os.chdir(line)
            sdk_build_cmd = ['make', '-j48']
            if nhso == True:
                sdk_build_cmd.append("nhso=true")
            if build_level == 'release':
                sdk_build_cmd.append("build_level=release")
            ret_code = exec_shell(sdk_build_cmd)
            if ret_code:
                sys.exit(1)
            end_time = time.time()
            print("build %s takes %f s" %  (line, end_time - start_time))
            org_output_path = self.org_target_output_path[idx]
            sdk_output_path_makefile = "%s-makefile" % org_output_path.replace(root_path, self.sdk_root_path)
            if not compare_path_bin(sdk_output_path_makefile, org_output_path):
                print("sdk build failed")
                sys.exit(1)
            os.chdir(org_pwd)
            idx = idx + 1
