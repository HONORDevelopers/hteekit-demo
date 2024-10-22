/*
 * Copyright (c) Honor Device Co., Ltd. 2013-2019. All rights reserved.
 * Description: Define linked list data types and basic operations
 * Create: 2013-04-12
 */

/*
 * @defgroup TEEC_List linked list data types and basic operations
 * @ingroup TEEC_API
 */

#ifndef TEE_CLIENT_LIST_H
#define TEE_CLIENT_LIST_H

/*
 * @ingroup TEEC_List
 * define linked list data struct
 */
struct ListNode {
    struct ListNode *next;  /* point to next node  */
    struct ListNode *prev;  /* point to prev node */
};

/*
 * @ingroup TEEC_List
 * @brief Covert ListNode to struct node
 */
#define OFFSET_OF(type, member) (unsigned long)(&(((type *)0)->member))
#define CONTAINER_OF(pos, type, member) (type *)(((char *)(pos)) - OFFSET_OF(type, member))

/*
 * @ingroup TEEC_List
 * @brief Define a linked list node
 *
 * @par Description:
 * Macro definition to define a list node and initialize it
 * @param name [IN] Defined Linked List Name
 */
#define LIST_DECLARE(name) \
    struct ListNode name = { \
        .next = &name, \
        .prev = &name, \
    }

/*
 * @ingroup  TEEC_List
 * @brief Initialize the linked list
 *
 * @par Description:
 * Initialize the linked list header
 *
 * @attention NULL
 * @param list [IN/OUT]Linked list pointer, the value cannot be empty
 *
 * @retval NULL
 *
 * @par depends:
 * @li libteec: The shared library to which this interface belongs
 * @li tee_client_list.h: The header file where the interface declaration is located
 */
static inline void ListInit(struct ListNode *list)
{
    list->next = list;
    list->prev = list;
}

/*
 * @ingroup TEEC_List
 * Get next node of linked list
 */
#define LIST_HEAD(list) ((list)->next)

/*
 * @ingroup TEEC_List
 * Get prev node of linked list
 */
#define LIST_TAIL(list) ((list)->prev)


/*
 * @ingroup TEEC_List
 * Determine if the linked list is empty
 */
#define LIST_EMPTY(list) ((list) == (list)->next)

/*
 * @ingroup  TEEC_List
 * @brief Insert new node from the head of the linked list
 *
 * @par Description:
 * Insert new node from the head of the linked list
 *
 * @attention NULL
 * @param list [IN/OUT]Pointer to the head of the list, the value cannot be empty
 * @param entry [IN/OUT]Added a linked list node pointer, the value cannot be empty
 *
 * @retval NULL
 *
 * @par depends:
 * @li libteec: The shared library to which this interface belongs
 * @li tee_client_list.h: The header file where the interface declaration is located
 * @see ListInsertTail
 */
static inline void ListInsertHead(struct ListNode *list, struct ListNode *entry)
{
    list->next->prev = entry;
    entry->next = list->next;
    entry->prev = list;
    list->next = entry;
}

/*
 * @ingroup  TEEC_List
 * @brief Insert new node from the end of the linked list
 *
 * @par Description:
 * Insert new node from the end of the linked list
 *
 * @attention NULL
 * @param list [IN/OUT]Pointer to the head of the list, the value cannot be empty
 * @param entry [IN/OUT]Added a linked list node pointer, the value cannot be empty
 *
 * @retval NULL
 *
 * @par depends:
 * @li libteec: The shared library to which this interface belongs
 * @li tee_client_list.h: The header file where the interface declaration is located
 * @see ListInsertHead
 */
static inline void ListInsertTail(struct ListNode *list, struct ListNode *entry)
{
    entry->next = list;
    entry->prev = list->prev;
    list->prev->next = entry;
    list->prev = entry;
}

/*
 * @ingroup  TEEC_List
 * @brief delete node
 *
 * @par Description:
 * delete the specified node
 *
 * @attention After returning, the user needs to release the memory of the deleted node
 * @param entry [IN]The pointer of the linked list node to be deleted. The value cannot be null.
 *
 * @retval NULL
 *
 * @par depends:
 * @li libteec: The shared library to which this interface belongs
 * @li tee_client_list.h: The header file where the interface declaration is located
 * @see NULL
 */
static inline void ListRemoveEntry(struct ListNode *entry)
{
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
}

/*
 * @ingroup  TEEC_List
 * @brief Delete the list head node
 *
 * @par Description:
 * Delete the specified list head node
 *
 * @attention
 * @param list [IN]After returning, the user can release the memory of the deleted node
 *
 * @retval #NULL linked list is NULL
 * @retval non-NULL list head node
 *
 * @par depends:
 * @li libteec: The shared library to which this interface belongs
 * @li tee_client_list.h: The header file where the interface declaration is located
 * @see ListRemoveTail
 */
