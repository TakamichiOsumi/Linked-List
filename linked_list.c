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
    new_ll->iter_index = 0;

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

unsigned int
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
ll_get_first_node(linked_list *ll){
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

void
ll_remove_all(linked_list *ll){
    void *p;

    while(true){
	p = ll_get_first_node(ll);

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
    return NULL;
}

linked_list *
ll_merge(linked_list *ll1, linked_list *ll2){
    node *n1, *n2;
    linked_list *result;
    int cmp;
    bool n1_shift = true, n2_shift = true;

    assert(ll1->key_access_cb == ll2->key_access_cb);
    assert(ll1->key_compare_cb == ll2->key_compare_cb);
    assert(ll1->free_cb == ll2->free_cb);

    /* If either list is empty, return the other one */
    if ((ll1 != NULL && ll1->head == NULL) &&
	(ll2 != NULL && ll2->head != NULL))
	return ll2;

    if ((ll2 != NULL && ll2->head == NULL) &&
	(ll1 != NULL && ll1->head != NULL))
	return ll1;

    /* If both lists are empty, return the first one */
    if ((ll1 != NULL && ll1->head == NULL) &&
	(ll2 != NULL && ll2->head == NULL))
	return ll1;

    result = ll_init(ll1->key_access_cb,
		     ll1->key_compare_cb,
		     ll1->free_cb);

    do {
	if (n1_shift){
	    n1 = ll_get_first_node(ll1);
	    if (n1 == NULL)
		break;
	}

	if (n2_shift){
	    n2 = ll_get_first_node(ll2);
	    if (n2 == NULL)
		break;
	}

	n1_shift = n2_shift = false;

	cmp = result->key_compare_cb(result->key_access_cb(n1),
				     result->key_access_cb(n2));

	if (cmp == -1 || cmp == 0){
	    /* n1's key < n2's key or those are equal */
	    ll_tail_insert(result, n1);
	    n1_shift = true;
	}else{
	    /* n1's key > n2's key */
	    ll_tail_insert(result, n2);
	    n2_shift = true;
	}

    } while(true);

    /*
     * After break, we have one element left in
     * either n1 or n2. Don't forget to add the left
     * one.
     */
    if (n1 != NULL)
	ll_tail_insert(result, n1);
    if (n2 != NULL)
	ll_tail_insert(result, n2);

    /*
     * Either input list is now empty,
     * but the other one isn't. Continue to insert
     * until the remaining list becomes empty.
     */
    while(ll_get_length(ll1) > 0)
	ll_tail_insert(result, ll_get_first_node(ll1));

    while(ll_get_length(ll2) > 0)
	ll_tail_insert(result, ll_get_first_node(ll2));

    assert(ll1->head == NULL);
    assert(ll2->head == NULL);

    return result;
}

void
ll_begin_iter(linked_list *ll){
    assert(ll->iter_index == 0);
    assert(ll->iter_in_progress == false);
    assert(ll->current_node == NULL);

    ll->iter_in_progress = true;
    /* Set the node which will be referenced next */
    ll->current_node = ll->head;
}

void *
ll_get_iter_node(linked_list *ll){
    node *n;

    assert(ll->iter_in_progress == true);

    ll->iter_index++;
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

    ll->iter_index = 0;
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

void
ll_asc_insert(linked_list *ll, void *new_data){
    node *new_node, *prev, *curr;
    bool found_smaller_key = false;

    if (!ll)
	return;

    new_node = ll_gen_node(new_data);

    /* If there is no node, just insert. */
    if (!ll->head){
	ll->node_count++;
	ll->head = new_node;
	return;
    }

    prev = curr = ll->head;
    do {
	if (ll->key_compare_cb(ll->key_access_cb(curr->data),
			       ll->key_access_cb(new_data)) == 1){
	    found_smaller_key = true;
	    break;
	}
	prev = curr;
    } while((curr = curr->next) != NULL);

    if (found_smaller_key == true){
	assert(prev != NULL);
	assert(curr != NULL);
	ll->node_count++;
	if (prev == ll->head){
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
}
