/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx password st
 * This file should be changed only infrequently and with great care.
 */

#ifndef DIAG_CMD_PASSWORD_ST_H
#define DIAG_CMD_PASSWORD_ST_H

#include <stdint.h>

#define DIAG_CONN_VERTIFY_SIZE 384
#define DIAG_PWD_MAX_LEN 32
#define DIAG_CONN_PUB_DATA_SIZE 384
#define EXT_DIAG_CONN_ERR_OK 0              /* Connection verification succeeded */
#define EXT_DIAG_CONN_ERR_SYS_CALL 1        /* Connection verification system error */
#define EXT_DIAG_CONN_ERR_PASS_WORD_WRONG 2 /* Connection verification password error */
#define EXT_DIAG_CONN_ERR_ACCOUNT_LOCK 3    /* Connection verification account lock */
#define EXT_DIAG_CONN_ERR_BUSY 6            /* Connection needs to preempt */

#define EXT_CHANGE_PWD_ERR_OK 0                  /* password has been updated */
#define EXT_CHANGE_PWD_ERR_SYS_CALL 1            /* System error */
#define EXT_CHANGE_PWD_ERR_NOT_SUPPORT 2         /* Not support change password */
#define EXT_CHANGE_PWD_ERR_PASSWORD_WRONG 3      /* The original password is incorrect */
#define EXT_CHANGE_PWD_ERR_ACCOUNT_LOCK 4        /* Account Lock */
#define EXT_CHANGE_PWD_ERR_PASSWORD_FORMAT_ERR 5 /* The password format is incorrect */

typedef struct {
    uint16_t err_no;        /* Error code (as above) */
    uint16_t res_try_cnt;   /* Remaining retries (units) */
    uint16_t res_lock_time; /* Remaining lock duration (unit s) */
    uint16_t pad;           /* Reserved */
} diag_cmd_conn_auth_ind_stru_t;

typedef struct {
    uint8_t pub_a[DIAG_CONN_PUB_DATA_SIZE];
} diag_cmd_conn_req_pub_a_stru_t;

/* diag connect rsp pub_b */
#define DIAG_CONN_SALT_SIZE 20
typedef struct {
    uint8_t pub_b[DIAG_CONN_PUB_DATA_SIZE];
    uint8_t salt[DIAG_CONN_SALT_SIZE];
    uint32_t salt_len;
    diag_cmd_conn_auth_ind_stru_t ret_ind; /* In order to be compatible with the old version, put it at the end */
} diag_cmd_conn_rsp_pub_b_stru_t;

/* diag connect req m */
#define DIAG_CONN_KEY_HASH_SIZE 32
typedef struct {
    uint8_t req_m1[DIAG_CONN_KEY_HASH_SIZE];
} diag_cmd_conn_req_m_stru_t;

/* diag connect rsp m */
typedef struct {
    uint32_t ret; /* ERRCODE_SUCC;ERRCODE_FAIL. */
    uint8_t rsp_m2[DIAG_CONN_KEY_HASH_SIZE];
    diag_cmd_conn_auth_ind_stru_t ret_ind; /* In order to be compatible with the old version, put it at the end */
} diag_cmd_conn_rsp_m_stru_t;

typedef struct {
    uint8_t conn_vertify[384]; /* 128 byte */
    uint8_t conn_salt[20];     /* 20 count */
    uint32_t salt_len;
} ext_nv_secure_conn_t;

typedef struct {
    uint8_t old_pwd[DIAG_PWD_MAX_LEN];
    uint8_t new_pwd[DIAG_PWD_MAX_LEN];
    uint8_t new_salt[DIAG_CONN_SALT_SIZE];
    uint8_t old_pwd_len;
    uint8_t new_pwd_len;
    uint8_t new_salt_len;
    uint8_t pad;
} diag_cmd_change_pwd_req_stru_t;

typedef diag_cmd_conn_auth_ind_stru_t diag_cmd_change_pwd_ind_stru_t;
#endif