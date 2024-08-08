#!/bin/sh
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
set -e 

root_dir=$1
bin_name=$2
out_dir=${root_dir}/output/ws63/acore/${bin_name}
romboot_bin_dir=${root_dir}/output/ws63/acore/ws63-romboot
romboot_bin=${root_dir}/output/ws63/acore/ws63-romboot/romboot.bin
create_hex_py=${root_dir}/build/script/utils/create_hex.py


if [ ! -f ${romboot_bin} ];then
    echo "[*] romboot not found, codepoint files will not be one!"
    exit 0
fi

pushd ${out_dir}
# Fill romboot bin to 36K
dd if=${romboot_bin} of=romboot.36K.bin bs=36K seek=0 count=1 conv=sync

# All in one rompack file
cat romboot.36K.bin ${bin_name}_rom.bin > ${bin_name}_rompack.bin
python3 ${create_hex_py} ${bin_name}_rompack.bin ${bin_name}_rompack.hex

# Split the rompack bin file into three parts: 128k, 128k, 48k
dd bs=128k count=1 if=${bin_name}_rompack.bin of=${bin_name}_rompack_part1.128k.bin
python3 ${create_hex_py} ${bin_name}_rompack_part1.128k.bin ${bin_name}_rompack_part1.128k.hex

dd bs=128k count=1 skip=1 if=${bin_name}_rompack.bin of=${bin_name}_rompack_part2.128k.bin
python3 ${create_hex_py} ${bin_name}_rompack_part2.128k.bin ${bin_name}_rompack_part2.128k.hex

dd bs=128k skip=2 if=${bin_name}_rompack.bin of=${bin_name}_rompack_part3.tmp.bin
dd bs=48k count=1 conv=sync if=${bin_name}_rompack_part3.tmp.bin of=${bin_name}_rompack_part3.48k.bin
rm -f ${bin_name}_rompack_part3.tmp.bin
python3 ${create_hex_py} ${bin_name}_rompack_part3.48k.bin ${bin_name}_rompack_part3.48k.hex

popd
