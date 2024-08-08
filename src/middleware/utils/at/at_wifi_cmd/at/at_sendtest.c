/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: no.
 * Create: 2022-3-8
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include "debug_print.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "at.h"
#include "at_wifi.h"
#include "los_cpup.h"
#include "los_task_pri.h"
#include "at_sendtest.h"

typedef struct {
    iperf_cfg_t cfg;
    td_bool finish;
    td_u64 total_len;
    td_u32 buffer_len;
    td_u8 *buffer;
    td_u32 sock_fd;
    td_u8 tos;
    td_u8 pad[3];
    td_u64 bandwidth;
} send_ctrl_t;

typedef struct {
    td_s32 id;
    td_u32 sec;
    td_u32 usec;
} send_udp_pkt_t;

#define SOCKET_NUMBER             5
#define MAC_STRING_LEN            17
#define MAC_SEP_COLON_TAG         ":"
#define MAC_LEN                   6
#define STR_LEN                   16
#define STR_SIZE                  2
#define BUF_LEN                   512
#define WOL_LEN                   102
#define BYTE_TO_BIT               8
#define LOAD_SLEEP_TIME_DEFAULT   30
#define SEC2USEC                  1000000
#define SENDTEST_UDP_DELAY        (SEC2USEC / LOSCFG_BASE_CORE_TICK_PER_SECOND)
#define SENDTEST_512K             0x80000
#define SENDTEST_HUNDRED          100
#define SENDTEST_INVALID_TASK_ID  0xFFFFFFFF

#define SENDTEST_MIN_IDEL_RATE 50

#define sendtest_printf(fmt, args...) printf(fmt, ##args)

static td_u32 g_send_taskid[SOCKETS_MAX_NUM] = {SENDTEST_INVALID_TASK_ID};
static td_u32 g_send_report_taskid[SOCKETS_MAX_NUM] = {SENDTEST_INVALID_TASK_ID};
static td_bool g_send_is_running[SOCKETS_MAX_NUM];
static send_ctrl_t g_send_ctrl[SOCKETS_MAX_NUM];
static td_u8 g_current_task_index = 0;
static td_s32 g_send_socket[SOCKETS_MAX_NUM];
static td_s32 g_send_listen_socket[SOCKETS_MAX_NUM];
static td_u8 g_send_report_running = false;
static td_u32 g_task_id_running_set = 0;
td_s32 g_send_delay_flag = 0;
td_s32 g_send_delay_cycle_time = SENDTEST_CYCLE_TIMES_ONCE_SCHEDULE;
static td_bool g_send_init_flag = false;
static struct timeval g_send_timeout_rx, g_send_timeout_tx;
td_s32 send_start_flag = 0;
td_s32 send_wol_flag = 0;
td_u8 send_wol_mac[MAC_LEN];
td_s32 gindex = 0;

inline static td_bool send_is_udp_client(int current_index)
{
    return ((g_send_ctrl[current_index].cfg.flag & SENDTEST_FLAG_CLIENT) &&
        (g_send_ctrl[current_index].cfg.flag & SENDTEST_FLAG_UDP));
}

inline static td_bool send_is_udp_server(int current_index)
{
    return ((g_send_ctrl[current_index].cfg.flag & SENDTEST_FLAG_SERVER) &&
        (g_send_ctrl[current_index].cfg.flag & SENDTEST_FLAG_UDP));
}

inline static td_bool send_is_tcp_client(int current_index)
{
    return ((g_send_ctrl[current_index].cfg.flag & SENDTEST_FLAG_CLIENT) &&
        (g_send_ctrl[current_index].cfg.flag & SENDTEST_FLAG_TCP));
}

inline static td_bool send_is_tcp_server(int current_index)
{
    return ((g_send_ctrl[current_index].cfg.flag & SENDTEST_FLAG_SERVER) &&
        (g_send_ctrl[current_index].cfg.flag & SENDTEST_FLAG_TCP));
}

static void send_init(void)
{
    td_s32 i;
    for (i = 0; (i < SOCKETS_MAX_NUM) && !g_send_init_flag; i++) {
        g_send_socket[i] = INVALID_SOCKET;
        g_send_listen_socket[i] = INVALID_SOCKET;
        g_send_is_running[i] = false;
    }
    g_send_init_flag = true;
}

static void printf_send(int index)
{
    td_bool ret;

    ret = send_is_udp_client(index);
    sendtest_printf("send_is_udp_client = %d\r\n", ret);
    ret = send_is_udp_server(index);
    sendtest_printf("send_is_udp_server = %d\r\n", ret);
    ret = send_is_tcp_client(index);
    sendtest_printf("send_is_tcp_client = %d\r\n", ret);
    ret = send_is_tcp_server(index);
    sendtest_printf("send_is_tcp_server = %d\r\n", ret);
    unused(ret);
}

static uint32_t send_get_float_int(float in)
{
    return (uint32_t)(((uint64_t)(in * SENDTEST_HUNDRED)) / SENDTEST_HUNDRED);
}

static uint32_t send_get_float_dec(float in)
{
    return (uint32_t)(((uint64_t)(in * SENDTEST_HUNDRED)) % SENDTEST_HUNDRED);
}

static void send_printf_bits(int index, td_u64 buff_len, td_u32 interval, td_u32 start_time, td_u32 end_time)
{
    int iperf_bits = SENDTEST_MBIT;
    char bit_type = 'M';
    float rate;

    if (interval == 0) {
        return;
    }

    if ((buff_len * BYTE_TO_BIT) / interval >= SENDTEST_MBIT) {
        iperf_bits = SENDTEST_MBIT;
        bit_type = 'M';
    } else {
        iperf_bits = SENDTEST_KBIT;
        bit_type = 'K';
    }
    rate = (float)(buff_len * BYTE_TO_BIT) / interval / iperf_bits;

    sendtest_printf("[%d]:: %4d-%4d sec       %d.%2d %cbits/sec\r\n", index, start_time, end_time,
        send_get_float_int(rate), send_get_float_dec(rate), bit_type);
    unused(iperf_bits);
    unused(bit_type);
}

