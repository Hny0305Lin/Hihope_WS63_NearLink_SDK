/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Common Boot for Standard Application Core
 *
 * Create: 2023-01-09
 */

#ifndef BOOT_SERIAL_H
#define BOOT_SERIAL_H

#include "boot_init.h"
#include "uart.h"

extern uart_bus_t g_hiburn_uart;

/**
 * @ingroup hct_boot_api
 * 串口参数结构体
 */
typedef struct {
    uint32_t baudrate;          /* 波特率，比如:115200 */
    uint8_t databit;         /* 5; 6; 7; 8 */
    uint8_t stopbit;         /* 1:stop_bit 1;  2: stop_bit 2. */
    uint8_t parity;          /* 0: None; 1:Odd; 2:Even */
    uint8_t flow_ctrl;       /* 0: None; 1:rts&&cts. 2:only rts; 3:only cts */
    uint8_t fifoline_tx_int; /* 0:tx FIFO≤1/8full; 1:tx FIFO≤1/4full; 2:tx FIFO≤1/2full;
                                 3:tx FIFO≤3/4full; 4:tx FIFO≤7/8full */
    uint8_t fifoline_rx_int; /* 0:rx FIFO≥1/8full; 1:rx FIFO≥1/4full; 2:rx FIFO≥1/2full;
                                 3:rx FIFO≥3/4full; 4:rx FIFO≥7/8full */
    uint8_t fifoline_rts;    /* 0:rx FIFO≥1/8full; 1:rx FIFO≥1/4full; 2:rx FIFO≥1/2full;
                                 3:rx FIFO≥3/4full; 4:rx FIFO≥7/8full */
    uint8_t pad;
} uart_param_stru;

#define DEFAULT_FIFOLINE_TX_INT 3
#define DEFAULT_FIFOLINE_RX_INT 1
#define DEFAULT_FIFOLINE_RTS    4

/**
 * @ingroup boot_error_status
 * 打印错误码类型
 */
