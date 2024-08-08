

import argparse
import os
import shutil


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="copy_files_to_interim")
    parser.add_argument('proj_root_dir', help="proj root dir")

    args = parser.parse_args()

    proj_root_dir = args.proj_root_dir

    target_dir = os.path.join(proj_root_dir, "interim_binary", "ws63", "bin", "boot_bin")
    if not os.path.isdir(target_dir):
        os.makedirs(target_dir)

    hilink_target_dir = os.path.join(proj_root_dir, "interim_binary", "ws63", "bin", "hilink_bin")
    if not os.path.isdir(hilink_target_dir):
        os.makedirs(hilink_target_dir)

    # key is src file, value is dest file or dir
    copy_map = {
        'output/ws63/acore/ws63-loaderboot/loaderboot.bin' : target_dir,
        'output/ws63/acore/ws63-loaderboot/root_loaderboot_sign.bin' : target_dir,
        'output/ws63/acore/ws63-liteos-app/efuse_cfg.bin' : target_dir,
        'output/ws63/acore/ws63-ssb/ssb.bin' : target_dir,
        'output/ws63/acore/ws63-ssb/ssb_sign.bin' : target_dir,
        'output/ws63/acore/ws63-flashboot/flashboot.bin' : target_dir,
        'output/ws63/acore/ws63-flashboot/flashboot_sign.bin' : target_dir,
        'output/ws63/acore/ws63-flashboot/flashboot_backup_sign.bin' : target_dir,
        'output/ws63/acore/ws63-liteos-hilink/ws63-liteos-hilink.bin' : hilink_target_dir,
        'output/ws63/acore/ws63-liteos-hilink/ws63-liteos-hilink-sign.bin' : hilink_target_dir,
    }

    for src_file, dest in copy_map.items():
        src_file = os.path.join(proj_root_dir, src_file)
        if not os.path.isfile(src_file):
            print(f'[!][copy_files_to_interim] File `{src_file}` not found, will skip it !')
            continue
        filename = src_file.split('/')[-1]
        if not os.path.isfile(os.path.join(target_dir, filename)):
            continue
        try:
            shutil.copy(src_file, dest)
        except BaseException as e:
            print(e)
            exit(-1)
        