if(LOSCFG_KERNEL_RUNSTOP)
# touch empty_file.c file
if (NOT EXISTS ${OUT}/empty_file.c)
    file(WRITE ${OUT}/empty_file.c "/* do nothing */\n")
endif()

set(LITEOS_WOW_TARGET vs_wow_server)
set(LITEOS_WOW_BASELIB_LD ${LITEOS_BASELIB})
list(TRANSFORM LITEOS_WOW_BASELIB_LD PREPEND -l)
set(LITEOS_WOW_LD_COMMAND ${CMAKE_LINKER} ${LITEOS_LDFLAGS} ${LITEOS_TABLES_WOW_LDFLAGS} -Map=${OUT}/${LITEOS_WOW_TARGET}.map -o ${OUT}/${LITEOS_WOW_TARGET} --start-group ${LITEOS_WOW_BASELIB_LD} --end-group)

add_custom_target(${LITEOS_WOW_TARGET}
    DEPENDS ${LITEOS_DEP_LIBS_INT}
    COMMAND bash -c "sh ${SCRIPTS_PATH}/scatter_sr/clear_ld.sh ${SCRIPTS_PATH}/ld"
    COMMAND ${LITEOS_WOW_LD_COMMAND}
    COMMAND bash -c "sh ${SCRIPTS_PATH}/scatter_sr/liblist.sh wow ${OUT}/${LITEOS_WOW_TARGET} ${OUT}/${LITEOS_WOW_TARGET}.map ${SCRIPTS_PATH}/scatter_sr ${LITEOS_COMPILER_GCCLIB_PATH} ${LITEOS_COMPILER_CXXLIB_PATH} ${OUT}/lib"
    BYPRODUCTS ${OUT}/${LITEOS_WOW_TARGET} ${OUT}/${LITEOS_WOW_TARGET}.map
)
endif()

if(LOSCFG_KERNEL_SCATTER)
# touch empty_file.c file
if (NOT EXISTS ${OUT}/empty_file.c)
    file(WRITE ${OUT}/empty_file.c "/* do nothing */\n")
endif()

set(LITEOS_SCATTER_TARGET vs_scatter_server)
set(LITEOS_SCATTER_BASELIB_LD ${LITEOS_BASELIB})
list(TRANSFORM LITEOS_SCATTER_BASELIB_LD PREPEND -l)
set(LITEOS_SCATTER_LD_COMMAND ${CMAKE_LINKER} ${LITEOS_LDFLAGS} ${LITEOS_TABLES_WOW_LDFLAGS} ${LITEOS_TABLES_SCATTER_LDFLAGS} -Map=${OUT}/${LITEOS_SCATTER_TARGET}.map -o ${OUT}/${LITEOS_SCATTER_TARGET} --start-group ${LITEOS_SCATTER_BASELIB_LD} --end-group)

add_custom_target(${LITEOS_SCATTER_TARGET}
    DEPENDS ${LITEOS_DEP_LIBS_INT}
    COMMAND bash -c "sh ${SCRIPTS_PATH}/scatter_sr/clear_ld.sh ${SCRIPTS_PATH}/ld"
    COMMAND ${LITEOS_SCATTER_LD_COMMAND}
    COMMAND bash -c "sh ${SCRIPTS_PATH}/scatter_sr/liblist.sh scatter ${OUT}/${LITEOS_SCATTER_TARGET} ${OUT}/${LITEOS_SCATTER_TARGET}.map ${SCRIPTS_PATH}/scatter_sr ${LITEOS_COMPILER_GCCLIB_PATH} ${LITEOS_COMPILER_CXXLIB_PATH} ${OUT}/lib"
    BYPRODUCTS ${OUT}/${LITEOS_SCATTER_TARGET} ${OUT}/${LITEOS_SCATTER_TARGET}.map
)
endif()

set(WOW_SCATTER_COMMAND "${SCRIPTS_PATH}/scatter_sr/clear_ld.sh ${SCRIPTS_PATH}/ld")
set(SCATTER_SIZE_COMMAND "${SCRIPTS_PATH}/scatter_sr/clear_ld.sh ${SCRIPTS_PATH}/ld")

if(LOSCFG_PLATFORM_OSAPPINIT)
    if(LOSCFG_KERNEL_RUNSTOP AND LOSCFG_KERNEL_SCATTER)
        set(WOW_SCATTER_COMMAND "${SCRIPTS_PATH}/scatter_sr/scatter.sh ${CROSS_COMPILE} y y ${SCRIPTS_PATH}/scatter_sr ${SCRIPTS_PATH}/ld ${LITEOSTOPDIR}/liteos_tmp ${LIB_BIGODIR}")
        set(SCATTER_SIZE_COMMAND "${SCRIPTS_PATH}/scatter_sr/scatter_size.sh ${OUT}/${LITEOS_TARGET}")
    elseif(LOSCFG_KERNEL_RUNSTOP)
        set(WOW_SCATTER_COMMAND "${SCRIPTS_PATH}/scatter_sr/scatter.sh ${CROSS_COMPILE} y n ${SCRIPTS_PATH}/scatter_sr ${SCRIPTS_PATH}/ld ${LITEOSTOPDIR}/liteos_tmp ${LIB_BIGODIR}")
    elseif(LOSCFG_KERNEL_SCATTER)
        set(WOW_SCATTER_COMMAND "${SCRIPTS_PATH}/scatter_sr/scatter.sh ${CROSS_COMPILE} n y ${SCRIPTS_PATH}/scatter_sr ${SCRIPTS_PATH}/ld ${LITEOSTOPDIR}/liteos_tmp ${LIB_BIGODIR}")
        set(SCATTER_SIZE_COMMAND "${SCRIPTS_PATH}/scatter_sr/scatter_size.sh ${OUT}/${LITEOS_TARGET}")
    endif()
endif()


if(LOSCFG_KERNEL_RUNSTOP OR LOSCFG_KERNEL_SCATTER)
    list(APPEND BUILD_LINKER_PRE_PROCESS
        COMMAND bash -c "sh ${WOW_SCATTER_COMMAND}"
    )
    list(APPEND BUILD_LINKER_POST_PROCESS
        COMMAND bash -c "sh ${SCATTER_SIZE_COMMAND}"
        COMMAND rm -f ${SCRIPTS_PATH}/scatter_sr/lib_list.*
        COMMAND rm -f ${SCRIPTS_PATH}/scatter_sr/symbol_list.*
        COMMAND bash -c "sh ${SCRIPTS_PATH}/scatter_sr/clear_ld.sh ${SCRIPTS_PATH}/ld"
    )
endif()
