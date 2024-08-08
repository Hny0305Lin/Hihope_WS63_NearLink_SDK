/**
 * @defgroup string String
 * @ingroup linux
 */

#ifndef _LINUX_STRING_H
#define _LINUX_STRING_H

#include_next "string.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup string
 * @brief Copy strings.
 *
 * @par Description:
 * This API is used to copy strings securely.
 *
 * @attention
 * None.
 *
 * @param  dest   [IN] The pointer to the destination address.
 * @param  src    [IN] The pointer to the source address.
 * @param  size   [IN] Size of the dest.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>string.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern size_t strlcpy(char *dest, const char *src, size_t size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_STRING_H */
