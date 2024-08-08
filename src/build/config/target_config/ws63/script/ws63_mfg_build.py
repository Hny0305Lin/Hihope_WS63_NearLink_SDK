import os
import sys
import shutil
import subprocess

current_path = os.path.dirname(os.path.realpath(__file__))
sdk_root_path = os.path.realpath(os.path.join(current_path, "../../../../../"))
sys.path.append(os.path.join(sdk_root_path, 'tools/pkg'))
from packet_create import packet_bin

def get_file_size(file_path: str)->int:
    try:
        return os.stat(file_path).st_size
    except BaseException as e:
        print(e)
        exit(-1)

cmd_app = [os.path.join(sdk_root_path, "build.py"), "ws63-liteos-app"]
cmd_mfg = [os.path.join(sdk_root_path, "build.py"), "ws63-liteos-mfg"]

ret_app = subprocess.run(cmd_app, cwd=sdk_root_path)
ret_mfg = subprocess.run(cmd_mfg, cwd=sdk_root_path)

if ret_app.returncode != 0 or ret_mfg.returncode != 0:
    print("build error!")
    exit(-1)

boot_bin_dir = os.path.join(sdk_root_path, "interim_binary", "ws63", "bin", "boot_bin")
param_bin_dir = os.path.join(sdk_root_path, "output", "ws63", "acore", "param_bin")
nv_bin_dir = os.path.join(sdk_root_path, "output", "ws63", "acore", "nv_bin")

loadboot_bin = os.path.join(boot_bin_dir, "root_loaderboot_sign.bin")
loadboot_bx = loadboot_bin + "|0x0|0x200000|0"

# params
params_bin = os.path.join(param_bin_dir, "root_params_sign.bin")
params_bx = params_bin + f"|0x200000|{hex(get_file_size(params_bin))}|1"

# flash boot
flashboot_bin = os.path.join(boot_bin_dir, "flashboot_sign.bin")
flashboot_bx = flashboot_bin + f"|0x202000|{hex(get_file_size(flashboot_bin))}|1"

# nv
nv_bin = os.path.join(nv_bin_dir, "ws63_all_nv.bin")
nv_bx = nv_bin + f"|0x224000|{hex(get_file_size(nv_bin))}|1"

app_bin_path = os.path.join(sdk_root_path, "output/ws63/acore/ws63-liteos-app/ws63-liteos-app-sign.bin")
mfg_app_bin_path = os.path.join(sdk_root_path, "output/ws63/acore/ws63-liteos-mfg/ws63-liteos-mfg-sign.bin")

app_templat_bx = app_bin_path + f"|0x22D000|{hex(get_file_size(app_bin_path))}|1"
mfg_app_bx = mfg_app_bin_path + f"|0x42D000|{hex(0x1BA000)}|1"

# 输出目录
fwpkg_outdir = os.path.join(sdk_root_path, "output", "ws63", "fwpkg", "ws63-liteos-mfg-all")

if os.path.exists(fwpkg_outdir):
    shutil.rmtree(fwpkg_outdir)
os.makedirs(fwpkg_outdir)

packet_post_agvs = list()
packet_post_agvs.append(loadboot_bx)
packet_post_agvs.append(params_bx)
packet_post_agvs.append(flashboot_bx)
packet_post_agvs.append(nv_bx)
packet_post_agvs.append(app_templat_bx)
packet_post_agvs.append(mfg_app_bx)

output_fwpkg_path = os.path.join(fwpkg_outdir, "mfg_app_sign.fwpkg")
packet_bin(output_fwpkg_path, packet_post_agvs)