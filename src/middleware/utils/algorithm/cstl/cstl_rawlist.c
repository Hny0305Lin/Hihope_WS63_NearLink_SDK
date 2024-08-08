/**
 * @file cstl_rawlist.c
 * @copyright Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief raw_list 实现
 * @details raw_list源码
 * @date 2021-04-15
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * </table>
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * </table>
 * *******************************************************************************************
 */

#include <stdlib.h>
#include "cstl_public_inner.h"
#include "cstl_rawlist.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 内部函数定义 */
CSTL_STATIC inline bool CstlRawListNodeInList(const CstlRawListNode *node)
{
    bool ret = false;

    if ((node->next != NULL) && (node->prev != NULL) &&
        ((const CstlRawListNode *)(node->next->prev) == node) &&
        ((const CstlRawListNode *)(node->prev->next) == node)) {
        ret = true;
    }

    return ret;
}

CSTL_STATIC inline bool CstlRawListEmptyCheck(const CstlRawList *list)
{
    return (&list->head)->next == &list->head;
}

CSTL_STATIC inline void CstlRawListAddAfterNode(CstlRawListNode *node, CstlRawListNode *where)
{
    node->next       = (where)->next;
    node->prev       = (where);
    where->next      = node;
    node->next->prev = node;
}

CSTL_STATIC inline void CstlRawListAddBeforeNode(CstlRawListNode *node, const CstlRawListNode *where)
{
    CstlRawListAddAfterNode(node, where->prev);
}

CSTL_STATIC inline bool CstlRawListIsFirstNode(const CstlRawList *list, const CstlRawListNode *node)
{
    bool ret = false;

    if ((const CstlRawListNode *)list->head.next == node) {
        ret = true;
    }

    return ret;
}

CSTL_STATIC inline bool CstlRawListIsLastNode(const CstlRawList *list, const CstlRawListNode *node)
{
    bool ret = false;

    if ((const CstlRawListNode *)list->head.prev == node) {
        ret = true;
    }

    return ret;
}

/* 删除链表节点，内部函数，不做入参校验 */
CSTL_STATIC void CstlRawListRemoveNode(CstlRawList *list, CstlRawListNode *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;

    if (list->freeFunc != NULL) {
        list->freeFunc((void *)node);
    }
}

__attribute__((visibility("default"))) int32_t CstlRawListInit(
    CstlRawList *list, CstlFreeFunc freeFunc)
{
    int32_t ret = (int32_t)ERRNO_CSTL_INPUT_INVALID;

    if (list != NULL) {
        list->head.next = &list->head;
        list->head.prev = &list->head;
        list->freeFunc  = freeFunc;
        ret = CSTL_OK;
    }

    return ret;
}

__attribute__((visibility("default"))) int32_t CstlRawListClear(
    CstlRawList *list)
{
    int32_t ret = (int32_t)ERRNO_CSTL_INPUT_INVALID;

    if (list != NULL) {
        while (!CstlRawListEmptyCheck(list)) {
            CstlRawListRemoveNode(list, (CstlRawListNode *)list->head.next);
        }

        ret = CSTL_OK;
    }

    return ret;
}

__attribute__((visibility("default"))) int32_t CstlRawListDeinit(
    CstlRawList *list)
{
    int32_t ret = (int32_t)ERRNO_CSTL_INPUT_INVALID;

    if (list != NULL) {
        ret = CstlRawListClear(list);
        list->freeFunc = NULL;
    }

    return ret;
}

__attribute__((visibility("default"))) bool CstlRawListEmpty(
    const CstlRawList *list)
{
    bool ret = true;

    if (list != NULL) {
        ret = CstlRawListEmptyCheck(list);
    }

    return ret;
}

CSTL_STATIC inline size_t CstlRawListSizeInner(const CstlRawList *list)
{
    size_t size = 0;
    const CstlRawListNode *node, *head;

    head = &list->head;
    for (node = head->next; node != head; node = node->next) {
        size++;
    }

    return size;
}

__attribute__((visibility("default"))) size_t CstlRawListSize(
    const CstlRawList *list)
{
    size_t size = 0;

    if ((list != NULL) && !CstlRawListEmptyCheck(list)) {
        size = CstlRawListSizeInner(list);
    }

    return size;
}

__attribute__((visibility("default"))) int32_t CstlRawListPushFront(
    CstlRawList *list, CstlRawListNode *node)
{
    int32_t ret = (int32_t)ERRNO_CSTL_INPUT_INVALID;

    if ((list != NULL) && (node != NULL)) {
        CstlRawListAddAfterNode(node, &(list->head));
        ret = CSTL_OK;
    }

    return ret;
}

__attribute__((visibility("default"))) int32_t CstlRawListPushBack(
    CstlRawList *list, CstlRawListNode *node)
{
    int32_t ret = (int32_t)ERRNO_CSTL_INPUT_INVALID;

    if ((list != NULL) && (node != NULL)) {
        CstlRawListAddBeforeNode(node, &(list->head));
        ret = CSTL_OK;
    }

    return ret;
}

__attribute__((visibility("default"))) int32_t CstlRawListInsert(
    const CstlRawListNode *curNode, CstlRawListNode *newNode)
{
    int32_t ret = (int32_t)ERRNO_CSTL_INPUT_INVALID;

    if ((curNode != NULL) && (newNode != NULL) && (CstlRawListNodeInList(curNode))) {
        CstlRawListAddBeforeNode(newNode, curNode);
        ret = CSTL_OK;
    }

    return ret;
}

