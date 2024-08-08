#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.

import json
import os
import sys
from os.path import join, isdir, abspath, exists, isabs, sep, normpath
import re

from common_utils import CommonUtils

work_dir = os.path.split(os.path.realpath(__file__))[0]
root_dir = os.path.join(work_dir, '..', '..', '..')
sys.path.append(os.path.join(root_dir, 'build'))
sys.path.append(os.path.join(root_dir, 'build', 'script'))
sys.path.append(os.path.join(root_dir, 'build', 'config'))
from enviroment import TargetEnvironment
from utils.build_utils import output_root

map_start_rule = r'Memory Configuration'
map_end_rule = r'Linker script and memory map'
# map文件中Cross Reference Table项不涉及镜像大小，结束map文件解析
map_file_size_end = r"Cross Reference Table"
elf_setion_rule = r'^\s[0-9]'
elf_symbol_rule = r'^[0-9]+'
file_size_content_rule = r'^(\s*\.[a-zA-Z]+)'


class BuildDataAnalyzer:
    _elf_path = ''
    _map_path = ''
    _build_config_path = ''
    _module_config = {}
    _compiler_path = ''
    _prefix = 'arm-none-eabi'
    _section_keys = []
    _environment = ''

    section_data = {}
    region_data = []
    size_data = {}
    elf_size_data = {}
    module_data = {}
    folder_data = {}
    rom_addr = {
        'start': 0,
        'end': 0
    }
    rom_parts = {}
    is_rom = True
    last_symbol = []
    last_line = ''
    is_unknown = False
    cur_section = ''
    cur_section_size = ''

    def __init__(self, paths):
        print('++++++++++start buildAnalyzer++++++++++')
        data = CommonUtils.get_prefix(paths['compiler_path'])
        self._prefix = data['prefix']
        self._environment = data['environment']
        for key, value in paths.items():
            print(f"{key}:{value}")
        self._elf_path = paths['elf_path']
        self._map_path = paths['map_path']
        self._compiler_path = paths['compiler_path']
        self._build_config_path = paths['build_config_path']
        self._remove_sections_config_path = paths['remove_sections_config_path']
        self._result_save_path = paths['result_save_path']

        try:
            save_dir = os.path.dirname(self._result_save_path)
            if not os.path.isdir(save_dir):
                os.makedirs(save_dir)
        except Exception as e:
            raise Exception(f"Failed to Create Save Directory,{e}")
        self._root_dir = os.getcwd()
        self._json_path = join(self._root_dir, 'memoryDetails.json')
        self._image_remove_section = []

    def do_build_analysis(self, paths):
        try:
            with open(self._remove_sections_config_path, 'r') as rv_f:
                for line in rv_f.readlines():
                    l_s = line.strip()
                    self._image_remove_section.append(l_s)

            self._module_config = self._get_build_json(paths.get('build_config_path', ""))
            json_obj = self._parse_map_file()
            self.result_filter(json_obj)
            # with open(self._result_save_path, 'w') as result_f:
            #     json.dump(json_obj, result_f, indent=4)
            print('++++++++++end buildAnalyzer++++++++++')
        except Exception as e:
            raise Exception('buildAnalyzer error') from e

    def result_filter(self, json_cont):
        """
        结果过滤
        """
        new_json = json_cont.get("moduleData", "")
        if not new_json:
            raise Exception("ImageAnalysis Failed")
        with open(self._result_save_path, 'w') as res_f:
            res_f.write('<h1>Image Analysis</h1>')
            res_f.write('<table border="1" width = "40%" cellspacing="0" cellpadding="0" align="left">')
            line = 0
            for key, value in new_json.items():
                if line == 0:
                    head_line = '<tr>'
                    for v in value:
                        if v == 'parent':
                            continue
                        head_line = head_line + '<th>' +  v + '</th>'
                    head_line = head_line + '<tr>\n'
                    res_f.write(head_line)
                    line = line + 1
                new_line = '<tr>'
                if value["parent"]:
                    continue
                for v in value:
                    if value[v] != '':
                        new_line = new_line + '<td>' + str(value[v]) + '</td>'
                new_line = new_line + '</tr>\n'
                res_f.write(new_line)

    @staticmethod
    def _get_build_json(file_path):
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                res = f.read()
                json_data = json.loads(s=res)
                return json_data
        except FileExistsError as e:
            print('file exists error')
            raise FileExistsError() from e

    def _parse_map_file(self):
        try:
            suffix = self._prefix + '-size.exe'
            if self._environment == 'linux':
                suffix = self._prefix + '-size'
            file_size_path = join(self._compiler_path, suffix)
            if not exists(file_size_path):
                print(f'size path not find: {file_size_path}')
                raise FileExistsError()
            size_arr = self.get_file_size(file_size_path, '-A', self._elf_path)
            size_arr.pop()
            size_arr.pop(0)
            self._set_elf_size_data(size_arr)
            json_obj = self._read_file()
            return json_obj
        except SystemError as e:
            print('map parse error')
            raise SystemError() from e

    def _set_elf_size_data(self, size_arr):
        order_section_keys = [".text", ".data", ".bss", ".reboot_retention"]
        order_elf_size_data = dict()
        for key in order_section_keys:
            order_elf_size_data[key] = {'addr': None, 'size': None}

        for item in size_arr:
            elf_key, elf_size, elf_addr = re.sub(r'\s+', ' ', item).split(' ')[:3]
            if elf_key.startswith('.') and (elf_size != '0' or elf_addr != '0'):
                order_elf_size_data[elf_key] = {'addr': int(elf_addr), 'size': int(elf_size)}
                if elf_key not in order_section_keys:
                    order_section_keys.append(elf_key)
        self._section_keys = list()
        for k, v in order_elf_size_data.items():
            if v.get("size", None) is None:
                continue
            if k in self._image_remove_section:
                continue
            self.elf_size_data[k] = v
            self._section_keys.append(k)

    def _read_file(self):
        try:
            self._read_file_to_arr()
            json_obj = self._parse_elf_file(self._elf_path)
            return json_obj
        except Exception as e:
            print('elf parse error')
            raise Exception() from e

    def _parse_elf_file(self, elf_path):
        suffix = self._prefix + '-objdump.exe'
        if self._environment == 'linux':
            suffix = self._prefix + '-objdump'
        obj_dump_path = join(self._compiler_path, suffix)
        if not obj_dump_path or not exists(obj_dump_path):
            print('Please configure the correct compiler type and compiler path')
            raise FileExistsError()
        try:
            data = CommonUtils.get_obj_dump_info(obj_dump_path, '-h', elf_path)
            self._set_section(data)
            json_obj = self._get_symbol_data(obj_dump_path, elf_path)
            return json_obj
        except SystemError as e:
            print('elf parse error')
            raise SystemError() from e

    def _set_section(self, data):
        for key, value in enumerate(data):
            line = re.sub(r'\s+', ' ', value)
            if re.search(elf_setion_rule, line):
                data_arr = line.split(' ')
                if len(data_arr) > 2 and data_arr[2] in self._image_remove_section:
                    continue
                next_line = ''
                if self._prefix != 'llvm' and key < len(data) - 1:
                    next_line = data[key + 1]
                self._set_section_data(data_arr, next_line)

    def _set_section_data(self, data_arr, next_line):
        size = int('0x' + data_arr[3], 16)
        vma = int('0x' + data_arr[4], 16)
        if vma > 0 or size != 0:
            lma = 0
            if next_line:
                lma = 0 if next_line.find('LOAD') < 0 else int('0x' + data_arr[5], 16)
            elif data_arr[5]:
                lma = int('0x' + data_arr[5], 16)
            region_name = []
            for region_item in self.region_data:
                start_addr = int(region_item['startAddr'], 16)
                end_addr = int(region_item['endAddr'], 16)
                if (start_addr <= vma < end_addr) or (lma != 0 and start_addr <= lma < end_addr):
                    region_name.append(region_item['name'])
                    region_item['used'] += size
            section = {
                'regionName': region_name,
                'runAddr': '0x' + data_arr[4],
                'loadAddr': '0x' + data_arr[5] if lma else '',
                'size': self._addr_to_size('0x' + data_arr[3]),
                'sizeOri': '0x' + data_arr[3],
                'children': []
            }
            self.section_data[data_arr[2]] = section

    def _get_symbol_data(self, obj_dump_path, elf_path):
        try:
            data = CommonUtils.get_obj_dump_info(obj_dump_path, '-t', elf_path)
            for item in data:
                line = re.sub(r'\s+', ' ', item)
                if re.search(elf_symbol_rule, line):
                    self._set_section_symbol_data(line)
        except SystemError as e:
            print('data parse error')
            raise SystemError() from e

        for region in self.region_data:
            used = self._addr_to_size(hex(region['used']))
            try:
                usage = region['used'] / region['size']
                region['usage'] = '%.2f' % (usage * 100) + '%'
            except ZeroDivisionError:
                region['usage'] = '0.00%'
            region['free'] = self._addr_to_size(hex(region['size'] - region['used']))
            region['used'] = used
            region['size'] = self._addr_to_size(hex(region['size']))
        json_obj = {
            'region': self.region_data,
            'details': self.section_data,
            'sizeData': self.size_data,
            'moduleData': self.module_data,
            'folderData': self.folder_data
        }
        return json_obj

    def _set_section_symbol_data(self, line):
        data_arr = line.split(' ')
        section_name = data_arr[3]
        symbol_name = data_arr[-2]
        type_name = data_arr[2]
        if len(data_arr[-1]) != 0:
            symbol_name = data_arr[-1]
        if section_name != symbol_name and section_name in self.section_data:
            section = self.section_data[section_name]
            vma = int('0x' + data_arr[0], 16)
            lma = 0
            if section['loadAddr']:
                lma = int(section['loadAddr'], 16) - int(section['runAddr'], 16) + vma
            symbol = {
                'name': symbol_name,
                'runAddr': '0x' + data_arr[0],
                'loadAddr': hex(lma) if lma else '',
                'size': self._addr_to_size('0x' + data_arr[4]),
                'sizeOri': '0x' + data_arr[4],
                'location': '',
                'type': type_name
            }
            self.section_data[section_name]['children'].append(symbol)

    @staticmethod
    def _addr_to_size(addr):
        size = int(addr, 16)
        if size < 1024:
            return f'{size} B'
        else:
            return '%.2f' % (size / 1024) + 'KB'

    @staticmethod
    def get_file_size(file_size_path, arg, elf_file_path):
        cmd = [file_size_path, arg, elf_file_path]
        data = CommonUtils.exec_cmd(cmd)
        return data.strip().split('\n')

    def _read_file_to_arr(self):
        is_start = False
        is_end = False
        with open(self._map_path, 'r') as f:
            line = f.readline()
            while line:
                line = line.strip()
                if line == map_file_size_end:
                    break
                if re.match(map_start_rule, line):
                    is_start = True
                elif re.match(map_end_rule, line):
                    is_end = True
                elif is_start and not is_end:
                    self._set_region_data(line)
                elif is_end:
                    self._parse_size_data(line)
                line = f.readline()

    def _set_region_data(self, line):
        if len(line) > 0 and not line.startswith('Name') and not line.startswith('*default*'):
            line_arr = re.sub(r'\s+', ' ', line).split(' ')
            end_addr = '0x%08x' % (int(line_arr[1], 16) + int(line_arr[2], 16))
            if len(line_arr) >= 3:
                region = {
                    'name': line_arr[0],
                    'startAddr': line_arr[1],
                    'endAddr': end_addr,
                    'size': int(line_arr[2], 16),
                    'sizeOri': line_arr[2],
                    'used': 0,
                    'usage': 0
                }
                self.region_data.append(region)
                name_sp = line_arr[0].split("_")
                if 'FLASH' in name_sp or "ROM" in name_sp:
                    self.rom_parts[line_arr[0]] = {'start': int(region['startAddr'], 16),
                                                   'end': int(region['endAddr'], 16)}

    def _parse_size_data(self, line):
        is_need_parse = False
        file_size_content_end_rule = re.compile(r'0x[0-9a-fA-F]{8,16}\s+0x[0-9a-fA-F]{1,16}\s+')
        file_size_symbol_rule = re.compile(r'^(\s*0x[0-9a-fA-F]{8,16})\s*\w+')
        file_rule = re.compile(r'^(\s+\*fill\*)\s+0x[0-9a-fA-F]{8,16}\s+0x[0-9a-fA-F]{1,16}')
        is_content_rule = len(line) > 0 and re.search(file_size_content_rule, line) is not None
        is_end_rule = line.find('.o') > 0 and re.search(file_size_content_end_rule, line) is not None
        if is_content_rule and is_end_rule:
            is_need_parse = True
        elif is_content_rule:
            self.last_line = line
            is_need_parse = False
        elif len(line) > 0 and is_end_rule:
            line = self.last_line + ' ' + line
            self.last_line = ''
            is_need_parse = True
        elif re.search(file_rule, line) is not None:
            self.last_line = ''
            is_need_parse = True
        else:
            self.last_line = ''
        if is_need_parse:
            if self.is_unknown and len(self.last_symbol) > 1 and self.last_symbol[0]['addr'] in self.size_data:
                self._reset_symbol_size_data()
            self.is_unknown = False
            line = line.strip()
            line_arr = re.sub(r'\s+', ' ', line).split(' ')
            for rv_s in self._image_remove_section:
                if line_arr[0].startswith(rv_s):
                    return
            addr = line_arr[1]
            self._get_cur_section(addr)
            if self.cur_section:
                self._calculate_size_data(line_arr, addr)
        elif self.is_unknown and re.search(file_size_symbol_rule, line):
            line_arr = re.sub(r'\s+', ' ', line).split(' ')
            addr = line_arr[0]
            name = line_arr[1]
            if len(self.last_symbol) == 1 and self.last_symbol[0]['addr'] != addr:
                self.is_unknown = False
                self.last_symbol = []
            else:
                self.last_symbol.append({
                    'addr': addr,
                    'name': name
                })

    def _calculate_size_data(self, line_arr, addr):
        cur_size = int(line_arr[2], 16)
        # .o或者.obj文件路径相对路径，包含括号里，可能没有.a文件一定有.o/.obj文件
        # 如 ../../../../interim_binary/brandy/libs/media/evb_standard/libaudio_player.a(liteplayer.cpp.obj)
        nor_path = normpath(line_arr[3])
        a_file_index = nor_path.find('.a(')
        if nor_path and a_file_index > 0:
            a_file_path = nor_path[0: a_file_index + 2]
            a_file_dir = os.path.dirname(a_file_path)
            a_file_name = os.path.basename(a_file_path)
            o_file_name = nor_path[a_file_index + 3: len(nor_path) - 1]
            o_file_dir = a_file_path
            # 统计.a文件大小
            param_size = dict(name=a_file_name,
                              parent='',
                              path=a_file_dir,
                              obj_name=o_file_name,
                              lib_name=a_file_name,
                              cur_size=cur_size)
            self._set_size_data(param_size)
            # 统计.o/.obj文件大小
            param_size = dict(name=o_file_name,
                              parent=a_file_name,
                              path=o_file_dir,
                              obj_name=o_file_name,
                              lib_name=a_file_name,
                              cur_size=cur_size)
            self._set_size_data(param_size)

        else:
            o_file_name = os.path.basename(nor_path)
            o_file_dir = os.path.dirname(nor_path)
            param_size = dict(name=o_file_name, parent='', path=o_file_dir,
                              obj_name=o_file_name, lib_name='', cur_size=cur_size)
            self._set_size_data(param_size)

        # 函数
        child = line_arr[0] + ':' + addr
        self._set_addr_data(addr)
        if cur_size != 0:
            param_size.update({'name': child, 'parent': o_file_name, 'path': ''})
            self._set_size_data(param_size, True)

    def _set_addr_data(self, addr):
        if addr in self.size_data:
            old_size = self.size_data[addr][self.cur_section_size]
            old_lib = self.size_data[addr]['lib']
            old_parent = self.size_data[addr]['parent']
            old_module_name = self.size_data[addr]['moduleName']
            old_component_name = self.size_data[addr]['componentName']
            if old_lib in self.size_data:
                self.size_data[old_lib][self.cur_section_size] = self.size_data[
                                                                     old_lib][self.cur_section_size] - old_size

            if old_parent in self.size_data:
                self.size_data[old_parent][self.cur_section_size] = self.size_data[
                                                                        old_parent][self.cur_section_size] - old_size
                self._set_old_parent_size(old_parent, old_size)

            if old_module_name in self.module_data:
                self.module_data[old_module_name][self.cur_section_size] = self.module_data[
                                                                               old_module_name][
                                                                               self.cur_section_size] - old_size

            if old_component_name in self.module_data:
                self.module_data[old_component_name][self.cur_section_size] = self.module_data[
                                                                                  old_component_name][
                                                                                  self.cur_section_size] - old_size

    def _set_old_parent_size(self, cur_parent, cur_size):
        if self.size_data[cur_parent]['parent'] == '' and 'path' in self.size_data[cur_parent]:
            path_list = self.size_data[cur_parent]['path'].split(sep)
            temp_folder = self.folder_data
            for path in path_list:
                if path in temp_folder and self.cur_section_size in temp_folder[path]:
                    temp_folder[path][self.cur_section_size] = temp_folder[path][self.cur_section_size] - cur_size
                temp_folder = temp_folder[path]

    def _set_size_data(self, param_size, is_symbol=False):
        # .a/.o/.obj文件的索引字符串：全路径
        name = param_size.get('name')
        # .o/.obj是否有父节点.a文件
        parent = param_size.get('parent')
        # .o/.obj文件名称
        obj_name = param_size.get('obj_name')
        # .a文件名称
        lib_name = param_size.get('lib_name')
        # .o/.obj文件大小
        cur_size = param_size.get('cur_size')
        # .a/.o/.obj文件的目录
        file_dir = param_size.get('path')
        if name in self.size_data and not is_symbol:
            if not self.size_data[name][self.cur_section_size]:
                self.size_data[name][self.cur_section_size] = cur_size
            else:
                self.size_data[name][self.cur_section_size] = self.size_data[name][self.cur_section_size] + cur_size
        elif name:
            symbol_name = name.split(':')[0]
            name = symbol_name
            self.size_data[name] = {
                'objName': symbol_name,
                'parent': parent,
                'path': file_dir,
                'romSize': 0,
                'moduleName': self._get_module_name(obj_name),
                'componentName': self._get_component_name(lib_name)
            }
            if is_symbol:
                self.size_data[name]['isSymbol'] = True
                self.size_data[name]['lib'] = lib_name
                self._set_module_data(self.size_data[name]['moduleName'], self.size_data[name]['componentName'],
                                      cur_size)
                self._set_module_data(self.size_data[name]['componentName'], '', cur_size)
                if self.size_data[name]['moduleName'] == '' and self.size_data[name]['componentName']:
                    self.size_data[name]['moduleName'] = self.size_data[name]['componentName']
            for item in self._section_keys:
                self.size_data[name][item] = 0
            self.size_data[name][self.cur_section_size] = cur_size
        if self.is_rom and name:
            self.size_data[name]['romSize'] = self.size_data[name]['romSize'] + cur_size

        if file_dir and not isabs(file_dir):
            self._set_folder_data(file_dir, name, cur_size, obj_name)

    def _set_folder_data(self, cur_path, cur_name, cur_size, child_name):
        """
        Split paths and assemble them into dict
        Example: "build/libs/main.o" assemble to {"build":{"child":[], "size":1, "libs"{"child":[main.o], "size":1}}}
        """
        path_list = cur_path.split(sep)
        temp_folder = self.folder_data

        for path in path_list:
            self._set_child_size(path, temp_folder, cur_size, [])
            temp_folder = temp_folder[path]

        if cur_name not in temp_folder['child']:
            if cur_name != child_name:
                self._set_child_size(cur_name, temp_folder, cur_size, [child_name])
                if cur_name in temp_folder and child_name not in temp_folder[cur_name]['child']:
                    temp_folder[cur_name]['child'].append(child_name)
            else:
                temp_folder['child'].append(cur_name)

    def _set_child_size(self, cur, folder, size, child_list):
        if cur not in folder:
            folder[cur] = {"child": child_list}
            for item in self._section_keys:
                folder[cur][item] = 0
            folder[cur][self.cur_section_size] = size
        else:
            if self.cur_section_size in folder[cur]:
                folder[cur][self.cur_section_size] = folder[cur][self.cur_section_size] + size
            else:
                folder[cur][self.cur_section_size] = size

    def _set_module_data(self, name, parent, cur_size):
        if name == '':
            return
        if name in self.module_data:
            if not self.module_data[name][self.cur_section_size]:
                self.module_data[name][self.cur_section_size] = cur_size
            else:
                self.module_data[name][self.cur_section_size] = self.module_data[name][self.cur_section_size] + cur_size
        elif name:
            self.module_data[name] = {
                'objName': name,
                'parent': parent,
                'romSize': 0
            }
            for item in self._section_keys:
                self.module_data[name][item] = 0
            self.module_data[name][self.cur_section_size] = cur_size
        if self.is_rom and name:
            self.module_data[name]['romSize'] = self.module_data[name]['romSize'] + cur_size

    def _get_module_name(self, obj_name):
        if obj_name in self._module_config and type(self._module_config[obj_name]) == str:
            return self._module_config[obj_name]
        else:
            return obj_name

    def _get_component_name(self, lib_name):
        if lib_name in self._module_config and type(self._module_config[lib_name]) == str:
            return self._module_config[lib_name]
        else:
            return lib_name

    def _get_cur_section(self, addr):
        self.cur_section = ''
        self.cur_section_size = ''

        for section_key in self.elf_size_data:
            section_size = self.elf_size_data[section_key]['size']
            section_addr = self.elf_size_data[section_key]['addr']
            if section_addr <= int(addr, 16) < section_addr + section_size:
                self.cur_section = section_key
                self.cur_section_size = self.cur_section
                break

        self.is_rom = False
        if self.cur_section in ('.text', '.rodata', '.data', '.l2m_gpu_text', '.plt_ramtext'):
            self.is_rom = True
        else:
            for rom_part, part_size in self.rom_parts.items():
                if part_size['start'] <= int(addr, 16) < part_size['end']:
                    self.is_rom = True
                    break

    def _reset_symbol_size_data(self):
        length = len(self.last_symbol)
        tmp_parent = self.last_symbol[0]['addr']
        all_size = int(self.size_data[tmp_parent][self.cur_section_size])

        for key, val in enumerate(self.last_symbol):
            if key != 0:
                tmp = val['addr']
                size = 0
                if key == length - 1:
                    size = all_size
                else:
                    size = int(self.last_symbol[key + 1]['addr'], 16) - int(self.last_symbol[key]['addr'], 16)
                    all_size = all_size - size
                tmp_size = {
                    'objName': self.last_symbol[key]['name'],
                    'parent': self.size_data[tmp_parent]['parent'],
                    'isSymbol': True,
                    'lib': self.size_data[tmp_parent]['lib'],
                    'moduleName': self.size_data[tmp_parent]['moduleName'],
                    'componentName': self.size_data[tmp_parent]['componentName']
                }
                for item in self._section_keys:
                    tmp_size[item] = 0
                self.size_data[tmp] = tmp_size
                self.size_data[tmp][self.cur_section_size] = size


