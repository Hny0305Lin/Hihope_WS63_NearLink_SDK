set(ROM_OUT_OBJ multi_binaries)
set(ROM_OUT_COMP_OBJ multi_binaries2)
set(BUILD_LINKER_PROCESS
    COMMAND ${SCRIPTS_PATH}/multi_binaries/multi_binaries.sh ${OUT} ${CROSS_COMPILE} ${LITEOS_COMPILER_GCCLIB_PATH} ${LITEOSTOPDIR}
    COMMAND ${LITEOS_LD_COMMAND} -Map=${OUT}/${ROM_OUT_OBJ}.map -o ${OUT}/${ROM_OUT_OBJ} --start-group ${LITEOS_BASELIB_LD} --end-group
    COMMAND ${CMAKE_OBJCOPY} -O binary ${OUT}/${ROM_OUT_OBJ} ${OUT}/${ROM_OUT_OBJ}.bin
    COMMAND ${CMAKE_OBJCOPY} --only-keep-debug ${OUT}/${ROM_OUT_OBJ} ${OUT}/${ROM_OUT_OBJ}.dbg
    COMMAND ${CMAKE_OBJDUMP} -t ${OUT}/${ROM_OUT_OBJ} |sort > ${OUT}/${ROM_OUT_OBJ}.sym.sorted
    COMMAND ${CMAKE_OBJDUMP} -dh ${OUT}/${ROM_OUT_OBJ} > ${OUT}/${ROM_OUT_OBJ}.asm
    COMMAND dd if=${OUT}/${ROM_OUT_OBJ}.bin of=${OUT}/${ROM_OUT_OBJ}_non_rom_boot.bin seek=0 count=1 bs=1k
    COMMAND dd if=${OUT}/${ROM_OUT_OBJ}.bin of=${OUT}/${ROM_OUT_OBJ}_rom.bin skip=1 count=50 bs=1k
    COMMAND dd if=${OUT}/${ROM_OUT_OBJ}.bin of=${OUT}/${ROM_OUT_OBJ}_non_rom.bin skip=51 bs=1k

    COMMAND mkdir -p ${OUT}/bin_test
    COMMAND ${CC} -DROM_BIN_TEST -E ${BOARD_LD_S_FILE} ${BOARD_LD_INCULDE_PATH} -o ${BOARD_LD_FILE} -P
    COMMAND ${LITEOS_LD_COMMAND} -Map=${OUT}/bin_test/${ROM_OUT_COMP_OBJ}.map -o ${OUT}/bin_test/${ROM_OUT_COMP_OBJ} --start-group ${LITEOS_BASELIB_LD} --end-group
    COMMAND ${CMAKE_OBJCOPY} -O binary ${OUT}/bin_test/${ROM_OUT_COMP_OBJ} ${OUT}/bin_test/${ROM_OUT_COMP_OBJ}.bin
    COMMAND ${CMAKE_OBJCOPY} --only-keep-debug ${OUT}/bin_test/${ROM_OUT_COMP_OBJ} ${OUT}/bin_test/${ROM_OUT_COMP_OBJ}.dbg
    COMMAND ${CMAKE_OBJDUMP} -t ${OUT}/bin_test/${ROM_OUT_COMP_OBJ} |sort > ${OUT}/bin_test/${ROM_OUT_COMP_OBJ}.sym.sorted
    COMMAND ${CMAKE_OBJDUMP} -dh ${OUT}/bin_test/${ROM_OUT_COMP_OBJ} > ${OUT}/bin_test/${ROM_OUT_COMP_OBJ}.asm
    COMMAND dd if=${OUT}/bin_test/${ROM_OUT_COMP_OBJ}.bin of=${OUT}/bin_test/${ROM_OUT_COMP_OBJ}_non_rom_boot.bin seek=0 count=1 bs=1k
    COMMAND dd if=${OUT}/bin_test/${ROM_OUT_COMP_OBJ}.bin of=${OUT}/bin_test/${ROM_OUT_COMP_OBJ}_rom.bin skip=1 count=50 bs=1k
    COMMAND dd if=${OUT}/bin_test/${ROM_OUT_COMP_OBJ}.bin of=${OUT}/bin_test/${ROM_OUT_COMP_OBJ}_non_rom.bin skip=51 bs=1k

    COMMAND ${SCRIPTS_PATH}/multi_binaries/cmp_file.sh ${OUT}/bin_test/${ROM_OUT_COMP_OBJ}_rom.bin ${OUT}/${ROM_OUT_OBJ}_rom.bin
    COMMAND ${PYTHON3} ${SCRIPTS_PATH}/analysis_liba_size.py ${SIZE} ${OUT}/lib/multi_binaries/ ${OUT}/lib/multi_binaries/
)