static td_s32 send_set_mss(td_s32 sock_fd)
{
    td_s32 ret, opt;
    ret = setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
    if (ret) {
        sendtest_printf("setsockopt error!\r\n");
        return ESP_FAIL;
    }
    return ESP_OK;
}

static int send_get_socket_error_code(int sock_fd)
{
    socklen_t optlen = sizeof(socklen_t);
    int result;
    int err;

    /* get the error state, and clear it */
    err = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &result, &optlen);
    if (err == -1) {
        return -1;
    }

    return result;
}

static void send_report_task(void)
{
    int i;
    td_u32 interval[SOCKETS_MAX_NUM] = {0};
    td_u64 last_len[SOCKETS_MAX_NUM] = {0};
    td_u32 cur[SOCKETS_MAX_NUM] = {0};
    td_u32 cur_interval[SOCKETS_MAX_NUM] = {0};

    sendtest_printf("\r\n%16s    %s\r\n", "Interval", "Bandwidth");

    while (g_task_id_running_set != 0) {
        for (i = 0; i < SOCKETS_MAX_NUM; i++) {
            if (!g_send_ctrl[i].finish || cur[i] == 0) {
                continue;
            }
            sendtest_printf("======");
            send_printf_bits(i, g_send_ctrl[i].total_len, cur[i], 0, cur[i]);
            cur_interval[i] = 0;
            cur[i] = 0;
            last_len[i] = 0;
            g_task_id_running_set &= ~(1 << i);
        }

        if (g_task_id_running_set == 0) {
            break;
        }

        LOS_TaskDelay(LOSCFG_BASE_CORE_TICK_PER_SECOND);

        for (i = 0; i < SOCKETS_MAX_NUM; i++) {
            if (!g_send_is_running[i] || g_send_ctrl[i].finish || (g_task_id_running_set & (1 << i)) == 0) {
                continue;
            }

            cur_interval[i] += 1;
            interval[i] = g_send_ctrl[i].cfg.interval;
            if (cur_interval[i] < interval[i]) {
                continue;
            }
            cur_interval[i] = 0;
            send_printf_bits(i, g_send_ctrl[i].total_len - last_len[i], interval[i], cur[i], cur[i] + interval[i]);
            cur[i] += interval[i];
            last_len[i] = g_send_ctrl[i].total_len;
            if (cur[i] >= g_send_ctrl[i].cfg.time) {
                sendtest_printf("\r\n cur >= time and break! \r\n");
                g_send_ctrl[i].finish = true;
                g_send_report_taskid[i] = SENDTEST_INVALID_TASK_ID;
                continue;
            }
        }
    }

    g_send_report_running = false;
    sendtest_printf("sendtest report finished \r\n");
}

static td_s32 send_start_report(int current_index)
{
    int ret;
    int index = current_index;
    TSK_INIT_PARAM_S stapp_task;

    /* report already running, notify report stask */
    g_task_id_running_set |= (1 << index);
    if (g_send_report_running) {
        return 0;
    }

    g_send_report_running = true;
    (void)memset_s(&stapp_task, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    /* create task for send_start_report */
    stapp_task.pfnTaskEntry = (TSK_ENTRY_FUNC)send_report_task;
    stapp_task.uwStackSize  = SENDTEST_REPORT_TASK_STACK;
    stapp_task.pcName = SENDTEST_REPORT_TASK_NAME;
    stapp_task.usTaskPrio = SENDTEST_REPORT_TASK_PRIORITY;
    stapp_task.uwResved   = LOS_TASK_STATUS_DETACHED;
    ret = LOS_TaskCreate(&g_send_report_taskid[index], &stapp_task);
    sendtest_printf("g_send_report_taskid[%d] = %d\r\n", index, g_send_report_taskid[index]);
    if (ret != ESP_OK) {
        g_task_id_running_set = 0;
        g_send_report_running = false;
        sendtest_printf("send_task create failed ! err code[%d]\r\n", ret);
        return ESP_FAIL;
    }

    return 0;
}

static void send_task_delay(void)
{
#ifdef LOSCFG_KERNEL_CPUP
    td_u32 ret = LOS_HistoryTaskCpuUsage(OsGetIdleTaskId(), CPUP_LAST_ONE_SECONDS);
    if (ret < SENDTEST_MIN_IDEL_RATE) {
        uapi_watchdog_kick();
    }
#else
    uapi_watchdog_kick();
#endif
}

static td_s32 send_run_tcp_server_rev(int index, int listen_socket, struct sockaddr *remote_sockaddr,
    socklen_t addr_len, struct sockaddr_in* remote_addr)
{
    g_send_listen_socket[index] = listen_socket;
    td_u8 *buffer = g_send_ctrl[index].buffer;
    int want_recv = g_send_ctrl[index].buffer_len;
    while (!g_send_ctrl[index].finish) {
        int sock_fd = accept(listen_socket, remote_sockaddr, &addr_len);
        if (sock_fd < 0) {
            closesocket(listen_socket);
            g_send_ctrl[index].finish = true;
            g_send_socket[index] = INVALID_SOCKET;
            g_send_listen_socket[index] = INVALID_SOCKET;
            if (g_send_ctrl[index].buffer) {
                buffer = NULL;
                free(g_send_ctrl[index].buffer);
                g_send_ctrl[index].buffer = NULL;
            }
            return ESP_FAIL;
        }
        g_send_socket[index] = sock_fd;
        sendtest_printf("accept: %d\r\n", htons(remote_addr->sin_port));
        send_start_report(index);
        setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &g_send_timeout_rx, sizeof(g_send_timeout_rx));
        if ((g_send_ctrl[index].buffer_len <= SENDTEST_TCP_MSS) && send_set_mss(sock_fd)) {
            closesocket(sock_fd);
            g_send_ctrl[index].finish = true;
            closesocket(listen_socket);
            g_send_socket[index] = INVALID_SOCKET;
            g_send_listen_socket[index] = INVALID_SOCKET;
            if (g_send_ctrl[index].buffer) {
                buffer = NULL;
                free(g_send_ctrl[index].buffer);
                g_send_ctrl[index].buffer = NULL;
            }
            return ESP_FAIL;
        }

        while (!g_send_ctrl[index].finish) {
            int actual_recv = recv(sock_fd, buffer, want_recv, 0);
            if (actual_recv < 0) {
                g_send_ctrl[index].finish = true;
                break;
            } else if (actual_recv == 0) {
                sendtest_printf("Tcp client complete!\r\n");
                g_send_ctrl[index].finish = true;
                break;
            } else {
                g_send_ctrl[index].total_len += actual_recv;
            }
            send_task_delay();
        }

        closesocket(sock_fd);
    }

    g_send_ctrl[index].finish = true;
    closesocket(listen_socket);
    g_send_socket[index] = INVALID_SOCKET;
    g_send_listen_socket[index] = INVALID_SOCKET;
    if (g_send_ctrl[index].buffer) {
        buffer = NULL;
        free(g_send_ctrl[index].buffer);
        g_send_ctrl[index].buffer = NULL;
    }
    return ESP_OK;
}

