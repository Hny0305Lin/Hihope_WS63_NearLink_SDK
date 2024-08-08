#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.

"""
* Description: NV binary create.
* Create: 2021-11-30
"""

import os
import re
import sys
import stat
import json
import hashlib
import struct
import shutil
import zlib
import binascii

g_root = os.path.realpath(__file__)
g_root = os.path.dirname(g_root)
g_root = os.path.realpath(os.path.join(g_root, "..", "..", ".."))
sys.path.append(os.path.join(g_root, 'build'))
sys.path.append(os.path.join(g_root, 'build', 'script'))
# print(g_root)

from conf_parser import BuildConfParser, ParserError
from build_utils import fn_str_to_int
from generate_data_stream import generate_data_stream

from ctypes import c_char, c_byte, c_ubyte, c_short, c_ushort, c_int, c_uint, Structure, sizeof

TD_CHAR = c_char
TD_S8 = c_byte
TD_U8 = c_ubyte
TD_S16 = c_short
TD_U16 = c_ushort
TD_S32 = c_int
TD_U32 = c_uint
g_u8_max = 0xFF
g_u16_max = 0xFFFF
class KeyHead(Structure):
    _fields_ = [
        ("magic", TD_U8),    # Magic number to indicate the start of the item
        ("valid", TD_U8),    # flag to indicate whether the value is valid
        ("length", TD_U16),   # Length of the key_data field in bytes
        ("type", TD_U8),      # Normal (0xFF) or permanent (0x00)  or keep (0x04  )
        ("upgrade", TD_U8),

        ("key_id", TD_U16),   # The Key ID
        ("enc_key", TD_U16),   # Allows some customisation of the data AES key used, 0x0 - key_data is plaintext, Others - key_data is encrypted
        ("version", TD_U16),  # Version of the key
        ("rnd", TD_U32)     # Key header crc calculated from length
    ]

class NvPageHead(Structure):
    _fields_ = [
        ("id", TD_U16),
        ("reserved", TD_U8),
        ("num_pages", TD_U8),
        ("inverted_details_word", TD_U32),
        ("last_write", TD_U32), # last_write
        ("unused", TD_U32), # We want this header to be 4 words long - this allows us to alias anything after it
    ]

# 1.根据指定的alias，合并所有配置文件
# 1.1 解析alias中所有type=Nv的配置。

# 2.如果需要，从合并的配置文件中提取NV项ID，生成ID枚举头文件提供给源码使用（构建过程不允许修改源码，因此该头文件只有需要的时候调用脚本生成）
# 3.预编译所有nv结构
# 4.解析配置文件中各个nv项的数据内容，与结构结合，生成bin



