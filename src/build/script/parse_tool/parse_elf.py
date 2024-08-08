#!/usr/bin/env python
# coding:utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.

import sys,string,re,os
from ctypes import *
from parse_basic import *
from xml_main import *
import json

g_drwaf_all_tag_tup = ('DW_TAG_compile_unit',
    'DW_TAG_subprogram',
    'DW_TAG_variable',
    'DW_TAG_GNU_call_site',
    'DW_TAG_typedef',
    'DW_TAG_base_type',
    'DW_TAG_array_type',
    'DW_TAG_subrange_type',
    'DW_TAG_pointer_type',
    'DW_TAG_subroutine_type',
    'DW_TAG_formal_parameter',
    'DW_TAG_const_type',
    'DW_TAG_structure_type',
    'DW_TAG_member',
    'DW_TAG_enumeration_type',
    'DW_TAG_enumerator',
    'DW_TAG_unspecified_parameters',
    'DW_TAG_lexical_block',
    'DW_TAG_inlined_subroutine',
    'DW_TAG_union_type',
    'DW_TAG_volatile_type',
    'DW_TAG_label',
    'DW_TAG_restrict_type',
    'DW_TAG_namespace',
    'DW_TAG_class_type',
    'DW_TAG_template_type_param',
    'DW_TAG_inheritance',
    'DW_TAG_GNU_template_parameter_pack',
    'DW_TAG_template_value_param',
    'DW_TAG_imported_declaration',
    'DW_TAG_reference_type',
    'DW_TAG_rvalue_reference_type',
    'DW_TAG_unspecified_type',
    'DW_TAG_imported_module',
    'DW_TAG_GNU_call_site_parameter')

g_drwaf_all_attr_tup = ('DW_AT_producer',
    'DW_AT_language',
    'DW_AT_name',
    'DW_AT_stmt_list',
    'DW_AT_comp_dir',
    'DW_AT_low_pc',
    'DW_AT_ranges',
    'DW_AT_high_pc',
    'DW_AT_frame_base',
    'DW_AT_GNU_all_call_sites',
    'DW_AT_decl_file',
    'DW_AT_decl_line',
    'DW_AT_prototyped',
    'DW_AT_type',
    'DW_AT_external',
    'DW_AT_location',
    'DW_AT_abstract_origin',
    'DW_AT_GNU_tail_call',
    'DW_AT_encoding',
    'DW_AT_byte_size',
    'DW_AT_count',
    'DW_AT_data_member_location',
    'DW_AT_const_value',
    'DW_AT_declaration',
    'DW_AT_inline',
    'DW_AT_call_file',
    'DW_AT_call_line',
    'DW_AT_call_column',
    'DW_AT_GNU_call_site_target',
    'DW_AT_bit_size',
    'DW_AT_bit_offset',
    'DW_AT_alignment',
    'DW_AT_sibling',
    'DW_AT_upper_bound',
    'DW_AT_entry_pc',
    'DW_AT_GNU_all_tail_call_sites',
    'DW_AT_specification',
    'DW_AT_linkage_name',
    'DW_AT_calling_convention',
    'DW_AT_accessibility',
    'DW_AT_artificial',
    'DW_AT_explicit',
    'DW_AT_containing_type',
    'DW_AT_virtuality',
    'DW_AT_vtable_elem_location',
    'DW_AT_noreturn',
    'DW_AT_object_pointer',
    'DW_AT_export_symbols',
    'DW_AT_enum_class',
    'DW_AT_GNU_call_site_value',
    'DW_AT_import')


class TreeNode:
    def __init__(self):
        self.data = None
        self.son = []
        self.parent = None

class Tag:
    def __init__(self):
        self.level_str=None
        self.entry_str=None
        self.type_str = None
        self.at_list = {}
        self.level = None
        self.son_tag_list = []
        self.parent = None
        self.at_name = None
        self.op_addr = None
        self.byte_size = None
        self.member_location = None
        self.random_name = None
        self.c_type_name = None
        self.member_name = None
        self.c_type_root_name = None
        self.member_array_tag = None
        self.member_type_tag = None
        self.member_bit_size = None
        self.root_tag = None
        self.var_array = []
        self.symbol_location = None

class Attribute:
    def __init__(self):
        self.entry_str=None
        self.type_str=None
        self.val_attr = None

class Signature:
    def __init__(self):
        self.signature=None
        self.sign_entry=0

