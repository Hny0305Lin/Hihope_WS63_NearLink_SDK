#!/usr/bin/env python3
# coding=utf-8

'''
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Description: part of hupg build scripts
'''

import argparse
import configparser
import os
import re
import hashlib
import sys
import subprocess
import copy
import struct
import platform
from ctypes import *

###############################Defining Basic Types###############################################
td_char = c_char
td_s8 = c_byte
td_u8 = c_ubyte
td_s16 = c_short
td_u16 = c_ushort
td_s32 = c_int
td_u32 = c_uint

file_dir = os.path.dirname(os.path.realpath(__file__))
g_root = os.path.realpath(os.path.join(file_dir, "..", ".."))

class fota_key_area_data(Structure):
    pass

class fota_info_area_data(Structure):
    pass

class upg_sha256_sign(Structure):
    _fields_ = [
        ("check_sum", td_u8 * 32),
        ("padding", td_u8 * 224),
    ]

class fota_image_hash_node(Structure):
    _fields_ = [
        ("image_id", td_u32),
        ("image_addr", td_u32),
        ("image_length", td_u32),
        ("image_hash", td_u8 * 32),
    ]

class fota_image_head(Structure):
    _fields_ = [
        ("header_magic", td_u32),
        ("image_id", td_u32),
        ("image_offset", td_u32),
        ("image_len", td_u32),
        ("image_hash", td_u8 * 32),
        ("old_image_len", td_u32),
        ("old_image_hash", td_u8 * 32),
        ("new_image_len", td_u32),
        ("version_ext", td_u32),
        ("version_mask", td_u32),
        ("decompress_flag", td_u32),
        ("re_enc_flag", td_u32),
        ("root_key_type", td_u32),
        ("enc_pk_l1", td_u8 * 16),
        ("enc_pk_l2", td_u8 * 16),
        ("iv", td_u8 * 16),
        ("padding", td_u8 * 4),
    ]

class image_key_area(Structure):
    _fields_ = [
        ("image_id", td_u32),
        ("structure_version", td_u32),
        ("structure_length", td_u32),
        ("signature_length", td_u32),
        ("key_owner_id", td_u32),
        ("key_id", td_u32),
        ("key_alg", td_u32),
        ("ecc_curve_type", td_u32),
        ("key_length", td_u32),
        ("key_version_ext", td_u32),
        ("mask_key_version_ext", td_u32),
        ("msid_ext", td_u32),
        ("mask_msid_ext", td_u32),
        ("maintenance_mode", td_u32),
        ("die_id", td_u8 * 16),
        ("code_info_addr", td_u32),
        ("reserved", td_u8 * 52),
        ("ext_pulic_key_area", td_u8 * 64),
        ("sig_key_area", td_u8 * 64),
    ]

class image_code_area(Structure):
    _fields_ = [
        ("image_id", td_u32),
        ("structure_version", td_u32),
        ("structure_length", td_u32),
        ("signature_length", td_u32),
        ("version_ext", td_u32),
        ("mask_version_ext", td_u32),
        ("msid_ext", td_u32),
        ("mask_msid_ext", td_u32),
        ("code_area_addr", td_u32),
        ("code_area_len", td_u32),
        ("code_area_hash", td_u8 * 32),
        ("code_enc_flag", td_u32),
        ("protection_key_l1", td_u8 * 16),
        ("protection_key_l2", td_u8 * 16),
        ("iv", td_u8 * 16),
        ("code_compress_flag", td_u32),
        ("code_uncompress_len", td_u32),
        ("text_segment_size", td_u32),
        ("reserved", td_u8 * 248),
        ("sig_code_info", td_u8 * 16),
        ("sig_code_info_ext", td_u8 * 16),
    ]

class imageInfo:
    def __init__(self, path):
        with open(path, 'rb') as fp:
            image_bin = fp.read()
        key_ver = image_bin[36:40]
        key_mask = image_bin[40:44]
        key_area_len = sizeof(image_key_area)
        code_ver = image_bin[key_area_len + 16:key_area_len + 20]
        code_mask = image_bin[key_area_len + 20:key_area_len + 24]
        self.key_ver = struct.unpack("I", key_ver)
        self.key_mask = struct.unpack("I", key_mask)
        self.code_ver = struct.unpack("I", code_ver)
        self.code_mask = struct.unpack("I", code_mask)

def fota_format_init(upg_format_path):
    sys.path.append(upg_format_path)
    from fota_format_st import fota_key_area_data_fields, fota_info_area_data_fields
    fota_key_area_data._fields_ = fota_key_area_data_fields
    fota_info_area_data._fields_ = fota_info_area_data_fields

def hex2dec(string_num):
    return int(string_num.upper(), 16)

