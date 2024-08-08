/**
* @file soc_os_stat.h
*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2019. All rights reserved.  \n
* Description: OS status APIs.   \n
* Author:
* Create: 2019-12-18
*/

/**
 * @defgroup os_stat OS Status
 * @ingroup osa
 */

#ifndef DFX_TASK_H
#define DFX_TASK_H


#include <stdint.h>
#include "dfx_os_st.h"
#include "errcode.h"

uint32_t dfx_os_get_task_cnt(void);

/**
* @ingroup  iot_task
* @brief  Obtains the task information.CNcomment:获取任务信息。CNend
*
* @par 描述:
*           Obtains the task information.CNcomment:获取任务信息。CNend
*
* @attention None
* @param  taskid      [IN] type #uint32_t，task ID. CNcomment:任务ID号。CNend
* @param  inf        [OUT] type #task_info_t* ，task information.For details about the task status,
*                          see the header file los_task_pri.h.
CNcomment:任务信息。任务状态具体信息请参考头文件los_task_pri.h。CNend
*
* @retval #ERRCODE_SUCC Success
* @retval #Other Failure. For details, see errcode.h.
* @par 依赖:
*           @li soc_os_stat.h：Describes the task APIs.CNcomment:文件用于描述任务相关接口。CNend
* @see  None
*/
errcode_t dfx_os_get_all_task_info(task_info_t *inf, uint32_t info_cnt);
errcode_t dfx_os_get_task_info(uint32_t taskid, task_info_t *info);

const char *dfx_os_task_status_convert_str(uint16_t task_status);

#endif
