/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  connectivity register, memory io.
 * Author:
 * Create:  2018-10-15
 */

#ifndef CHIP_IO_H
#define CHIP_IO_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef WIN32
uint8_t readb(uint32_t addr);
#endif
#define ON            (true)
#define OFF           (false)
#define POS(x)        (x)

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef BIT
#define BIT(x) (1UL << (uint32_t)(x))
#endif

#ifndef UNUSED
#define UNUSED(var) do { (void)(var); } while (0)
#endif

/* Make true and false actual booleans, not ints, to avoid Lint moaning. GCC defines them as ints. */
#ifdef true
#undef true
#endif

#ifdef false
#undef false
#endif

#define true ((bool) 1)
#define false ((bool) 0)

/** PMU_LPM wakeup types */
typedef enum {
    TURN_OFF,
    TURN_ON
} switch_type_t;

/** reg32 bit pos types */
typedef enum {
    POS_16 = 16,
    POS_17,
    POS_18,
    POS_19,
    POS_20,
    POS_21,
    POS_22,
    POS_23,
    POS_24,
    POS_25,
    POS_26,
    POS_27,
    POS_28,
    POS_29,
    POS_30,
    POS_31
} REG32_POS;

/** reg32 bit pos types */
typedef enum {
    POS_0,
    POS_1,
    POS_2,
    POS_3,
    POS_4,
    POS_5,
    POS_6,
    POS_7,
    POS_8,
    POS_9,
    POS_10,
    POS_11,
    POS_12,
    POS_13,
    POS_14,
    POS_15
} REG16_POS;

#ifndef BIT_31
#define BIT_31 ((uint32_t)(1 << 31))
#define BIT_30 ((uint32_t)(1 << 30))
#define BIT_29 ((uint32_t)(1 << 29))
#define BIT_28 ((uint32_t)(1 << 28))
#define BIT_27 ((uint32_t)(1 << 27))
#define BIT_26 ((uint32_t)(1 << 26))
#define BIT_25 ((uint32_t)(1 << 25))
#define BIT_24 ((uint32_t)(1 << 24))
#define BIT_23 ((uint32_t)(1 << 23))
#define BIT_22 ((uint32_t)(1 << 22))
#define BIT_21 ((uint32_t)(1 << 21))
#define BIT_20 ((uint32_t)(1 << 20))
#define BIT_19 ((uint32_t)(1 << 19))
#define BIT_18 ((uint32_t)(1 << 18))
#define BIT_17 ((uint32_t)(1 << 17))
#define BIT_16 ((uint32_t)(1 << 16))
#define BIT_15 ((uint32_t)(1 << 15))
#define BIT_14 ((uint32_t)(1 << 14))
#define BIT_13 ((uint32_t)(1 << 13))
#define BIT_12 ((uint32_t)(1 << 12))
#define BIT_11 ((uint32_t)(1 << 11))
#define BIT_10 ((uint32_t)(1 << 10))
#define BIT_9  ((uint32_t)(1 << 9))
#define BIT_8  ((uint32_t)(1 << 8))
#define BIT_7  ((uint32_t)(1 << 7))
#define BIT_6  ((uint32_t)(1 << 6))
#define BIT_5  ((uint32_t)(1 << 5))
#define BIT_4  ((uint32_t)(1 << 4))
#define BIT_3  ((uint32_t)(1 << 3))
#define BIT_2  ((uint32_t)(1 << 2))
#define BIT_1  ((uint32_t)(1 << 1))
#define BIT_0  ((uint32_t)(1 << 0))
#endif

/*******************************内存操作宏定义 ****************************************/
/* liteos中有相同宏名定义，并且定义使用有差异，需要使用芯片定义宏，将其他已定义相同宏undef */
#ifdef writel
#undef writel
#endif

#ifdef writew
#undef writew
#endif

#ifdef writeb
#undef writeb
#endif

#ifndef UT_TEST
#define writel(addr, d)                             (*(volatile uint32_t*)(uintptr_t)(addr) = (uint32_t)(d))
#define writew(addr, d)                             (*(volatile uint16_t*)(uintptr_t)(addr) = (uint16_t)(d))
#else
#define writel(addr, d)
#define writew(addr, d)
#endif
#define writeb(addr, d)                             (*(volatile uint8_t*)(uintptr_t)(addr) = (uint8_t)(d))