def fill_fota_key_area(cf, key_ver, key_mask):
    key_area_len = sizeof(fota_key_area_data)
    temp_offset = [0]
    temp_offset[0] = temp_offset[0] + key_area_len
    fota_key_bin_tmp = bytearray(temp_offset[0])
    fota_key_bin = fota_key_bin_tmp[0:key_area_len]
    fota_key_head = fota_key_area_data.from_buffer(fota_key_bin)

    fota_key_head.image_id = hex2dec(cf.get('FOTA_KEY_AREA', 'ImageId'))
    fota_key_head.struct_version = hex2dec(cf.get('FOTA_KEY_AREA', 'StructVersion'))
    fota_key_head.struct_length = key_area_len
    fota_key_head.key_owner_id = int(cf.get('FOTA_KEY_AREA', 'KeyOwnerId'))
    fota_key_head.key_id = int(cf.get('FOTA_KEY_AREA', 'KeyId'))
    fota_key_head.key_alg = hex2dec(cf.get('FOTA_KEY_AREA', 'KeyAlg'))
    if (fota_key_head.key_alg == 0x2A13C856 or fota_key_head.key_alg == 0x2A13C867):
        fota_key_head.signature_length = 32
    else:
        fota_key_head.signature_length = sizeof(fota_key_head.sig_fota_key_area)
    fota_key_head.ecc_curve_type = 0
    fota_key_head.key_length = 0
    # fota_key_head.fota_key_version_ext = key_ver  # Version of FOTA_External_Public_Key
    # fota_key_head.mask_fota_key_version_ext = key_mask
    fota_key_head.fota_key_version_ext = hex2dec(cf.get('FOTA_KEY_AREA', 'KeyVersion'))
    fota_key_head.mask_fota_key_version_ext = hex2dec(cf.get('FOTA_KEY_AREA', 'KeyVersionMask'))

    fota_key_head.msid_ext = hex2dec(cf.get('FOTA_KEY_AREA', 'Msid'))
    fota_key_head.mask_msid_ext = hex2dec(cf.get('FOTA_KEY_AREA', 'MsidMask'))
    fota_key_head.maintenance_mode = 0
    # fota_key_head.die_id = 0
    fota_key_head.fota_info_addr = 0  # 相对fota key区末尾的地址，0表示key area后接fota info区
    # 剩下3个：预留区、公钥、签名由签名工具填充
    return key_area_len, fota_key_bin

def fill_fota_info_area(cf, image_num, fota_obj):
    area_len = sizeof(fota_info_area_data)
    key_alg = hex2dec(cf.get('FOTA_KEY_AREA', 'KeyAlg'))
    temp_offset = [0]
    temp_offset[0] = temp_offset[0] + area_len
    upg_bin = bytearray(temp_offset[0])
    upg_bin = upg_bin[0:area_len]
    fota_info_head = fota_info_area_data.from_buffer(upg_bin)

    fota_info_head.image_id = hex2dec(cf.get('FOTA_INFO_AREA', 'ImageId'))
    fota_info_head.struct_version = hex2dec(cf.get('FOTA_INFO_AREA', 'StructVersion'))
    fota_info_head.struct_length = area_len
    if (key_alg == 0x2A13C856 or key_alg == 0x2A13C867):
        fota_info_head.signature_length = 32
    else:
        fota_info_head.signature_length = sizeof(fota_info_head.sign_fota_info)
    fota_info_head.fota_version_ext = hex2dec(cf.get('FOTA_INFO_AREA', 'Version'))
    fota_info_head.mask_fota_version_ext = hex2dec(cf.get('FOTA_INFO_AREA', 'VersionMask'))
    fota_info_head.msid_ext = hex2dec(cf.get('FOTA_INFO_AREA', 'Msid'))
    fota_info_head.mask_msid_ext = hex2dec(cf.get('FOTA_INFO_AREA', 'MsidMask'))
    # fota_info_head.image_hash_table_addr = 0 # 所有镜像处理完后再计算hash
    # fota_info_head.image_hash_table_length = 0
    # fota_info_head.image_hash_table_hash = 0
    fota_info_head.image_num = image_num
    fota_info_head.hardware_id = int(cf.get('FOTA_INFO_AREA', 'HardwareID'))
    if hasattr(fota_obj, 'hardware_id') and fota_info_head.hardware_id == 0:
        fota_info_head.hardware_id = fota_obj.hardware_id

    #fota_info_head.reserved = 0 # 预留给用户
    #fota_info_head.sign_fota_info = 0 # 签名

    return area_len, upg_bin

def set_fota_info_area(fota_info_len, upg_bin, hash_table_addr, hash_table_length, hash_table_hash):
    tmp_bin = upg_bin[0:fota_info_len]
    fota_info_head = fota_info_area_data.from_buffer(tmp_bin)
    fota_info_head.image_hash_table_addr = hash_table_addr
    fota_info_head.image_hash_table_length = hash_table_length
    tmp_bin[40:72] = hash_table_hash[0:32] # 填充fota_info_head.image_hash_table_hash
    upg_bin[0:fota_info_len] = tmp_bin
    return

def make_sha256_unsecure_signature(content):
    # common段非安全签名
    # Non-secure signature of common field
    signature_bin = bytearray(sizeof(upg_sha256_sign))
    signature = upg_sha256_sign.from_buffer(signature_bin)
    common_head_sh = hashlib.sha256()
    common_head_sh.update(content)
    common_head_hash = common_head_sh.digest()
    signature_bin[0:sizeof(signature.check_sum)] = common_head_hash
    return signature_bin

def fill_fota_image_hash_node(hash_bin, image_id, image_index, tmp_hash, addr_offset, image_length):
    hash_node_size = sizeof(fota_image_hash_node)
    start_offset = image_index * hash_node_size
    tmp_hash_bin = bytearray(hash_node_size)
    fota_hash= fota_image_hash_node.from_buffer(tmp_hash_bin)

    fota_hash.image_id = image_id
    fota_hash.image_addr = addr_offset
    fota_hash.image_length = image_length
    tmp_hash_bin[12:44] = tmp_hash[0:32]

    hash_bin[start_offset:start_offset + hash_node_size] = tmp_hash_bin
    return

