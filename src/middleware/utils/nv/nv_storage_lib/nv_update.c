/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2022. All rights reserved.
 * Description: KV Storage Library flash updating module implementation
 */

#include "nv_update.h"
#include "nv_store.h"
#include "nv_page.h"
#include "nv_key.h"
#include "nv_porting.h"
#include "nv_config.h"
#include "nv_reset.h"
#include "nv_nvregion.h"
#if defined(CONFIG_PARTITION_FEATURE_SUPPORT)
#include "partition.h"
#endif
#include "assert.h"
#include "common_def.h"
#include "uapi_crc.h"
#ifndef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
#include "flash_task_mutex.h"
#include "flash_task_adapt.h"
#endif

#define KV_WRITE_KEY_BUFFER_SIZE    NV_KEY_DATA_CHUNK_LEN
#define KV_WRITE_CHUNK_SIZE         32

#define assert__(x)        ((void)0)

/* Pointer to current state machine being processed.
 * Will point to the last instance in a list of kv_active_state_machine_t, or NULL if
 * no state machine is being processed */
STATIC kv_active_state_machine_t *g_current_state_machine = NULL;

/* Holds a search filter being used by searching operations performed by state machines */
STATIC kv_key_filter_t g_search_filter;

/* Holds details of key data to be written to a store */
STATIC kv_key_details_t g_new_key_details;

/* The key within a KV store begin accessed by a state machine */
STATIC kv_key_handle_t g_current_key;

/* The flash page within a KV store begin accessed by a state machine */
STATIC kv_page_handle_t g_current_page;

/* The KV store begin accessed by a state machine */
STATIC kv_store_t g_current_store;

/* The position in a flash page where a new key will be written */
STATIC kv_key_location g_current_store_write_location;

/* Memory address of flash page being used to defrag an existing page into */
STATIC kv_page_location g_defrag_page_location = NULL;


STATIC errcode_t kv_create_write_buffer(uint32_t write_location, uint32_t size);
STATIC void kv_remove_write_buffer(void);
STATIC kv_managed_source_buffer_t* kv_update_source_buffer(uint8_t *data, uint32_t data_length);
STATIC void kv_remove_source_buffer(void);
STATIC void kv_release_crypto(void);
STATIC errcode_t begin_state_machine(const kv_state_machine_t *machine);
STATIC void end_state_machine(void);

/* State machine action functions */
STATIC kv_update_event_t kv_update_action_store_write_buffer(void);
STATIC kv_update_event_t kv_update_action_prime_write_buffer(void);

STATIC kv_update_event_t kv_update_action_select_first_page(void);
STATIC kv_update_event_t kv_update_action_select_next_page(void);
STATIC kv_update_event_t kv_update_action_find_first_key(void);
STATIC kv_update_event_t kv_update_action_find_next_key(void);
STATIC kv_update_event_t kv_update_action_prepare_delete_key(void);

STATIC kv_update_event_t kv_update_action_find_defrag_page(void);
STATIC kv_update_event_t kv_update_action_erase_defrag_page(void);
STATIC kv_update_event_t kv_update_action_prepare_copy_key(void);

STATIC kv_update_event_t kv_update_action_prepare_defrag_page(void);
STATIC kv_update_event_t kv_update_action_copy_all_keys(void);
STATIC kv_update_event_t kv_update_action_prepare_page_header(void);
STATIC kv_update_event_t kv_update_action_find_write_position(void);
STATIC kv_update_event_t kv_update_action_defrag_current_page(void);
STATIC kv_update_event_t kv_update_action_update_nvregion_map(void);

STATIC kv_update_event_t kv_update_action_prepare_store(void);
STATIC kv_update_event_t kv_update_action_prepare_write_key(void);
STATIC kv_update_event_t kv_update_action_claim_crypto(void);
STATIC kv_update_event_t kv_update_action_erase_old_keys(void);

STATIC kv_update_event_t kv_update_action_update_nvmap_for_erase_key(void);
STATIC kv_update_event_t kv_update_action_update_nvmap_for_new_key(void);


/* State machine to iterate through keys in a store marking all keys as erased
 * - Uses a pre-configured filter to determine which keys to select for erasing
 */
static const kv_update_transition_t g_erase_keys_transitions[] = {
    {.state = STATE_SELECT_FIRST_PAGE,  .event = EVENT_PAGE_SELECTED,       .next_state = STATE_FIND_FIRST_KEY  },
    {.state = STATE_SELECT_FIRST_PAGE,  .event = EVENT_PAGE_NOT_SELECTED,   .next_state = STATE_EXIT            },
    {.state = STATE_SELECT_NEXT_PAGE,   .event = EVENT_PAGE_SELECTED,       .next_state = STATE_FIND_FIRST_KEY  },
    {.state = STATE_SELECT_NEXT_PAGE,   .event = EVENT_PAGE_NOT_SELECTED,   .next_state = STATE_EXIT            },
    {.state = STATE_FIND_FIRST_KEY,     .event = EVENT_KEY_FOUND,           .next_state = STATE_PREP_DELETE_KEY },
    {.state = STATE_FIND_FIRST_KEY,     .event = EVENT_KEY_NOT_FOUND,       .next_state = STATE_SELECT_NEXT_PAGE},
    {.state = STATE_FIND_NEXT_KEY,      .event = EVENT_KEY_FOUND,           .next_state = STATE_PREP_DELETE_KEY },
    {.state = STATE_FIND_NEXT_KEY,      .event = EVENT_KEY_NOT_FOUND,       .next_state = STATE_SELECT_NEXT_PAGE},
    {.state = STATE_PREP_DELETE_KEY,    .event = EVENT_WRITE_BUFFER_PRIMED, .next_state = STATE_PERFORM_WRITE   },
    {.state = STATE_PERFORM_WRITE,      .event = EVENT_WRITE_BUFFER_STORED, .next_state = STATE_UPDATE_MAP_FOR_OLD },
    {.state = STATE_UPDATE_MAP_FOR_OLD, .event = EVENT_PAGE_MAP_UPDATED,    .next_state = STATE_FIND_NEXT_KEY   },
    {.state = STATE_PERFORM_WRITE,      .event = EVENT_SUSPEND,             .next_state = STATE_SUSPENDED       },
    {.state = STATE_EXIT,               .event = EVENT_NONE,                .next_state = STATE_EXIT            }
};
static const kv_update_action_t g_erase_keys_actions[] = {
    {.state = STATE_SELECT_FIRST_PAGE,  .action = kv_update_action_select_first_page },
    {.state = STATE_SELECT_NEXT_PAGE,   .action = kv_update_action_select_next_page  },
    {.state = STATE_FIND_FIRST_KEY,     .action = kv_update_action_find_first_key    },
    {.state = STATE_FIND_NEXT_KEY,      .action = kv_update_action_find_next_key     },
    {.state = STATE_PREP_DELETE_KEY,    .action = kv_update_action_prepare_delete_key},
    {.state = STATE_PERFORM_WRITE,      .action = kv_update_action_store_write_buffer},
    {.state = STATE_UPDATE_MAP_FOR_OLD, .action = kv_update_action_update_nvmap_for_erase_key},
    {.state = STATE_EXIT,               .action = NULL                            }
};
static const kv_state_machine_t g_erase_keys_machine = {
    .initial_state     = STATE_SELECT_FIRST_PAGE,
    .resume_state      = STATE_PERFORM_WRITE,
    .exit_event        = EVENT_KEYS_ERASED,
    .transition_table  = g_erase_keys_transitions,
    .action_table      = g_erase_keys_actions,
    .write_buffer_size = sizeof(uint32_t)
};

/*
 * State machine to locate a defrag page
 * - Scans NV Region for an unused page of flash
 * - Sets g_defrag_page_location to point the unused flash page, thus declaring it to be the current defrag page
 * - Ensures defrag page is erased
 */
static const kv_update_transition_t g_prep_defrag_page_transitions[] = {
    {.state = STATE_FIND_DEFRAG,  .event = EVENT_DEFRAG_FOUND,  .next_state = STATE_ERASE_DEFRAG},
    {.state = STATE_ERASE_DEFRAG, .event = EVENT_DEFRAG_ERASED, .next_state = STATE_EXIT        },
    {.state = STATE_ERASE_DEFRAG, .event = EVENT_SUSPEND,       .next_state = STATE_SUSPENDED   },
    {.state = STATE_EXIT,         .event = EVENT_NONE,          .next_state = STATE_EXIT        }
};
static const kv_update_action_t g_prep_defrag_page_actions[] = {
    {.state = STATE_FIND_DEFRAG,  .action = kv_update_action_find_defrag_page },
    {.state = STATE_ERASE_DEFRAG, .action = kv_update_action_erase_defrag_page},
    {.state = STATE_EXIT,         .action = NULL                           }
};
static const kv_state_machine_t g_prep_defrag_page_machine = {
    .initial_state     = STATE_FIND_DEFRAG,
    .resume_state      = STATE_ERASE_DEFRAG,
    .exit_event        = EVENT_DEFRAG_PREPARED,
    .transition_table  = g_prep_defrag_page_transitions,
    .action_table      = g_prep_defrag_page_actions,
    .write_buffer_size = 0
};

/*
 * State machine to copy keys from the current page to the defrag page.
 * Keys to copy are selected by a pre-configured filter, contained in g_search_filter
 * Current page identified by g_current_page
 * Defrag page identified by g_defrag_page_location
 */
static const kv_update_transition_t g_copy_all_keys_transitions[] = {
    {.state = STATE_FIND_FIRST_KEY, .event = EVENT_KEY_FOUND,            .next_state = STATE_PREP_COPY_KEY  },
    {.state = STATE_FIND_FIRST_KEY, .event = EVENT_KEY_NOT_FOUND,        .next_state = STATE_EXIT           },
    {.state = STATE_FIND_NEXT_KEY,  .event = EVENT_KEY_FOUND,            .next_state = STATE_PREP_COPY_KEY  },
    {.state = STATE_FIND_NEXT_KEY,  .event = EVENT_KEY_NOT_FOUND,        .next_state = STATE_EXIT           },
    {.state = STATE_PREP_COPY_KEY,  .event = EVENT_COPY_KEY_READY,       .next_state = STATE_PRIME_WRITE    },
    {.state = STATE_PRIME_WRITE,    .event = EVENT_WRITE_BUFFER_PRIMED,  .next_state = STATE_PERFORM_WRITE  },
    {.state = STATE_PRIME_WRITE,    .event = EVENT_WRITE_DATA_EXHAUSTED, .next_state = STATE_FIND_NEXT_KEY  },
    {.state = STATE_PERFORM_WRITE,  .event = EVENT_WRITE_BUFFER_STORED,  .next_state = STATE_PRIME_WRITE    },
    {.state = STATE_PERFORM_WRITE,  .event = EVENT_SUSPEND,              .next_state = STATE_SUSPENDED      },
    {.state = STATE_EXIT,           .event = EVENT_NONE,                 .next_state = STATE_EXIT           }
};
static const kv_update_action_t g_copy_all_keys_actions[] = {
    {.state = STATE_FIND_FIRST_KEY, .action = kv_update_action_find_first_key    },
    {.state = STATE_FIND_NEXT_KEY,  .action = kv_update_action_find_next_key     },
    {.state = STATE_PREP_COPY_KEY,  .action = kv_update_action_prepare_copy_key  },
    {.state = STATE_PRIME_WRITE,    .action = kv_update_action_prime_write_buffer},
    {.state = STATE_PERFORM_WRITE,  .action = kv_update_action_store_write_buffer},
    {.state = STATE_EXIT,           .action = NULL                            }
};
static const kv_state_machine_t g_copy_all_keys_machine = {
    .initial_state     = STATE_FIND_FIRST_KEY,
    .resume_state      = STATE_PERFORM_WRITE,
    .exit_event        = EVENT_ALL_KEYS_COPIED,
    .transition_table  = g_copy_all_keys_transitions,
    .action_table      = g_copy_all_keys_actions,
    .write_buffer_size = KV_WRITE_KEY_BUFFER_SIZE
};

