import subprocess
import os
import sys
import shutil
import platform
import tarfile
import subprocess

file_dir = os.path.dirname(os.path.realpath(__file__))
g_root = os.path.realpath(os.path.join(file_dir, "..", "..", "..", "..", ".."))
sys.path.append(os.path.join(g_root, 'build', 'script'))
sys.path.append(os.path.join(g_root, "build", "config"))
from enviroment import TargetEnvironment
import param_packet

current_path = os.getcwd()
cwd_path = os.path.split(os.path.realpath(__file__))[0]
os.chdir(cwd_path)
if "windows" in platform.platform().lower():
    sign_tool = "../../../../../tools/bin/sign_tool/sign_tool_pltuni.exe"
else:
    sign_tool = "../../../../../tools/bin/sign_tool/sign_tool_pltuni"
out_put = "../../../../../output/ws63/acore"
pktbin = "../../../../../output/ws63/pktbin"
inter_bin = "../../../../../interim_binary/ws63/bin/boot_bin"
efuse_csv = "../script/efuse.csv"
boot_bin = "../../../../../output/ws63/acore/boot_bin"
mfg_bin = "../../../../../application/ws63/ws63_liteos_mfg"

def merge(file_first, file_second, file_out):
    if "windows" in platform.platform().lower():
        out_path = os.path.normpath(os.path.realpath(file_out)).rsplit('\\' , 1)[0]
    else:
        out_path = os.path.normpath(os.path.realpath(file_out)).rsplit('/' , 1)[0]
    print("merge out path: ", out_path)
    if not os.path.exists(out_path):
        os.makedirs(out_path)
    ret = open(file_out, 'wb')
    with open(file_first, 'rb') as file_1:
        for i in file_1:
            ret.write(i)
    with open(file_second, 'rb') as file_2:
        for i in file_2:
            ret.write(i)
    ret.close()


def move_file(src_path, dst_path, file_name):
    src_file = os.path.join(src_path, file_name)
    if not os.path.exists(dst_path):
        os.mkdir(dst_path)
    dst_file = os.path.join(dst_path, file_name)
    shutil.move(src_file, dst_file)


def sign_app(file_path, type, cfg_name):
    if os.path.isfile(file_path):
        print("sign name: ", file_path)
        dd64c(file_path)
        ret = subprocess.run([sign_tool, type, cfg_name])
        if ret.returncode == 0:
            print(file_path, " generated successfully!!!")
        else:
            print(file_path, " generated failed!!!")
        shutil.copy(file_path, pktbin)
    else:
        pass


def generate_fill_bin(file_path, size):
    with open(file_path, 'wb') as f:
        f.write(bytes([0xFF] * size))


def dd64c(input_file_path):
    file_size = os.path.getsize(input_file_path)
    print(input_file_path, "size: ", file_size)
    if file_size % 64 != 0:
        max_size = int((file_size / 64)+1) * 64
        if file_size < max_size:
            file_content = open(input_file_path, 'ab')
            file_content.write(bytes([0] * int(max_size - file_size)))


if os.path.exists(pktbin):
    shutil.rmtree(pktbin)
os.makedirs(pktbin)

# generate params.bin
params_cmd = ["../param_sector/param_sector.json", "params.bin"]
if sys.argv[1] == "ws63-liteos-app-iot" or sys.argv[1] == "ws63-liteos-hilink":
    target_env = TargetEnvironment(sys.argv[1])
    defines = target_env.get('defines')
    if "CONFIG_SUPPORT_HILINK_INDIE_UPGRADE" in defines:
        params_cmd = ["../param_sector/param_sector_hilink_indie_upgrade.json", "params.bin"]
if sys.argv[1] == "ws63-liteos-msmart" or sys.argv[1] == "ws63-liteos-msmart-xts":
    params_cmd = ["../param_sector/param_sector_ms.json", "params.bin"]

print("generate params.bin...")
param_packet.gen_flash_part_bin(params_cmd[0], params_cmd[1])
if os.path.isfile("params.bin"):
    dd64c("params.bin")
    print("params.bin generate successfully!!!")

    # generate params_sign.bin
    param_bin_ecc_cmd = [sign_tool, "0", "param_bin_ecc.cfg"]
    ret = subprocess.run(param_bin_ecc_cmd, cwd=cwd_path)
    if ret.returncode == 0:
        print("params_sign.bin generate successfully!!!")
    else:
        print("params_sign.bin generate failed!!!")

    # generate root public key
    root_pubk_cmd = [sign_tool, "1", "root_pubk.cfg"]
    ret = subprocess.run(root_pubk_cmd, cwd=cwd_path)
    if ret.returncode == 0:
        print("root_pubk.bin generate successfully!!!")
    else:
        generate_fill_bin("root_pubk.bin", 0x80)
        print("generate fill root_pubk.bin")

    # packet root public key and param.bin
    merge("root_pubk.bin", "params_sign.bin", "root_params_sign.bin")
    print("root_params_sign.bin generate successfully!!!")
    move_file(cwd_path, os.path.join(out_put, "param_bin"), "root_params_sign.bin")

