/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: struction of AT command and public function
 */

#if ! defined (AT_WIFI_REGISTER_AT_CMD_TALBE_H)
#define AT_WIFI_REGISTER_AT_CMD_TALBE_H

#include "at.h"
typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 30 */
    int32_t                 para2;
} dhcp_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /* Optional.   Length: 30 */
    const uint8_t           *para2; /* Optional.   Length: 30 */
    const uint8_t           *para3; /* Optional.   Length: 30 */
    const uint8_t           *para4; /* Optional.   Length: 30 */
    const uint8_t           *para5; /* Optional.   Length: 30 */
    const uint8_t           *para6; /* Optional.   Length: 30 */
    const uint8_t           *para7; /* Optional.   Length: 30 */
    const uint8_t           *para8; /* Optional.   Length: 30 */
    const uint8_t           *para9; /* Optional.   Length: 30 */
    const uint8_t           *para10; /* Optional.   Length: 30 */
    const uint8_t           *para11; /* Optional.   Length: 30 */
    const uint8_t           *para12; /* Optional.   Length: 30 */
    const uint8_t           *para13; /* Optional.   Length: 30 */
    const uint8_t           *para14; /* Optional.   Length: 30 */
    const uint8_t           *para15; /* Optional.   Length: 30 */
    const uint8_t           *para16; /* Optional.   Length: 30 */
    const uint8_t           *para17; /* Optional.   Length: 30 */
    const uint8_t           *para18; /* Optional.   Length: 30 */
} iperf_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /* Optional.   Length: 30 */
    const uint8_t           *para2; /* Optional.   Length: 30 */
    const uint8_t           *para3; /* Optional.   Length: 30 */
    const uint8_t           *para4; /* Optional.   Length: 30 */
    const uint8_t           *para5; /* Optional.   Length: 30 */
    const uint8_t           *para6; /* Optional.   Length: 30 */
    const uint8_t           *para7; /* Optional.   Length: 30 */
    const uint8_t           *para8; /* Optional.   Length: 30 */
    const uint8_t           *para9; /* Optional.   Length: 30 */
    const uint8_t           *para10; /* Optional.   Length: 30 */
    const uint8_t           *para11; /* Optional.   Length: 30 */
    const uint8_t           *para12; /* Optional.   Length: 30 */
    const uint8_t           *para13; /* Optional.   Length: 30 */
    const uint8_t           *para14; /* Optional.   Length: 30 */
    const uint8_t           *para15; /* Optional.   Length: 30 */
} sendtest_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /* Optional.   Length: 32 */
    const uint8_t           *para2; /* Optional.   Length: 32 */
    const uint8_t           *para3; /* Optional.   Length: 32 */
    const uint8_t           *para4; /* Optional.   Length: 32 */
    const uint8_t           *para5; /* Optional.   Length: 32 */
    const uint8_t           *para6; /* Optional.   Length: 32 */
    const uint8_t           *para7; /* Optional.   Length: 32 */
    const uint8_t           *para8; /* Optional.   Length: 32 */
    const uint8_t           *para9; /* Optional.   Length: 32 */
    const uint8_t           *para10; /* Optional.   Length: 32 */
    const uint8_t           *para11; /* Optional.   Length: 32 */
    const uint8_t           *para12; /* Optional.   Length: 32 */
    const uint8_t           *para13; /* Optional.   Length: 32 */
    const uint8_t           *para14; /* Optional.   Length: 32 */
    const uint8_t           *para15; /* Optional.   Length: 32 */
} ping_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 30 */
    int32_t                 para2;
} dhcps_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /* Optional.   Length: 30 */
    const uint8_t           *para2; /* Optional.   Length: 30 */
    const uint8_t           *para3; /* Optional.   Length: 30 */
    const uint8_t           *para4; /* Optional.   Length: 30 */
    const uint8_t           *para5; /* Optional.   Length: 30 */
    const uint8_t           *para6; /* Optional.   Length: 30 */
} ifcfg_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
} mac_args_t;

typedef struct {
    uint32_t                para_map;
} netstat_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /*    Length: 32 */
    const uint8_t           *para2; /*    Length: 32 */
    const uint8_t           *para3; /*    Length: 32 */
    const uint8_t           *para4; /*    Length: 32 */
    const uint8_t           *para5; /*    Length: 32 */
} ping6_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *para1; /* Range: 1..2 */
    const uint8_t           *para2; /*    Length: 32 */
} dns_args_t;