/*
 * State machine to defrag a store page
 * Makes use of the g_prep_defrag_page_machine state machine
 * Makes use of the g_copy_all_keys_machine state machine
 * - Locates an unused flash page in the NV Region and erased it
 * - Copies valid keys from page being defragged to the new page
 * - Writes an updated page header to top of the new page, so that it supercedes the recently defragged page
 */
static const kv_update_transition_t g_defrag_page_transitions[] = {
    {.state = STATE_PREP_DEFRAG,         .event = EVENT_DEFRAG_PREPARED,      .next_state = STATE_COPY_ALL_KEYS      },
    {.state = STATE_COPY_ALL_KEYS,       .event = EVENT_ALL_KEYS_COPIED,      .next_state = STATE_PREP_PAGE_HEADER   },
    {.state = STATE_PREP_PAGE_HEADER,    .event = EVENT_WRITE_BUFFER_PRIMED,  .next_state = STATE_WRITE_PAGE_HEADER  },
    {.state = STATE_WRITE_PAGE_HEADER,   .event = EVENT_WRITE_BUFFER_STORED,  .next_state = STATE_UPDATE_NVREGION_MAP},
    {.state = STATE_WRITE_PAGE_HEADER,   .event = EVENT_SUSPEND,              .next_state = STATE_SUSPENDED          },
    {.state = STATE_UPDATE_NVREGION_MAP, .event = EVENT_NVREGION_MAP_UPDATED, .next_state = STATE_EXIT               },
    {.state = STATE_EXIT,                .event = EVENT_NONE,                 .next_state = STATE_EXIT               }
};
static const kv_update_action_t g_defrag_page_actions[] = {
    {.state = STATE_PREP_DEFRAG,         .action = kv_update_action_prepare_defrag_page},
    {.state = STATE_COPY_ALL_KEYS,       .action = kv_update_action_copy_all_keys      },
    {.state = STATE_PREP_PAGE_HEADER,    .action = kv_update_action_prepare_page_header},
    {.state = STATE_WRITE_PAGE_HEADER,   .action = kv_update_action_store_write_buffer },
    {.state = STATE_UPDATE_NVREGION_MAP, .action = kv_update_action_update_nvregion_map},
    {.state = STATE_EXIT,                .action = NULL                             }
};
static const kv_state_machine_t g_defrag_page_machine = {
    .initial_state     = STATE_PREP_DEFRAG,
    .resume_state      = STATE_WRITE_PAGE_HEADER,
    .exit_event        = EVENT_DEFRAG_COMPLETE,
    .transition_table  = g_defrag_page_transitions,
    .action_table      = g_defrag_page_actions,
    .write_buffer_size = sizeof(kv_page_header_t)
};

/*
 * State machine to prepare a store for writing a key
 * May make use of the g_defrag_page_machine state machine
 * - Selects a suitable page in a store for receiving a key
 * - Kicks off a defrag of the selected page if necessary
 */
static const kv_update_transition_t g_prepare_store_transitions[] = {
    {.state = STATE_FIND_WRITE_POS, .event = EVENT_DEFRAG_REQUIRED, .next_state = STATE_DEFRAG_PAGE   },
    {.state = STATE_FIND_WRITE_POS, .event = EVENT_WRITE_POS_FOUND, .next_state = STATE_EXIT          },
    {.state = STATE_DEFRAG_PAGE,    .event = EVENT_DEFRAG_COMPLETE, .next_state = STATE_FIND_WRITE_POS},
    {.state = STATE_EXIT,           .event = EVENT_NONE,            .next_state = STATE_EXIT          }
};
static const kv_update_action_t g_prepare_store_actions[] = {
    {.state = STATE_FIND_WRITE_POS, .action = kv_update_action_find_write_position},
    {.state = STATE_DEFRAG_PAGE,    .action = kv_update_action_defrag_current_page},
    {.state = STATE_EXIT,           .action = NULL                             }
};
STATIC const kv_state_machine_t g_prepare_store_machine = {
    .initial_state     = STATE_FIND_WRITE_POS,
    .resume_state      = STATE_INVALID,
    .exit_event        = EVENT_STORE_READY,
    .transition_table  = g_prepare_store_transitions,
    .action_table      = g_prepare_store_actions,
    .write_buffer_size = 0
};

/*
 * State machine to write a key to a store
 * Makes use of the g_prepare_store_machine state machine
 */
static const kv_update_transition_t g_write_key_transitions[] = {
    {.state = STATE_PREP_STORE,         .event = EVENT_STORE_READY,           .next_state = STATE_PREP_KEY_DATA     },
    {.state = STATE_PREP_KEY_DATA,      .event = EVENT_KEY_UPDATE_NOT_NEEDED, .next_state = STATE_EXIT              },
    {.state = STATE_PREP_KEY_DATA,      .event = EVENT_KEY_DATA_READY,        .next_state = STATE_CLAIM_CRYPTO      },
    {.state = STATE_CLAIM_CRYPTO,       .event = EVENT_CRYPTO_CLAIMED,        .next_state = STATE_PRIME_WRITE       },
    {.state = STATE_PRIME_WRITE,        .event = EVENT_WRITE_BUFFER_PRIMED,   .next_state = STATE_PERFORM_WRITE     },
    {.state = STATE_PRIME_WRITE,        .event = EVENT_WRITE_DATA_EXHAUSTED,  .next_state = STATE_UPDATE_MAP_FOR_NEW},
    {.state = STATE_UPDATE_MAP_FOR_NEW, .event = EVENT_PAGE_MAP_UPDATED,      .next_state = STATE_ERASE_OLD_KEYS    },
    {.state = STATE_PERFORM_WRITE,      .event = EVENT_WRITE_BUFFER_STORED,   .next_state = STATE_PRIME_WRITE       },
    {.state = STATE_PERFORM_WRITE,      .event = EVENT_SUSPEND,               .next_state = STATE_SUSPENDED         },
    {.state = STATE_ERASE_OLD_KEYS,     .event = EVENT_KEYS_ERASED,           .next_state = STATE_EXIT              },
    {.state = STATE_EXIT,               .event = EVENT_NONE,                  .next_state = STATE_EXIT              }
};
static const kv_update_action_t g_write_key_actions[] = {
    {.state = STATE_PREP_STORE,         .action = kv_update_action_prepare_store     },
    {.state = STATE_PREP_KEY_DATA,      .action = kv_update_action_prepare_write_key },
    {.state = STATE_CLAIM_CRYPTO,       .action = kv_update_action_claim_crypto      },
    {.state = STATE_PRIME_WRITE,        .action = kv_update_action_prime_write_buffer},
    {.state = STATE_PERFORM_WRITE,      .action = kv_update_action_store_write_buffer},
    {.state = STATE_UPDATE_MAP_FOR_NEW, .action = kv_update_action_update_nvmap_for_new_key},
    {.state = STATE_ERASE_OLD_KEYS,     .action = kv_update_action_erase_old_keys    },
    {.state = STATE_EXIT,               .action = NULL                            }
};
static const kv_state_machine_t g_write_key_machine = {
    .initial_state     = STATE_PREP_STORE,
    .resume_state      = STATE_CLAIM_CRYPTO,
    .exit_event        = EVENT_WRITE_COMPLETE,
    .transition_table  = g_write_key_transitions,
    .action_table      = g_write_key_actions,
    .write_buffer_size = KV_WRITE_KEY_BUFFER_SIZE
};

/* Attempts to write all data in a write buffer to flash */
STATIC kv_update_event_t kv_update_action_store_write_buffer(void)
{
    errcode_t ret;
    uint32_t read_location;
    uint32_t write_length;
    uint32_t written;

    kv_managed_write_buffer_t *write_buffer = g_current_state_machine->write_buffer;

    /* Skip over section of write buffer that has already been written, due to being suspended */
    if (write_buffer->write_location < write_buffer->resume_location) {
        uint32_t data_already_written = write_buffer->resume_location - write_buffer->write_location;
        if (data_already_written > write_buffer->data_length) {
            data_already_written = write_buffer->data_length;
        }
        write_buffer->data_consumed += (uint16_t)data_already_written;
        write_buffer->write_location += data_already_written;
    }

    /* Attempt to write chunks of data from the write_buffer to flash */
    while (write_buffer->data_consumed < write_buffer->data_length) {
        /* Determine size and position of chunk to write */
        read_location = (uint32_t)(uintptr_t)write_buffer->data + write_buffer->data_consumed;
        write_length = uapi_min(KV_WRITE_CHUNK_SIZE, write_buffer->data_length - write_buffer->data_consumed);

        /* Attempt to write chunk */
        ret = kv_key_write_flash(write_buffer->write_location, write_length, (uint8_t *)(uintptr_t)read_location);
        if (ret == ERRCODE_SUCC) {
            written = write_length;
        } else {
            written = 0;
        }
        write_buffer->data_consumed += (uint16_t)written;
        /* Update write position, automatically allowing for subsequent writes to occur sequentially in flash */
        write_buffer->write_location += written;
        if (written < write_length) {
            /* Write was aborted early */
            write_buffer->resume_location = write_buffer->write_location;
            return EVENT_SUSPEND;
        }
    }
    /* Write completed */
    return EVENT_WRITE_BUFFER_STORED;
}
STATIC errcode_t kv_update_hash_encrypt_chunk(kv_managed_write_buffer_t *write_buffer,
    const kv_managed_source_buffer_t *source_buffer, uint32_t chunk_dest, uint32_t chunk_len)
{
    /* Need to cal CRC? */
    errcode_t ret = ERRCODE_SUCC;
    if (source_buffer->crc_data) {
        if (!write_buffer->crc_claimed) {
            return ERRCODE_NV_HASH_UNAVAILABLE;
        }
#if (defined(CONFIG_NV_SUPPORT_CRC16_VERIFY) && (CONFIG_NV_SUPPORT_CRC16_VERIFY == NV_YES))
        write_buffer->crc_ret =
            (uint16_t)uapi_crc16((uint16_t)write_buffer->crc_ret, (const uint8_t *)(uintptr_t)chunk_dest, chunk_len);
#else
        write_buffer->crc_ret = uapi_crc32(write_buffer->crc_ret, (const uint8_t *)(uintptr_t)chunk_dest, chunk_len);
#endif
    }

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    /* Need to hash data? */
    if (source_buffer->hash_data) {
        if (!write_buffer->hash_claimed) {
            return ERRCODE_NV_HASH_UNAVAILABLE;
        }
        nv_crypto_update_hash((const uint8_t *)(uintptr_t)chunk_dest, chunk_len);
    }

    /* Need to encrypt data? */
    if (source_buffer->encrypt_data) {
        if (!write_buffer->encrypt_claimed) {
            return ERRCODE_NV_AES_UNAVAILABLE;
        }

        ret = nv_crypto_encode(write_buffer->crypto_handle, (uintptr_t)chunk_dest, (uintptr_t)chunk_dest, chunk_len);
    }

    if (source_buffer->gcm_tag_data || source_buffer->hash_data) {
        if ((source_buffer->gcm_tag_data && !write_buffer->gcm_tag_claimed) ||
            (source_buffer->hash_data && !write_buffer->hash_claimed)) {
            return ERRCODE_NV_HASH_UNAVAILABLE;
        }
#if (defined(CONFIG_NV_SUPPORT_CRC16_VERIFY) && (CONFIG_NV_SUPPORT_CRC16_VERIFY == NV_YES))
        write_buffer->crc_ret =
            (uint16_t)uapi_crc16((uint16_t)write_buffer->crc_ret, (const uint8_t *)(uintptr_t)chunk_dest, chunk_len);
#else
        write_buffer->crc_ret = uapi_crc32(write_buffer->crc_ret, (const uint8_t *)(uintptr_t)chunk_dest, chunk_len);
#endif
    }
#endif
    return ret;
}

