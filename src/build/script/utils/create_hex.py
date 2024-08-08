#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.

import sys
import struct


def create_hex_file(_bin, _hex):
    with open(str(_bin), "rb") as binfile, open(
                str(_hex), "wb") as hexfile:
        while True:
            bindata = binfile.read(4)
            if not bindata:
                break
            if len(bindata) != 4:
                bindata = bindata + b'\x00' * (4 - len(bindata))
            longdata, = struct.unpack("<L", bindata)
            hexstr = '{:x}'.format(longdata)  # dec to hex number str
            hexstr = '%s\r\n' % '{:0>8}'.format(hexstr).upper()
            hexfile.write(str.encode(hexstr))

if __name__ == "__main__":
    create_hex_file(sys.argv[1], sys.argv[2])
