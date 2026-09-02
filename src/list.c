#include <list.h>
#include <cmsis.h>
#include <mem.h>
#include <process.h>

struct list *list_node_create(void *data)
{
    struct list *t = kalloc(sizeof(struct list));
    t->data = data;
    t->next = NULL;
    t->prev = NULL;
    return t;
}

/* TODO: change the name of this function*/
struct list *list_add_node(struct list *head, struct list *node)
{
    struct list *t = head;

    if (!node)
        return NULL;
    if (!t)
        return node;

    while (t->next != NULL)
        t = t->next;
    t->next = node;
    node->next = NULL;
    node->prev = t;
    return head;
}

inline struct list *list_add_node_prepend(struct list *head, struct list *node)
{
    head->prev = node;
    node->prev = NULL;
    node->next = head;
    return node;
}

struct list *
list_add_node_func_based(struct list *head, struct list *node, uint32_t (*comp)(struct list *, struct list *))
{
    struct list *t = head, *t1 = NULL;

    if (!node)
        return NULL;
    if (!t)
        return node;

    if (comp(node, t)) {
        node->next = t;
        node->prev = NULL;
        t->prev = node;
        return node;
    }
    t1 = t;
    t = t->next;

    while (t != NULL) {
        if (comp(node, t)) {
            t->prev->next = node;
            node->prev = t->prev;
            t->prev = node;
            node->next = t;
            return head;
        }
        t1 = t;
        t = t->next;
    }
    t1->next = node;
    node->prev = t1;
    node->next = NULL;
    return head;
}

inline struct list *list_rem_head(struct list *head)
{
    struct list *t = head->next;

    head->next->prev = NULL;
    head->next = NULL;
    return t;    
}

inline struct list *list_rem_node(struct list *head ,struct list *node)
{
    struct list *t = head;

    if (node->next)
        node->next->prev = node->prev;
    if (node->prev)
        node->prev->next = node->next;
    else
        t = node->next;
    node->next = NULL;
    node->prev = NULL;
}