static td_s32 send_get_inet6_addr_from_ip6addr(struct sockaddr_in6 *addr6, ip_addr_t *send_ctrl_ip, int sock)
{
#if defined(LWIP_IPV4) && defined(LWIP_IPV6) && (LWIP_IPV4) && (LWIP_IPV6)
    inet6_addr_from_ip6addr(&(addr6->sin6_addr), &(send_ctrl_ip->u_addr.ip6));
#elif defined(LWIP_IPV6) && (LWIP_IPV6)
    inet6_addr_from_ip6addr(&(addr6->sin6_addr), send_ctrl_ip);
#else
    closesocket(sock);
    return ESP_FAIL;
#endif
    return ESP_OK;
}

static td_s32 send_get_ip4addr(struct sockaddr_in *addr, ip_addr_t *send_ctrl_ip, int sock)
{
#if defined(LWIP_IPV4) && defined(LWIP_IPV6) && (LWIP_IPV4) && (LWIP_IPV6)
    addr->sin_addr.s_addr = send_ctrl_ip->u_addr.ip4.addr;
#elif defined(LWIP_IPV4) && (LWIP_IPV4)
    addr->sin_addr.s_addr = send_ctrl_ip->addr;
#else
    closesocket(sock);
    return ESP_FAIL;
#endif
    return ESP_OK;
}

static td_s32 send_run_tcp_server(int current_index)
{
    struct sockaddr *sockaddr = NULL;
    socklen_t addr_len;
    struct sockaddr *remote_sockaddr = NULL;
    struct sockaddr_in remote_addr;
    struct sockaddr_in6 remote_addr6;
    struct sockaddr_in addr = {0};
    struct sockaddr_in6 addr6 = {0};
    int domain = AF_INET;
    int opt;
    int index = current_index;

    if (g_send_ctrl[index].cfg.flag & SENDTEST_FLAG_IP6) {
        domain = AF_INET6;
    }

    int listen_socket = socket(domain, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket < 0) {
        return ESP_FAIL;
    }

    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (g_send_ctrl[index].cfg.flag & SENDTEST_FLAG_IP6) {
        addr6.sin6_family = AF_INET6;
        if (send_get_inet6_addr_from_ip6addr(&addr6, &(g_send_ctrl[index].cfg.sip), listen_socket) != ESP_OK) {
            return ESP_FAIL;
        }
        addr6.sin6_port = htons(g_send_ctrl[index].cfg.sport);
        sockaddr = (struct sockaddr *)&addr6;
        addr_len = sizeof(addr6);
        remote_sockaddr = (struct sockaddr *)&remote_addr6;
    } else {
        addr.sin_family = AF_INET;
        addr.sin_port = htons(g_send_ctrl[index].cfg.sport);
        if (send_get_ip4addr(&addr, &(g_send_ctrl[index].cfg.sip), listen_socket) != ESP_OK) {
            return ESP_FAIL;
        }
        sockaddr = (struct sockaddr *)&addr;
        addr_len = sizeof(addr);
        remote_sockaddr = (struct sockaddr *)&remote_addr;
    }

    if (bind(listen_socket, sockaddr, addr_len) != 0) {
        closesocket(listen_socket);
        return ESP_FAIL;
    }

    if (listen(listen_socket, SOCKET_NUMBER) < 0) {
        closesocket(listen_socket);
        return ESP_FAIL;
    }

    return send_run_tcp_server_rev(index, listen_socket, remote_sockaddr, addr_len, &remote_addr);
}

static td_s32 send_run_udp_server_rev(int index, int sock_fd, struct sockaddr_in* addr, socklen_t addr_len)
{
    td_bool connect_flag = false;
    td_u8 *buffer = g_send_ctrl[index].buffer;
    int want_recv = g_send_ctrl[index].buffer_len;

    sendtest_printf("want recv=%d", want_recv);

    g_send_socket[index] = sock_fd;
    while (!g_send_ctrl[index].finish) {
        int actual_recv = recvfrom(sock_fd, buffer, want_recv, 0, (struct sockaddr *)addr, &addr_len);
        if (actual_recv < 0) {
        } else if (*buffer == 0xff) {
            g_send_ctrl[index].finish = true;
            break;
        } else {
            if (!connect_flag) {
                send_start_report(index);
                connect_flag = true;
            }
            g_send_ctrl[index].total_len += actual_recv;
        }
        send_task_delay();
    }

    g_send_ctrl[index].finish = true;
    closesocket(sock_fd);
    g_send_socket[index] = INVALID_SOCKET;
    if (g_send_ctrl[index].buffer) {
        buffer = NULL;
        free(g_send_ctrl[index].buffer);
        g_send_ctrl[index].buffer = NULL;
    }
    return ESP_OK;
}