def lzma_compress_bin(src_file, dst_file, lzma_tool):
    # print('lzma compress tool :', lzma_tool)
    # print('lzma    src   file :', src_file)
    # print('lzma    out   file :', dst_file)
    cmd_list0 = []
    if platform.system().lower() == "linux":
        cmd_list0.append('chmod')
        cmd_list0.append('755')
        cmd_list0.append(lzma_tool)
    elif platform.system().lower() == "windows":
        cmd_list0.append('cacls')
        cmd_list0.append(lzma_tool)
        cmd_list0.append('/p everyone:f /e')
    str_cmd=' '.join(cmd_list0)
    ret = subprocess.run(str_cmd, shell=True)
    if ret.returncode != 0:
        sys.exit("chmod lzma tool failed: %s" % ret)

    cmd_list = []
    cmd_list.append(lzma_tool)
    cmd_list.append('-d12 -lc0 -lp0 e')
    cmd_list.append(src_file)
    cmd_list.append(dst_file)
    str_cmd=' '.join(cmd_list)
    ret = subprocess.run(str_cmd, shell=True)
    if ret.returncode != 0:
        sys.exit("run lzma tool failed: %s" % ret)

def build_diff_cfg_file(fota_obj, basename, image_pairs, image_bin_file):
    diff_cfg_file = os.path.join(fota_obj.temp_dir, '%s_diff.cfg'%basename)
    diff_cfg_content = [
        'OldVersionFile="%s"' % image_pairs[0],
        'NewVersionFile="%s"' % image_pairs[1],
        'DiffFile="%s"' % image_bin_file
        ]
    remove_file(diff_cfg_file)
    create_dirs(os.path.dirname(diff_cfg_file))
    with open(diff_cfg_file, 'w') as fp:
        fp.write('\n'.join(diff_cfg_content))
    return diff_cfg_file

# 解密要求16byte对齐，将lzma压缩头填充至16字节对齐：[head(13byte) + body(N byte)] -> [head(16byte) + body(N byte)]
def fill_lzma_head(upg_lzma_file, filepath, filename):
    temp_offset3 = [0]
    dst_bin = bytearray(temp_offset3[0])
    file_sta = os.stat(upg_lzma_file)
    image_len = file_sta.st_size
    with open(upg_lzma_file, 'rb') as fp:
        src_bin= fp.read()
    head_len = 13; # lzma压缩头长度
    align_len = 16 # 16byte对齐后的lzma压缩头长度

    dst_bin[0:head_len] = src_bin[0:head_len]
    dst_bin[head_len:align_len] = bytearray(align_len - head_len)
    dst_bin[align_len:align_len + image_len - head_len] = src_bin[head_len:image_len]
    remove_file(upg_lzma_file)
    dst_lzma_file = upg_lzma_file
    with open(dst_lzma_file, 'wb') as fp:
        fp.write(dst_bin)
    return dst_lzma_file

def get_image_bin_data(fota_image, image_confg_key, fota_obj):
    image_bin_file = fota_obj.input_dict.get(image_confg_key)
    fota_image.new_image_len = os.stat(image_bin_file).st_size
    (filepath, tempfilename) = os.path.split(image_bin_file)
    (filename, extension) = os.path.splitext(tempfilename)
    if fota_image.image_id == 0xCB9E063C: # NV镜像不做处理，保持原数据
        pass
    elif fota_image.decompress_flag == 0x3C7896E1: # 压缩ID
        # 制作压缩升级文件
        upg_lzma_file = os.path.join(filepath, '%s.lzma'%filename)
        lzma_compress_bin('"%s"' % image_bin_file, '"%s"' % upg_lzma_file, fota_obj.lzma_tool)
        image_bin_file = fill_lzma_head(upg_lzma_file, filepath, filename)
    elif fota_image.decompress_flag == 0x44494646: # 差分ID
        # 制作差分升级文件
        if fota_obj.diff_image_info == None:
            sys.exit("[ERROR] diff image dict is None!!!")
        image_pairs = fota_obj.diff_image_info.get(image_confg_key)
        if image_pairs == None:
            sys.exit("[ERROR] diff bin is None!!!")
        # 镜像头记录新老镜像长度
        fota_image.old_image_len = os.stat(image_pairs[0]).st_size
        fota_image.new_image_len = os.stat(image_pairs[1]).st_size
        # 制作旧镜像hash
        with open(image_pairs[0], 'rb') as fp: # file为旧镜像
            old_image_bin = fp.read()
        old_image_hash = make_sha256_unsecure_signature(old_image_bin)
        fota_image.old_image_hash[:] = old_image_hash[0:32]
        # 差分后的镜像
        image_bin_file = os.path.join(os.path.dirname(image_pairs[1]), '%s_diff.bin'%filename)
        # 差分配置文件
        diff_cfg_file = build_diff_cfg_file(fota_obj, filename, image_pairs, image_bin_file)
        cmd = ' '.join([fota_obj.upg_tool, '4', '"%s"' % diff_cfg_file])
        ret = subprocess.run(cmd, shell=True)
        if ret.returncode != 0:
            sys.exit("run diff tool failed: %d" % ret)
    with open(image_bin_file, 'rb') as fp: # 不压缩时新镜像和原镜像一致, file为原镜像
        image_bin= fp.read()
    file_stats = os.stat(image_bin_file)
    return image_bin, file_stats.st_size

