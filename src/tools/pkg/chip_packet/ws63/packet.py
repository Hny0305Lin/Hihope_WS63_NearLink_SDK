#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    packet files
# ============================================================================

import os
import sys
import shutil
import subprocess
import platform
import tarfile
from typing import List

PY_PATH = os.path.dirname(os.path.realpath(__file__))
sys.path.append(PY_PATH)
PKG_DIR = os.path.dirname(PY_PATH)
PKG_DIR = os.path.dirname(PKG_DIR)

from packet_create import create_sha_file
from packet_create import packet_bin

TOOLS_DIR = os.path.dirname(PKG_DIR)
SDK_DIR = os.path.dirname(TOOLS_DIR)
sys.path.append(os.path.join(SDK_DIR, "build", "script"))
sys.path.append(os.path.join(SDK_DIR, "build", "config"))
from enviroment import TargetEnvironment

def get_file_size(file_path: str)->int:
    try:
        return os.stat(file_path).st_size
    except BaseException as e:
        print(e)
        exit(-1)


def create_tar(source_dir, output_filename):
    with tarfile.open(output_filename, "w") as tar:
        tar.add(source_dir)


# ws63
def make_all_in_one_packet(pack_style_str, extr_defines):
    # make all in one packet
    boot_bin_dir = os.path.join(SDK_DIR, "output", "ws63", "acore", "boot_bin")
    param_bin_dir = os.path.join(SDK_DIR, "output", "ws63", "acore", "param_bin")
    nv_bin_dir = os.path.join(SDK_DIR, "output", "ws63", "acore", "nv_bin")
    efuse_bin_dir = os.path.join(SDK_DIR, "output", "ws63", "acore", "boot_bin")

    # loader boot
    loadboot_bin = os.path.join(boot_bin_dir, "root_loaderboot_sign.bin")
    loadboot_bx = loadboot_bin + "|0x0|0x200000|0"

    # secure stage boot
    ssb_bin = os.path.join(boot_bin_dir, "ssb_sign.bin")
    ssb_bx = ssb_bin + f"|0x202000|{hex(get_file_size(ssb_bin))}|1"

    # flash boot
    flashboot_bin = os.path.join(boot_bin_dir, "flashboot_sign.bin")
    flashboot_bx = flashboot_bin + f"|0x220000|{hex(get_file_size(flashboot_bin))}|1"

    # flash boot backup
    flashboot_backup_bin = os.path.join(boot_bin_dir, "flashboot_backup_sign.bin")
    flashboot_backup_bx = flashboot_backup_bin + f"|0x210000|{hex(get_file_size(flashboot_backup_bin))}|1"

    # params
    params_bin = os.path.join(param_bin_dir, "root_params_sign.bin")
    params_bx = params_bin + f"|0x200000|{hex(get_file_size(params_bin))}|1"

    # nv
    nv_bin = os.path.join(nv_bin_dir, "ws63_all_nv.bin")
    nv_bx = nv_bin + f"|0x5FC000|0x4000|1"

    # nv backup
    nv_backup_bin = os.path.join(nv_bin_dir, "ws63_all_nv_backup.bin")
    nv_backup_bx = nv_backup_bin + f"|0x20C000|0x4000|1"

    # hilink
    target_env = TargetEnvironment(pack_style_str)
    defines = target_env.get('defines')
    if "CONFIG_SUPPORT_HILINK_INDIE_UPGRADE" in defines:
        hilink_bin_dir = os.path.join(SDK_DIR, "interim_binary", "ws63", "bin", "hilink_bin")
        hilink_bin = os.path.join(hilink_bin_dir, "ws63-liteos-hilink-sign.bin")
        hilink_bx = hilink_bin + f"|0x406000|{hex(get_file_size(hilink_bin))}|1"

    # efuse bin
    efuse_bin = os.path.join(efuse_bin_dir, "efuse_cfg.bin")
    efuse_bx = efuse_bin + "|0x0|0x200000|3"

    # app and rom
    app_templat_bx = '%s|0x230000|%s|1'

    # 输出目录
    fwpkg_outdir = os.path.join(SDK_DIR, "output", "ws63", "fwpkg", pack_style_str)

    # 键为target名，值为rom bin、app bin的名字
    available_targets_map = {
        'ws63-liteos-testsuite': [
            'ws63-liteos-testsuite_rom.bin',
            'ws63-liteos-testsuite-sign.bin',
        ],
        'ws63-liteos-testsuite-asic': [
            'ws63-liteos-testsuite-asic_rom.bin',
            'ws63-liteos-testsuite-asic-sign.bin',
        ],
        'ws63-freertos-testsuite': [
            'ws63-freertos-testsuite_rom.bin',
            'ws63-freertos-testsuite-sign.bin',
        ],
        'ws63-alios-testsuite': [
            'ws63-alios-testsuite_rom.bin',
            'ws63-alios-testsuite-sign.bin',
        ],
        'ws63-freertos-app': [
            'ws63-freertos-app_rom.bin',
            'ws63-freertos-app-sign.bin',
        ],
        'ws63-alios-app': [
            'ws63-alios-app_rom.bin',
            'ws63-alios-app-sign.bin',
        ],
        'ws63-liteos-bgle-all': [
            'ws63-liteos-bgle-all_rom.bin',
            'ws63-liteos-bgle-all-sign.bin',
        ],
        'ws63-liteos-bgle-all-asic': [
            'ws63-liteos-bgle-all-asic_rom.bin',
            'ws63-liteos-bgle-all-asic-sign.bin',
        ],
        'ws63-liteos-btc-only':[
            'ws63-liteos-btc-only_rom.bin',
            'ws63-liteos-btc-only-sign.bin',
        ],
        'ws63-liteos-btc-only-asic':[
            'ws63-liteos-btc-only-asic_rom.bin',
            'ws63-liteos-btc-only-asic-sign.bin',
        ],
        'ws63-liteos-gle-sparklyzer': [
            'ws63-liteos-gle-sparklyzer_rom.bin',
            'ws63-liteos-gle-sparklyzer-sign.bin',
        ],
        'ws63-freertos-btc-only': [
            'ws63-freertos-btc-only_rom.bin',
            'ws63-freertos-btc-only-sign.bin',
        ],
        'ws63-alios-btc-only': [
            'ws63-alios-btc-only_rom.bin',
            'ws63-alios-btc-only-sign.bin',
        ],
        'ws63-liteos-app': [
            'ws63-liteos-app_rom.bin',
            'ws63-liteos-app-sign.bin',
        ],
        'ws63-liteos-perf': [
            'ws63-liteos-perf_rom.bin',
            'ws63-liteos-perf-sign.bin',
        ],
        'ws63-liteos-app-asic': [
            'ws63-liteos-app-asic_rom.bin',
            'ws63-liteos-app-asic-sign.bin',
        ],
        'ws63-liteos-mfg': [
            'ws63-liteos-mfg_rom.bin',
            'ws63-liteos-mfg-sign.bin',
        ],
        'ws63-liteos-app-gree': [
            'ws63-liteos-app-gree.bin',
            'ws63-liteos-app-gree-sign.bin',
        ],
        'ws63-liteos-msmart': [
            'ws63-liteos-msmart_rom.bin',
            'ws63-liteos-msmart-sign.bin',
        ],
        'ws63-liteos-msmart-xts': [
            'ws63-liteos-msmart-xts_rom.bin',
            'ws63-liteos-msmart-xts-sign.bin',
        ],
        'ws63-liteos-xts': [
            'ws63-liteos-xts_rom.bin',
            'ws63-liteos-xts-sign.bin',
        ],
        'ws63-liteos-app-iot': [
            'ws63-liteos-app-iot_rom.bin',
            'ws63-liteos-app-iot-sign.bin',
        ],
        'ws63-liteos-testsuite-radar': [
            'ws63-liteos-testsuite-radar.bin',
            'ws63-liteos-testsuite-radar-sign.bin',
        ]
    }

    if pack_style_str in available_targets_map:
        # 生成前清空
        if os.path.exists(fwpkg_outdir):
            shutil.rmtree(fwpkg_outdir)
        os.makedirs(fwpkg_outdir)

        rom_bin = os.path.join(SDK_DIR, "output", "ws63", "acore", pack_style_str, available_targets_map[pack_style_str][0])
        app_bin = os.path.join(SDK_DIR, "output", "ws63", "acore", pack_style_str, available_targets_map[pack_style_str][1])
        for fpath in (rom_bin, app_bin):
            if not os.path.isfile(fpath):
                print(f'[!] warning: File `{fpath}` is not exists !Skip fwpkg generate for target `{pack_style_str}` !')
                return

        app_bx = app_templat_bx % (app_bin, hex(get_file_size(app_bin)))

        if pack_style_str == 'ws63-liteos-mfg':
            output_bin_dir = os.path.join(SDK_DIR, "output", "ws63", "acore")
            packet_post_agvs = list()
            packet_post_agvs.append(loadboot_bx)
            packet_post_agvs.append(params_bx)
            packet_post_agvs.append(ssb_bx)
            packet_post_agvs.append(flashboot_bx)
            packet_post_agvs.append(flashboot_backup_bx)
            packet_post_agvs.append(nv_bx)
            packet_post_agvs.append(nv_backup_bx)
            app_bin = os.path.join(output_bin_dir, "ws63-liteos-app", "ws63-liteos-app-sign.bin")
            app_bx = app_bin + f"|0x230000|{hex(get_file_size(app_bin))}|1"

            mfg_bin = os.path.join(output_bin_dir, "ws63-liteos-mfg", "ws63-liteos-mfg-sign.bin")
            mfg_bx = mfg_bin + f"|0x470000|{hex(0x183000)}|1" # 0x183000为产测分区B区大小
            packet_post_agvs.append(app_bx)
            packet_post_agvs.append(mfg_bx)
            fpga_fwpkg = os.path.join(fwpkg_outdir, f"{pack_style_str}_all.fwpkg")
            packet_bin(fpga_fwpkg, packet_post_agvs)
            return

        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(params_bx)
        packet_post_agvs.append(ssb_bx)
        packet_post_agvs.append(flashboot_bx)
        packet_post_agvs.append(flashboot_backup_bx)
        packet_post_agvs.append(nv_bx)
        packet_post_agvs.append(nv_backup_bx)
        if "CONFIG_SUPPORT_HILINK_INDIE_UPGRADE" in defines:
            packet_post_agvs.append(hilink_bx)
        packet_post_agvs.append(app_bx)
        if "SUPPORT_EFUSE" in extr_defines or "PACKET_MFG_BIN" in extr_defines:
            print("efuse pack")
            packet_post_agvs.append(efuse_bx)

        if "PACKET_MFG_BIN" in extr_defines:
            mfg_sign_bin = os.path.join(SDK_DIR, "output", "ws63", "acore", "boot_bin", "ws63-liteos-mfg-sign.bin")
            if os.path.exists(mfg_sign_bin):
                mfg_bx = mfg_sign_bin + f"|0x470000|{hex(0x183000)}|1" # 0x183000为产测分区B区大小
                packet_post_agvs.append(mfg_bx)
            else:
                print("warning: don't find ws63-liteos-mfg-sign.bin...")
        else:
            if os.path.exists(os.path.join(SDK_DIR, "output", "ws63", "pktbin", "ws63-liteos-mfg.bin")):
                os.remove(os.path.join(SDK_DIR, "output", "ws63", "pktbin", "ws63-liteos-mfg.bin"))

        fpga_fwpkg = os.path.join(fwpkg_outdir, f"{pack_style_str}_all.fwpkg")
        packet_bin(fpga_fwpkg, packet_post_agvs)

        packet_post_agvs = list()
        packet_post_agvs.append(loadboot_bx)
        packet_post_agvs.append(app_bx)
        fpga_loadapp_only_fwpkg = os.path.join(fwpkg_outdir, f"{pack_style_str}_load_only.fwpkg")
        packet_bin(fpga_loadapp_only_fwpkg, packet_post_agvs)

        if "windows" in platform.system().lower():
            os.chdir(os.path.join(SDK_DIR, "output", "ws63"))
            create_tar('./pktbin', 'pktbin.zip')
        else:
            print("not windows.")
            subprocess.run(["tar", "-cf", "pktbin.zip", "./pktbin"], cwd=os.path.join(SDK_DIR, "output", "ws63"))


def is_packing_files_exist(soc, pack_style_str):
    return
