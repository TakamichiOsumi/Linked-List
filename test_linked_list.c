#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "linked_list.h"

#define BUF_SIZE 64
typedef struct employee {
    int id;
    char name[BUF_SIZE];
} employee;

static void
test_employee_id(employee *e, int expected){
    if (e == NULL){
	fprintf(stderr, "input employee data is null. exit for failure.\n");
	exit(-1);
    }

    if (e->id != expected){
	fprintf(stderr, "expected the employee id to be %d, but it was %d.\n",
		expected, e->id);
	exit(-1);
    }
}

static bool
employee_key_match(void *data, void *key){
    employee *e = (employee *) data; /* 'data' was (void *) &employee. */

    if (e->id == (uintptr_t) key)
	return true;
    else
	return false;
}

/* Do nothing, since there is no dynamic member in employee */
static void
employee_free(void *data){}

static void
test_basic_operations(void){
    employee e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxx" },
	e5 = { 5, "yyy" };
    linked_list *ll;

    ll = ll_init(employee_key_match, employee_free);

    assert(ll_is_empty(ll));
    ll_insert(ll, (void *) &e1);
    ll_insert(ll, (void *) &e2);
    ll_insert(ll, (void *) &e3);
    ll_insert(ll, (void *) &e4);
    ll_insert(ll, (void *) &e5);
    assert(ll_get_length(ll) == 5);

    assert(ll_search_by_key(ll, (void *) 3) != NULL);
    assert(ll_search_by_key(ll, (void *) 0) == NULL);

    /* Remove a node in the middle of the list */
    ll_remove(ll, (void *) 3);
    assert(ll_search_by_key(ll, (void *) 3) == NULL);
    assert(ll_get_length(ll) == 4);
    /* Remove the first node */
    ll_remove(ll, (void *) 1);
    assert(ll_get_length(ll) == 3);
    /* Remove the last node */
    ll_remove(ll, (void *) 5);
    assert(ll_get_length(ll) == 2);
    ll_remove(ll, (void *) 2);
    ll_remove(ll, (void *) 4);
    assert(ll_get_length(ll) == 0);

    ll_destroy(ll);

    printf("done with the basic tests...\n");
}

static void
test_get_first_operation(void){
    employee e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxx" },
	e5 = { 5, "yyy" };
    linked_list *ll;

    ll = ll_init(employee_key_match, employee_free);

    assert(ll_is_empty(ll));
    ll_insert(ll, (void *) &e1);
    ll_insert(ll, (void *) &e2);
    ll_insert(ll, (void *) &e3);
    ll_insert(ll, (void *) &e4);
    ll_insert(ll, (void *) &e5);

    test_employee_id((employee *) ll_get_first_node(ll)->data, 5);
    test_employee_id((employee *) ll_get_first_node(ll)->data, 4);
    test_employee_id((employee *) ll_get_first_node(ll)->data, 3);
    test_employee_id((employee *) ll_get_first_node(ll)->data, 2);
    test_employee_id((employee *) ll_get_first_node(ll)->data, 1);

    printf("done with the tests to get first node...\n");

    ll_destroy(ll);
}

static void
test_clean_up_operation(void){
    employee e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxx" },
	e5 = { 5, "yyy" };
    linked_list *ll;

    ll = ll_init(employee_key_match, employee_free);

    assert(ll_is_empty(ll));

    ll_insert(ll, (void *) &e1);
    ll_insert(ll, (void *) &e2);
    ll_insert(ll, (void *) &e3);
    ll_insert(ll, (void *) &e4);
    ll_insert(ll, (void *) &e5);

    if (ll_get_length(ll) != 5){
	fprintf(stderr, "test to insert nodes has failed\n");
	exit(-1);
    }

    ll_remove_all(ll);

    if (ll_get_length(ll) != 0){
	fprintf(stderr, "test to remove all nodes has failed\n");
	exit(-1);
    }

    printf("done with the tests to remove nodes...\n");
}


int
main(void){

    printf("<test basic operations>\n");
    test_basic_operations();

    printf("<test operation to get first node>\n");
    test_get_first_operation();

    printf("<test clean-up operation>\n");
    test_clean_up_operation();

    return 0;
}