typedef struct {
    uint32_t                para_map;
    uint16_t                para1; /* Range: 0..65535 */
    uint8_t                 para2; /* Range: 0..255   */
    uint8_t                 para3; /* Range: 0..255   */
    uint8_t                 para4; /* Range: 0..255   */
    uint8_t                 para5; /* Range: 0,20,32  */
    uint8_t                 para6; /* Range: 0,1      */
} addfilter_args_t;

typedef struct {
    uint32_t                para_map;
    uint16_t                para1; /* Range: 0..65535 */
    uint8_t                 para2; /* Range: 0..255   */
    uint8_t                 para3; /* Range: 0..255   */
    uint8_t                 para4; /* Range: 0..255   */
    uint8_t                 para5; /* Range: 0,20,32  */
    uint8_t                 para6; /* Range: 0,1      */
} delfilter_args_t;

typedef struct {
    uint32_t                para_map;
    uint8_t                 para1; /* Range: 0,1   */
    uint8_t                 para2; /* Range: 0,1   */
    uint8_t                 para3; /* Range: 0,1   */
} queryfilter_args_t;

typedef struct {
    uint32_t                para_map;
    uint8_t                 *para1; /* Range: 0..7   */
    uint8_t                 *para2; /*    Length: 32 */
    uint8_t                 *para3; /*    Length: 32 */
    uint8_t                 *para4; /*    Length: 32 */
} ipstart_args_t;

typedef struct {
    uint32_t                para_map;
    uint8_t                 *para1; /*    Length: 32 */
    uint8_t                 *para2; /*    Length: 32 */
    uint8_t                 *para3; /*    Length: 1024 */
    uint8_t                 *para4; /*    Length: 32 */
    uint8_t                 *para5; /*    Length: 1024 */
} ipsend_args_t;

typedef struct {
    uint32_t                para_map;
    uint8_t                 *para1; /*    Length: 32 */
    uint8_t                 *para2; /*    Length: 32 */
} iplisten_args_t;

typedef struct {
    uint32_t                para_map;
    uint8_t                 *para1; /*    Length: 32 */
} ipclose_args_t;

typedef union {
    dhcp_args_t         dhcp;
    dhcps_args_t        dhcps;
    ifcfg_args_t        ifcfg;
    iperf_args_t        iperf;
    ping_args_t         ping;
    dns_args_t          dns;
    mac_args_t          mac;
    netstat_args_t      netstat;
    ping6_args_t        ping6;
    addfilter_args_t    addfilter;
    delfilter_args_t    delfilter;
    queryfilter_args_t  queryfilter;
    ipstart_args_t      ipstart;
    ipsend_args_t       ipsend;
    iplisten_args_t     iplisten;
    ipclose_args_t      ipclose;
} at_wifi_register_args_t;

typedef struct {
    uint32_t                para_map;
    uint8_t                 *para1; /*    Length: 32 */
    uint8_t                 *para2; /*    Length: 32 */
} brctl_args_t;

/* AT+SYSINFO */
at_ret_t at_sys_info(void);

/* AT+SYSCHANNEL */
at_ret_t cmd_start_syschannel(void);

/* AT+RESYSCHAN */
at_ret_t cmd_reinit_syschannel(void);

/* AT+ADDFILTER */
at_ret_t at_syschannel_add_filter(const addfilter_args_t *args);
/* AT+DELFILTER */
at_ret_t at_syschannel_del_filter(const delfilter_args_t *args);
/* AT+QUERYFILTER */
at_ret_t at_syschannel_query_filter(const queryfilter_args_t *args);

/* AT+DNS */
at_ret_t at_show_dns(void);
at_ret_t at_dns(const dns_args_t *args);

/* AT+MAC */
at_ret_t cmd_get_macaddr(void);
at_ret_t cmd_set_macaddr(const mac_args_t *args);

/* AT+NETSTAT */
at_ret_t at_netstat(void);

/* AT+PING6 */
at_ret_t at_ping6(const ping6_args_t *args);

/* AT Command */
at_ret_t at_setup_dhcp(const dhcp_args_t *args);

/* AT Command */
at_ret_t at_iperf(const iperf_args_t *args);