/* Copy data from the selected source buffer to the write_buffer, hashing and encrypting the data as necessary */
STATIC errcode_t prime_next_chunk(void)
{
    kv_managed_source_buffer_t *source_buffer = g_current_state_machine->current_source;
    kv_managed_write_buffer_t *write_buffer = g_current_state_machine->write_buffer;

    uint32_t remaining_dest_space = write_buffer->size - write_buffer->data_length;
    uint32_t remaining_src_data = source_buffer->data_length - source_buffer->data_consumed;
    uint16_t chunk_len = (uint16_t)uapi_min(remaining_dest_space, remaining_src_data);

    uint32_t chunk_dest = (uint32_t)(uintptr_t)write_buffer->data + write_buffer->data_length;
    uint32_t chunk_src = (uint32_t)(uintptr_t)source_buffer->data + source_buffer->data_consumed;

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (source_buffer->encrypt_data) {
        /* For encrypted data, the length of each operation is the same as that of NV reading.
         * CNCommont: 对于加密数据，每次操作的数据长度与读NV时保持一致（KEY_DATA_CHUNK_LEN），因此当write_buffer剩余空间
         * 小于KEY_DATA_CHUNK_LEN时，暂不处理.
         */
        chunk_len = (uint16_t)uapi_min(chunk_len, NV_KEY_DATA_CHUNK_LEN);
        if (remaining_dest_space < NV_KEY_DATA_CHUNK_LEN) {
            return ERRCODE_NV_BUFFER_PRIMED_PREMATURELY;
        }
    }
#endif

    kv_attributes_t attributes = (source_buffer->encrypt_data) ? KV_ATTRIBUTE_ENCRYPTED : 0;
    if (remaining_dest_space < kv_key_padded_data_length(attributes, chunk_len)) {
        /* Not enough space for padded data, exit early and write what we have */
        return ERRCODE_NV_BUFFER_PRIMED_PREMATURELY;
    }

    /* Copy a chunk of source_buffer data to write buffer */
    if ((chunk_src >= FLASH_PHYSICAL_ADDR_START) &&
        (chunk_src + chunk_len) <= FLASH_PHYSICAL_ADDR_END) {
        /* Source buffer is referencing data in Flash */
        errcode_t res = kv_key_helper_copy_flash(chunk_dest, chunk_src, chunk_len);
        if (res != ERRCODE_SUCC) {
            return res;
        }
    } else {
        int32_t result;
        /* Source buffer is referencing data in RAM */
        result = memcpy_s((void *)(uintptr_t)chunk_dest, remaining_dest_space,
                          (const void *)(uintptr_t)chunk_src, chunk_len);
        if (result != (int32_t)EOK) {
            assert__(false);
        }
    }

    /* Round chunk_len up to a multiple of 4 or 16 bytes, depending upon the encryption requirement */
    /* We have already checked there is enough space in the write buffer                            */
    chunk_len = kv_key_padded_data_length(attributes, chunk_len);

    errcode_t hash_crypt_ret;
    hash_crypt_ret = kv_update_hash_encrypt_chunk(write_buffer, source_buffer, chunk_dest, chunk_len);
    if (hash_crypt_ret != ERRCODE_SUCC) {
        return hash_crypt_ret;
    }

    write_buffer->data_length += chunk_len;
    source_buffer->data_consumed += chunk_len;
    return ERRCODE_SUCC;
}

/* Select next source buffer, filling it with a hash if required */
STATIC errcode_t select_next_source_buffer(void)
{
    kv_managed_source_buffer_t *source_buffer = g_current_state_machine->current_source->next;
    kv_managed_write_buffer_t *write_buffer = g_current_state_machine->write_buffer;

    g_current_state_machine->current_source = source_buffer;

    /* Has the next source_buffer buffer been designated to receive the hash calculation */
    if ((source_buffer != NULL) && (source_buffer->receive_hash)) {
        if (write_buffer->crc_claimed) {
            if (source_buffer->data_length < KV_CRYPTO_CRC_SIZE) {
                return ERRCODE_NV_KEY_HASH_BUFFER_TOO_SMALL;
            }

            write_buffer->crc_ret = kv_crc32_swap(write_buffer->crc_ret);
            (void)memcpy_s((void *)source_buffer->data, KV_CRYPTO_CRC_SIZE, (const void *)&write_buffer->crc_ret,
                KV_CRYPTO_CRC_SIZE);
            write_buffer->crc_claimed = false;
            source_buffer->data_length = KV_CRYPTO_CRC_SIZE;
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
        } else if (write_buffer->hash_claimed) {
            if (source_buffer->data_length < KV_CRYPTO_HASH_SIZE) {
                return ERRCODE_NV_KEY_HASH_BUFFER_TOO_SMALL;
            }
            (void)memset_s(source_buffer->data,  source_buffer->data_length, 0, source_buffer->data_length);
            nv_crypto_complete_hash(source_buffer->data);

            write_buffer->crc_ret = kv_crc32_swap(write_buffer->crc_ret);
            (void)memcpy_s((void *)(source_buffer->data + KV_CRYPTO_HASH_SIZE - KV_CRYPTO_CRC_SIZE),
                KV_CRYPTO_CRC_SIZE, (const void *)&write_buffer->crc_ret, KV_CRYPTO_CRC_SIZE);

            write_buffer->hash_claimed = false;
            source_buffer->data_length = KV_CRYPTO_HASH_SIZE;
        } else if (write_buffer->gcm_tag_claimed) {
            uint32_t tag_len = NV_AES_GCM_TAG_LENGTH;
            if (source_buffer->data_length < KV_CRYPTO_HASH_SIZE) {
                return ERRCODE_NV_KEY_HASH_BUFFER_TOO_SMALL;
            }

            write_buffer->crc_ret = kv_crc32_swap(write_buffer->crc_ret);
            memset_s(source_buffer->data, source_buffer->data_length, 0, source_buffer->data_length);

            nv_crypto_get_tag(write_buffer->crypto_handle, source_buffer->data, &tag_len);

            (void)memcpy_s((void *)(source_buffer->data + KV_CRYPTO_HASH_SIZE - KV_CRYPTO_CRC_SIZE),
                KV_CRYPTO_CRC_SIZE, (const void *)&write_buffer->crc_ret, KV_CRYPTO_CRC_SIZE);

            write_buffer->gcm_tag_claimed = false;
            source_buffer->data_length = KV_CRYPTO_HASH_SIZE;
#endif
        } else {
            return ERRCODE_NV_HASH_UNAVAILABLE;
        }
    }

    return ERRCODE_SUCC;
}

/* Gather data from one or more source buffers to fill write_buffer with more data ready for writing to flash */
STATIC kv_update_event_t kv_update_action_prime_write_buffer(void)
{
    errcode_t res;

    kv_managed_source_buffer_t *source_buffer = g_current_state_machine->current_source;
    kv_managed_write_buffer_t *write_buffer = g_current_state_machine->write_buffer;

    (void)memset_s(write_buffer->data, write_buffer->size, 0, write_buffer->size);

    write_buffer->data_length = 0;
    write_buffer->data_consumed = 0;

    while ((source_buffer != NULL) && (write_buffer->data_length < write_buffer->size)) {
        if (source_buffer->data_consumed < source_buffer->data_length) {
            res = prime_next_chunk();
            if (res == ERRCODE_NV_BUFFER_PRIMED_PREMATURELY) {
                /* Not enough space for padded data so just write what we have for now */
                break;
            }
            if (res != ERRCODE_SUCC) {
                g_current_state_machine->error_code = res;
                return EVENT_ERROR;
            }
        } else {
            /* Select next source buffer */
            res = select_next_source_buffer();
            if (res != ERRCODE_SUCC) {
                g_current_state_machine->error_code = res;
                return EVENT_ERROR;
            }
            source_buffer = g_current_state_machine->current_source;
        }
    }

    /* Did we manage to place any data in the write buffer? */
    if (write_buffer->data_length == 0) {
        kv_release_crypto();
        return EVENT_WRITE_DATA_EXHAUSTED;
    } else {
        return EVENT_WRITE_BUFFER_PRIMED;
    }
}

/*
 * Attempts to obtain a handle to the first page of the g_current_store
 * - g_current_page will be populated if a page is obtained
 */
STATIC kv_update_event_t kv_update_action_select_first_page(void)
{
    errcode_t res;

    res = kv_store_get_page_handle(g_current_store, 0, &g_current_page);
    if (res == ERRCODE_SUCC) {
        return EVENT_PAGE_SELECTED;
    }
    return EVENT_PAGE_NOT_SELECTED;
}

/*
 * Attempts to obtain a handle to the next page of the g_current_store, based on g_current_page
 * - g_current_page will be updated if a page is obtained
 */
STATIC kv_update_event_t kv_update_action_select_next_page(void)
{
    errcode_t res;
    uint32_t page_index;
    if (kv_page_get_index(&g_current_page, &page_index) != ERRCODE_SUCC) {
        return EVENT_PAGE_NOT_SELECTED;
    }
    page_index++;
    if (page_index < kv_store_get_page_count(g_current_store)) {
        res = kv_store_get_page_handle(g_current_store, page_index, &g_current_page);
        if (res == ERRCODE_SUCC) {
            return EVENT_PAGE_SELECTED;
        }
    }

    return EVENT_PAGE_NOT_SELECTED;
}

/*
 * Attempts to obtain the first key in the g_current_page, that conforms to g_search_filter
 * - g_current_key will be populated if a key is obtained
 */
STATIC kv_update_event_t kv_update_action_find_first_key(void)
{
    errcode_t res;

    res = kv_page_find_first_key(&g_current_page, &g_search_filter, &g_current_key);
    while (res == ERRCODE_SUCC) {
        if (g_current_key.key_location != g_search_filter.location) {
            return EVENT_KEY_FOUND;
        }
        res = kv_page_find_next_key(&g_current_page, &g_search_filter, &g_current_key);
    }
    return EVENT_KEY_NOT_FOUND;
}

/*
 * Attempts to obtain the next key in the g_current_page, based on g_current_key and that conforms to g_search_filter
 * - g_current_key will be updated if a key is obtained
 */