class ParseCtrl():
    def __init__(self):
        self.debug_type = None
        self.symbol_dic_init=False
        self.parse_debug_info_phase_0=False
        self.parse_debug_info_phase_1=False
        self.out_type = 'python'
        self.symbol_dic={}
        self.tag_list = []
        self.tag_dic_by_entry = {}
        self.sign_list = []
        self.sign_entry_by_id = {}
        self.last_tag = [None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,
                         None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None,None]#支持48个等级
        self.tag_name_dic = {}
        self.attr_name_dic = {}
        self.random_val = 0
        self.class_fd = None
        self.out_type_dic={}
        self.var_symbol_dic={}
        self.c_basic_type=[
            'char','int','unsigned char','bool'
        ]

        self.name_convert_dic = {
            'bool':'td_bool',
            'char':'td_char',
            'signed char':'td_char',
            'unsigned char':'td_uchar',
            'int':'td_s32',
            'unsigned int':'td_u32',
            'short':'td_s16',
            'unsigned short':'td_u16',
            'short unsigned int':'td_u16',
            'long':'td_s32',
            'unsigned long':'td_u32',
            'long long':'td_s64',
            'unsigned long long':'td_u64',
            'float':'td_float',
            'double':'td_double',
            '__ARRAY_SIZE_TYPE__':'td_u32',
            'long unsigned int':'td_u32',
            '_Bool':'td_bool',
        }

        for key in self.name_convert_dic:
            self.out_type_dic[self.name_convert_dic[key]]=True

    def print_root_basic_type(self,fp):
        if self.out_type=='python':
            print('from ctypes import *',file=self.class_fd)
            print('td_bool=c_bool',file=fp)
            print('td_char=c_char',file=fp)
            print('td_uchar=c_ubyte',file=fp)
            print('td_s16=c_short',file=fp)
            print('td_u16=c_ushort',file=fp)
            print('td_s32=c_int',file=fp)
            print('td_u32=c_uint',file=fp)
            print('td_s64=c_longlong',file=fp)
            print('td_u64=c_ulonglong',file=fp)
            print('td_float=c_float',file=fp)
            print('td_double=c_double',file=fp)
        elif self.out_type=='c':
            print('typedef unsigned char td_bool;',file=fp)
            print('typedef char td_char;',file=fp)
            print('typedef unsigned char td_uchar;',file=fp)
            print('typedef short td_s16;',file=fp)
            print('typedef unsigned short td_u16;',file=fp)
            print('typedef short unsigned int td_u16;',file=fp) #add for arm, UINT16 is short unsiged int
            print('typedef int td_s32;',file=fp)
            print('typedef unsigned int td_u32;',file=fp)
            print('typedef long long td_s64;',file=fp)
            print('typedef unsigned long long td_u64;',file=fp)
            print('typedef float td_float;',file=fp)
            print('typedef double td_double;',file=fp)
        self.out_type_dic['td_bool']=True
        self.out_type_dic['td_char']=True
        self.out_type_dic['td_uchar']=True
        self.out_type_dic['td_s16']=True
        self.out_type_dic['td_u16']=True
        self.out_type_dic['td_s32']=True
        self.out_type_dic['td_u32']=True
        self.out_type_dic['td_s64']=True
        self.out_type_dic['td_u64']=True
        self.out_type_dic['td_float']=True
        self.out_type_dic['td_double']=True


    def append_space(self,line,cnt):
        i=0
        while i<cnt:
            line=' '+line
            i=i+1
        return line

    def conver_base_name(self,name):#将基础类型转为ctypes类型
        if name==None:
            return None
        if name in self.name_convert_dic:
            return self.name_convert_dic[name]
        return None

    def create_random_name(self):#生成一个随机名称
        self.random_val = self.random_val+1
        return "random_type_%d"%(self.random_val)

    def create_random_var_name(self):#生成一个随机名称
        self.random_val = self.random_val+1
        return "random_var_%d"%(self.random_val)

    def stat_add_tag_type(self,tag):#统计量
        self.tag_name_dic[tag.type_str] = tag.type_str

    def stat_add_attr_type(self,attr):#统计量
        self.attr_name_dic[attr.type_str] = attr.type_str

    def print_tag_info(self,tag,level):#打印tag
        suojin=''
        i=0
        while i<level:
            suojin="%s%s"%(suojin,'\t')
            i=i+1

        #print("%s[tag]type=%s(%s) level=%d,entry=%s,"%(suojin,tag.type_str,tag.at_name,tag.level,tag.entry_str))
        if tag.at_list != None:
            for key in tag.at_list:
                attr = tag.at_list[key]
                print('[attr]entry=%s,type=%s,val=%s'%(attr.entry_str,attr.type_str,attr.val_attr))
        if tag.at_name != None:
            print("%sat_name=%s"%(suojin,tag.at_name))
        if tag.random_name != None:
            print("%srandom_name=%s"%(suojin,tag.random_name))
        if tag.byte_size != None:
            print("%sbyte_size=%d"%(suojin,tag.byte_size))
        if tag.op_addr != None:
            print("%saddr=%d"%(suojin,tag.op_addr))
        if tag.member_location != None:
            print("%soffset=%s"%(suojin,tag.member_location))
        if tag.c_type_name != None:
            print("%sc_type_name=%s"%(suojin,tag.c_type_name))
        if tag.c_type_root_name != None:
            print("%sc_type_root_name=%s"%(suojin,tag.c_type_root_name))

        if len(tag.var_array) != 0:
            print("%svar_array=%s"%(suojin,tag.var_array))

        if tag.type_str == 'DW_TAG_member':
            self.print_tag_info(tag.member_type_tag,level+1)
            if(tag.member_array_tag):
                self.print_tag_info(tag.member_array_tag,level+1)

        if(level==0):
            print('')
            print('')
        #input("print_tag")

    def append_tag(self,tag):
        if tag:
            self.tag_list.append(tag)
            self.tag_dic_by_entry[tag.entry_str]=tag
            if tag.level!=0:
                self.last_tag[tag.level-1].son_tag_list.append(tag)
                tag.parent = self.last_tag[tag.level-1]
            self.last_tag[tag.level]=tag
            #self.print_tag_info(tag,0)

    def get_tag_by_entry_name(self,entry_str):#通过入口名查找tag
        if entry_str==None:
            return None
        if entry_str not in self.tag_dic_by_entry:
            return None
        return self.tag_dic_by_entry[entry_str]

    def get_at_name(self,tag):#获取名称(变量、类型等)
        if 'DW_AT_name' not in tag.at_list:
            return None
        result=re.findall(r'[\w\s]+$',tag.at_list['DW_AT_name'].val_attr.strip())
        if result:
            return result[0].strip()
        return None

    def get_at_type(self,tag):#获取入口名
        if 'DW_AT_type' not in tag.at_list:
            return None
        if 'signature:' in tag.at_list['DW_AT_type'].val_attr:
            result = re.findall(r'signature:(\s\w+)',tag.at_list['DW_AT_type'].val_attr.strip())
            if result[0].strip() not in _g_parse_ctrl.sign_entry_by_id:
                return None
            entry_str = _g_parse_ctrl.sign_entry_by_id[result[0].strip()]
            return entry_str
        result = re.findall('[<]([\w\s]+)[>]$',tag.at_list['DW_AT_type'].val_attr.strip())
        entry_str = result[0][2:]
        return entry_str

    def get_at_byte_size(self,tag):#获取bytesize(类型)
        if tag.type_str == "DW_TAG_pointer_type":
            return 4
        if 'DW_AT_byte_size' not in tag.at_list:
            return None
        str = tag.at_list['DW_AT_byte_size'].val_attr
        return int(str,0)

    def get_at_bit_size(self,tag):#获取bytesize(类型)
        if 'DW_AT_bit_size' not in tag.at_list:
            return None
        str = tag.at_list['DW_AT_bit_size'].val_attr
        return int(str,0)

    def get_at_data_member_location(self,tag):#获取member相对偏移
        if 'DW_AT_data_member_location' not in tag.at_list:
            return None
        str = tag.at_list['DW_AT_data_member_location'].val_attr
        return int(str,0)

    def get_at_count(self,tag):#数组长度
        if 'DW_AT_count' not in tag.at_list:
            return None
        str = tag.at_list['DW_AT_count'].val_attr
        if (str.startswith('<')):
            return None
        return int(str,0)

    def get_at_location(self,tag):
        if 'DW_AT_location' not in tag.at_list:
            return None
        str = tag.at_list['DW_AT_location'].val_attr.strip()

        result = re.findall(r'DW_OP_addr:(\s\w+)',str)
        if result:
            #print(str,'------------',result[0])
            #print(str,' result=',result[0])
            return int(result[0],16)
        else:
            #print(str,' result=None')
            return None

    def get_root_tag(self,tag):#查找一个typedef类型的根类型
        if tag.type_str != 'DW_TAG_typedef':
            return tag

        at_name = self.get_at_name(tag)
        if at_name=='td_void' or at_name=='MC_VOID':#特殊情况
            return tag

        entry_str = self.get_at_type(tag)
        if entry_str==None:#特殊情况td_void
            return None
            print("[ASSERT][LINE=%d]"%(sys._getframe().f_lineno),tag.entry_str)
            assert(False)
        new_tag = self.get_tag_by_entry_name(entry_str)
        if new_tag==None:
            print("[ASSERT][LINE=%d]"%(sys._getframe().f_lineno),tag.entry_str)
            assert(False)
        return self.get_root_tag(new_tag)

    def byte_size_2_c_type_name(self,byte_size):#根据类型size转基础类型(枚举转换)
        if byte_size==1:
            return 'td_u8'
        elif byte_size==2:
            return 'td_u16'
        elif byte_size == 4:
            return 'td_u32'
        elif byte_size==8:
            return 'c_ulonglong'
        return None

    def process_phase1_base_type(self,tag):
        at_name = self.get_at_name(tag)
        tag.at_name = at_name
        at_name = self.conver_base_name(at_name)
        tag.c_type_name = at_name
        tag.c_type_root_name = at_name
        tag.byte_size = self.get_at_byte_size(tag)
        tag.root_tag = tag
        #self.print_tag_info(tag,0)

    def process_phase1_enumeration_type(self,tag):
        at_name = self.get_at_name(tag)
        tag.at_name = at_name
        tag.byte_size = self.get_at_byte_size(tag)
        tag.c_type_name = self.byte_size_2_c_type_name(tag.byte_size)
        tag.c_type_root_name = tag.c_type_name
        tag.root_tag = tag
        #self.print_tag_info(tag,0)
    def process_phase1_pointer_type(self,tag):
        tag.byte_size = 4
        tag.c_type_name = 'td_u32'
        tag.c_type_root_name = 'td_u32'
        tag.root_tag = tag
        #self.print_tag_info(tag,0)

    def process_phase1_structure_type(self,tag):
        at_name = self.get_at_name(tag)
        tag.byte_size = self.get_at_byte_size(tag)
        tag.at_name = at_name
        if tag.at_name==None:
            tag.random_name = self.create_random_name()
            tag.c_type_name = tag.random_name
            tag.c_type_root_name = tag.random_name
            tag.root_tag = tag
        else:
            tag.c_type_name = tag.at_name
            tag.c_type_root_name = tag.at_name
            tag.root_tag = tag
        #self.print_tag_info(tag,0)

    def process_phase1_union_type(self,tag):
        at_name = self.get_at_name(tag)
        tag.byte_size = self.get_at_byte_size(tag)
        tag.at_name = at_name
        if tag.at_name==None:
            tag.random_name = self.create_random_name()
            tag.c_type_name = tag.random_name
            tag.c_type_root_name = tag.random_name
            tag.root_tag = tag
        else:
            tag.c_type_name = tag.at_name
            tag.c_type_root_name = tag.at_name
            tag.root_tag = tag
        #self.print_tag_info(tag,0)

    def process_phase1_type_def(self,tag):
        at_name = self.get_at_name(tag)
        tag.at_name = at_name
        if tag.at_name==None:
            return
            assert(False)
        tag.c_type_name = tag.at_name
        root_tag = self.get_root_tag(tag)
        if root_tag==None:
            return
            assert(False)
        tag.byte_size = root_tag.byte_size #TBD 是否已经生成
        tag.c_type_root_name = root_tag.c_type_root_name #TBD 是否已经生成
        tag.root_tag = root_tag
        #self.print_tag_info(tag,0)

    def process_phase_array_type(self,tag):
        tag.var_array = []
        for son in tag.son_tag_list:
            if son.type_str == 'DW_TAG_subrange_type':
                cnt = self.get_at_count(son)
                if cnt==None: #特殊情况 data[]
                    cnt=0
                tag.var_array.append(cnt)
        #self.print_tag_info(tag,0)

    def process_phase1_member_relation_tag(self,member_tag,tag):
        entry_str = self.get_at_type(tag)
        new_tag = self.get_tag_by_entry_name(entry_str)
        if not new_tag:
            return
        if new_tag.type_str=='DW_TAG_array_type':
            member_tag.member_array_tag = new_tag
        elif new_tag.type_str=='DW_TAG_volatile_type' or new_tag.type_str=='DW_TAG_const_type':
            pass
        elif new_tag.type_str=='DW_TAG_typedef':
            member_tag.member_type_tag = new_tag
        elif (new_tag.type_str=='DW_TAG_base_type'
                or new_tag.type_str=='DW_TAG_structure_type'
                or new_tag.type_str=='DW_TAG_enumeration_type'
                or new_tag.type_str=='DW_TAG_pointer_type'
                or new_tag.type_str=='DW_TAG_union_type' ):
            member_tag.member_type_tag = new_tag
        elif (new_tag.type_str=='DW_TAG_class_type'
                or new_tag.type_str=='DW_TAG_reference_type'):
            member_tag.member_type_tag=None#不支持
            return
        else:
            print("[new tag type can not support parse]new_tag.type_str=%s" % new_tag.type_str)
            return

        if member_tag.member_type_tag==None:
            self.process_phase1_member_relation_tag(member_tag,new_tag)
        #self.print_tag_info(tag,0)

    def process_phase1_member(self,tag):
        tag.member_location = self.get_at_data_member_location(tag)
        tag.at_name = self.get_at_name(tag)
        tag.member_name = tag.at_name
        tag.member_bit_size = self.get_at_bit_size(tag)
        if tag.member_name==None:#结构体内部内嵌练合体
            tag.member_name = self.create_random_var_name()
        if tag.member_location==None:#有些联合体没有location
            pass
        self.process_phase1_member_relation_tag(tag,tag)
        #self.print_tag_info(tag,0)

    def process_phase1_variable(self,tag):
        tag.at_name = self.get_at_name(tag)
        tag.symbol_location = self.get_at_location(tag)
        if tag.symbol_location:
            self.process_phase1_member_relation_tag(tag,tag)
            self.var_symbol_dic[tag.at_name]=tag
            #print(tag.at_name)

    def process_phase1_tag(self,tag):
        #tag_process(tag)
        #self.print_tag_info(tag,0)
        for son in tag.son_tag_list:
            self.process_phase1_tag(son)

        if tag.type_str == "DW_TAG_base_type":
            self.process_phase1_base_type(tag)
        if tag.type_str == 'DW_TAG_enumeration_type':
            self.process_phase1_enumeration_type(tag)
        if tag.type_str == 'DW_TAG_structure_type':
            self.process_phase1_structure_type(tag)
        if tag.type_str == 'DW_TAG_typedef':
            self.process_phase1_type_def(tag)
        if tag.type_str == 'DW_TAG_pointer_type':
            self.process_phase1_pointer_type(tag)
        if tag.type_str == 'DW_TAG_member':
            self.process_phase1_member(tag)
        if tag.type_str == 'DW_TAG_array_type':
            self.process_phase_array_type(tag)
        if tag.type_str == 'DW_TAG_union_type':
            self.process_phase1_union_type(tag)
        if tag.type_str == 'DW_TAG_variable':
            self.process_phase1_variable(tag)

    def process_phase1(self):
        if self.parse_debug_info_phase_1==True:
            return
        process_list = ['DW_TAG_base_type',
            'DW_TAG_structure_type',
            'DW_TAG_enumeration_type',
            'DW_TAG_pointer_type',
            'DW_TAG_member',
            'DW_TAG_array_type',
            'DW_TAG_union_type',
            'DW_TAG_variable',
        ]
        for tag in self.tag_list:
            if tag.level!=1:
                continue
            if tag.type_str not in process_list:
                continue
            self.process_phase1_tag(tag)

        process_list = ['DW_TAG_typedef'
        ]

        for tag in self.tag_list:
            if tag.level!=1:
                continue
            if tag.type_str not in process_list:
                continue
            self.process_phase1_tag(tag)

        self.parse_debug_info_phase_1=True

    def print_tag_2_class(self,tag,lines):
        #print("print=",tag.entry_str)
        for line in lines:
            print(line,file=self.class_fd)

    def process_print_enumeration_type(self,tag):
        return DEFINE_FAIL

    def process_print_python_typedef(self,tag):

        if tag.c_type_name=='td_void' or tag.c_type_name=='MC_VOID':
            return DEFINE_SUCCESS
        if tag.c_type_root_name in self.out_type_dic:
            lines=[]
            line='%s=%s'%(tag.c_type_name,tag.c_type_root_name)
            lines.append(line)
            self.print_tag_2_class(tag,lines)
            self.out_type_dic[tag.c_type_name] = True
            return DEFINE_SUCCESS
        else:#原类型还没有定义,先打印原类型
            if (tag.root_tag)==None:# TD_VOID
                return DEFINE_FAIL
            ret = self.print_depend_tag(tag.root_tag)
            if ret!=DEFINE_SUCCESS:
                return ret
            return self.process_print_typedef(tag)

    def process_print_c_typedef(self,tag):
        #print(tag.c_type_name,tag.c_type_root_name)

        if tag.c_type_name=='td_void' or tag.c_type_name=='MC_VOID':
            return DEFINE_SUCCESS
        if tag.c_type_root_name in self.out_type_dic:
            lines=[]
            line='typedef %s %s;'%(tag.c_type_root_name,tag.c_type_name)
            lines.append(line)
            self.print_tag_2_class(tag,lines)
            self.out_type_dic[tag.c_type_name] = True
            return DEFINE_SUCCESS
        else:#原类型还没有定义,先打印原类型
            if (tag.root_tag)==None:# TD_VOID
                return DEFINE_FAIL
            ret = self.print_depend_tag(tag.root_tag)
            if ret!=DEFINE_SUCCESS:
                return ret
            return self.process_print_typedef(tag)

    def process_print_typedef(self,tag):
        if self.out_type=='python':
            return self.process_print_python_typedef(tag)
        elif self.out_type=='c':
            return self.process_print_c_typedef(tag)

    def process_print_python_member(self,tag):
        if tag.member_type_tag==None:
            return (DEFINE_FAIL,[])

        if tag.member_array_tag:

            tmp=r'%s'%(self.get_c_type_name(tag.member_type_tag))
            for val in tag.member_array_tag.var_array[::-1]:
                tmp=r'(%s*%d)'%(tmp,val)
            line=r"('%s', %s),"%(tag.member_name,tmp)
            ret = self.print_depend_tag(tag.member_type_tag)
            return (ret,line)
        elif tag.member_bit_size:
            line=r"('%s', %s,%d),"%(tag.member_name,self.get_c_type_name(tag.member_type_tag),tag.member_bit_size)
            ret = self.print_depend_tag(tag.member_type_tag)
            return (ret,line)
        else:
            line=r"('%s', %s),"%(tag.member_name,self.get_c_type_name(tag.member_type_tag))
            ret = self.print_depend_tag(tag.member_type_tag)
            return (ret,line)

    def process_print_c_member(self,tag):
        if tag.member_type_tag==None:
            return (DEFINE_FAIL,[])

        if tag.member_array_tag:
            tmp=r'%s %s'%(self.get_c_type_name(tag.member_type_tag),tag.member_name)
            for val in tag.member_array_tag.var_array:
                tmp=r'%s[%d]'%(tmp,val)
            line=r"%s;"%(tmp)
            ret = self.print_depend_tag(tag.member_type_tag)
            return (ret,line)
        elif tag.member_bit_size:
            line=r"%s %s:%d;"%(self.get_c_type_name(tag.member_type_tag),tag.member_name,tag.member_bit_size)
            ret = self.print_depend_tag(tag.member_type_tag)
            return (ret,line)
        else:
            line=r"%s %s;"%(self.get_c_type_name(tag.member_type_tag),tag.member_name)
            ret = self.print_depend_tag(tag.member_type_tag)
            return (ret,line)

    def process_print_member(self,tag):
        if self.out_type=='python':
            return self.process_print_python_member(tag)
        elif self.out_type=='c':
            return self.process_print_c_member(tag)

    def get_c_type_name(self,tag):
        return tag.c_type_name

    def process_print_python_structure_type(self,tag):
        lines=[]
        line="class %s(Structure):"%(self.get_c_type_name(tag))
        if self.get_c_type_name(tag)=='tskTaskControlBlock' and not tag.son_tag_list:
            return DEFINE_SUCCESS
        if self.get_c_type_name(tag)=='QueueDefinition' and not tag.son_tag_list:
            return DEFINE_SUCCESS
        if tag.c_type_name in self.out_type_dic:
            return DEFINE_SUCCESS
        lines.append(line)
        line=self.append_space('_fields_ = [',4)
        lines.append(line)

        for son in tag.son_tag_list:
            if son.type_str=='DW_TAG_member':
                (ret,line)=self.process_print_member(son)
                if ret!= DEFINE_SUCCESS:
                    return ret
                line=self.append_space(line,8)
                lines.append(line)

        line=self.append_space(']',4)
        lines.append(line)
        self.print_tag_2_class(tag,lines)
        self.out_type_dic[tag.c_type_name] = True
        return DEFINE_SUCCESS

    def process_print_c_structure_type(self,tag):
        lines=[]
        line="typedef struct {"
        lines.append(line)

        for son in tag.son_tag_list:
            if son.type_str=='DW_TAG_member':
                (ret,line)=self.process_print_member(son)
                if ret!= DEFINE_SUCCESS:
                    return ret
                line=self.append_space(line,4)
                lines.append(line)

        line=self.append_space('}%s;'%(self.get_c_type_name(tag)),0)
        lines.append(line)
        self.print_tag_2_class(tag,lines)
        self.out_type_dic[tag.c_type_name] = True
        return DEFINE_SUCCESS

    def process_print_structure_type(self,tag):
        if self.out_type=='python':
            return self.process_print_python_structure_type(tag)
        elif self.out_type=='c':
            return self.process_print_c_structure_type(tag)

    def process_print_python_union_type(self,tag):
        lines=[]
        line="class %s(Union):"%(self.get_c_type_name(tag))
        lines.append(line)
        line=self.append_space('_fields_ = [',4)
        lines.append(line)

        for son in tag.son_tag_list:
            if son.type_str=='DW_TAG_member':
                (ret,line)=self.process_print_member(son)
                if ret!= DEFINE_SUCCESS:
                    return ret
                line=self.append_space(line,8)
                lines.append(line)

        line=self.append_space(']',4)
        lines.append(line)
        self.print_tag_2_class(tag,lines)
        self.out_type_dic[tag.c_type_name] = True
        return DEFINE_SUCCESS

    def process_print_c_union_type(self,tag):
        lines=[]
        line="typedef union {"
        lines.append(line)
        for son in tag.son_tag_list:
            if son.type_str=='DW_TAG_member':
                (ret,line)=self.process_print_member(son)
                if ret!= DEFINE_SUCCESS:
                    return ret
                line=self.append_space(line,4)
                lines.append(line)

        line=self.append_space('}%s;'%(self.get_c_type_name(tag)),0)
        lines.append(line)
        self.print_tag_2_class(tag,lines)
        self.out_type_dic[tag.c_type_name] = True
        return DEFINE_SUCCESS

    def process_print_union_type(self,tag):
        if self.out_type=='python':
            return self.process_print_python_union_type(tag)
        elif self.out_type=='c':
            return self.process_print_c_union_type(tag)

    def print_tag(self,tag):
        ret=DEFINE_SUCCESS
        if self.get_c_type_name(tag)!='tskTaskControlBlock' and self.get_c_type_name(tag)!='QueueDefinition' and tag.c_type_name in self.out_type_dic:
            return DEFINE_SUCCESS
        if tag.type_str == "DW_TAG_enumeration_type":
            ret = self.process_print_enumeration_type(tag)
        elif tag.type_str == "DW_TAG_typedef":
            ret = self.process_print_typedef(tag)
        elif tag.type_str == "DW_TAG_structure_type":
            ret = self.process_print_structure_type(tag)
        elif tag.type_str == "DW_TAG_union_type":
            ret = self.process_print_union_type(tag)
        else:
            return DEFINE_FAIL
        return ret

    def print_depend_tag(self,tag):
        #print("depend=",tag.entry_str)
        ret = self.print_tag(tag)
        if ret!=DEFINE_SUCCESS:
            pass
        else:
            pass
        return ret

    def print_enter_tag(self,tag):
        #print('enter=',tag.entry_str)
        ret = self.print_tag(tag)
        if ret!=DEFINE_SUCCESS:
            pass
        else:
            pass
        return ret

    def process_phase2(self,out_file,language,type_name_list):
        _g_parse_ctrl.out_type=language
        self.out_type_dic={}
        print("out_file:")
        print(out_file)
        with open(out_file,'w+') as fp:
            _g_parse_ctrl.class_fd = fp
            self.print_root_basic_type(_g_parse_ctrl.class_fd)
            process_list = ['DW_TAG_structure_type',
                'DW_TAG_enumeration_type',
                'DW_TAG_typedef',
                'DW_TAG_pointer_type',
                'DW_TAG_union_type',
            ]
            for tag in self.tag_list:
                if tag.level!=1:
                    continue
                if tag.type_str not in process_list:
                    continue
                if type_name_list!=None and (tag.c_type_name not in type_name_list):
                    continue
                self.print_enter_tag(tag)

    def print_var_array(self,bin,type_tag):
        pass

    def print_var_type(self,bin,type_tag):
        tag = type_tag.root_tag
        print(tag.entry_str)
        if tag.type_str=='DW_TAG_union_type':
            pass

    def print_g_var(self,var_name):
        tag=None
        arry_tag=None
        if var_name in self.var_symbol_dic:
            tag = self.var_symbol_dic[var_name]
        if tag==None:
            return

        if tag.member_array_tag:
            arry_tag=tag.member_array_tag
            for x in arry_tag.var_array:
                print(x)

        bin = bytearray(108)
        self.print_var_type(bin,tag.member_type_tag)
        print(tag.at_name)

    def process_phase_0(self,file):
        """读取debug info file,生成通用tag信息
        Args
            file: debug info file路径
        return
        """
        if self.parse_debug_info_phase_0==True:
            return
        with open(file,'r') as fp:
            lines=fp.readlines()
            self.parse_debug_info_lines(lines)
        self.parse_debug_info_phase_0=True

    def parase_debug_types(self, line, sign):
        if('Compilation Unit' in line):
            result = line.split(' ')
            sign.sign_entry = int(result[4][:-1],16)
        if('Signature:' in line):
            result = line.split(':')
            sign.signature = result[1].strip()
        if('Type Offset:' in line):
            result = line.split(':')
            sign.sign_entry += int(result[1].strip(), 16)
            self.sign_entry_by_id[sign.signature] = '{:08X}'.format(sign.sign_entry + int('0x10000000', 16))
            sign.signature = None
            sign.sign_entry = 0
    def parse_debug_info_lines(self,lines):
        pass_st_lines=['application.elf','wstp.elf','tstp.elf','bt.elf','Contents of the','Compilation Unit',
                        'Length:','Version:','Abbrev Offset:','Pointer Size:', 'Signature', 'Type Offset','$']

        tag = None
        new_lines = []
        sign = Signature()
        for line in lines:
            line = line.strip();
            if('debug_types' in line):
                _g_parse_ctrl.debug_type = True
            if('debug_info' in line):
                _g_parse_ctrl.debug_type = False
            if _g_parse_ctrl.debug_type:
                self.parase_debug_types(line, sign)
            if(self.match_line_list(line,pass_st_lines)):
                continue
            if re.match('[<]\w+[>][<]\w+[>]:',line):#tag start line
                if tag:
                    _g_parse_ctrl.append_tag(tag)
                tag = Tag()
                self.parase_debug_info_tag_start_line(line,tag)
            else:
                self.parase_debug_info_tag_other_line(line,tag)
        _g_parse_ctrl.append_tag(tag)

    def parase_debug_info_tag_start_line(self,line,tag):
        result = re.findall(r'[<](.*?)[>]',line)#取<>中的数据
        tag.level_str = result[0]
        if _g_parse_ctrl.debug_type == True:
            tag.entry_str = '{:08X}'.format(int(result[1], 16) + int('0x10000000', 16))
        else:
            tag.entry_str = result[1]
        tag.level = int(tag.level_str,10)
        result = re.findall(r'[(](.*?)[)]', line)#取()中的数据
        if(result):
            tag.type_str = result[0]
            _g_parse_ctrl.stat_add_tag_type(tag)

    def parase_debug_info_tag_other_line(self,line,tag):
        attr = Attribute();
        result = re.findall(r'^[<](.*?)[>]',line)#取第一个<>中的数据
        # attr.entry_str = result[0]
        if _g_parse_ctrl.debug_type == True:
            attr.entry_str = '{:08X}'.format(int(result[0], 16) + int('0x10000000', 16))
        else:
            attr.entry_str = result[0]
        result = re.findall('DW_AT_\w+',line)#取DW_AT_xxx
        attr.type_str = result[0]
        result = re.findall('DW_AT_\w+\s*:([\s,\S]*)',line)
        attr.val_attr = result[0].strip()
        if attr.type_str == "DW_AT_type" and _g_parse_ctrl.debug_type == True:
            result = re.findall('[<]([\w\s]+)[>]$', attr.val_attr.strip())
            if result != []:
                entry_str = result[0][2:]
                attr.val_attr = '<0x{:08X}>'.format(int(entry_str, 16) + int('0x10000000', 16))
        _g_parse_ctrl.stat_add_attr_type(attr)
        tag.at_list[attr.type_str]=attr

    def match_line_list(self,line,mach_list):
        for x in mach_list:
            if re.match(x,line):
                return True
        return False

    def parse_nm_file(self,file,dic):
        """生成symbol字典
        Args
            file: nm file路径
            dic:symbol字典
        return
        """
        if self.symbol_dic_init==True:
            return
        with open(file,'r') as fp:
            lines=fp.readlines()
        for line in lines:
            line=line.strip()
            if len(line)==0:
                continue
            list = line.split('|')
            if len(list)!=7:
                continue

            sysmbol = Sysmbol()
            sysmbol.key_name = list[0].strip()
            sysmbol.addr = int(list[1].strip(),16)
            sysmbol.class_type = list[2].strip()
            sysmbol.type = list[3].strip()
            if len(list[4].strip()):
                sysmbol.size = int(list[4].strip(),16)
            sysmbol.section = list[6].strip()
            sysmbol.guess_size=0

            dic[sysmbol.key_name] = sysmbol
            if sysmbol.size:
                pass
            else:
                pass
        self.symbol_dic_init = True

