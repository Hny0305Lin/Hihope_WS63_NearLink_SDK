/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "at_register.h"
#include <string.h>

#include <lwip/inet.h>
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "ping.h"
#include "misc.h"
#include "ifconfig.h"
#include "wifi_device.h"
#include "at.h"
#include "at_wifi.h"
#include "at_utils.h"
#include "at_cmd_register.h"
#include "at_ccpriv.h"
#include "at_sendtest.h"
#include "at_wifi_cmd_register_table.h"
#include "debug_print.h"
#include "version_porting.h"
#ifdef __LITEOS__
#include "los_task_pri.h"
#endif
#ifdef _PRE_SYSCHANNEL_FEATURE
#include "syschannel_api.h"
#include "syschannel_filter_ipv4.h"
#include "cmsis_os2.h"
#include "clock_recover.h"
#include "pinctrl.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef CONFIG_IPERF_SUPPORT
TD_EXTERN td_u32 CmdIperf(td_s32 argc, TD_CONST td_char **argv);
#endif

#ifdef _PRE_SYSCHANNEL_FEATURE
TD_EXTERN syschannel_filter_list_stru *syschannel_get_filter_list(osal_void);
#define SYSCHANNEL_TASK_STACK_SIZE 0x1000
#define SYSCHANNEL_TASK_PRIO (osPriority_t)(25)
#define TASK_COMMON_APP_DELAY_MS 1000
#endif

#define los_unref_param(P)  ((P) = (P))

#define EXT_IP_LINK_ID_MAX            8
#define EXT_IP_TCP_SERVER_LISTEN_NUM  4           /* TCP Maximum number of clients that can be received by the server */
#define EXT_IP_RESV_BUF_LEN           1024        /* IP packet receiving buffer */
#define EXT_IP_SEND_BUF_LEN           1024        /* IP packet sending buffer, which must be the same as the value of
                                                 EXT_AT_DATA_MAX_LEN. */
#define EXT_IP_MUX_WAIT_TIME          TD_SYS_WAIT_FOREVER
#define EXT_PRINT_SIZE_MAX            128
#define EXT_IP_UDP_LINK_MAX           4           /* Maximum number of manually created UDP links */

typedef struct {
    td_s32 sfd;
    td_u8 link_stats;
    td_u8 link_res;
    td_u8 ext_ip_protocol;
    td_u8 res;
} ext_ip_conn_ctl_stru;

typedef struct {
    td_s32 sfd;
    td_u8 link_stats;
    td_u8 res[3]; /* 3 bytes reserved */
} ext_ip_listen_socket_stru;

typedef enum {
    EXT_IP_NULL = 0,
    EXT_IP_TCP  = 1,
    EXT_IP_UDP  = 2,

    EXT_IP_PROTOCAL_BUTT
} ext_ip_protocol;
typedef td_u8 ip_protocol_uint8;

typedef enum {
    EXT_IP_LINK_RES_INIT = 0, /* Initial Value */
    EXT_IP_LINK_MANUAL  = 1,  /* Creating a Link manually */
    EXT_IP_LINK_AUTO  = 2,    /* Creating Links automatically */

    EXT_IP_LINK_RES_BUTT
} ext_ip_link_res;
typedef td_u8 ip_link_res_uint8;

typedef enum {
    EXT_IP_LINK_ID_IDLE = 0,      /* Idle state */
    EXT_IP_LINK_WAIT_RESV,
    EXT_IP_LINK_WAIT_CLOSE,
    EXT_IP_LINK_USER_CLOSE,
    EXT_IP_LINK_SERVER_LISTEN,    /* SERVER Listening state */

    EXT_IP_LINK_STAUS_BUTT
} ext_ip_link_stats;
typedef td_u8 ip_link_stats_uint8;

#ifndef CONFIG_FACTORY_TEST_MODE
TD_PRV ext_ip_conn_ctl_stru g_ip_link_ctl[EXT_IP_LINK_ID_MAX];
TD_PRV td_s8 g_ip_task_exit;
TD_PRV ext_ip_listen_socket_stru g_listen_fd;
TD_PRV td_s32 g_ip_taskid = -1;
TD_PRV td_s8 g_link_id = -1;
TD_PRV in_addr_t g_peer_ipaddr;
TD_PRV td_u16 g_peer_port;
TD_PRV osal_mutex g_ip_mux_id = {};
#endif

TD_PRV td_u32 at_task_show(td_void)
{
#ifndef CONFIG_FACTORY_TEST_MODE
// AT+TASK命令,打印进程信息,freertos不涉及
#ifdef __LITEOS__
    osal_printk("task_info:\r\n");
    OsDbgTskInfoGet(OS_ALL_TASK_MASK);
#endif
#endif
    return EXT_ERR_SUCCESS;
}

#ifdef _PRE_SYSCHANNEL_FEATURE
static td_bool g_chan_reint_type = TD_FALSE;
TD_PRV int syschannel_set_ipv6_default_filter(void)
{
    int ret;
    syschannel_ipv6_filter filter_ipv6 = {0};
    filter_ipv6.local_port = 68;             /* 68 UDP port */
    filter_ipv6.packet_type = 17; /* UDP 17, TCP 6 */
    filter_ipv6.match_mask = WIFI_FILTER_MASK_LOCAL_PORT | WIFI_FILTER_MASK_PROTOCOL;
    filter_ipv6.config_type = WIFI_FILTER_LWIP; /* XX_VLWIP to T31, XX_LWIP to ws53 */
    ret = uapi_syschannel_add_filter((td_char*)&filter_ipv6, sizeof(filter_ipv6), WIFI_FILTER_TYPE_IPV6);
    printf("add mcu dhcp filter6 ret %d\n", ret);
    return ret;
}

TD_PRV int syschannel_set_ipv4_default_filter(void)
{
    int ret;
    syschannel_ipv4_filter filter_ipv4 = {0};
    filter_ipv4.local_port = 68;             /* 68 DHCP UDP port */
    filter_ipv4.packet_type = 17; /* UDP 17, TCP 6 */
    filter_ipv4.match_mask = WIFI_FILTER_MASK_LOCAL_PORT | WIFI_FILTER_MASK_PROTOCOL;
    filter_ipv4.config_type = WIFI_FILTER_LWIP; /* XX_VLWIP to T31, XX_LWIP to ws53 */
    ret = uapi_syschannel_add_filter((td_char*)&filter_ipv4, sizeof(filter_ipv4), WIFI_FILTER_TYPE_IPV4);
    printf("add mcu listen 68 filter4 ret 0x%x\n", ret);

    (void)memset_s(&filter_ipv4, sizeof(filter_ipv4), 0, sizeof(filter_ipv4));
    filter_ipv4.local_port = 67;             /* 67 DHCP UDP port */
    filter_ipv4.packet_type = 17; /* UDP 17, TCP 6 */
    filter_ipv4.match_mask = WIFI_FILTER_MASK_LOCAL_PORT | WIFI_FILTER_MASK_PROTOCOL;
    filter_ipv4.config_type = WIFI_FILTER_LWIP; /* XX_VLWIP to T31, XX_LWIP to ws53 */
    ret = uapi_syschannel_add_filter((td_char*)&filter_ipv4, sizeof(filter_ipv4), WIFI_FILTER_TYPE_IPV4);
    printf("add mcu listen 67 filter4 ret 0x%x\n", ret);

    (void)memset_s(&filter_ipv4, sizeof(filter_ipv4), 0, sizeof(filter_ipv4));
    filter_ipv4.local_port = 6001;          /* 6001 TCP port */
    filter_ipv4.packet_type = 6; /* UDP 17, TCP 6 */
    filter_ipv4.match_mask = WIFI_FILTER_MASK_PROTOCOL | WIFI_FILTER_MASK_LOCAL_PORT;
    filter_ipv4.config_type = WIFI_FILTER_LWIP; /* XX_VLWIP to T31, XX_LWIP to ws53 */
    ret = uapi_syschannel_add_filter((td_char*)&filter_ipv4, sizeof(filter_ipv4), WIFI_FILTER_TYPE_IPV4);
    printf("add mcu listen 6001 filter4 ret 0x%x\n", ret);

    (void)memset_s(&filter_ipv4, sizeof(filter_ipv4), 0, sizeof(filter_ipv4));
    filter_ipv4.remote_port = 6002;          /* 6002 TCP port */
    filter_ipv4.packet_type = 6; /* UDP 17, TCP 6 */
    filter_ipv4.match_mask = WIFI_FILTER_MASK_PROTOCOL | WIFI_FILTER_MASK_REMOTE_PORT;
    filter_ipv4.config_type = WIFI_FILTER_LWIP; /* XX_VLWIP to T31, XX_LWIP to ws53 */
    ret = uapi_syschannel_add_filter((td_char*)&filter_ipv4, sizeof(filter_ipv4), WIFI_FILTER_TYPE_IPV4);
    printf("add mcu listen 6002 filter4 ret 0x%x\n", ret);

    (void)memset_s(&filter_ipv4, sizeof(filter_ipv4), 0, sizeof(filter_ipv4));
    filter_ipv4.local_port = 7001;           /* 7001 UDP port */
    filter_ipv4.packet_type = 17; /* UDP 17, TCP 6 */
    filter_ipv4.match_mask = WIFI_FILTER_MASK_LOCAL_PORT | WIFI_FILTER_MASK_PROTOCOL;
    filter_ipv4.config_type = WIFI_FILTER_LWIP; /* XX_VLWIP to T31, XX_LWIP to ws53 */
    ret = uapi_syschannel_add_filter((td_char*)&filter_ipv4, sizeof(filter_ipv4), WIFI_FILTER_TYPE_IPV4);
    printf("add mcu listen 7001 filter4 ret 0x%x\n", ret);

    (void)memset_s(&filter_ipv4, sizeof(filter_ipv4), 0, sizeof(filter_ipv4));
    filter_ipv4.remote_port = 7002;           /* 7002 UDP port */
    filter_ipv4.packet_type = 17; /* UDP 17, TCP 6 */
    filter_ipv4.match_mask = WIFI_FILTER_MASK_REMOTE_PORT | WIFI_FILTER_MASK_PROTOCOL;
    filter_ipv4.config_type = WIFI_FILTER_LWIP; /* XX_VLWIP to T31, XX_LWIP to ws53 */
    ret = uapi_syschannel_add_filter((td_char*)&filter_ipv4, sizeof(filter_ipv4), WIFI_FILTER_TYPE_IPV4);
    printf("add mcu listen 7002 filter4 ret 0x%x\n", ret);
    return ret;
}