if not os.path.isdir(boot_bin):
    shutil.copytree(inter_bin, boot_bin)

if not os.path.isfile(os.path.join(boot_bin, "ws63-liteos-mfg.bin")):
    shutil.copy(os.path.join(mfg_bin, "ws63-liteos-mfg.bin"), boot_bin)

#sign ssb
if os.path.isfile(os.path.join(out_put, "ws63-ssb/ssb.bin")):
    dd64c(os.path.join(out_put, "ws63-ssb/ssb.bin"))
    shutil.copy(os.path.join(out_put, "ws63-ssb/ssb.bin"), pktbin)
    ret1 = subprocess.run([sign_tool, "0", "ssb_bin_ecc.cfg"])
    if ret1.returncode == 0:
        print("ssb_sign.bin generated successfully!!!")
        shutil.copy(os.path.join(out_put, "ws63-ssb/ssb_sign.bin"), boot_bin)
    else:
        print("ssb_sign.bin generated failed!!!")

#sign interim ssb
if os.path.isfile(os.path.join(inter_bin, 'ssb.bin')):
    jug_ssb = False
    if not os.path.isfile(os.path.join(out_put, "ws63-ssb/ssb.bin")):
        os.makedirs(os.path.join(out_put, 'ws63-ssb'))
        shutil.copy(os.path.join(inter_bin, 'ssb.bin'), os.path.join(out_put, "ws63-ssb"))
        sign_app(os.path.join(out_put, "ws63-ssb/ssb.bin"), "0", "ssb_bin_ecc.cfg")
        jug_ssb = True
    shutil.copy(os.path.join(out_put, "ws63-ssb/ssb_sign.bin"), boot_bin)
    print("ssb_sign.bin generated successfully!!!")
    if jug_ssb:
        shutil.rmtree(os.path.join(out_put, 'ws63-ssb'))

#sign flash boot
if os.path.isfile(os.path.join(out_put, "ws63-flashboot/flashboot.bin")):
    dd64c(os.path.join(out_put, "ws63-flashboot/flashboot.bin"))
    shutil.copy(os.path.join(out_put, "ws63-flashboot/flashboot.bin"), pktbin)
    ret1 = subprocess.run([sign_tool, "0", "flash_bin_ecc.cfg"])
    ret2 = subprocess.run([sign_tool, "0", "flash_backup_bin_ecc.cfg"])
    if ret1.returncode == 0 and ret2.returncode == 0:
        print("flash_sign.bin generated successfully!!!")
    else:
        print("flash_sign.bin generated failed!!!")

if os.path.isfile(os.path.join(out_put, "ws63-flashboot/flashboot.bin")):
    shutil.copy(os.path.join(out_put, "ws63-flashboot/flashboot.bin"), boot_bin)
    shutil.copy(os.path.join(out_put, "ws63-flashboot/flashboot_sign.bin"), boot_bin)
    shutil.copy(os.path.join(out_put, "ws63-flashboot/flashboot_backup_sign.bin"), boot_bin)

if os.path.isfile(os.path.join(out_put, "ws63-ate-flash", "ws63-ate-flash.bin")):
    dd64c(os.path.join(out_put, "ws63-ate-flash", "ws63-ate-flash.bin"))
    ret1 = subprocess.run([sign_tool, "0", "flash_htol_bin_ecc.cfg"])
    if ret1.returncode == 0:
        print("ws63_ate_flash.bin generated successfully!!!")
    else:
        print("ws63_ate_flash.bin generated failed!!!")

if os.path.isfile(os.path.join(out_put, "ws63-loaderboot", "loaderboot.bin")):
    dd64c(os.path.join(out_put, "ws63-loaderboot", "loaderboot.bin"))
    shutil.copy(os.path.join(out_put, "ws63-loaderboot", "loaderboot.bin"), pktbin)
    ret1 = subprocess.run([sign_tool, "0", "loaderboot_bin_ecc.cfg"])
    if ret1.returncode == 0:
        print("loaderboot_sign.bin generated successfully!!!")
    else:
        print("loaderboot_sign.bin generated failed!!!")
    merge("root_pubk.bin", "loaderboot_sign.bin", os.path.join(out_put, "ws63-loaderboot", "root_loaderboot_sign.bin"))
    shutil.copy(os.path.join(out_put, "ws63-loaderboot/root_loaderboot_sign.bin"), boot_bin)
    print("root_loaderboot_sign.bin generated successfully!!!")
    os.remove("loaderboot_sign.bin")

#sign loaderboot.bin interim
if os.path.isfile(os.path.join(inter_bin, 'loaderboot.bin')):
    jug_loaderboot = False
    if not os.path.isfile(os.path.join(out_put, "ws63-loaderboot/loaderboot.bin")):
        os.makedirs(os.path.join(out_put, 'ws63-loaderboot'))
        jug_loaderboot = True
        shutil.copy(os.path.join(inter_bin, 'loaderboot.bin'), os.path.join(out_put, "ws63-loaderboot"))
        sign_app(os.path.join(out_put, "ws63-loaderboot/loaderboot.bin"), "0", "loaderboot_bin_ecc.cfg")
        if(os.path.isfile("loaderboot_sign.bin")):
            print("loaderboot_sign.bin generated successfully!!!")
            merge("root_pubk.bin", 'loaderboot_sign.bin', os.path.join(boot_bin, 'root_loaderboot_sign.bin'))
            print("root_loaderboot_sign.bin generated successfully!!!")
        os.remove('loaderboot_sign.bin')
    if jug_loaderboot:
        shutil.rmtree(os.path.join(out_put, "ws63-loaderboot"))