static inline struct ListNode *ListRemoveHead(struct ListNode *list)
{
    struct ListNode *entry = NULL;
    if (!LIST_EMPTY(list)) {
        entry = list->next;
        ListRemoveEntry(entry);
    }
    return entry;
}

/*
 * @ingroup  TEEC_List
 * @brief Delete the end of the linked list
 *
 * @par Description:
 * Delete the specified list tail node
 *
 * @attention After returning, the user can release the memory of the deleted node
 * @param list [IN]Pointer to the head of the list, the value cannot be empty
 *
 * @retval NULL linked list is NULL
 * @retval Non-empty list tail node
 *
 * @par depends:
 * @li libteec: The shared library to which this interface belongs
 * @li tee_client_list.h: The header file where the interface declaration is located
 * @see ListRemoveHead
 */
static inline struct ListNode *ListRemoveTail(struct ListNode *list)
{
    struct ListNode *entry = NULL;
    if (!LIST_EMPTY(list)) {
        entry = list->prev;
        ListRemoveEntry(entry);
    }
    return entry;
}

/*
 * @ingroup  TEEC_List
 * @brief Find a pointer to the data structure type from the pointer ptr of member member
 *
 * @par Description:From the pointer ptr of member member to find the pointer (starting address)
 * of the data structure type to which it belongs
 *
 * @attention NULL
 * @param ptr [IN]Linked list pointer, the value cannot be empty
 * @param type [IN]The structure type of the pointer ptr
 * @param member [IN]Pointer ptr points to member member in structure type
 *
 * @retval Non-empty Returns the starting address of the
 * structure type where ptr is located. The return type is type
 */
#define LIST_ENTRY(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/*
 * @ingroup  TEEC_List
 * @brief Traversing linked list nodes
 *
 * @par Description:
 * Traversing the linked list, returning each linked list node
 *
 * @attention NULL
 * @param pos [OUT]Linked list pointer, the value cannot be empty
 * @param list [IN]Pointer to the head of the list, the value cannot be empty
 *
 * @retval Non-empty return list node
 */
#define LIST_FOR_EACH(pos, list) \
    for (pos = (list)->next; pos != (list); pos = pos->next)

/*
 * @ingroup  TEEC_List
 * @brief Traversing linked list nodes safely
 *
 * @par Description:
 * Traversing the linked list, returning each linked list node
 *
 * @attention NULL
 * @param pos [OUT]Linked list pointer, the value cannot be empty
 * @param list [IN]Pointer to the head of the list, the value cannot be empty
 *
 * @retval Non-empty return list node
 */
#define LIST_FOR_EACH_SAFE(pos, n, list) \
    for ((pos) = (list)->next, (n) = (pos)->next; \
        (pos) != (list); \
        (pos) = (n), (n) = (pos)->next)

/*
 * @ingroup  TEEC_List
 * @brief Iterate over the start address of the data structure where the list node is located
 *
 * @par Description:
 * Traverse the linked list and obtain the starting address
 * of the data structure where the linked list node is located
 *
 * @attention NULL
 * @param pos [IN]Linked list pointer, the value cannot be empty
 * @param list [IN]Pointer to the head of the list, the value cannot be empty
 * @param member [IN]Member to which the linked list node belongs
 *
 * @retval Non-empty start address of the data structure where the linked list
 * node is located, the return type is typeof (* pos)
 */
#define LIST_FOR_EACH_ENTRY(pos, list, member) \
    for (pos = LIST_ENTRY((list)->next, typeof(*pos), member); \
        &pos->member != (list); \
        pos = LIST_ENTRY(pos->member.next, typeof(*pos), member))

/*
 * @ingroup  TEEC_List
 * @brief Iterate over the start address of the data structure where the list node is located
 *
 * @par Description:
 * Traverse the linked list and obtain the starting address
 * of the data structure where the linked list node is located
 *
 * @attention NULL
 * @param pos [IN]Linked list pointer, the value cannot be empty
 * @param n [IN]Linked list pointer, the value cannot be empty
 * @param list [IN]Pointer to the head of the list, the value cannot be empty
 * @param member [IN]Member to which the linked list node belongs
 *
 * @retval Non-empty start address of the data structure where the linked list
 * node is located, the return type is typeof (* pos)
 */
#define LIST_FOR_EACH_ENTRY_SAFE(pos, n, list, member) \
    for (pos = LIST_ENTRY((list)->next, typeof(*pos), member), \
        n = LIST_ENTRY(pos->member.next, typeof(*pos), member); \
        &pos->member != (list); \
        pos = n, n = LIST_ENTRY(n->member.next, typeof(*n), member))

#endif