TD_PRV int syschannel_set_default_filter(void)
{
    int ret = uapi_syschannel_set_default_filter(WIFI_FILTER_VLWIP);
    printf("set all net packets foward to camera default.\n");

    if (syschannel_set_ipv4_default_filter() != EXT_ERR_SUCCESS) {
        ret = EXT_ERR_FAILURE;
    }

    if (syschannel_set_ipv6_default_filter() != EXT_ERR_SUCCESS) {
        ret = EXT_ERR_FAILURE;
    }
    return ret;
}

TD_PRV int sdio_init_task_body(void *param)
{
    unused(param);
    osal_printk("syschannel start sdio init:%d\r\n", g_chan_reint_type);
    osDelay(TASK_COMMON_APP_DELAY_MS);
    uapi_watchdog_disable();
#ifndef CONFIG_FACTORY_TEST_MODE
    if (g_chan_reint_type) {
        if (uapi_syschannel_dev_reinit(SDIO_TYPE) != EXT_ERR_SUCCESS) {
            osal_printk("syschannel_init:: syschannel_dev_reinit failed");
        }
        uapi_watchdog_enable();
        return 0;
    } else {
        if (uapi_syschannel_dev_init(SDIO_TYPE) != EXT_ERR_SUCCESS) {
            osal_printk("syschannel_init:: syschannel_dev_init failed");
            uapi_watchdog_enable();
            return 0;
        }
    }
#endif
    uapi_watchdog_enable();

    syschannel_set_default_filter();
    osal_printk("finish sdio init\r\n");
    return 0;
}
#endif

/* AT+ADDFILTER */
at_ret_t at_syschannel_add_filter(const addfilter_args_t *args)
{
#ifdef _PRE_SYSCHANNEL_FEATURE
    td_s32 ret = 0;
    syschannel_ipv4_filter* filter = TD_NULL;
    syschannel_ipv6_filter* ipv6_filter = TD_NULL;
    unsigned char type = args->para6;
    int len = 0;
    PRINT("syschannel_ipv4_filter length is:%d \r\n", sizeof(syschannel_ipv4_filter));
    PRINT("syschannel_ipv6_filter length is:%d \r\n", sizeof(syschannel_ipv6_filter));
    /* 区分ipv4和ipv6 */
    if (args->para6 == 0) {
        if (args->para1 != 0) {
            filter = (syschannel_ipv4_filter *)malloc(sizeof(syschannel_ipv4_filter));
            (void)memset_s(filter, sizeof(syschannel_ipv4_filter), 0, sizeof(syschannel_ipv4_filter));
            filter->local_port = args->para1;             /* 67 DHCP port */
            filter->packet_type = args->para2; /* UDP 17, TCP 6 */
            filter->match_mask = args->para3;
            filter->config_type = args->para4; /* XX_VLWIP to T31, XX_LWIP to ws53 */
        }

        if (args->para5 == 0) {
            len = sizeof(syschannel_ipv4_filter);
        } else {
            len = args->para5;
        }

        ret = uapi_syschannel_add_filter((char *)filter, len, type);
        free(filter);
        return ret;
    } else if (args->para6 == 1) {
        if (args->para1 != 0) {
            ipv6_filter = (syschannel_ipv6_filter *)malloc(sizeof(syschannel_ipv6_filter));
            (void)memset_s(ipv6_filter, sizeof(syschannel_ipv6_filter), 0, sizeof(syschannel_ipv6_filter));
            ipv6_filter->local_port = args->para1;             /* 67 DHCP port */
            ipv6_filter->packet_type = args->para2; /* UDP 17, TCP 6 */
            ipv6_filter->match_mask = args->para3;
            ipv6_filter->config_type = args->para4; /* XX_VLWIP to T31, XX_LWIP to ws53 */
        }

        if (args->para5 == 0) {
            len = sizeof(syschannel_ipv6_filter);
        } else {
            len = args->para5;
        }

        ret = uapi_syschannel_add_filter((char *)ipv6_filter, len, type);
        free(ipv6_filter);
        return ret;
    }
    return AT_RET_SYNTAX_ERROR;
#else
    los_unref_param(args);
    return AT_RET_OK;
#endif
}

at_ret_t at_syschannel_del_filter(const delfilter_args_t *args)
{
#ifdef _PRE_SYSCHANNEL_FEATURE
    td_s32 ret = 0;
    syschannel_ipv4_filter* filter = TD_NULL;
    syschannel_ipv6_filter* ipv6_filter = TD_NULL;
    unsigned char type = args->para6;
    int len = 0;
    PRINT("syschannel_ipv4_filter length is:%d \r\n", sizeof(syschannel_ipv4_filter));
    PRINT("syschannel_ipv6_filter length is:%d \r\n", sizeof(syschannel_ipv6_filter));
    /* 区分ipv4和ipv6 */
    if (args->para6 == 0) {
        if (args->para1 != 0) {
            filter = (syschannel_ipv4_filter *)malloc(sizeof(syschannel_ipv4_filter));
            (void)memset_s(filter, sizeof(syschannel_ipv4_filter), 0, sizeof(syschannel_ipv4_filter));
            filter->local_port = args->para1;             /* 67 DHCP port */
            filter->packet_type = args->para2; /* UDP 17, TCP 6 */
            filter->match_mask = args->para3;
            filter->config_type = args->para4; /* XX_VLWIP to T31, XX_LWIP to ws53 */
        }

        if (args->para5 == 0) {
            len = sizeof(syschannel_ipv4_filter);
        } else {
            len = args->para5;
        }

        ret = uapi_syschannel_del_filter((char *)filter, len, type);
        free(filter);
        return ret;
    } else if (args->para6 == 1) {
        if (args->para1 != 0) {
            ipv6_filter = (syschannel_ipv6_filter *)malloc(sizeof(syschannel_ipv6_filter));
            (void)memset_s(ipv6_filter, sizeof(syschannel_ipv6_filter), 0, sizeof(syschannel_ipv6_filter));
            ipv6_filter->local_port = args->para1;             /* 67 DHCP port */
            ipv6_filter->packet_type = args->para2; /* UDP 17, TCP 6 */
            ipv6_filter->match_mask = args->para3;
            ipv6_filter->config_type = args->para4; /* XX_VLWIP to T31, XX_LWIP to ws53 */
        }

        if (args->para5 == 0) {
            len = sizeof(syschannel_ipv6_filter);
        } else {
            len = args->para5;
        }

        ret = uapi_syschannel_del_filter((char *)ipv6_filter, len, type);
        free(ipv6_filter);
        return ret;
    }
    return AT_RET_SYNTAX_ERROR;
#else
    los_unref_param(args);
    return AT_RET_OK;
#endif
}

