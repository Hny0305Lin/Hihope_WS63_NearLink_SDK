#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Build Update Pkg File
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# ============================================================================

import os
import sys
import argparse
import shutil
import subprocess

file_dir = os.path.dirname(os.path.realpath(__file__))
g_root = os.path.realpath(os.path.join(file_dir, "..", "..", "..", ".."))
sys.path.append(os.path.join(g_root, 'build', 'script'))
from build_upg_pkg import begin

sys.path.append(os.path.join(g_root, "build", "config"))
sys.path.remove(os.path.join(g_root, 'build', 'config', "target_config", "ws63"))
from enviroment import TargetEnvironment

class upg_base_info:
    def __init__(self):
        self.root_path = g_root
        # 升级包结构配置文件
        self.fota_format_path = os.path.join(self.root_path, "build", "config", "target_config", "ws63", "fota")
        # 产品升级配置文件
        self.fota_cfg = os.path.join(self.root_path, "build", "config", "target_config", "ws63", "fota", "fota.cfg")
        # 产品镜像输出路径
        self.output = os.path.join(self.root_path, "output", "ws63")
        # 产品升级镜像包输出路径
        self.upg_output = os.path.join(self.output, "upgrade")
        # 产品签名加密前原始镜像
        self.upg_pkt = os.path.join(self.upg_output, "ota_pktbin")
        self.upg_pkt_zip = os.path.join(self.upg_output, "ota_pktbin.zip")
        # 产品升级制作临时文件输出路径
        self.temp_dir = os.path.join(self.upg_output, "temp_dir")

        # 产品镜像路径
        self.flashboot = os.path.join(self.root_path, "output", "ws63", "acore","boot_bin", "flashboot_sign.bin")
        self.app_bin = os.path.join(self.output, "acore", "ws63-liteos-app", "ws63-liteos-app-sign.bin")
        self.app_iot_bin = os.path.join(self.output, "acore", "ws63-liteos-app-iot", "ws63-liteos-app-iot-sign.bin")
        self.hilink_bin = os.path.join(self.output, "acore", "ws63-liteos-hilink", "ws63-liteos-hilink-sign.bin")
        self.test_bin = os.path.join(self.output, "acore", "ws63-liteos-testsuite", "ws63-liteos-testsuite-sign.bin")
        self.nv_bin = os.path.join(self.output, "acore", "nv_bin", "ws63_all_nv.bin")

        # 签名加密前原始镜像
        self.src_boot = os.path.join(self.root_path, "output", "ws63", "acore", "boot_bin", "flashboot.bin")
        self.src_app = os.path.join(self.output, "acore", "ws63-liteos-app", "ws63-liteos-app.bin")
        self.src_test = os.path.join(self.output, "acore", "ws63-liteos-testsuite", "ws63-liteos-testsuite.bin")

        # 差分升级时，以下路径配置为上一个版本的镜像路径
        self.flashboot_old_bin = os.path.join(self.output, "acore", "old_version", "flashboot_sign.bin")
        self.app_old_bin = os.path.join(self.output, "acore", "old_version", "ws63-liteos-app-sign.bin")
        self.app_iot_old_bin = os.path.join(self.output, "acore", "old_version", "ws63-liteos-app-iot-sign.bin")
        self.hilink_old_bin = os.path.join(self.output, "acore", "old_version", "ws63-liteos-hilink-sign.bin")
        self.test_old_bin = os.path.join(self.output, "acore", "old_version", "ws63-liteos-testsuite-sign.bin")
        self.nv_old_bin = os.path.join(self.output, "acore", "old_version", "ws63_all_nv.bin")

def get_new_image(input_param, info):
    image_list = []
    if 'app' in input_param:
        image_list.append("=".join([info.app_bin, "application"]))
    if 'app_iot' in input_param:
        image_list.append("=".join([info.app_iot_bin, "application"]))
        target_env = TargetEnvironment("ws63-liteos-app-iot")
        defines = target_env.get('defines')
        if "CONFIG_SUPPORT_HILINK_INDIE_UPGRADE" in defines:
            image_list.append("=".join([info.hilink_bin, "hilink"]))
    if 'hilink' in input_param:
        image_list.append("=".join([info.hilink_bin, "hilink"]))
    if 'test' in input_param:
        image_list.append("=".join([info.test_bin, "application"]))
    if 'boot' in input_param:
        image_list.append("=".join([info.flashboot, "flashboot"]))
    if 'nv' in input_param:
        image_list.append("=".join([info.nv_bin, "nv"]))

    new_image = "|".join(image_list)
    return new_image


def get_old_image(input_param, info):
    image_list = []
    if 'app' in input_param:
        image_list.append("=".join([info.app_old_bin, "application"]))
    if 'app_iot' in input_param:
        image_list.append("=".join([info.app_iot_old_bin, "application"]))
        target_env = TargetEnvironment("ws63-liteos-app-iot")
        defines = target_env.get('defines')
        if "CONFIG_SUPPORT_HILINK_INDIE_UPGRADE" in defines:
            image_list.append("=".join([info.hilink_old_bin, "hilink"]))
    if 'hilink' in input_param:
        image_list.append("=".join([info.hilink_old_bin, "hilink"]))
    if 'test' in input_param:
        image_list.append("=".join([info.test_old_bin, "application"]))
    if 'boot' in input_param:
        image_list.append("=".join([info.flashboot_old_bin, "flashboot"]))
    if 'nv' in input_param:
        image_list.append("=".join([info.nv_old_bin, "nv"]))

    old_image = "|".join(image_list)
    return old_image

def get_parameters():
    parser = argparse.ArgumentParser()

    parser.add_argument('--pkt', type=str, default = 'app',
                        help='需要生成的镜像,包括: app,boot,nv')

    config, unknown = parser.parse_known_args()
    return config

# 打包签名加密前的原镜像
def make_pkt(info, input_param):
    if os.path.exists(info.upg_pkt):
        shutil.rmtree(info.upg_pkt)
    os.makedirs(info.upg_pkt, exist_ok=True)
    shutil.copy(info.fota_cfg, info.upg_pkt)
    if 'app' in input_param:
        shutil.copy(info.src_app, info.upg_pkt)
    if 'test' in input_param:
        shutil.copy(info.src_test, info.upg_pkt)
    if 'boot' in input_param:
        shutil.copy(info.src_boot, info.upg_pkt)
    if 'nv' in input_param:
        shutil.copy(info.nv_bin, info.upg_pkt)
    if os.path.exists(info.upg_pkt_zip):
        os.remove(info.upg_pkt_zip)
    cmd = ' '.join(['tar -cf %s -C %s .' % (info.upg_pkt_zip, info.upg_pkt)])
    ret = subprocess.run(cmd, shell=True)
    if ret.returncode:
        print("make_pkt shell cmd", cmd, ", err: ", ret.returncode)

if __name__ == '__main__':
    info = upg_base_info()
    conf = get_parameters()
    input_param = conf.pkt.split(",")

    conf.app_name        = "update"
    conf.upg_format_path = info.fota_format_path
    conf.base            = info.fota_cfg
    conf.temp_dir        = info.temp_dir
    conf.new_images      = get_new_image(input_param, info)
    conf.old_images      = get_old_image(input_param, info)
    conf.output_dir      = info.upg_output
    conf.type            = 0
    begin(conf)
    make_pkt(info, input_param)