#ifndef readl
#ifndef UT_TEST
#define readl(addr)                                 (*(volatile uint32_t*)(uintptr_t)(addr))
#else
#define readl(addr)                                 NULL
#endif
#endif

#ifndef readw
#define readw(addr)                                 (*(volatile uint16_t*)(uintptr_t)(addr))
#endif

#ifndef readb
#define readb(addr)                                 (*(volatile uint8_t*)(uintptr_t)(addr))
#endif

/******************************寄存器操作宏定义 ***************************************/
/********************分为base + offset 和直接给出地址两类*****************************/
#define reg(base, offset)                           (*(volatile uint32_t *)(uintptr_t)((uint32_t)(base) + (offset)))
#define regb(base, offset)                          (*(volatile uint8_t *)(uintptr_t)((uint32_t)(base) + (offset)))
#define regw(base, offset)                          (*(volatile uint16_t *)(uintptr_t)((uint32_t)(base) + (offset)))

#define reg32(register)                             (*(volatile uint32_t*)(uintptr_t)(register))
#define reg16(register)                             (*(volatile uint16_t*)(uintptr_t)(register))
#define reg8(register)                              (*(volatile uint8_t*)(uintptr_t)(register))

/**************************base+register offset*********************************************************/
#define reg_clrbit(base, offset, pos)               ((reg(base, offset)) &= ~((uint32_t)(1) << (pos)))
#define regw_clrbit(base, offset, pos)              ((regw(base, offset)) &= ~((uint16_t)(1) << (pos)))
#define regb_clrbit(base, offset, pos)              ((regb(base, offset)) &= ~((uint8_t)(1) << (pos)))

#define reg_setbit(base, offset, pos)               ((reg(base, offset)) |= ((uint32_t)(1) << (pos)))
#define regw_setbit(base, offset, pos)              ((regw(base, offset)) |= (uint16_t)((uint32_t)(1) << (pos)))
#define regb_setbit(base, offset, pos)              ((regb(base, offset)) |= ((uint8_t)(1) << (pos)))

/* 将var中[pos, pos + bits-1]比特清零,  pos从0开始编号 e.g BDOM_CLR_BITS(var, 4, 2) 表示将Bit5~4清零 */
#define reg_clrbits(base, offset, pos, bits)        (reg(base, offset) &= ~((((uint32_t)1 << (bits)) - 1) << (pos)))
#define regw_clrbits(base, offset, pos, bits)       (regw(base, offset) &= ~((((uint16_t)1 << (bits)) - 1) << (pos)))
#define regb_clrbits(base, offset, pos, bits)       (regb(base, offset) &= ~((((uint8_t)1 << (bits)) - 1) << (pos)))


/* 将var中[pos, pos + bits-1]比特设置为val,  pos从0开始编号
   e.g BDOM_SET_BITS(var, 4, 2, 2) 表示将Bit5~4设置为b'10 */
#define reg_setbits(b, r, pos, bits, val)  (reg(b, r) = (reg(b, r) & (~((((uint32_t)1 << (bits)) - 1) << (pos)))) \
                                                        | ((uint32_t)((val) & (((uint32_t)1 << (bits)) - 1)) << (pos)))
#define regw_setbits(b, r, pos, bits, val) (regw(b, r) = (regw(b, r) & (~((((uint16_t)1 << (bits)) - 1) << (pos)))) \
                                                        | ((uint16_t)((val) & (((uint16_t)1 << (bits)) - 1)) << (pos)))
#define regb_setbits(b, r, pos, bits, val) (regb(b, r) = (regb(b, r) & (~((((uint8_t)1 << (bits)) - 1) << (pos)))) \
                                                        | ((uint8_t)((val) & (((uint8_t)1 << (bits)) - 1)) << (pos)))
/* 获取var中[pos, pos + bits-1]比特值,  pos从0开始编号 */
#define reg_getbits(base, offset, pos, bits)         ((reg(base, offset) >> (pos)) & (((uint32_t)1 << (bits)) - 1))
#define regw_getbits(base, offset, pos, bits)        ((regw(base, offset) >> (pos)) & (((uint16_t)1 << (bits)) - 1))
#define regb_getbits(base, offset, pos, bits)        ((regb(base, offset) >> (pos)) & (((uint8_t) 1<< (bits)) - 1))