STATIC kv_update_event_t kv_update_action_find_next_key(void)
{
    errcode_t res;

    res = kv_page_find_next_key(&g_current_page, &g_search_filter, &g_current_key);
    while (res == ERRCODE_SUCC) {
        if (g_current_key.key_location != g_search_filter.location) {
            return EVENT_KEY_FOUND;
        }
        res = kv_page_find_next_key(&g_current_page, &g_search_filter, &g_current_key);
    }
    return EVENT_KEY_NOT_FOUND;
}

/*
 * Prepares the g_current_state_machine->write_buffer to mark g_current_key as invalid (and hence erased)
 * Will not mark keys with the attribute KV_ATTRIBUTE_PERMANENT as invalid
 */
STATIC kv_update_event_t kv_update_action_prepare_delete_key(void)
{
    kv_managed_write_buffer_t *write_buffer = NULL;
    kv_attributes_t attributes = kv_key_attributes(&g_current_key);
    if (((uint32_t)attributes & (uint32_t)KV_ATTRIBUTE_PERMANENT) != 0  &&
        (g_new_key_details.focre_write != true)) {
        g_current_state_machine->error_code = ERRCODE_NV_TRYING_TO_MODIFY_A_PERMANENT_KEY;
        return EVENT_ERROR;
    }

    write_buffer = g_current_state_machine->write_buffer;
    if (write_buffer == NULL) {
        g_current_state_machine->error_code = ERRCODE_NV_WRITE_BUFFER_NOT_ALLOCATED;
        return EVENT_ERROR;
    }
    if (write_buffer->size < sizeof(uint32_t)) {
        g_current_state_machine->error_code = ERRCODE_NV_WRITE_BUFFER_TOO_SMALL;
        return EVENT_ERROR;
    }

    /* As only 4 bytes are being written, directly prime write buffer rather that setting up a source buffer */
    *(uint32_t *)write_buffer->data = KV_KEY_INVALID;
    write_buffer->data_consumed = 0;

    if (g_nv_header_magic == KV_KEY_MAGIC) {
        write_buffer->data_length = (uint16_t)sizeof(uint8_t);
        /* Need to manually configure the write location for each key being deleted */
        /* as they will not follow in an ordered sequence                           */
        write_buffer->write_location = (uint32_t)(uintptr_t)((uint8_t *)g_current_key.key_location + sizeof(uint8_t));
    } else {
        write_buffer->data_length = (uint16_t)sizeof(uint16_t);
        write_buffer->write_location = (uint32_t)(uintptr_t)g_current_key.key_location;
    }

    return EVENT_WRITE_BUFFER_PRIMED;
}

STATIC kv_update_event_t kv_update_action_prepare_copy_key(void)
{
    kv_managed_write_buffer_t *write_buffer = g_current_state_machine->write_buffer;
    if (write_buffer == NULL) {
        g_current_state_machine->error_code = ERRCODE_NV_WRITE_BUFFER_NOT_ALLOCATED;
        return EVENT_ERROR;
    }
    if (write_buffer->write_location == 0) {
        write_buffer->write_location = (uint32_t)(uintptr_t)g_defrag_page_location + (uint32_t)sizeof(kv_page_header_t);
    }

    /* Use a source buffer to manage copy progress of key data */
    uint8_t *source = (uint8_t *)g_current_key.key_location;
    uint16_t length = kv_key_flash_size(&g_current_key);
    kv_managed_source_buffer_t *source_buffer = kv_update_source_buffer(source, length);
    g_current_state_machine->current_source = source_buffer;  /* Point current source buffer at first buffer */
    if (source_buffer == NULL) {
        g_current_state_machine->error_code = ERRCODE_MALLOC;
        return EVENT_ERROR;
    }

    return EVENT_COPY_KEY_READY;
}

/*
 * Attempts to locate an unused page of flash, in the KV NV flash region, to use as the current defrag page
 * g_defrag_page_location will be set to point to the unused flash page
 */
STATIC kv_update_event_t kv_update_action_find_defrag_page(void)
{
    errcode_t res;
    g_defrag_page_location = NULL;
    res = kv_nvregion_find_unused_page(&g_defrag_page_location);
    if (res == ERRCODE_SUCC) {
        nv_log_debug("[NV] found defrag_page. loc = 0x%x\r\n", g_defrag_page_location);
        return EVENT_DEFRAG_FOUND;
    } else {
        g_current_state_machine->error_code = res;
        return EVENT_ERROR;
    }
}

/*
 * Attempts to erase the current defrag page, as defined by g_defrag_page_location
 */
STATIC kv_update_event_t kv_update_action_erase_defrag_page(void)
{
    errcode_t res;
    res = kv_nvregion_erase_page(g_defrag_page_location);
    if (res == ERRCODE_SUCC) {
        return EVENT_DEFRAG_ERASED;
    } else if (res == ERRCODE_NV_WRITE_VETOED) {
        return EVENT_SUSPEND;
    } else {
        g_current_state_machine->error_code = res;
        return EVENT_ERROR;
    }
}

/*
 * Attempts to start the g_prep_defrag_page_machine state machine
 */
STATIC kv_update_event_t kv_update_action_prepare_defrag_page(void)
{
    errcode_t res = begin_state_machine(&g_prep_defrag_page_machine);
    if (res == ERRCODE_SUCC) {
        return EVENT_NONE;
    } else {
        g_current_state_machine->error_code = res;
        return EVENT_ERROR;
    }
}

/*
 * Attempts to start the g_copy_all_keys_machine state machine
 * Configures g_search_filter to find all valid (un-erased) keys
 */
STATIC kv_update_event_t kv_update_action_copy_all_keys(void)
{
    /* Configure search filter to copy all valid keys */
    g_search_filter.pattern = 0;
    g_search_filter.mask = 0;
    g_search_filter.state = KV_KEY_FILTER_STATE_VALID;
    g_search_filter.type = KV_KEY_FILTER_TYPE_ANY;
    g_search_filter.location = 0;

    errcode_t res = begin_state_machine(&g_copy_all_keys_machine);
    if (res == ERRCODE_SUCC) {
        return EVENT_NONE;
    } else {
        g_current_state_machine->error_code = res;
        return EVENT_ERROR;
    }
}

/*
 * Prepares g_current_state_machine->write_buffer with information to construct a page header for the
 * current defrag page, as defined by g_defrag_page_location
 * All information is obtained from g_current_page
 * The page sequence_number is updated to indicate that this page will supersede g_current_page
 */
STATIC kv_update_event_t kv_update_action_prepare_page_header(void)
{
    kv_managed_write_buffer_t *write_buffer = g_current_state_machine->write_buffer;
    if (write_buffer == NULL) {
        g_current_state_machine->error_code = ERRCODE_NV_WRITE_BUFFER_NOT_ALLOCATED;
        return EVENT_ERROR;
    }
    write_buffer->write_location = (uint32_t)(uintptr_t)g_defrag_page_location;

    if (memcpy_s(write_buffer->data, sizeof(kv_page_header_t), &g_current_page, sizeof(kv_page_header_t)) != EOK) {
        return EVENT_ERROR;
    }
    write_buffer->data_length = (uint16_t)sizeof(kv_page_header_t);
    write_buffer->data_consumed = 0;

    /* Too specific, move to kv_page? */
    kv_page_header_t *defrag_page_header = (kv_page_header_t *)write_buffer->data;
    defrag_page_header->sequence_number++;
    defrag_page_header->inverted_sequence_number = ~defrag_page_header->sequence_number;

    return EVENT_WRITE_BUFFER_PRIMED;
}

/*
 * Forces a re-scan of the KV NV flash region to update a map of KV pages
 */
STATIC kv_update_event_t kv_update_action_update_nvregion_map(void)
{
    errcode_t res;

    res = kv_nvregion_scan();
    if (res != ERRCODE_SUCC) {
        g_current_state_machine->error_code = res;
        return EVENT_ERROR;
    }
    return EVENT_NVREGION_MAP_UPDATED;
}

/*
 * Attempts to find a suitable write location in g_current_store for g_new_key_details
 * Will set g_current_store_write_location if enough space is found in g_current_store
 * Will indicate that a defrag is required if there is not currently enough free space in g_current_store
 * Will return an error if a defrag would not free up enough space for the new key
 */
STATIC kv_update_event_t kv_update_action_find_write_position(void)
{
    errcode_t res;
    uint16_t required_space;
    kv_key_handle_t new_key;
    kv_page_status_t page_status;

    /* Work out space needed for new key */
    kv_key_build_from_new(&new_key, &g_new_key_details, 0);
    required_space = kv_key_flash_size(&new_key);

    res = kv_store_find_write_page(g_current_store, required_space, &g_current_page, &page_status);
    if (res == ERRCODE_SUCC) {
        g_current_store_write_location = (kv_key_location)(uintptr_t)page_status.first_writable_location;
        return EVENT_WRITE_POS_FOUND;
    } else if (res == ERRCODE_NV_DEFRAGMENTATION_NEEDED) {
        return EVENT_DEFRAG_REQUIRED;
    } else {
        g_current_state_machine->error_code = res;
        return EVENT_ERROR;
    }
}

/*
 * Attempts to start the g_defrag_page_machine state machine
 */
STATIC kv_update_event_t kv_update_action_defrag_current_page(void)
{
    errcode_t res = begin_state_machine(&g_defrag_page_machine);
    if (res == ERRCODE_SUCC) {
        return EVENT_NONE;
    } else {
        g_current_state_machine->error_code = res;
        return EVENT_ERROR;
    }
}

/*
 * Attempts to start the g_prepare_store_machine state machine
 */
STATIC kv_update_event_t kv_update_action_prepare_store(void)
{
    errcode_t res = begin_state_machine(&g_prepare_store_machine);
    if (res == ERRCODE_SUCC) {
        return EVENT_NONE;
    } else {
        g_current_state_machine->error_code = res;
        return EVENT_ERROR;
    }
}

/*
 * Configure write location for g_current_state_machine->write_buffer
 * Attempt to allocate memory for AES Control data structure, if key data is to be encrypted
 */
STATIC kv_update_event_t kv_update_helper_prepare_write_buffer(void)
{
    kv_managed_write_buffer_t *write_buffer = g_current_state_machine->write_buffer;

    if (write_buffer == NULL) {
        g_current_state_machine->error_code = ERRCODE_NV_WRITE_BUFFER_NOT_ALLOCATED;
        return EVENT_ERROR;
    }
    write_buffer->write_location = (uint32_t)(uintptr_t)g_current_store_write_location;
    return EVENT_KEY_DATA_READY;
}

/*
 * Attempt to allocate a kv_managed_source_buffer_t for a key header
 */
STATIC kv_update_event_t kv_update_helper_setup_key_header_source_buffer(void)
{
    kv_managed_source_buffer_t *source_buffer = NULL;

    /* Setup source buffers for key header */
    source_buffer = kv_update_source_buffer((uint8_t *)&g_current_key.header, sizeof(kv_key_header_t));
    if (source_buffer == NULL) {
        g_current_state_machine->error_code = ERRCODE_NV_KEY_HEADER_BUFFER_NOT_ALLOCATED;
        return EVENT_ERROR;
    }

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (((uint32_t)g_new_key_details.attributes & (uint32_t)KV_ATTRIBUTE_ENCRYPTED) != 0) {
#if (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_YES)
        source_buffer->hash_data = true;
#else
        source_buffer->gcm_tag_data = true;
#endif
    } else {
        source_buffer->crc_data = true;
    }

#else
    source_buffer->crc_data = true;
#endif

    source_buffer->encrypt_data = false;
    return EVENT_KEY_DATA_READY;
}