def fill_fota_image_head(fota_image, cf, image_confg_key, start_offset, image_offset, fota_image_head_size):
    fota_image.header_magic = hex2dec(cf.get(image_confg_key, 'HeaderMagic'))
    fota_image.image_id = hex2dec(cf.get(image_confg_key, 'ImageId'))
    fota_image.image_offset = start_offset + image_offset + fota_image_head_size
    fota_image.old_image_len = 0
    fota_image.new_image_len = 0
    # fota_image.version_ext = int(''.join(map(str, info.code_ver)))
    # fota_image.version_mask = int(''.join(map(str, info.code_mask)))
    fota_image.version_ext = hex2dec(cf.get(image_confg_key, 'version_ext'))
    fota_image.version_mask = hex2dec(cf.get(image_confg_key, 'version_mask'))
    fota_image.decompress_flag = hex2dec(cf.get(image_confg_key, 'DecompressFlag'))
    fota_image.re_enc_flag = hex2dec(cf.get(image_confg_key, 'ReRncFlag'))
    fota_image.root_key_type = hex2dec(cf.get(image_confg_key, 'RootKeyType'))

def get_image_bin_final(image_bin, image_len):
    temp_offset1 = [0]
    image_bin_1 = bytearray(temp_offset1[0])
    image_bin_1[0:image_len] = image_bin
    tmp = image_len % 16

    if tmp != 0: # 对镜像长度按照16B对齐
        tmp_hash_bin = bytearray(16 - tmp)
        image_bin_1[image_len:image_len + 16 - tmp] = tmp_hash_bin
        image_len = image_len + 16 - tmp
    return image_bin_1, image_len

def fill_fota_image_secure_head(upg_bin, fota_image, cf, image_confg_key, image_bin):
    tmp_hash = make_sha256_unsecure_signature(image_bin)
    fota_image.image_hash[0:32] = tmp_hash[0:32]  # 填充fota_image.image_hash
    # 填充加密字段
    ReRncFlag = cf.get(image_confg_key, 'ReRncFlag')
    if ReRncFlag == "0x3C7896E1":
        fota_image_head_size = sizeof(fota_image_head)
        if cf.get(image_confg_key, 'ProtectionKeyL1') != "":
            data_byte1 = bytes.fromhex(cf.get(image_confg_key, 'ProtectionKeyL1'))
            fota_image.enc_pk_l1[0:16] = data_byte1
        if cf.get(image_confg_key, 'ProtectionKeyL2') != "":
            data_byte2 = bytes.fromhex(cf.get(image_confg_key, 'ProtectionKeyL2'))
            fota_image.enc_pk_l2[0:16] = data_byte2
        if cf.get(image_confg_key, 'Iv') != "":
            data_byte3 = bytes.fromhex(cf.get(image_confg_key, 'Iv'))
            fota_image.iv[0:16] = data_byte3

def create_encry_cfg(fota_obj, upg_align_file, upg_encry_file):
    remove_file(fota_obj.encry_conf_file)
    create_dirs(os.path.dirname(fota_obj.encry_conf_file))
    # 从基础文件中读取SIGN_CFG段，并写入到加密配置文件
    with open(fota_obj.encry_conf_file, 'w') as pro_file:
        for item in fota_obj.base_cfg.items("SIGN_CFG"):
            if item[0] == 'UpgImagePath':
                pro_file.write('SrcFile="%s"\n'%upg_align_file)
            elif item[0] == 'UpgSignedImagePath':
                pro_file.write('DstFile="%s"\n'%upg_encry_file)
            elif item[0] == 'SignSuite':
                pro_file.write('SignSuite=1\n')
        for item in fota_obj.base_cfg.items("application"):
            if item[0] == 'PlainKey':
                pro_file.write('PlainKey=%s\n'%item[1])
            if item[0] == 'Iv':
                pro_file.write('Iv=%s\n'%item[1])

def encry_ota_image(cf, image_confg_key, image_bin_1, fota_obj):
    image_bin_file = fota_obj.input_dict.get(image_confg_key)
    (filepath, tempfilename) = os.path.split(image_bin_file)
    (filename, extension) = os.path.splitext(tempfilename)

    upg_align_file = os.path.join(filepath, '%s.lzma.align'%filename) # 压缩并16字节对齐的文件
    upg_encry_file = os.path.join(filepath, '%s.lzma.encry'%filename) # 加密文件
    with open(upg_align_file, 'wb+') as fp:
        fp.write(image_bin_1)

    create_encry_cfg(fota_obj, upg_align_file, upg_encry_file)
    cmd = ' '.join([fota_obj.upg_tool, '5', '"%s"' % fota_obj.encry_conf_file])
    subprocess.run(cmd, shell=True)
    with open(upg_encry_file, 'rb') as fp:
        image_bin_1= fp.read()
    return image_bin_1

def fill_fota_image(cf, image_confg_key, all_image_bin, fota_obj, start_offset, image_offset):
    # 根据对应核的配置填充升级镜像头
    fota_image_head_size = sizeof(fota_image_head)
    upg_bin = bytearray(fota_image_head_size)
    fota_image = fota_image_head.from_buffer(upg_bin)
    fill_fota_image_head(fota_image, cf, image_confg_key, start_offset, image_offset, fota_image_head_size)

    # 根据升级文件方式标志，将当前镜像处理获得新镜像（压缩、差分、原镜像三者之一）
    image_bin, image_len= get_image_bin_data(fota_image, image_confg_key, fota_obj)
    print("old_image_len: ", fota_image.old_image_len,
          "new_image_len: ", fota_image.new_image_len,
          "recovery image_len: ", image_len)

    # 最终的镜像本体, 将镜像数据做对齐处理
    image_bin_1, image_bin_1_len = get_image_bin_final(image_bin, image_len)
    fota_image.image_len = image_bin_1_len

    # 加密
    ReRncFlag = cf.get(image_confg_key, 'ReRncFlag')
    if ReRncFlag == "0x3C7896E1":
        image_bin_1 = encry_ota_image(cf, image_confg_key, image_bin_1, fota_obj)

    # 头部安全填充
    fill_fota_image_secure_head(upg_bin, fota_image, cf, image_confg_key, image_bin_1)

    # 组合镜像
    # 1. 头部填充
    head_end = start_offset + fota_image_head_size
    all_image_bin[start_offset:head_end] = upg_bin

    # 2. 镜像本体填充
    img_end = head_end + image_bin_1_len
    all_image_bin[head_end:img_end] = image_bin_1
    return fota_image_head_size + image_bin_1_len

