#include <cmsis.h>

struct list {
    void *data;
    struct list *prev, *next;
};

struct list *list_node_create(void *);
struct list *list_add_node(struct list *head, struct list *node);
struct list *
list_add_node_func_based(struct list *head, struct list *node, uint32_t (*comp)(struct list *, struct list *));
struct list *list_rem_head(struct list *head);
struct list *list_add_node_prepend(struct list *head, struct list *node);
struct list *list_rem_node(struct list *head ,struct list *node);