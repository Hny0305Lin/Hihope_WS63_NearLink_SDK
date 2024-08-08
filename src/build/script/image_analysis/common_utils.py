#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.

from os import listdir
from os.path import isdir, exists
import re
import subprocess
import sys


ELF_SYMBOL_RULE = r'^[0-9a-zA-Z]{8}'


class CommonUtils:
    symbol_key_arr = []
    symbol_data = {}

    @staticmethod
    def get_prefix(path):
        data = {
            'prefix': '',
            'environment': ''
        }
        if not exists(path) or not isdir(path):
            print(f'compiler path not find or not a directory: {path}')
            raise FileExistsError()
        cp_files = listdir(path)

        objdump_win_sign = '-objdump.exe'
        objdump_linux_sign = '-objdump'
        has_compiler = False

        for file_name in cp_files:
            if file_name.endswith(objdump_win_sign):
                data['prefix'] = file_name[0: file_name.rindex(objdump_win_sign)]
                data['environment'] = 'windows'
                has_compiler = True
                break
            elif file_name.endswith(objdump_linux_sign):
                data['prefix'] = file_name[0: file_name.rindex(objdump_linux_sign)]
                data['environment'] = 'linux'
                has_compiler = True
                break
        if not has_compiler:
            print(f'Incorrect compiler path: {path}')
            raise Exception()

        return data

    @staticmethod
    def check_elf_file(file_path):
        with open(file_path, 'rb') as f:
            elf_ident = f.read(16)
            elf_str = ''
            for i in elf_ident:
                elf_str += hex(i)[2:]
            return elf_str.find('7f454c46') >= 0

    @staticmethod
    def get_obj_dump_info(obj_dump_path, arg, elf_file_path, is_return_arr=True, is_convert=True):
        result = []
        if not exists(obj_dump_path):
            return result
        cmd = [obj_dump_path, arg, elf_file_path]
        data = CommonUtils.exec_cmd(cmd, is_convert)
        if data:
            if is_return_arr:
                result = data.strip().split('\n')
            else:
                result.append(data)
        return result

    @staticmethod
    def exec_cmd(cmd, is_convert=True):
        try:
            p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            out, _ = p.communicate(timeout=120)
            if is_convert:
                out = out.decode()
            return out
        except subprocess.TimeoutExpired as e:
            print('cmd timeouot')
            raise SystemError() from e
        except SystemError as e:
            print('cmd error')
            raise SystemError() from e

    @staticmethod
    def get_symbol_data(obj_dump_path='', elf_file_path=''):
        if not obj_dump_path or not exists(obj_dump_path):
            raise FileExistsError(f'objdump path not find: {obj_dump_path}')
        if not elf_file_path or not exists(elf_file_path):
            raise FileExistsError(f'elf path not find: {elf_file_path}')
        CommonUtils.symbol_data = {}
        CommonUtils.symbol_key_arr = []
        res = CommonUtils.get_obj_dump_info(obj_dump_path, '-t', elf_file_path)
        for item in res:
            line = re.sub(r'\s+', ' ', item)
            if re.search(ELF_SYMBOL_RULE, line):
                data_arr = line.split(' ')
                section_name = data_arr[3]
                symbol_name = data_arr[-1]
                type_name = data_arr[2]
                if section_name != symbol_name and type_name == 'F':
                    CommonUtils.symbol_data[data_arr[0]] = symbol_name
        for keys in CommonUtils.symbol_data.keys():
            hex_str = '0x' + keys
            CommonUtils.symbol_key_arr.append(int(hex_str, 16))

        if len(CommonUtils.symbol_key_arr) == 0:
            return
        CommonUtils.symbol_key_arr.sort(reverse=True)

    @staticmethod
    def get_function_name(num):
        index = 0
        d_value = sys.maxsize
        if num.find('0x') < 0:
            num = '0x' + num
        for i, symbol_key_val in enumerate(CommonUtils.symbol_key_arr):
            new_value = int(num, 16) - symbol_key_val
            if d_value >= new_value >= 0:
                if new_value == d_value and symbol_key_val < CommonUtils.symbol_key_arr[index]:
                    continue
                index = i
                d_value = new_value
                break
        if CommonUtils.symbol_key_arr[index]:
            addr = hex(CommonUtils.symbol_key_arr[index])
            return {
                'funcname': CommonUtils.symbol_data['%08x' % int(addr, 16)],
                'funcaddr': '0x%08x' % int(addr, 16)
            }
        return {
            'funcname': '',
            'funcaddr': ''
        }

    @staticmethod
    def read_uint16_le(data, offset=0):
        return int.from_bytes(data[offset:offset + 2][::-1], byteorder='big')

    @staticmethod
    def read_uint32_le(data, offset=0):
        return int.from_bytes(data[offset:offset + 4][::-1], byteorder='big')
