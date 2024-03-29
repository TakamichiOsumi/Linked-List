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
ll_init(bool (*key_compare_cb)(void *p, void *key),
	void (*free_cb)(void *data)){
    linked_list *new_ll;

    if ((new_ll = (linked_list *) malloc(sizeof(linked_list))) == NULL){
	perror("malloc");
	exit(-1);
    }

    new_ll->node_count = 0;
    new_ll->head = NULL;
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

node *
ll_get_first_node(linked_list *ll){
    node *n = NULL;

    if (ll == NULL)
	return NULL;

    if (ll->head != NULL){
	n = ll->head;
	ll->head = ll->head->next;
	ll->node_count--;
    }

    return n;
}

node *
ll_search_by_key(linked_list *ll, void *key){
    node *n;

    if (!ll || !ll->head || !key)
	return NULL;

    n = ll->head;
    while(n){
	if (ll->key_compare_cb(n->data, key)){
	    return n;
	}
	n = n->next;
    }

    return NULL;
}

node *
ll_remove(linked_list *ll, void *key){
    bool found = false;
    node *prev, *cur;

    if (!ll || !key || !ll->head || !ll->key_compare_cb)
	return NULL;

    prev = cur = ll->head;
    while(cur){
	if (ll->key_compare_cb(cur->data, key)){
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
	ll->head = cur->next == NULL ? NULL : cur->next;
    }else if (cur->next == NULL){
	/* the last node */
	assert(prev != NULL);
	prev->next = NULL;
    }else{
	/* a node in the middle of other nodes */
	prev->next = cur->next;
    }

    ll->node_count--;
    return cur;
}

void
ll_remove_all(linked_list *ll){
    node *n;

    while(true){
	n = ll_get_first_node(ll);

	if (!n){
	    /* we are done */
	    break;
	}else{
	    /* found a node */
	    if (ll->free_cb){
		ll->free_cb(n);
	    }
	    n->data = NULL;
	    n->next = NULL;
	    free(n);
	}
    }
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

node *
ll_get_iter_node(linked_list *ll){
    node *n;

    assert(ll->iter_in_progress == true);

    ll->iter_index++;
    n = ll->current_node;

    /* If the next node is null, then return null */
    if (n == NULL)
	return NULL;
    else{
	/* Shift the node to the next one for the next call */
	n = ll->current_node;
	ll->current_node = ll->current_node->next;
	return n;
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
    if (ll == NULL || ll->head == NULL)
	return;

    ll_remove_all(ll);

    assert(ll_get_length(ll) == 0);

    free(ll);
}
