/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
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
#include "at_utils.h"

#include <td_base.h>
#include <securec.h>
#include <string.h>
#include <soc_osal.h>
#include <stdarg.h>

#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_AT_SSID_MAX_LEN 32
#define EXT_AT_CUSTOM_PKT_MAX_LEN 2800

td_u32 at_param_null_check(td_s32 argc, TD_CONST td_char **argv)
{
    for (td_s32 i = 0; i < argc; i++) {
        if (argv[i] == TD_NULL) {
            return EXT_ERR_FAILURE;
        }
    }
    return EXT_ERR_SUCCESS;
}

td_u32 integer_check(TD_CONST td_char *val)
{
    td_u32 len;
    td_u32 i;
    td_char *buf = (td_char *)val;
    if (buf == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    len = strlen(buf);
    if ((*buf == '0') && (len != 1)) {
        return EXT_ERR_FAILURE;
    }
    for (i = 0; i < len; i++) {
        if ((*buf < '0') || (*buf > '9')) {
            return EXT_ERR_FAILURE;
        }
        buf++;
    }
    return EXT_ERR_SUCCESS;
}

td_u32 alnum_check(TD_CONST td_char *val)
{
    td_u32 i;
    td_u32 len;
    td_char *buf = (td_char *)val;
    if (buf == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    len = strlen(buf);
    if ((*buf == '0') && (len != 1)) {
        return EXT_ERR_FAILURE;
    }
    for (i = 0; i < len; i++) {
        if (((*buf >= '0') && (*buf <= '9')) || ((*buf >= 'a') && (*buf <= 'f')) ||
            ((*buf >= 'A') && (*buf <= 'F'))) {
            buf++;
        } else {
            return EXT_ERR_FAILURE;
        }
    }
    return EXT_ERR_SUCCESS;
}

TD_PRV td_u32 at_check_mac_elem(TD_CONST td_char elem)
{
    if (elem >= '0' && elem <= '9') {
        return EXT_ERR_SUCCESS;
    } else if (elem >= 'A' && elem <= 'F') {
        return EXT_ERR_SUCCESS;
    } else if (elem >= 'a' && elem <= 'f') {
        return EXT_ERR_SUCCESS;
    } else if (elem == ':') {
        return EXT_ERR_SUCCESS;
    }

    return EXT_ERR_FAILURE;
}

td_u32 cmd_strtoaddr(TD_CONST td_char *param, td_uchar *mac_addr, td_u32 addr_len)
{
    td_u32 cnt;
    td_char *tmp1 = (td_char *)param;
    td_char *tmp2 = TD_NULL;
    td_char *tmp3 = TD_NULL;

    for (cnt = 0; cnt < 17; cnt++) {    /* 17 */
        if (at_check_mac_elem(param[cnt]) != EXT_ERR_SUCCESS) {
            return EXT_ERR_FAILURE;
        }
    }

    for (cnt = 0; cnt < (addr_len - 1); cnt++) {
        tmp2 = (char*)strsep(&tmp1, ":");
        if (tmp2 == TD_NULL) {
            return EXT_ERR_AT_INVALID_PARAMETER;
        }
        mac_addr[cnt] = (td_uchar)strtoul(tmp2, &tmp3, 16); /* 16 */
    }

    if (tmp1 == TD_NULL) {
        return EXT_ERR_AT_INVALID_PARAMETER;
    }
    mac_addr[cnt] = (td_uchar)strtoul(tmp1, &tmp3, 16); /* 16 */
    return EXT_ERR_SUCCESS;
}

TD_PRV char *at_strrchr(TD_CONST char *src, int c)
{
    return strrchr(src, c);
}

TD_PRV td_u32 at_strlen(TD_CONST char *src)
{
    return strlen(src);
}

TD_PRV char *at_dup_binstr(TD_CONST char *src, td_u32 len)
{
    char *res = TD_NULL;
    int ret;

    if (src == TD_NULL) {
        return TD_NULL;
    }
    res = osal_kmalloc((td_ulong)(len + 1), OSAL_GFP_KERNEL);
    if (res == TD_NULL) {
        return TD_NULL;
    }
    ret = memcpy_s(res, len, src, len);
    if (ret != EOK) {
        osal_kfree(res);
        res = TD_NULL;
        return TD_NULL;
    }
    res[len] = '\0';

    return res;
}

TD_PRV int at_hex2num(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;    /* add 10 */
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;    /* add 10 */
    }
    return -1;
}

TD_PRV int at_hex2byte(TD_CONST char *hex)
{
    int a, b;
    TD_CONST char *tmp_hex = hex;
    a = at_hex2num(*tmp_hex++);
    if (a < 0) {
        return -1;
    }
    b = at_hex2num(*tmp_hex++);
    if (b < 0) {
        return -1;
    }
    return ((td_u32)a << 4) | (td_u32)b;    /* 4 */
}

/**
 * at_hexstr2bin - Convert ASCII hex string into binary data
 * @hex: ASCII hex string (e.g., "01ab")
 * @buf: Buffer for the binary data
 * @len: Length of the text to convert in bytes (of buf); hex will be double
 * this size
 * Returns: 0 on success, -1 on failure (invalid hex string)
 */
TD_PRV int at_hexstr2bin(TD_CONST char *hex, td_u8 *buf, td_u32 len)
{
    td_u32 i;
    TD_CONST char *ipos = hex;
    td_u8 *opos = buf;

    for (i = 0; i < len; i++) {
        int a = at_hex2byte(ipos);
        if (a < 0) {
            return -1;
        }
        *opos++ = a;
        ipos += 2;  /* add 2 */
    }
    return 0;
}

TD_PRV td_void at_printf_decode_slash(td_u8 *buf, TD_CONST char **str_pos, td_u32 *str_len)
{
    TD_CONST char *pos = *str_pos;
    td_u32 len = *str_len;
    int val;

    pos++;
    switch (*pos) {
        case '\\':
        case '"':
            buf[len++] = *pos;
            pos++;
            break;
        case 'n':
            buf[len++] = '\n';
            pos++;
            break;
        case 'r':
            buf[len++] = '\r';
            pos++;
            break;
        case 't':
            buf[len++] = '\t';
            pos++;
            break;
        case 'e':
            buf[len++] = '\033';
            pos++;
            break;
        case 'x':
            pos++;
            val = at_hex2byte(pos);
            if (val < 0) {
                val = at_hex2num(*pos);
                if (val < 0) {
                    break;
                }
                buf[len++] = val;
                pos++;
            } else {
                buf[len++] = val;
                pos += 2;   /* add 2 */
            }
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            val = *pos++ - '0';
            if (*pos >= '0' && *pos <= '7') {
                val = val * 8 + (*pos++ - '0'); /* 8 */
            }
            if (*pos >= '0' && *pos <= '7') {
                val = val * 8 + (*pos++ - '0'); /* 8 */
            }
            buf[len++] = val;
            break;
        default:
            break;
    }

    *str_pos = pos;
    *str_len = len;
}

TD_PRV td_u32 at_printf_decode(td_u8 *buf, td_u32 maxlen, TD_CONST char *str)
{
    TD_CONST char *pos = str;
    td_u32 len = 0;

    while (*pos) {
        if (len + 1 >= maxlen) {
            break;
        }
        switch (*pos) {
            case '\\':
                at_printf_decode_slash(buf, &pos, &len);
                break;
            default:
                buf[len++] = *pos++;
                break;
        }
    }
    if (maxlen > len) {
        buf[len] = '\0';
    }

    return len;
}

TD_PRV char *at_parse_string_normal(TD_CONST char *value, td_u32 *len)
{
    TD_CONST char *pos = TD_NULL;
    TD_CONST char *src = value;
    char *str = TD_NULL;

    src++;
    pos = at_strrchr(src, '"');
    if (pos == TD_NULL || pos[1] != '\0') {
        return TD_NULL;
    }
    *len = pos - src;
    str = at_dup_binstr(src, *len);
    if (str == TD_NULL) {
        return TD_NULL;
    }
    return str;
}

TD_PRV char *at_parse_string_with_p(TD_CONST char *value, td_u32 *len)
{
    TD_CONST char *pos = TD_NULL;
    TD_CONST char *src = value;
    char *tstr = TD_NULL;
    char *str = TD_NULL;
    td_u32 tlen;

    src += 2;     /* add 2 */
    pos = at_strrchr(src, '"');
    if (pos == TD_NULL || pos[1] != '\0') {
        return TD_NULL;
    }
    tlen = pos - src;
    tstr = at_dup_binstr(src, tlen);
    if (tstr == TD_NULL) {
        return TD_NULL;
    }

    str = osal_kmalloc((td_ulong)(tlen + 1), OSAL_GFP_KERNEL);
    if (str == TD_NULL) {
        osal_kfree(tstr);
        return TD_NULL;
    }

    *len = at_printf_decode((td_u8 *)str, tlen + 1, tstr);
    osal_kfree(tstr);
    return str;
}

TD_PRV char *at_parse_string_other(TD_CONST char *value, td_u32 *len)
{
    td_u8 *str = TD_NULL;
    td_u32 tlen;
    td_u32 hlen = at_strlen(value);
    if (hlen & 1) {
        return TD_NULL;
    }
    tlen = hlen / 2;    /* 2 */
    str = osal_kmalloc((td_ulong)(tlen + 1), OSAL_GFP_KERNEL);
    if (str == TD_NULL) {
        return TD_NULL;
    }
    if (at_hexstr2bin(value, str, tlen)) {
        osal_kfree((char*)str);
        return TD_NULL;
    }
    str[tlen] = '\0';
    *len = tlen;
    return (char *)str;
}

char *at_parse_string(TD_CONST char *value, td_u32 *len)
{
    if (len == TD_NULL) {
        return TD_NULL;
    }
    if (*value == '"') {
        return at_parse_string_normal(value, len);
    } else if (*value == 'P' && value[1] == '"') {
        return at_parse_string_with_p(value, len);
    } else {
        return at_parse_string_other(value, len);
    }
}

td_s32 convert_bin_to_dec(td_s32 pbin)
{
    td_s32 result = 0;

    while (pbin)
    {
      result += pbin % 2;
      pbin /= 2;
    }

    return result;
}

/*****************************************************************************
 函 数 名  : at_str_to_hex
 功能描述  : 字符串转16进制
*****************************************************************************/
td_void at_str_to_hex(TD_CONST td_char *param, td_u32 len, unsigned char *value)
{
    td_u16 index;
    if (param == TD_NULL || len > EXT_AT_CUSTOM_PKT_MAX_LEN) {
        return;
    }
    for (index = 0; index < len; index++) {
        /* 每2位字符转为一个十六进制，第一位乘以16 */
        value[index / 2] = (unsigned char)(value[index / 2] * 16 * (index % 2) + at_hex2num(*param));
        param++;
    }
}

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif
