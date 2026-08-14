struct list {
    void *data;
    struct list *prev, *next;
};

struct list *list_node_create(void *);
struct list *list_add_node(struct list *head, struct list *node);