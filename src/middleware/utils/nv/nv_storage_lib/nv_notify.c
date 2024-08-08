/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022. All rights reserved.
 * Description: KV NOTIFY MODULE -- UTILS.
 */

#include "nv_notify.h"
#include "nv_storage.h"
#include "nv_porting.h"

#if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES)

bool nv_change_notify_segment_is_valid(nv_direct_ctrl_t *nv_ctrl, uint16_t min_key, uint16_t max_key)
{
    nv_changed_proc_t *notify_list = nv_ctrl->nv_change_notify_list;
    for (uint8_t i = 0; i < nv_ctrl->notify_registered_nums; i++) {
        if ((min_key < notify_list[i].min_key && max_key < notify_list[i].min_key) ||
            (min_key > notify_list[i].max_key && max_key > notify_list[i].max_key)) {
            continue;
        }
        return false;
    }
    return true;
}

errcode_t nv_direct_notify_list_init(void)
{
    nv_direct_ctrl_t *nv_ctrl = nv_direct_get_nv_ctrl();
    nv_ctrl->notify_regitser_max_nums = MCORE_REGISTER_NV_NOTIFY_MAX_NUM;
    nv_ctrl->notify_registered_nums = 0;
    uint8_t notify_func_nums = nv_ctrl->notify_regitser_max_nums;
    nv_changed_proc_t **notify_list = &nv_ctrl->nv_change_notify_list;
    if (*notify_list != NULL) {
        kv_free(*notify_list);
        *notify_list = NULL;
    }

    *notify_list = (nv_changed_proc_t *)kv_malloc(notify_func_nums * sizeof(nv_changed_proc_t));
    if (*notify_list == NULL) {
        return ERRCODE_MALLOC;
    }
    return ERRCODE_SUCC;
}


void nv_change_notify(uint16_t key)
{
    nv_direct_ctrl_t *nv_ctrl = nv_direct_get_nv_ctrl();
    nv_changed_proc_t *notify_list = nv_ctrl->nv_change_notify_list;
    if (notify_list == NULL) {
        return;
    }

    for (uint8_t index = 0; index < nv_ctrl->notify_registered_nums; index++) {
        if ((key < notify_list[index].min_key) || (key > notify_list[index].max_key)) {
            continue;
        }

        /* There may have the same range, this would cause multiple calls to the functions. */
        if (notify_list[index].func != NULL) {
            notify_list[index].func(key);
        }
    }
}
#endif /* #if (CONFIG_NV_SUPPORT_CHANGE_NOTIFY == NV_YES) */
