/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library flash updating module interface
 */

#ifndef NV_UPDATE_H_
#define NV_UPDATE_H_

#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
#include "nv_task_adapt.h"
#else
#include "flash_task_types.h"
#endif
#include "nv_store.h"
#include "errcode.h"
#include "nv_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* Unused states and events should be removed */
typedef enum {
    STATE_INVALID,

    /* erase_all_keys, erase_old_keys, copy_all_keys */
    STATE_SELECT_FIRST_PAGE,
    STATE_SELECT_NEXT_PAGE,
    STATE_FIND_FIRST_KEY,
    STATE_FIND_NEXT_KEY,
    STATE_FIND_EXISTING_KEY,
    STATE_PREP_COPY_KEY,
    STATE_PREP_DELETE_KEY,
    STATE_PREP_MODIFY_KEY,
    STATE_UPDATE_MAP_FOR_OLD,

    /* prep_defrag_page, defrag_page */
    STATE_FIND_DEFRAG,
    STATE_ERASE_DEFRAG,
    STATE_PREP_DEFRAG,
    STATE_COPY_ALL_KEYS,
    STATE_PREP_PAGE_HEADER,
    STATE_WRITE_PAGE_HEADER,
    STATE_UPDATE_NVREGION_MAP,

    /* prepare_write */
    STATE_FIND_WRITE_POS,
    STATE_DEFRAG_PAGE,

    /* write_key */
    STATE_PREP_STORE,
    STATE_PREP_KEY_DATA,
    STATE_CLAIM_CRYPTO,
    STATE_ERASE_OLD_KEYS,

    STATE_PRIME_WRITE,
    STATE_PERFORM_WRITE,
    STATE_UPDATE_MAP_FOR_NEW,

    STATE_SUSPENDED = 0x1E,
    STATE_EXIT      = 0x1F
} kv_update_state_t;

typedef enum {
    EVENT_NONE,
    EVENT_SUSPEND,

    EVENT_WRITE_DATA_EXHAUSTED,
    EVENT_WRITE_BUFFER_PRIMED,
    EVENT_WRITE_BUFFER_STORED,

    /* erase_all_keys, erase_old_keys, copy_all_keys, modify_key */
    EVENT_PAGE_SELECTED,
    EVENT_PAGE_NOT_SELECTED,
    EVENT_KEY_FOUND,
    EVENT_KEY_NOT_FOUND,
    EVENT_COPY_KEY_READY,

    /* erase_all_keys */
    EVENT_KEYS_ERASED,

    /* modify_key */
    EVENT_KEY_UPDATE_REQUIRED,
    EVENT_KEY_UPDATE_NOT_NEEDED,

    /* prep_defrag_page */
    EVENT_DEFRAG_FOUND,
    EVENT_DEFRAG_ERASED,

    /* defrag_page */
    EVENT_DEFRAG_PREPARED,
    EVENT_ALL_KEYS_COPIED,
    EVENT_PAGE_HEADER_READY,
    EVENT_NVREGION_MAP_UPDATED,

    /* prepare_write */
    EVENT_DEFRAG_REQUIRED,
    EVENT_DEFRAG_COMPLETE,
    EVENT_WRITE_POS_FOUND,

    /* write_key */
    EVENT_STORE_READY,
    EVENT_KEY_DATA_READY,
    EVENT_CRYPTO_CLAIMED,
    EVENT_WRITE_COMPLETE,
    EVENT_PAGE_MAP_UPDATED,

    EVENT_ERROR = 0x1F
} kv_update_event_t;

/* State machine action function */
typedef kv_update_event_t (*kv_state_action)(void);

/* Data to be written to flash can be scattered across a chain of one or more source buffers */
typedef struct managed_source_buffer {
    struct managed_source_buffer *next;      /* Pointer to next source buffer in chain */
    uint8_t *data;                           /* Pointer to data in RAM/Flash */
    uint16_t data_length;                    /* Length of the data */
    uint16_t data_consumed;                  /* How much of the data has been read so far */
    bool release_data : 1;              /* Data pointer is a temporary buffer that needs freeing */
    bool receive_hash : 1;              /* Data buffer should receive a hash/CRC calculation */
    bool hash_data : 1;                 /* Indicates that the data needs hashing */
    bool crc_data : 1;                  /* Indicates that the data needs CRC calculation */
    bool encrypt_data : 1;              /* Indicates that the data need encrypting */
    bool gcm_tag_data : 1;              /* Indicates that the data needs AES GCM tag */
} kv_managed_source_buffer_t;

