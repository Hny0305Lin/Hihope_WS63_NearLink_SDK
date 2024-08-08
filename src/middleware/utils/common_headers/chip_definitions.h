/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  Basic chip definitions
 * Author:
 * Create:  2018-10-15
 */
#ifndef LIB_COMMON_HEADERS_CHIP_DEFINITIONS_H
#define LIB_COMMON_HEADERS_CHIP_DEFINITIONS_H

/** @defgroup CHIP_Base CHIP Base Definitions
 * CHIP Base Definitions
 * @ingroup CHIP_Base
 * */
/** @defgroup CHIP_ARMCORE CHIP ARM Core Specifics
 * CHIP ARM Core Application Core Specifics
 * @ingroup CHIP_Base
 * @{
 * */
#define BT       0
#define PROTOCOL 1
#define APPS     2
#define GNSS     3
#define SECURITY 4
#define HIFI     PROTOCOL
#define WIFI     5
#define CONTROL_CORE 6
#define SENSOR   7

#define CM3      0
#define CM7      1
#define RISCV31  2
#define RISCV70  3
#define RISCV32  4

#define CHIP_LIBRA           (TARGET_CHIP_LIBRA)
#define CHIP_SOCMN1          (TARGET_CHIP_SOCMN1)
#define CHIP_BS25            (TARGET_CHIP_BS25)
#define CHIP_BRANDY          (TARGET_CHIP_BRANDY)
#define CHIP_SW39            (TARGET_CHIP_SW39)
#define CHIP_SW21            (TARGET_CHIP_SW21)

#if (CHIP_LIBRA == 1)
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 5 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip
#elif (CHIP_SOCMN1 == 1)
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 4 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip
#elif (CHIP_BS25 == 1)
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 2 // Total cores in chip
#define CPU_NUM  2    // Total cpus in chip
#elif (CHIP_BS20 == 1 || CHIP_BS21  == 1 || CHIP_BS21A  == 1 || \
       CHIP_BS21E == 1 || CHIP_BS22 == 1 || CHIP_BS26 == 1)
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 1 // Total cores in chip
#define CPU_NUM  1    // Total cpus in chip
#elif (CHIP_WS53 == 1)
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 2 // Total cores in chip
#define CPU_NUM  2    // Total cpus in chip
#elif (CHIP_WS63 == 1)
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 1 // Total cores in chip
#define CPU_NUM  1    // Total cpus in chip
#elif (CHIP_BRANDY == 1)
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 3 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip
#elif (CHIP_SW39 == 1)
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 6 // Total cores in chip
#define CPU_NUM  3    // Total cpus in chip
#elif (CHIP_CAT1 == 1)
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 2 // Total cores in chip
#define CPU_NUM  2    // Total cpus in chip
#elif (CHIP_SW21 == 1)
#define MASTER_BY_ALL   APPS
#define CORE_NUMS 5 // Total cores in chip
#define CPU_NUM  5    // Total cpus in chip
#else
#error Please define master control core
#endif

#define OTHER_CPU_NUM (CPU_NUM - 1) // Other cpus number, exclude current cpu.

#define MASTER_BY_LIBRA_ONLY        (CHIP_LIBRA && (CORE == APPS))
#define MASTER_BY_SOCMN1_ONLY       (CHIP_SOCMN1 && (CORE == APPS))
#define MASTER_BY_BS25_ONLY         (CHIP_BS25 && (CORE == APPS))
#define MASTER_BY_BS20_ONLY         (CHIP_BS20 && (CORE == APPS))
#define MASTER_BY_BS21_ONLY         (CHIP_BS21 && (CORE == APPS))
#define MASTER_BY_BS21E_ONLY        (CHIP_BS21E && (CORE == APPS))
#define MASTER_BY_BS21A_ONLY        (CHIP_BS21A && (CORE == APPS))
#define MASTER_BY_BS22_ONLY         (CHIP_BS22 && (CORE == APPS))
#define MASTER_BY_BS26_ONLY         (CHIP_BS26 && (CORE == APPS))
#define MASTER_BY_BRANDY_ONLY       (CHIP_BRANDY && (CORE == APPS))
#define MASTER_BY_WS53_ONLY         (CHIP_WS53 && (CORE == APPS))
#define MASTER_BY_SW39_ONLY         (CHIP_SW39 && (CORE == APPS))
#define MASTER_BY_SW21_ONLY         (CHIP_SW21 && (CORE == APPS))

