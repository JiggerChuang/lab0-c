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
    struct list_head *new_head = (struct list_head *) malloc(
                                sizeof(struct list_head) * 1);
    if (!new_head)
        return NULL;
    new_head->next = new_head;
    new_head->prev = new_head;
    return new_head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    
    struct list_head *curr = head->next;
    while (curr != head){
        element_t *tmp = container_of(curr, element_t, list);
        curr = curr->next;
        free(tmp->value);
        free(tmp);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_element = (element_t *) malloc(sizeof(element_t) * 1);
    if (!new_element)
        return false;
    
    // new_element->value = strdup(s);
    // if (!new_element->value){
    //     free(new_element);
    //     return false;
    // }
    new_element->value = (char *) malloc(sizeof(char) * strlen(s) + 1);
    if (!new_element->value)
        return false;
    strcpy(new_element->value, s);

    new_element->list.next = head->next;
    head->next->prev = &new_element->list;
    new_element->list.prev = head;
    head->next = &new_element->list;

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    
    element_t *new_element = (element_t *) malloc(sizeof(element_t) * 1);
    if (!new_element)
        return false;
    
    new_element->value = (char *) malloc(sizeof(char) * strlen(s) + 1);
    if (!new_element->value)
        return false;
    strcpy(new_element->value, s);

    new_element->list.prev = head->prev;
    new_element->list.next = head;
    head->prev->next = &new_element->list;
    head->prev = &new_element->list;
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || !q_size(head))
        return NULL;

    element_t *rm_element = container_of(head->next, element_t, list);
    head->next = rm_element->list.next;
    rm_element->list.next->prev = head;
    
    if (sp){
        strncpy(sp, rm_element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return rm_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || !q_size(head))
        return NULL;

    element_t *rm_element = container_of(head->prev, element_t, list);
    rm_element->list.prev->next = head;
    head->prev = rm_element->list.prev;

    if (sp){
        strncpy(sp, rm_element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return rm_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || head->next == head || head->prev == head)
        return 0;

    struct list_head *curr = head->next;
    int len = 0;
    while (curr != head){
        len++;
        curr = curr->next;
    }

    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || !head->next)
        return false;

    struct list_head *slow = head;
    struct list_head *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    element_t *rm_element = container_of(slow->next, element_t, list);
    slow->next = slow->next->next;
    slow->next->prev = slow;
    free(rm_element->value);
    free(rm_element);
    
    return true;
}

/* Delete all nodes that have duplicate string */
// https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/description/
bool q_delete_dup(struct list_head *head)
{
    if (!head || head->next == head)
        return false;
    
    struct list_head *start = head;
    struct list_head *curr = start->next;

    while (curr != head && curr->next != head) {
        if (curr->next != head &&
            !strcmp(container_of(curr, element_t, list)->value,
            container_of(curr->next, element_t, list)->value)) {
            
            while (curr->next != head &&
                !strcmp(container_of(curr, element_t, list)->value,
                container_of(curr->next, element_t, list)->value)) {
                element_t *tmp = container_of(curr, element_t, list);
                curr = curr->next;
                free(tmp->value);
                free(tmp);
            }
            element_t *tmp = container_of(curr, element_t, list);
            curr = curr->next;
            free(tmp->value);
            free(tmp);
            start->next = curr;
            curr->prev = start;
        } else {
            start = start->next;
            curr = curr->next;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
// https://leetcode.com/problems/swap-nodes-in-pairs/
void q_swap(struct list_head *head)
{
    if (!head || !head->next)
        return;

    struct list_head *dummy = head;
    while (dummy->next != head && dummy->next->next != head) {
        struct list_head *first = dummy->next;
        struct list_head *second = first->next;
        first->next = second->next;
        second->next = first;
        dummy->next = second;
        dummy = first;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || head->next == head || head->prev == head)
        return;

    // struct list_head *tmp_print = head;
    // list_for_each (tmp_print, head)
    //     printf("%s ", container_of(tmp_print, element_t, list)->value);
    // printf("\n");
    // exit(-1);

    struct list_head *curr = head;
    struct list_head *tmp = NULL;
    do {
        tmp = curr->next;
        curr->next = curr->prev;
        curr->prev = tmp;
        curr = tmp;
    } while (curr != head);
}

/* Reverse the nodes of the list k at a time */
// https://leetcode.com/problems/reverse-nodes-in-k-group/
void q_reverseK(struct list_head *head, int k)
{
    if (!head || head->next == head || k == 1)
        return;
    
    struct list_head *prev_gp_end = head;
    struct list_head *curr = head->next;
    while (curr != head) {
        struct list_head *gp_start = curr;
        struct list_head *gp_end = curr;
        for (int i = 1; i < k; i++) {
            gp_end = gp_end->next;
            if (gp_end == head) {
                return;
            }
        }
        struct list_head *next_gp_start = gp_end->next;

        // detach the group
        gp_start->prev->next = next_gp_start;
        next_gp_start->prev = gp_start->prev;
        struct list_head *tmp_head_next = head->next;
        struct list_head *tmp_head_prev = head->prev;
        gp_start->prev = head;
        gp_end->next = head;
        head->next = gp_start;
        head->prev = gp_end;

        q_reverse(gp_start);
        head->next = tmp_head_next;
        head->prev = tmp_head_prev;

        // reconnect the reversed group
        prev_gp_end->next = gp_end;
        gp_end->prev = prev_gp_end;
        gp_start->next = next_gp_start;
        next_gp_start->prev = gp_start;

        // update pointer for next iteration
        prev_gp_end = gp_start;
        curr = next_gp_start;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}