static td_s32 send_run_udp_server(int current_index)
{
    struct sockaddr *sockaddr = NULL;
    socklen_t addr_len;
    struct sockaddr_in addr = {0};
    struct sockaddr_in6 addr6 = {0};
    int domain = AF_INET;
    int opt;
    int index = current_index;

    if (g_send_ctrl[index].cfg.flag & SENDTEST_FLAG_IP6) {
        domain = AF_INET6;
    }

    int sock_fd = socket(domain, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd < 0) {
        return ESP_FAIL;
    }

    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (g_send_ctrl[index].cfg.flag & SENDTEST_FLAG_IP6) {
        addr6.sin6_family = AF_INET6;
        if (send_get_inet6_addr_from_ip6addr(&addr6, &(g_send_ctrl[index].cfg.sip), sock_fd) != ESP_OK) {
            return ESP_FAIL;
        }
        addr6.sin6_port = htons(g_send_ctrl[index].cfg.sport);
        sockaddr = (struct sockaddr *)&addr6;
        addr_len = sizeof(addr6);
    } else {
        addr.sin_family = AF_INET;
        addr.sin_port = htons(g_send_ctrl[index].cfg.sport);
        if (send_get_ip4addr(&addr, &(g_send_ctrl[index].cfg.sip), sock_fd) != ESP_OK) {
            return ESP_FAIL;
        }
        sockaddr = (struct sockaddr *)&addr;
        addr_len = sizeof(addr);
    }
    if (bind(sock_fd, sockaddr, addr_len) != 0) {
        closesocket(sock_fd);
        return ESP_FAIL;
    }

    return send_run_udp_server_rev(index, sock_fd, &addr, addr_len);
}

static void send_run_udp_client_delay(td_u32* delay)
{
    if (*delay < SENDTEST_MAX_DELAY) {
        *delay <<= 1;
    }
}

static void send_write_udp_fin(td_s32 sock_fd, td_u8 *buf, td_s32 buf_len, const struct sockaddr *to,
    socklen_t tolen)
{
    td_s32 rc;
    fd_set read_set;
    struct timeval timeout;
    td_s32 count = 0;

    *buf = 0xff;
    *(buf + 1) = 0xff;
    while (count < SENDTEST_UDP_LAST_DATA_TRY) {
        count++;

        // write data
        sendto(sock_fd, buf, buf_len, 0, to, tolen);
        if (rc < 0) {
            sendtest_printf("send_write_udp_fin: sendto failed on %d, errno = %d\r\n", sock_fd, errno);
        }

        // wait until the socket is readable, or our timeout expires
        FD_ZERO(&read_set);
        FD_SET(sock_fd, &read_set);
        timeout.tv_sec  = 0;
        timeout.tv_usec = 250000; // quarter second, 250 ms

        rc = select(sock_fd + 1, &read_set, (fd_set *)NULL, (fd_set *)NULL, &timeout);
        if (rc == 0) {
            // select timed out
            continue;
        } else {
            // socket ready to read
            rc = recv(sock_fd, buf, buf_len, 0);
            if (rc < 0) {
#ifdef CUSTOM_AT_COMMAND
                sendtest_printf("write_UDP_FIN: rc < 0, read error, errno = %d\r\n", errno);
#else
                sendtest_printf("write_UDP_FIN: rc < 0, read error \r\n");
#endif
                break;
            }
        }
    }

    if (count != SENDTEST_UDP_LAST_DATA_TRY) {
        sendtest_printf("WARNING: did not receive ack of last datagram after 2 tries.\r\n");
    }
}

static int64_t time_usec(struct timeval end, struct timeval start)
{
    const int32_t max_gap = 2000; /* 2000: max delta */
    int32_t delta;

    if (end.tv_sec - start.tv_sec > max_gap) {
        delta = max_gap;
    } else if (end.tv_sec - start.tv_sec < -max_gap) {
        delta = -max_gap;
    } else {
        delta = end.tv_sec - start.tv_sec;
    }
    return (delta * SENDTEST_MBIT) + (end.tv_usec - start.tv_usec);
}

static void sendtest_delay(td_u32 usecs)
{
    td_u32 ticks = usecs / SENDTEST_UDP_DELAY;

    if (ticks) {
        LOS_TaskDelay(ticks);
    } else {
        LOS_TaskDelay(1); /* 1: atleast one tick */
    }
}

static td_s32 send_run_udp_client_send(int index, int sock_fd, struct sockaddr *to, socklen_t tolen)
{
    td_s32 delay = 0;
    td_s32 adjust;
    struct timeval last_pkt_time, pkt_time;

    if (send_start_report(index) != 0) {
        return ESP_FAIL;
    }

    td_u8 *buffer = g_send_ctrl[index].buffer;
    int want_send = g_send_ctrl[index].buffer_len;
    g_send_socket[index] = sock_fd;
    if (send_wol_flag == 1) {
        memset_s(g_send_ctrl[index].buffer, MAC_LEN, 0xff, MAC_LEN);
    }

    gettimeofday(&last_pkt_time, NULL);
    while (!g_send_ctrl[index].finish) {
        gettimeofday(&pkt_time, NULL);
        adjust = g_send_ctrl[index].cfg.delay + time_usec(last_pkt_time, pkt_time);
        last_pkt_time.tv_sec  = pkt_time.tv_sec;
        last_pkt_time.tv_usec = pkt_time.tv_usec;
        if ((adjust > 0) || (delay > 0)) {
            delay += adjust;
        }

        if (delay > (int32_t)SENDTEST_UDP_DELAY) {
            sendtest_delay(delay);
        }

        int actual_send = sendto(sock_fd, buffer, want_send, 0, to, tolen);
        if (actual_send != want_send) {
            int err = send_get_socket_error_code(sock_fd);
            if (err == ENOMEM) {
                sendtest_delay(0);
            } else {
                sendtest_printf("udp client send abort: err=%d", err);
                break;
            }
        } else {
            g_send_ctrl[index].total_len += actual_send;
        }
        send_task_delay();
    }

    send_write_udp_fin(sock_fd, buffer, g_send_ctrl[index].buffer_len, to, tolen);
    send_wol_flag = 0;
    g_send_ctrl[index].finish = true;
    closesocket(sock_fd);
    g_send_socket[index] = INVALID_SOCKET;
    if (g_send_ctrl[index].buffer) {
        free(g_send_ctrl[index].buffer);
        g_send_ctrl[index].buffer = NULL;
    }
    return ESP_OK;
}