#define MASTER_ONLY                 (MASTER_BY_LIBRA_ONLY || \
                                     MASTER_BY_SOCMN1_ONLY || MASTER_BY_BS25_ONLY || \
                                     MASTER_BY_BRANDY_ONLY || MASTER_BY_BS20_ONLY || \
                                     MASTER_BY_BS21_ONLY || MASTER_BY_BS21A_ONLY || \
                                     MASTER_BY_BS22_ONLY || MASTER_BY_BS26_ONLY || \
                                     MASTER_BY_WS53_ONLY || MASTER_BY_SW39_ONLY || \
                                     MASTER_BY_SW21_ONLY || MASTER_BY_BS21E_ONLY)

#define MCU_ONLY                    (MASTER_BY_LIBRA_ONLY || MASTER_BY_SOCMN1_ONLY || \
                                     MASTER_BY_BS25_ONLY || MASTER_BY_BRANDY_ONLY || \
                                     MASTER_BY_WS53_ONLY || CHIP_WS63 || CHIP_BS20 || CHIP_BS21 || CHIP_BS21E || \
                                     CHIP_BS21A || CHIP_BS22 || CHIP_BS26 || MASTER_BY_SW39_ONLY || \
                                     MASTER_BY_SW21_ONLY)


#define SLAVE_BY_LIBRA_BT          (CHIP_LIBRA && (CORE == BT))
#define SLAVE_BY_LIBRA_GNSS        (CHIP_LIBRA && (CORE == GNSS))
#define SLAVE_BY_LIBRA_SECURITY    (CHIP_LIBRA && (CORE == SECURITY))
#define SLAVE_BY_LIBRA_ONLY        (SLAVE_BY_LIBRA_BT || SLAVE_BY_LIBRA_GNSS || SLAVE_BY_LIBRA_SECURITY)
#define SLAVE_BY_SOCMN1_ONLY       (CHIP_SOCMN1 && (CORE == BT))
#define SLAVE_BY_SW39_BT           (CHIP_SW39 && (CORE == BT))
#define SLAVE_BY_BS25_ONLY         (CHIP_BS25 && (CORE == BT))
#define SLAVE_BY_BRANDY_BT         (CHIP_BRANDY && (CORE == BT))
#define SLAVE_BY_BRANDY_DSP        (CHIP_BRANDY && (CORE == DSP))
#define SLAVE_BY_BRANDY_ONLY       (SLAVE_BY_BRANDY_BT || SLAVE_BY_BRANDY_DSP)
#define SLAVE_BY_WS53_ONLY         (CHIP_WS53 && (CORE == CONTROL_CORE))

#define CHIP_LIBRA_FPGA            (CHIP_LIBRA && (LIBRA_CHIP_FPGA))
#define CHIP_LIBRA_CS              (CHIP_LIBRA && (LIBRA_CHIP_CS))

#define CHIP_SOCMN1_FPGA             (CHIP_SOCMN1 && (SOCMN1_CHIP_FPGA))
#define CHIP_SOCMN1_V100             (CHIP_SOCMN1 && (SOCMN1_CHIP_V100))
#define CHIP_SOCMN1_V200             (CHIP_SOCMN1 && (SOCMN1_CHIP_V200))

#define CHIP_BS25_FPGA              (CHIP_BS25 && (BS25_CHIP_FPGA))
#define CHIP_BS25_V100              (CHIP_BS25 && (BS25_CHIP_V100))

#ifdef PRE_FPGA
    #define FPGA 1
    #define ASIC 0
#endif

#ifdef PRE_ASIC
    #define FPGA 0
    #define ASIC 1
#endif

