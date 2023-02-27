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
    if (!sp || !head || (q_size(head) == 0))
        return NULL;

    element_t *rm_element = list_first_entry(head, element_t, list);
    if (!rm_element) {
        return NULL;
    }

    list_del(&rm_element->list);
    strncpy(sp, rm_element->value, bufsize - 1);
    sp[bufsize - 1] = '\0';

    return rm_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!sp || !head || (q_size(head) == 0))
        return NULL;

    element_t *rm_element = list_last_entry(head, element_t, list);
    if (!rm_element) {
        return NULL;
    }

    list_del(&rm_element->list);
    strncpy(sp, rm_element->value, bufsize - 1);
    sp[bufsize - 1] = '\0';

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
    if (!head)
        return false;

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
    if (!head)
        return false;

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
// https://leetcode.com/problems/swap-nodes-in-pairs/
void q_swap(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *node, *safe;
    for (node = (head)->next, safe = node->next;
         (node != (head) && safe != (head));) {
        struct list_head *tmp_node = node->prev;
        node->next = safe->next;
        node->prev = safe;
        tmp_node->next = safe;

        struct list_head *tmp_safe = safe->next;
        safe->next = node;
        safe->prev = tmp_node;
        tmp_safe->prev = node;

        /* move curr node to next two nodes */
        node = node->next;
        safe = node->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *node;
    for (node = (head)->prev; node != (head); node = node->next) {
        struct list_head *tmp = node->next;
        node->next = node->prev;
        node->prev = tmp;
    }

    /* reverse prev and next of head node */
    struct list_head *tmp_head = head->next;
    head->next = head->prev;
    head->prev = tmp_head;
}

/* Reverse the nodes of the list k at a time */
// https://leetcode.com/problems/reverse-nodes-in-k-group/
void q_reverseK(struct list_head *head, int k)
{
    if (!head || k > q_size(head) || k == 0 || k == 1)
        return;

    int loop_cnt = q_size(head) / k;
    struct list_head *curr, *safe;
    curr = head->next;

    for (int i = 0; i < loop_cnt; i++) {
        /* get truncated of k nodes and linked with tmp_head */
        struct list_head *base;
        LIST_HEAD(reverse_head);

        base = curr->prev;
        safe = curr;
        for (int j = 0; j < k; j++) {
            LIST_HEAD(tmp_head);
            struct list_head *tmp_safe = safe;
            safe = safe->next;

            /* Use tmp_head to store
             * current single node to be
             * spliced to reverse_head
             */
            tmp_head.next = tmp_safe;
            tmp_safe->prev = &tmp_head;
            tmp_head.prev = tmp_safe;
            tmp_safe->next = &tmp_head;

            list_splice_tail(&tmp_head, &reverse_head);
        }

        /* Do reverse */
        q_reverse(&reverse_head);
        base->next = reverse_head.next;
        reverse_head.next->prev = base;
        safe->prev = reverse_head.prev;
        reverse_head.prev->next = safe;

        /* Move curr to next k nodes */
        curr = safe;
    }
}

void mergeTwoLists(struct list_head *L1, struct list_head *L2)
{
    if (!L1 || !L2)
        return;

    struct list_head *lptr = L1->next;
    struct list_head *rptr = L2->next;
    for (; lptr != L1 && rptr != L2;) {
        /* L1->value > L2->value */
        if (strcmp(list_entry(lptr, element_t, list)->value,
                   list_entry(rptr, element_t, list)->value) > 0) {
            struct list_head *tmp_rptr = rptr->next;
            list_del(rptr);

            /* prev of lptr points to rptr
             * next of rptr points to lptr
             */
            lptr->prev->next = rptr;
            rptr->prev = lptr->prev;
            rptr->next = lptr;
            lptr->prev = rptr;

            /* rptr move to next node */
            rptr = tmp_rptr;
        } else { /* L1->value <= L2->value */
            lptr = lptr->next;
        }
    }

    /* if the iteration of L1 and L2 over
     * and there are elements in L2 then
     * splice them to the tail of L1
     */
    if (q_size(L2) != 0) {
        list_splice_tail(L2, L1);
    }
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || q_size(head) == 0 || q_size(head) == 1)
        return;

    /* middle is the position of list
     * left stores beginning addr of left part of list
     * right stores beginning addr of right part of list
     * */
    int middle = q_size(head) / 2;
    LIST_HEAD(right);

    /* get middle node of list */
    struct list_head *curr = head;
    for (int i = 0; i < middle; i++)
        curr = curr->next;

    /* partition into left part and right part */
    list_cut_position(&right, head, curr);

    q_sort(head);
    q_sort(&right);
    mergeTwoLists(head, &right);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
// https://leetcode.com/problems/remove-nodes-from-linked-list/
int q_descend(struct list_head *head)
{
    if (!head || q_size(head) == 0)
        return 0;

    element_t *entry, *safe;
    for (int i = 0; i < q_size(head); i++) {
        int flag = 0;
        list_for_each_entry_safe (entry, safe, head, list) {
            if (&safe->list == head)
                break;

            if (strcmp(entry->value, safe->value) < 0) {
                list_del(&entry->list);
                q_release_element(entry);
                flag = 1;
            }
        }
        if (!flag)
            break;
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
// https://leetcode.com/problems/merge-k-sorted-lists/
int q_merge(struct list_head *head)
{
    if (!head)
        return 0;

    queue_contex_t *h = container_of(head->next, queue_contex_t, chain);
    struct list_head *safe = head->next->next;
    for (; safe != head;) {
        struct list_head *tmp = safe;
        safe = safe->next;
        queue_contex_t *s = container_of(tmp, queue_contex_t, chain);

        mergeTwoLists(&container_of(h->q, element_t, list)->list,
                      &container_of(s->q, element_t, list)->list);
        INIT_LIST_HEAD(s->q);
    }

    return q_size(h->q);
}
