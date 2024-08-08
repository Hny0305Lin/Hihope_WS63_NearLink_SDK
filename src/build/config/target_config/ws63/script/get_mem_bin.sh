#!/bin/sh
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
set -e 
 
ROOT_PATH=$(realpath $1)
TARGET_NAME=$2
BIN_NAME=$3
 
ELF_PATH=${ROOT_PATH}/output/ws63/acore/${TARGET_NAME}/${BIN_NAME}.elf
TOOLCHAIN=${ROOT_PATH}/tools/bin/compiler/riscv/cc_riscv32_musl_100/cc_riscv32_musl_fp/bin/riscv32-linux-musl-
WORK_DIR=${ROOT_PATH}/output/ws63/mem_bins
MEMFILE=${ROOT_PATH}/output/ws63/acore/${TARGET_NAME}/${BIN_NAME}.mem
SRAM_PKT_RAM=${ROOT_PATH}/output/ws63/mem_bins/sram_pkt_ram.bin
SRAM_ALL=${ROOT_PATH}/output/ws63/mem_bins/SRAM_START_ADDR_0xA00000.bin
SRAM=${ROOT_PATH}/output/ws63/mem_bins/sram.bin
 
OBJCOPY=${TOOLCHAIN}objcopy
READELF=${TOOLCHAIN}readelf
 
get_section_meminfo(){
    local elf_path=$1
    local section_name=$2
 
    ${READELF} -S ${elf_path}|grep " ${section_name}  "|awk '{if($1=="["){print $5,$6,$7,$4}else{print $4,$5,$6,$3}}' 
}
 
get_mem_sections(){
    local memfile=$1
    local mem_name=$2
    
    cat ${memfile} | grep -Pzo "(?s)\n\s*${mem_name}.+?\n\s*\n"|awk '{print $1}'|grep -aP "^\."
}
 
gen_section_to_bin(){
    local elf_path=$1
    local section_name_list=($(echo $2))
    local first_section_name=$3
    local output_bin_path=$4
    local mem_info
    local params=()
    local lma
    local first_mem_info=($(get_section_meminfo $elf_path $first_section_name))
 
    for section_name in ${section_name_list[@]};do
        if [ "$section_name" = "$first_section_name" ];then
            params+="-j ${first_section_name} --change-section-lma=${first_section_name}=0 "
            if [ "${first_mem_info[3]}" = "NOBITS" ];then
                params+="--set-section-flags=${first_section_name}=contents "
            fi
            continue
        fi
 
        mem_info=($(get_section_meminfo $elf_path $section_name))
        if [ $((0x${mem_info[2]} - 0)) -eq 0 ];then echo "skip $section_name";continue;fi
 
        lma=$((0x${mem_info[0]} - 0x${first_mem_info[0]}))
        params+="-j $section_name --change-section-lma=${section_name}=${lma} "
        if [ "${mem_info[3]}" = "NOBITS" ];then
            params+="--set-section-flags=${section_name}=contents "
        fi
    done
 
    ${OBJCOPY} --gap-fill 0x00 -O binary ${params[@]} ${elf_path} ${output_bin_path}
}
 
dtcm_sections=($(get_mem_sections $MEMFILE DTCM))
itcm_sections=($(get_mem_sections $MEMFILE ITCM))
sram_sections=($(get_mem_sections $MEMFILE SRAM))
 
mkdir -p $WORK_DIR
pushd ${WORK_DIR}
    gen_section_to_bin ${ELF_PATH} "${dtcm_sections[*]}" ${dtcm_sections[0]} DTCM_START_ADDR_0x180000.bin
    gen_section_to_bin ${ELF_PATH} "${itcm_sections[*]}" ${itcm_sections[0]} ITCM_START_ADDR_0x14c000.bin
    gen_section_to_bin ${ELF_PATH} "${sram_sections[*]}" ${sram_sections[0]} sram.bin
    dd if=/dev/zero of=${ROOT_PATH}/output/ws63/mem_bins/sram_pkt_ram.bin bs=49152 seek=0 count=1 conv=sync
    cat ${ROOT_PATH}/output/ws63/mem_bins/sram_pkt_ram.bin ${ROOT_PATH}/output/ws63/mem_bins/sram.bin > ${ROOT_PATH}/output/ws63/mem_bins/SRAM_START_ADDR_0xA00000.bin
    dd if=/dev/zero of=${ROOT_PATH}/output/ws63/mem_bins/flash_gap.bin bs=266240 seek=0 count=1 conv=sync
    cat ${ROOT_PATH}/output/ws63/mem_bins/flash_gap.bin ${ROOT_PATH}/output/ws63/acore/$3/$3-sign.bin > ${ROOT_PATH}/output/ws63/mem_bins/FLASH.bin
    python3 ${ROOT_PATH}/build/config/target_config/ws63/script/process_sfc_hex.py ${ROOT_PATH}/output/ws63/mem_bins/FLASH.bin ${ROOT_PATH}/output/ws63/mem_bins/SFC_DATA 2097152
    rm -rf FLASH.bin flash_gap.bin sram.bin sram_pkt_ram.bin temp.hex
popd