/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: for osal adapt.
 *
 * Create: 2023-04-23
*/

#include "crypto_osal_lib.h"
#include "drv_trng.h"
#include "crypto_osal_adapt.h"
#include "td_base.h"
#include "boot_serial.h"

td_char const hex2ascii_data[] = "0123456789abcdefghijklmnopqrstuvwxyz";
#define hex2ascii(hex) (hex2ascii_data[hex])

#define crypto_fallthrough() __attribute__((fallthrough))

#define toupper(c) ((c) - 0x20 * (((c) >= 'a') && ((c) <= 'z')))

static td_char *ksprintn(td_char *nbuf, td_u32 num, td_s32 base, td_s32 *lenp, td_s32 upper)
{
    td_char *p, c;

    p = nbuf;
    *p = '\0';
    do {
        c = hex2ascii(num % base);
        *++p = upper ? toupper(c) : c;
    } while (num /= base);
    if (lenp != TD_NULL) {
        *lenp = p - nbuf;
    }

    return (p);
}

static td_s32 kvprintf(td_char const * fmt, td_void (*func)(td_char), va_list ap)
{
#define crypto_pchar(c) do {                         \
        td_char cc = (c);      \
        if (func != TD_NULL)             \
            (*func)(cc); \
        retval++;             \
} while (0)
    td_char nbuf[sizeof(td_s32) * 8 + 1];   // 8: 8 bits in one byte
    const td_char *p, *percent;
    td_s32 ch, n;
    td_u32 num;
    td_s32 base, tmp, width, neg, sign, dot;
    td_s32 dwidth, upper;
    td_char padc;
    td_s32 stop = 0;
    td_s32 retval = 0;

    num = 0;

    if (fmt == TD_NULL) {
        fmt = "(fmt null)n";
    }

    for (;;) {
        padc = ' ';
        width = 0;
        while ((ch = (td_uchar)*fmt++) != '%' || stop) {
            if (ch == '\0') {
                return (retval);
            }
            crypto_pchar(ch);
        }
        percent = fmt - 1;
        neg = 0;
        sign = 0;
        dot = 0;
        dwidth = 0;
        upper = 0;

    reswitch:
        switch (ch = (td_uchar)*fmt++) {
            case '%':
                crypto_pchar(ch);
                break;
            case '0':
                if (!dot) {
                    padc = '0';
                    goto reswitch;
                }
                crypto_fallthrough();
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                for (n = 0;; ++fmt) {
                    n = n * 10 + ch - '0';  /* 10: Decimal data conversion. */
                    ch = *fmt;
                    if (ch < '0' || ch > '9') {
                        break;
                    }
                }
                if (dot) {
                    dwidth = n;
                } else {
                    width = n;
                }
                goto reswitch;
            case 'l': // Must use l flag to use u32
                goto reswitch;
            case 'c':
                crypto_pchar(va_arg(ap, td_s32));
                break;
            case 'd':
            case 'i':
                base = 10;  /* 10: Decimal data. */
                sign = 1;
                goto handle_sign;
            case 'u':
                base = 10;  /* 10: Decimal data. */
                goto handle_nosign;
            case 's':
                p = va_arg(ap, td_char *);
                if (p == TD_NULL) {
                    p = "(null)";
                }
                if (!dot) {
                    n = strlen(p);
                } else {
                    for (n = 0; n < dwidth && p[n]; n++) {
                        continue;
                    }
                }
                width -= n;

                if (width > 0) {
                    while (width--) {
                        crypto_pchar(padc);
                    }
                }
                while (n--) {
                    crypto_pchar(*p++);
                }
                break;
            case 'X':
                upper = 1;
                crypto_fallthrough();
            case 'x':
                base = 16;  /* 16: Hexadecimal data. */
                goto handle_nosign;
            handle_nosign:
                sign = 0;
                num = va_arg(ap, td_u32);
                goto number;
            handle_sign:
                num = va_arg(ap, td_s32);
            number:
                if (sign && (td_s32)num < 0) {
                    neg = 1;
                    num = -(td_s32)num;
                }
                p = ksprintn(nbuf, num, base, &tmp, upper);
                if (neg) {
                    tmp++;
                }
                if (padc != '0' && width && (width -= tmp) > 0) {
                    while (width--) {
                        crypto_pchar(padc);
                    }
                }
                if (neg) {
                    crypto_pchar('-');
                }
                if (width && (width -= tmp) > 0) {
                    while (width--) {
                        crypto_pchar(padc);
                    }
                }
                while (*p) {
                    crypto_pchar(*p--);
                }
                break;
            default:
                while (percent < fmt) {
                    crypto_pchar(*percent++);
                }
                /*
                 * Since we ignore an formatting argument it is no
                 * longer safe to obey the remaining formatting
                 * arguments as the arguments will no longer match
                 * the format specs.
                 */
                stop = 1;
                break;
        }
    }
#undef crypto_pchar
    return 0;
}

static td_void debug_putc(const td_char c)
{
    (void)serial_putc(c);
}

td_void crypto_boot_debug_message(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    kvprintf(fmt, debug_putc, ap);
    va_end(ap);
}

void __attribute__((weak)) crypto_cache_flush(uintptr_t base_addr, td_u32 size)
{
    crypto_unused(base_addr);
    crypto_unused(size);
}

void __attribute__((weak)) crypto_cache_inv(uintptr_t base_addr, td_u32 size)
{
    crypto_unused(base_addr);
    crypto_unused(size);
}

void __attribute__((weak)) crypto_cache_all(void)
{
}

td_bool crypto_wait_func_is_support(void)
{
    return TD_FALSE;
}