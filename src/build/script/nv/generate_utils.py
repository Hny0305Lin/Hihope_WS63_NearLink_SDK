#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.

"""
* Description: Utilities of compile system.
* Change: 2023-3-17
"""
import os
import sys
import glob

def generate_nv_source(input, output):
    if input == 'noneed':
        headers = glob.glob(os.path.join("*.h"))
    else:
        headers = glob.glob(os.path.join(input, "*.h"))
    # dummy C file that includes all the required headers to get structure defs.
    lines = []
    for header in headers:
        lines.append('#include <%s>' % os.path.basename(header))

    with open(output, "w") as f:
        f.write("\n".join(lines)) # for compatible representation to old implementation.

def generate_nv_dir(nv_c_path):
    if not os.path.exists(nv_c_path):
        os.makedirs(nv_c_path)

if __name__ == "__main__":
    if sys.argv[1] == 'NV':
        generate_nv_source(sys.argv[2], sys.argv[3])
    if sys.argv[1] == 'MKPATH':
        generate_nv_dir(sys.argv[2])