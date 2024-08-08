#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    build system entry, receive param & start to build
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# ============================================================================
"""
接收参数列表及解释:
    -c:
        clean后编译
    -j:
        -j<num>, 以num线程数编译
        默认为机器最大线程数
    -def=:
        -def=XXX,YYY,ZZZ=x,...  向本次编译target中添加XXX、YYY、ZZZ=x编译宏
        可使用-def=-:XXX 来屏蔽XXX宏
        可使用-def=-:ZZZ=x 来添加或者修改ZZZ宏
    -component=:
        -component=XXX,YYY,...  仅编译XXX,YYY组件
    -ninja:
        使用ninja生成中间文件
        默认使用Unix makefile
    -[release/debug]:
        debug:     在生成反汇编文件时信息更加全面但也更耗时
        release:   在生成反汇编文件时节省时间
        默认为debug
    -dump:
        输出target的编译参数
    -nhso:
        不更新HSO数据库
    -out_libs:
        -out_libs=file_path, 不再链接成elf, 转而将所有.a打包成一个大的.a
    others:
        作为匹配编译target_names的关键字
"""
import os
import sys
from distutils.spawn import find_executable

sys.dont_write_bytecode = True
root_dir = os.path.split(os.path.realpath(__file__))[0]
sys.path.append(os.path.join(root_dir, 'build', 'config'))
sys.path.append(os.path.join(root_dir, 'build', 'script'))

from cmake_builder import CMakeBuilder

def check_enviroment():
    if not find_executable("cmake"):
        print("cmake is not installed or not added to system path.")
    if not find_executable("ninja") and not find_executable("make"):
        print("make/ninja is not installed or not added to system path.")

builder = CMakeBuilder(sys.argv)

builder.build()