def tag_cmn_attr_process(tag):
    if 'DW_AT_type' in tag.at_list:
        str = tag.at_list['DW_AT_type'].val_attr.strip()
        result = re.search('[<](\w*)[>]',str)
        input("yyyyyyyy")

def tag_process_variable(tag):
    if 'DW_AT_name' in tag.at_list:
        str = tag.at_list['DW_AT_name'].val_attr
        result = re.search('\w+$',str)
        tag.at_name=result[0]

    if 'DW_AT_location' in tag.at_list:
        str = tag.at_list['DW_AT_location'].val_attr
        result=re.findall(r'DW_OP_addr:\s*(\w*)',str)
        if result:
            tag.op_addr=int(result[0],16)

def _parse_get_symbol_info(var_name):
    if var_name not in _g_parse_ctrl.symbol_dic:
        return None
    return _g_parse_ctrl.symbol_dic[var_name]

def _parse_get_u32_symbol_val(name):
    addr = parse_get_symbol_addr(name)
    obj=parse_memory_2_class(addr,c_uint,4)
    addr=obj.value
    return addr

def _parse_get_symbol_addr(var_name):
    symbol=parse_get_symbol_info(var_name)
    if symbol==None:
        return None
    return symbol.addr

def _parse_addr_2_function_name(addr):
    for key in _g_parse_ctrl.symbol_dic:
        symbol = _g_parse_ctrl.symbol_dic[key]
        if symbol.type=='FUNC' and symbol.addr==addr:
            return symbol.key_name
    return None

