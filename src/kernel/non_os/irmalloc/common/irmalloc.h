/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:   malloc function
 * Author:
 * Create:
 */

#ifndef IRMALLOC_H
#define IRMALLOC_H

#include "std_def.h"

/**
 * @defgroup connectivity_libs_irmalloc IRMALLOC
 * @ingroup  connectivity_libs
 * @{
 */
// for IAR compiler
#ifdef __cplusplus
extern "C" {
#endif

/** Initialise the internal memory with the default pool passed in via the linker */
void irmalloc_init_default(void);

/**
 * @brief  Add an additional memory pool to allocate from; the irmalloc implementation is not obliged to use this memory
 * @param  start is the start memory address of the new pool
 * @param  size is number of bytes in the new memory pool
 */
void irmalloc_new_pool(unsigned char *start, size_t size);

/**
 * @brief  Allocate a block of memory from internal RAM
 * @param  size size of the block to allocate
 * @return pointer to allocated memory, or NULL
 */
//lint -function(malloc,irmalloc)   irmalloc has the same semantics as malloc.
void *irmalloc(size_t size);


void *irmemalign(size_t boundary, size_t size);

/**
 * @brief  Allocate a block of memory from internal RAM or panic on failure
 * @param  size size of the block to allocate
 * @return pointer to allocated memory
 */
//lint -function(malloc(0),irmalloc_panicking)   irmalloc has the same semantics as malloc, but doesn't return NULL.
//lint -function(malloc(1),irmalloc_panicking)   irmalloc has the same semantics as malloc, but doesn't return NULL.
void *irmalloc_panicking(size_t size);

/**
 * @brief  Allocate a block of memory and zero it.
 * @param  size size of the block to allocate
 * @return pointer to allocated memory, or NULL
 */
//lint -function(malloc,irzalloc)   irzalloc has the same semantics as malloc.
void *irzalloc(size_t size);

/**
 * @brief  Allocate a block of memory and zero it.
 * @param  size size of each block to allocate
 * @param  number number of blocks to allocate
 * @return pointer to allocated memory, or NULL
 */
#define ircalloc(size, number) irzalloc((size) * (number))

/**
 * @brief  Free a block of memory
 * @param  buf
 */
//lint -function(free,irfree)   irfree has the same semantics as free.
void irfree(void *buf);

/**
 * @brief  Duplicate the contents of a block of memory into irmalloc.
 * @param  orig pointer to memory to duplicate
 * @param  size size of the block to duplicate
 * @return pointer to block contain
 */
void *irmalloc_dup(const void *orig, size_t size);

/**
 * @brief  Reallocate a block of memory
 */
//lint -function(realloc,irrealloc)  irrealloc has the same semantics as realloc.
void *irrealloc(void *buf, size_t size);

/** Reallocate a block of memory or panic on failure */
// irrealloc_panicking has the same semantics as realloc, but doesn't return NULL.
void *irrealloc_panicking(void *buf, size_t size);

/**
 * @brief  Get some human readable statistics on the memory management state
 * @return the irmalloc'ed memory buffer containing a NULL terminated ASCII string of the human readbale stats;
 * this memory must be irfree'd by the calling code
 * note The stats format is variable and is not guaranteed to be parsable.NULL is returned if no memory is irmallocable
 */
char *irmalloc_get_human_readable_stats(void);

/**
 * @brief  Get some macine readable statistics on the memory management state
 * @param  current_allocated pointer to memory to hold current allocated size
 * @param  total_free pointer to memory to hold current free pool size
 * @param  max_free pointer to memory to hold maximum free block size
 * @param  num_allocs pointer to memory to hold accumulated number of mallocs
 * @param  num_frees pointer to memory to hold accumulated number of frees
 */
void irmalloc_get_machine_readable_stats(uint32_t *current_allocated, uint32_t *total_free,
    uint32_t *max_free, uint32_t *num_allocs, uint32_t *num_frees);

#ifdef __cplusplus
}
#endif
/**
 * @}
 */
#endif
