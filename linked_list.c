#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

static node*
ll_gen_node(void *p){
    node *n;

    if ((n = (node *) malloc(sizeof(node))) == NULL){
	perror("malloc");
	exit(-1);
    }

    n->data = p;
    n->next = NULL;

    return n;
}

linked_list *
ll_init(void *(*key_access_cb)(void *data),
	int (*key_compare_cb)(void *data, void *key),
	void (*free_cb)(void *data)){
    linked_list *new_ll;

    if ((new_ll = (linked_list *) malloc(sizeof(linked_list))) == NULL){
	perror("malloc");
	exit(-1);
    }

    new_ll->node_count = 0;
    new_ll->head = NULL;
    new_ll->key_access_cb = key_access_cb;
    new_ll->key_compare_cb = key_compare_cb;
    new_ll->free_cb = free_cb;

    new_ll->current_node = NULL;
    new_ll->iter_in_progress = false;

    return new_ll;
}

bool
ll_is_empty(linked_list *ll){
    assert(ll != NULL);
	
    if (ll->node_count == 0)
	return true;
    else
	return false;
}

int
ll_get_length(linked_list *ll){
    return ll->node_count;
}

void
ll_insert(linked_list *ll, void *data){
    node *new_node;

    if (!ll)
	return;

    new_node = ll_gen_node(data);
    ll->node_count++;
    if (!ll->head){
	ll->head = new_node;
	return;
    }

    new_node->next = ll->head;
    ll->head = new_node;
}

void
ll_tail_insert(linked_list *ll, void *data){
    node *new_node;

    if (!ll)
	return;

    new_node = ll_gen_node(data);
    ll->node_count++;

    if (!ll->head){
	ll->head = new_node;
	return;
    }else{
	node *curr, *prev;

	prev = curr = ll->head;
	while(curr != NULL){
	    prev = curr;
	    curr = curr->next;
	}
	prev->next = new_node;
    }
}

void *
ll_remove_first_data(linked_list *ll){
    if (ll == NULL || ll->head == NULL)
	return NULL;
    else{
	node *n;
	void *p;

	n = ll->head;
	ll->head = ll->head->next;
	ll->node_count--;

	/* clean up */
	p = n->data;
	n->next = NULL;
	n->data = NULL;
	free(n);

	return p;
    }
}

void *
ll_ref_index_data(linked_list *ll, int index){
    node *n;
    int iter;

    if (ll == NULL || ll->head == NULL ||
	index < 0 || ll_get_length(ll) < index){
	return NULL;
    }

    n = ll->head;
    for (iter = 0; iter < ll_get_length(ll); iter++){
	if (iter == index)
	    return n->data;
	n = n->next;
    }

    assert(0);

    return NULL;
}

/* Don't remove the hit node from the list */
void*
ll_search_by_key(linked_list *ll, void *key){
    node *n;

    if (!ll || !ll->head || !key ||
	!ll->key_access_cb || !ll->key_compare_cb)
	return NULL;

    n = ll->head;
    while(n){
	if (ll->key_compare_cb(ll->key_access_cb(n->data),
			       key) == 0){
	    return n->data;
	}
	n = n->next;
    }

    return NULL;
}

void *
ll_remove_by_key(linked_list *ll, void *key){
    bool found = false;
    node *prev, *cur;
    void *p;

    if (!ll || !key || !ll->head ||
	!ll->key_access_cb || !ll->key_compare_cb)
	return NULL;

    prev = cur = ll->head;
    while(cur){
	if (ll->key_compare_cb(ll->key_access_cb(cur->data),
			       key) == 0){
	    found = true;
	    break;
	}
	prev = cur;
	cur = cur->next;
    }

    if (!found)
	return NULL;

    if (ll->head == cur){
	/* the first node */
	ll->head = cur->next;
	cur->next = NULL;
	p = cur->data;
    }else if (cur->next == NULL){
	/* the last node */
	assert(prev != NULL);
	prev->next = NULL;
	p = cur->data;
    }else{
	/* a node in the middle of other nodes */
	prev->next = cur->next;
	cur->next = NULL;
	p = cur->data;
    }

    ll->node_count--;
    free(cur);

    return p;
}

void *
ll_replace_by_key(linked_list *ll, void *old_key, void *new_data){
    node *curr;

    if (ll == NULL || ll->head == NULL)
	return NULL;

    curr = ll->head;
    while(true){

	if (ll->key_compare_cb(old_key,
			       ll->key_access_cb(curr->data)) == 0){
	    void *tmp = curr->data;

	    curr->data = new_data;

	    return tmp;
	}

	if ((curr = curr->next) == NULL)
	    break;
    }

    return NULL;
}