def _parse_addr_in_2_function_name(addr):
    for key in _g_parse_ctrl.symbol_dic:
        symbol = _g_parse_ctrl.symbol_dic[key]
        size = symbol.size if symbol.size!=None else symbol.guess_size
        if symbol.type=='FUNC' and addr>=symbol.addr and addr < (symbol.addr+size):
            return symbol.key_name
    return None

def get_array_and_type_info(tag,member_tag):
    entry_str = _g_parse_ctrl.get_at_type(tag)
    if entry_str == None:
        member_tag.member_type_tag=None#不支持
        return
    new_tag = _g_parse_ctrl.get_tag_by_entry_name(entry_str)
    if new_tag.type_str=='DW_TAG_array_type':
        member_tag.member_array_tag = new_tag
    elif new_tag.type_str=='DW_TAG_volatile_type' or new_tag.type_str=='DW_TAG_const_type':
        pass
    elif new_tag.type_str=='DW_TAG_typedef':
        member_tag.member_type_tag = new_tag
    elif (new_tag.type_str=='DW_TAG_base_type'
            or new_tag.type_str=='DW_TAG_structure_type'
            or new_tag.type_str=='DW_TAG_enumeration_type'
            or new_tag.type_str=='DW_TAG_pointer_type'
            or new_tag.type_str=='DW_TAG_union_type' ):
        member_tag.member_type_tag = new_tag
    elif new_tag.type_str=='DW_TAG_class_type':
        member_tag.member_type_tag=None#不支持
        return
    else:
        print("[ASSERT][LINE=%d]"%(sys._getframe().f_lineno),member_tag.entry_str)
        assert(False)

    if member_tag.member_type_tag==None:
        get_array_and_type_info(member_tag,new_tag)