at_ret_t at_syschannel_query_filter(const queryfilter_args_t *args)
{
#ifdef _PRE_SYSCHANNEL_FEATURE
    td_s32 ret = 0;
    char *filter = TD_NULL;
    int *num = TD_NULL;
    td_u8 filter_num;
    if (args->para2 != 0) {
        num = (int *)malloc(sizeof(int));
    }
    unsigned char type = args->para3;
    if (args->para1 == 0) {
        ret = uapi_syschannel_query_filter(TD_NULL, num, type);
        PRINT("num = %d \r\n", *num);
        if (args->para2 != 0) {
            free(num);
        }
        return 0;
    }
    syschannel_filter_list_stru *syschannel_filter = syschannel_get_filter_list();
    ret = uapi_syschannel_query_filter((char **)&filter, num, type);
    if (filter == TD_NULL) {
        PRINT("return filter is null!!!!! \r\n");
    }
    /* 区分ipv4和ipv6 */
    if (args->para1 != 0) {
        if ((ret == 0) && (filter != TD_NULL) && (args->para3 == 0)) {
            filter_num = syschannel_filter->ipv4_filter_num;
            syschannel_ipv4_filter *ipv4_filter = (syschannel_ipv4_filter *)filter;
            for (int i = 0; i < filter_num; i++) {
                PRINT("local_port= %d; packet_type=%d; match_mask=%d; config_type = %d \r\n", ipv4_filter[i].local_port,
                    ipv4_filter[i].packet_type, ipv4_filter[i].match_mask, ipv4_filter[i].config_type);
            }
        } else if ((ret == 0) && (filter != TD_NULL) && (args->para3 == 1)) {
            filter_num = syschannel_filter->ipv6_filter_num;
            syschannel_ipv6_filter *ipv6_filter = (syschannel_ipv6_filter *)filter;
            for (int i = 0; i < filter_num; i++) {
                PRINT("local_port= %d; packet_type=%d; match_mask=%d; config_type = %d \r\n", ipv6_filter[i].local_port,
                    ipv6_filter[i].packet_type, ipv6_filter[i].match_mask, ipv6_filter[i].config_type);
            }
        }
    }

    if (args->para2 != 0) {
        PRINT("length = %d \r\n", *num);
    }
    free(num);
    return ret;
#else
    los_unref_param(args);
    return AT_RET_OK;
#endif
}

#ifdef _PRE_SYSCHANNEL_FEATURE
static void sdio_hw_init(void)
{
    u_m_cken_0 m_cken_0;
    u_m_cken_1 m_cken_1;

    m_cken_0.u32 = readl(M_CTL_RB_M_CLKEN0);
    m_cken_0.bits.sdio_dev_clken = 1;
    writel(M_CTL_RB_M_CLKEN0, m_cken_0.u32);

    m_cken_1.u32 = readl(M_CTL_RB_M_CLKEN1);
    m_cken_1.bits.sdio_ahb_clken = 1;
    writel(M_CTL_RB_M_CLKEN1, m_cken_1.u32);

    uapi_pin_init();
    uapi_pin_set_mode(S_MGPIO0, 1);
    writel(0x570360b8, 1); // S_MGPIO1
    uapi_pin_set_mode(S_MGPIO2, 1);
    uapi_pin_set_mode(S_MGPIO3, 1);
    uapi_pin_set_mode(S_MGPIO4, 1);
    uapi_pin_set_mode(S_AGPIO5, 1);
}
#endif

TD_PRV td_u32 at_task_syschannel(td_bool reinit)
{
#ifdef _PRE_SYSCHANNEL_FEATURE
    osThreadAttr_t attr;

    sdio_hw_init();
    g_chan_reint_type = reinit;
    idle_set_open_pm(0);   // 关A核低功耗流程

    attr.name = "syschannel_init";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = SYSCHANNEL_TASK_STACK_SIZE;
    attr.priority = SYSCHANNEL_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)sdio_init_task_body, NULL, &attr) == NULL) {
        osal_printk("Falied to create syschannel init task!\n");
        return AT_RET_SYNTAX_ERROR;
    }
#else
    unused(reinit);
#endif
    return AT_RET_OK;
}

