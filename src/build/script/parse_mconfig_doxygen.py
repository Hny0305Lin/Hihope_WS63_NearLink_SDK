#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
import sys

if __name__ == "__main__":
    mconfig_path = sys.argv[1]
    with open(mconfig_path, 'r') as fp:
        lines = fp.readlines()
        for line in lines:
            if '#define' in line:
                defs = line.split()
                print('%s=%s \\' % (defs[1], defs[2]))