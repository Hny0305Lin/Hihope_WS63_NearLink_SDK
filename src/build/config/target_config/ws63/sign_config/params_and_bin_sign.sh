#!/bin/bash

set -e

script_dir=$(cd $(dirname $0); pwd)
pushd $script_dir

sign_tool=../../../../../tools/bin/sign_tool/sign_tool_pltuni
pack_tool=../../../../script/param_packet.py
out_put=../../../../../output/ws63/acore
pktbin=../../../../../output/ws63/pktbin
inter_bin=../../../../../interim_binary/ws63/bin/boot_bin
efuse_csv=../script/efuse.csv

padd_align_64byte()
{
    input_file=$1
    tmp_file=$input_file.tmp
    size=`wc -c < $input_file`
    block_cnt=`expr $size / 64 + 1`
    dd if=$input_file of=$tmp_file bs=64c seek=0 count=$block_cnt conv=sync
    mv $tmp_file $input_file
}

rm -rf $pktbin
mkdir $pktbin

#Generate param.bin
python3 $pack_tool ../param_sector/param_sector.json params.bin
if [ -f params.bin ]; then
    padd_align_64byte params.bin
    echo "params.bin generate succed!!!"
    #sign params.bin
    $sign_tool 0 param_bin_ecc.cfg
    echo "params_sign.bin generate succed!!!"
    #Generate root public key
    $sign_tool 1 root_pubk.cfg
    echo "root_pubk.bin generate succed!!!"
    #packet root public key and param.bin
    cat root_pubk.bin params_sign.bin > root_params_sign.bin
    echo "root_params_sign.bin generate succed!!!"
fi

#sign ssb
if [ -f $out_put"/ws63-ssb/ssb.bin" ]; then
    padd_align_64byte $out_put"/ws63-ssb/ssb.bin"
    cp $out_put"/ws63-ssb/ssb.bin" $pktbin
    $sign_tool 0 ssb_bin_ecc.cfg
    echo "ssb_sign.bin generate succed!!!"
fi

#sign flash boot
if [ -f $out_put"/ws63-flashboot/flashboot.bin" ]; then
    padd_align_64byte $out_put"/ws63-flashboot/flashboot.bin"
    cp $out_put"/ws63-flashboot/flashboot.bin" $pktbin
    $sign_tool 0 flash_bin_ecc.cfg
    $sign_tool 0 flash_backup_bin_ecc.cfg
    echo "flash_sign.bin generate succed!!!"
fi

if [ -f $out_put"/ws63-ate-flash/ws63-ate-flash.bin" ]; then
    padd_align_64byte $out_put"/ws63-ate-flash/ws63-ate-flash.bin"
    $sign_tool 0 flash_htol_bin_ecc.cfg
    echo "ws63-ate-flash.bin generate succed!!!"
fi

#sign loaderboot
if [ -f $out_put"/ws63-loaderboot/loaderboot.bin" ]; then
    padd_align_64byte $out_put"/ws63-loaderboot/loaderboot.bin"
    cp $out_put"/ws63-loaderboot/loaderboot.bin" $pktbin
    $sign_tool 0 loaderboot_bin_ecc.cfg
    echo "loaderboot_sign.bin generate succed!!!"
    #pack root public bin and loaderboot
    cat root_pubk.bin loaderboot_sign.bin > $out_put"/ws63-loaderboot/root_loaderboot_sign.bin"
    echo "root_loaderboot_sign.bin generate succed!!!"
    rm -rf loaderboot_sign.bin
fi

