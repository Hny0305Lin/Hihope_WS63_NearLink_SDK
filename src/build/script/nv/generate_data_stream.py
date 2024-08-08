#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.

"""
* Description: NV binary create.
* Create: 2020-3-10
"""
from ctypes import *
import os
import re
import ctypes
import sys
import pycparser

from parse_msgdefs import Visitor
from conf_parser import ParserError

nextIsBitfield = 0
bitField = 0
bitLength = 0
lastByteLength = 0
totalByteLen = 0
baseByteSize = 0
# type define
class generate_data_stream:
    def __init__(self):
        self.v = Visitor()

    def phase_etypes(self, file):
        code = pycparser.parse_file(file)
        tmp = Visitor()
        tmp.visit(code)
        self.v.typedefs.update(tmp.typedefs)

    def is_dec(self, s):
        try:
            int(s)
            return True
        except ValueError:
            pass
        return False

    def is_hex(self, s):
        try:
            int(s, 16)
            return True
        except ValueError:
            pass
        return False

    def byte_len(self, value):
        if value < 256:
            return 1
        elif value < 65536:
            return 2
        elif value < 4294967296:
            return 4
        else:
            return 8

# value 的几种case需要分别考虑
#1.直接数字
#2.直接字符串
#3.枚举
#4.简单数组
#5.结构体
#6.结构体数组
#7.指针
#8.指针数组
    def get_value(self, value_str):
        if self.is_dec(value_str):
            value = int(value_str)
            return value
        elif self.is_hex(value_str):
            value = int(value_str, 16)
            return value
        return None

    def get_char_value(self, value_str):
        if type(value_str) is int:
            return value_str
        if len(value_str) == 1:
            return ord(value_str)
        return None

    def get_value_for_array(self, value_str):
        if '[' not in value_str or ']' not in value_str:
            return None
        value_line_list = list(filter(None, re.split(r'[;,\s\"\[\]]\s*', value_str)))
        return value_line_list

    def get_value_for_char_array(self, value_str):
        if '\"' not in value_str:
            return None
        value_str = value_str.replace('\"','')
        value_line_list = [x for x in value_str]
        return value_line_list

    def get_value_str(self, value):
        value_line_list = list(filter(None, value.split('\n')))
        value_list = []
        for i in range(len(value_line_list)):
            tmp_list = list(filter(None, value_line_list[i].split(' = ')))
            value_list.append(tmp_list[1])
        return value_list

    def get_enum_value(self, enum_fields, enum_str):
        if 1:
            enum_value = enum_fields.members.get(enum_str)
            if enum_value is None:
                msg = "[error] [%s] not a enum value, please check!!" % enum_str
                raise ParserError(msg)
            return enum_value

        for field in enum_fields.members:
            if enum_str == field:
                return enum_fields.members[field]

    def get_bitfield_value(self, fields, typename, value):
        global nextIsBitfield
        global bitField
        global bitLength
        global lastByteLength
        global totalByteLen
        global baseByteSize

        bufferData = b''
        bitsize = self.get_value(fields.bitsize)
        bitLength += bitsize

        if value.bit_length() > bitsize:
            msg = "[error] [%s]'s value exceeds its bit width!!" % typename
            raise ParserError(msg)
        if bitLength == bitsize:
            baseByteSize = sizeof(fields)
            totalByteLen = 0
        writeLen = 0
        # 前后类型相同，不压缩处理
        if bitLength > bitsize and sizeof(fields) == lastByteLength:
            if bitLength > baseByteSize * 8:
                writelen = max(self.byte_len(bitField), lastByteLength)
                bufferData += bitField.to_bytes(writelen, byteorder="little", signed=True) if bitField < 0 \
                    else bitField.to_bytes(writelen, byteorder="little", signed=False)
                totalByteLen += writelen
                bitField = 0
                bitLength = bitsize  # 记录未写入的bit
        # 前后类型不同，考虑压缩场景
        elif bitLength > bitsize and sizeof(fields) != lastByteLength:
            baseByteSize = sizeof(fields) if sizeof(fields) > lastByteLength else lastByteLength
            if bitLength > baseByteSize * 8: # 不压缩处理
                writelen = max(self.byte_len(bitField), lastByteLength)
                bufferData += bitField.to_bytes(writelen, byteorder="little", signed=True) if bitField < 0 \
                    else bitField.to_bytes(writelen, byteorder="little", signed=False)
                totalByteLen += writelen
                # 对齐处理
                if totalByteLen % sizeof(fields) != 0:
                    bitField = 0
                    alignByteLen = sizeof(fields) - totalByteLen if sizeof(fields) > totalByteLen \
                        else totalByteLen - sizeof(fields)
                    bufferData += bitField.to_bytes(alignByteLen, byteorder="little", signed=True) if bitField < 0 \
                        else bitField.to_bytes(alignByteLen, byteorder="little", signed=False)
                    totalByteLen += alignByteLen
                bitField = 0
                bitLength = bitsize
            if totalByteLen % baseByteSize != 0:  # 非对齐场景，不压缩
                writelen = max(self.byte_len(bitField), lastByteLength)
                bufferData += bitField.to_bytes(writelen, byteorder="little", signed=True) if bitField < 0 \
                    else bitField.to_bytes(writelen, byteorder="little", signed=False)
                totalByteLen += writelen
                bitField = 0
                bitLength = bitsize
        lastByteLength = sizeof(fields)
        if bitLength > bitsize:
            bitField = (bitField | (value << (bitLength - bitsize)))
        else:
            bitField = value

        # print("bitsize: ",bitsize)
        # print("value: ",value)
        # print("lastByteLength: ",lastByteLength)
        # print("bitLength: ",bitLength)
        # print("totalByteLen :", totalByteLen)
        # print("bitField: ",bitField)
        if bitLength == (sizeof(fields) * 8):
            writelen = max(self.byte_len(bitField), sizeof(fields))
            bufferData += bitField.to_bytes(writelen, byteorder="little", signed=True) if bitField < 0 \
                else bitField.to_bytes(writelen, byteorder="little", signed=False)
            totalByteLen += writelen
            bitField = 0
            bitLength = 0
            return bufferData
        if nextIsBitfield == 0:
            # 后面非位域，数据输出。如果后面嵌套结构体，结构体起始仍为位域，仍需要考虑压缩
            writelen = max(self.byte_len(bitField), sizeof(fields))
            bufferData += bitField.to_bytes(writelen, byteorder="little", signed=True) if bitField < 0 \
                else bitField.to_bytes(writelen, byteorder="little", signed=False)
            totalByteLen += writelen
            bitField = 0
            bitLength = 0
            if totalByteLen % baseByteSize != 0:
                bitField = 0
                alignByteLen = baseByteSize - (totalByteLen % baseByteSize)
                bufferData += bitField.to_bytes(alignByteLen, byteorder="little", signed=True) if bitField < 0 \
                    else bitField.to_bytes(alignByteLen, byteorder="little", signed=False)
        # print("bufferData: ", bufferData)
        return bufferData

    def print_type(self, typename, value, isBaseType, isEnum, isSomeKindOfArray, isUnion, isPointer):
        print("%s, value: %s, isBaseType :" % (typename, value), isBaseType) if isBaseType else None
        print("%s, value: %s, isEnum :" % (typename, value), isEnum) if isEnum else None
        print("%s, value: %s, isSomeKindOfArray :" % (typename, value), isSomeKindOfArray) if isSomeKindOfArray else None
        print("%s, value: %s, isUnion :" % (typename, value), isUnion) if isUnion else None
        print("%s, value: %s, isPointer :" % (typename, value), isPointer) if isPointer else None


    def recursion_parse(self, fields, type_name, value):
        global nextIsBitfield
        # print('recurse field: ', fields)
        # print('recurse type_name: ', type_name)
        # print('recurse value: ', value)
        isBitfield = hasattr(fields, "bitsize")
        isBaseType = not hasattr(fields, "_fields_")
        isEnum = hasattr(fields, "members")
        isSomeKindOfArray = issubclass(fields, ctypes.Array)
        isUnion = isinstance(fields, ctypes.Union)
        isPointer = hasattr(fields, "contents")
        #self.print_type(type_name, value, isBaseType, isEnum, isSomeKindOfArray, isUnion, isPointer)

        if ((isBaseType and not isSomeKindOfArray) or isEnum or isPointer) and \
            (type(value) is list or type(value) is dict):
                msg = "[error] [%s] is not a array or a structrue, the value cannot be a list or a dict!!" % type_name
                raise ParserError(msg)

        if not ((isBaseType and not isSomeKindOfArray) or isEnum or isPointer) and \
            not (type(value) is list or type(value) is dict):
                msg = "[error] [%s] is a array or a structrue, the value must be a list or a dict!!" % type_name
                raise ParserError(msg)

        if isUnion and not isSomeKindOfArray and type(value) is list:
            msg = "[error] [%s] is a union, the value must be a hex or int or a dict!!" % type_name
            raise ParserError(msg)

        buffer = b''
        if isEnum:
            buffer += self.get_enum_value(fields, value).to_bytes(sizeof(fields), byteorder="little")
            # print("buffer: ", buffer)
            return buffer

        if isBaseType and not isSomeKindOfArray:
            # print("base size: ", sizeof(fields))
            # print("fileds: ", fields)
            # print("type: ", type(fields))
            if sizeof(fields) == 1 and \
               isinstance(value, str) and \
               not value.startswith('0x') and not value.startswith('0X'):
                value = self.get_char_value(value)
            else:
                value = self.get_value(value)

            if isBitfield:
                buffer += self.get_bitfield_value(fields, type_name, value)
            else:
                buffer += value.to_bytes(sizeof(fields), byteorder="little", signed=True) if value < 0 \
                    else value.to_bytes(sizeof(fields), byteorder="little", signed=False)
            # print("buffer: ", buffer)
            return buffer

        if isUnion and not isSomeKindOfArray:
            # print("union size: ", sizeof(fields))
            # union要么指定成员赋值，要么直接整体赋值,不能是list,list无法知道对具体哪个成员赋值
            if type(value) is not dict:
                buffer += self.get_value(value).to_bytes(sizeof(fields), byteorder="little")
            else:
                # 增加枚举结构的解析
                pass
            # print("buffer: ", buffer)
            return buffer

        if isSomeKindOfArray:
            if type(value) is not list:
                msg = "[error] [%s] is a array, the value must be a list!!" % type_name
                raise ParserError(msg)

            # print("array type: ", fields._type_)
            # print("array length: ", fields._length_)
            idx = 0
            for val in value:
                buffer += self.recursion_parse(fields._type_, '%s[%d]' % (type_name, idx), val)
                idx += 1
            # print("sizeof array [%s]: " % type_name, sizeof(fields))
            if len(buffer) > sizeof(fields):
                msg = "[error] the value is oversized the array: [%s]!!" % type_name
                raise ParserError(msg)
            buffer += bytearray(sizeof(fields) - len(buffer))
            # print("buffer: ", buffer)
            return buffer

        all_types = fields._fields_
        # print('all_types in struct [%s] : ' % type_name, all_types)
        if type(value) is list:
            typesLen = len(all_types)
            typesIdx = 0
            for (item, item_class) in all_types:
                if len(value) == 0:
                    buffer += bytearray(sizeof(item_class))
                    continue
                if typesIdx + 1 < typesLen:
                    nextField = all_types[typesIdx + 1][1]
                    nextIsBitfield = hasattr(nextField, "bitsize")
                else:
                    nextIsBitfield = 0
                buffer += self.recursion_parse(item_class, item, value[0])
                del(value[0])
                typesIdx += 1
            if len(value) != 0:
                msg = "[error] the value is not match the type: [%s]!!" % type_name
                raise ParserError(msg)
            return buffer

        if type(value) is dict:
            typesLen = len(all_types)
            typesIdx = 0
            for (item, item_class) in all_types:
                item_value = value.get(item)
                if item_value is None:
                    buffer += bytearray(sizeof(item_class))
                    continue
                if typesIdx + 1 < typesLen:
                    nextField = all_types[typesIdx + 1][1]
                    nextIsBitfield = hasattr(nextField, "bitsize")
                else:
                    nextIsBitfield = 0
                buffer += self.recursion_parse(item_class, item, item_value)
                typesIdx += 1
            return buffer

    def generate(self, struct_name, value):
        fields = self.v.typedefs.get(struct_name)
        # print("struct: %s, value: " %struct_name, value)
        # print('types: ', self.v.typedefs)
        if fields is None:
            msg = "[error] not found the type [%s]!" % struct_name
            raise ParserError(msg)
        buffer = self.recursion_parse(fields, struct_name, value)
        return buffer, len(buffer)


