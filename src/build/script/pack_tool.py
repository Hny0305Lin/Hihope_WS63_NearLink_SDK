#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.

import os
import sys
import shutil
from utils.build_utils import CopyModule, output_root, root_path
from enviroment import TargetEnvironment, BuildEnvironment, chip_copy_target

def find_copy_target(target_name):
    for chip in chip_copy_target:
        if target_name in chip_copy_target[chip]:
            print(target_name, chip)
            return chip
    return None

class packTool:
    def __init__(self, pack_name, target_name):
        self.pack_name = pack_name
        self.target_name = target_name
        self.pack_suffix = ['bin', 'elf', 'lst', 'asm', 'nm', 'map', 'mem', 'hex', 'info', 'fwpkg', 'ko']
        self.pack_dir = ['parse_tool']
        self.pack_bin_suffix = ['bin']

    def __replace_copy_target_path(self, path):
        path = path.replace("<root>", root_path)
        path = path.replace("<out_root>", output_root)
        path = path.replace("<pack_target>", self.pack_name)
        return path

    def __pack_copy_target__(self, chip, copy_target):
        for item in copy_target:
            src = self.__replace_copy_target_path(item['src'])
            dst = self.__replace_copy_target_path(item['dst'])
            if not dst and os.path.exists(src):
                print("remove %s" % src)
                if os.path.isfile(src):
                    os.remove(src)
                elif os.path.isdir(src):
                    shutil.rmtree(src)
                continue
            print("copy %s -> %s" % (src, dst))
            c = CopyModule(copy_header = False)
            c.copy(src, dst)

    def pack(self):
        chip = find_copy_target(self.target_name)
        if chip is not None:
            self.__pack_copy_target__(chip, chip_copy_target[chip][self.target_name])
            return
        target_env = TargetEnvironment(self.target_name)
        chip = target_env.get('chip')
        output_path = target_env.get_output_path()
        pack_path = os.path.join(output_root, 'package', chip, self.pack_name)
        pack_target_path = os.path.join(pack_path, self.target_name)
        pack_bin_path = pack_path
        print("Start pack target %s" % self.target_name)
        if not os.path.exists(pack_target_path):
            os.makedirs(pack_target_path)
        for file_name in os.listdir(output_path):
            ext_name = os.path.splitext(file_name)[-1][1:]
            if ext_name not in self.pack_suffix:
                continue
            src = os.path.join(output_path, file_name)
            dst = os.path.join(pack_target_path, file_name)
            shutil.copy(src, dst)
            print("copy %s -> %s" % (src, dst))
            if ext_name not in self.pack_bin_suffix:
                continue
            bin_dst = os.path.join(pack_bin_path, file_name)
            print("copy %s -> %s" % (src, bin_dst))
            shutil.copy(src, bin_dst)

        for file_name in os.listdir(output_path):
            if file_name not in self.pack_dir:
                continue
            src = os.path.join(output_path, file_name)
            dst = os.path.join(pack_target_path, file_name)
            shutil.copytree(src, dst)
            print("copytree %s -> %s" % (src, dst))