#define reg_clrbitmsk(base, offset, msk)             ((reg(base, offset)) &= ~(msk))
#define regw_clrbitmsk(base, offset, msk)            ((regw(base, offset)) &= ~(msk))
#define regb_clrbitmsk(base, offset, msk)            ((regb(base, offset)) &= ~(msk))

#define reg_setbitmsk(base, offset, msk)             ((reg(base, offset)) |= (msk))
#define regw_setbitmsk(base, offset, msk)            ((regw(base, offset)) |= (msk))
#define regb_setbitmsk(base, offset, msk)            ((regb(base, offset)) |= (msk))

#define reg_read(base, offset, result)               ((result) = reg(base, offset))
#define reg_readw(base, offset, result)              ((result) = regw(base, offset))
#define reg_readb(base, offset, result)              ((result) = regb(base, offset))

#define reg_write(base, offset, data)                (reg(base, offset) = (data))
#define reg_writew(base, offset, data)               (regw(base, offset) = (data))
#define reg_writeb(base, offset, data)               (regb(base, offset) = (data))

/**************************only register*************************************************************/
#define reg32_clrbit(register, pos)                    (reg32(register) &= ~((uint32_t)((1U) << (uint32_t)(pos))))
#ifndef UT_TEST
#define reg16_clrbit(register, pos)                    (reg16(register) &= ~((uint16_t)((1U) << (uint16_t)(pos))))
#else
#define reg16_clrbit(register, pos)
#endif
#define reg8_clrbit(register, pos)                     (reg8(register) &= ~((uint8_t)((1U) << (pos))))

#define reg32_setbit(register, pos)                    (reg32(register) |= ((uint32_t)((1U) << (uint32_t)(pos))))
#ifndef UT_TEST
#define reg16_setbit(register, pos)                    (reg16(register) |= ((uint16_t)((1U) << (uint16_t)(pos))))
#else
#define reg16_setbit(register, pos)
#endif
#define reg8_setbit(register, pos)                     (reg8(register) |= ((uint8_t)((1U) << (pos))))

/* 读取寄存器指定位的值,pos从0开始编号 */
#ifndef UT_TEST
#define reg32_getbit(register, pos)                    ((reg32(register) >> (pos)) & ((uint32_t)(1U)))
#define reg16_getbit(register, pos)                    ((reg16(register) >> (pos)) & ((uint16_t)(1U)))
#else
#define reg32_getbit(register, pos)                     NULL
#define reg16_getbit(register, pos)                     NULL
#endif
#define reg8_getbit(register, pos)                     ((reg8(register) >> (pos)) & ((uint8_t)(1U)))

/* 将var中[pos, pos + bits-1]比特清零,  pos从0开始编号 e.g BDOM_CLR_BITS(var, 4, 2) 表示将Bit5~4清零 */
#define reg32_clrbits(register, pos, bits)             (reg32(register) &= ~((((uint32_t)1 << (bits)) - 1) << (pos)))
#define reg16_clrbits(register, pos, bits)             (reg16(register) &= \
                                                        ~(uint16_t)((((uint32_t)1 << (bits)) - 1) << (pos)))
#define reg8_clrbits(register, pos, bits)              (reg8(register) &= \
                                                        ~(uint8_t)((((uint32_t)1 << (bits)) - 1) << (pos)))

/* 将var中[pos, pos + bits-1]比特设置为val,  pos从0开始编号 e.g BDOM_SET_BITS(var, 4, 2, 2) 表示将Bit5~4设置为b'10 */
#define reg32_setbits(r, p, b, v) (reg32(r) = (reg32(r) & \
                                              (~((((uint32_t)1 << (uint32_t)(b)) - 1) << (uint32_t)(p)))) \
                                              | ((uint32_t)((uint32_t)(v) & \
                                              (((uint32_t)1 << (uint32_t)(b)) - 1)) << (uint32_t)(p)))

#define reg16_setbits(r, p, b, v) (reg16(r) = (reg16(r) & \
                                              (uint16_t)(~((((uint32_t)1 << (uint16_t)(b)) - 1) << (uint16_t)(p)))) \
                                              | (uint16_t)((uint32_t)((uint16_t)(v) & \
                                              (((uint32_t)1 << (uint16_t)(b)) - 1)) << (uint16_t)(p)))

