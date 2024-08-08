/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: OSAL Error Definition.
 */

#ifndef OSAL_ERRNO_H_
#define OSAL_ERRNO_H_

#define OS_HWI_PRIO_IGNORE 6

/* Basic common errno 1~34 */
#define OSAL_EINTR (-4)  /* Interrupted system call */
#define OSAL_EINVAL (-22)  /* Interrupted system call */
#define OSAL_ETIME (-62)  /* Timer expired */
#define OSAL_EOVERFLOW (-75)  /* Value too large for defined data type */

#define OSAL_ERESTARTSYS 512

#ifndef ERESTARTSYS
#define ERESTARTSYS 512
#endif

#define OSAL_NONEOS_DEFAULT_RET (-200) /* NoneOS's default return value */
#ifndef EOK
#define EOK 0
#endif

#endif /* OSAL_ERRNO_H_ */