__attribute__((visibility("default"))) int32_t CstlRawListPopFront(
    CstlRawList *list)
{
    int32_t ret = (int32_t)ERRNO_CSTL_INPUT_INVALID;
    CstlRawListNode *firstNode;

    if ((list != NULL) && (!CstlRawListEmptyCheck(list))) {
        firstNode = list->head.next;
        CstlRawListRemoveNode(list, firstNode);
        ret = CSTL_OK;
    }

    return ret;
}

__attribute__((visibility("default"))) int32_t CstlRawListPopBack(
    CstlRawList *list)
{
    int32_t ret = (int32_t)ERRNO_CSTL_INPUT_INVALID;
    CstlRawListNode *lastNode;

    if (list != NULL) {
        if (!CstlRawListEmptyCheck(list)) {
            lastNode = list->head.prev;
            CstlRawListRemoveNode(list, lastNode);
            ret = CSTL_OK;
        } else {
            ret = (int32_t)ERRNO_CSTL_ELEMENT_EMPTY;
        }
    }

    return ret;
}

CSTL_STATIC void CstlRawListEraseInner(CstlRawList *list, CstlRawListNode *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;

    if ((list != NULL) && !CstlRawListEmptyCheck(list) && (list->freeFunc != NULL)) {
        list->freeFunc((void *)node);
    }
}

__attribute__((visibility("default"))) int32_t CstlRawListErase(
    CstlRawList *list, CstlRawListNode *node)
{
    int32_t ret = (int32_t)ERRNO_CSTL_INPUT_INVALID;

    if ((node != NULL) && (CstlRawListNodeInList(node))) {
        CstlRawListEraseInner(list, node);
        ret = CSTL_OK;
    }

    return ret;
}

__attribute__((visibility("default"))) CstlRawListNode *CstlRawListFront(
    const CstlRawList *list)
{
    CstlRawListNode *front = NULL;

    if ((list != NULL) && (!CstlRawListEmptyCheck(list))) {
        front = list->head.next;
    }

    return front;
}

__attribute__((visibility("default"))) CstlRawListNode *CstlRawListBack(
    const CstlRawList *list)
{
    CstlRawListNode *back = NULL;

    if ((list != NULL) && (!CstlRawListEmptyCheck(list))) {
        back = list->head.prev;
    }

    return back;
}

__attribute__((visibility("default"))) CstlRawListNode *CstlRawListPrev(
    const CstlRawList *list, const CstlRawListNode *node)
{
    CstlRawListNode *prev;

    if ((list == NULL) || (node == NULL) ||
        (CstlRawListEmptyCheck(list)) || (CstlRawListIsFirstNode(list, node)) || (!CstlRawListNodeInList(node))) {
        prev = NULL;
    } else {
        prev = node->prev;
    }

    return prev;
}

__attribute__((visibility("default"))) CstlRawListNode *CstlRawListNext(
    const CstlRawList *list, const CstlRawListNode *node)
{
    CstlRawListNode *next;

    if ((list == NULL) || (node == NULL) ||
        (CstlRawListEmptyCheck(list)) || (CstlRawListIsLastNode(list, node)) || (!CstlRawListNodeInList(node))) {
        next = NULL;
    } else {
        next = node->next;
    }

    return next;
}

CSTL_STATIC void CstlRawListSortInner(CstlRawList *list, CstlDataCmpFunc cmpFunc)
{
    ssize_t i, j;
    ssize_t count;
    int32_t cmpResult;
    CstlRawListNode *node;
    CstlRawListNode *nextNode;

    count = (ssize_t)CstlRawListSizeInner(list);
    for (i = 0; i < (count - 1); ++i) {
        node = CstlRawListFront(list);
        for (j = 0; j < (count - i - 1); ++j) {
            nextNode = node->next;
            cmpResult = cmpFunc(node, nextNode);
            if (cmpResult > 0) {
                node->prev->next = nextNode;
                nextNode->prev = node->prev;

                node->prev = nextNode;
                node->next = nextNode->next;
                nextNode->next->prev = node;
                nextNode->next = node;

                continue;
            }
            node = node->next;
        }
    }
}

/* 链表排序函数，此处函数钩子cmpFunc的两个入参，类型需由用户保证是（CstlRawListNode *） */
__attribute__((visibility("default"))) int32_t CstlRawListSort(
    CstlRawList *list, CstlDataCmpFunc cmpFunc)
{
    int32_t ret = (int32_t)ERRNO_CSTL_INPUT_INVALID;

    if ((list != NULL) && (cmpFunc != NULL)) {
        CstlRawListSortInner(list, cmpFunc);
        ret = CSTL_OK;
    }

    return ret;
}

/* 链表节点查找函数，此处函数钩子nodeMatchFunc的第一个入参，类型需由用户保证是（CstlRawListNode *） */
__attribute__((visibility("default"))) CstlRawListNode *CstlRawListNodeFind(
    const CstlRawList *list, CstlMatchFunc nodeMatchFunc, uintptr_t data)
{
    CstlRawListNode *ans = NULL;
    CstlRawListNode *node;
    const CstlRawListNode *head;

    if ((list != NULL) && (nodeMatchFunc != NULL)) {
        head = (const CstlRawListNode *)(&list->head);
        node = head->next;
        while ((const CstlRawListNode *)node != head) {
            if (nodeMatchFunc((void *)node, data)) {
                ans = node;
                break;
            }
            node = node->next;
        }
    }

    return ans;
}

#ifdef __cplusplus
}
#endif
