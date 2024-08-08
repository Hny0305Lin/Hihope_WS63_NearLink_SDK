/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides sdk version message \n
 *
 * History: \n
 * 2022-05-12, Create file. \n
 */

#ifndef SDK_VERSION_H
#define SDK_VERSION_H

#include "stdint.h"

/**
 * @defgroup middleware_utils_version Version
 * @ingroup  middleware_utils
 * @{
 */

/**
 * @if Eng
 * @brief  obtain sdk id string
 * @retval sdk id string
 * @endif
 */
const char *uapi_sdk_read_id(void);

/**
 * @}
 */

#endif /* SDK_VERSION_H */