typedef enum {
    ERRCODE_UART_INIT = 0x3600,              /* UART认证初始化失败 */
    ERRCODE_LOADER_ERR = 0x3601,             /* 加载loaderboot失败 */
    ERRCODE_BOOT_DECRYPT_ERR = 0x3602,       /* boot解密失败 */
    ERRCODE_BOOT_LEN_ERR = 0x3603,           /* boot长度检验失败 */
    ERRCODE_BOOT_HEAD_ERR = 0x3604,          /* boot头校验失败 */
    ERRCODE_BOOT_SECURE_BOOT_ERR = 0x3605,   /* SECURE BOOT读取失败 */
    ERRCODE_BOOT_ROOT_KEY_LEN_ERR = 0x3606,  /* rootkye长度不正确 */
    ERRCODE_BOOT_ROOT_KEY_ERR = 0x3607,      /* rootkey证失败 */
    ERRCODE_BOOT_READ_ENCPT_ERR = 0x3608,    /* SECURE FLAG读取失败 */
    ERRCODE_BOOT_SUB_KEY_LEN_ERR = 0x3609,   /* subkey长度不正确 */
    ERRCODE_BOOT_SUB_KEY_CAT_ERR = 0x360A,   /* subkey Category不正确 */
    ERRCODE_BOOT_SUB_KEY_RSIM_ERR = 0x360B,  /* subkey RSIM校验失败 */
    ERRCODE_BOOT_SUB_KEY_ERR = 0x360C,       /* subkey认证失败 */
    ERRCODE_BOOT_CODE_VERIFY_ERR = 0x360D,   /* code校验失败 */
    ERRCODE_BOOT_DIE_ID_ERR = 0x360E,        /* DIE ID校验失败 */
    ERRCODE_BOOT_VER_ERR = 0x360F,           /* flashboot版本号校验失败 */
    ERRCODE_BOOT_GET_TAIL_ERR = 0x3610,      /* 获取flash尾地址失败 */
    ERRCODE_BOOT_BACKUP_TAIL_ERR = 0x3611,   /* 备份flashboot魔术字和长度校验失败 */
    ERRCODE_BOOT_MAIN_CHECK_ERR = 0x3612,    /* 非安全启动主flashboot完整性校验失败 */
    ERRCODE_BOOT_BACKUP_CHECK_ERR = 0x3613,  /* 非安全启动备份flashboot完整性校验失败 */
    ERRCODE_MALLOC_EXAUST_ERR = 0x3614,      /* 堆内存不足 */
    ERRCODE_MALLOC_STRUCT_ERR = 0x3615,      /* 堆内存管理结构体被破坏 */
    ERRCODE_NO_ENCPT_CNT_LEFT_ERR = 0x3616,  /* flash加密烧写次数已用完 */
    ERRCODE_CRYPTO_PREPARE_ERR = 0x3617,     /* flash加解密准备失败 */
    ERRCODE_CRYPTO_KEY_EMPTY_ERR = 0x3618,   /* flash加解密工作密钥为空 */
    ERRCODE_CRYPTO_KEY_INVALID_ERR = 0x3619, /* flash加解密工作密钥无效 */
    ERRCODE_CRYPTO_KEY_SAVE_ERR = 0x361A,    /* flash加解密工作密钥保存失败 */
    ERRCODE_CRYPTO_KEY_DECRYPT_ERR = 0x361B, /* flash加解密工作密钥解密失败 */
    ERRCODE_CRYPTO_FW_DECRYPT_ERR = 0x361C,  /* flash加解密固件加密失败 */
    ERRCODE_CRYPTO_FW_ENCRYPT_ERR = 0x361D,  /* flash加解密固件解密失败 */
    ERRCODE_CRYPTO_NV_ENCRYPT_ERR = 0x361E,  /* flash加解密NV区加密失败 */
    ERRCODE_BOOT_MAIN_HASH_ERR = 0x361F,     /* 非安全启动主flashboot hash校验失败 */
    ERRCODE_BOOT_BACKUP_HASH_ERR = 0x3620,   /* 非安全启动备份flashboot hash校验失败 */
    ERRCODE_BOOT_LOAD_HASH_ERR = 0x3621,     /* 非安全启动loaderboot hash校验失败 */
    ERRCODE_BOOT_FLASH_ERASE_ERR = 0x3622,   /* flash 擦除失败 */
    ERRCODE_BOOT_FLASH_RESUME_ERR = 0x3623,  /* flash 备份恢复 */
    ERRCODE_IDX_MAX,                         /* 错误码类型最大值 */
} hi_print_errno;

void hiburn_uart_init(uart_param_stru uart_param, uart_bus_t uart_bus);
void hiburn_uart_deinit(void);
#ifndef ATE_CLOSE_DEBUG
void boot_put_errno(uint32_t print_errno);
void boot_puthex(uint32_t h, bool print0);
void boot_msg0(const char *s);
void boot_msg1(const char *s, uint32_t h);
void boot_msg2(const char *s, uint32_t h1, uint32_t h2);
void boot_msg4(const char *s, uint32_t h1, uint32_t h2, uint32_t h3, uint32_t h4);
#else
#define boot_put_errno(print_errno)
#define boot_puthex(h, rint0)
#define boot_msg0(s)
#define boot_msg1(s, h)
#define boot_msg2(s, h1, h2)
#define boot_msg4(s, h1, h2, h3, h4)
#endif
void serial_set_mute(void);
void serial_cancel_mute(void);
void serial_putc(const char c);
void serial_puts(const char *s);
void serial_put_buf(const char *buffer, int32_t length);
uint8_t serial_getc(void);
int32_t serial_tstc(void);
void serial_puthex(uint32_t h, bool print_all);
uint32_t serial_getc_timeout(uint32_t timeout_us, uint8_t *ch);

#endif
