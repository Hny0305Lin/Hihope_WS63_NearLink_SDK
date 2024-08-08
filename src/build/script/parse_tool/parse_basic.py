#!/usr/bin/env python
# coding:utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.

import sys,string,re,os
from ctypes import *

DEFINE_SUCCESS=0
DEFINE_FAIL=1

class Sysmbol:
    """符号表中的符号
    """
    def __init__(self):
        self.addr=None #符号地址
        self.key_name = None #符号名
        self.class_type=None #符号类型 'a','t','b','d'...
        self.type = None #符号类型 'FUNC','OBJECT','NOTYPE' ...
        self.size = None #符号size,不是都有
        self.section=None #符号落在哪个段
        self.guess_size=None #计算出的符号大小(不准),TBD

class _BasicCtrl:
    def __init__(self):
        self.temp_memory = bytearray(4096) # 临时空间，用于计算

def uapi_str_to_int(str,num):
    """字符串转数字
    Args
        str: 字符串
        num: 进制
    return
        int:数值
    """
    if num!=None:
        return int(str,num)
    match1=re.match(r'\s*0x',str)
    match2=re.match(r'\s*0X',str)
    if match1 or match2:
        return int(str,16)
    else:
        return int(str,10)


def uapi_is_hex_word(word):
    """判断是否是16进制字符串
    Args
        str: 字符串
    return
        True:是
        False:不是
    """
    if word is None:
        return False
    for char in word:
        if char not in string.hexdigits:
            return False
    return True

_g_ctype_x16_class=[c_byte,c_ubyte,c_short,c_ushort,c_int,c_uint,c_long,c_ulong,c_longlong,c_ulonglong,c_size_t,c_ssize_t]
_g_ctype_float_class=[c_float,c_double,c_longdouble]
_g_ctype_char=[c_char,c_bool]

def _print_ctypes_obj(var_name,obj,t,fd):
    if type(obj)==int:
        print('%s=0x%x'%(var_name,obj),file=fd)
        return
    if type(obj)==bytes:
        print('%s'%(var_name),obj,file=fd)
        return
    if type(obj)==bool:
        print('%s'%(var_name),obj,file=fd)
        return
    if type(obj)==float:
        print('%s=%f'%(var_name,obj),file=fd)
        return

    if type(obj) in _g_ctype_x16_class:
        print('%s=0x%x'%(var_name,obj.value),file=fd)
        return
    if type(obj) in _g_ctype_float_class:
        print('%s=%f'%(var_name,obj.value),file=fd)
        return
    if type(obj) in _g_ctype_char:
        print('%s'%(var_name),obj.value,file=fd)
        return

    if hasattr(obj, '_length_'):
        if hasattr(obj,'value'):
            print('%s='%(var_name),obj.value,file=fd)
        else:
            i=0
            for m in obj:
                prefix="%s[%d]"%(var_name,i)
                _print_ctypes_obj(prefix,m,getattr(obj,'_type_'),fd)
                i=i+1
        return
    if hasattr(obj, '_fields_'):
        for field in obj._fields_:
            name = field[0]
            val = getattr(obj, field[0])
            _print_ctypes_obj(var_name+'.'+name,val,field[1],fd)
        return

    assert(False)

def uapi_print_ctypes_obj(var_name,obj,fd=sys.stdout):
    """打印ctypes object
    Args
        var_name: 打印名称
        obj: 被打印的obj
        fd: 输出目标
    return
    """
    print(var_name+'{',file=fd)
    _print_ctypes_obj('    ',obj,type(obj),fd)
    print('}',file=fd)

def ctype_member_offset(c_member,c_class):
    """计算ctype类型的成员相对类的偏移
    Args
        c_member: 成员名字符串
        c_class: ctype类
        fd: 输出目标
    return
        int:偏移
    """
    list=c_member.split('.')
    obj=c_class.from_buffer(_g_basic_ctrl.temp_memory)
    temp_obj=obj
    for member in list:
        x=getattr(temp_obj,member)
        temp_obj=x
    return addressof(x)-addressof(obj)

class _ParseBinItem:
    def __init__(self):
        file_name=None
        start_addr=None
        size = None
        data=None