/*
 * Attempt to allocate a kv_managed_source_buffer_t for key data
 */
STATIC kv_update_event_t kv_update_helper_setup_key_data_source_buffer(bool release_key_data)
{
    kv_managed_source_buffer_t *source_buffer = NULL;

    /* Setup source buffer for key data */
    source_buffer = kv_update_source_buffer((uint8_t *)g_new_key_details.kvalue, g_new_key_details.kvalue_length);
    if (source_buffer == NULL) {
        g_current_state_machine->error_code = ERRCODE_NV_KEY_DATA_BUFFER_NOT_ALLOCATED;
        return EVENT_ERROR;
    }
    source_buffer->release_data = release_key_data;

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (((uint32_t)g_new_key_details.attributes & (uint32_t)KV_ATTRIBUTE_ENCRYPTED) != 0) {
#if (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_YES)
        source_buffer->hash_data = true;
#else
        source_buffer->gcm_tag_data = true;
#endif
    } else {
        source_buffer->crc_data = true;
    }

#else
    source_buffer->crc_data = true;
#endif

    source_buffer->encrypt_data = (((uint32_t)g_new_key_details.attributes & (uint32_t)KV_ATTRIBUTE_ENCRYPTED) != 0);
    return EVENT_KEY_DATA_READY;
}

/*
 * Attempt to allocate a kv_managed_source_buffer_t for a key hash
 * Will also attempt to allocate a buffer to contain the calculated hash
 */
STATIC kv_update_event_t kv_update_helper_setup_key_hash_source_buffer(void)
{
    uint8_t *key_hash = NULL;
    kv_managed_source_buffer_t *source_buffer = NULL;
    uint32_t hash_crc_len = KV_CRYPTO_CRC_SIZE;
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (((uint32_t)g_new_key_details.attributes & (uint32_t)KV_ATTRIBUTE_ENCRYPTED) != 0) {
        hash_crc_len = KV_CRYPTO_HASH_SIZE;
    }
#endif

    /* Setup source buffer for key hash                                                               */
    /* The hash is calculated while data is written to flash then placed into this buffer for writing */
    key_hash = (uint8_t *)kv_malloc(hash_crc_len);
    if (key_hash == NULL) {
        g_current_state_machine->error_code = ERRCODE_MALLOC;
        return EVENT_ERROR;
    }
    source_buffer = kv_update_source_buffer(key_hash, hash_crc_len);
    if (source_buffer == NULL) {
        kv_free(key_hash);
        g_current_state_machine->error_code = ERRCODE_NV_KEY_HASH_BUFFER_NOT_ALLOCATED;
        return EVENT_ERROR;
    }
    source_buffer->release_data = true;
    source_buffer->receive_hash = true;
    source_buffer->encrypt_data = false;
    return EVENT_KEY_DATA_READY;
}

/*
 * Prepare kv_managed_source_buffers for the three separate parts of a key i.e. header, data and hash
 * Ensures state machine write buffer is ready to receive data for writing
 * CNcomment: 准备好NV写入需要的source buffer，NV的三个部分（header、数据、HASH/CRC）被放置在三个source buffer中
 */
STATIC kv_update_event_t kv_update_helper_prepare_source_buffers(bool release_key_data)
{
    kv_update_event_t event;

    event = kv_update_helper_prepare_write_buffer();
    if (event == EVENT_KEY_DATA_READY) {
        event = kv_update_helper_setup_key_header_source_buffer();
    }
    if (event == EVENT_KEY_DATA_READY) {
        event = kv_update_helper_setup_key_data_source_buffer(release_key_data);
    }
    if ((event != EVENT_KEY_DATA_READY) && release_key_data) {
        /* Ensure we release any new key data as it will not have been placed into a source buffer */
        /* which get cleaned up automatically when a state machine ends                            */
        kv_free((void *)g_new_key_details.kvalue);
        g_new_key_details.kvalue = NULL;
    }
    if (event == EVENT_KEY_DATA_READY) {
        event = kv_update_helper_setup_key_hash_source_buffer();
    }
    return event;
}

/*
 * Checks if g_current_key would end up being modified by any changes detailed in g_new_key_details
 * Will return an error if g_current_key is permanent
 */
STATIC kv_update_event_t kv_update_helper_check_for_key_updates(void)
{
    errcode_t res;
    kv_attributes_t attributes = kv_key_attributes(&g_current_key);
    if ((((uint32_t)attributes & (uint32_t)KV_ATTRIBUTE_PERMANENT) != 0) &&
        (g_new_key_details.focre_write != true)) {
        /* We can't modify permanent keys in any way */
        g_current_state_machine->error_code = ERRCODE_NV_TRYING_TO_MODIFY_A_PERMANENT_KEY;
        return EVENT_ERROR;
    }
    if (((uint32_t)attributes & (uint32_t)KV_ATTRIBUTE_NON_UPGRADE) !=
        ((uint32_t)g_new_key_details.attributes & (uint32_t)KV_ATTRIBUTE_NON_UPGRADE)) {
        /* Attributes are being updated */
        return EVENT_KEY_UPDATE_REQUIRED;
    }
    if (((uint32_t)attributes & (uint32_t)g_new_key_details.attributes) != (uint32_t)g_new_key_details.attributes) {
        /* Attributes are being updated */
        return EVENT_KEY_UPDATE_REQUIRED;
    }

    if (g_current_key.header.length != g_new_key_details.kvalue_length) {
        /* New data is of a different length */
        return EVENT_KEY_UPDATE_REQUIRED;
    }
    res = kv_helper_compare_key_data(&g_current_key, g_new_key_details.kvalue,
                                     (uint16_t)g_new_key_details.kvalue_length);
    if (res != ERRCODE_SUCC) {
        /* New data content is different */
        return EVENT_KEY_UPDATE_REQUIRED;
    }

    /* Key not actually being updated */
    return EVENT_KEY_UPDATE_NOT_NEEDED;
}

/*
 * Constructs a new key header from g_new_key_details, copying the attributes of any existing key with the same key_id
 * Updates g_current_key with details of the new key header
 */
STATIC kv_update_event_t kv_update_action_prepare_write_key(void)
{
    errcode_t res = kv_store_find_valid_key(g_current_store, g_new_key_details.key_id, &g_current_key);
    if (res == ERRCODE_SUCC) {
        kv_update_event_t event = kv_update_helper_check_for_key_updates();
        if (event != EVENT_KEY_UPDATE_REQUIRED) {
            nv_log_debug("[NV] The Key not need update. Key id = 0x%x\r\n", g_current_key.header.key_id);
            return event;
        }

        /* Combine existing key attributes with those requested for the new key */
        if (g_current_key.header.type != KV_KEY_TYPE_NORMAL) {
            g_new_key_details.attributes = (kv_attributes_t)((uint32_t)g_new_key_details.attributes |
                                                             (uint32_t)KV_ATTRIBUTE_PERMANENT);
        }
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
        if (g_current_key.header.enc_key != 0) {
            g_new_key_details.attributes = (kv_attributes_t)((uint32_t)g_new_key_details.attributes |
                                                             (uint32_t)KV_ATTRIBUTE_ENCRYPTED);
        }
#endif
    }

    kv_key_build_from_new(&g_current_key, &g_new_key_details, g_current_store_write_location);
    return kv_update_helper_prepare_source_buffers(false);
}

STATIC errcode_t kv_update_helper_get_current_key(uint8_t **old_kvalue, uint32_t *kvalue_length)
{
    kv_key_handle_t current_key;
    errcode_t res;

    res = kv_store_find_valid_key(g_current_store, g_new_key_details.key_id, &current_key);
    if (res != ERRCODE_SUCC) {
        return res;
    }

    kv_attributes_t attributes = kv_key_attributes(&current_key);
    if (((uint32_t)attributes & KV_ATTRIBUTE_PERMANENT) != 0) {
        return ERRCODE_SUCC;
    }

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (((uint32_t)attributes & KV_ATTRIBUTE_ENCRYPTED) != 0) {
        /* if old key is encrypted, the new key must be encrypted too */
        g_new_key_details.attributes = (kv_attributes_t)((uint32_t)g_new_key_details.attributes |
                                                         (uint32_t)KV_ATTRIBUTE_ENCRYPTED);
    }
#endif

    *kvalue_length = current_key.header.length;
    *old_kvalue = (uint8_t *)kv_malloc(current_key.header.length);
    if (*old_kvalue == NULL) {
        return ERRCODE_MALLOC;
    }

    res = kv_key_read_data(&current_key, *old_kvalue);
    if (res != ERRCODE_SUCC) {
        kv_free((void *)*old_kvalue);
    }
    return res;
}

/*
 * Attempt to claim access to hardware AES and hash functions
 */
STATIC kv_update_event_t kv_update_action_claim_crypto(void)
{
    kv_managed_write_buffer_t *write_buffer = g_current_state_machine->write_buffer;

    /* Reset write location */
    write_buffer->write_location = (uint32_t)(uintptr_t)g_current_key.key_location;

    /* Point current source buffer at first buffer and reset data_consumed values */
    g_current_state_machine->current_source = g_current_state_machine->source_buffer;
    kv_managed_source_buffer_t *source_buffer = g_current_state_machine->source_buffer;
    while (source_buffer != NULL) {
        source_buffer->data_consumed = 0;
        source_buffer = source_buffer->next;
    }

    write_buffer->crypto_handle = INVAILD_CRYPTO_HANDLE;

#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (((uint32_t)g_new_key_details.attributes & (uint32_t)KV_ATTRIBUTE_ENCRYPTED) != 0) {
        /* Claim cryptographic engines */
        if (!write_buffer->encrypt_claimed) {
            errcode_t res = nv_crypto_claim_aes(&write_buffer->crypto_handle, &(g_current_key.header));
            if (res != ERRCODE_SUCC) {
                g_current_state_machine->error_code = res;
                return EVENT_ERROR;
            }
            write_buffer->encrypt_claimed = true;
        }

#if (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_YES)
        if (!write_buffer->hash_claimed) {
            errcode_t res = nv_crypto_start_hash();
            if (res != ERRCODE_SUCC) {
                nv_crypto_release_aes(write_buffer->crypto_handle);
                write_buffer->crypto_handle = INVAILD_CRYPTO_HANDLE;
                write_buffer->encrypt_claimed = false;
                g_current_state_machine->error_code = res;
                return EVENT_ERROR;
            }
            write_buffer->crc_ret = 0;
            write_buffer->hash_claimed = true;
        }
#else
        if (!write_buffer->gcm_tag_claimed) {
            write_buffer->crc_ret = 0;
            write_buffer->gcm_tag_claimed = true;
        }
#endif /* #if (CONFIG_NV_SUPPORT_HASH_FOR_CRYPT == NV_YES) */
    } else {
#endif /* #if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES) */
        if (!write_buffer->crc_claimed) {
            write_buffer->crc_ret = 0;
            write_buffer->crc_claimed = true;
        }
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    }
#endif

    return EVENT_CRYPTO_CLAIMED;
}

