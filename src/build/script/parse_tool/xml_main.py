#!/usr/bin/env python
# coding:utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.

import sys,string,re,os
from ctypes import *
import json
from xml.etree import ElementTree  as  ET


def _xml_parse_tree(file):
    tree = ET.parse(file)
    root = tree.getroot()
    return (tree,root)


def _xml_find_son_by_tag(parent,tag_name):
    match_list=[]
    for child in parent:
        if(child.tag==tag_name):
            match_list.append(child)
    return match_list


def _xml_find_child_by_tag(match_list,parent,tag_name):
    for child in parent:
        if(child.tag==tag_name):
            match_list.append(child)
        _xml_find_child_by_tag(match_list,child,tag_name)

def _parse_create_struct_file(struct_list,debug_info_file,out_file):
    #print(struct_list)
    #print(out_file)
    with open(out_file,'w+') as fp:
        print(struct_list,file=fp)

def parse_tools_xml(xml_file,debug_info_file,out_idr):
    (tree,root)=_xml_parse_tree(xml_file)
    list=_xml_find_son_by_tag(root,'GROUP')
    item_list=[]
    for item in list:
        if 'AUTO_STRUCT' in item.attrib and item.attrib['AUTO_STRUCT']=="YES":
            item_list.append(item)
    result_list = []
    for item in item_list:
        req_list=[]
        ind_list=[]
        struct_list=[]
        _xml_find_child_by_tag(req_list,item,'REQ')
        _xml_find_child_by_tag(ind_list,item,'IND')
        for req in req_list:
            struct_list.append(req.attrib['STRUCTURE'])
        for ind in ind_list:
            struct_list.append(ind.attrib['STRUCTURE'])
        #_parse_create_struct_file(struct_list,debug_info_file,os.path.join(out_idr,item.attrib['DATA_STRUCT_FILE']))
        result_list.extend(struct_list)
    return result_list
