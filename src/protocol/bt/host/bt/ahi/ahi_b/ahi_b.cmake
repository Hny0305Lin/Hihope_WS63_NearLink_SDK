# 向编译目标添加编译选项
set(CFLAG ${ccflags})

# 向编译目标添加编译宏
set(CCDEFINES ${defines})

set(defines ${defines} "BTH_SYS_PART=100")


set(
    bts_ram_src_list
    ${CMAKE_CURRENT_SOURCE_DIR}/ahi_common/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/ahi_ble/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/ahi_vendor/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/ahi_bta/inc
)

set(BTH_PUBLIC_HDR_LIST "${BTH_PUBLIC_HDR_LIST}" "${bts_ram_src_list}" CACHE INTERNAL "bt ram obj list" FORCE)

######################################### service start ######################################

# 添加 host service 子目录
if(${CHIP} MATCHES "bs20|bs21|bs21e|bs21a|bs22|bs26")
    # add_subdirectory_if_exist(ahi_base)
    # add_subdirectory_if_exist(ahi_mpc)
    if(DEFINED CONFIG_AT_PTS_TEST_SUPPORT) # 只PTS版本编译
        add_subdirectory_if_exist(ahi_common)
    endif()
    add_subdirectory_if_exist(ahi_ble)
    add_subdirectory_if_exist(ahi_vendor)
    add_subdirectory_if_exist(ahi_bta)
elseif(${CHIP} MATCHES "ws63")
    add_subdirectory_if_exist(ahi_common)
    add_subdirectory_if_exist(ahi_ble)
    add_subdirectory_if_exist(ahi_vendor)
    add_subdirectory_if_exist(ahi_bta)
elseif(${CHIP} MATCHES "ws53")
    add_subdirectory_if_exist(ahi_common)
    add_subdirectory_if_exist(ahi_ble)
    add_subdirectory_if_exist(ahi_vendor)
    add_subdirectory_if_exist(ahi_bta)
else()
    add_subdirectory(ahi_common)
    add_subdirectory(ahi_ble)
    add_subdirectory(ahi_br)
    add_subdirectory(ahi_vendor)
endif()