/*
 * Attempts to erases all instances of g_current_key (in g_current_store), by marking them as invalid
 * Configures g_search_filter to find all valid keys with an id matching g_current_key.header.key_id
 * Starts the g_erase_keys_machine state machine to perform the erasing of keys
 */
STATIC kv_update_event_t kv_update_action_erase_old_keys(void)
{
    g_search_filter.pattern = g_current_key.header.key_id;
    g_search_filter.mask = 0xFFFF;
    g_search_filter.state = KV_KEY_FILTER_STATE_VALID;
    g_search_filter.type = KV_KEY_FILTER_TYPE_ANY;
    g_search_filter.location = g_current_key.key_location;

    errcode_t res = begin_state_machine(&g_erase_keys_machine);
    if (res == ERRCODE_SUCC) {
        return EVENT_NONE;
    } else {
        g_current_state_machine->error_code = res;
        return EVENT_ERROR;
    }
}

STATIC kv_update_event_t kv_update_action_update_nvmap_for_erase_key(void)
{
    kv_nvregion_map_t *nv_map = kv_nvregion_get_map();
    uint32_t page_number = kv_nvregion_get_page_number(g_current_page.page_location);
    if (page_number >= nv_map->num_entries) {
        nv_log_debug("[NV] get erase page_number failed! page_location = 0x%x\r\n", g_current_page.page_location);
        g_current_state_machine->error_code = ERRCODE_NV_INVALID_PAGE;
        return EVENT_ERROR;
    }

    uint16_t key_size = (uint32_t)sizeof(kv_key_header_t);
    uint16_t hash_crc_size = KV_CRYPTO_CRC_SIZE;
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (g_current_key.header.enc_key == AES_KDFKEY_SDRK_TYPE) {
        hash_crc_size = KV_CRYPTO_HASH_SIZE;
    }
#endif
    key_size += hash_crc_size;

    kv_attributes_t attributes = kv_key_attributes(&g_current_key);
    key_size += kv_key_padded_data_length(attributes, g_current_key.header.length);

    nv_page_status_map_t *page_status = &(nv_map->page_status_map[page_number]);
    page_status->reclaimable_space += key_size;
    nv_log_debug("[NV] update nv map for erase key = 0x%x (page = 0x%x) size = %d\r\n",
        g_current_key.header.key_id, g_current_page.page_location, key_size);
    return EVENT_PAGE_MAP_UPDATED;
}

STATIC kv_update_event_t kv_update_action_update_nvmap_for_new_key(void)
{
    kv_nvregion_map_t *nv_map = kv_nvregion_get_map();

    uint32_t page_number = kv_nvregion_get_page_number(g_current_page.page_location);
    if (page_number >= nv_map->num_entries) {
        nv_log_debug("[NV] get new key page_number failed! page_location = 0x%x\r\n", g_current_page.page_location);
        g_current_state_machine->error_code = ERRCODE_NV_INVALID_PAGE;
        return EVENT_ERROR;
    }

    uint16_t key_size = (uint32_t)sizeof(kv_key_header_t);
    uint16_t hash_crc_size = KV_CRYPTO_CRC_SIZE;
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
    if (((uint32_t)g_new_key_details.attributes & (uint32_t)KV_ATTRIBUTE_ENCRYPTED) != 0) {
        hash_crc_size = KV_CRYPTO_HASH_SIZE;
    }
#endif

    key_size += hash_crc_size;
    key_size += kv_key_padded_data_length(g_new_key_details.attributes, (uint16_t)g_new_key_details.kvalue_length);

    nv_page_status_map_t *page_status = &(nv_map->page_status_map[page_number]);
    page_status->used_space += key_size;
    page_status->first_writable_offset += key_size;
    nv_log_debug("[NV] update nv map for new key = 0x%x (page = 0x%x) size = %d\r\n",
        g_new_key_details.key_id, g_current_page.page_location, key_size);
    return EVENT_PAGE_MAP_UPDATED;
}

/*
 * Begins a new state machine by creating a new entry at the end of the active state machines list
 * and updating g_current_state_machine
 *
 * Either called to begin processing a *new* top-level state machine, before calling process_state_machine()
 * or called by a state machine action function to kick off the processing of a new (nested) state machine
 */
STATIC errcode_t begin_state_machine(const kv_state_machine_t *machine)
{
    kv_active_state_machine_t *new_state_machine =
        (kv_active_state_machine_t *)kv_zalloc(sizeof(kv_active_state_machine_t));
    if (new_state_machine == NULL) {
        return ERRCODE_MALLOC;
    }
    new_state_machine->prev = g_current_state_machine;

    /* Configure default initial state and event for new state machine */
    new_state_machine->machine = machine;
    new_state_machine->state = machine->initial_state;
    new_state_machine->event = EVENT_NONE;
    new_state_machine->error_code = ERRCODE_SUCC;
    g_current_state_machine = new_state_machine;

    if (g_current_state_machine->machine->write_buffer_size > 0) {
        return kv_create_write_buffer(0, g_current_state_machine->machine->write_buffer_size);
    } else {
        return ERRCODE_SUCC;
    }
}

/*
 * Should only be called by process_state_machine()
 *
 * Ends current state machine and updates g_current_state_machine to point to the previous state machine.
 * Passes either the pre-defined exit_event or the last event returned by an action function as the
 * event received by the previous state machine.
 */
STATIC void end_state_machine(void)
{
    if (g_current_state_machine != NULL) {
        kv_remove_write_buffer();
        kv_remove_source_buffer();

        kv_active_state_machine_t *prev_state_machine = g_current_state_machine->prev;
        if (prev_state_machine != NULL) {
            if (g_current_state_machine->machine->exit_event != EVENT_NONE) {
                /* Single explicit exit event defined for exiting state machine */
                prev_state_machine->event = g_current_state_machine->machine->exit_event;
            } else {
                /* Pass last event raised from exiting state machine back to invoking state machine */
                prev_state_machine->event = g_current_state_machine->event;
            }
        }

        kv_free(g_current_state_machine);
        g_current_state_machine = prev_state_machine;
    }
}

static void clean_state_machine(void)
{
    while (g_current_state_machine != NULL) {
        end_state_machine();
    }
}

/*
 * Should only be called by process_state_machine()
 *
 * Attempts to locate and call an action function based on the state of the current state machine
 * An action function may spawn another state machine and hence cause g_current_state_machine to change.
 *
 * g_current_state_machine->event is used to record the return value from the action function.
 *
 * If a new state machine is spawned, this return value will effectively be lost as the procedure for processing
 * new state machines is to assume there is no initial event, just an initial state and thus action to perform.
 *
 * If an action function is not found then g_current_state_machine->event will not be updated.  This really should
 * be reserved for when state transitions cause g_current_state_machine->state to be one of the following
 * special cases, which are handled by process_state_machine():
 *   - STATE_INVALID
 *   - STATE_SUSPENDED
 *   - STATE_EXIT
 */
static void invoke_current_state_action(void)
{
    if (g_current_state_machine != NULL) {
        /* Search action table to locate an action function for the current state */
        /* STATE_EXIT always marks end of an action table                         */
        const kv_update_action_t *update_action = g_current_state_machine->machine->action_table;
        while (update_action->state != STATE_EXIT) {
            if (update_action->state == g_current_state_machine->state) {
                /* Found a matching entry */
                break;
            }
            update_action++;
        }

        /* Call action function if we have found one */
        if (update_action->action != NULL) {
            /* Action function could cause a change of g_current_state_machine */
            g_current_state_machine->event = (*update_action->action)();
        }
    }
}

/*
 * Should only be called by process_state_machine()
 *
 * Uses the last event recorded for the state machine, in g_current_state_machine->event, to select the
 * next appropriate state for the current state machine.
 *
 * In the case of no suitable entry on the transition_table an error state of STATE_INVALID is entered.
 * This will cause process_state_machine() to terminate processing of all queued state machines and
 * return an error.
 */
STATIC void update_current_state(void)
{
    /* Don't update anything for EVENT_NONE.  We'll eventually call the action function again */
    if ((g_current_state_machine != NULL) && (g_current_state_machine->event != EVENT_NONE)) {
        /* Search transition table looking for a transition entry for current state and event */
        /* A transition table is always terminated with transition->state == STATE_EXIT       */
        const kv_update_transition_t *transition = g_current_state_machine->machine->transition_table;
        while (transition->state != STATE_EXIT) {
            if ((g_current_state_machine->state == transition->state) &&
                (g_current_state_machine->event == transition->event)) {
                /* Found a matching entry */
                break;
            }
            transition++;
        }

        /* Update current state if we have found a suitable transition table entry */
        if (transition->state != STATE_EXIT) {
            g_current_state_machine->state = transition->next_state;
        } else {
            /* No state transition found for the current state and event */
            g_current_state_machine->state = STATE_INVALID;
        }
    }
}

#if defined (DEBUG_PRINT_ENABLED)
static const char *g_update_state_strings[STATE_EXIT + 1] = {
    [STATE_INVALID] =             "STATE_INVALID",
    [STATE_SELECT_FIRST_PAGE] =   "STATE_SELECT_FIRST_PAGE",
    [STATE_SELECT_NEXT_PAGE] =    "STATE_SELECT_NEXT_PAGE",
    [STATE_FIND_FIRST_KEY] =      "STATE_FIND_FIRST_KEY",
    [STATE_FIND_NEXT_KEY] =       "STATE_FIND_NEXT_KEY",
    [STATE_FIND_EXISTING_KEY] =   "STATE_FIND_EXISTING_KEY",
    [STATE_PREP_COPY_KEY] =       "STATE_PREP_COPY_KEY",
    [STATE_PREP_DELETE_KEY] =     "STATE_PREP_DELETE_KEY",
    [STATE_PREP_MODIFY_KEY] =     "STATE_PREP_MODIFY_KEY",
    [STATE_FIND_DEFRAG] =         "STATE_FIND_DEFRAG",
    [STATE_ERASE_DEFRAG] =        "STATE_ERASE_DEFRAG",
    [STATE_PREP_DEFRAG] =         "STATE_PREP_DEFRAG",
    [STATE_COPY_ALL_KEYS] =       "STATE_COPY_ALL_KEYS",
    [STATE_PREP_PAGE_HEADER] =    "STATE_PREP_PAGE_HEADER",
    [STATE_WRITE_PAGE_HEADER] =   "STATE_WRITE_PAGE_HEADER",
    [STATE_UPDATE_NVREGION_MAP] = "STATE_UPDATE_NVREGION_MAP",
    [STATE_FIND_WRITE_POS] =      "STATE_FIND_WRITE_POS",
    [STATE_DEFRAG_PAGE] =         "STATE_DEFRAG_PAGE",
    [STATE_PREP_STORE] =          "STATE_PREP_STORE",
    [STATE_PREP_KEY_DATA] =       "STATE_PREP_KEY_DATA",
    [STATE_CLAIM_CRYPTO] =        "STATE_CLAIM_CRYPTO",
    [STATE_ERASE_OLD_KEYS] =      "STATE_ERASE_OLD_KEYS",
    [STATE_PRIME_WRITE] =         "STATE_PRIME_WRITE",
    [STATE_PERFORM_WRITE] =       "STATE_PERFORM_WRITE",
    [STATE_SUSPENDED] =           "STATE_SUSPENDED",
    [STATE_EXIT] =                "STATE_EXIT"
};

