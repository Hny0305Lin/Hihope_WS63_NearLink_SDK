/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: FLASH task shared definitions between the SCPU and A/PCPU.
 */

#ifndef FLASH_TASK_ETYPES_H
#define FLASH_TASK_ETYPES_H

#include "stdbool.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/** Identifier to check that a section of memory is indeed a flash_task_node */
#define FLASH_TASK_NODE_IDENTIFIER        0xd3a71f85

/* Normal values of the flash_task_node.result field; anything else is an error being reported - must remain in sync
 * with the SCPU soc_errno.  The A/PCPU can use these defines in case they have a different set of error codes */
#define FLASH_TASK_DISCARD                0x80002040
#define FLASH_TASK_DISCARDED              0x80002041
#define FLASH_TASK_READY                  0x80002042
#define FLASH_TASK_BEING_PROCESSED        0x80002043
#define FLASH_TASK_COMPLETED              0x80003130

#define FLASH_TASK_ERASE_ALL_MAGIC_VALUE_LEN    10

typedef enum {
    /* Create KV storage in flash memory */
    FLASH_TASK_KV_PAGE_INIT = 0,
    /* A new or modification to an existing key-value */
    FLASH_TASK_KV_DATA,
    /* Modify the attribute on an existing key-value */
    FLASH_TASK_KV_ATTRIBUTE,
    /* Erase a key-value */
    FLASH_TASK_KV_ERASE,
    /* Write / overwrite some FOTA patch data in FLASH */
    FLASH_TASK_FOTA_DATA,
    /* Erase some FOTA patch data in FLASH and re-write the FOTA header */
    FLASH_TASK_FOTA_ERASE,
    /* Write some data to ACPU storage in FLASH */
    FLASH_TASK_ACPU_DATA,
    /* Erase some ACPU storage in FLASH */
    FLASH_TASK_ACPU_ERASE,
    /* Number of task types */
    FLASH_TASK_NUM_TYPES,
} flash_task_type_t;


/** @brief  Key type */
typedef uint16_t kv_storage_key;

typedef struct {
    uint32_t        kv_addr;
    uint32_t        kv_size;
} flash_task_kv_init_t;

typedef struct {
    const uint8_t   *kvalue;
    kv_storage_key   key;
    uint16_t           kvalue_length;
    uint8_t            attribute;
    bool          force_write;
} flash_task_kv_data_t;

typedef struct {
    kv_storage_key  key;
    uint8_t         attribute;
} flash_task_kv_attribute_t;

typedef struct {
    kv_storage_key  key;
} flash_task_kv_erase_t;

typedef struct {
    kv_storage_key  key;
} flash_task_kv_backup_t;

typedef struct {
    /* Used to ensure erase all is not called by mistake. */
    uint8_t   magic_value[FLASH_TASK_ERASE_ALL_MAGIC_VALUE_LEN];
} kv_erase_all_t;

typedef struct {
    uint8_t   *src; /* Mapped address in calling core memory map, can in RAM or FLASH. */
    uint8_t   *dst; /* Mapped address in calling core memory map FOTA FLASH region
                     * - must not include the FOTA header. */
    uint16_t  length;
} flash_task_flash_data_t;

typedef struct {
    uint32_t  start_address; /* Core mapped address in FOTA FLASH region; must be at the start of a flash page. */
    uint32_t  length;        /* Must be a multiple of the page size. */
} flash_task_flash_erase_t;

typedef union {
    flash_task_kv_init_t      kv_region;
    flash_task_kv_data_t      kv;
    flash_task_kv_attribute_t kv_attribute;
    flash_task_kv_erase_t     kv_erase;
    flash_task_flash_data_t   flash;
    flash_task_flash_erase_t  flash_erase;
} task_node_data_t;

typedef struct owner_callback_data_t owner_callback;

/**
 * @brief  Flash task local callback from the flash_task_process_completed_task(...) function.
 *
 * @param result    is the result of the flash task execution on the SCPU.
 * @param callback  is the callback struct containing any information that was prepared for use during the callback.
 * @return          soc_errno value SOC_ERR_SUCCESS or another error code indicating that the callback has failed; this
 *                  will be returned by the flash_task_process_completed_task(...) function. */
typedef errcode_t (*flash_task_completed_callback)(errcode_t result, owner_callback *callback);

struct owner_callback_data_t {
    /** Callback function to call if not EXT_NULL. */
    flash_task_completed_callback  func;
    /* Include any owner data to be available to the callback function upon execution. */
};

typedef struct flash_task_description_t flash_task_node;
struct flash_task_description_t {
    /** A flash_task identifier to maximise the chance that a bug is detected quickly. */
    uint32_t                               identifier;
    /** Next task in the queue or EXT_NULL if nothing. */
    volatile flash_task_node              *next;
    /** FOR THE TASK PRODUCER: This is private to the core that owns the cache and will not be examined by the SCPU task
     * handlers.  It will nominally be used by the owning core as the call-back it should call when it discovers that
     * this task has been completed combined with a pointer to a struct with any state information.
     *
     * NB: IN THE SCPU FLASH TASK HANDLER DURING EXECUTION ONLY: this will be nulled out in the copy initially provided
     * to the SCPU task handler and can be used to store task state while the task is in progress, even if the task
     * must be temporarily suspended due to an A/PCPU wake-up. */
    owner_callback                         *owner_callback;
    /** The state_code is a soc_errno and is used as a lock so is volatile so that it can be read/written from each core
     * properly.
     *   FLASH_TASK_DISCARD         - Set by the owning core so that the SCPU will stop checking this memory
     *                                      (and will acknowledge this by setting this to DISCARDED).
     *   FLASH_TASK_DISCARDED       - Set by the SCPU to acknowledge that this task has not been processed and
     *                                      was discarded by request of the owning core.
     *                                      (the SCPU is no longer examining this memory which can be modified/freed).
     *   FLASH_TASK_READY           - Set by the owning core so that the SCPU will process this task
     *                                      (and will update it when finished) so the owning core cannot change
     *                                      anything other than the owner_callback once this has been set.
     *   FLASH_TASK_BEING_PROCESSED - Set by the SCPU to indicate that this task is being processed;
     *                                      this may take some time if the SCPU has to wait for other cores to sleep.
     *                                      The owning core must not change anything other than the owner_callback.
     *   FLASH_TASK_COMPLETED       - Set by the SCPU to indicate that this task completed successfully
     *                                      (the SCPU is no longer examining this memory which can be modified/freed).
     *   any other value            - Provides an error code; contact customer support to obtain it's meaning
     *                                      (the SCPU is no longer examining this memory which can be modified/freed).
     */
    volatile uint32_t                      state_code;
    /** The flash task to be performed. */
    flash_task_type_t                        task;
    /** The unique per task data contents. */
    task_node_data_t                         data;
    /** A checksum formed by xoring the flash write task information (task and data) and is used to spot errors. */
    uint32_t                               checksum;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _EXTERNAL_ETYPES_5GNBRBIV100_SHARED_FLASH_TASK_ETYPES_H */