static td_s32 send_run_udp_client(int current_index)
{
    struct sockaddr_in addr = {0};
    struct sockaddr_in6 addr6 = {0};
    struct sockaddr *to = NULL;
    socklen_t tolen;
    int domain = AF_INET;
    int opt;
    int index = current_index;

    if (g_send_ctrl[index].cfg.flag & SENDTEST_FLAG_IP6) {
        domain = AF_INET6;
    }

    int sock_fd = socket(domain, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd < 0) {
        return ESP_FAIL;
    }
    if (send_wol_flag == 1) {
        setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
    } else {
        setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }
    sendtest_printf("g_send_ctrl.tos = 0x%x,", g_send_ctrl[index].tos);
    td_s32 tos = (int)g_send_ctrl[index].tos;
    td_s32 ret = setsockopt(sock_fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
    if (ret) {
        sendtest_printf("setsockopt -S tos error!\r\n");
        closesocket(sock_fd);
        return ESP_FAIL;
    }

    if (g_send_ctrl[index].cfg.flag & SENDTEST_FLAG_IP6) {
        addr6.sin6_family = AF_INET6;
        if (send_get_inet6_addr_from_ip6addr(&addr6, &(g_send_ctrl[index].cfg.dip), sock_fd) != ESP_OK) {
            return ESP_FAIL;
        }
        addr6.sin6_port = htons(g_send_ctrl[index].cfg.dport);
        to = (struct sockaddr *)&addr6;
        tolen = sizeof(addr6);
    } else {
        addr.sin_family = AF_INET;
        addr.sin_port = htons(g_send_ctrl[index].cfg.dport);
        if (send_get_ip4addr(&addr, &(g_send_ctrl[index].cfg.dip), sock_fd) != ESP_OK) {
            return ESP_FAIL;
        }
        to = (struct sockaddr *)&addr;
        tolen = sizeof(addr);
    }

    return send_run_udp_client_send(index, sock_fd, to, tolen);
}

static td_s32 send_run_tcp_client_send(int index, int sock_fd)
{
    int actual_send = 0;

    g_send_socket[index] = sock_fd;
    g_send_ctrl[index].finish = false;
    if (send_start_report(index) != 0) {
        return ESP_FAIL;
    }
    td_u8 *buffer = g_send_ctrl[index].buffer;
    int want_send = g_send_ctrl[index].buffer_len;
    sendtest_printf("\r\n want_send = %d \r\n", want_send);
    if (send_wol_flag == 1) {
        memset_s(g_send_ctrl[index].buffer, MAC_LEN, 0xff, MAC_LEN);
    }
    setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &g_send_timeout_tx, sizeof(g_send_timeout_tx));
    while (!g_send_ctrl[index].finish) {
        actual_send = send(sock_fd, buffer, want_send, 0);
        if (actual_send <= 0) {
            break;
        } else {
            g_send_ctrl[index].total_len += actual_send;
        }
        send_task_delay();
    }

    g_send_ctrl[index].finish = true;
    send_wol_flag = 0;
    closesocket(sock_fd);
    g_send_socket[index] = INVALID_SOCKET;
    if (g_send_ctrl[index].buffer) {
        buffer = NULL;
        free(g_send_ctrl[index].buffer);
        g_send_ctrl[index].buffer = NULL;
    }
    return ESP_OK;
}

static td_s32 send_run_tcp_client(int current_index)
{
    struct sockaddr_in remote_addr = {0};
    struct sockaddr_in6 remote_addr6 = {0};
    struct sockaddr *to = NULL;
    int domain = AF_INET;
    socklen_t tolen;
    int opt;
    int tos;
    int index = current_index;

    domain = ((g_send_ctrl[index].cfg.flag & SENDTEST_FLAG_IP6) == 0) ? AF_INET : SENDTEST_FLAG_IP6;

    int sock_fd = socket(domain, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd < 0) {
        return ESP_FAIL;
    }
    if (send_wol_flag == 1) {
        setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
    }
    tos = (int)g_send_ctrl[index].tos;
    if (setsockopt(sock_fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos))) {
        sendtest_printf("setsockopt -S tos error!\r\n");
        closesocket(sock_fd);
        return ESP_FAIL;
    }
    if ((g_send_ctrl[index].buffer_len <= SENDTEST_TCP_MSS) && send_set_mss(sock_fd)) {
        closesocket(sock_fd);
        return ESP_FAIL;
    }

    if (g_send_ctrl[index].cfg.flag & SENDTEST_FLAG_IP6) {
        remote_addr6.sin6_family = AF_INET6;
        if (send_get_inet6_addr_from_ip6addr(&remote_addr6, &(g_send_ctrl[index].cfg.dip), sock_fd) != ESP_OK) {
            return ESP_FAIL;
        }
        remote_addr6.sin6_port = htons(g_send_ctrl[index].cfg.dport);
        to = (struct sockaddr *)&remote_addr6;
        tolen = sizeof(remote_addr6);
    } else {
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = htons(g_send_ctrl[index].cfg.dport);
        if (send_get_ip4addr(&remote_addr, &(g_send_ctrl[index].cfg.dip), sock_fd) != ESP_OK) {
            return ESP_FAIL;
        }
        to = (struct sockaddr *)&remote_addr;
        tolen = sizeof(remote_addr);
    }
    if (connect(sock_fd, to, tolen) < 0) {
        closesocket(sock_fd);
        return ESP_FAIL;
    }

    return send_run_tcp_client_send(index, sock_fd);
}