#if defined (DEBUG_KV_UPDATE_STATE_MACHINE)
static const char *g_update_event_strings[EVENT_ERROR + 1] = {
    [EVENT_NONE] =                  "EVENT_NONE",
    [EVENT_SUSPEND] =               "EVENT_SUSPEND",
    [EVENT_WRITE_DATA_EXHAUSTED] =  "EVENT_WRITE_DATA_EXHAUSTED",
    [EVENT_WRITE_BUFFER_PRIMED] =   "EVENT_WRITE_BUFFER_PRIMED",
    [EVENT_WRITE_BUFFER_STORED] =   "EVENT_WRITE_BUFFER_STORED",
    [EVENT_PAGE_SELECTED] =         "EVENT_PAGE_SELECTED",
    [EVENT_PAGE_NOT_SELECTED] =     "EVENT_PAGE_NOT_SELECTED",
    [EVENT_KEY_FOUND] =             "EVENT_KEY_FOUND",
    [EVENT_KEY_NOT_FOUND] =         "EVENT_KEY_NOT_FOUND",
    [EVENT_COPY_KEY_READY] =        "EVENT_COPY_KEY_READY",
    [EVENT_KEYS_ERASED] =           "EVENT_KEYS_ERASED",
    [EVENT_KEY_UPDATE_REQUIRED] =   "EVENT_KEY_UPDATE_REQUIRED",
    [EVENT_KEY_UPDATE_NOT_NEEDED] = "EVENT_KEY_UPDATE_NOT_NEEDED",
    [EVENT_DEFRAG_FOUND] =          "EVENT_DEFRAG_FOUND",
    [EVENT_DEFRAG_ERASED] =         "EVENT_DEFRAG_ERASED",
    [EVENT_DEFRAG_PREPARED] =       "EVENT_DEFRAG_PREPARED",
    [EVENT_ALL_KEYS_COPIED] =       "EVENT_ALL_KEYS_COPIED",
    [EVENT_PAGE_HEADER_READY] =     "EVENT_PAGE_HEADER_READY",
    [EVENT_NVREGION_MAP_UPDATED] =  "EVENT_NVREGION_MAP_UPDATED",
    [EVENT_DEFRAG_REQUIRED] =       "EVENT_DEFRAG_REQUIRED",
    [EVENT_DEFRAG_COMPLETE] =       "EVENT_DEFRAG_COMPLETE",
    [EVENT_WRITE_POS_FOUND] =       "EVENT_WRITE_POS_FOUND",
    [EVENT_STORE_READY] =           "EVENT_STORE_READY",
    [EVENT_KEY_DATA_READY] =        "EVENT_KEY_DATA_READY",
    [EVENT_CRYPTO_CLAIMED] =        "EVENT_CRYPTO_CLAIMED",
    [EVENT_WRITE_COMPLETE] =        "EVENT_WRITE_COMPLETE",
    [EVENT_ERROR] =                 "EVENT_ERROR"
};
#endif
#endif

static void dprint_state_machine(const char *str)
{
    unused(str);
#if defined (DEBUG_KV_UPDATE_STATE_MACHINE)
    if (g_current_state_machine != NULL) {
        nv_log_debug("%08X:  ", (uint32_t)g_current_state_machine);
        nv_log_debug("%s  ", g_update_state_strings[g_current_state_machine->state]);
        nv_log_debug("%s  ", str);
        nv_log_debug("%s\r\n", g_update_event_strings[g_current_state_machine->event]);
    } else {
        nv_log_debug("*** State Machine  %s ***\r\n", str);
    }
#endif
}

/*
 * Function used to process queued state machines
 *
 * There is only ever one state machine being processed at any one time.
 *
 * State machines are started (queued) by explicitly calling begin_state_machine() before this function is called.
 * State machines themselves can spawn a nested state machine by also calling begin_state_machine() in an action
 * function.
 * When started, a state machine will be in the initial_state defined for the state machine.
 *
 * State machines are ended normally by transitioning to STATE_EXIT or abnormally by transitioning to STATE_INVALID.
 * In such instances, process_state_machine(), will call end_state_machine() to tidy up.
 *
 * State machines can be suspended at any point by transitioning to STATE_SUSPENDED.
 * When resumed, by calling process_state_machine() again, they will start from the resume_state defined for the
 * state machine.
 *
 * g_current_state_machine points to the current active state machine
 *
 * When a state machine is processed for the first time, or resumed following suspension, it is assumed there is no
 * current event that exists for the state machine in this context.  An action function for the current state, which
 * will be defined by initial_state or resume_state, will provide an event to provide for further transitions through
 * the state machine.
 */
STATIC errcode_t process_state_machine(void)
{
    errcode_t error_code = ERRCODE_SUCC;
    uint64_t flash_access_time = 0;
    unused(flash_access_time);

    while (g_current_state_machine != NULL) {
        /* Call action function for the current state */
        dprint_state_machine("---");
        invoke_current_state_action();
        dprint_state_machine("-->");

        /* Check event returned by action function */
        if (g_current_state_machine->event == EVENT_ERROR) {
            /* Ripple back down chain of state machines and report error */
            error_code = g_current_state_machine->error_code;
            clean_state_machine();
            return error_code;
        }

        /* Transition state machine */
        update_current_state();
        dprint_state_machine("<--");

        /* Has the state machine finished? */
        while ((g_current_state_machine != NULL) && (g_current_state_machine->state == STATE_EXIT)) {
            invoke_current_state_action();  /* Exit state could have an action function if it needs to do more */
            end_state_machine();            /* than just end the current state machine                         */
            update_current_state();
            dprint_state_machine("<==");
        }

        /* Has state machine processing been suspended? */
        if ((g_current_state_machine != NULL) && (g_current_state_machine->state == STATE_SUSPENDED)) {
#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
            /* if in one-core system, the flash must be writen in one time */
            /* so once the suspend happened, clear the state_machine */
            clean_state_machine();
#else
            /* We need to suspend processing due to other cores waking up */
            /* Configure state machine to resume from the correct place */
            g_current_state_machine->state = g_current_state_machine->machine->resume_state;
            g_current_state_machine->event = EVENT_NONE;
#endif
            kv_release_crypto();
            return ERRCODE_NV_WRITE_VETOED;
        }

        /* Has the state machine entered an invalid state? */
        if ((g_current_state_machine != NULL) && (g_current_state_machine->state == STATE_INVALID)) {
#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
            clean_state_machine();
#endif
            kv_release_crypto();
            return ERRCODE_NV_STATE_INVALID;
        }
    }

    return error_code;
}

