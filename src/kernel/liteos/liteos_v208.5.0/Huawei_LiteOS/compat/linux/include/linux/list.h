/**
 * @defgroup list List
 * @ingroup linux
 */

#ifndef _LINUX_LIST_H
#define _LINUX_LIST_H

#include "stddef.h"
#include "linux/wait.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct list_head {
    struct list_head *next;
    struct list_head *prev;
} list_head_t;

struct hlist_head {
    struct hlist_node *first;
};

typedef struct hlist_node {
    struct hlist_node *next, **pprev;
} hlist_node_t;

#define LIST_HEAD_INIT(name) { &(name), &(name) }

/**
 * @ingroup list
 * @brief Declares variables and initializes the linked list header.
 *
 * @par Description:
 * This API is used to declares variables and initializes the linked list header.
 * @attention
 * None.
 *
 * @param name    [IN] A node in a doubly linked list.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see INIT_LIST_HEAD
 */
#define LIST_HEAD(name)     list_head_t name = LIST_HEAD_INIT(name)

/**
 * @ingroup list
 * @brief Initialize the input node to a doubly linked list.
 *
 * @par Description:
 * This API is used to initialize the input node (the first parameter list) to a doubly linked list.
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param list    [IN] A node in a doubly linked list.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see LIST_HEAD
 */
static __inline__ void INIT_LIST_HEAD(list_head_t *list)
{
    list->next = list;
    list->prev = list;
}

/**
 * @ingroup list
 * @brief Identify whether a specified doubly linked list is empty or not.
 *
 * @par Description:
 * This API is used to judge whether a doubly linked list is empty or not.
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param list  [IN] Doubly linked list.
 *
 * @retval #1  The doubly linked list is empty.
 * @retval #0  The doubly linked list is not empty.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
static __inline__ int list_empty(const list_head_t *list)
{
    return list->next == list;
}

/**
 * @ingroup list
 * @brief Insert an entry after the specified entry.
 *
 * @par Description:
 * This API is used to insert an entry after the specified entry.
 * @attention
 * <ul>
 * <li> The parameters passed in should be legal pointers. </li>
 * </ul>
 *
 * @param newNode      [IN] The new node to be inserted.
 * @param afterNode    [IN] Point to where the node is inserted.
 *
 * @retval None
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see list_del | list_add_tail
 */
static __inline__ void list_add(list_head_t *newNode, list_head_t *afterNode)
{
    list_head_t *nextNode = afterNode->next;
    newNode->prev = afterNode;
    newNode->next = nextNode;
    nextNode->prev = newNode;
    afterNode->next = newNode;
}

/**
 * @ingroup list
 * @brief Insert an entry before the specified entry.
 *
 * @par Description:
 * This API is used to insert an entry before the specified entry.
 * @attention
 * <ul>
 * <li> The parameters passed in should be legal pointers. </li>
 * </ul>
 *
 * @param newNode      [IN] The new node to be inserted.
 * @param beforeNode   [IN] Point to where the node is inserted.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see list_add
 */
static __inline__ void list_add_tail(list_head_t *newNode, list_head_t *beforeNode)
{
    list_head_t *pstprev = beforeNode->prev;
    newNode->next = beforeNode;
    newNode->prev = pstprev;
    pstprev->next = newNode;
    beforeNode->prev = newNode;
}

/**
 * @ingroup list
 * @brief Gets the address of the container structure.
 *
 * @par Description:
 * This API gets the address of the structure where the list #ptr is located based on the address of the #ptr.
 * @attention
 * None.
 *
 * @param ptr     [IN] Type #list_head_t *  The node of the doubly linked list.
 * @param type    [IN] The type of structure.
 * @param member  [IN] The doubly linked list name in the structure.
 *
 * @retval The pointer to the address of the container structure.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see list_for_each_entry
 */
#define list_entry(ptr, type, member)      LOS_DL_LIST_ENTRY(ptr, type, member)

/**
 * @ingroup list
 * @brief Traverse a doubly linked list.
 *
 * @par Description:
 * This API is used to traverse a doubly linked list. The API is a loop. The start node of the
 * doubly linked list is the second parameter list. And in each loop, the obtained pointer to
 * the next node of the doubly linked list is outputted in the first parameter item.
 * @attention
 * None.
 *
 * @param node      [IN/OUT] Type #list_head_t *  The pointer to the next node in the doubly linked list.
 * @param list      [IN] Type #list_head_t *      The pointer to the node of the doubly linked list to be traversed.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see list_for_each_safe | list_for_each_entry
 */
#define list_for_each(node, list)  for ((node) = (list)->next; (node) != (list); (node) = (node)->next)

/**
 * @ingroup list
 * @brief Traverse a doubly linked list and obtain the address of the structure containing the linked list node.
 *
 * @par Description:
 * Traverse a doubly linked list and obtain the address of the structure containing the linked list node.
 * @attention
 * None.
 *
 * @param list    [IN/OUT]  The pointer to the structure that contains the doubly linked list node.
 * @param head    [IN]      Type #list_head_t *  The start node of the doubly linked list to be traversed.
 * @param item    [IN]      The doubly linked list name in the structure.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see list_entry | list_for_each_entry_safe | list_for_each
 */
#define list_for_each_entry(list, head, item)                         \
for ((list) = list_entry((head)->next, __typeof__(*list), item);      \
     &((list)->item) != (head);                                       \
     (list) = list_entry((list)->item.next, __typeof__(*list), item))

/**
 * @ingroup list
 * @brief Traverse a doubly linked list safe against removal of list entry.
 *
 * @par Description:
 * This API is used to traverse a doubly linked list safe against removal of list entry.
 * @attention
 * None.
 *
 * @param pos      [IN/OUT] Type #list_head_t *  Cursor for looping.
 * @param n        [IN/OUT] Type #list_head_t *  The pointer to the next node of the first parameter #pos.
 * @param head     [IN]     Type #list_head_t *  The pointer to the node of the doubly linked list to be traversed.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see list_for_each | list_for_each_entry_safe
 */
