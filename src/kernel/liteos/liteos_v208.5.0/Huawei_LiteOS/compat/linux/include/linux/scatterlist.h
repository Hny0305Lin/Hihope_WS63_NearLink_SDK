/**
 * @defgroup scatterlist Scatterlist
 * @ingroup linux
 */

#ifndef _LINUX_SCATTERLIST_H
#define _LINUX_SCATTERLIST_H

#include "linux/string.h"
#include "linux/kernel.h"
#include "types.h"
#include "asm/bug.h"
#include "los_printf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef unsigned long dma_addr_t;
typedef unsigned int uint;

typedef struct scatterlist {
#ifdef CONFIG_DEBUG_SG
    ulong       sg_magic;           /**< Used for memory exception check. */
#endif
    ulong       page_link;          /**< Indicates the page on which the memory block resides. */
    uint        offset;             /**< Indicates the offset of the memory block in the page. */
    uint        length;             /**< Length of the memory block. */
    dma_addr_t  dma_address;        /**< Actual start address of the memory block. */
#ifdef CONFIG_NEED_SG_DMA_LENGTH
    uint        dma_length;         /**< The length information of a memory block.(not used) */
#endif
} scatterlist_t;

/* Used for memory exception check. */
#define COMPAT_SG_MAGIC    0x87654321
#define SG_MAGIC           COMPAT_SG_MAGIC

 /**
 * @ingroup  scatterlist
 * @brief Mark the end of the scatterlist.
 *
 * @par Description:
 * This API is used to mark the end of the scatterlist.
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param  psg [IN/OUT] SG entryScatterlist.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>scatterlist.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
static inline void sg_mark_end(scatterlist_t *psg)
{
#ifdef CONFIG_DEBUG_SG
    BUG_ON(psg->sg_magic != COMPAT_SG_MAGIC);
#endif
    /* Set termination bit, clear potential chain bit */
    psg->page_link |= 0x02U;
    psg->page_link &= ~0x01U;
}

 /**
 * @ingroup  scatterlist
 * @brief Initialize the sg table.
 *
 * @par Description:
 * This API is used to initialize the sg table.
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param  psgl  [IN/OUT] The SG table.
 * @param  nents [IN] Number of entries in table.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>scatterlist.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
static inline void sg_init_table(scatterlist_t *psgl, unsigned int nents)
{
    (void)memset(psgl, 0, sizeof(scatterlist_t) * nents);

    sg_mark_end(&psgl[nents - 1]);
}

 /**
 * @ingroup  scatterlist
 * @brief Point the #dma_address of the SG table to the #buf.
 *
 * @par Description:
 * This API is used to point the #dma_address of the SG table to the #buf.
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param  psg      [IN/OUT] The SG table.
 * @param  buf      [IN] Point to the data.
 * @param  buflen   [IN] Length of buf.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>scatterlist.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
static inline void sg_set_buf(scatterlist_t *psg, const void *buf, unsigned int buflen)
{
    psg->dma_address = (uintptr_t)buf;
    psg->offset = 0;
    psg->length = buflen;
}

 /**
 * @ingroup  scatterlist
 * @brief Initialize the sg table and set SG table point at #buf.
 *
 * @par Description:
 * This API is used to initialize the sg table and set SG table point at #buf.
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param  psg      [IN/OUT] The SG table.
 * @param  buf      [IN] Point to the data.
 * @param  buflen   [IN] Length of buf.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>scatterlist.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
static inline void sg_init_one(scatterlist_t *psg, const void *buf, unsigned int buflen)
{
    sg_init_table(psg, 1);
    sg_set_buf(psg, buf, buflen);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_SCATTERLIST_H */
