/**
 * @defgroup checksum Checksum
 * @ingroup linux
 */

#ifndef _LINUX_CHECKSUM_H
#define _LINUX_CHECKSUM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup checksum
 * @brief Computes 32-bit intermediate sums.
 *
 * @par Description:
 * This API is used to calculates the checksum of the buff, length len, and incorporates the result into the input
 * parameter "wsum".
 * @attention
 * None.
 *
 * @param buf   [IN]he pointer to the data address.
 * @param len   [IN]The number of bits to be checksum.
 * @param wsum  [IN]The initial sum added to the results.
 *
 * @retval unsigned int   Checksum of the buffer.
 * @par Dependency:
 * <ul><li>checksum.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
unsigned int csum_partial(const void *buf, int len, unsigned int wsum);

/**
 * @ingroup checksum
 * @brief Copy and checksum.
 *
 * @par Description:
 * This API is used to calculate the checksum of the buff, length len, incorporates the result into the input
 * parameter "wsum", and copy the data.
 * @attention
 * None.
 *
 * @param src   [IN]The pointer to the source address.
 * @param dst   [IN]The pointer to the destination address.
 * @param len   [IN]The number of bits to be copied.
 * @param wsum  [IN]The initial sum added to the results.
 *
 * @retval unsigned int   Checksum of the buffer.
 * @par Dependency:
 * <ul><li>checksum.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
unsigned int csum_partial_copy_nocheck(const void *src, void *dst, int len, unsigned int wsum);

/**
 * @ingroup checksum
 * @brief checksum.
 *
 * @par Description:
 * This API is used to calculate the checksum.
 * @attention
 * <ul>
 * <li>This function is for LWIP_CHKSUM.</li>
 * </ul>
 *
 * @param buf   [IN]The pointer to the source address.
 * @param len   [IN]The length of buff.
 *
 * @retval unsigned int   Checksum of the buffer.
 * @par Dependency:
 * <ul><li>checksum.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
unsigned short in_cksum(const void *buf, int len);

/**
 * @ingroup checksum
 * @brief Copy and checksum.
 *
 * @par Description:
 * This API is used to calculate the checksum and copy the data.
 * @attention
 * <ul>
 * <li>This function is for LWIP_CHKSUM_COPY.</li>
 * </ul>
 *
 * @param src   [IN]The pointer to the source address.
 * @param dst   [IN]The pointer to the destination address.
 * @param len   [IN]The number of bits to be copied.
 *
 * @retval unsigned int   Checksum of the buffer.
 * @par Dependency:
 * <ul><li>checksum.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
unsigned short in_cksum_copy(const void *src, void *dst, int len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
