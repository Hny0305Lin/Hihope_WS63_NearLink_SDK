#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "lwip")

set(CMAKE_LWIP_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/lwip_v2.1.3)
if("liteos_208_6_0_b017_cat1" IN_LIST TARGET_COMPONENT)
    set(CMAKE_LWIP_ADAPTER_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/lwip_adapter/liteos_207)
elseif("liteos_208_5_0" IN_LIST TARGET_COMPONENT)
    set(CMAKE_LWIP_ADAPTER_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/lwip_adapter/liteos_207)
elseif("liteos_207_0_0" IN_LIST TARGET_COMPONENT)
    set(CMAKE_LWIP_ADAPTER_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/lwip_adapter/liteos_207)
elseif("liteos_206" IN_LIST TARGET_COMPONENT)
    set(CMAKE_LWIP_ADAPTER_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/lwip_adapter/liteos_206)
elseif("freertos" IN_LIST TARGET_COMPONENT)
    set(CMAKE_LWIP_ADAPTER_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/lwip_adapter/freertos)
elseif("alios_v3.3.0" IN_LIST TARGET_COMPONENT)
    set(CMAKE_LWIP_ADAPTER_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/lwip_adapter/alios)
endif()

set(CMAKE_LITEOS_DIR_V207
    ${CMAKE_SOURCE_DIR}/kernel/liteos/liteos_v207.0.0)

set(SOURCES
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/tcp_out.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/autoip.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/dhcps.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/igmp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/icmp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/dhcp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/lowpower.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/timeouts.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ip6in4.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/debug.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/mdns.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/tcp_sack.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/tcp_in.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/altcp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/tftpc.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/filter.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/sntp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/altcp_tcp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/def.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/dns.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/stats.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/inet6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/mld6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/icmp6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/dhcp6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/ip6_frag.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/ip6_addr.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/ethip6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/nd6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/ip6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/init.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/sys.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/nat64/nat64_addr.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/nat64/nat64_dns64.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/nat64/nat64_v4_dhcpc.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/nat64/nat64.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/altcp_alloc.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/netif/ifaddrs.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/netif/lowpan6_common.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/netif/lowpan6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/if_api.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/netifapi.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/l3event.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/err.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/nat64_api.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/ethtool.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/netdb.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/nettool/ping.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/nettool/misc.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/nettool/ifconfig.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/nettool/utility.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/nettool/netstat.c
)

set(PUBLIC_HEADER
    ${CMAKE_LWIP_SOURCE_DIR}/src/include
    ${CMAKE_LWIP_ADAPTER_DIR}/src/include/lwip
    ${CMAKE_LWIP_ADAPTER_DIR}/src/include
)

set(PRIVATE_HEADER
    ${CMAKE_LWIP_SOURCE_DIR}/src/include
    ${CMAKE_LWIP_SOURCE_DIR}/src/include/lwip/nettool
    ${ROOT_DIR}/include/middleware/services/wifi
)

if("__ALIOS__" IN_LIST DEFINES)
    list(APPEND PRIVATE_HEADER
        ${ROOT_DIR}/kernel/alios/utils/libc/musl_1.2.3/include
        ${ROOT_DIR}/kernel/alios/utils/libc/adapt/include
        ${ROOT_DIR}/kernel/alios/utils/libc/adapt/musl_1.2.3/include
    )
endif()

set(PRIVATE_DEFINES
    LOSCFG_BASE_CORE_USE_MULTI_LIST
)

set(PUBLIC_DEFINES
    LWIP_CONFIG_FILE=\"lwip/lwipopts_default.h\"
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error=logical-op
    -Wno-error=sign-compare
    -Wno-error=jump-misses-init
    -Wno-sign-compare
    -Wno-jump-misses-init
    -Wno-error=unused-parameter
    -Wno-unused-parameter
    -Wno-unused-but-set-variable
    -Wno-error=unused-variable
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})

build_component()

set(COMPONENT_NAME "lwip_tcm")
set(SOURCES
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/sockets.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/inet_chksum.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ip.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/udp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/etharp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/ip4.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/ip4_addr.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/ip4_frag.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ip_addr.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/pbuf.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/mem.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/memp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/netif.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/tcp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/raw.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/netif/ethernet.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/netif/driverif.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/netbuf.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/tcpip.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/api_msg.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/api_lib.c
    ${CMAKE_LWIP_ADAPTER_DIR}/src/arch/sys_arch.c
)

set(COMPONENT_CCFLAGS
    -Wno-error=logical-op
    -Wno-error=sign-compare
    -Wno-error=jump-misses-init
    -Wno-sign-compare
    -Wno-jump-misses-init
    -Wno-error=unused-parameter
    -Wno-unused-parameter
    -Wno-unused-but-set-variable
    -Wno-error=unused-variable
)

set(PRIVATE_HEADER
    ${CMAKE_LWIP_SOURCE_DIR}/src/include
    ${CMAKE_LWIP_SOURCE_DIR}/src/include/lwip/nettool
    ${ROOT_DIR}/include/middleware/services/wifi
)

if("__ALIOS__" IN_LIST DEFINES)
    list(APPEND PRIVATE_HEADER
        ${ROOT_DIR}/kernel/alios/utils/libc/musl_1.2.3/include
        ${ROOT_DIR}/kernel/alios/utils/libc/adapt/include
        ${ROOT_DIR}/kernel/alios/utils/libc/adapt/musl_1.2.3/include
    )
endif()

set(WHOLE_LINK
    true
)

set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})

build_component()
