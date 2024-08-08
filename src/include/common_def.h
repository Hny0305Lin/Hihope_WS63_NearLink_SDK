/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Common define \n
 *
 */

#ifndef COMMON_DEF_H
#define COMMON_DEF_H
#ifndef __KERNEL__
#include <stdint.h>
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#ifndef NULL
#define NULL 0L
#endif

#ifndef array_size
#define array_size(x)   (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef min
#define min(x, y)       (((x) < (y)) ? (x) : (y))
#endif

#ifndef max
#define max(x, y)       (((x) > (y)) ? (x) : (y))
#endif

#ifdef __GNUC__
#ifndef likely
#define likely(x)       __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x)     __builtin_expect(!!(x), 0)
#endif
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

#ifndef unused
#define unused(var)     (void)(var)
#endif


#ifndef STATIC
#if (defined(CONFIG_ROM_COMPILE) || defined(UT_TEST))
#define STATIC
#else
#define STATIC static
#endif
#endif

#ifndef INLINE
#if (defined(CONFIG_ROM_COMPILE) || defined(UT_TEST))
#define INLINE  static inline
#else
#define INLINE  inline
#endif
#endif


#ifndef bit
#define bit(x)              (1UL << (uint32_t)(x))
#endif


#define makeu16(a, b)       ((uint16_t)(((uint8_t)(a)) | ((uint16_t)((uint8_t)(b))) << 8))
#define makeu32(a, b)       ((uint32_t)(((uint16_t)(a)) | ((uint32_t)((uint16_t)(b))) << 16))
#define makeu64(a, b)       ((uint64_t)(((uint32_t)(a)) | ((uint64_t)((uint32_t)(b))) << 32))
#define joinu32(a, b, c, d) makeu32(makeu16((a), (b)), makeu16((c), (d)))

#define hiu32(x)            ((uint32_t)(((uint64_t)(x) >> 32) & 0xFFFFFFFF))
#define lou32(x)            ((uint32_t)(x))

#define hiu16(x)            ((uint16_t)(((uint32_t)(x) >> 16) & 0xFFFF))
#define lou16(x)            ((uint16_t)(x))

#define hiu8(x)             ((uint8_t)(((uint16_t)(x) >> 8) & 0xFF))
#define lou8(x)             ((uint8_t)(x))

#define uapi_max(a, b)      (((a) > (b)) ? (a) : (b))
#define uapi_min(a, b)      (((a) < (b)) ? (a) : (b))

#define uapi_reg_write(addr, val)              (*(volatile unsigned int *)(uintptr_t)(addr) = (val))
#define uapi_reg_read(addr, val)               ((val) = *(volatile unsigned int *)(uintptr_t)(addr))
#define uapi_reg_write32(addr, val)            (*(volatile unsigned int *)(uintptr_t)(addr) = (val))
#define uapi_reg_read32(addr, val)             ((val) = *(volatile unsigned int *)(uintptr_t)(addr))
#define uapi_reg_read_val32(addr)              (*(volatile unsigned int*)(uintptr_t)(addr))
#define uapi_reg_setbitmsk(addr, msk)          ((uapi_reg_read_val32(addr)) |= (msk))
#define uapi_reg_clrbitmsk(addr, msk)          ((uapi_reg_read_val32(addr)) &= ~(msk))
#define uapi_reg_clrbit(addr, pos)             ((uapi_reg_read_val32(addr)) &= ~((unsigned int)(1) << (pos)))
#define uapi_reg_setbit(addr, pos)             ((uapi_reg_read_val32(addr)) |= ((unsigned int)(1) << (pos)))
#define uapi_reg_clrbits(addr, pos, bits) (uapi_reg_read_val32(addr) &= ~((((unsigned int)1 << (bits)) - 1) << (pos)))
#define uapi_reg_setbits(addr, pos, bits, val) (uapi_reg_read_val32(addr) =           \
    (uapi_reg_read_val32(addr) & (~((((unsigned int)1 << (bits)) - 1) << (pos)))) | \
    ((unsigned int)((val) & (((unsigned int)1 << (bits)) - 1)) << (pos)))
#define uapi_reg_getbits(addr, pos, bits) ((uapi_reg_read_val32(addr) >> (pos)) & (((unsigned int)1 << (bits)) - 1))

#define uapi_reg_write16(addr, val)        (*(volatile unsigned short *)(uintptr_t)(addr) = (val))
#define uapi_reg_read16(addr, val)         ((val) = *(volatile unsigned short *)(uintptr_t)(addr))
#define uapi_reg_read_val16(addr)          (*(volatile unsigned short*)(uintptr_t)(addr))
#define uapi_reg_clrbit16(addr, pos)       ((uapi_reg_read_val16(addr)) &= ~((unsigned short)(1) << (pos)))
#define uapi_reg_setbit16(addr, pos)       ((uapi_reg_read_val16(addr)) |= ((unsigned short)(1) << (pos)))
#define uapi_reg_clrbits16(addr, pos, bits) (uapi_reg_read_val16(addr) &= \
    ~((((unsigned short)1 << (bits)) - 1) << (pos)))
#define uapi_reg_setbits16(addr, pos, bits, val) (uapi_reg_read_val16(addr) =           \
    (uapi_reg_read_val16(addr) & (~((((unsigned short)1 << (bits)) - 1) << (pos)))) | \
    ((unsigned short)((val) & (((unsigned short)1 << (bits)) - 1)) << (pos)))
#define uapi_reg_getbits16(addr, pos, bits) ((uapi_reg_read_val16(addr) >> (pos)) & (((unsigned short)1 << (bits)) - 1))

#define uapi_reg_write8(addr, val)        (*(volatile unsigned char *)(uintptr_t)(addr) = (val))
#define uapi_reg_read8(addr, val)         ((val) = *(volatile unsigned char *)(uintptr_t)(addr))
#define uapi_reg_read_val8(addr)          (*(volatile unsigned char*)(uintptr_t)(addr))
#define uapi_reg_clrbit8(addr, pos)       ((uapi_reg_read_val8(addr)) &= ~((unsigned char)(1) << (pos)))
#define uapi_reg_setbit8(addr, pos)       ((uapi_reg_read_val8(addr)) |= ((unsigned char)(1) << (pos)))
#define uapi_reg_clrbits8(addr, pos, bits) (uapi_reg_read_val8(addr) &= ~((((unsigned char)1 << (bits)) - 1) << (pos)))
#define uapi_reg_setbits8(addr, pos, bits, val) (uapi_reg_read_val8(addr) =           \
    (uapi_reg_read_val8(addr) & (~((((unsigned char)1 << (bits)) - 1) << (pos)))) | \
    ((unsigned char)((val) & (((unsigned char)1 << (bits)) - 1)) << (pos)))
#define uapi_reg_getbits8(addr, pos, bits) ((uapi_reg_read_val8(addr) >> (pos)) & (((unsigned char)1 << (bits)) - 1))

/**
 * @if Eng
 * @brief   The offset of member in the structrue.
 * @else
 * @brief   结构体成员偏移
 * @endif
 */
#define fieldoffset(s, m)   ((uint32_t) &(((s *)0)->m))


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
