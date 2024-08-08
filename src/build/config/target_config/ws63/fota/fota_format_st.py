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

import struct
from ctypes import *

###############################Defining Basic Types###############################################
td_char = c_char
td_s8 = c_byte
td_u8 = c_ubyte
td_s16 = c_short
td_u16 = c_ushort
td_s32 = c_int
td_u32 = c_uint

fota_key_area_data_fields = [
    ("image_id", td_u32),
    ("struct_version", td_u32),
    ("struct_length", td_u32),
    ("signature_length", td_u32),
    ("key_owner_id", td_u32),
    ("key_id", td_u32),
    ("key_alg", td_u32),
    ("ecc_curve_type", td_u32),
    ("key_length", td_u32),
    ("fota_key_version_ext", td_u32),
    ("mask_fota_key_version_ext", td_u32),
    ("msid_ext", td_u32),
    ("mask_msid_ext", td_u32),
    ("maintenance_mode", td_u32),
    ("die_id", td_u8 * 16),
    ("fota_info_addr", td_u32),
    ("reserved", td_u8 * 52),
    ("fota_external_public_key", td_u8 * 64),
    ("sig_fota_key_area", td_u8 * 64),
]

fota_info_area_data_fields = [
    ("image_id", td_u32),
    ("struct_version", td_u32),
    ("struct_length", td_u32),
    ("signature_length", td_u32),
    ("fota_version_ext", td_u32),
    ("mask_fota_version_ext", td_u32),
    ("msid_ext", td_u32),
    ("mask_msid_ext", td_u32),
    ("image_hash_table_addr", td_u32),
    ("image_hash_table_length", td_u32),
    ("image_hash_table_hash", td_u8 * 32),
    ("image_num", td_u32),
    ("hardware_id", td_u32),
    ("reserved", td_u8 * 112),
    ("sign_fota_info", td_u8 * 64),
]