def make_upg(fota_obj):
    cf = fota_obj.base_cfg
    temp_offset = [0]
    upg_bin = bytearray(temp_offset[0])
    image_num = len(fota_obj.input_dict)

    # FOTA Key Area 填充
    # version字段暂来自配置文件，因此key area填充放到最前面
    fota_key_len, fota_key_bin_buff = fill_fota_key_area(cf, 0, 0)
    # fota_info区填充（除hash以外，hash数据最后再填充）
    fota_info_len, fota_info_bin_buff = fill_fota_info_area(cf, image_num, fota_obj)

    hash_node_offset = fota_key_len + fota_info_len
    all_hash_node_len = image_num * sizeof(fota_image_hash_node)
    tmp = all_hash_node_len % 16
    if tmp != 0:
        all_hash_node_len = all_hash_node_len + 16 - tmp
        # hash node 尾部多余长度
        tmp_hash_bin = bytearray(16 - tmp)

    image_offset = hash_node_offset + all_hash_node_len
    all_image_len = 0
    temp_offset1 = [0]
    all_image_bin = bytearray(temp_offset1[0])
    temp_offset2 = [0]
    all_hash_bin_buff = bytearray(temp_offset2[0])

    image_index = 0
    for image_confg_key in fota_obj.input_dict:
        #image_confg_key = 'IMAGE_' + str(i + 1)
        image_id = hex2dec(cf.get(image_confg_key, 'ImageId'))

        # 填充升级镜像头部结构，返回镜像总长度（压缩后的实际长度，不包含对齐的长度）和头结构的总长度、头部的非安全hash
        image_length = fill_fota_image(cf, image_confg_key, all_image_bin, fota_obj, all_image_len, image_offset)
        head_hash = make_sha256_unsecure_signature(all_image_bin[all_image_len : all_image_len + sizeof(fota_image_head)])
        # 计算下一个镜像的起始位置和偏移位置
        addr_offset = all_image_len + image_offset
        all_image_len = all_image_len + image_length
        print(image_confg_key, ':', image_length, ': addr_offset:', addr_offset)
        # 将计算出来的镜像头hash放到hash段的bin中，并初始化hash的结构
        fill_fota_image_hash_node(all_hash_bin_buff, image_id, image_index, head_hash, addr_offset, image_length)
        image_index += 1 # 镜像索引递增

    if tmp != 0:
        all_hash_bin_buff[all_hash_node_len - len(tmp_hash_bin):all_hash_node_len] = tmp_hash_bin

    # 计算hash table表hash值，填到fota_info_area
    hash_table_hash = make_sha256_unsecure_signature(all_hash_bin_buff)
    set_fota_info_area(fota_info_len, fota_info_bin_buff, hash_node_offset, all_hash_node_len, hash_table_hash)

    upg_bin[0:fota_key_len] = fota_key_bin_buff
    upg_bin[fota_key_len:fota_key_len + fota_info_len] = fota_info_bin_buff
    upg_bin[hash_node_offset:hash_node_offset + all_hash_node_len] = all_hash_bin_buff
    upg_bin[image_offset:image_offset + all_image_len] = all_image_bin
    remove_file(fota_obj.upg_image)
    create_dirs(os.path.dirname(fota_obj.upg_image))
    with open(fota_obj.upg_image, 'wb+') as fp:
        fp.write(upg_bin)
    # print_upg(upg_bin, fota_obj.upg_image)
    return

def print_upg_byte_list(pname, byte_in):
    length = len(byte_in)
    print("[%s=0x"%pname,  end="")
    for i in range(0, length):
        print("%02x"%byte_in[i],  end="")
    print("]")

def print_upg_byte(pname, byte_in):
    length = len(byte_in)
    print("[%s ="%pname)
    for i in range(0, length):
        if (i + 1) % 16 != 0:
            print("0x%02x, " % byte_in[i], end="")
        elif (i + 1) == length:
            print("0x%02x]" % byte_in[i])
        else:
            print("0x%02x, " % byte_in[i])

def print_fota_key_area(fota_key_head):
    print("[image_id=0x%x]" % (fota_key_head.image_id))
    print("[struct_version=0x%x]" % (fota_key_head.struct_version))
    print("[struct_length=0x%x]" % (fota_key_head.struct_length))
    print("[signature_length=0x%x]" % (fota_key_head.signature_length))
    print("[key_owner_id=0x%x]" % (fota_key_head.key_owner_id))
    print("[key_id=0x%x]" % (fota_key_head.key_id))
    print("[key_alg=0x%x]]" % (fota_key_head.key_alg))
    print("[ecc_curve_type=0x%x]" % (fota_key_head.ecc_curve_type))
    print("[key_length=0x%x]]" % (fota_key_head.key_length))
    print("[fota_key_version_ext=0x%x]" % (fota_key_head.fota_key_version_ext))
    print("[mask_fota_key_version_ext=0x%x]" % (fota_key_head.mask_fota_key_version_ext))
    print("[msid_ext=0x%x]" % (fota_key_head.msid_ext))
    print("[mask_msid_ext=0x%x]" % (fota_key_head.mask_msid_ext))
    print("[maintenance_mode=0x%x]" % (fota_key_head.maintenance_mode))
    print_upg_byte("die_id", fota_key_head.die_id)
    print("[fota_info_addr=0x%x]]" % (fota_key_head.fota_info_addr))
    #print_upg_byte("reserved", fota_key_head.reserved)
    print_upg_byte("fota_external_public_key", fota_key_head.fota_external_public_key)
    print_upg_byte("sig_fota_key_area", fota_key_head.sig_fota_key_area)

