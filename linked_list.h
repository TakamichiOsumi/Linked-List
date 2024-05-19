#ifndef __LINKED_LIST__
#define __LINKED_LIST__

#include <stdbool.h>

typedef struct node {
    void *data;
    struct node *next;
} node;

/*
 * Expect only one caller just for now.
 */
typedef struct linked_list {
    uintptr_t node_count;
    node *head;

    /* Specify how to access key of application data */
    void *(*key_access_cb)(void *data);

    /*
     * Must return
     * -1 when key1 < key2,
     *  0 when key1 == key2 and
     *  1 when key1 > key2.
     */
    int (*key_compare_cb)(void *key1, void *key2);

    void (*free_cb)(void *data);

    /* internal members for iteration */
    node *current_node;
    bool iter_in_progress;
} linked_list;

linked_list *ll_init(void *(*key_access_cb)(void *data),
		     int (*key_compare_cb)(void *key1,
					   void *key2),
		     void (*free_cb)(void *data));

bool ll_is_empty(linked_list *ll);
unsigned int ll_get_length(linked_list *ll);

/* basic operations (insert, delete, search ... etc) */
void ll_insert(linked_list *ll, void *p);
void ll_tail_insert(linked_list *ll, void *p);
void ll_asc_insert(linked_list *ll, void *p);
void ll_index_insert(linked_list *ll, void *p, int index);
void *ll_remove_first_data(linked_list *ll);
void *ll_ref_index_data(linked_list *ll, int index);
void *ll_search_by_key(linked_list *ll, void *key);
void *ll_remove_by_key(linked_list *ll, void *key);
void *ll_tail_remove(linked_list *ll);
void ll_remove_all(linked_list *ll);

/* some extra features */
linked_list *ll_split(linked_list *ll, int no_nodes);
linked_list *ll_merge(linked_list *ll1, linked_list *ll2);

/* iteration feature */
void ll_begin_iter(linked_list *ll);
void *ll_get_iter_node(linked_list *ll);
void ll_end_iter(linked_list *ll);

void ll_destroy(linked_list *ll);

#endif
