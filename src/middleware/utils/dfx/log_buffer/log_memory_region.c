/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  LOG MEMORY REGION SETUP MODULE
 * Author:
 * Create:  2018-10-15
 */

#include "stdbool.h"
#include "chip_core_definition.h"
#include "product.h"
#if (CORE == MASTER_BY_ALL) || (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES)
#include "securec.h"
#endif
#include "soc_osal.h"
#include "std_def.h"
#include "log_memory_definitions.h"
#include "common_def.h"
#include "log_memory_region.h"

/*
  *  Configuration parameters
  */
#define CONTOL_BLOCK_LOGGING_LENGTH sizeof(log_memory_region_control_t)

// Offsets from the start of the memory region
#if CORE_NUMS == 1
#define APPLICATION_CORE_LOGGING_OFFSET ((CONTOL_BLOCK_LOGGING_LENGTH))
#elif CHIP_WS53
#define CONTROL_CORE_LOGGING_OFFSET     (CONTOL_BLOCK_LOGGING_LENGTH)
#define APPLICATION_CORE_LOGGING_OFFSET ((CONTROL_CORE_LOGGING_OFFSET) + (CONTROL_LOGGING_LENGTH))
#else
#define BT_CORE_LOGGING_OFFSET          (CONTOL_BLOCK_LOGGING_LENGTH)
#define APPLICATION_CORE_LOGGING_OFFSET ((BT_CORE_LOGGING_OFFSET) + (BT_LOGGING_LENGTH))
// Fail compilation if they are not word aligned
cassert((BT_CORE_LOGGING_OFFSET & 0x3) == 0, LOG_MEMORY_REGION_C_);
cassert((APPLICATION_CORE_LOGGING_OFFSET & 0x3) == 0, LOG_MEMORY_REGION_C_);
#endif
#if CORE_NUMS > 2
#define HIFI_CORE_LOGGING_OFFSET        ((APPLICATION_CORE_LOGGING_OFFSET) + (APP_LOGGING_LENGTH))
#endif
#if defined(CHIP_LIBRA) && (CHIP_LIBRA != 0)
#define GNSS_CORE_LOGGING_OFFSET        ((HIFI_CORE_LOGGING_OFFSET) + (DSP_LOGGING_LENGTH))
#define SEC_CORE_LOGGING_OFFSET        ((GNSS_CORE_LOGGING_OFFSET) + (GNSS_LOGGING_LENGTH))
#endif
#define CONTOL_BLOCK_MASS_LENGTH        sizeof(massdata_memory_region_control_t)

// Offsets from the start of the memory region
// no dsp mass data for now
#define BT_CORE_MASS_OFFSET             (MASSDATA_REGION_START + CONTOL_BLOCK_MASS_LENGTH)
#define APPLICATION_CORE_MASS_OFFSET    (BT_CORE_MASS_OFFSET + BT_MASSDATA_LENGTH)

static bool g_log_inited = false;

/*
 *  Private function definitions
 */
static void logger_security_initialize_control_block(void)
{
    // copy control block header
    log_memory_region_control_t *initial_ctrl = (log_memory_region_control_t *)(uintptr_t)LOGGING_REGION_START;
#if CORE_NUMS == 1
    initial_ctrl->offset[LOG_MEMORY_REGION_SECTION_0] = APPLICATION_CORE_LOGGING_OFFSET;
#else
#if CHIP_WS53
    initial_ctrl->offset[LOG_MEMORY_REGION_SECTION_0] = CONTROL_CORE_LOGGING_OFFSET;
#else
    initial_ctrl->offset[LOG_MEMORY_REGION_SECTION_0] = (uint32_t)BT_CORE_LOGGING_OFFSET;
#endif
    initial_ctrl->offset[LOG_MEMORY_REGION_SECTION_1] = (uint32_t)APPLICATION_CORE_LOGGING_OFFSET;
#endif
#if CORE_NUMS > 2
    initial_ctrl->offset[LOG_MEMORY_REGION_SECTION_2] = (uint32_t)HIFI_CORE_LOGGING_OFFSET;
#endif
#if defined(CHIP_LIBRA) && (CHIP_LIBRA != 0)
    initial_ctrl->offset[LOG_MEMORY_REGION_SECTION_3] = GNSS_CORE_LOGGING_OFFSET;
    initial_ctrl->offset[LOG_MEMORY_REGION_SECTION_4] = SEC_CORE_LOGGING_OFFSET;
#endif
    initial_ctrl->length = LOGGING_REGION_LENGTH;
}

/*
  *  Public function definitions
  */
// For some builds lint thinks it would be a good idea to move the prototype for this to be in this file
/* Initialise the logger_security module.
 * Sets the shared memory ready for logging */
void log_memory_region_init(void)
{
    uint32_t irq = osal_irq_lock();
    if (unlikely(g_log_inited)) {
        osal_irq_restore(irq);
        return;
    }
#if CORE == MASTER_BY_ALL
    memset_s((void *)(uintptr_t)LOGGING_REGION_START, LOGGING_REGION_LENGTH, 0, LOGGING_REGION_LENGTH);
#endif
    logger_security_initialize_control_block();
    g_log_inited = true;
    osal_irq_restore(irq);
}

#if (USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == YES)
void massdata_memory_region_init(void)
{
    memset_s((void *)(uintptr_t)MASSDATA_REGION_START, MASSDATA_REGION_LENGTH, 0, MASSDATA_REGION_LENGTH);
    massdata_memory_region_control_t *mass_ctrl = (massdata_memory_region_control_t *)(uintptr_t)MASSDATA_REGION_START;

    mass_ctrl->region_num = MASS_MEMORY_REGION_MAX_NUMBER;
    mass_ctrl->mem_len = MASSDATA_REGION_LENGTH;
    mass_ctrl->section_control[MASS_MEMORY_REGION_SECTION_0].region_start = BT_CORE_MASS_OFFSET;
    mass_ctrl->section_control[MASS_MEMORY_REGION_SECTION_0].region_len = BT_MASSDATA_LENGTH;

    mass_ctrl->section_control[MASS_MEMORY_REGION_SECTION_1].region_start = APPLICATION_CORE_MASS_OFFSET;
    mass_ctrl->section_control[MASS_MEMORY_REGION_SECTION_1].region_len = APP_MASSDATA_LENGTH -
                                                                          CONTOL_BLOCK_MASS_LENGTH;

    return;
}
#endif

bool log_memory_is_init(void)
{
    return g_log_inited;
}