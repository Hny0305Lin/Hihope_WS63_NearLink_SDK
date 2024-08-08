#!/usr/bin/env python
# coding:utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.

import sys,string,re,os
from ctypes import *

import auto_class as auto_class
from parse_basic import *
import json

def _process_g_var(list,result_fp):
    name=list[0].strip()
    addr=int(list[1],16)
    size=list[2]
    type_name=list[3].strip()

    if hasattr(auto_class, type_name):
        class_obj=getattr(auto_class, type_name)
        if class_obj==None:
            return
        #if sizeof(class_obj)!=size:
        #    return
        obj = parse_memory_2_class(addr,class_obj,sizeof(class_obj))
        if(obj):
            uapi_print_ctypes_obj(name,obj,fd=result_fp)
            print('',file=result_fp)

def _process_g_array_item(name,addr,class_obj,result_fp):
    pass

def _process_g_array(list,result_fp):
    name=list[0].strip()
    addr=int(list[1],16)
    size=list[2]
    type_name=list[3].strip()
    str=list[4].strip()
    array_list=json.loads(str)

    if hasattr(auto_class, type_name):
        class_obj=getattr(auto_class, type_name)
        if class_obj==None:
            return
        for n in array_list[::-1]:
            class_obj=class_obj*n
        if sizeof(class_obj)!=size:
            return
        obj = parse_memory_2_class(addr,class_obj,sizeof(class_obj))
        if(obj):
            uapi_print_ctypes_obj(name,obj,fd=result_fp)
            print('',file=result_fp)

def _process_lines(lines,result_fp):
    print("----------------------content of 全局变量:----------------------",file=result_fp)
    for line in lines:
        line=line.strip()
        list=line.split('|')
        if len(list)==4:
            _process_g_var(list,result_fp)
        elif len(list)==5:
            _process_g_array(list,result_fp)

def parse_print_global_var(var_file,result_fp):
    with open(var_file,'r') as fp:
        lines=fp.readlines()
        _process_lines(lines,result_fp)
    print("parse_print_global_var end!")