/* Creates a write buffer used to prepare data, gathered from one or more source buffers, for writing to flash */
STATIC errcode_t kv_create_write_buffer(uint32_t write_location, uint32_t size)
{
    if (size == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    if (g_current_state_machine->write_buffer != NULL) {
        return ERRCODE_FAIL;
    }

    uint8_t *data = (uint8_t *)kv_malloc(size);
    if (data == NULL) {
        return ERRCODE_MALLOC;
    }
    kv_managed_write_buffer_t *write_buffer =
        (kv_managed_write_buffer_t *)kv_zalloc(sizeof(kv_managed_write_buffer_t));
    if (write_buffer == NULL) {
        kv_free(data);
        return ERRCODE_MALLOC;
    }

    write_buffer->data = data;
    write_buffer->size = (uint16_t)size;
    write_buffer->data_consumed = (uint16_t)size;
    write_buffer->write_location = write_location;
    write_buffer->resume_location = write_location;
    g_current_state_machine->write_buffer = write_buffer;

    return ERRCODE_SUCC;
}

/* Removes the write buffer owned by the current state machine */
STATIC void kv_remove_write_buffer(void)
{
    if (g_current_state_machine->write_buffer != NULL) {
        kv_free(g_current_state_machine->write_buffer->data);
        kv_free(g_current_state_machine->write_buffer);
        g_current_state_machine->write_buffer = NULL;
    }
}

/* Every key consists of a header, data and a hash.  Multiple kv_managed_source_buffer_t are constructed to
 * manage these separate blocks of data.
 * They keep track of where the data is located and how much of it has
 * been written to flash.
 * These buffers are used in a scatter-gather type approach to feed a single write buffer that holds data
 * that has been fully prepared for writing to flash */
STATIC kv_managed_source_buffer_t* kv_update_source_buffer(uint8_t *data, uint32_t data_length)
{
    kv_managed_source_buffer_t *source_buffer = kv_zalloc(sizeof(kv_managed_source_buffer_t));
    if (source_buffer == NULL) {
        return NULL;
    }

    source_buffer->data = data;
    source_buffer->data_length = (uint16_t)data_length;
    source_buffer->next = NULL;

    /* Append new buffer to end of list */
    kv_managed_source_buffer_t **next_buffer = &g_current_state_machine->source_buffer;
    while (*next_buffer != NULL) {
        next_buffer = &(*next_buffer)->next;
    }
    *next_buffer = source_buffer;
    return source_buffer;
}

/* Removes kv_managed_source_buffer_t data structures, freeing memory holding actual data too, if necessary */
STATIC void kv_remove_source_buffer(void)
{
    kv_managed_source_buffer_t *current = g_current_state_machine->source_buffer;
    while (current != NULL) {
        kv_managed_source_buffer_t *next = current->next;
        if (current->release_data) {
            kv_free(current->data);
        }
        kv_free(current);
        current = next;
    }
    g_current_state_machine->source_buffer = NULL;
}

/* Releases claim over hardware AES and hash functions */
STATIC void kv_release_crypto(void)
{
    if (g_current_state_machine == NULL) {
        return;
    }
    kv_managed_write_buffer_t *write_buffer = g_current_state_machine->write_buffer;
    uint32_t status;

    if (write_buffer != NULL) {
        status = osal_irq_lock();
        if (write_buffer->crc_claimed) {
            write_buffer->crc_ret = 0;
            write_buffer->crc_claimed = false;
        }
#if (CONFIG_NV_SUPPORT_ENCRYPT == NV_YES)
        if (write_buffer->hash_claimed) {
            write_buffer->hash_claimed = false;
        }
        if (write_buffer->gcm_tag_claimed) {
            write_buffer->gcm_tag_claimed = false;
        }
        if (write_buffer->encrypt_claimed) {
            nv_crypto_release_aes(write_buffer->crypto_handle);
            write_buffer->crypto_handle = INVAILD_CRYPTO_HANDLE;
            write_buffer->encrypt_claimed = false;
        }
#endif
        osal_irq_restore(status);
    }
}

STATIC bool active_state_machine(void)
{
    if (g_current_state_machine != NULL) {
        return true;
    }
    return false;
}

STATIC uint32_t determine_flash_task_state_code(errcode_t res)
{
    switch (res) {
        case ERRCODE_NV_WRITE_VETOED:
        case ERRCODE_FLASH_TASK_PE_VETO:
            return FLASH_TASK_BEING_PROCESSED;
        case ERRCODE_SUCC:
            return FLASH_TASK_COMPLETED;
        default:
            return (uint32_t)res;
    }
}

#if defined(CONFIG_PARTITION_FEATURE_SUPPORT)
STATIC errcode_t nv_read_partition_addr(partition_ids_t partition_nv_id, uint32_t *start_address, uint32_t *size)
{
    /* 获取分区表标记 */
    errcode_t ret;
    partition_information_t info;

    ret = uapi_partition_get_info(partition_nv_id, &info);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    *start_address = info.part_info.addr_info.addr + FLASH_PHYSICAL_ADDR_START;
    *size = info.part_info.addr_info.size;

    return ERRCODE_SUCC;
}
#endif

errcode_t kv_update_init(cores_t core)
{
    unused(core);

    errcode_t res;
    uint32_t kv_start_addr = 0;
    uint32_t kv_size = 0;
    uint32_t kv_backup_start_addr = 0;
    uint32_t kv_backup_size = 0;
#if defined(CONFIG_PARTITION_FEATURE_SUPPORT)
    res = nv_read_partition_addr(PARTITION_NV_DATA, &kv_start_addr, &kv_size);
    if (res != ERRCODE_SUCC) {
        return res;
    }
    kv_backup_start_addr = kv_start_addr + KV_STORE_DATA_SIZE;
    kv_backup_size = KV_BACKUP_DATA_SIZE;
#if defined(CONFIG_NV_SUPPORT_BACKUP_REGION) && (CONFIG_NV_SUPPORT_BACKUP_REGION == NV_YES)
    res = nv_read_partition_addr(PARTITION_NV_DATA_BACKUP, &kv_backup_start_addr, &kv_backup_size);
    if (res != ERRCODE_SUCC) {
        return res;
    }
#endif
#endif
    if (kv_start_addr != 0 && kv_size != 0) {
        kv_nvregion_init(kv_start_addr, KV_STORE_DATA_SIZE, kv_backup_start_addr, kv_backup_size);
    } else {
        kv_nvregion_init(KV_STORE_START_ADDR, KV_STORE_DATA_SIZE, KV_BACKUP_START_ADDR, KV_BACKUP_DATA_SIZE);
    }

    /* Scan NV region looking for KV pages */
    res = kv_nvregion_scan();
    if (res != ERRCODE_SUCC) {
        return res;
    }

    /* Check expected KV pages have been found */
    for (uint8_t store = (uint8_t)KV_STORE_APPLICATION; store < (uint8_t)KV_STORE_MAX_NUM; store++) {
        uint16_t store_id = kv_store_get_id(store);
        uint8_t pages_num = kv_store_get_page_count(store);
        for (uint8_t page_index = 0; page_index < pages_num; page_index++) {
            res = kv_nvregion_find_page(store_id, page_index, NULL, NULL);
            if (res != ERRCODE_SUCC) {
                /* NV Region does not contain an expected KV page, attempt to create it */
                res = kv_nvregion_create_page(store_id, page_index);
            }
            if (res != ERRCODE_SUCC) {
                return ERRCODE_FAIL;
            }
        }
    }
    return ERRCODE_SUCC;
}

#if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES)
STATIC errcode_t kv_updata_backup_page_head_is_valid(const kv_page_header_t *backup_head)
{
    if ((backup_head->details.store_id == KV_STORE_ID_BACKUP) &&
        ((~backup_head->inverted_details_word) == *(uint32_t *)(uintptr_t)(&backup_head->details)) &&
        ((~backup_head->inverted_sequence_number) == backup_head->sequence_number)) {
            return ERRCODE_SUCC;
    }
    return ERRCODE_FAIL;
}

STATIC errcode_t kv_updata_backup_fail_process(uint32_t back_maybe_need_process_location)
{
    errcode_t res;
    kv_page_location unused_page_location = NULL;
    uint32_t page_head_size = (uint32_t)sizeof(kv_page_header_t);
    kv_nvregion_area_t* nvregion_area =  nv_get_region_area();
    if (nvregion_area == NULL) {
        return ERRCODE_FAIL;
    }
    /* 去找工作区可以换页的页，然后将可以换页的页地址给传出 */
    res = kv_nvregion_find_unused_page(&unused_page_location);
    if (res != ERRCODE_SUCC) {
        return res;
    }
    /* 获取工作区换页页的页头  */
    kv_page_header_t store_head_buffer;
    res = kv_key_helper_copy_flash((uint32_t)(uintptr_t)&store_head_buffer,
        (uint32_t)(uintptr_t)unused_page_location, (uint16_t)page_head_size);
    if (res != ERRCODE_SUCC) {
        return res;
    }
    /* 如果页头中包含 KV_STORE_ID_BACKUP 并且该page_location与maybe_need_process_location时都指向同一页 那么说明存在掉电丢数据的情况 */
    if (kv_updata_backup_page_head_is_valid(&store_head_buffer) == ERRCODE_SUCC) {
        uint32_t back_page_location = nvregion_area->nv_backup_addr +
        store_head_buffer.details.page_index * KV_PAGE_SIZE;
        if (back_page_location == back_maybe_need_process_location) {
            res = kv_key_erase_flash(back_page_location, KV_PAGE_SIZE);
            if (res != ERRCODE_SUCC) {
                return res;
            }
            res = kv_backup_copy_unused_page_to_dragpage(back_page_location, (uint32_t)(uintptr_t)unused_page_location);
            if (res != ERRCODE_SUCC) {
                return res;
            }
        }
    }
    return ERRCODE_SUCC;
}

errcode_t kv_update_backup_init(void)
{
    /* Initializes the page header of the backup area */
    errcode_t res;
    uint32_t backup_not_inited_count = 0; /* 重启时备份区没有初始化的数量为0 */
    uint32_t back_maybe_need_process_location = 0;
    kv_nvregion_area_t* nvregion_area =  nv_get_region_area();
    if (nvregion_area == NULL) {
        return ERRCODE_FAIL;
    }
    uint32_t back_page_location = nvregion_area->nv_backup_addr;
    kv_page_header_t backup_head_buffer;
    for (uint32_t page_index = 0; page_index < KV_BACKUP_PAGE_NUM; page_index++) {
        res = kv_key_helper_copy_flash((uint32_t)(uintptr_t)&backup_head_buffer,
            (uint32_t)back_page_location, sizeof(kv_page_header_t));
        if (res != ERRCODE_SUCC) {
            return res;
        }
        if (kv_updata_backup_page_head_is_valid(&backup_head_buffer) == ERRCODE_FAIL) {
            backup_not_inited_count++;
            res = kv_nvregion_write_page((kv_page_location)(uintptr_t)back_page_location,
                                         KV_STORE_ID_BACKUP, (uint8_t)page_index);
            if (res != ERRCODE_SUCC) {
                return res;
            }
            back_maybe_need_process_location = back_page_location;
        }
        back_page_location += KV_PAGE_SIZE;
    }

    /* 如果备份区没有初始化的页的数量为0, 说明备份区页不存在异常，正常重启 */
    /* 如果备份区未初始化的页的数量大于1, 说明是刚刚烧录状态，对备份区页头的初始化 */
    /* 如果备份区没有初始化的页的数量为1, 那么说明可能存在掉电异常场景进行处理 */
    if (backup_not_inited_count == 1) {
        res = kv_updata_backup_fail_process(back_maybe_need_process_location);
        if (res != ERRCODE_SUCC) {
            return res;
        }
    }
    return res;
}
#endif /* #if (CONFIG_NV_SUPPORT_BACKUP_RESTORE == NV_YES) */


/* Top level function used to mark a single key in a store as invalid (and hence erased) */
/* Will actually scan a store for all keys matching the specified key_id, to ensure they are all marked as erased */
errcode_t kv_update_erase_key(kv_store_t core, flash_task_node *sanitised_tasks)
{
    if (!active_state_machine()) {
        g_current_store = core;

        /* Configure search filter to obtain all valid instances of a specific key_id */
        g_search_filter.pattern = sanitised_tasks->data.kv_erase.key;
        g_search_filter.mask = 0xFFFF;
        g_search_filter.state = KV_KEY_FILTER_STATE_VALID;
        g_search_filter.type = KV_KEY_FILTER_TYPE_ANY;
        g_search_filter.location = 0;

        /* Queue Erase Keys state machine for processing */
        begin_state_machine(&g_erase_keys_machine);
    }

    /* Run state machine until it is either suspended (yielding due to flash contention) or */
    /* completed (either successfully or otherwise)                                         */
    sanitised_tasks->state_code = determine_flash_task_state_code(process_state_machine());
#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
    if (sanitised_tasks->state_code == FLASH_TASK_COMPLETED) {
        return ERRCODE_SUCC;
    } else {
        return (errcode_t)sanitised_tasks->state_code;
    }
#else
    return ERRCODE_SUCC;
#endif
}

/* Top level function to write a new key or modify an existing key-value */
/* Will cause a new instance of the key to be generated */
errcode_t kv_update_write_key(kv_store_t core, flash_task_node *sanitised_task)
{
    if (!active_state_machine()) {
        g_current_store = core;

        /* Log details of new key to be written */
        g_new_key_details.key_id = sanitised_task->data.kv.key;
        g_new_key_details.kvalue = sanitised_task->data.kv.kvalue;
        g_new_key_details.kvalue_length = sanitised_task->data.kv.kvalue_length;
        g_new_key_details.attributes = sanitised_task->data.kv.attribute;
        g_new_key_details.focre_write = sanitised_task->data.kv.force_write;

        /* Queue Write Key state machine for processing */
        begin_state_machine(&g_write_key_machine);
    }

    /* Run state machine until it is either suspended (yielding due to flash contention) or */
    /* completed (either successfully or otherwise)                                         */
    sanitised_task->state_code = determine_flash_task_state_code(process_state_machine());
#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
    if (sanitised_task->state_code == FLASH_TASK_COMPLETED) {
        return ERRCODE_SUCC;
    } else {
        return (errcode_t)sanitised_task->state_code;
    }
#else
    return ERRCODE_SUCC;
#endif
}

/* Modify the attribute on an existing key-value */
/* Top level function to modify the attributes of an existing key */
/* Will cause a new instance of the key to be generated */
/* Existing key attributes will be maintained */
errcode_t kv_update_modify_attribute(kv_store_t core, flash_task_node *sanitised_task)
{
    uint8_t *old_kvalue = NULL;
    errcode_t res;
    if (!active_state_machine()) {
        g_current_store = core;
        /* Log details of *additional* attributes to be applied to an existing key */
        g_new_key_details.key_id = sanitised_task->data.kv_attribute.key;
        g_new_key_details.kvalue = NULL;
        g_new_key_details.kvalue_length = 0;
        g_new_key_details.attributes = sanitised_task->data.kv_attribute.attribute;
        g_new_key_details.focre_write = false;

        res = kv_update_helper_get_current_key(&old_kvalue, &g_new_key_details.kvalue_length);
        if (res != ERRCODE_SUCC) {
            return res;
        }
        g_new_key_details.kvalue = old_kvalue;
        /* Queue Modify Key state machine for processing */
        begin_state_machine(&g_write_key_machine);
    }

    /* Run state machine until it is either suspended (yielding due to flash contention) or */
    /* completed (either successfully or otherwise)                                         */
    sanitised_task->state_code = determine_flash_task_state_code(process_state_machine());
#ifdef CONFIG_NV_SUPPORT_SINGLE_CORE_SYSTEM
    kv_free((void *)old_kvalue);
    if (sanitised_task->state_code == FLASH_TASK_COMPLETED) {
        return ERRCODE_SUCC;
    } else {
        return (errcode_t)sanitised_task->state_code;
    }
#else
    if (sanitised_task->state_code != FLASH_TASK_BEING_PROCESSED) {
        kv_free((void *)old_kvalue);
    }
    return ERRCODE_SUCC;
#endif
}

