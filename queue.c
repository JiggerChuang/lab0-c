#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!new) {
        free(new);
        return NULL;
    }

    INIT_LIST_HEAD(new);
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    struct list_head *node, *safe;

    list_for_each_safe (node, safe, l) {
        element_t *tmp = list_entry(node, element_t, list);
        q_release_element(tmp);
    }

    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!s || !head)
        return false;

    element_t *new = (element_t *) malloc(sizeof(element_t));
    if (!new) {
        return false;
    }
    new->value = (char *) malloc((strlen(s) + 1));
    if (!new->value) {
        free(new);
        return false;
    }
    strncpy(new->value, s, strlen(s) + 1);

    list_add(&new->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!s || !head)
        return false;

    element_t *new = (element_t *) malloc(sizeof(element_t));
    if (!new) {
        return false;
    }
    new->value = (char *) malloc((strlen(s) + 1));
    if ((!new->value)) {
        free(new);
        return false;
    }

    strncpy(new->value, s, strlen(s) + 1);
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!sp || !head)
        return NULL;

    element_t *rm_element = list_first_entry(head, element_t, list);
    if (!rm_element) {
        return NULL;
    }

    list_del(&rm_element->list);
    strncpy(sp, rm_element->value, bufsize);

    return rm_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!sp || !head)
        return NULL;

    element_t *rm_element = list_last_entry(head, element_t, list);
    if (!rm_element) {
        return NULL;
    }

    list_del(&rm_element->list);
    strncpy(sp, rm_element->value, bufsize);

    return rm_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
// https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
bool q_delete_mid(struct list_head *head)
{
    struct list_head *slow_ptr, *fast_ptr;
    slow_ptr = head->next;
    fast_ptr = head->next;

    while (fast_ptr != head && fast_ptr->next != head) {
        slow_ptr = slow_ptr->next;
        fast_ptr = fast_ptr->next->next;
    }

    list_del(slow_ptr);
    q_release_element(list_entry(slow_ptr, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
// https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
bool q_delete_dup(struct list_head *head)
{
    struct list_head *curr, *safe;

    /* use self-defined loop because I want to change
     * the condition node != (head)
     * to safe != (head)
     */
    for (curr = (head)->next, safe = curr->next;
         (safe != (head) && curr != (head));) {
        /* If curr->value != safe->value then go to next node */
        if (strcmp(list_entry(curr, element_t, list)->value,
                   list_entry(safe, element_t, list)->value)) {
            curr = safe, safe = curr->next;
            continue;
        }

        /* base used for previous curr
         *  tmp_head used to points to the list node to deleted
         */
        struct list_head *base;
        struct list_head *tmp_head = q_new();
        tmp_head->next = curr;
        base = curr->prev;
        for (; safe != head;) {
            if (strcmp(list_entry(curr, element_t, list)->value,
                       list_entry(safe, element_t, list)->value))
                break;

            safe = safe->next;
        }

        base->next = safe;
        safe->prev->next = tmp_head;
        curr->prev = tmp_head;
        tmp_head->prev = safe->prev;
        safe->prev = base;

        curr = base->next;
        safe = curr->next;

        /* Delete duplicated nodes */
        q_free(tmp_head);
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
