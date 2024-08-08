/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dfx mem
 * This file should be changed only infrequently and with great care.
 */

#include "dfx_mem.h"
#include "errcode.h"
#include "los_memory.h"

errcode_t dfx_mem_get_sys_pool_info(mdm_mem_info_t *info)
{
    LOS_MEM_POOL_STATUS pool_info;

    if (LOS_MemInfoGet(m_aucSysMem0, &pool_info) != 0) {
        return ERRCODE_FAIL;
    }

    info->total = pool_info.uwTotalUsedSize + pool_info.uwTotalFreeSize;
    info->used = pool_info.uwTotalUsedSize;
    info->free = pool_info.uwTotalFreeSize;
    info->max_free_node_size = pool_info.uwMaxFreeNodeSize;
    info->used_node_num = pool_info.uwUsedNodeNum;
    info->free_node_num = pool_info.uwFreeNodeNum;
#ifdef LOSCFG_MEM_TASK_STAT
    info->peek_size = pool_info.uwUsageWaterLine;
#endif

    return ERRCODE_SUCC;
}