#define list_for_each_safe(pos, n, head) \
    for ((pos) = (head)->next, (n) = (pos)->next; (pos) != (head); (pos) = (n), (n) = (pos)->next)

/**
 * @ingroup list
 * @brief Obtains the address of the structure containing the head node of the doubly linked list.
 *
 * @par Description:
 * This API is used to obtain the address of the structure containing the head node of the doubly linked list.
 * @attention
 * None.
 *
 * @param ptr     [IN] Type #list_head_t *  The kead node of the doubly linked list.
 * @param type    [IN] Structure name.
 * @param member  [IN] The doubly linked list name in the structure.
 *
 * @retval The pointer to the structure that contains the head node of the doubly linked list.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see list_entry
 */
#define list_first_entry(ptr, type, member)     list_entry((ptr)->next, type, member)

/* ist_is_last - tests whether @list is the last entry in list @head */
/**
 * @ingroup list
 * @brief Check whether the node #list is the tail node of the doubly linked list.
 *
 * @par Description:
 * This API is used to check whether the node #list is the tail node of the doubly linked list.
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param list     [IN] Type #list_head_t *  The node to be checked.
 * @param head     [IN] The head node of the doubly linked list.
 *
 * @retval #1  The node #list is the tail node of the doubly linked list.
   @retval #0  The node #list is not the tail node of the doubly linked list.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
static inline int list_is_last(const list_head_t *list, const list_head_t *head)
{
    return list->next == head;
}

/**
 * @ingroup list
 * @brief Traverses the structure containing double linked list nodes, and it is safe against removal of list entry.
 *
 * @par Description:
 * This API is used to traverses the structure containing double linked list nodes.
 * @attention
 * None.
 *
 * @param pos        [IN/OUT]  The pointer to the structure that contains the doubly linked list node.
 * @param n          [IN/OUT]  Type #list_head_t *  The pointer to the next node of the first parameter #pos.
 * @param head       [IN]      Type #list_head_t *  The start node of the doubly linked list to be traversed.
 * @param member     [IN]      The doubly linked list name in the structure.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see list_entry | list_for_each_entry | list_for_each_safe
 */
#define list_for_each_entry_safe(pos, n, head, member)                            \
    for ((pos) = list_entry((head)->next, __typeof__(*(pos)), member),            \
         (n) = list_entry((pos)->member.next, __typeof__(*(pos)), member);        \
         &(pos)->member != (head);                                                \
         (pos) = (n), (n) = list_entry((n)->member.next, __typeof__(*n), member))

/**
 * @ingroup list
 * @brief Reversely traverses a structure containing double linked list nodes.
 *
 * @par Description:
 * Reversely traverses a structure containing double linked list nodes.
 * @attention
 * None.
 *
 * @param pos     [IN/OUT]  The pointer to the structure that contains the doubly linked list node.
 * @param head    [IN]      Type #list_head_t *  The start node of the doubly linked list to be traversed.
 * @param member  [IN]      The doubly linked list name in the structure.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see list_entry | list_for_each_entry | list_for_each
 */
#define list_for_each_entry_reverse(pos, head, member)                            \
        for ((pos) = list_entry((head)->prev, __typeof__(*(pos)), member);        \
             &(pos)->member != (head);                                            \
             (pos) = list_entry((pos)->member.prev, __typeof__(*(pos)), member))

/**
 * @ingroup list
 * @brief Delete a specified node from a doubly linked list.
 *
 * @par Description:
 * This API is used to delete a specified node from a doubly linked list.
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param node    [IN] Node to be deleted.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see list_add
 */
static __inline__ void list_del(list_head_t *node)
{
    node->next->prev = node->prev;
    node->prev->next = node->next;
}

/**
 * @ingroup list
 * @brief Delete a node from the linked list by using the previous node and next node.
 *
 * @par Description:
 * This API is used to delete a node from the linked list by using the previous node and next node.
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param prev    [IN] The previous node of the target delete node.
 * @param next    [IN] The next node of the target delete node.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
static inline void __list_del(list_head_t *prev, list_head_t *next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * @ingroup list
 * @brief Delete a specified node from a doubly linked list.
 *
 * @par Description:
 * This API is used to delete a specified node from a doubly linked list.
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * <li> list_del is an external interface, and __list_del_entry、__list_del are internal kernel interfaces. </li>
 * </ul>
 *
 * @param entry    [IN] Node to be deleted.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see __list_del | list_del
 */
static inline void __list_del_entry(list_head_t *entry)
{
    __list_del(entry->prev, entry->next);
}

/**
 * @ingroup list
 * @brief Delete the #list from the original double linked list,
 * and insert it as the head node into another double linked list.
 *
 * @par Description:
 * This API is used to deletes the #list from the original double linked list,
 * and insert it as the head node into another double linked list.
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param list    [IN] Node to be deleted.
 * @param head    [IN] Head of the inserted double linked list.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see __list_del_entry | list_add
 */
static inline void list_move(list_head_t *list, list_head_t *head)
{
    __list_del_entry(list);
    list_add(list, head);
}

/**
 * @ingroup list
 * @brief Deletes the node #entry from the double linked list and reinitialize it.
 *
 * @par Description:
 * This API is used to deletes the node #entry from the double linked list and reinitialize it.
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param entry    [IN] Node to be deleted.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>list.h: the header file that contains the API declaration.</li></ul>
 * @see __list_del | INIT_LIST_HEAD
 */
static inline void list_del_init(list_head_t *entry)
{
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_LIST_H */