static void send_task_traffic(void)
{
    int i;
    UINT32 task_id = LOS_CurTaskIDGet();

    for (i = 0; i < SOCKETS_MAX_NUM; i++) {
        if (task_id == g_send_taskid[i]) {
            sendtest_printf("id = %d\r\n", g_send_taskid[i]);
            break;
        }
    }
    if (i == SOCKETS_MAX_NUM) {
        sendtest_printf("task_id  error \r\n");
        return;
    }
    if (send_is_udp_client(i)) {
        send_run_udp_client(i);
    } else if (send_is_udp_server(i)) {
        send_run_udp_server(i);
    } else if (send_is_tcp_client(i)) {
        send_run_tcp_client(i);
    } else if (send_is_tcp_server(i)) {
        send_run_tcp_server(i);
    } else {
        sendtest_printf("flag is error\r\n");
    }

    if (g_send_ctrl[i].buffer) {
        free(g_send_ctrl[i].buffer);
        g_send_ctrl[i].buffer = NULL;
    }
    sendtest_printf("[%d]::task_id %d exit\r\n", i, g_send_taskid[i]);
    g_send_taskid[i] = SENDTEST_INVALID_TASK_ID;
    g_send_is_running[i] = false;
    g_send_delay_cycle_time = SENDTEST_CYCLE_TIMES_ONCE_SCHEDULE;
    g_send_delay_flag = 0;
}

static td_u32 send_get_buffer_len(int index)
{
    if (send_is_udp_client(index)) {
        return SENDTEST_UDP_TX_LEN;
    } else if (send_is_udp_server(index)) {
        return SENDTEST_UDP_RX_LEN;
    } else if (send_is_tcp_client(index)) {
        return SENDTEST_TCP_TX_LEN;
    } else {
        return SENDTEST_TCP_RX_LEN;
    }
    return 0;
}