/* AT Command */
at_ret_t at_sendtest(const sendtest_args_t *args);

/* AT Command */
at_ret_t at_ping(const ping_args_t *args);

/* AT Command */
at_ret_t at_setup_dhcps(const dhcps_args_t *args);

/* AT Command */
at_ret_t at_lwip_ifconfig_getip(void);
at_ret_t at_lwip_ifconfig(const ifcfg_args_t *args);

/* AT Command */
at_ret_t at_start_ip(const ipstart_args_t *args);

/* AT Command */
at_ret_t at_ip_send(const ipsend_args_t *args);

/* AT Command */
at_ret_t at_ip_tcp_server(const iplisten_args_t *args);

/* AT Command */
at_ret_t at_ip_close_link(const ipclose_args_t *args);

/* AT+BRCTL */
at_ret_t at_brctl(const brctl_args_t *args);

const int32_t dhcp_para2_values[] = {
    0, 1, 2
};
const at_para_parse_syntax_t dhcp_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(dhcp_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, dhcp_para2_values},
        .offset = offsetof(dhcp_args_t, para2)
    },
};

const at_para_parse_syntax_t iperf_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para9)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para10)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para11)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para12)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para13)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para14)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para15)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para16)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para17)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(iperf_args_t, para18)
    },
};

const at_para_parse_syntax_t sendtest_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para9)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para10)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para11)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para12)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para13)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para14)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(sendtest_args_t, para15)
    },
};

const at_para_parse_syntax_t ping_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para6)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para7)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para8)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para9)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para10)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para11)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para12)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para13)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para14)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ping_args_t, para15)
    },
};

const int32_t dhcps_para2_values[] = {
    0, 1
};
const at_para_parse_syntax_t dhcps_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(dhcps_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, dhcps_para2_values},
        .offset = offsetof(dhcps_args_t, para2)
    },
};

const at_para_parse_syntax_t ifcfg_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(ifcfg_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(ifcfg_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(ifcfg_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(ifcfg_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(ifcfg_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 30,
        .offset = offsetof(ifcfg_args_t, para6)
    },
};

const at_para_parse_syntax_t dns_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(dns_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(dns_args_t, para2)
    },
};

const at_para_parse_syntax_t mac_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(mac_args_t, para1)
    },
};

const at_para_parse_syntax_t ping6_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 45,
        .offset = offsetof(ping6_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 45,
        .offset = offsetof(ping6_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 45,
        .offset = offsetof(ping6_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 45,
        .offset = offsetof(ping6_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 45,
        .offset = offsetof(ping6_args_t, para5)
    },
};

const int32_t addfilter_para5_values[] = {
    0, 20, 32
};
const int32_t addfilter_para6_values[] = {
    0, 1
};
const at_para_parse_syntax_t addfilter_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(addfilter_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(addfilter_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(addfilter_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(addfilter_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, addfilter_para5_values},
        .offset = offsetof(addfilter_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, addfilter_para6_values},
        .offset = offsetof(addfilter_args_t, para6)
    },
};

const int32_t delfilter_para5_values[] = {
    0, 20, 32
};
const int32_t delfilter_para6_values[] = {
    0, 1
};
const at_para_parse_syntax_t delfilter_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 65535,
        .offset = offsetof(delfilter_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(delfilter_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(delfilter_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(delfilter_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, delfilter_para5_values},
        .offset = offsetof(delfilter_args_t, para5)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, delfilter_para6_values},
        .offset = offsetof(delfilter_args_t, para6)
    },
};

const int32_t queryfilter_para1_values[] = {
    0, 1
};
const int32_t queryfilter_para2_values[] = {
    0, 1
};
const int32_t queryfilter_para3_values[] = {
    0, 1
};
const at_para_parse_syntax_t queryfilter_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, queryfilter_para1_values},
        .offset = offsetof(queryfilter_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, queryfilter_para2_values},
        .offset = offsetof(queryfilter_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {2, queryfilter_para3_values},
        .offset = offsetof(queryfilter_args_t, para3)
    },
};

const at_para_parse_syntax_t ipstart_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ipstart_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ipstart_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ipstart_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ipstart_args_t, para4)
    },
};