def _traverse_g_var(global_file):
    temp_dic_a={}
    temp_dic_b={}
    with open(global_file, "w+") as var_fp:
        for key in _g_parse_ctrl.symbol_dic:
            symbol = _g_parse_ctrl.symbol_dic[key]
            if symbol.type=='OBJECT':
                temp_dic_a[symbol.key_name]=1
                pass

        for tag in _g_parse_ctrl.tag_list:
            if tag.type_str!='DW_TAG_variable':
                continue
            if tag.level!=1:
                continue
            at_name=_g_parse_ctrl.get_at_name(tag)
            if at_name not in _g_parse_ctrl.symbol_dic:
                continue
            get_array_and_type_info(tag,tag)
            if(tag.member_array_tag):
                _g_parse_ctrl.process_phase_array_type(tag.member_array_tag)
            if(tag.member_array_tag and tag.member_type_tag):
                print("%s|0x%x|%s|%s|"%(at_name,_g_parse_ctrl.symbol_dic[at_name].addr,_g_parse_ctrl.symbol_dic[at_name].size,tag.member_type_tag.c_type_name),tag.member_array_tag.var_array,file=var_fp)
            elif (tag.member_type_tag):
                print("%s|0x%x|%s|%s"%(at_name,_g_parse_ctrl.symbol_dic[at_name].addr,_g_parse_ctrl.symbol_dic[at_name].size,tag.member_type_tag.c_type_name),file=var_fp)
            else:
                print("%s|0x%x|%s|%s"%(at_name,_g_parse_ctrl.symbol_dic[at_name].addr,_g_parse_ctrl.symbol_dic[at_name].size,''), file=var_fp)