/* Write buffer, used to gather chunks of data from one or more source buffers */
typedef struct {
    uint8_t *data;                /* RAM buffer used to hold chunks of data to be written to flash */
    uint32_t write_location;      /* Current location in flash to write to */
    uint32_t resume_location;     /* Write location reached when state machine was suspended */
    uint32_t crypto_handle;       /* AES crypo handle */
    uint16_t size;                /* Size of the RAM buffer */
    uint16_t data_length;         /* Length of data written to RAM buffer */
    uint32_t crc_ret;             /* crc_ret counter */
    uint16_t data_consumed;       /* How much of the data has been written to flash */
    bool hash_claimed : 1;        /* Hash function (SHA256) has been claimed */
    bool encrypt_claimed : 1;     /* Encryption function (AES) has been claimed */
    bool crc_claimed : 1;         /* CRC function has been claimed */
    bool gcm_tag_claimed : 1;     /* AES GCM tag has been claimed */
} kv_managed_write_buffer_t;

/* State machine state transition table entry */
typedef struct {
    kv_update_state_t state : 5;       /* Current state                           */
    kv_update_state_t next_state : 5;  /* State after the transition has occurred */
    kv_update_event_t event : 5;       /* Event required to cause the transition (raised by an action function) */
} kv_update_transition_t;

/* State machine state action table entry */
typedef struct {
    kv_update_state_t state;   /* Applicable state             */
    kv_state_action action;    /* Action function to be called */
} kv_update_action_t;

/* Structure used to describe a state machine */
typedef struct {
    const kv_update_transition_t *transition_table;  /* State Transition table for this state machine */
    const kv_update_action_t *action_table;  /* State Action table for this state machine */
    uint16_t write_buffer_size;        /* Size of the write buffer needed for this state machine */
    kv_update_state_t initial_state;   /* State to set on initial entry to this state machine */
    kv_update_state_t resume_state;    /* State to set on resuming this state machine */
    kv_update_event_t exit_event;      /* Event to send to previous state machine, if no exit action function defined */
} kv_state_machine_t;

/* Used to hold run-time details of a state machine being processed */
/* Individual state machines will often invoke a sub state machine.
 * These data structures will be used to form a linked list of currently active state machines, although only one
 * state machine is ever being processed at any one time.
 * The tail of the list will be the initial state machine, started by one of the four kv_update_xxx() functions at the
 * end of this file.
 * The head of the list will be the current state machine being actively processed.
 * The 'prev' field provides a path back to the initial state machine
 */
typedef struct kv_active_state_machine_t {
    struct kv_active_state_machine_t *prev;      /* Previous state machine. Will be NULL if this is a root machine */
    const kv_state_machine_t *machine;           /* State machine being processed */
    kv_managed_write_buffer_t *write_buffer;     /* Holds chunk of data to be written to flash */
    kv_managed_source_buffer_t *source_buffer;   /* Linked list of source data buffers used to populate write buffer */
    kv_managed_source_buffer_t *current_source;  /* Current source buffer being used */
    kv_update_state_t state;                     /* Current state machine state */
    kv_update_event_t event;                     /* Last event raised (will have caused transition to current state) */
    errcode_t error_code;                       /* Records the error code produced when raising an error event */
} kv_active_state_machine_t;

errcode_t kv_update_init(cores_t core);
errcode_t kv_update_erase_key(kv_store_t core, flash_task_node *sanitised_tasks);
errcode_t kv_update_write_key(kv_store_t core, flash_task_node *sanitised_task);
errcode_t kv_update_modify_attribute(kv_store_t core, flash_task_node *sanitised_task);
errcode_t kv_update_backup_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