#define reg8_setbits(r, p, b, v)  (reg8(r) = (reg8(r) & (~((((uint8_t)1 << (b)) - 1) << (p)))) \
                                              | ((uint8_t)((v) & (((uint8_t)1 << (b)) - 1)) << (p)))

/* 获取var中[pos, pos + bits-1]比特值,  pos从0开始编号 */
#define reg32_getbits(register, pos, bits)           ((reg32(register) >> (pos)) & (((uint32_t)1 << (bits)) - 1))
#define reg16_getbits(register, pos, bits)           ((reg16(register) >> (pos)) & \
                                                      ((uint16_t)((uint32_t)1 << (bits)) - 1))
#define reg8_getbits(register, pos, bits)            ((reg8(register) >> (pos)) & (((uint8_t)1 << (bits)) - 1))

#define reg32_clrbitmsk(register, msk)               ((reg32(register)) &= ~(msk))
#define reg16_clrbitmsk(register, msk)               ((reg16(register)) &= ~(msk))
#define reg8_clrbitmsk(register, msk)                ((reg8(register)) &= ~(msk))

#define reg32_setbitmsk(register, msk)               ((reg32(register)) |= (msk))
#define reg16_setbitmsk(register, msk)               ((reg16(register)) |= (msk))
#define reg8_setbitmsk(register, msk)                ((reg8(register)) |= (msk))

#define reg32_read(register, result)                 ((result) = reg32(register))
#define reg16_read(register, result)                 ((result) = reg16(register))
#define reg8_read(register, result)                  ((result) = reg8(register))

#define reg32_write(register, data)                  (reg32(register) = (data))
#define reg16_write(register, data)                  (reg16(register) = (data))
#define reg8_write(register, data)                   (reg8(register) = (data))

#define p8(addr)                                    (*((volatile uint8_t*)(addr)))
#define p16(addr)                                   (*((volatile uint16_t*)(addr)))
#define p32(addr)                                   (*((volatile uint32_t*)(addr)))

#define point8_setbit(point, pos)                   (p8(point) |= (uint8_t)((1U) << (pos)))
#define point16_setbit(point, pos)                  (p16(point) |= (uint16_t)((1U) << (pos)))
#define point32_setbit(point, pos)                  (p32(point) |= (uint32_t)((1U) << (pos)))

#define point8_setbits(p, pos, b, v)  (p8(p) = (uint8_t)(p8(p) & (~((((uint8_t)1 << (b)) - 1) << (pos)))) \
                                               | ((uint8_t)((v) & (((uint8_t)1 << (b)) - 1)) << (pos)))

#define point16_setbits(p, pos, b, v) (p16(p) = (uint16_t)(p16(p) & \
                                                (~((((uint16_t)1 << (uint16_t)(b)) - 1) << (uint16_t)(pos)))) \
                                                | ((uint16_t)((uint16_t)(v) & \
                                                (((uint16_t)1 << (uint16_t)(b)) - 1)) << (uint16_t)(pos)))

#define point32_setbits(p, pos, b, v) (p32(p) = (uint32_t)(p32(p) & (~((((uint32_t)1 << (b)) - 1) << (pos)))) \
                                               | ((uint32_t)((v) & (((uint32_t)1 << (b)) - 1)) << (pos)))

#define point8_clrbit(point, pos)                  (p8(point) &= ~(uint8_t)((1U) << (pos)))
#define point16_clrbit(point, pos)                 (p16(point) &= ~(uint16_t)((1U) << (pos)))
#define point32_clrbit(point, pos)                 (p32(point) &= ~(uint32_t)((1U) << (pos)))

#define point8_clrbits(point, pos, bits)           (p8(point) &= ~((((uint8_t)1 << (bits)) - 1) << (pos)))
#define point16_clrbits(point, pos, bits)          (p16(point) &= ~((((uint16_t)1 << (bits)) - 1) << (pos)))
#define point32_clrbits(point, pos, bits)          (p32(point) &= ~((((uint32_t)1 << (bits)) - 1) << (pos)))

#ifdef __cplusplus
#if __cplusplus
        }
#endif
#endif
#endif