class BuildNv:
    def __init__(self, alias, root=None, targets=None, backup=False, use_crc16=False):
        self.alias = alias
        self.root = root if root is not None else g_root
        self.targets = targets
        self.is_backup = backup
        self.use_crc16 = use_crc16
        self.tmp_path = os.path.join(self.root, json_conf["BUILD_TEMP_PATH"])
        self.nv_relative_path = os.path.join(self.root, json_conf["NV_RELATIVE_PATH"])
        self.nv_root = os.path.join(self.root, json_conf["NV_DEFAULT_CFG_DIR"])
        self.nv_output_dir = os.path.join(self.root, json_conf["OUT_BIN_DIR"])
        if not backup:
            self.nv_output_name = json_conf["OUT_BIN_NAME"]
        else:
            self.nv_output_name = json_conf["OUT_BACKUP_BIN_NAME"]
        self.nv_ver_src_dict = dict()
        self.nv_ver_dict = dict()
        self.nv_flash_cfg = None
        self.nv_cores_ver_bin = dict()
        self.nv_chip_ver_bin = dict()
        self.nv_flash_page_index = dict()

    def set_nv_output_dir(self, path):
        self.nv_output_dir = path

    def start_work(self):
        self._merge_cfgs()
        self._load_nv_flash_cfg()
        self._parse_etypes()
        self._gen_binary()
        self._create_header()


    def _merge_cfgs(self):
        '''
        Merge config sources in self.nv_ver_src_dict.
        This will build the self.nv_ver_dict like the following tree:
                              |--- ver1 : {
                              |            "merged_cfg" : json file after merge all nv configuration with the same product type.
                              |            "prod_type" : "XXXX" }
                              |
            chip|---target1---|--- ver2 :
                |             |
                |             |--- ver3 :
                |             |
                |             |--- core : "Each target corresponds to one core."
                |---targetN...
        '''
        for target in self.alias:
            if self._nv_ver_prepare(target) is False:
                continue
        # print('nv_ver_src_dict: ', self.nv_ver_src_dict)

        for chip in self.nv_ver_src_dict:
            src_chip_dict = self.nv_ver_src_dict[chip]
            # print("src_chip_dict =",src_chip_dict)
            self.nv_ver_dict[chip] = {}
            chip_dict = self.nv_ver_dict[chip]
            for target in src_chip_dict:
                if chip_dict.get(target) is None:
                    chip_dict[target] = {'core':src_chip_dict[target]['core']}
                nv_tmp_dir = os.path.join(self.nv_relative_path)
                for ver_name in src_chip_dict[target]:
                    if ver_name == 'core':
                        continue
                    cfg_file_prefix = os.path.join(nv_tmp_dir, 'cfg', '%s_nv' % (target)) # 生成中间文件路径
                    # print("cfg_file_prefix = ", cfg_file_prefix)
                    chip_dict[target][ver_name] = self._merge_ver_cfg(cfg_file_prefix, src_chip_dict[target][ver_name])

    def _parse_etypes(self):
        for chip in self.nv_ver_dict:
            chip_dict = self.nv_ver_dict[chip]
            for target in chip_dict:
                # scons in chip dir or nv_config dir? etypes path depends on scons path
                nv_tmp_dir = os.path.join(self.tmp_path, target)
                etypes_path = os.path.join(nv_tmp_dir, "%s.etypes" % target) # 中间文件xxx.etypes路径
                if os.path.exists(etypes_path) is not True: # 判断中间文件是否存在，如果不在说明该模块没有被编译，需要加入到编译链接中
                    etypes_path = os.path.join(self.tmp_path, "etypes", "%s.etypes" % target)
                    if os.path.exists(etypes_path) is not True:
                        msg = "[error] [%s] need add nv_config module in alias! %s" % (target, etypes_path)
                        raise ParserError(msg)
                stream_gen = generate_data_stream()
                stream_gen.phase_etypes(etypes_path)
                chip_dict[target]["stream_gen"] = stream_gen
        dtabase_txt = os.path.join(self.root, json_conf['DATABASE_TXT_FILE'])
        shutil.copy(etypes_path, dtabase_txt)


    def _gen_binary(self):
        '''
                            |--- ver1 : binary file of ver1.
                            |   (version name :  product_type + version name)
            chip|---core1---|--- ver2 :
                |           |
                |           |--- ver3 :
                |---coreN...
        '''
        self._gen_binary_prepare()
        self._gen_target_version_binary()
        self._gen_chip_nv_binary()

    def _gen_target_version_binary(self):
        for chip in self.nv_cores_ver_bin:
            cores = self.nv_cores_ver_bin[chip]
            # print("cores =", cores)
            for core in cores:
                cores[core] = self._gen_version_binary(self.nv_ver_dict[chip], chip, core)

    def _gen_version_binary(self, chip_ver_dict, chip, core):
        ver_binary_dict = dict()
        # print("chip_ver_dict = ", chip_ver_dict) # 字典信息包含核名字，配置文件路径
        if chip_ver_dict is None:
            return ver_binary_dict
        for target in chip_ver_dict:
            if chip_ver_dict[target].get('core') != core: # 判断字典里面的target读出来的core是否和core相同
                print("chip_ver_dict[target].get('core') = ", chip_ver_dict[target].get('core'))
                continue
            stream_gen = chip_ver_dict[target].get('stream_gen')
            for ver in chip_ver_dict[target]:
                # print("ver =", ver)
                if ver == 'core' or ver == 'stream_gen':
                    continue
                stream_gen = chip_ver_dict[target].get('stream_gen')
                cfg_file = chip_ver_dict[target][ver]["merged_cfg"]
                # print("cfg_file =",cfg_file)
                stream = self._gen_nv_stream(cfg_file, stream_gen, chip, core)
                nv_ver_bin = \
                    os.path.join(self.nv_relative_path, 'bin', '%s_nv.bin' % (core))
                # print("nv_ver_bin = ", nv_ver_bin) # 生成nvbin文件的路径
                prod_type =  chip_ver_dict[target][ver]["prod_type"]
                prod_type = "all" if prod_type is None else prod_type
                ver_binary_dict["%s_%s" % (chip, prod_type)] = self._write_binary_to_file(nv_ver_bin, stream)
        return ver_binary_dict

    def _write_binary_to_file(self, file_path, stream):
        if os.path.exists(os.path.dirname(file_path)) is False:
            os.makedirs(os.path.dirname(file_path))
        if os.path.exists(file_path) is True:
            os.remove(file_path)
        flags = os.O_WRONLY | os.O_CREAT | os.O_EXCL
        modes = stat.S_IWUSR | stat.S_IRUSR
        with os.fdopen(os.open(file_path, flags, modes), 'wb') as fout:
            fout.write(stream)
        return file_path

    def _gen_binary_prepare(self):
        for chip in self.nv_ver_dict:
            self.nv_chip_ver_bin[chip] = dict()
            self.nv_cores_ver_bin[chip] = dict()
            self.nv_flash_page_index[chip] = dict()
            nv_flash_chip_cfg = self.nv_flash_cfg[chip]
            cores = nv_flash_chip_cfg["cores"].keys()
            chip_nv_ver = self.nv_ver_dict[chip]
            for target in chip_nv_ver:
                core = chip_nv_ver[target].get('core')
                for core in cores:
                    if core not in cores:
                        msg = "[error] [%s] [%s] not a core cfg in nv_storage_cfg.json!" % (target, core)
                        raise ParserError(msg)
                    if self.nv_cores_ver_bin[chip].get(core) is None:
                        self.nv_cores_ver_bin[chip][core] = dict()
            page_size = fn_str_to_int(nv_flash_chip_cfg["size"]["page_size"])
            total_size = 0
            for core in cores:
                core_page_nums = nv_flash_chip_cfg['cores'][core]['page_nums']
                total_size += core_page_nums * page_size
                self.nv_flash_page_index[chip][core] = [(sizeof(NvPageHead) + num * page_size, (num + 1) * page_size) \
                    for num in range(0, core_page_nums)]
            if total_size > fn_str_to_int(nv_flash_chip_cfg["size"]['flash_size']):
                msg = "[error] cores size: %s, over total flash size: %s!" % \
                    (total_size, nv_flash_chip_cfg["size"]['flash_size'])
                raise ParserError(msg)

    def _gen_nv_stream(self, cfg_file, stream_gen, chip, core):
        core_nv_bin = self._init_nv_page_head(chip, core)

        cfg_data = BuildConfParser(cfg_file).get_conf_data()
        key_id_list = []
        last_key_item_start_addr = 0
        for module in cfg_data:
            for key_item in cfg_data[module]:
                if key_item == 'module_id':
                    continue
                key_struct_name = cfg_data[module][key_item].get("structure_type")
                key_value = cfg_data[module][key_item].get("value")
                key_attr = cfg_data[module][key_item].get("attributions")
                key_id = cfg_data[module][key_item].get("key_id")
                key_id = fn_str_to_int(key_id) if type(key_id) is not int else key_id
                if key_struct_name is None or key_value is None or key_attr is None or key_value == []:
                    msg = "[error] 'structure_type' 'value' 'attributions' must be configured!"
                    raise ParserError(msg)

                if key_id in key_id_list:
                    msg = "[error] key id:%d repeated, please check!" % key_id
                    raise ParserError(msg)
                key_id_list.append(key_id)

                key_data, key_data_len = stream_gen.generate(key_struct_name, key_value)
                page_index, key_item_start_addr = self._find_usable_addr(chip, core, key_data, key_data_len)
                core_nv_bin, key_item_start_addr = \
                    self._init_key_head(core_nv_bin, key_item_start_addr, key_data_len, key_id, key_attr)

                core_nv_bin, key_item_start_addr = \
                    self._set_key_data(core_nv_bin, key_item_start_addr, key_data, key_data_len)
                core_nv_bin, key_item_start_addr = \
                    self._set_key_hash(core_nv_bin, key_item_start_addr, key_data_len)
                self._update_core_index(chip, core, page_index, key_item_start_addr)
                last_key_item_start_addr = max(last_key_item_start_addr, key_item_start_addr)
        core_nv_bin = self._set_unused_page(chip, core_nv_bin, last_key_item_start_addr)
        self._reset_core_index(chip, core)
        return core_nv_bin

    def _set_unused_page(self, chip, core_nv_bin, key_item_start_addr):
        page_size = fn_str_to_int(self.nv_flash_cfg[chip]['size']['page_size'])
        core_nv_end_addr = (key_item_start_addr + (page_size - 1)) & ~(page_size - 1)
        for i in range(key_item_start_addr, core_nv_end_addr):
            core_nv_bin[i] = 0xFF
        return core_nv_bin[0 : core_nv_end_addr]

    def _gen_chip_nv_binary(self):
        for chip in self.nv_cores_ver_bin:
            chip_bins = self.nv_cores_ver_bin[chip]
            ver_list = []
            for core in chip_bins:
                ver_list.extend(chip_bins[core].keys())

            ver_list = set(ver_list)
            for ver in ver_list:
                self.nv_chip_ver_bin[chip][ver] = self._assemble_ver_bins(chip, ver)

    def _assemble_ver_bins(self, chip, ver):
        flash_bin = bytearray(fn_str_to_int(self.nv_flash_cfg[chip]['size']['flash_size']))
        for i in range(0, len(flash_bin)):
            flash_bin[i] = 0xFF

        start_addr = 0
        chip_bins = self.nv_cores_ver_bin[chip]
        cores = self.nv_cores_ver_bin[chip]
        for core in chip_bins:
            ver_bin = chip_bins[core].get(ver)
            flash_bin, start_addr = self._append_file_to_stream(flash_bin, start_addr, chip, core, ver_bin)
            chip_ver_bin_file = os.path.join(self.nv_output_dir, self.nv_output_name)
        # 目前只考虑单核场景去除 NV bin 末尾的无用 FF
        return self._write_binary_to_file(chip_ver_bin_file, flash_bin[0 : start_addr])

    def _append_file_to_stream(self, flash_bin, start_addr, chip, core, ver_bin):
        core_bin_size = fn_str_to_int(self.nv_flash_cfg[chip]['size']["page_size"]) * \
                        self.nv_flash_cfg[chip]["cores"][core]['page_nums']
        core_nv_bin = b''
        if ver_bin is None:
            core_nv_bin = self._init_nv_page_head(chip, core)
        else:
            with open(ver_bin, 'rb') as f:
                core_nv_bin = f.read()
        #print('core_nv_bin = ', core_nv_bin)
        tail_addr = start_addr + len(core_nv_bin)
        flash_bin[start_addr : tail_addr] = core_nv_bin

        return flash_bin, tail_addr

    def _reset_core_index(self, chip, core):
        nv_flash_chip_cfg = self.nv_flash_cfg[chip]
        page_size = fn_str_to_int(nv_flash_chip_cfg['size']["page_size"])
        core_page_nums = nv_flash_chip_cfg['cores'][core]['page_nums']
        self.nv_flash_page_index[chip][core] = [(sizeof(NvPageHead) + num * page_size, (num + 1) * page_size) \
            for num in range(0, core_page_nums)]

    def _update_core_index(self, chip, core, index, addr):
        (start_addr, page_max_addr) = self.nv_flash_page_index[chip][core][index]
        if start_addr >= addr or addr > page_max_addr:
            msg = "[error] addr %s invalid!" % addr
            raise ParserError(msg)
        self.nv_flash_page_index[chip][core][index] = (addr, page_max_addr)
        #print("update page index: \n", self.nv_flash_page_index)

    def _find_usable_addr(self, chip, core, key_data, key_data_len):
        page_size = fn_str_to_int(self.nv_flash_cfg[chip]['size'].get('page_size'))
        key_item_total_len = self._get_key_item_len(key_data_len)
        if key_item_total_len > page_size - sizeof(NvPageHead):
            msg = "[error] key over page size !" % key_id
            raise ParserError(msg)

        index = 0
        for (start_addr, page_max_addr) in self.nv_flash_page_index[chip][core]:
            if start_addr + key_item_total_len > page_max_addr:
                index += 1
                continue
            return index, start_addr
        msg = "[error] no more enough space for [%s]!" % core
        raise ParserError(msg)

    def _get_key_type_from_attr(self, key_id, attr):
        if (attr & 1) and (not (attr & ~1)):
            return 0xFF
        elif (attr & 2) and (not (attr & ~2)):
            return 0x00
        elif (attr & 4) and (not (attr & ~4)):
            return 0xFF
        else:
            msg = "[error] attribution config err: [id-%s] [attr-%s] !" % (key_id, attr)
            raise ParserError(msg)

    def _get_key_upgrade_from_attr(self, key_id, attr):
        if (attr & 1) and (not (attr & ~1)):
            return 0xFF
        elif (attr & 2) and (not (attr & ~2)):
            return 0xFF
        elif (attr & 4) and (not (attr & ~4)):
            return 0x00
        else:
            msg = "[error] attribution config err: [id-%s] [attr-%s] !" % (key_id, attr)
            raise ParserError(msg)

    def _get_key_item_len(self, key_data_len):
        if key_data_len % 4 != 0:
            key_data_len += 4 - key_data_len % 4
        return sizeof(KeyHead) + 4 + key_data_len

    def _set_key_hash(self, nv_bin, key_item_start_addr, key_data_len):
        if key_data_len % 4 != 0:
            key_data_len += 4 - key_data_len % 4
        hash_start_addr = key_item_start_addr - key_data_len - sizeof(KeyHead)
        hash_end_addr = hash_start_addr + key_data_len + sizeof(KeyHead)
        if not self.use_crc16:
            crc32num = zlib.crc32(nv_bin[hash_start_addr : hash_end_addr])
        else:
            crc32num = binascii.crc_hqx(nv_bin[hash_start_addr : hash_end_addr], 0)
        crc32ret = '{:0>8X}'.format(crc32num)
        crc32ret = re.sub(r"(?<=\w)(?=(?:\w\w)+$)", " 0x", crc32ret)
        crc32ret = '0x' + crc32ret
        crc32list = [int(x,16) for x in crc32ret.split(" ")]
        sha256bytearray = bytes(crc32list)
        tail_addr = key_item_start_addr + len(sha256bytearray)
        nv_bin[key_item_start_addr : tail_addr] = sha256bytearray
        return nv_bin, tail_addr

    def _set_key_data(self, nv_bin, key_item_start_addr, key_data, key_data_len):
        if key_data_len % 4 != 0:
            for i in range(0, 4 - key_data_len % 4) :
                key_data += b'\x00'
            key_data_len += 4 - key_data_len % 4
        tail_addr = key_item_start_addr + key_data_len
        nv_bin[key_item_start_addr : tail_addr] = key_data
        return nv_bin, tail_addr

    def _init_key_head(self, nv_bin, key_item_start_addr, key_data_len, key_id, key_attr):
        nv_key_st = KeyHead.from_buffer(nv_bin[key_item_start_addr:])

        nv_key_st.magic = 0xA9
        nv_key_st.length = key_data_len
        nv_key_st.type = self._get_key_type_from_attr(key_id, key_attr)
        nv_key_st.upgrade = self._get_key_upgrade_from_attr(key_id, key_attr)
        nv_key_st.key_id = key_id

        nv_key_st.version = 65535
        nv_key_st.enc_key = 0 # 目前不支持加密，且nv加密部分不由nv脚本做，可能放到初始化？
        tail_addr = key_item_start_addr + sizeof(KeyHead)
        nv_bin[key_item_start_addr : tail_addr] = nv_key_st
        return nv_bin, tail_addr

    def _init_nv_page_head(self, chip, core):
        nv_flash_chip_cfg = self.nv_flash_cfg[chip]
        default_page_nums = nv_flash_chip_cfg.get('default_page_nums')
        page_size = fn_str_to_int(nv_flash_chip_cfg['size']['page_size'])
        page_nums = nv_flash_chip_cfg['cores'][core]['page_nums']
        if not self.is_backup:
            page_id_start = nv_flash_chip_cfg['cores'][core]['page_id_start']
        else:
            page_id_start = '0x34B2'
        core_nv_size = page_nums * page_size
        core_nv_bin = bytearray(core_nv_size)

        for i in range(0, core_nv_size):
            core_nv_bin[i] = 0xFF

        for i in range(0, page_nums):
            start_addr = i * page_size
            nv_page_head = NvPageHead.from_buffer(core_nv_bin[start_addr:])
            nv_page_head.id = fn_str_to_int(page_id_start)
            nv_page_head.reserved = 1
            nv_page_head.num_pages = i
            nv_page_head.inverted_details_word = ~int.from_bytes(struct.pack('HBB', \
                nv_page_head.id, nv_page_head.reserved, nv_page_head.num_pages), 'little')
            nv_page_head.last_write = 0
            nv_page_head.unused = ~nv_page_head.last_write
            core_nv_bin[start_addr : start_addr + sizeof(NvPageHead)] = nv_page_head
        return core_nv_bin

    def _load_nv_flash_cfg(self):
        self.nv_flash_cfg = dict()
        for chip in self.nv_ver_dict:
            cfg_file = os.path.join(self.root, json_conf["NV_TARGET_JSON_PATH"])
            self.nv_flash_cfg[chip] = BuildConfParser(cfg_file).get_conf_data()

    def _add_nv_ver(self, chip, target, core, ver, common_cfg, ver_cfg, prod_type=None):
        '''
        Add version config into self.nv_ver_src_dict.
        There are three configuration scenarios.One target may correspond to multiple NV versions.

                              |--- ver1: { srcs:[default, common, cfg1], prod_type: }
                              |
            chip|---target1---|--- ver2: { srcs:[default, cfg2],         prod_type: }
                |             |
                |             |--- ver3: { srcs:[default, common],       prod_type: }
                |             |
                |             |--- core : "Each target corresponds to one core."
                |---targetN...
        '''

        ver_cfgs = []
        if os.path.exists(common_cfg) is True:
            ver_cfgs.append(common_cfg)
        if ver_cfg is not None and os.path.exists(ver_cfg):
            ver_cfgs.append(ver_cfg)

        if self.nv_ver_src_dict.get(chip) is None:
            self.nv_ver_src_dict[chip] = dict()

        chip_dict = self.nv_ver_src_dict[chip]
        if chip_dict.get(target) is not None and chip_dict[target].get(ver) is not None:
            msg = "[error] Ver config Repeate!"
            raise ParserError(msg)

        if chip_dict.get(target) is None:
            chip_dict[target] = {ver:{"srcs":ver_cfgs, 'prod_type': prod_type}}
        else:
            chip_dict[target].update({ver:{"srcs":ver_cfgs, 'prod_type': prod_type}})

        if chip_dict[target].get('core') is None:
            chip_dict[target]['core'] = core
        elif chip_dict[target].get('core') != core:
            msg = "[error] [%s] core not match!" % target
            raise ParserError(msg)

    def _nv_ver_prepare(self, target):
        '''
            1. Check nv configurations.
            2. Add all correct config into self.nv_ver_src_dict..
        '''
        if type(self.alias[target]) is list:
            return False
        target_type = self.alias[target].get("TYPE")
        if target_type is None or target_type != 'nv':
            return False

        core = self.alias[target].get("CORE")
        if core is None:
            msg = "[error] core name not exist!"
            raise ParserError(msg)
        chip = self.alias[target].get("CHIP")
        #default_cfg = os.path.join(self.nv_root, '%s_default.json' % core)
        '''
        if chip is None or os.path.exists(default_cfg) is False:
            msg = "[error] chip name OR %s not exist!" % default_cfg
            raise ParserError(msg)
        '''
        kernel_name = self.alias[target].get("KERNEL_BIN")
        if kernel_name is None:
            msg = "[error] KERNEL is null!"
            raise ParserError(msg)
        if self.targets is not None and kernel_name not in self.targets:
            return False
        cfgs = self.alias[target].get("COMPONENT")
        if cfgs is None:
            msg = "[error] COMPONENT is null!"
            raise ParserError(msg)

        prod_type = self.alias[target].get("PRODUCT_TYPE")
        if prod_type == "":
            prod_type = None
        if prod_type is not None and type(prod_type) is not str:
            msg = "[error] PRODUCT_TYPE must be a string type, one kernel only suuport one product type!"
            raise ParserError(msg)

        cfg_dir = os.path.join(self.nv_root, kernel_name)
        common_cfg = os.path.join(cfg_dir, 'common.json')

        for cfg in cfgs:
            cfg_file = os.path.join(cfg_dir, '%s.json' % cfg) if cfg != 'common' else None
            self._add_nv_ver(chip, kernel_name, core, cfg, common_cfg, cfg_file, prod_type)

    def _prod_type_filter(self, srcs, prod_type = None):
        combination = dict()
        module_dict = dict()
        for src in srcs:
            # print("[INFO] nv config src file: ", src)
            src_conf = BuildConfParser(src).get_conf_data()
            for module in src_conf:
                module_id = src_conf.get(module).get('module_id')
                if module_id is None:
                    msg = "[error][file:%s][%s] module_id is null!" % (src, module)
                    raise ParserError(msg)
                module_id = fn_str_to_int(module_id) if type(module_id) is not int else module_id
                if module_id > g_u8_max :
                    msg = "[error][file:%s][%s] module_id is more than 0xFF!" % (src, module)
                    raise ParserError(msg)

                # diffrent module must config with diffrent module_id
                if module_id in module_dict:
                    if module_dict[module_id] != module:
                        msg = "[error][file:%s][%s] module_id is the same to [%s]!" % (src, module, module_dict[module_id])
                        raise ParserError(msg)
                else:
                    module_dict[module_id] = module

                if module not in combination:
                    combination[module] = {'module_id': module_id}
                elif combination.get(module).get('module_id') != module_id:
                    msg = "[error][%s][%s] module_id is not same as other file!" % (src, module)
                    raise ParserError(msg)

                for item in src_conf.get(module):
                    key_cfg = src_conf.get(module).get(item)
                    if item == 'module_id':
                        continue

                    key_id = key_cfg.get('key_id')
                    key_id = None if key_id is None else (fn_str_to_int(key_id) if type(key_id) is not int else key_id)
                    # print("key id: %d, module id: %d, key>>8: %d"%( key_id, module_id, key_id>>8))
                    if key_id is None or key_id > g_u16_max :
                        msg = "[error][file:%s][%s][%s] key_id is null or more than unsighed 16 or not match with module_id!" % (src, module, item)
                        raise ParserError(msg)

                    item_prod_type = key_cfg.get('product_type')
                    key_status = key_cfg.get('key_status')
                    #print('prodtype: %s, key prod: %s'%(prod_type, item_prod_type))
                    if (prod_type == item_prod_type or (item_prod_type is not None and prod_type in item_prod_type)) \
                        and key_status == 'alive':
                        combination[module].update({item:key_cfg})
        return combination

    def _nv_cfg_writer(self, dst_file, combination):
        if os.path.exists(os.path.dirname(dst_file)) is False:
            os.makedirs(os.path.dirname(dst_file))
        if os.path.exists(dst_file):
            os.remove(dst_file)
        flags = os.O_WRONLY | os.O_CREAT | os.O_EXCL
        modes = stat.S_IWUSR | stat.S_IRUSR
        with os.fdopen(os.open(dst_file, flags, modes), 'w') as fout:
            fout.write(json.dumps(combination, indent=4))

    def _merge_ver_cfg(self, file_prefix, ver_cfg):
        srcs = ver_cfg.get('srcs')
        if srcs is None:
            msg = "[error] ver cfg file is null!"
            raise ParserError(msg)

        prod_type = ver_cfg.get('prod_type')
        combination = self._prod_type_filter(srcs, prod_type)
        dst_file = '%s.json' % file_prefix
        self._nv_cfg_writer(dst_file, combination)
        return { "merged_cfg" : dst_file, "prod_type" : prod_type}

    def _create_header(self):
        pass