#define CHIP_BS20_FPGA          (CHIP_BS20 && (FPGA))
#define CHIP_BS20_V100          (CHIP_BS20 && (ASIC))

#define CHIP_BS21_FPGA          (CHIP_BS21 && (FPGA))
#define CHIP_BS21_V100          (CHIP_BS21 && (ASIC))

#define CHIP_BS21E_FPGA          (CHIP_BS21E && (FPGA))
#define CHIP_BS21E_V100          (CHIP_BS21E && (ASIC))

#define CHIP_BS21A_FPGA          (CHIP_BS21A && (FPGA))
#define CHIP_BS21A_V100          (CHIP_BS21A && (ASIC))

#define CHIP_BS22_FPGA          (CHIP_BS22 && (FPGA))
#define CHIP_BS22_V100          (CHIP_BS22 && (ASIC))

#define CHIP_BS26_FPGA          (CHIP_BS26 && (FPGA))
#define CHIP_BS26_V100          (CHIP_BS26 && (ASIC))

#define CHIP_WS63_FPGA          (CHIP_WS63 && (FPGA))
#define CHIP_WS63_V100          (CHIP_WS63 && (ASIC))

#define CHIP_BRANDY_FPGA       (CHIP_BRANDY && (BRANDY_CHIP_FPGA))
#define CHIP_BRANDY_V100       (CHIP_BRANDY && (BRANDY_CHIP_V100))

#define CHIP_SW39_FPGA         (CHIP_SW39 && (SW39_CHIP_FPGA))

#define CHIP_SW21_FPGA         (CHIP_SW21 && (SW21_CHIP_FPGA))
#define CHIP_SW21_V100         (CHIP_SW21 && (SW21_CHIP_V100))

#define CHIP_WS53_FPGA         (CHIP_WS53 && (FPGA))
#define CHIP_WS53_V100          (CHIP_WS53 && (ASIC))

#define CHIP_FPGA             (CHIP_LIBRA_FPGA || CHIP_SOCMN1_FPGA || CHIP_BS25_FPGA || CHIP_SW39_FPGA || \
                               CHIP_BRANDY_FPGA || CHIP_BS20_FPGA || CHIP_BS21_FPGA || CHIP_BS21A_FPGA || \
                               CHIP_BS22_FPGA || CHIP_BS26_FPGA || CHIP_WS63_FPGA || CHIP_WS53_FPGA || \
                               CHIP_SW21_FPGA || CHIP_BS21E_FPGA)

#define CHIP_SOCMN1_ASIC       (CHIP_SOCMN1_V100 || CHIP_SOCMN1_V200)
#define CHIP_BS25_ASIC        (CHIP_BS25_V100)
#define CHIP_BS20_ASIC        (CHIP_BS20_V100)
#define CHIP_BS21_ASIC        (CHIP_BS21_V100)
#define CHIP_BS21E_ASIC        (CHIP_BS21E_V100)
#define CHIP_BS21A_ASIC        (CHIP_BS21A_V100)
#define CHIP_BS22_ASIC        (CHIP_BS22_V100)
#define CHIP_BS26_ASIC        (CHIP_BS26_V100)
#define CHIP_BRANDY_ASIC      (CHIP_BRANDY_V100)
#define CHIP_WS63_ASIC        (CHIP_WS63_V100)
#define CHIP_WS53_ASIC        (CHIP_WS53_V100)
#define CHIP_SW21_ASIC        (CHIP_SW21_V100)

#define CHIP_ASIC             (CHIP_LIBRA_CS || CHIP_SOCMN1_ASIC || \
                               CHIP_BS25_ASIC || CHIP_BRANDY_ASIC || \
                               CHIP_BS20_ASIC || CHIP_BS21_ASIC || \
                               CHIP_BS21A_ASIC || CHIP_BS22_ASIC || \
                               CHIP_BS26_ASIC || CHIP_WS63_ASIC || \
                               CHIP_WS53_ASIC || CHIP_SW21_ASIC || \
                               CHIP_BS21E_ASIC)

/** @} end of group CHIP_ARMCORE */
#endif