def print_fota_info_area(fota_info_head):
    print("[image_id=0x%x]" % (fota_info_head.image_id))
    print("[struct_version=0x%x]" % (fota_info_head.struct_version))
    print("[struct_length=0x%x]" % (fota_info_head.struct_length))
    print("[signature_length=0x%x]" % (fota_info_head.signature_length))
    print("[fota_version_ext=0x%x]" % (fota_info_head.fota_version_ext))
    print("[mask_fota_version_ext=0x%x]" % (fota_info_head.mask_fota_version_ext))
    print("[msid_ext=0x%x]" % (fota_info_head.msid_ext))
    print("[mask_msid_ext=0x%x]" % (fota_info_head.mask_msid_ext))
    print("[image_hash_table_addr=0x%x]" % (fota_info_head.image_hash_table_addr))
    print("[image_hash_table_length=0x%x]" % (fota_info_head.image_hash_table_length))
    #print_upg_byte("image_hash_table_hash", fota_info_head.image_hash_table_hash)
    print_upg_byte_list("image_hash_table_hash", fota_info_head.image_hash_table_hash)
    print("[image_num=0x%x]" % (fota_info_head.image_num))
    print("[hardware_id=0x%x]" % (fota_info_head.hardware_id))
    #print_upg_byte("reserved", fota_info_head.reserved)
    print_upg_byte("sign_fota_info", fota_info_head.sign_fota_info)

def print_fota_image_hash_table(fota_image_hash_table):
    print("[image_id=0x%x]" % (fota_image_hash_table.image_id))
    print("[image_addr=0x%x]" % (fota_image_hash_table.image_addr))
    print("[image_length=0x%x]" % (fota_image_hash_table.image_length))
    #print_upg_byte("image_hash", fota_image_hash_table.image_hash)
    print_upg_byte_list("image_hash", fota_image_hash_table.image_hash)

def print_fota_image_head(image_head):
    print("[header_magic=0x%x]" % (image_head.header_magic))
    print("[image_id=0x%x]" % (image_head.image_id))
    print("[image_offset=0x%x]" % (image_head.image_offset))
    print("[image_len=0x%x]" % (image_head.image_len))
    print("[new_image_len=0x%x]" % (image_head.new_image_len))
    #print_upg_byte("new_image_hash", image_head.new_image_hash)
    print_upg_byte_list("image_hash", image_head.image_hash)
    print("[old_image_len=0x%x]" % (image_head.old_image_len))
    print_upg_byte_list("old_image_hash", image_head.old_image_hash)
    #print_upg_byte("old_image_hash", image_head.old_image_hash)
    print("[version_ext=0x%x]" % (image_head.version_ext))
    print("[version_mask=0x%x]" % (image_head.version_mask))
    print("[decompress_flag=0x%x]" % (image_head.decompress_flag))
    print("[root_key_type=0x%x]" % (image_head.root_key_type))
    print("[re_enc_flag=0x%x]" % (image_head.re_enc_flag))
    print_upg_byte("enc_pk_l1", image_head.enc_pk_l1)
    print_upg_byte("enc_pk_l2", image_head.enc_pk_l2)
    print_upg_byte("iv", image_head.iv)
    #print_upg_byte("padding", image_head.padding)

def print_upg(upg_bin, image_file):
    print("upg_bin_len %s"%len(upg_bin))
    print("-------------%s fota_key_area start-------------" % (image_file))
    fota_key_head = fota_key_area_data.from_buffer(upg_bin)
    key_area_len = fota_key_head.struct_length
    print_fota_key_area(fota_key_head)
    print("-------------%s fota_info_area start-------------" % (image_file))
    if fota_key_head.fota_info_addr == 0:
        fota_info_offset = key_area_len
    else:
        fota_info_offset = fota_key_head.fota_info_addr
    fota_info_head = fota_info_area_data.from_buffer(upg_bin[fota_info_offset:])
    print_fota_info_area(fota_info_head)

    image_num = fota_info_head.image_num
    hash_node_size = sizeof(fota_image_hash_node)
    if fota_info_head.image_hash_table_addr == 0:
        hash_table_offset = fota_info_offset + key_area_len
    else:
        hash_table_offset = fota_info_head.image_hash_table_addr

    for i in range(0, image_num):
        print("-------------fota hash table start NO.%02d-------------" % (i + 1))
        hash_table_info = fota_image_hash_node.from_buffer(upg_bin[hash_table_offset + i * hash_node_size:])
        print_fota_image_hash_table(hash_table_info)
        print("-------------fota image head start NO.%02d-------------" % (i + 1))
        image_head_offset = hash_table_info.image_addr
        image_head = fota_image_head.from_buffer(upg_bin[image_head_offset:])
        print_fota_image_head(image_head)

