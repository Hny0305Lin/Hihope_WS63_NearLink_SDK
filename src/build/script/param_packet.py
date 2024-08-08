#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.

import os
import sys
import struct
sys.path.append(os.path.dirname(__file__))
from conf_parser import BuildConfParser, ParserError
from utils.build_utils import fn_str_to_int

##
##  Params Area Stucture
##
##  Image_ID:       4B
##  Struct_Version: 4B
##  Version:        4B
##  Item_Offset:    2B
##  Item_Count:     1B
##  ItemID_List:    Item_Count
##  Padding_Area:   Item_Offset - 15 - Item_Count
##  Item_List:      (4B + 4B) * Item_Count
##
def param_area(dst_file, image_id, stru_ver, version, param_info) -> None:

    item_num = len(param_info)
    item_offset = 15 + item_num
    item_offset = int((item_offset + 3) / 4) * 4

    ## image_id, stru_ver, version, item_offset, item_num
    bin_size = 1024

    binary = struct.pack("<IIIHB", image_id, stru_ver, version, item_offset, item_num)

    ## item id list
    for item in param_info:
        binary += struct.pack("<B", item[0])

    ## pad for 4-byte-align
    for i in range(0, item_offset - 15 - item_num):
        binary += struct.pack("<B", 0x00)

    ## item list
    for item in param_info:
        binary += struct.pack("<II", item[1], item[2])

    binary += bytearray(bin_size - len(binary))
    # print("len", len(binary), binary)

    ## write to file
    with open(dst_file, "wb") as f:
        f.write(binary)

    print("Build params success!")

def gen_flash_part_bin(param_file, dst_file):
    partition_cfg = BuildConfParser(param_file).get_conf_data().get("Partition_Tbl")
    if partition_cfg is None:
        msg = "[error] flash partiton config NULL:[%s]!" % param_file
        raise ParserError(msg)
    image_id = partition_cfg.get("image_id")
    stru_ver = partition_cfg.get("stru_ver")
    version  = partition_cfg.get("version")
    param_info = partition_cfg.get("param_info")
    if image_id is None or stru_ver is None or version is None or param_info is None:
        msg = "[error] flash partiton config err:[%s]!" % param_file
        raise ParserError(msg)

    table = []
    for param in param_info:
        if len(param) != 3:
            msg = "[error] flash partiton table err:[%s]!" % param_file
            raise ParserError(msg)
        table.append([fn_str_to_int(x) for x in param])
    param_area(dst_file, fn_str_to_int(image_id), fn_str_to_int(stru_ver), fn_str_to_int(version), table)

## python3 param_packet.py XXX.json YYY.bin
def test():
    param_file = sys.argv[1]
    dst_file = sys.argv[2]
    gen_flash_part_bin(param_file, dst_file)

if __name__ == '__main__':
    test()