if __name__ == '__main__':
    env = TargetEnvironment(sys.argv[1])
    chip = env.get('chip')
    core = env.get('core')
    bin_name = env.get('bin_name')
    hso_xlm_chip_name = env.get('hso_xml_chip')

    ELF_PATH = os.path.join(output_root, chip, core, sys.argv[1], bin_name + ".elf")
    MAP_PATH = os.path.join(output_root, chip, core, sys.argv[1], bin_name + ".map")
    RESULT_SAVE_PATH = os.path.join(output_root, chip, core, sys.argv[1], "image_analysis_result.html")
    COMPILER_PATH = sys.argv[2]
    # for debug
    # COMPILER_PATH = os.path.join(root_dir, "tools", "bin", "compiler", "linx", "linx_170", "linx-llvm-binary-debug",
    #                              "linx-llvm-binary-release-musl", "bin", "riscv32")

    if hso_xlm_chip_name != None: # just for melody
        chip = hso_xlm_chip_name
    BUILD_CONFIG_PATH = os.path.join(root_dir, 'build', 'config', 'target_config', chip, 'image_analysis_cfg', 'module.json')
    REMOVE_SECTIONS_CONFIG_PATH = os.path.join(root_dir, 'build', 'config', 'target_config', chip, 'image_analysis_cfg', "remove_sections.txt")

    PAHT_DICT = {"elf_path": ELF_PATH,
                 "map_path": MAP_PATH,
                 "compiler_path": COMPILER_PATH,
                 "build_config_path": BUILD_CONFIG_PATH,
                 "remove_sections_config_path": REMOVE_SECTIONS_CONFIG_PATH,
                 "result_save_path": RESULT_SAVE_PATH,
                 }

    IMAGE_ANALYSE_RUN = BuildDataAnalyzer(PAHT_DICT)
    IMAGE_ANALYSE_RUN.do_build_analysis(PAHT_DICT)

    print('Image Analyse Done ^_^')