class _ParseMemoryManager:
    def __init__(self):
        self.momory_list=[]

    def register_memory(self,file_name,start_addr,size):
        with open(file_name,'rb') as fp:
            item = _ParseBinItem()
            data = fp.read()
            item.file_name=file_name
            item.start_addr = start_addr
            item.data=bytearray(data)
            item.size=size
            self.momory_list.append(item)
    def register_bin(bin,start_addr,size):
        item = _ParseBinItem()
        item.file_name=None
        item.start_addr = start_addr
        item.data=bytearray(bin)
        item.size=size
        self.momory_list.append(item)

    def memory_get(self,addr,size):
        match_item=None
        for item in self.momory_list:
            if addr >= item.start_addr and addr+size < item.start_addr+item.size:
                match_item=item
                break
        if match_item==None:
            return None
        addr=addr-match_item.start_addr
        return match_item.data[addr:addr+size]

class _HookFunctions:
    def __init__(self):
        self.get_symbol_info = None
        self.get_u32_symbol_val = None
        self.get_symbol_addr = None
        self.addr_2_function_name = None
        self.addr_in_2_function_name = None

def parse_memory_get(addr,size):
    """获取一段空间
    Args
        addr: 起始地址
        size:大小
    return
        binary:成功返回一段空间
        None:这段空间不存在
    """
    return _g_parse_memory_manager.memory_get(addr,size)

def parse_memory_2_class(addr,class_type,class_size):
    """将一段空间转换成一个ctype obj
    Args
        addr: 起始地址
        class_type: class 类型
        class_size: class size
    return
        obj:返回一个ctype obj
        None:这段空间不存在
    """
    bin = parse_memory_get(addr,class_size)
    if bin==None:
        return None
    return class_type.from_buffer(bin)

def parse_memory_2_string(addr,max_len):
    obj=parse_memory_2_class(addr,(c_char*max_len),sizeof((c_char*max_len)))
    if obj==None:
        return None
    else:
        return obj.value.decode('utf-8')

def parse_memory_register(file_name,start_addr,size):
    """注册一段内存
    Args
        file_name: 内存来自文件，文件全路径
        start_addr: 内存起始地址
        size: 内存大小
    return
    """
    _g_parse_memory_manager.register_memory(file_name,start_addr,size)

def parse_memory_register_bin(bin,start_addr,size):
    """注册一段内存
    Args
        bin: 内存一段binary
        start_addr: 内存起始地址
        size: 内存大小
    return
    """
    _g_parse_memory_manager.register_bin(bin,start_addr,size)

def parse_get_symbol_info(name):
    """获取symbolinfo
    Args
        name: 符号名
    return
        obj:成功,Sysmbol类型的obj
        None:获取失败
    """
    if _g_hook_functions.get_symbol_info==None:
        assert(False)
        return None
    return _g_hook_functions.get_symbol_info(name)

def parse_get_u32_symbol_val(name):
    """获取值
    Args
        name: 符号名
    return
        int:成功,获取到的值
        None:获取失败
    """
    if _g_hook_functions.get_u32_symbol_val==None:
        assert(False)
        return None
    return _g_hook_functions.get_u32_symbol_val(name)

def parse_get_symbol_addr(name):
    """获取符号地址
    Args
        name: 符号名
    return
        int:成功,获取到的值
        None:获取失败
    """
    if _g_hook_functions.get_symbol_addr==None:
        assert(False)
        return None
    return _g_hook_functions.get_symbol_addr(name)

def parse_addr_2_function_name(addr):
    if _g_hook_functions.addr_2_function_name==None:
        assert(False)
        return None
    return _g_hook_functions.addr_2_function_name(addr)

def parse_addr_in_2_function_name(addr):
    if _g_hook_functions.addr_in_2_function_name==None:
        assert(False)
        return None
    return _g_hook_functions.addr_in_2_function_name(addr)

def uapi_register_function(name,function):
    if name=="get_symbol_info":
        _g_hook_functions.get_symbol_info = function
    elif name=="get_u32_symbol_val":
        _g_hook_functions.get_u32_symbol_val = function
    elif name=="get_symbol_addr":
        _g_hook_functions.get_symbol_addr=function
    elif name=="addr_2_function_name":
        _g_hook_functions.addr_2_function_name = function
    elif name=="addr_in_2_function_name":
        _g_hook_functions.addr_in_2_function_name = function
    else:
        assert(False)

_g_parse_memory_manager = _ParseMemoryManager()
_g_basic_ctrl=_BasicCtrl()
_g_hook_functions =_HookFunctions()