static td_s32 send_start(void)
{
    td_s32 ret;
    TSK_INIT_PARAM_S stapp_task;

    (void)memset_s(&stapp_task, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    /* create task for iperf */
    stapp_task.pfnTaskEntry = (TSK_ENTRY_FUNC)send_task_traffic;
    stapp_task.uwStackSize  = SENDTEST_TRAFFIC_TASK_STACK;
    stapp_task.pcName = SENDTEST_TRAFFIC_TASK_NAME;
    stapp_task.usTaskPrio = SENDTEST_TRAFFIC_TASK_PRIORITY;
    stapp_task.uwResved   = LOS_TASK_STATUS_DETACHED;
    ret = LOS_TaskCreate(&g_send_taskid[g_current_task_index], &stapp_task);
    sendtest_printf("g_send_taskid[%d] = %d\r\n", g_current_task_index, g_send_taskid[g_current_task_index]);
    if (ret != ESP_OK) {
        sendtest_printf("create task %s failed", SENDTEST_TRAFFIC_TASK_NAME);
        free(g_send_ctrl[g_current_task_index].buffer);
        g_send_ctrl[g_current_task_index].buffer = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

static td_s32 send_stop(td_s32 index)
{
    if (g_send_is_running[index]) {
        g_send_ctrl[index].finish = true;
        sendtest_printf("wait stop ...\r\n");
        (td_void)usleep(300000);  /* 300000 延时300毫秒 */

        closesocket(g_send_socket[index]);
        g_send_socket[index] = INVALID_SOCKET;
        sendtest_printf("sendtest stop\r\n");
    } else {
        sendtest_printf("[%d] not running!\r\n", index);
    }
    closesocket(g_send_listen_socket[index]);
    g_send_listen_socket[index] = INVALID_SOCKET;

    return ESP_OK;
}

static int send_check_parameter_deal(const char **argv, int i, const char* cmd_table, int cmd_table_size)
{
    int j = 0;

    char *charp = (char *)argv[i];
    if (*charp == '-') {
        charp++;
        for (j = 0; j < cmd_table_size; j++) {
            if (*charp == cmd_table[j]) {
                break;
            }
        }
        if (j == cmd_table_size) {
            sendtest_printf("invalid option \"-%s\"\r\n", charp);
            return -1;
        }
    }

    return 0;
}

static int send_check_parameter(int argc, const char **argv)
{
    int i = 0;
    const char cmd_table[] = {'i', 'l', 'u', 's', 'c', 't', 'p', 'k', 'a', 'S', 'B', 'V', 'o', 'g', 'b'};

    for (i = 0; i < argc; i++) {
        if (send_check_parameter_deal(argv, i, cmd_table, sizeof(cmd_table)) != 0) {
            return -1;
        }
    }
    return 0;
}

static BOOL send_overflow32(int index, int argc, const char **argv, td_u32 *gpara)
{
    if (index + 1 == argc) {
        sendtest_printf("parameter %s is error\r\n", argv[index]);
        return true;
    }
    *gpara = atoi(argv[++index]);

    return false;
}

static BOOL send_overflow16(int index, int argc, const char **argv, uint16_t *gpara)
{
    if (index + 1 == argc) {
        sendtest_printf("parameter %s is error\r\n", argv[index]);
        return true;
    }
    *gpara = atoi(argv[++index]);

    return false;
}

static BOOL send_overflow8(int index, int argc, const char **argv, td_u8 *gpara)
{
    if (index + 1 == argc) {
        sendtest_printf("parameter %s is error\r\n", argv[index]);
        return true;
    }
    *gpara = atoi(argv[++index]);

    return false;
}

static unsigned int strtoaddrl(const char *param, unsigned char *mac_addr)
{
    unsigned int ui_cnt;
    char *tmp1 = (char *)param;
    char *tmp2;
    char *tmp3 = NULL;

    for (ui_cnt = 0; ui_cnt < (MAC_LEN - 1); ui_cnt++) {
        tmp2 = (char *)strsep(&tmp1, MAC_SEP_COLON_TAG);
        if (tmp2 == NULL) {
            return 1;
        }
        mac_addr[ui_cnt] = (unsigned char)strtoul(tmp2, &tmp3, STR_LEN);
    }

    if (tmp1 == NULL) {
        return 1;
    }
    mac_addr[ui_cnt] = (unsigned char)strtoul(tmp1, &tmp3, STR_LEN);
    return EXT_ERR_SUCCESS;
}

static void str2nhex(const td_char *string, td_u8 *dest, int destlen)
{
    td_u32 i;
    td_u32 len;
    td_u8 hex[BUF_LEN];

    if (string == NULL) {
        return;
    }

    len = strlen(string);
    if (len > sizeof(hex)) {
        return;
    }

    for (i = 0; i < len; i += STR_SIZE) {
        if ((string[i] >= 'a') && (string[i] <= 'f')) {
            hex[i / STR_SIZE] = (string[i] - 'a' + 0x0a) << 0x4;
        } else if ((string[i] >= 'A') && (string[i] <= 'F')) {
            hex[i / STR_SIZE] = (string[i] - 'A' + 0x0a) << 0x4;
        } else {
            hex[i / STR_SIZE] = (string[i] - '0') << 0x4;
        }

        if ((string[i + 1] >= 'a') && (string[i + 1] <= 'f')) {
            hex[i / STR_SIZE] |= string[i + 1] - 'a' + 0x0a;
        } else if ((string[i+1] >= 'A') && (string[i + 1] <= 'F')) {
            hex[i / STR_SIZE] |= string[i + 1] - 'A' + 0x0a;
        } else {
            hex[i / STR_SIZE] |= string[i + 1] - '0';
        }
    }
    memcpy_s(dest, destlen, hex, len / STR_SIZE);
    return;
}
static td_s32 send_server_second_order_para(td_s32 index, td_s32 argc, const char **argv)
{
    td_s32 i = 0;
    td_s32 ret;
    ip_addr_t sip;

    for (i = 0; i < argc; i++) {
        if ((!strcmp(argv[i], "-p")) && send_overflow16(i, argc, argv, &g_send_ctrl[index].cfg.sport)) {
            return ESP_FAIL;
        }

        if (!strcmp(argv[i], "-B")) {
            if (i + 1 == argc) {
                sendtest_printf("%s error\r\n", argv[i]);
                return ESP_FAIL;
            }
            ret = ipaddr_aton(argv[++i], &sip);
            if (!ret) {
                return ESP_FAIL;
            }
            g_send_ctrl[index].cfg.sip = sip;
        }
    }
    return ESP_OK;
}

static td_s32 send_client_second_order_para(td_s32 index, td_s32 argc, const char **argv)
{
    td_s32 i = 0;
    for (i = 0; i < argc; i++) {
        if ((!strcmp(argv[i], "-p")) && send_overflow16(i, argc, argv, &g_send_ctrl[index].cfg.dport)) {
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

static td_s32 send_server_ip_config(td_s32 index)
{
    ip_addr_t sip;
    errno_t rc;

    if (g_send_ctrl[index].cfg.flag & SENDTEST_FLAG_IP6) {
#if defined(LWIP_IPV6) && (LWIP_IPV6)
        rc = memcpy_s(&sip, sizeof(sip), IP6_ADDR_ANY, sizeof(sip));
#else
        return ESP_FAIL;
#endif
        if (rc != ESP_OK) {
            sendtest_printf("%s memcpy_s failed(%d).", __FUNCTION__, rc);
            return ESP_FAIL;
        }
    } else {
        sip = netif_list->ip_addr;
    }
    g_send_ctrl[index].cfg.sip = sip;

    return ESP_OK;
}

static unsigned int cmd_sendtest_stop(int argc, const char **argv, int j)
{
    if (strcmp(argv[j], "-k") == 0) {
        if ((j + 1) == argc) {
            sendtest_printf("kill all sendtest\r\n", argv[j]);
            for (int i = 0; i < SOCKETS_MAX_NUM; i++) {
                send_stop(i);
            }
        } else {
            td_s32 stop_index = atoi(argv[j + 1]);
            send_stop(stop_index);
        }
        return 0;
    }

    return 1;
}

static unsigned int cmd_sendtest_cyc(int argc, const char **argv, td_u8 index)
{
    ip_addr_t dip;

    for (int i = 0; i < argc; i++) {
        if ((!strcmp(argv[i], "-i")) && send_overflow32(i, argc, argv, &g_send_ctrl[index].cfg.interval)) {
            return 0;
        }

        if (strcmp(argv[i], "-u") == 0) {
            g_send_ctrl[index].cfg.flag &= (~SENDTEST_FLAG_TCP);
            g_send_ctrl[index].cfg.flag |= SENDTEST_FLAG_UDP;
        }

        if (strcmp(argv[i], "-s") == 0) {
            g_send_ctrl[index].cfg.flag |= SENDTEST_FLAG_SERVER;
            if (send_server_ip_config(index)) {
                sendtest_printf("server ip error\r\n");
                return 0;
            }

            if (send_server_second_order_para(index, argc, argv)) {
                sendtest_printf("server parameter error\r\n");
                return 0;
            }
        }

        if (strcmp(argv[i], "-c") == 0) {
            g_send_ctrl[index].cfg.flag |= SENDTEST_FLAG_CLIENT;
            if (i + 1 == argc) {
                sendtest_printf("%s error\r\n", argv[i]);
                return 0;
            }

            if (!ipaddr_aton(argv[++i], &dip)) {
                sendtest_printf("client ip is error\r\n");
                return 0;
            }
            g_send_ctrl[index].cfg.dip = dip;
            sendtest_printf("cfg.dip = %d \r\n", g_send_ctrl[index].cfg.dip);
            if (send_client_second_order_para(index, argc, argv)) {
                sendtest_printf("parameter is error\r\n");
                return 0;
            }
        }

        if (((strcmp(argv[i], "-l") == 0) && send_overflow32(i, argc, argv, &g_send_ctrl[index].buffer_len)) ||
            ((strcmp(argv[i], "-t") == 0) && send_overflow32(i, argc, argv, &g_send_ctrl[index].cfg.time)) ||
            ((strcmp(argv[i], "-S") == 0) && send_overflow8(i, argc, argv, &g_send_ctrl[index].tos)) ||
            ((strcmp(argv[i], "-b") == 0) && send_overflow8(i, argc, argv, &g_send_ctrl[index].bandwidth))) {
            return 0;
        }

        if (strcmp(argv[i], "-V") == 0) {
            g_send_ctrl[index].cfg.flag |= SENDTEST_FLAG_IP6;
        }

        if (strcmp(argv[i], "-o") == 0) {
            memset_s(send_wol_mac, sizeof(send_wol_mac), 0, sizeof(send_wol_mac));
            i++;
            if (strtoaddrl(argv[i], send_wol_mac) != EXT_ERR_SUCCESS) {
                sendtest_printf("Mac set failed!");
                return 0;
            }
            send_wol_flag = 1;
            sendtest_printf("send_wol_mac : 0x%02x,0x%02x,0x%02x,0x%02x,**,**, \n", send_wol_mac[0], send_wol_mac[1],
                send_wol_mac[2], send_wol_mac[3]);
        }
        if (strcmp(argv[i], "-g") == 0) {
            send_wol_flag = 0x2;
            gindex = i + 1;
        }
    }

    return 1;
}

static int get_sendtest_delay(int index)
{
    int delay;

    delay = (int)((uint64_t)g_send_ctrl[index].buffer_len *
        ((BYTE2BIT * (float)SENDTEST_MBIT) /g_send_ctrl[index].bandwidth));
    if ((delay < 0) || (delay > SENDTEST_MBIT)) {
        delay = SENDTEST_MBIT;
    }
    return delay;
}

static unsigned int cmd_sendtest_send(int argc, const char **argv, td_u8 index)
{
    unsigned int j = 0;
    unsigned int len = 0;

    if (g_send_ctrl[index].buffer_len == 0) {
        g_send_ctrl[index].buffer_len = send_get_buffer_len(index);
    }

    if (g_send_ctrl[index].bandwidth == 0) {
        g_send_ctrl[index].bandwidth = 1;
    }
    g_send_ctrl[index].bandwidth = g_send_ctrl[index].bandwidth * SENDTEST_512K;
    g_send_ctrl[index].cfg.delay = get_sendtest_delay(index);

    if (g_send_ctrl[index].cfg.interval == 0) {
        sendtest_printf("Error:[%d]\r\n", index);
        return 0;
    }

    g_send_ctrl[index].buffer = (td_u8 *)malloc(g_send_ctrl[index].buffer_len);
    if (!g_send_ctrl[index].buffer) {
        sendtest_printf("not enough mem\r\n");
        return 0;
    }

    if (memset_s(g_send_ctrl[index].buffer, g_send_ctrl[index].buffer_len, 0, g_send_ctrl[index].buffer_len) != EOK) {
        free(g_send_ctrl[index].buffer);
        g_send_ctrl[index].buffer = NULL;
        return 0;
    }

    if (send_wol_flag == 0x1) {
        if (g_send_ctrl[index].buffer_len != WOL_LEN) {
            sendtest_printf("magic frame len Error, -l should be 102 !\n");
            free(g_send_ctrl[index].buffer);
            g_send_ctrl[index].buffer = NULL;
            return 0;
        }
        for (len = MAC_LEN, j = 0; j < STR_LEN; j++) {
            memcpy_s(g_send_ctrl[index].buffer + len, MAC_LEN, send_wol_mac, MAC_LEN);
            len += MAC_LEN;
        }
    } else if (send_wol_flag == 0x2) {
        str2nhex(argv[gindex], g_send_ctrl[index].buffer, g_send_ctrl[index].buffer_len);
    } else {
        for (len = 0; len < g_send_ctrl[index].buffer_len; len++) {
            *(g_send_ctrl[index].buffer + len) = j;
            j++;
            if (j > 250) { /* 循环250次 */
                j = 0;
            }
        }
    }

    return 1;
}

unsigned int cmd_sendtest(int argc, const char *argv[])
{
    td_u8 index = 0;
    td_bool is_contain_a = TD_FALSE;
    gindex = 0;

    if (argc < 1) {
        sendtest_printf("param error\r\n");
        return 0;
    }

    if (send_check_parameter(argc, argv) != 0) {
        return 0;
    }

    for (int i = 0; i < argc; i++) {
        sendtest_printf("argv[%d]: %s\r\n", i, argv[i]);
    }

    for (int j = 0; j < argc; j++) {
        if ((!strcmp(argv[j], "-a")) && send_overflow8(j, argc, argv, &index)) {
            return 0;
        }

        if (cmd_sendtest_stop(argc, argv, j) != 1) {
            return 0;
        }
    }

    for (int j = 0; j < argc; j++) {
        if (strcmp(argv[j], "-a") == 0 || strcmp(argv[j], "-k") == 0) {
            is_contain_a = TD_TRUE;
            break;
        }
    }

    if (is_contain_a != TD_TRUE) {
        return 0;
    }

    if (index >= SOCKETS_MAX_NUM) {
        sendtest_printf("option -a is invalid, valid range form 0 to %d\r\n", SOCKETS_MAX_NUM - 1);
        return 0;
    }

    if (g_send_is_running[index]) {
        sendtest_printf("[%d]::send task is running, change option -a\r\n", index);
        return 0;
    }

    send_init();
    g_current_task_index = index;
    sendtest_printf("sizeof(send_ctrl_t) = %d,\r\n", sizeof(send_ctrl_t));
    memset_s(&g_send_ctrl[index], sizeof(send_ctrl_t), 0, sizeof(send_ctrl_t));
    g_send_ctrl[index].cfg.flag = 0x4;
    g_send_ctrl[index].cfg.sport = SENDTEST_DEFAULT_PORT;
    g_send_ctrl[index].cfg.dport = SENDTEST_DEFAULT_PORT;
    g_send_ctrl[index].cfg.interval = SENDTEST_DEFAULT_INTERVAL;
    g_send_ctrl[index].cfg.time = SENDTEST_DEFAULT_TIME;
    g_send_ctrl[index].finish = false;

    if (cmd_sendtest_cyc(argc, argv, index) != 1) {
        return 0;
    }

    if (cmd_sendtest_send(argc, argv, index) != 1) {
        return 0;
    }

    g_send_is_running[index] = true;
    g_send_ctrl[index].finish = false;
    g_send_timeout_rx.tv_sec = SENDTEST_SOCKET_RX_TIMEOUT;
    g_send_timeout_tx.tv_sec = SENDTEST_SOCKET_TX_TIMEOUT;
    printf_send(index);
    if (send_start() != ESP_OK) {
        return 0;
    }
    g_current_task_index = 0;
    sendtest_printf("send task creat end!\r\n");

    return 1;
}