def get_parameters():
    parser = argparse.ArgumentParser()

    parser.add_argument('-app_name', type=str, default='',
                        help='生成镜像的应用名')

    parser.add_argument('-upg_format_path', type=str, default='',
                        help='升级包结构配置文件')

    parser.add_argument('-base', type=str, default='',
                        help='基础配置文件路径')

    parser.add_argument('-temp_dir', type=str, default='',
                        help='临时配置文件路径')

    parser.add_argument('-old_images', type=str, default='',
                        help='老版本镜像文件路径')

    parser.add_argument('-new_images', type=str, default='',
                        help='新版本镜像文件路径')

    parser.add_argument('-output_dir', type=str, default='',
                        help='输出文件路径')

    parser.add_argument('-type', type=int, default=0,
                        help='执行类型')

    parser.add_argument('-sign_tool', type=int, default=0,
                        help='签名工具选择 0-本地签名工具， 1-在线签名工具')
    config = parser.parse_args()
    return config

config_item = ['SignSuite', 'RootKeyFile', 'SubKeyFile', 'SrcFile', 'DstFile']
class Properties:
    def getProperties(self, dir):
        # 处理升级配置文件，获取有效配置，将有效配置保存到字典中
        # 各个镜像的配置不一样，这部分也被过滤掉了，后面要怎么区分？
        #########################################################################
        properties = {}
        with open(dir, 'r', encoding='utf-8') as pro_file:
            for line in pro_file:
                if line.find('=') > 0 and '#' not in line:
                    strs = line.replace('\n', '').split('=')
                    properties[strs[0]] = strs[1]
        return properties

    # 获取签名相关的配置
    def setProperties(self, dir, properties):
        with open(dir, 'w') as pro_file:
            for item in properties:
                if item in config_item:
                    pro_file.write("{}={}\n".format(item, properties[item]))

def remove_file(path):
    if os.path.exists(path):  # 如果文件存在
        # 删除文件，可使用以下两种方法。
        os.remove(path)
def create_dirs(path):
    os.makedirs(path, exist_ok=True)

def print_fota_file(image_file):
    with open(image_file, 'rb') as fp:
        upg_bin = fp.read()
    print_upg_byte(image_file, upg_bin)


class myconf(configparser.ConfigParser):
    def __init__(self, defaults=None):
        configparser.ConfigParser.__init__(self, defaults=defaults)

    # 这里重写了optionxform方法，直接返回选项名
    def optionxform(self, optionstr):
        return optionstr

# def create_upg_cfg(base, input, conf_dir):
def create_upg_cfg(fota_obj):
    img_cfg = copy.deepcopy(fota_obj.base_cfg)
    cf_list = img_cfg.sections() # 获取所有section，返回值为list

    input_cf_key_list = []
    input_cf_key_list.append('FOTA_KEY_AREA')
    input_cf_key_list.append('FOTA_INFO_AREA')

    # 将输入的段加入到列表
    for image_confg_key in fota_obj.input_dict:
        input_cf_key_list.append(image_confg_key)

    for item in cf_list:
        # 将基础配置文件中，和输入无关的段删除
        if item not in input_cf_key_list:
            img_cfg.remove_section(item)

    remove_file(fota_obj.image_conf_file)
    print(fota_obj.image_conf_file)
    print(os.path.dirname(fota_obj.image_conf_file))
    create_dirs(os.path.dirname(fota_obj.image_conf_file))
    with open(fota_obj.image_conf_file, 'w') as configfile:
        img_cfg.write(configfile)

def create_sign_cfg(fota_obj):
    remove_file(fota_obj.sign_conf_file)
    create_dirs(os.path.dirname(fota_obj.sign_conf_file))
    # 从基础文件中读取SIGN_CFG段，并写入到签名配置文件
    with open(fota_obj.sign_conf_file, 'w') as pro_file:
        for item in fota_obj.base_cfg.items("SIGN_CFG"):
            if item[0] == 'UpgImagePath':
                pro_file.write('SrcFile="%s"\n'%fota_obj.upg_image)
            elif item[0] == 'UpgSignedImagePath':
                pro_file.write('DstFile="%s"\n'%fota_obj.upg_signed_image)
            elif 'KeyFile' in item[0]:
                pro_file.write('%s="%s"\n'% (item[0], os.path.join(fota_obj.root_path, item[1])))
            else:
                pro_file.write('%s=%s\n'%item)
        for item in fota_obj.base_cfg.items("FOTA_KEY_AREA"):
            if item[0] == 'ImageId':
                pro_file.write('%s=%s\n'%item)

def check_fota_image_id(fota_file):
    # 各个镜像的image_id需保证不一致
    cf = myconf()
    cf.read(fota_file, encoding='utf-8')
    cf_list = cf.sections()
    image_id_dict = {}
    no_sections = ['FOTA_KEY_AREA', 'FOTA_INFO_AREA']
    if cf.get('FOTA_KEY_AREA', 'ImageId') != cf.get('FOTA_INFO_AREA', 'ImageId'):
        msg = "FOTA_KEY_AREA and FOTA_INFO_AREA have different ImageId.\n"
        sys.exit(msg)
    for section in cf_list:
        if section in no_sections:
            continue
        image_id = cf.get(section, 'ImageId')
        if image_id in image_id_dict:
            msg = "The ImageId of %s and %s are the same.\n" % (section, image_id_dict.get(image_id))
            sys.exit(msg)
        else:
            image_id_dict[image_id] = section

def check_image_key(input_dict):
    # 检查nv是否与主程序一起打包
    pkg_bin_list = input_dict.keys()
    if 'nv' in pkg_bin_list and 'application' not in pkg_bin_list:
        msg = "The NV cannot be packed separately.It has to be with application!\n"
        sys.exit(msg)

    # 检查索引文件与资源文件是否在一起打包
    if 'res_data' in pkg_bin_list and 'res_index' not in pkg_bin_list:
        msg = "The res_index file and res_data should be together.\n"
        sys.exit(msg)