void *
ll_tail_remove(linked_list *ll){
    node *prev, *curr;
    int i, list_len = ll_get_length(ll);
    void *data;

    if (ll == NULL || ll->head == NULL)
	return NULL;

    if (list_len == 1)
	return ll_remove_first_data(ll);

    ll->node_count--;

    curr = ll->head;
    for (i = 1; i <= (list_len - 1); i++){
	prev = curr;
	curr = curr->next;
    }

    assert(curr->next == NULL);
    data = curr->data;
    free(curr);
    prev->next = NULL;

    return data;
}

void
ll_remove_all(linked_list *ll){
    void *p;

    while(true){
	p = ll_remove_first_data(ll);

	/* are we done ? */
	if (!p)
	    break;

	/* found a node */
	if (ll->free_cb){
	    ll->free_cb(p);
	}
    }
}

linked_list *
ll_split(linked_list *ll, int no_nodes){
    linked_list *new_list;
    void *p;
    int i;

    if (ll_get_length(ll) < no_nodes){
	return ll;
    }else if (no_nodes <= 0){
	return ll;
    }

    new_list = ll_init(ll->key_access_cb,
		       ll->key_compare_cb,
		       ll->free_cb);

    for (i = 0; i < no_nodes; i++){
	p = ll_remove_first_data(ll);
	ll_tail_insert(new_list, p);
    }

    return new_list;
}

/*
 * If either list is empty, move the left keys to the
 * newly created linked list. In any case, drain both
 * input lists completely.
 */
linked_list *
ll_merge(linked_list *ll1, linked_list *ll2){
    void *d1 = NULL, *d2 = NULL;
    linked_list *result;
    int cmp;
    bool d1_shift, d2_shift;

    assert(ll1->key_access_cb == ll2->key_access_cb);
    assert(ll1->key_compare_cb == ll2->key_compare_cb);
    assert(ll1->free_cb == ll2->free_cb);

    result = ll_init(ll1->key_access_cb,
		     ll1->key_compare_cb,
		     ll1->free_cb);

    /* Handle the cases of empty list */
    if (ll_get_length(ll1) == 0 && ll_get_length(ll2) == 0)
	return result;

    if (ll_get_length(ll1) == 0 && ll_get_length(ll2) != 0){
	while(ll_get_length(ll2) > 0)
	    ll_tail_insert(result, ll_remove_first_data(ll2));
	return result;
    }

    if (ll_get_length(ll1) != 0 && ll_get_length(ll2) == 0){
	while(ll_get_length(ll1) > 0)
	    ll_tail_insert(result, ll_remove_first_data(ll1));
	return result;
    }

    /* We have two lists with data to merge */
    d1_shift = d2_shift = true;
    do {
	if (d1_shift){
	    d1 = ll_remove_first_data(ll1);
	    if (d1 == NULL)
		break;
	}

	if (d2_shift){
	    d2 = ll_remove_first_data(ll2);
	    if (d2 == NULL)
		break;
	}

	d1_shift = d2_shift = false;

	cmp = result->key_compare_cb(result->key_access_cb(d1),
				     result->key_access_cb(d2));

	if (cmp == -1 || cmp == 0){
	    /* d1 key < d2 key or those are equal */
	    ll_tail_insert(result, d1);
	    d1_shift = true;
	}else{
	    /* d1 key > d2 key */
	    ll_tail_insert(result, d2);
	    d2_shift = true;
	}

    } while(true);

    /*
     * After break, we have one element left in
     * either d1 or d2. Don't forget to add the left
     * one.
     */
    if (d1 != NULL)
	ll_tail_insert(result, d1);
    if (d2 != NULL)
	ll_tail_insert(result, d2);

    /*
     * Either input list is now empty,
     * but the other one isn't. Continue to insert
     * until the remaining list becomes empty.
     */
    while(ll_get_length(ll1) > 0)
	ll_tail_insert(result, ll_remove_first_data(ll1));

    while(ll_get_length(ll2) > 0)
	ll_tail_insert(result, ll_remove_first_data(ll2));

    assert(ll1->head == NULL);
    assert(ll2->head == NULL);

    return result;
}

void
ll_begin_iter(linked_list *ll){
    assert(ll->iter_in_progress == false);
    assert(ll->current_node == NULL);

    ll->iter_in_progress = true;
    /* Set the node which will be referenced next */
    ll->current_node = ll->head;
}