# sign app
sign_app(os.path.join(out_put, "ws63-liteos-testsuite/ws63-liteos-testsuite.bin"), "0", "testsuit_app_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-app/ws63-liteos-app.bin"), "0", "liteos_app_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-hilink/ws63-liteos-hilink.bin"), "0", "liteos_hilink_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-app-gree/ws63-liteos-app-gree.bin"), "0", "liteos_app_gree_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-msmart/ws63-liteos-msmart.bin"), "0", "liteos_msmart_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-msmart-xts/ws63-liteos-msmart-xts.bin"), "0", "liteos_msmart_xts_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-xts/ws63-liteos-xts.bin"), "0", "liteos_xts_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-app-iot/ws63-liteos-app-iot.bin"), "0", "liteos_app_iot_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-btc-only/ws63-liteos-btc-only.bin"), "0", "btc_only_app_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-btc-only-asic/ws63-liteos-btc-only-asic.bin"), "0", "liteos_btc_only_asic_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-gle-sparklyzer/ws63-liteos-gle-sparklyzer.bin"), "0", "sparklyzer_btc_only_app_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-mfg/ws63-liteos-mfg.bin"), "0", "liteos_mfg_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-perf/ws63-liteos-perf.bin"), "0", "liteos_perf_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-app-asic/ws63-liteos-app-asic.bin"), "0", "liteos_app_asic_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-freertos-testsuite/ws63-freertos-testsuite.bin"), "0", "freertos_testsuit_app_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-freertos-app/ws63-freertos-app.bin"), "0", "freertos_app_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-freertos-wifi-only/ws63-freertos-wifi-only.bin"), "0", "freertos_wifi_only_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-freertos-btc-only/ws63-freertos-btc-only.bin"), "0", "freertos_btc_only_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-alios-app/ws63-alios-app.bin"), "0", "alios_app_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-alios-testsuite/ws63-alios-testsuite.bin"), "0", "alios_testsuit_app_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-alios-btc-only/ws63-alios-btc-only.bin"), "0", "alios_btc_only_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-bgle-all/ws63-liteos-bgle-all.bin"), "0", "liteos_bgle_all_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-bgle-all-asic/ws63-liteos-bgle-all-asic.bin"), "0", "liteos_bgle_all_asic_bin_ecc.cfg")

sign_app(os.path.join(out_put, "ws63-liteos-testsuite-radar/ws63-liteos-testsuite-radar.bin"), "0", "liteos_testsuite_radar_bin_ecc.cfg")

sign_app(os.path.join(boot_bin, "ws63-liteos-mfg.bin"), "0", "liteos_mfg_bin_factory_ecc.cfg")

move_file(cwd_path, os.path.join(out_put, "param_bin"), "params.bin")
# clean middle files
os.remove("params_sign.bin")
os.remove("root_pubk.bin")

if os.path.isfile(os.path.join(out_put, "ws63-liteos-app", "ws63-liteos-app.bin")):
    shutil.copy(os.path.join(out_put, "ws63-liteos-app", "ws63-liteos-app.bin"), pktbin)

if os.path.isfile(os.path.join(out_put, "ws63-liteos-hilink", "ws63-liteos-hilink.bin")):
    shutil.copy(os.path.join(out_put, "ws63-liteos-hilink", "ws63-liteos-hilink.bin"), pktbin)

if os.path.isfile(os.path.join(inter_bin, "ssb.bin")):
    shutil.copy(os.path.join(inter_bin, "ssb.bin"), pktbin)

if os.path.isfile(os.path.join(inter_bin, "flashboot.bin")):
    shutil.copy(os.path.join(inter_bin, "flashboot.bin"), pktbin)

if os.path.isfile(os.path.join(inter_bin, "loaderboot.bin")):
    shutil.copy(os.path.join(inter_bin, "loaderboot.bin"), pktbin)

if os.path.isfile(efuse_csv):
    shutil.copy(efuse_csv, pktbin)

if os.path.isfile(os.path.join(out_put, "param_bin", "params.bin")):
    shutil.copy(os.path.join(out_put, "param_bin", "params.bin"), pktbin)

if os.path.isfile(os.path.join(out_put, "nv_bin", "ws63_all_nv.bin")):
    shutil.copy(os.path.join(out_put, "nv_bin", "ws63_all_nv.bin"), pktbin)

if os.path.isfile(os.path.join(out_put, "nv_bin", "ws63_all_nv_backup.bin")):
    shutil.copy(os.path.join(out_put, "nv_bin", "ws63_all_nv_backup.bin"), pktbin)

os.chdir(current_path)