def sign_upg_file(fota_obj):
    if fota_obj.sign_tool == 1:
        # 在线签名，将不会在fota_obj.output_dir下生成最终升级包文件
        # 需usr自行对fota_obj.temp_dir下的未签名文件update_temp.fwpkg进行签名
        return
    cmd = ' '.join([fota_obj.upg_tool, '3', '"%s"' % fota_obj.sign_conf_file])
    subprocess.run(cmd, shell=True)

class fota_info:
    def __init__(self, user_input):
        self.root_path = g_root
        self.app_name = user_input.app_name
        self.temp_dir = user_input.temp_dir
        self.output_dir = user_input.output_dir
        self.image_conf_file = os.path.join(self.temp_dir, '%s.cfg' % user_input.app_name) # 升级镜像的配置文件
        self.sign_conf_file = os.path.join(self.temp_dir, '%s_sign.cfg' % user_input.app_name) # 升级镜像的签名配置文件
        self.encry_conf_file = os.path.join(self.temp_dir, '%s_encry.cfg' % user_input.app_name) # 升级镜像的加密配置文件
        if hasattr(user_input, 'sign_tool'):
            self.sign_tool = user_input.sign_tool
        else:
            self.sign_tool = 0

        if hasattr(user_input, 'hardware_id'):
            self.hardware_id = user_input.hardware_id

        self.base_cfg = myconf()
        self.base_cfg.read(user_input.base, encoding='utf-8')
        self._set_upg_image_path()
        self._set_tool_path()

        self.input_dict = self._split_input_info(user_input.new_images)
        self.original_dict = self._split_input_info(user_input.old_images)
        print("input_dict:", self.input_dict)
        print("original_dict:", self.original_dict)
        self.diff_image_info = self._get_image_path_for_diff_image(self.input_dict, self.original_dict) # 差分镜像信息

    def _set_tool_path(self):
        if platform.system().lower() == "linux":
            self.upg_tool = self.base_cfg.get('TOOLS', 'UpgToolPath')
            self.lzma_tool = self.base_cfg.get('TOOLS', 'LzmaToolPath')
        elif platform.system().lower() == "windows":
            self.upg_tool = self.base_cfg.get('TOOLS', 'UpgToolWinPath')
            self.lzma_tool = self.base_cfg.get('TOOLS', 'LzmaToolWinPath')
        self.upg_tool = '"%s"' % os.path.join(self.root_path, self.upg_tool)
        self.lzma_tool = '"%s"' % os.path.join(self.root_path, self.lzma_tool)
        print(self.upg_tool)
        print(self.lzma_tool)

    def _set_upg_image_path(self):
        self.upg_image = self.base_cfg.get('SIGN_CFG', 'UpgImagePath')
        self.upg_signed_image = self.base_cfg.get('SIGN_CFG', 'UpgSignedImagePath')
        if self.upg_image == '':
            self.upg_image = os.path.join(self.temp_dir, '%s_temp.fwpkg' % self.app_name)
        if self.upg_signed_image == '':
            self.upg_signed_image = os.path.join(self.output_dir, '%s.fwpkg' % self.app_name)

    def _split_input_info(self, input):
        input = input.replace('\n','').replace('\r','').replace('\t','')
        if input == '':
            return None

        bin_list = input.split('|')
        image_num = len(bin_list)
        bin_dict = {}
        res_data = ""
        for i in range(0, image_num):
            bin_file, image_confg_key = bin_list[i].split('=')
            if image_confg_key == "res_data":
                res_data = bin_file
            else:
                bin_dict[image_confg_key] = bin_file

        if res_data != "":
            bin_dict["res_data"] = res_data

        return bin_dict

    def _get_image_path_for_diff_image(self, new_version_dict, org_version_dict):
        # 解析各个镜像的老版本镜像路径、新版本路径，并一一对应
        # 制作差分升级文件的镜像需要提供老版本文件路径，提供老版本的同时也必须提供对应的新版本路径
        if org_version_dict == None:
            return None

        image_ref = {}
        for image_confg_key in org_version_dict:
            if image_confg_key in image_ref:
                sys.exit("[ERROR] old_images bin param repeated!!!")
            image_ref[image_confg_key] = [
                org_version_dict[image_confg_key], # 存放老镜像路径
                "", # 存放新镜像路径
                ""  # 存放镜像路径的前缀，取新路径前缀
                ]

        for image_confg_key in new_version_dict:
            if image_confg_key in image_ref:
                image_ref[image_confg_key][1] = new_version_dict[image_confg_key] # 后存放新版本镜像到1
                image_ref[image_confg_key][2] = os.path.splitext(os.path.basename(image_ref[image_confg_key][0]))[0]

        for key in image_ref:
            if image_ref[key][1] == "":
                sys.exit("[ERROR] not find a matched image bin!!!")
        return image_ref

def create_fota_file(user_input):
    fota_format_init(user_input.upg_format_path)
    fota_obj = fota_info(user_input)
    print("root_path:", fota_obj.root_path)
    # 检查打包镜像限制
    check_image_key(fota_obj.input_dict)
    # 生成镜像配置文件
    create_upg_cfg(fota_obj)
    # 镜像ID校验
    check_fota_image_id(fota_obj.image_conf_file)
    # 升级文件制作
    make_upg(fota_obj)
    # 生成签名配置文件
    create_sign_cfg(fota_obj)
    # 升级文件签名
    sign_upg_file(fota_obj)

def begin(user_input):
    if user_input.type == 0:
        create_fota_file(user_input)
    else:
        print_fota_file(user_input.new_images)

if __name__ == "__main__":
    user_input = get_parameters()
    begin(user_input)
