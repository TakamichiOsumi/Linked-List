#ifndef __LINKED_LIST__
#define __LINKED_LIST__

#include <stdbool.h>

typedef struct node {
    void *data;
    struct node *next;
} node;

typedef struct linked_list {
    uintptr_t node_count;
    node *head;
    bool (*key_compare_cb)(void *data, void *key);
} linked_list;

linked_list *ll_init(bool (*search_cb)(void *p, void *key));
bool ll_is_empty(linked_list *ll);
unsigned int ll_get_length(linked_list *ll);
void ll_insert(linked_list *ll, void *p);
node *ll_search_by_key(linked_list *ll, void *key);
node *ll_remove(linked_list *ll, void *key);
void ll_remove_all(linked_list **ll);

#endif