def parse_elf_step_1(debug_info_file,out_class_file,out_ctype_file,nm_file,global_file,diag_dir,xml_file):
    type_name_list = parse_tools_xml(xml_file, debug_info_file, diag_dir)
    _g_parse_ctrl.parse_nm_file(nm_file,_g_parse_ctrl.symbol_dic)
    _g_parse_ctrl.process_phase_0(debug_info_file)
    _g_parse_ctrl.process_phase1()
    _g_parse_ctrl.process_phase2(out_class_file,'python',None)
    _g_parse_ctrl.process_phase2(out_ctype_file,'c',type_name_list)
    _traverse_g_var(global_file)
    uapi_register_function('get_symbol_info',_parse_get_symbol_info)
    uapi_register_function('get_u32_symbol_val',_parse_get_u32_symbol_val)
    uapi_register_function('get_symbol_addr',_parse_get_symbol_addr)
    uapi_register_function('addr_2_function_name',_parse_addr_2_function_name)
    uapi_register_function('addr_in_2_function_name',_parse_addr_in_2_function_name)

def parse_elf_step_2(nm_file):
    _g_parse_ctrl.parse_nm_file(nm_file,_g_parse_ctrl.symbol_dic)
    uapi_register_function('get_symbol_info',_parse_get_symbol_info)
    uapi_register_function('get_u32_symbol_val',_parse_get_u32_symbol_val)
    uapi_register_function('get_symbol_addr',_parse_get_symbol_addr)
    uapi_register_function('addr_2_function_name',_parse_addr_2_function_name)
    uapi_register_function('addr_in_2_function_name',_parse_addr_in_2_function_name)

_g_parse_ctrl = ParseCtrl()


