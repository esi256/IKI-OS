#include <list.h>
#include <cmsis.h>
#include <mem.h>

struct list *list_node_create(void *data)
{
    struct list *t = kalloc(sizeof(struct list));
    t->data = data;
    t->next = NULL;
    t->prev = NULL;
    return t;
}

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

void list_rem_node(void)
{
    return;
}

void list_remove(void)
{
    return;
}