#sign app
if [ -f $out_put"/ws63-liteos-testsuite/ws63-liteos-testsuite.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-testsuite/ws63-liteos-testsuite.bin"
    $sign_tool 0 testsuit_app_bin_ecc.cfg
    echo "testsuit_app_bin_ecc generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-app/ws63-liteos-app.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-app/ws63-liteos-app.bin"
    cp $out_put"/ws63-liteos-app/ws63-liteos-app.bin" $pktbin
    $sign_tool 0 liteos_app_bin_ecc.cfg
    echo "ws63-liteos-app generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-hilink/ws63-liteos-hilink.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-hilink/ws63-liteos-hilink.bin"
    cp $out_put"/ws63-liteos-hilink/ws63-liteos-hilink.bin" $pktbin
    $sign_tool 0 liteos_hilink_bin_ecc.cfg
    echo "ws63-liteos-hilink generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-app-gree/ws63-liteos-app-gree.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-app-gree/ws63-liteos-app-gree.bin"
    cp $out_put"/ws63-liteos-app-gree/ws63-liteos-app-gree.bin" $pktbin
    $sign_tool 0 liteos_app_gree_bin_ecc.cfg
    echo "ws63-liteos-app-gree generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-msmart/ws63-liteos-msmart.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-msmart/ws63-liteos-msmart.bin"
    cp $out_put"/ws63-liteos-msmart/ws63-liteos-msmart.bin" $pktbin
    $sign_tool 0 liteos_msmart_bin_ecc.cfg
    echo "ws63-liteos-msmart generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-msmart-xts/ws63-liteos-msmart-xts.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-msmart-xts/ws63-liteos-msmart-xts.bin"
    cp $out_put"/ws63-liteos-msmart-xts/ws63-liteos-msmart-xts.bin" $pktbin
    $sign_tool 0 liteos_msmart_xts_bin_ecc.cfg
    echo "ws63-liteos-msmart-xts generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-xts/ws63-liteos-xts.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-xts/ws63-liteos-xts.bin"
    cp $out_put"/ws63-liteos-xts/ws63-liteos-xts.bin" $pktbin
    $sign_tool 0 liteos_xts_bin_ecc.cfg
    echo "ws63-liteos-xts generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-app-iot/ws63-liteos-app-iot.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-app-iot/ws63-liteos-app-iot.bin"
    cp $out_put"/ws63-liteos-app-iot/ws63-liteos-app-iot.bin" $pktbin
    $sign_tool 0 liteos_app_iot_bin_ecc.cfg
    echo "ws63-liteos-app-iot generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-mfg/ws63-liteos-mfg.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-mfg/ws63-liteos-mfg.bin"
    cp $out_put"/ws63-liteos-mfg/ws63-liteos-mfg.bin" $pktbin
    $sign_tool 0 liteos_mfg_bin_ecc.cfg
    echo "ws63-liteos-mfg generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-perf/ws63-liteos-perf.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-perf/ws63-liteos-perf.bin"
    $sign_tool 0 liteos_perf_bin_ecc.cfg
    echo "ws63-liteos-perf generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-testsuite-radar/ws63-liteos-testsuite-radar.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-testsuite-radar/ws63-liteos-testsuite-radar.bin"
    $sign_tool 0 liteos_testsuite_radar_bin_ecc.cfg
    echo "liteos_testsuite_radar_bin_ecc generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-btc-only/ws63-liteos-btc-only.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-btc-only/ws63-liteos-btc-only.bin"
    $sign_tool 0 btc_only_app_bin_ecc.cfg
    echo "btc_only_app_bin_ecc generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-btc-only-asic/ws63-liteos-btc-only-asic.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-btc-only-asic/ws63-liteos-btc-only-asic.bin"
    $sign_tool 0 liteos_btc_only_asic_bin_ecc.cfg
    echo "liteos_btc_only_asic_bin_ecc generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-gle-sparklyzer/ws63-liteos-gle-sparklyzer.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-gle-sparklyzer/ws63-liteos-gle-sparklyzer.bin"
    $sign_tool 0 sparklyzer_btc_only_app_bin_ecc.cfg
    echo "sparklyzer_btc_only_app_bin_ecc generate succed!!!"
fi

if [ -f $out_put"/ws63-freertos-testsuite/ws63-freertos-testsuite.bin" ]; then
    padd_align_64byte $out_put"/ws63-freertos-testsuite/ws63-freertos-testsuite.bin"
    $sign_tool 0 freertos_testsuit_app_bin_ecc.cfg
    echo "freertos_testsuit_app_bin_ecc generate succed!!!"
fi

if [ -f $out_put"/ws63-freertos-app/ws63-freertos-app.bin" ]; then
    padd_align_64byte $out_put"/ws63-freertos-app/ws63-freertos-app.bin"
    $sign_tool 0 freertos_app_bin_ecc.cfg
    echo "freertos_app_bin_ecc generate succed!!!"
fi

if [ -f $out_put"/ws63-freertos-wifi-only/ws63-freertos-wifi-only.bin" ]; then
    padd_align_64byte $out_put"/ws63-freertos-wifi-only/ws63-freertos-wifi-only.bin"
    $sign_tool 0 freertos_wifi_only_bin_ecc.cfg
    echo "freertos_wifi_only_bin_ecc copy generate succed!!!"
fi

if [ -f $out_put"/ws63-freertos-btc-only/ws63-freertos-btc-only.bin" ]; then
    padd_align_64byte $out_put"/ws63-freertos-btc-only/ws63-freertos-btc-only.bin"
    $sign_tool 0 freertos_btc_only_bin_ecc.cfg
    echo "freertos_btc_only_bin_ecc copy generate succed!!!"
fi

if [ -f $out_put"/ws63-alios-app/ws63-alios-app.bin" ]; then
    padd_align_64byte $out_put"/ws63-alios-app/ws63-alios-app.bin"
    $sign_tool 0 alios_app_bin_ecc.cfg
    echo "alios_app_bin_ecc copy generate succed!!!"
fi

if [ -f $out_put"/ws63-alios-testsuite/ws63-alios-testsuite.bin" ]; then
    padd_align_64byte $out_put"/ws63-alios-testsuite/ws63-alios-testsuite.bin"
    $sign_tool 0 alios_testsuit_app_bin_ecc.cfg
    echo "alios_testsuit_app_bin_ecc copy generate succed!!!"
fi

if [ -f $out_put"/ws63-alios-btc-only/ws63-alios-btc-only.bin" ]; then
    padd_align_64byte $out_put"/ws63-alios-btc-only/ws63-alios-btc-only.bin"
    $sign_tool 0 alios_btc_only_bin_ecc.cfg
    echo "alios_testsuit_app_bin_ecc copy generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-bgle-all/ws63-liteos-bgle-all.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-bgle-all/ws63-liteos-bgle-all.bin"
    $sign_tool 0 liteos_bgle_all_bin_ecc.cfg
    echo "liteos_bgle_all_bin_ecc copy generate succed!!!"
fi

if [ -f $out_put"/ws63-liteos-bgle-all-asic/ws63-liteos-bgle-all-asic.bin" ]; then
    padd_align_64byte $out_put"/ws63-liteos-bgle-all-asic/ws63-liteos-bgle-all-asic.bin"
    $sign_tool 0 liteos_bgle_all_asic_bin_ecc.cfg
    echo "liteos_bgle_all_bin_ecc copy generate succed!!!"
fi

#deal tmp param file
rm -rf params_sign.bin
rm -rf root_pubk.bin
rm -rf $out_put"/param_bin/"
mkdir $out_put"/param_bin/"
mv root_params_sign.bin $out_put"/param_bin/"
mv params.bin $out_put"/param_bin/"

#pack all unsgin bin
cp $inter_bin"/ssb.bin" $pktbin || :
cp $inter_bin"/flashboot.bin" $pktbin || :
cp $inter_bin"/loaderboot.bin" $pktbin || :
cp $efuse_csv $pktbin || :
cp $out_put"/param_bin/params.bin" $pktbin || :
cp $out_put"/nv_bin/ws63_all_nv.bin" $pktbin || :
pushd $pktbin"/../"
rm -rf pktbin.zip
tar -cf pktbin.zip ./pktbin
popd

popd
exit 0