/*****************************************************************************
 功能描述  :设置mac地址
*****************************************************************************/
at_ret_t cmd_set_macaddr(const mac_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    td_uchar mac_addr[WIFI_MAC_LEN] = {0};

    if (strlen(args->para1) != 17) { /* 17 mac string len */
        return AT_RET_SYNTAX_ERROR;
    }

    td_u32 ret = cmd_strtoaddr(args->para1, mac_addr, WIFI_MAC_LEN);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (wifi_set_base_mac_addr((td_char*)mac_addr, WIFI_MAC_LEN) != 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    PRINT("OK\r\n");
    return AT_RET_OK;
}

/*****************************************************************************
 功能描述  :获取mac地址
*****************************************************************************/
at_ret_t cmd_get_macaddr(void)
{
    td_uchar mac_addr[WIFI_MAC_LEN] = {0};

    if (wifi_get_base_mac_addr((unsigned char *)mac_addr, WIFI_MAC_LEN) != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    PRINT("+MAC:" EXT_AT_MACSTR "\r\n", ext_at_mac2str(mac_addr));
    PRINT("OK\r\n");

    return AT_RET_OK;
}

at_ret_t at_iperf(const iperf_args_t *args)
{
#ifdef CONFIG_IPERF_SUPPORT
#ifdef __LITEOS__
    // 防止LiteOS任务无法回收，触发一次任务回收
    LOS_TaskResRecycle();
#endif
    if (CmdIperf(convert_bin_to_dec(args->para_map), (td_char *)&args->para1) == 0) {
        PRINT("AT_IPERF OK\r\n");
        return AT_RET_OK;
    }
    PRINT("AT_IPERF ERROR\r\n");
    return AT_RET_SYNTAX_ERROR;
#else
    los_unref_param(args);
    return AT_RET_OK;
#endif
}

at_ret_t at_sendtest(const sendtest_args_t *args)
{
#ifdef CONFIG_SENDTEST_SUPPORT
#ifdef __LITEOS__
    // 防止LiteOS任务无法回收，触发一次任务回收
    LOS_TaskResRecycle();
#endif
    if (cmd_sendtest(convert_bin_to_dec(args->para_map), (td_char *)&args->para1) != 0) {
        uapi_at_printf("OK\r\n");
        return AT_RET_OK;
    }

    return AT_RET_SYNTAX_ERROR;
#else
    los_unref_param(args);
    return AT_RET_OK;
#endif
}

td_s32 at_check_null_param(int argc, const char **argv)
{
    td_s32 i;
    if ((argc < 1) || (argv == NULL)) {
        return -1;
    }

    for (i = 0; i < argc; i++) {
        if (argv[i] == NULL) {
            return -1;
        }
    }
    return 0;
}

at_ret_t at_ping(const ping_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    if (at_check_null_param(argc, &args->para1) != 0) {
        return AT_RET_SYNTAX_ERROR;
    }
#ifdef __LITEOS__
    // 防止LiteOS任务无法回收，触发一次任务回收
    LOS_TaskResRecycle();
#endif
    if (os_shell_ping(argc, (td_char *)&args->para1) == 0) {
        return AT_RET_OK;
    }

    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_ping6(const ping6_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    if (at_check_null_param(argc, &args->para1) != 0) {
        return AT_RET_SYNTAX_ERROR;
    }
#ifdef __LITEOS__
    // 防止LiteOS任务无法回收，触发一次任务回收
    LOS_TaskResRecycle();
#endif
#if defined(LWIP_IPV6) && (LWIP_IPV6)
    if (os_shell_ping6(argc, (td_char *)&args->para1) == 0) {
        return AT_RET_OK;
    }
#endif

    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_sys_info(void)
{
    print_version();
    td_s32 ret = at_task_show();
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

at_ret_t cmd_start_syschannel(void)
{
    td_s32 ret = at_task_syschannel(TD_FALSE);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

at_ret_t cmd_reinit_syschannel(void)
{
    td_s32 ret = at_task_syschannel(TD_TRUE);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

at_ret_t at_dns(const dns_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    if (os_shell_dns(argc, (td_char *)&args->para1) == 0) {
        return AT_RET_OK;
    }
    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_show_dns(void)
{
    if (os_shell_show_dns() == 0) {
        return AT_RET_OK;
    }
    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_netstat(void)
{
    if (os_shell_netstat(0, TD_NULL) == 0) {
        return AT_RET_OK;
    }
    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_setup_dhcp(const dhcp_args_t *args)
{
    td_s32 ret;

#ifndef CONFIG_FACTORY_TEST_MODE
    struct netif *netif_p = netifapi_netif_find((td_char *)(args->para1));
    if (netif_p == TD_NULL) {
        uapi_at_report("netif_p null\r\n");
        return AT_RET_SYNTAX_ERROR;
    }

    if (args->para2 == 1) {
        ret = netifapi_dhcp_start(netif_p);
    } else if (args->para2 == 0) {
        ret = netifapi_dhcp_stop(netif_p);
    } else if (args->para2 == 2) {
        ret = netifapi_netif_common(netif_p, dhcp_clients_info_show, TD_NULL);
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
#endif
    if (ret == 0) {
        uapi_at_report("OK\r\n");
        return AT_RET_OK;
    }

    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_setup_dhcps(const dhcps_args_t *args)
{
    td_s32 ret;

#ifndef CONFIG_FACTORY_TEST_MODE
    struct netif *netif_p = netifapi_netif_find((td_char *)(args->para1));
    if (netif_p == TD_NULL) {
        uapi_at_report("netif_p == TD_NULL\r\n");
        return AT_RET_SYNTAX_ERROR;
    }
    if (ip_addr_isany_val(netif_p->ip_addr)) {
        uapi_at_report("Please set ip address for dhcp server\r\n");
        return AT_RET_SYNTAX_ERROR;
    }
    if (args->para2 == 1) {
        ret = netifapi_dhcps_start(netif_p, TD_NULL, 0);
    } else if (args->para2 == 0) {
        ret = netifapi_dhcps_stop(netif_p);
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
#endif
    if (ret == 0) {
        uapi_at_report("OK\r\n");
        return AT_RET_OK;
    }

    return AT_RET_SYNTAX_ERROR;
}

TD_PRV td_u32 lwip_ifconfig_check(td_s32 argc, const ifcfg_args_t *args)
{
    if ((argc == 0) || (argc == 1)) {
        return EXT_ERR_SUCCESS;
    } else if (argc == 2) { /* 2个命令参数场景 */
        if ((strcmp("up", (td_char *)(args->para2)) == 0) || (strcmp("down", (td_char *)(args->para2)) == 0)) {
            return EXT_ERR_SUCCESS;
        } else {
            return EXT_ERR_FAILURE;
        }
    } else if (argc == 6) { /* 6个命令参数场景 */
        if ((strcmp("netmask", (td_char *)(args->para3)) == 0) &&
            (strcmp("gateway", (td_char *)(args->para5)) == 0) && /* 2/4:配置netmask和gateway */
            (strcmp("inet", (td_char *)(args->para2)) != 0) && (strcmp("inet6", (td_char *)(args->para2)) != 0)) {
            return EXT_ERR_SUCCESS;
        } else {
            return EXT_ERR_FAILURE;
        }
    } else {
        return EXT_ERR_FAILURE;
    }
}

#ifndef CONFIG_FACTORY_TEST_MODE
TD_PRV td_void lwip_ifconfig_down(struct netif *netif)
{
    (td_void)netifapi_netif_set_link_down(netif);
    (td_void)netifapi_netif_set_down(netif);
    (td_void)netif_l2_netdev_status_callback(netif);
    (td_void)netifapi_netif_set_addr(netif, TD_NULL, TD_NULL, TD_NULL);
#if defined(LWIP_IPV6) && (LWIP_IPV6)
    for (td_u8 index = 0; index < LWIP_IPV6_NUM_ADDRESSES; index++) {
        (td_void)netifapi_netif_rmv_ip6_address(netif, &netif->ip6_addr[index]);
    }
#endif
}

TD_PRV td_void lwip_ifconfig_up(struct netif *netif, const ifcfg_args_t *args)
{
    ip4_addr_t loop_ipaddr, loop_netmask, loop_gw;

    (td_void)netifapi_netif_set_up(netif);
    (td_void)netif_l2_netdev_status_callback(netif);
    if ((strcmp((td_char *)(args->para1), EXT_DEFAULT_IFNAME_AP) == 0) ||
        (strcmp((td_char *)(args->para1), EXT_DEFAULT_IFNAME_MESH) == 0)) {
        (td_void)netifapi_netif_set_link_up(netif);
#if defined(LWIP_IPV6) && (LWIP_IPV6)
        (td_void)netifapi_netif_add_ip6_linklocal_address(netif, TD_TRUE);
#endif
    } else if (strcmp((td_char *)(args->para1), EXT_DEFAULT_IFNAME_LOCALHOST) == 0) {
        (td_void)netifapi_netif_set_link_up(netif);
        IP4_ADDR(&loop_gw, 127, 0, 0, 1);       /* gateway 127.0.0.1 */
        IP4_ADDR(&loop_ipaddr, 127, 0, 0, 1);   /* ipaddr 127.0.0.1 */
        IP4_ADDR(&loop_netmask, 255, 0, 0, 0);  /* netmask 255.0.0.0 */
        (td_void)netifapi_netif_set_addr(netif, &loop_ipaddr, &loop_netmask, &loop_gw);
        (td_void)netifapi_netif_set_up(netif);
    }
}
#endif

at_ret_t at_lwip_ifconfig_getip(void)
{
    td_char *args = TD_NULL;
    lwip_ifconfig(0, &args);
    osal_printk("OK\r\n");
    return AT_RET_OK;
}

at_ret_t at_lwip_ifconfig(const ifcfg_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);

    td_u32 ret = lwip_ifconfig_check(argc, args);
    if (ret != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
#ifndef CONFIG_FACTORY_TEST_MODE
    if (argc == 2) { /* 2:参数个数 */
        struct netif *netif = netifapi_netif_find((td_char *)(args->para1));
        if (netif == TD_NULL) {
            uapi_at_report("netif == TD_NULL\r\n");
            return AT_RET_SYNTAX_ERROR;
        }

        if (strcmp((td_char *)(args->para2), "down") == 0) {
            lwip_ifconfig_down(netif);
        } else if (strcmp((td_char *)(args->para2), "up") == 0) {
            lwip_ifconfig_up(netif, args);
        }
    } else {
        ret = lwip_ifconfig(argc, (td_char *)&args->para1);
        if (ret == 3) { /* 3:up down 执行成功 */
            uapi_at_printf("OK\r\n");
        }
        return (ret == 0 || ret == 3) ? AT_RET_OK : AT_RET_SYNTAX_ERROR;  /* 3:up down 执行成功 */
    }
#endif
    uapi_at_report("OK\r\n");
    return AT_RET_OK;
}

#ifndef CONFIG_FACTORY_TEST_MODE
TD_PRV td_u32 ip_is_all_link_idle(td_void)
{
    int i;
    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    for (i = 0; i < EXT_IP_LINK_ID_MAX; i++) {
        if (g_ip_link_ctl[i].link_stats != EXT_IP_LINK_ID_IDLE) {
            osal_mutex_unlock(&g_ip_mux_id);
            return EXT_ERR_FAILURE;
        }
    }
    osal_mutex_unlock(&g_ip_mux_id);
    return EXT_ERR_SUCCESS;
}

TD_PRV td_void ip_set_monitor_socket(fd_set *read_set, td_s32 *sfd_max)
{
    td_s32 sfd_max_inter = 0;
    td_u8 i;
    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    for (i = 0; i < EXT_IP_LINK_ID_MAX; i++) {
        if (g_ip_link_ctl[i].link_stats == EXT_IP_LINK_WAIT_RESV) {
            FD_SET(g_ip_link_ctl[i].sfd, read_set);
            if (g_ip_link_ctl[i].sfd > sfd_max_inter) {
                sfd_max_inter = g_ip_link_ctl[i].sfd;
            }
        }
    }
    if (g_listen_fd.link_stats == EXT_IP_LINK_SERVER_LISTEN) {
        FD_SET(g_listen_fd.sfd, read_set);
        if (g_listen_fd.sfd > sfd_max_inter) {
            sfd_max_inter = g_listen_fd.sfd;
        }
    }
    *sfd_max = sfd_max_inter;
    osal_mutex_unlock(&g_ip_mux_id);

    return;
}

TD_PRV td_u32 ip_ip_resv_show_msg(td_u8 link_id)
{
    struct sockaddr_in cln_addr = {0};
    socklen_t cln_addr_len = (socklen_t)sizeof(cln_addr);
    td_u32 print_len = 0;
    td_s32 ret;

    td_char *ip_buffer = (td_char*)malloc(EXT_IP_RESV_BUF_LEN + 1);
    if (ip_buffer == TD_NULL) {
        printf("{ip_ip_resv_output:ip buffer malloc fail}\r\n");
        return EXT_ERR_FAILURE;
    }

    /* 规则6.6: 禁止使用内存操作类危险函数 例外场景(3)从堆中分配内存后，赋予初值 */
    memset_s(ip_buffer, EXT_IP_RESV_BUF_LEN + 1, 0, EXT_IP_RESV_BUF_LEN + 1);
    errno = 0;
    ret = recvfrom(g_ip_link_ctl[link_id].sfd, ip_buffer, EXT_IP_RESV_BUF_LEN, 0,
        (struct sockaddr *)&cln_addr, (socklen_t *)&cln_addr_len);
    if (ret < 0) {
        printf("link %d RESV FAIL\r\n", link_id);
        if ((errno != EINTR) && (errno != EAGAIN)) {
            g_ip_link_ctl[link_id].link_stats = EXT_IP_LINK_WAIT_CLOSE;
        }
        free(ip_buffer);
        return EXT_ERR_FAILURE;
    } else if (ret == 0) {
        g_ip_link_ctl[link_id].link_stats = EXT_IP_LINK_WAIT_CLOSE;
        free(ip_buffer);
        return EXT_ERR_FAILURE;
    }

    if (ret < EXT_PRINT_SIZE_MAX) {
        printf("\r\n+IPD,%d,%d,%s,%d:%s", link_id, ret, inet_ntoa(cln_addr.sin_addr), htons(cln_addr.sin_port),
            ip_buffer);
    } else if ((ret >= EXT_PRINT_SIZE_MAX) && (ret <= EXT_IP_RESV_BUF_LEN)) {
        printf("\r\n+IPD,%d,%d,%s,%d:", link_id, ret, inet_ntoa(cln_addr.sin_addr), htons(cln_addr.sin_port));
        do {
            char print_out_buff[EXT_PRINT_SIZE_MAX] = {0};
            if ((memset_s(print_out_buff, sizeof(print_out_buff), 0x0, sizeof(print_out_buff)) != EOK) ||
                (memcpy_s(print_out_buff, sizeof(print_out_buff) - 1, ip_buffer + print_len,
                sizeof(print_out_buff) - 1) != EOK)) {
                printf("{ip_ip_resv_output: print_out_buff memset_s/memcpy_s fail\r\n}");
            }
            printf("%s", print_out_buff);

            ret -= sizeof(print_out_buff) - 1;
            print_len += sizeof(print_out_buff) - 1;
        } while (ret >= (EXT_PRINT_SIZE_MAX - 1));

        if (ret > 0) {
            printf("%s", ip_buffer + print_len);
        }
    }
    free(ip_buffer);
    return EXT_ERR_SUCCESS;
}

TD_PRV td_void ip_ip_resv_output(TD_CONST fd_set *read_set)
{
    td_u8 link_id;

    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    for (link_id = 0; link_id < EXT_IP_LINK_ID_MAX; link_id++) {
        if ((g_ip_link_ctl[link_id].link_stats == EXT_IP_LINK_WAIT_RESV) && (FD_ISSET(g_ip_link_ctl[link_id].sfd,
            read_set))) {
            if (ip_ip_resv_show_msg(link_id) != EXT_ERR_SUCCESS) {
                continue;
            }
        }
    }
    osal_mutex_unlock(&g_ip_mux_id);
    return;
}

TD_PRV td_void ip_set_tcp_link_info(td_u8 link_id, td_s32 sfd)
{
    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    g_ip_link_ctl[link_id].sfd = sfd;
    g_ip_link_ctl[link_id].link_stats = EXT_IP_LINK_WAIT_RESV;
    g_ip_link_ctl[link_id].link_res = EXT_IP_LINK_MANUAL;
    g_ip_link_ctl[link_id].ext_ip_protocol = EXT_IP_TCP;
    osal_mutex_unlock(&g_ip_mux_id);
}

TD_PRV td_u32 ip_has_idle_udp_link(td_void)
{
    td_u8 udp_link_count = 0;
    td_u8 link_id;

    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    for (link_id = 0; link_id < EXT_IP_LINK_ID_MAX; link_id++) {
        if (g_ip_link_ctl[link_id].ext_ip_protocol == EXT_IP_UDP) {
            udp_link_count++;
        }
    }
    osal_mutex_unlock(&g_ip_mux_id);
    if (udp_link_count >= EXT_IP_UDP_LINK_MAX) {
        return EXT_ERR_FAILURE;
    }
    return EXT_ERR_SUCCESS;
}

TD_PRV td_void ip_set_udp_link_info(td_u8 link_id, td_s32 sfd)
{
    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    g_ip_link_ctl[link_id].sfd = sfd;
    g_ip_link_ctl[link_id].link_stats = EXT_IP_LINK_WAIT_RESV;
    g_ip_link_ctl[link_id].link_res = EXT_IP_LINK_MANUAL;
    g_ip_link_ctl[link_id].ext_ip_protocol = EXT_IP_UDP;
    osal_mutex_unlock(&g_ip_mux_id);
}

TD_PRV td_void ip_tcp_send(td_u8 link_id, TD_CONST td_char *send_msg)
{
    td_s32 ret;
    td_u32 send_len;
    send_len = strlen(send_msg);
    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    ret = send(g_ip_link_ctl[link_id].sfd, send_msg, send_len, 0);
    osal_mutex_unlock(&g_ip_mux_id);
    if (ret <= 0) {
        uapi_at_printf("ERROR\r\n");
        return;
    }
    uapi_at_printf("SEND %d bytes\r\nOK\r\n", ret);
    return;
}

TD_PRV td_void ip_udp_send(td_u8 link_id, in_addr_t peer_ipaddr, td_u16 peer_port, TD_CONST td_char *send_msg)
{
    td_s32 ret;
    struct sockaddr_in cln_addr = {0};
    td_u32 send_len;
    send_len = strlen(send_msg);
    cln_addr.sin_family = AF_INET;
    cln_addr.sin_addr.s_addr = peer_ipaddr;
    cln_addr.sin_port = htons(peer_port);
    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    ret = sendto(g_ip_link_ctl[link_id].sfd, send_msg, send_len, 0,
        (struct sockaddr *)&cln_addr, (socklen_t)sizeof(cln_addr));
    osal_mutex_unlock(&g_ip_mux_id);
    if (ret <= 0) {
        uapi_at_printf("ERROR\r\n");
        return;
    }
    uapi_at_printf("SEND %d bytes\r\nOK\r\n", ret);
    return;
}

td_void at_send_recv_data(td_char *data)
{
    td_char *send_msg = data;
    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    if (g_ip_link_ctl[g_link_id].ext_ip_protocol == EXT_IP_TCP) {
        ip_tcp_send(g_link_id, send_msg);
    } else if (g_ip_link_ctl[g_link_id].ext_ip_protocol == EXT_IP_UDP) {
        ip_udp_send(g_link_id, g_peer_ipaddr, g_peer_port, send_msg);
        g_peer_ipaddr = 0;
        g_peer_port = 0;
    }
    g_link_id = -1;
    osal_mutex_unlock(&g_ip_mux_id);
    return;
}
#endif

#ifndef CONFIG_FACTORY_TEST_MODE
TD_PRV td_u32 ip_close_link(td_s32 link_id)
{
    if (link_id >= EXT_IP_LINK_ID_MAX) {
        uapi_at_printf("invalid link\r\n");
        return EXT_ERR_FAILURE;
    }

    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    if (g_ip_link_ctl[link_id].link_stats == EXT_IP_LINK_ID_IDLE) {
        uapi_at_printf("invalid link\r\n");
        osal_mutex_unlock(&g_ip_mux_id);
        return EXT_ERR_FAILURE;
    }

    g_ip_link_ctl[link_id].link_stats = EXT_IP_LINK_USER_CLOSE;
    osal_mutex_unlock(&g_ip_mux_id);
    return EXT_ERR_SUCCESS;
}

TD_PRV td_u32 ip_tcp_server_close(td_void)
{
    int i;
    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    if (g_listen_fd.link_stats == EXT_IP_LINK_ID_IDLE) {
        uapi_at_printf("no server\r\n");
        osal_mutex_unlock(&g_ip_mux_id);
        return EXT_ERR_FAILURE;
    }

    g_listen_fd.link_stats = EXT_IP_LINK_USER_CLOSE;

    /* 服务器关闭时，与之相连接的所有link都要关闭 */
    for (i = 0; i < EXT_IP_LINK_ID_MAX; i++) {
        if (g_ip_link_ctl[i].link_res == EXT_IP_LINK_AUTO) {
            g_ip_link_ctl[i].link_stats = EXT_IP_LINK_WAIT_CLOSE;
        }
    }
    osal_mutex_unlock(&g_ip_mux_id);
    return EXT_ERR_SUCCESS;
}

TD_PRV td_void ip_link_release(td_u8 link_id)
{
    closesocket(g_ip_link_ctl[link_id].sfd);
    g_ip_link_ctl[link_id].sfd = -1;
    g_ip_link_ctl[link_id].link_stats = EXT_IP_LINK_ID_IDLE;
    g_ip_link_ctl[link_id].link_res = EXT_IP_LINK_RES_INIT;
    g_ip_link_ctl[link_id].ext_ip_protocol = EXT_IP_NULL;
}

TD_PRV td_void ip_monitor_link_close(td_void)
{
    td_u8 i;
    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    for (i = 0; i < EXT_IP_LINK_ID_MAX; i++) {
        if (g_ip_link_ctl[i].link_stats == EXT_IP_LINK_WAIT_CLOSE) {
            uapi_at_printf("link %d CLOSED\r\n", i);
            ip_link_release(i);
        } else if (g_ip_link_ctl[i].link_stats == EXT_IP_LINK_USER_CLOSE) {
            uapi_at_printf("link %d CLOSED\r\n", i);
            ip_link_release(i);
            uapi_at_printf("OK\r\n");
        }
    }

    if (g_listen_fd.link_stats == EXT_IP_LINK_WAIT_CLOSE) {
        closesocket(g_listen_fd.sfd);
        g_listen_fd.sfd = -1;
        g_listen_fd.link_stats = EXT_IP_LINK_ID_IDLE;
    } else if (g_listen_fd.link_stats == EXT_IP_LINK_USER_CLOSE) {
        closesocket(g_listen_fd.sfd);
        g_listen_fd.sfd = -1;
        g_listen_fd.link_stats = EXT_IP_LINK_ID_IDLE;
        uapi_at_printf("OK\r\n");
    }
    osal_mutex_unlock(&g_ip_mux_id);
}

TD_PRV td_void ip_tcp_accept(td_void)
{
    struct sockaddr_in cln_addr = {0};
    socklen_t cln_addr_len = (socklen_t)sizeof(cln_addr);
    td_s32 resv_fd;
    td_s8 link_id = -1;
    td_s8 i;
    td_u32 opt = 1;

    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    resv_fd = accept(g_listen_fd.sfd, (struct sockaddr *)&cln_addr, (socklen_t *)&cln_addr_len);
    if (resv_fd < 0) {
        uapi_at_printf("{accept failed, return is %d}\r\n", resv_fd);
        osal_mutex_unlock(&g_ip_mux_id);
        return;
    }

    /* An unused link ID is found.CNcomment:找到一个没有被使用的link id */
    for (i = 0; i < EXT_IP_LINK_ID_MAX; i++) {
        if (g_ip_link_ctl[i].link_stats == EXT_IP_LINK_ID_IDLE) {
            link_id = i;
            break;
        }
    }
    if ((i >= EXT_IP_LINK_ID_MAX) || (link_id == -1)) {
        uapi_at_printf("no link id to use now\r\n");
        closesocket(resv_fd);
        osal_mutex_unlock(&g_ip_mux_id);
        return;
    }
    setsockopt(g_ip_link_ctl[link_id].sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    /* Updating Connection Information */
    g_ip_link_ctl[link_id].sfd = resv_fd;
    g_ip_link_ctl[link_id].link_stats = EXT_IP_LINK_WAIT_RESV;
    g_ip_link_ctl[link_id].link_res = EXT_IP_LINK_AUTO;
    g_ip_link_ctl[link_id].ext_ip_protocol = EXT_IP_TCP;
    uapi_at_printf("%d,CONNECT\r\n", link_id);
    osal_mutex_unlock(&g_ip_mux_id);

    return;
}

TD_PRV td_void ip_monitor_failure_proc(td_void)
{
    td_u32 i;

    for (i = 0; i < EXT_IP_LINK_ID_MAX; i++) {
        if (g_ip_link_ctl[i].link_stats != EXT_IP_LINK_ID_IDLE) {
            ip_link_release(i);
        }
    }
    if (g_listen_fd.link_stats != EXT_IP_LINK_ID_IDLE) {
        closesocket(g_listen_fd.sfd);
        g_listen_fd.sfd = -1;
        g_listen_fd.link_stats = EXT_IP_LINK_ID_IDLE;
    }

    uapi_at_printf("{ip_monitor : ip monitor exit}\r\n");
}

TD_PRV td_void ip_monitor(td_void)
{
    td_s32 sfd_max;
    fd_set read_set;
    struct timeval time_val;
    td_s32 ret;

    osal_mutex_init(&g_ip_mux_id);
    g_ip_task_exit = 0;
    while (!g_ip_task_exit) {
#ifdef LOS_CONFIG_SUPPORT_CPUP
        uapi_cpup_load_check_proc(uapi_task_get_current_id(), LOAD_SLEEP_TIME_DEFAULT);
#endif
        /* When all links are in the idle state and no socket is listened on, exit ip_monitor. */
        if ((ip_is_all_link_idle() == EXT_ERR_SUCCESS) && (g_listen_fd.link_stats == EXT_IP_LINK_ID_IDLE)) {
            osal_mutex_destroy(&g_ip_mux_id);
            g_ip_task_exit = 1;
            continue;
        }

        ip_monitor_link_close();

        FD_ZERO(&read_set);
        sfd_max = 0;
        ip_set_monitor_socket(&read_set, &sfd_max);
        time_val.tv_sec = 0;
        time_val.tv_usec = 500000; /* 500000 Timeout interval is 500ms */
        ret = lwip_select(sfd_max + 1, &read_set, 0, 0, &time_val);
        if (ret < 0) {
            printf("{ip_monitor : socket select failure\r\n");
            ip_monitor_failure_proc();
            osal_mutex_destroy(&g_ip_mux_id);
            break;
        } else if (ret == 0) {
            continue;
        }
        ip_ip_resv_output(&read_set);

        if ((g_listen_fd.link_stats == EXT_IP_LINK_SERVER_LISTEN) && (FD_ISSET(g_listen_fd.sfd, &read_set))) {
            ip_tcp_accept();
        }
    }
    g_ip_taskid = -1;
    return;
}

TD_PRV td_u32 ip_creat_ip_task(td_void)
{
#ifdef __LITEOS__
    TSK_INIT_PARAM_S start_ip_task;
    if (g_ip_taskid > 0) {
        return EXT_ERR_SUCCESS;
    }
    start_ip_task.pfnTaskEntry = (TSK_ENTRY_FUNC)ip_monitor;
    start_ip_task.uwStackSize  = 4096;  /* 4096:任务栈大小设置为4k */
    start_ip_task.pcName = "at_ip_monitor_task";
    start_ip_task.usTaskPrio = 28; /* 28:Task Priority */
    start_ip_task.uwResved = LOS_TASK_STATUS_DETACHED;
    td_u32 ret = LOS_TaskCreate((UINT32 *)(&g_ip_taskid), &start_ip_task);
    if (ret != EXT_ERR_SUCCESS) {
        uapi_at_printf("{ip_creat_ip_task:task create failed 0x%08x.}\r\n", ret);
        return EXT_ERR_FAILURE;
    }
#endif
    return EXT_ERR_SUCCESS;
}

TD_PRV td_u32 ip_start_tcp_client(td_u8 link_id, const td_char *peer_ipaddr, td_u16 peer_port)
{
    td_s32 ret;
    td_u32 opt = 0;
    td_s32 tos;
    struct sockaddr_in srv_addr = {0};

    if (link_id >= EXT_IP_LINK_ID_MAX) {
        return EXT_ERR_FAILURE;
    }
    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    if (g_ip_link_ctl[link_id].link_stats != EXT_IP_LINK_ID_IDLE) {
        uapi_at_printf("invalid link\r\n");
        osal_mutex_unlock(&g_ip_mux_id);
        return EXT_ERR_FAILURE;
    }
    osal_mutex_unlock(&g_ip_mux_id);

    td_s32 sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        uapi_at_printf("{ip_start_tcp_client: socket fail}\r\n");
        return EXT_ERR_FAILURE;
    }

    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    tos = 128; /* 128:TOS设定为128，对应tid = 4，WLAN_WME_AC_VI */
    ret = setsockopt(sfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
    if (ret) {
        uapi_at_printf("{ip_start_tcp_client: setsockopt TOPS fail, return is %d}\r\n", ret);
        closesocket(sfd);
        return EXT_ERR_FAILURE;
    }
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(peer_ipaddr);
    srv_addr.sin_port = htons (peer_port);
    ret = connect(sfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
    if (ret != 0) {
        uapi_at_printf("{ip_start_tcp_client: connect fail, return is %d}\r\n", ret);
        closesocket(sfd);
        return EXT_ERR_FAILURE;
    }

    ip_set_tcp_link_info(link_id, sfd);
    if (ip_creat_ip_task() != EXT_ERR_SUCCESS) {
        uapi_at_printf("{ip_start_tcp_client: creat ip task fail}\r\n");
        osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
        ip_link_release(link_id);
        osal_mutex_unlock(&g_ip_mux_id);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

TD_PRV td_u32 ip_start_udp(td_u8 link_id, td_u16 local_port)
{
    struct sockaddr_in srv_addr = {0};
    td_s32 ret;
    td_u32 opt = 0;
    td_u32 tos;

    if ((link_id >= EXT_IP_LINK_ID_MAX) || (ip_has_idle_udp_link() != EXT_ERR_SUCCESS)) {
        return EXT_ERR_FAILURE;
    }
    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    if (g_ip_link_ctl[link_id].link_stats != EXT_IP_LINK_ID_IDLE) {
        uapi_at_printf("invalid link\r\n");
        osal_mutex_unlock(&g_ip_mux_id);
        return EXT_ERR_FAILURE;
    }
    osal_mutex_unlock(&g_ip_mux_id);

    td_s32 sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1) {
        uapi_at_printf("{ip_start_udp: socket fail}\r\n");
        return EXT_ERR_FAILURE;
    }

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_addr.sin_port = htons(local_port);
    ret = bind(sfd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
    if (ret != 0) {
        uapi_at_printf("{ip_start_udp:bind failed, return is %d}\r\n", ret);
        closesocket(sfd);
        return EXT_ERR_FAILURE;
    }
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    tos = 128; /* 128:TOS is set to 128, corresponding to tid = 4，WLAN_WME_AC_VI */
    ret = setsockopt(sfd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
    if (ret) {
        uapi_at_printf("{ip_start_udp: setsockopt TOPS fail, return is %d}\r\n", ret);
        closesocket(sfd);
        return EXT_ERR_FAILURE;
    }

    ip_set_udp_link_info(link_id, sfd);
    if (ip_creat_ip_task() != EXT_ERR_SUCCESS) {
        uapi_at_printf("{ip_start_tcp_client: creat ip task fail}\r\n");
        osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
        ip_link_release(link_id);
        osal_mutex_unlock(&g_ip_mux_id);
        return EXT_ERR_FAILURE;
    }

    return EXT_ERR_SUCCESS;
}

at_ret_t at_start_ip(const ipstart_args_t *args)
{
    td_u8 link_id;
    td_u16 peer_port;
    td_u16 local_port;

    td_s32 argc = convert_bin_to_dec(args->para_map);
    if (((argc != 3) && (argc != 4)) || at_check_null_param(argc, &args->para1) != 0) {  /* 3 4 */
        return AT_RET_SYNTAX_ERROR;
    }

    link_id = (td_u8)atoi(args->para1);
    const td_char *protocol = args->para2;
    if (strcmp(protocol, "tcp") == 0) {
        if ((argc != 4) || (integer_check(args->para4) == EXT_ERR_FAILURE)) { /* 4 Parameter verification */
            return AT_RET_SYNTAX_ERROR;
        }
        const td_char *peer_ipaddr = args->para3;
        peer_port = (td_u16)atoi(args->para4);
        if (ip_start_tcp_client(link_id, peer_ipaddr, peer_port) != EXT_ERR_SUCCESS) {
            return AT_RET_SYNTAX_ERROR;
        }
    } else if (strcmp(protocol, "udp") == 0) {
        if ((argc != 3) || integer_check(args->para3) == EXT_ERR_FAILURE) { /* 3 Parameter verification */
            return AT_RET_SYNTAX_ERROR;
        }
        local_port = (td_u16)atoi(args->para3); /* 2 Parameter verification */
        if (ip_start_udp(link_id, local_port) != EXT_ERR_SUCCESS) {
            return AT_RET_SYNTAX_ERROR;
        }
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
    printf("OK\r\n");
    return AT_RET_OK;
}

at_ret_t ip_send_tcp(td_s32 argc, const ipsend_args_t *args)
{
    td_char *send_msg = NULL;
    td_u16 len;
    td_char *msg = NULL;

    if (argc != 3) { /* 3 Parameter verification */
        uapi_at_printf("invalid link\r\n");
        osal_mutex_unlock(&g_ip_mux_id);
        return AT_RET_SYNTAX_ERROR;
    }

    send_msg = args->para3;

    len = (td_u16)atoi(args->para2);
    msg = (td_char*)malloc(len + 1);
    if (msg == TD_NULL) {
        printf("ip_send_tcp::malloc fail\r\n");
        osal_mutex_unlock(&g_ip_mux_id);
        return AT_RET_SYNTAX_ERROR;
    }
    memset_s(msg, len + 1, '\0', len + 1);
    if (strncpy_s(msg, len + 1, send_msg, len) != EOK) {
        printf("ip_send_tcp::strncpy_s fail\r\n");
        free(msg);
        osal_mutex_unlock(&g_ip_mux_id);
        return AT_RET_SYNTAX_ERROR;
    }

    ip_tcp_send(g_link_id, msg);

    free(msg);
    osal_mutex_unlock(&g_ip_mux_id);
    return AT_RET_OK;
}

at_ret_t ip_send_udp(td_s32 argc, const ipsend_args_t *args)
{
    td_char *send_msg = NULL;
    td_u16 len;
    td_char *msg = NULL;

    if (argc != 5) { /* 5 Parameter verification */
        uapi_at_printf("invalid link\r\n");
        osal_mutex_unlock(&g_ip_mux_id);
        return AT_RET_SYNTAX_ERROR;
    }
    g_peer_ipaddr = inet_addr(args->para3);
    if (integer_check(args->para4) == EXT_ERR_FAILURE) {
        osal_mutex_unlock(&g_ip_mux_id);
        return AT_RET_SYNTAX_ERROR;
    }
    g_peer_port = (td_u16)atoi(args->para4);
    send_msg = args->para5;

    len = (td_u16)atoi(args->para2);
    msg = (td_char*)malloc(len + 1);
    if (msg == TD_NULL) {
        printf("ip_send_udp::malloc fail\n");
        osal_mutex_unlock(&g_ip_mux_id);
        return AT_RET_SYNTAX_ERROR;
    }
    memset_s(msg, len + 1, '\0', len + 1);
    if (strncpy_s(msg, len + 1, send_msg, len) != EOK) {
        printf("ip_send_udp::strncpy_s fail\n");
        free(msg);
        osal_mutex_unlock(&g_ip_mux_id);
        return AT_RET_SYNTAX_ERROR;
    }

    ip_udp_send(g_link_id, g_peer_ipaddr, g_peer_port, msg);

    free(msg);
    osal_mutex_unlock(&g_ip_mux_id);
    return AT_RET_OK;
}

at_ret_t at_ip_send(const ipsend_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    if ((at_param_null_check(argc, &args->para1) != 0) || ((argc != 3) && (argc != 5)) || /* 3 5 */
        ((integer_check(args->para1) == EXT_ERR_FAILURE) || (integer_check(args->para2) == EXT_ERR_FAILURE)) ||
        (atoi(args->para2) <= 0) || (atoi(args->para2) > EXT_IP_SEND_BUF_LEN)) {
        return AT_RET_SYNTAX_ERROR;
    }

    g_link_id = (td_u16)atoi(args->para1);
    if ((g_link_id < 0) || (g_link_id >= EXT_IP_LINK_ID_MAX)) {
        uapi_at_printf("invalid link\r\n");
        return AT_RET_SYNTAX_ERROR;
    }

    osal_mutex_lock_timeout(&g_ip_mux_id, EXT_IP_MUX_WAIT_TIME);
    if (g_ip_link_ctl[g_link_id].link_stats == EXT_IP_LINK_ID_IDLE) {
        uapi_at_printf("invalid link\r\n");
        osal_mutex_unlock(&g_ip_mux_id);
        return AT_RET_SYNTAX_ERROR;
    }

    if (g_ip_link_ctl[g_link_id].ext_ip_protocol == EXT_IP_TCP) {
        return ip_send_tcp(argc, args);
    } else if (g_ip_link_ctl[g_link_id].ext_ip_protocol == EXT_IP_UDP) {
        return ip_send_udp(argc, args);
    }

    osal_mutex_unlock(&g_ip_mux_id);
    return AT_RET_SYNTAX_ERROR;
}

TD_PRV td_u32 ip_tcp_server_start(td_u16 local_port)
{
    struct sockaddr_in srv_addr = {0};
    td_s32 ret;
    td_u32 opt = 1;

    if (g_listen_fd.link_stats == EXT_IP_LINK_SERVER_LISTEN) {
        uapi_at_printf("server is running\r\n");
        return EXT_ERR_FAILURE;
    }

    g_listen_fd.sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_listen_fd.sfd == -1) {
        uapi_at_printf("{ip_tcp_server_start: creat socket failed}\r\n");
        return EXT_ERR_FAILURE;
    }

    setsockopt(g_listen_fd.sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_addr.sin_port = htons(local_port);
    ret = bind(g_listen_fd.sfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
    if (ret != 0) {
        uapi_at_printf("{ip_tcp_server_start:bind failed, return is %d}\r\n", ret);

        closesocket(g_listen_fd.sfd);
        g_listen_fd.sfd = -1;
        g_listen_fd.link_stats = EXT_IP_LINK_ID_IDLE;
        return EXT_ERR_FAILURE;
    }

    ret = listen(g_listen_fd.sfd, EXT_IP_LINK_SERVER_LISTEN);
    if (ret != 0) {
        uapi_at_printf("{ip_tcp_server_start:listen failed, return is %d\n}", ret);

        closesocket(g_listen_fd.sfd);
        g_listen_fd.sfd = -1;
        g_listen_fd.link_stats = EXT_IP_LINK_ID_IDLE;
        return EXT_ERR_FAILURE;
    }

    if (ip_creat_ip_task() != EXT_ERR_SUCCESS) {
        uapi_at_printf("{ip_tcp_server_start:ip_creat_ip_task fail}\r\n");

        closesocket(g_listen_fd.sfd);
        g_listen_fd.sfd = -1;
        g_listen_fd.link_stats = EXT_IP_LINK_ID_IDLE;
        return EXT_ERR_FAILURE;
    }

    g_listen_fd.link_stats = EXT_IP_LINK_SERVER_LISTEN;

    return EXT_ERR_SUCCESS;
}

at_ret_t at_ip_tcp_server(const iplisten_args_t *args)
{
    td_u16 local_port;
    td_s32 server_ctl;

    td_s32 argc = convert_bin_to_dec(args->para_map);
    if (at_param_null_check(argc, &args->para1) != 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    if ((argc != 1) && (argc != 2)) { /* 1 2 Parameter verification */
        return AT_RET_SYNTAX_ERROR;
    }
    if (integer_check(args->para1) == EXT_ERR_FAILURE) {
        return AT_RET_SYNTAX_ERROR;
    }

    server_ctl = atoi(args->para1);
    if (server_ctl == 1) {
        if ((args->para2 == TD_NULL) || (integer_check(args->para2) == EXT_ERR_FAILURE)) {
            return AT_RET_SYNTAX_ERROR;
        }
        local_port = (td_u16)atoi(args->para2);
        if (ip_tcp_server_start(local_port) != EXT_ERR_SUCCESS) {
            return AT_RET_SYNTAX_ERROR;
        }
    } else if (server_ctl == 0) {
        if (argc != 1) {
            return AT_RET_SYNTAX_ERROR;
        }
        if (ip_tcp_server_close() != EXT_ERR_SUCCESS) {
            return AT_RET_SYNTAX_ERROR;
        }
        return AT_RET_OK;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
    uapi_at_printf("OK\r\n");

    return AT_RET_OK;
}

at_ret_t at_ip_close_link(const ipclose_args_t *args)
{
    td_s32 link_id;
    td_s32 argc = convert_bin_to_dec(args->para_map);
    if ((argc != 1) || (integer_check(args->para1) == EXT_ERR_FAILURE)) {
        return AT_RET_SYNTAX_ERROR;
    }

    link_id = atoi(args->para1);
    if (ip_close_link(link_id) != EXT_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

at_ret_t at_brctl(const brctl_args_t *args)
{
    td_s32 argc = convert_bin_to_dec(args->para_map);
    if ((argc != 1) && (argc != 2)) { /* 2:operation + ifname */
        return AT_RET_SYNTAX_ERROR;
    }

    if ((strcmp(args->para1, "addbr") == 0) || (strcmp(args->para1, "delbr") == 0)) {
        return wifi_brctl_setbr(args->para1);
    } else if (strcmp(args->para1, "show") == 0) {
        return wifi_brctl_show(args->para1);
    } else if ((strcmp(args->para1, "addif") == 0) || (strcmp(args->para1, "delif") == 0)) {
        return wifi_brctl_setif(args->para1, args->para2);
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
}
#endif

#define AT_GENERAL_FUNC_NUM (sizeof(at_wifi_register_parse_table) / sizeof(at_wifi_register_parse_table[0]))

td_void los_at_general_cmd_register(td_void)
{
    uapi_at_register_cmd(at_wifi_register_parse_table, AT_GENERAL_FUNC_NUM);
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
