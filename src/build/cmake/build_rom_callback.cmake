if(NOT DEFINED BUILD_ROM_CALLBACK)
    set(BUILD_ROM_CALLBACK False)
endif()
set(OBJ_TEMP_DIR "${PROJECT_BINARY_DIR}/obj_tmp")

macro(reg_rom_callback)
    if(NOT TARGET ${COMPONENT_NAME})
        set(ROM_LIB ${LIBS})
        set(DEPEND_TARGET)
    else()
        get_target_property(ORI_LIB_DIR ${COMPONENT_NAME} BINARY_DIR)
        get_target_property(CUSTOM_LIB_DIR ${COMPONENT_NAME} ARCHIVE_OUTPUT_DIRECTORY)
        if(CUSTOM_LIB_DIR)
            set(ROM_LIB ${CUSTOM_LIB_DIR}/lib${COMPONENT_NAME}.a)
        else()
            set(ROM_LIB ${ORI_LIB_DIR}/lib${COMPONENT_NAME}.a)
        endif()
        set(DEPEND_TARGET ${COMPONENT_NAME})
    endif()

    add_custom_target(GEN_ROM_CB_${COMPONENT_NAME} ALL
        WORKING_DIRECTORY ${OBJ_TEMP_DIR}
        COMMAND echo "${ROM_LIB}"
        COMMAND ${CP} ${ROM_LIB} ${OBJ_TEMP_DIR}
        COMMAND ${CMAKE_AR} -x lib${COMPONENT_NAME}.a
        DEPENDS ${DEPEND_TARGET}
    )
endmacro()

function(build_rom_callback)
    execute_process(
        COMMAND ${MKDIR} ${OBJ_TEMP_DIR}
    )
    add_custom_target(BUILD_ROM_CALLBACK ALL
        COMMAND ${CMAKE_LINKER} -r ${OBJ_TEMP_DIR}/*.o* -o ${OBJ_TEMP_DIR}/rom_bin.o
        COMMAND ${CMAKE_NM} -u ${OBJ_TEMP_DIR}/rom_bin.o > rom_bin_raw.undef
        COMMAND ${CMAKE_READELF} -W -r ${OBJ_TEMP_DIR}/rom_bin.o > rom_bin.rel
        COMMAND ${CMAKE_READELF} -W -s ${OBJ_TEMP_DIR}/rom_bin.o > rom_symbol.list
        COMMAND ${CMAKE_READELF} -W -s ${TARGET_NAME}.elf > image_symbol.list
        DEPENDS ${TARGET_NAME}
    )

endfunction(build_rom_callback)
