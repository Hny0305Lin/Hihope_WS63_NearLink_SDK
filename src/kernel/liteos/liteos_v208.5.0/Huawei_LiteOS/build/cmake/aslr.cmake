if(LOSCFG_LASLR)
set(LITEOS_ASLR_SCRIPTPATH ${LITEOS_SCRIPTPATH}/aslr)
set(ASLR_REL_FILE ${OUT}/aslr/rel/aslr_gspr_info.c)
set(ASLR_REL_OBJECT ${OUT}/aslr/rel/aslr_gspr_info.o)

set(ASLR_POST_CMD)
if (LOSCFG_LASLR_GSPR)
set(ASLR_TYPE "GSPR")
if (EXISTS ${ASLR_REL_FILE})
    list(APPEND LITEOS_LD_COMMAND ${ASLR_REL_OBJECT})
    list(APPEND ASLR_POST_CMD
        COMMAND ${PYTHON3} ${LITEOS_ASLR_SCRIPTPATH}/generate_gspr_rel_info.py ${ASLR_TYPE}_check ${CROSS_COMPILE} ${OUT}/${LITEOS_TARGET}
        COMMAND bash -c "sh ${LITEOSTOPDIR}/build/scripts/diff_files.sh ${ASLR_REL_FILE}.bak ${ASLR_REL_FILE}.check"
    )
else()
    list(APPEND ASLR_POST_CMD
        # generate image rel info
        COMMAND ${PYTHON3} ${LITEOS_ASLR_SCRIPTPATH}/generate_gspr_rel_info.py ${ASLR_TYPE} ${CROSS_COMPILE} ${OUT}/${LITEOS_TARGET}
    )
endif()
endif()

if (LOSCFG_LASLR_GOT)
set(ASLR_TYPE "GOT")
list(APPEND ASLR_POST_CMD
    # check image relocate type
    COMMAND ${PYTHON3} ${LITEOS_ASLR_SCRIPTPATH}/check_aslr_rel_type.py ${ASLR_TYPE} ${CROSS_COMPILE} ${OUT}/${LITEOS_TARGET}
)
endif()

list(APPEND ASLR_POST_CMD
    # check aslr image independence
    COMMAND ${PYTHON3} ${LITEOS_ASLR_SCRIPTPATH}/check_aslr_image.py ${OUT}/${LITEOS_TARGET} ${OUT}/${LITEOS_TARGET}.map ${CROSS_COMPILE} ${LITEOS_LDFLAGS}
)

list(APPEND BUILD_LINKER_POST_PROCESS 
    ${ASLR_POST_CMD}
)
endif()