const at_para_parse_syntax_t ipsend_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ipsend_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ipsend_args_t, para2)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 1024,
        .offset = offsetof(ipsend_args_t, para3)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ipsend_args_t, para4)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 1024,
        .offset = offsetof(ipsend_args_t, para5)
    },
};

const at_para_parse_syntax_t iplisten_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(iplisten_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(iplisten_args_t, para2)
    },
};

const at_para_parse_syntax_t ipclose_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(ipclose_args_t, para1)
    },
};

const at_para_parse_syntax_t brctl_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(brctl_args_t, para1)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 32,
        .offset = offsetof(brctl_args_t, para2)
    },
};

const at_cmd_entry_t at_wifi_register_parse_table[] = {
    {
        "DHCP",
        8,
        0,
        dhcp_syntax,
        NULL,
        (at_set_func_t)at_setup_dhcp,
        NULL,
        NULL,
    },
    {
        "PING",
        4,
        0,
        ping_syntax,
        NULL,
        (at_set_func_t)at_ping,
        NULL,
        NULL,
    },
    {
        "DHCPS",
        6,
        0,
        dhcps_syntax,
        NULL,
        (at_set_func_t)at_setup_dhcps,
        NULL,
        NULL,
    },
    {
        "IFCFG",
        6,
        0,
        ifcfg_syntax,
        at_lwip_ifconfig_getip,
        (at_set_func_t)at_lwip_ifconfig,
        NULL,
        NULL,
    },
    {
        "IPERF",
        6,
        0,
        iperf_syntax,
        NULL,
        (at_set_func_t)at_iperf,
        NULL,
        NULL,
    },
    {
        "SENDTEST",
        9,
        0,
        sendtest_syntax,
        NULL,
        (at_set_func_t)at_sendtest,
        NULL,
        NULL,
    },
    {
        "DNS",
        10,
        0,
        dns_syntax,
        NULL,
        (at_set_func_t)at_dns,
        at_show_dns,
        NULL,
    },
    {
        "MAC",
        8,
        0,
        mac_syntax,
        NULL,
        (at_set_func_t)cmd_set_macaddr,
        cmd_get_macaddr,
        NULL,
    },
    {
        "NETSTAT",
        7,
        0,
        NULL,
        at_netstat,
        NULL,
        NULL,
        NULL,
    },
    {
        "PING6",
        9,
        0,
        ping6_syntax,
        NULL,
        (at_set_func_t)at_ping6,
        NULL,
        NULL,
    },
    {
        "SYSINFO",
        17,
        0,
        NULL,
        at_sys_info,
        NULL,
        NULL,
        NULL,
    },
    {
        "SYSCHANNEL",
        10,
        0,
        NULL,
        cmd_start_syschannel,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "RESYSCHAN",
        9,
        0,
        NULL,
        cmd_reinit_syschannel,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "ADDFILTER",
        9,
        0,
        addfilter_syntax,
        NULL,
        (at_set_func_t)at_syschannel_add_filter,
        NULL,
        NULL,
    },
    {
        "DELFILTER",
        9,
        0,
        delfilter_syntax,
        NULL,
        (at_set_func_t)at_syschannel_del_filter,
        NULL,
        NULL,
    },
    {
        "QUERYFILTER",
        11,
        0,
        queryfilter_syntax,
        NULL,
        (at_set_func_t)at_syschannel_query_filter,
        NULL,
        NULL,
    },
    {
        "IPSTART",
        11,
        0,
        ipstart_syntax,
        NULL,
        (at_set_func_t)at_start_ip,
        NULL,
        NULL,
    },
    {
        "IPSEND",
        11,
        0,
        ipsend_syntax,
        NULL,
        (at_set_func_t)at_ip_send,
        NULL,
        NULL,
    },
    {
        "IPLISTEN",
        11,
        0,
        iplisten_syntax,
        NULL,
        (at_set_func_t)at_ip_tcp_server,
        NULL,
        NULL,
    },
    {
        "IPCLOSE",
        11,
        0,
        ipclose_syntax,
        NULL,
        (at_set_func_t)at_ip_close_link,
        NULL,
        NULL,
    },
    {
        "BRCTL",
        11,
        0,
        brctl_syntax,
        NULL,
        (at_set_func_t)at_brctl,
        NULL,
        NULL,
    },
};

#endif  /* AT_WIFI_REGISTER_AT_CMD_TALBE_H */

