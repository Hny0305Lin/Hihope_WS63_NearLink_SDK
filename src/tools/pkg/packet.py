#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    packet files
# ============================================================================

import os
import sys
import importlib

sys.path.append(os.path.dirname(os.path.realpath(__file__)))
sys.dont_write_bytecode = True

def main():
    arg_ls = sys.argv
    build_soc = sys.argv[1]
    build_target = sys.argv[2]
    build_extr_defines = sys.argv[3]

    load_fmt = "chip_packet.%s.packet" %build_soc
    load_mod = importlib.import_module(load_fmt)

    lost_file = load_mod.is_packing_files_exist(build_soc, build_target)
    if lost_file:
        lost = ";".join(lost_file)
        print(f"cannot find {lost}")
        exit(-1)
    if build_soc in ["ws63"]:
        load_mod.make_all_in_one_packet(build_target, build_extr_defines)
    else:
        load_mod.make_all_in_one_packet(build_target)

if __name__ == "__main__":
    main()