#DEBUG

if __name__=="__main__":
    class g_env:
        flash_size = 0x3000
        protocolKvIndex = 0
        appKvIndex = 0x1000
        secureKvindex = 0x2000
        KV_STORE_DIR = {'security' : 0 ,'protocol' : 1 ,'application' : 2 ,'asset' : 3 }
        KV_STATUS_DIR = {'alive':0xffff,'reserved':0,'deprecated':1}
        KV_PAGE_ID = {'security' : 0xcb7e ,'protocol' : 0xda81 ,'application' : 0x254d ,'backup' : 0x34b2}

    g_kv_pairs = {
        'yml_test_eg' : {
            'value' : {
                'num_supported_bands' : 1,
                'band_ids' :[8, 7, 6]
            },
            'permanence': False
        },
        'yml_test_eg2_mixed' : {
            'value' : {
                'param1' : 1,
                'param2' : [[1,2,], [3,4], [5]]
            },
            'permanence': False
        },
        'yml_test_eg2' : {
            'value' : [1, [[1,2,], [3,4]]],
            'permanence': False
        },

        'test_nv_type_nest_deep' :{
            'value' : [1, "C", 0,
                        [[1, 0, 0x2222, [["TEST_NV_ENUM_1"], ["TEST_NV_ENUM_2"]]],
                        [3, 0, 0x4444, [["TEST_NV_ENUM_3"], ["TEST_NV_ENUM_4"]]]]
            ],
            'permanence': False
        },

        'test_nv_type_nest_deep____pure_value' :{
            'value' : [1, 0, "C",
                        [[1, 0x2222, 0, [[0x1111], [0x2222]]]],
                        [3, 0x4444, 0, [[0x33334444], [0x5555]]]
            ],
            'permanence': False
        },
        'test_nv_type_nest_deep___dict' :{
            'value' : [1, [[1,2,], [3,4]]],
            'permanence': False
        },
        'test_nv_type_nest_deep___mixed' :{
            'value' : [1, [[1,2,], [3,4]]],
            'permanence': False
        }
    }

    struct_name = sys.argv[1]
    etypes = sys.argv[2]
    test = generate_data_stream()
    test.phase_etypes(etypes)
    nv_file_Bin = bytearray(g_env.flash_size)
    for i in range(0, g_env.flash_size):
        nv_file_Bin[i] = 0xFF
    # SetKvPageHead(nv_file_Bin)

    for keyValueItem in g_kv_pairs:
        if keyValueItem != struct_name:
            continue
        value,value_len = test.generate(struct_name, g_kv_pairs[struct_name]['value'])
        print("value: ", value)
        print("value_len: ", value_len)
        with open('nv.bin', 'wb') as f:
            f.write(value)
        break
