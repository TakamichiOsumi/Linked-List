#include <assert.h>
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
ll_init(bool (*key_compare_cb)(void *p, void *key)){
    linked_list *new_ll;

    if ((new_ll = (linked_list *) malloc(sizeof(linked_list))) == NULL){
	perror("malloc");
	exit(-1);
    }

    new_ll->node_count = 0;
    new_ll->head = NULL;
    new_ll->key_compare_cb = key_compare_cb;

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
ll_remove_all(linked_list **ll){
    /* todo */
}