# 检查配置文件中是否有必须配置项
def check_key(json_conf):
    check_item = ['OUT_BIN_DIR', 'BUILD_TEMP_PATH', 'NV_TARGET_JSON_PATH', \
        'NV_RELATIVE_PATH', 'NV_DEFAULT_CFG_DIR', 'DATABASE_TXT_FILE']
    keys = dict.keys(json_conf)
    for check_key in check_item:
        if check_key not in keys:
            msg = "[error] [nv_binary] need add ConfigMap (%s) in json_conf!" % (check_key)
            raise ParserError(msg)

def test(targets, flag, backup, use_crc16):
    root = g_root
    nv_target_json_path = os.path.join(root, json_conf["NV_TARGET_JSON_PATH"])
    alias_conf = BuildConfParser(nv_target_json_path).get_conf_data()
    worker = BuildNv(alias_conf, root, targets, backup, use_crc16)
    if flag:
        worker.set_nv_output_dir(os.path.join(root, json_conf["OUT_BIN_DIR"]))
    worker.start_work()

def nv_begin(in_path, targets, flag, gen_backup=False, use_crc16=False):
    global json_conf
    with open(in_path, 'r') as i:
        json_conf = json.load(i)

    check_key(json_conf)
    test(targets, flag, False, use_crc16)
    if gen_backup:
        test(targets, flag, True, use_crc16)
    print("build nv bin success!!")

if __name__ == "__main__":
    in_path = sys.argv[2]
    targets = sys.argv[3].split()
    flag = len(sys.argv) == 3

    nv_begin(in_path, targets, flag)
