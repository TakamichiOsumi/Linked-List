#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

#define BUF_SIZE 64
typedef struct employee {
    unsigned int id;
    char name[BUF_SIZE];
} employee;

bool
employee_key_match(void *data, void *key){
    employee *e = (employee *) data; /* 'data' was (void *) &employee. */

    if (e->id == (uintptr_t) key)
	return true;
    else
	return false;
}
    

void
test_basic_operations(void){
    employee e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxx" },
	e5 = { 5, "yyy" };
    linked_list *ll;

    ll = ll_init(employee_key_match);

    assert(ll_is_empty(ll));
    ll_insert(ll, (void *) &e1);
    ll_insert(ll, (void *) &e2);
    ll_insert(ll, (void *) &e3);
    ll_insert(ll, (void *) &e4);
    ll_insert(ll, (void *) &e5);
    assert(ll_get_length(ll) == 5);

    assert(ll_search_by_key(ll, (void *) 3) != NULL);
    assert(ll_search_by_key(ll, (void *) 0) == NULL);

    ll_remove(ll, (void *) 3);
    assert(ll_search_by_key(ll, (void *) 3) == NULL);
    assert(ll_get_length(ll) == 4);
    ll_remove(ll, (void *) 1);
    assert(ll_get_length(ll) == 3);
    ll_remove(ll, (void *) 5);
    assert(ll_get_length(ll) == 2);

    printf("done with the basic tests...\n");
}

int
main(void){

    printf("<test basic operations>\n");
    test_basic_operations();

    return 0;
}