void *
ll_get_iter_data(linked_list *ll){
    node *n;

    assert(ll->iter_in_progress == true);

    n = ll->current_node;

    /* If the current node is null, then return null */
    if (n == NULL)
	return NULL;
    else{
	/* Shift the node to the next one for the next call */
	ll->current_node = ll->current_node->next;
	return n->data;
    }
}

void
ll_end_iter(linked_list *ll){
    assert(ll->iter_in_progress == true);

    ll->iter_in_progress = false;
    ll->current_node = NULL;
}

void
ll_destroy(linked_list *ll){
    if (ll == NULL || ll->head == NULL){
	free(ll);
	return;
    }

    ll_remove_all(ll);

    assert(ll_get_length(ll) == 0);

    free(ll);
}

/*
 * Insert an entry in ascending order and return
 * the index of inserted position.
 *
 * Return -1 on failure. The return value starts
 * from 0 as the first index.
 */
int
ll_asc_insert(linked_list *ll, void *new_data){
    node *new_node, *prev, *curr;
    bool found_larger_key = false;
    int inserted_pos = 0;

    if (!ll)
	return -1;

    new_node = ll_gen_node(new_data);

    /* If there is no node, just insert. */
    if (!ll->head){
	ll->node_count++;
	ll->head = new_node;
	return 0;
    }

    prev = curr = ll->head;
    do {
	if (ll->key_compare_cb(ll->key_access_cb(curr->data),
			       ll->key_access_cb(new_data)) == 1){
	    found_larger_key = true;
	    break;
	}
	prev = curr;
	inserted_pos++;
    } while((curr = curr->next) != NULL);

    if (found_larger_key == true){
	assert(prev != NULL);
	assert(curr != NULL);
	ll->node_count++;
	if (prev == curr){
	    /*
	     * If the smaller key is the first node,
	     * then need to reconnect the list's head
	     * as well.
	     */
	    ll->head = new_node;
	    new_node->next = curr;
	}else{
	    /*
	     * Insert before the smaller key. In other
	     * words, reconnect nodes in the order of
	     * 'prev', 'new_node' and 'curr'.
	     */
	    new_node->next = curr;
	    prev->next = new_node;
	}
    }else{
	/* Insert at the end */
	assert(curr == NULL);
	ll->node_count++;
	prev->next = new_node;
    }

    return inserted_pos;
}

void
ll_index_insert(linked_list *ll, void *new_data, int index){
    if (!ll || index < 0)
	return;

    if (ll_get_length(ll) < index)
	return;

    if (index == 0){
	ll_insert(ll, new_data);
    }else if(index == ll_get_length(ll)){
	ll_tail_insert(ll, new_data);
    }else{
	node *prev, *curr, *new_node;
	int iter = 0;

	new_node = ll_gen_node(new_data);
	prev = curr = ll->head;
	do {
	    if (iter == index){
		new_node->next = curr;
		prev->next = new_node;
		ll->node_count++;
		return;
	    }
	    prev = curr;
	    iter++;
	} while((curr = curr->next) != NULL);

	assert(0);
    }
}

void *
ll_index_remove(linked_list *ll, int index){
    node *prev, *curr;
    void *data;
    int i = 0;

    if (ll == NULL || ll->head == NULL ||
	index < 0 || ll_get_length(ll) - 1 < index)
	return NULL;

    /* The first node ? */
    if (index == 0)
	return ll_remove_first_data(ll);

    /* The last node ? */
    if (index == ll_get_length(ll) - 1)
	return ll_tail_remove(ll);

    prev = curr = ll->head;

    while(true){

	if (i == index){
	    /* This must be an internal node */
	    assert(curr->next != NULL);

	    ll->node_count--;
	    prev->next = curr->next;

	    data = curr->data;

	    /* free the node */
	    if (ll->free_cb)
		ll->free_cb(data);
	    curr->data = NULL;
	    curr->next = NULL;
	    free(curr);

	    return data;
	}

	i++;
	prev = curr;
	curr = curr->next;
	if (curr == NULL)
	    break;
    }

    assert(0);

    return NULL;
}

bool
ll_has_key(linked_list *ll, void *key)
{
    int i;
    void *p;

    if (ll == NULL || ll->head == NULL)
	return false;

    ll_begin_iter(ll);
    for (i = 0; i < ll_get_length(ll); i++){
	p = ll_get_iter_data(ll);
	if (ll->key_compare_cb(ll->key_access_cb(p),
			       key) == 0){
	    ll_end_iter(ll);
	    return true;
	}
    }
    ll_end_iter(ll);

    return false;
}
