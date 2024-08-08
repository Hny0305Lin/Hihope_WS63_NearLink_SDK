/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  log OML exception producer interface
 * Author:
 * Create:
 */

#ifndef LOG_OML_EXCEPTION_H
#define LOG_OML_EXCEPTION_H

#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
#define AULREG_VALUE_INDEX 32
#elif (ARCH == CM3) || (ARCH == CM7)
#define AULREG_VALUE_INDEX 16
#endif

#include "log_oam_logger.h"
#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
#include "vectors.h"
#elif (ARCH == CM7)
#include "exception.h"
#endif
typedef struct {
    om_msg_header_stru_t msg_header;
    uint32_t stack_limit;
    uint32_t fault_type;
    uint32_t fault_reason;
    uint32_t address;
    uint32_t reg_value[AULREG_VALUE_INDEX];
    uint32_t psp_value;
    uint32_t lr_value;
    uint32_t pc_value;
    uint32_t psps_value;
    uint32_t primask_value;
    uint32_t fault_mask_value;
    uint32_t bserpri_value;
    uint32_t control_value;
    uint8_t msg_tail;
} __attribute__((packed)) om_exception_info_stru_t;

enum OM_FAULT_TYPE_ENUM {
    OM_HARD_FAULT,
    OM_MEM_FAULT,
    OM_BUS_FAULT,
    OM_USAGE_FAULT,
    OM_LOOP_DEAD,
    OM_WDT_TIMEOUT,
    OM_INDEX_BUTT,
};

enum OM_RISCV_FAULT_TYPE_ENMU {
    OM_UNKNOW_FAULT,

    OM_INSTRUCTION_ADDRESS_MISALIGNED,
    OM_INSTRUCTION_ACCESS_FAULT,
    OM_ILLEGAL_INSTRUCTION,
    OM_BREAKPOINT,
    OM_LOAD_ADDERSS_MISALIGNED,
    OM_LOAD_ACCESS_FAULT,
    OM_STORE_OR_AMO_ADDRESS_MISALIGNED,
    OM_STORE_OR_AMO_ACCESS_FALUT,
    OM_ENVIRONMENT_CALL_FROM_UMODE,
    OM_ENVIRONMENT_CALL_FROM_SMODE,
    OM_RESERVED_0,
    OM_ENVIRONMENT_CALL_FROM_MMODE,
    OM_INSTRUCTION_PAGE_FAULT,
    OM_LOAD_PAGE_FAULT,
    OM_RESERVED_1,
    OM_STORE_OR_AMO_PAGE_FAULT,
#if (ARCH == RISCV31) || (ARCH == RISCV32)
    OM_RISCV_HARD_FAULT,
    OM_LOCK_UP,
#else
    OM_ASYNCHRONOUS_EXCEPTION,
#endif

    OM_NMI_INTERRUPT,
    OM_WDT_TIMEOUT_INTERRUPT,
    OM_APPLICATION_XIP_CTRL_INTERRUPT,
    OM_APPLICATION_XIP_CACHE_INTERRUPT,
    OM_APPLICATION_MDMA_INTERRUPT,
    OM_APPLICATION_SMDMA_INTERRUPT,
};

enum REG_NUM_INDEX {
    REG_NUM_0,
    REG_NUM_1,
    REG_NUM_2,
    REG_NUM_3,
    REG_NUM_4,
    REG_NUM_5,
    REG_NUM_6,
    REG_NUM_7,
    REG_NUM_8,
    REG_NUM_9,
    REG_NUM_10,
    REG_NUM_11,
    REG_NUM_12,
    REG_NUM_13,
    REG_NUM_14,
    REG_NUM_15,
    REG_NUM_16,
    REG_NUM_17,
    REG_NUM_18,
    REG_NUM_19,
    REG_NUM_20,
    REG_NUM_21,
    REG_NUM_22,
    REG_NUM_23,
    REG_NUM_24,
    REG_NUM_25,
    REG_NUM_26,
    REG_NUM_27,
    REG_NUM_28,
    REG_NUM_29,
    REG_NUM_30,
    REG_NUM_31,
};

enum PARA1_INDEX {
    PARA1_INDEX_0,
    PARA1_INDEX_1,
    PARA1_INDEX_2,
    PARA1_INDEX_3,
    PARA1_INDEX_4,
    PARA1_INDEX_5,
    PARA1_INDEX_6,
    PARA1_INDEX_7,
};

enum PARA2_INDEX {
    PARA2_INDEX_0,
    PARA2_INDEX_1,
    PARA2_INDEX_2,
    PARA2_INDEX_3,
    PARA2_INDEX_4,
    PARA2_INDEX_5,
    PARA2_INDEX_6,
    PARA2_INDEX_7,
};

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO)
#define DUMP_MAX_LENGTH_PER_TRANS (5000U - 15U)
#endif

/**
 * @brief  Dump the current task stack when in systemfaults and watchdog timeout.
 */
void log_oml_dump_stack(void);

/**
 * @brief  Dump the ram when hardfault or watchdog.
 * @param  addr Dump ram start addr.
 * @param  length Dump ram length.
 */
void log_exception_send_data(const uint8_t *data, uint16_t length);

/**
 * @brief  Dump the whole rom when hardfault or watchdog.
 */
void log_oml_memory_dump(void);

#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
/**
 * @brief  Invoked by exception to dump exception information.
 * @param  irq_id Interrupt ID.
 * @param  exc_buf_addr Exception buff addr.
 */
void log_exception_dump(uint32_t irq_id, exc_context_t *exc_buf_addr);
#else
/**
 * @brief  Invoked by exception to dump exception information.
 * @param  int_id Interrupt ID.
 * @param  reason Fault reason.
 * @param  addr Fault address.
 * @param  exc_info cpu exception info.
 */
void log_exception_dump(uint32_t int_id, uint32_t reason, uint32_t addr, exc_info_t *exc_info);
#endif

#if ((ARCH == RISCV31) || (ARCH == RISCV32) || (ARCH == RISCV70))
/**
 * @brief  SDT interface to send SDT format fault message.
 * @param  irq_id Interrupt ID.
 * @param  exc_buf_addr Exception buff addr.
 */
void log_oml_exception_info_send(uint32_t irq_id, const exc_context_t *exc_buf_addr);
#else
/**
 * @brief  SDT interface to send SDT format fault message.
 * @param  address Fault address.
 * @param  fault_type Fault type.
 * @param  fault_reason Fault reason.
 * @param  exc_info exception information.
 */
void log_oml_exception_info_send(uint32_t address, uint32_t fault_type, uint32_t fault_reason,
                                 const exc_info_t *exc_info);
#endif

/**
 * @brief  Register exception dump to fault interrupt.
 */
void default_register_hal_exception_dump_callback(void);

#endif
