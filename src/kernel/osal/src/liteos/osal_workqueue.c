/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: workqueue
 *
 * Create: 2021-12-16
 */

#include <linux/workqueue.h>
#include "soc_osal.h"
#include "osal_errno.h"
#include "osal_inner.h"

OSAL_LIST_HEAD(g_wq_list);
struct wq_node {
    osal_workqueue *osal_work;
    struct work_struct *work;
    struct osal_list_head node;
};

static osal_workqueue *osal_find_work(const struct work_struct *work)
{
    struct osal_list_head *cur = NULL;

    if (work == NULL) {
        osal_log("parameter invalid!\n");
        return NULL;
    }

    if (osal_list_empty(&g_wq_list) != 0) {
        osal_log("g_wq_list is empty!\n");
        return NULL;
    }
    osal_list_for_each(cur, &g_wq_list)
    {
        struct wq_node *ws = osal_list_entry(cur, struct wq_node, node);
        if (ws->work == work) {
            return ws->osal_work;
        }
    }
    osal_log("find work failed!\n");
    return NULL;
}

static int osal_del_work(const struct work_struct *work)
{
    struct osal_list_head *cur = NULL;
    struct osal_list_head *next = NULL;

    if (work == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }

    if (osal_list_empty(&g_wq_list) != 0) {
        osal_log("g_wq_list is empty!\n");
        return OSAL_FAILURE;
    }
    osal_list_for_each_safe(cur, next, &g_wq_list)
    {
        struct wq_node *ws = osal_list_entry(cur, struct wq_node, node);
        if (ws->work == work) {
            osal_list_del(cur);
            LOS_MemFree((void *)m_aucSysMem0, (void *)ws);
            return OSAL_SUCCESS;
        }
    }
    osal_log("find work failed!\n");
    return OSAL_FAILURE;
}

static void osal_work_handler(struct work_struct *work)
{
    osal_workqueue *ow = osal_find_work(work);
    if ((ow != NULL) && (ow->handler != NULL)) {
        ow->handler(ow);
    }
}

int osal_workqueue_init(osal_workqueue *work, osal_workqueue_handler handler)
{
    struct work_struct *w = NULL;
    struct wq_node *w_node = NULL;

    if (work == NULL || work->work != NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }

    w = LOS_MemAlloc((void *)m_aucSysMem0, sizeof(struct work_struct));
    if (w == NULL) {
        osal_log("LOS_MemAlloc failed!\n");
        return OSAL_FAILURE;
    }

    w_node = LOS_MemAlloc((void *)m_aucSysMem0, sizeof(struct wq_node));
    if (w_node == NULL) {
        osal_log("LOS_MemAlloc failed!\n");
        LOS_MemFree((void *)m_aucSysMem0, (void *)w);
        return OSAL_FAILURE;
    }
    INIT_WORK(w, osal_work_handler);
    work->work = w;
    work->handler = handler;
    w_node->osal_work = work;
    w_node->work = w;
    osal_list_add(&(w_node->node), &g_wq_list);
    return OSAL_SUCCESS;
}

int osal_workqueue_schedule(osal_workqueue *work)
{
    if (work == NULL || work->work == NULL) {
        osal_log("parameter invalid!\n");
        return FALSE;
    }
    if (!schedule_work(work->work)) {
        return FALSE;
    }
    return TRUE;
}

void osal_workqueue_destroy(osal_workqueue *work)
{
    if (work == NULL || work->work == NULL) {
        osal_log("parameter invalid!\n");
        return;
    }
    osal_del_work(work->work);
    LOS_MemFree((void *)m_aucSysMem0, work->work);
    work->work = NULL;
}

int osal_workqueue_flush(osal_workqueue *work)
{
    if (work == NULL || work->work == NULL) {
        osal_log("parameter invalid!\n");
        return OSAL_FAILURE;
    }
    if (!flush_work(work->work)) {
        return FALSE;
    }
    return TRUE;
}
