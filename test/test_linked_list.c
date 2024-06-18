#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../linked_list.h"

#define BUF_SIZE 64
typedef struct employee {
    uintptr_t id;
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
		(int) expected, (int) e->id);
	exit(-1);
    }
}

static void*
employee_key_access(void *data){
    employee *e;

    assert(data != NULL);
    e = (employee *) data;

    return (void *) e->id;
}

static int
employee_key_match(void *key1, void *key2){
    uintptr_t k1 = (uintptr_t) key1,
	k2 = (uintptr_t) key2;

    /* printf("\t %lu vs %lu\n", k1, k2); */

    if (k1 < k2){
	return -1;
    }else if (k1 == k2){
	return 0;
    }else{
	return 1;
    }
}

/* Do nothing, since there is no dynamic member in employee */
static void
employee_free(void *data){}

static void
employee_print(void *data){
    employee *e = (employee *) data;

    printf("id = %d, name = %s\n",
	   (int) e->id, e->name);
}

static void
test_basic_operations(void){
    employee e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxx" },
	e5 = { 5, "yyy" };
    linked_list *ll;

    ll = ll_init(employee_key_access, employee_key_match,
		 employee_free);

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
    ll_remove_by_key(ll, (void *) 3);
    assert(ll_search_by_key(ll, (void *) 3) == NULL);
    assert(ll_get_length(ll) == 4);
    /* Remove the first node */
    ll_remove_by_key(ll, (void *) 1);
    assert(ll_get_length(ll) == 3);
    /* Remove the last node */
    ll_remove_by_key(ll, (void *) 5);
    assert(ll_get_length(ll) == 2);
    ll_remove_by_key(ll, (void *) 2);
    ll_remove_by_key(ll, (void *) 4);
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

    ll = ll_init(employee_key_access,
		 employee_key_match, employee_free);

    assert(ll_is_empty(ll));
    ll_insert(ll, (void *) &e1);
    ll_insert(ll, (void *) &e2);
    ll_insert(ll, (void *) &e3);
    ll_insert(ll, (void *) &e4);
    ll_insert(ll, (void *) &e5);

    test_employee_id((employee *) ll_remove_first_data(ll), 5);
    test_employee_id((employee *) ll_remove_first_data(ll), 4);
    test_employee_id((employee *) ll_remove_first_data(ll), 3);
    test_employee_id((employee *) ll_remove_first_data(ll), 2);
    test_employee_id((employee *) ll_remove_first_data(ll), 1);

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

    ll = ll_init(employee_key_access, employee_key_match,
		 employee_free);

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

static void
test_iteration_operation(void){
    employee *iter,
	e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" };
    linked_list *ll;

    ll = ll_init(employee_key_access, employee_key_match,
		 employee_free);

    /* Test 1 : executing begin/end functions without any operation */
    ll_begin_iter(ll);
    ll_end_iter(ll);

    /* Test 2 : executing ll_get_iter_data without any data */
    ll_begin_iter(ll);
    if (ll_get_iter_data(ll) != NULL){
	fprintf(stderr, "invalid node has been returned\n");
	exit(-1);
    }
    ll_end_iter(ll);

    /* Test 3 : executing ll_get_iter_data for one data */
    ll_insert(ll, (void *) &e1);
    ll_begin_iter(ll);
    if ((iter = ll_get_iter_data(ll)) == NULL){
	fprintf(stderr, "inserted node has not been returned\n");
	exit(-1);
    }
    if ((iter = ll_get_iter_data(ll)) != NULL){
	fprintf(stderr, "non-existent node has been returned\n");
	exit(-1);
    }
    ll_end_iter(ll);

    /* Test 4 : executing ll_get_iter_data for multiple nodes */
    ll_insert(ll, (void *) &e2);
    ll_insert(ll, (void *) &e3);

    printf("Dump three nodes\n");
    ll_begin_iter(ll);
    while((iter = (employee *) ll_get_iter_data(ll)) != NULL){
	employee_print(iter);
    }
    ll_end_iter(ll);

    /* Test 5 : break in the middle of iteration and close the iteration */
    printf("Break in the middle of iteration\n");
    ll_begin_iter(ll);
    while((iter = (employee *) ll_get_iter_data(ll)) != NULL){
	employee_print(iter);
	if (iter->id == 2){
	    break;
	}
    }
    ll_end_iter(ll);

    ll_destroy(ll);

    printf("done with the tests to iterate nodes...\n");
}

static void
test_tail_insert(void){
    linked_list *ll;
    employee *iter,
	e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxxx" },
	e5 = { 5, "yyyy" };

    int expected_val = 1;

    ll = ll_init(employee_key_access, employee_key_match,
		 employee_free);
    ll_tail_insert(ll, (void *) &e1);
    ll_tail_insert(ll, (void *) &e2);
    ll_tail_insert(ll, (void *) &e3);
    ll_tail_insert(ll, (void *) &e4);
    ll_tail_insert(ll, (void *) &e5);

    assert(ll_get_length(ll) == 5);
    ll_begin_iter(ll);
    while((iter = (employee *) ll_get_iter_data(ll)) == NULL){
	assert(iter->id == expected_val);
	expected_val++;
    }
    ll_end_iter(ll);
    ll_destroy(ll);
}

static void
test_asc_order_insert(void){
    linked_list *ll;
    employee *iter,
	e0 = { 0, "abc" },
	e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxxx" },
	e5 = { 5, "yyyy" };
    uintptr_t expected_id = 0;

    ll = ll_init(employee_key_access, employee_key_match,
		 employee_free);

    ll_asc_insert(ll, (void *) &e2);
    ll_asc_insert(ll, (void *) &e1);
    ll_asc_insert(ll, (void *) &e0);
    ll_asc_insert(ll, (void *) &e4);
    ll_asc_insert(ll, (void *) &e5);
    ll_asc_insert(ll, (void *) &e3);
    assert(ll_get_length(ll) == 6);

    ll_begin_iter(ll);
    while((iter = (employee *) ll_get_iter_data(ll)) != NULL){
	assert(iter->id == expected_id);
	expected_id++;
    }
    ll_end_iter(ll);

    ll_remove_all(ll);

    ll_destroy(ll);
}

/* bug fix */
static void
test_asc_order_insert_v2(void){
    linked_list *ll;
    employee *iter,
	e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxxx" };
    uintptr_t expected_id = 1;

    ll = ll_init(employee_key_access, employee_key_match,
		 employee_free);

    ll_asc_insert(ll, (void *) &e4);
    ll_asc_insert(ll, (void *) &e1);
    ll_asc_insert(ll, (void *) &e2);
    ll_asc_insert(ll, (void *) &e3);
    assert(ll_get_length(ll) == 4);

    ll_begin_iter(ll);
    while((iter = (employee *) ll_get_iter_data(ll)) != NULL){
	employee_print(iter);
	assert(iter->id == expected_id);
	expected_id++;
    }
    ll_end_iter(ll);

    ll_remove_all(ll);

    ll_destroy(ll);
}

static void
test_merge_lists(void){
    linked_list *ll1, *ll2, *merged1, *merged2;
    employee *iter,
	e0 = { 0, "abc" },
	e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxxx" },
	e5 = { 5, "yyyy" },
	e6 = { 6, "zzzz" },
	e7 = { 7, "xyz"  },
	e8 = { 8, "xyzz" };
    uintptr_t expected_id = 0;

    ll1 = ll_init(employee_key_access, employee_key_match,
		  employee_free);
    ll2 = ll_init(employee_key_access, employee_key_match,
		  employee_free);

    ll_asc_insert(ll1, (void *) &e0);
    ll_asc_insert(ll1, (void *) &e1);
    ll_asc_insert(ll1, (void *) &e2);
    ll_asc_insert(ll2, (void *) &e3);
    ll_asc_insert(ll2, (void *) &e4);
    ll_asc_insert(ll2, (void *) &e5);
    ll_asc_insert(ll1, (void *) &e6);
    ll_asc_insert(ll2, (void *) &e7);
    ll_asc_insert(ll2, (void *) &e8);

    /*
     * ll1 : 0, 1, 2, 6
     * ll2 : 3, 4, 5, 7, 8
     */
    merged1 = ll_merge(ll1, ll2);
    ll_begin_iter(merged1);
    while((iter = (employee *) ll_remove_first_data(merged1)) != NULL){
	employee_print(iter);
	assert(expected_id == iter->id);
	expected_id++;
    }
    ll_end_iter(merged1);
    ll_destroy(merged1);

    /* Test by swapping two lists */
    ll_asc_insert(ll1, (void *) &e0);
    ll_asc_insert(ll1, (void *) &e1);
    ll_asc_insert(ll1, (void *) &e2);
    ll_asc_insert(ll2, (void *) &e3);
    ll_asc_insert(ll2, (void *) &e4);
    ll_asc_insert(ll2, (void *) &e5);
    ll_asc_insert(ll1, (void *) &e6);
    ll_asc_insert(ll2, (void *) &e7);
    ll_asc_insert(ll2, (void *) &e8);
    expected_id = 0;

    /* swapped */
    merged2 = ll_merge(ll2, ll1);
    ll_begin_iter(merged2);
    while((iter = (employee *) ll_remove_first_data(merged2)) != NULL){
	assert(expected_id == iter->id);
	employee_print(iter);
	expected_id++;
    }
    ll_end_iter(merged2);
    ll_destroy(merged2);
}

static void
test_split_list(void){
    linked_list *ll1, *ll2;
    employee *iter,
	e0 = { 0, "abc" },
	e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxxx" },
	e5 = { 5, "yyyy" },
	e6 = { 6, "zzzz" },
	e7 = { 7, "xyz"  },
	e8 = { 8, "xyzz" };
    uintptr_t expected_id = 0;
    int split_no = 3;

    ll1 = ll_init(employee_key_access, employee_key_match,
		  employee_free);

    ll_asc_insert(ll1, (void *) &e0);
    ll_asc_insert(ll1, (void *) &e1);
    ll_asc_insert(ll1, (void *) &e2);
    ll_asc_insert(ll1, (void *) &e3);
    ll_asc_insert(ll1, (void *) &e4);
    ll_asc_insert(ll1, (void *) &e5);
    ll_asc_insert(ll1, (void *) &e6);
    ll_asc_insert(ll1, (void *) &e7);
    ll_asc_insert(ll1, (void *) &e8);

    ll2 = ll_split(ll1, split_no);

    /* dump ll2 */
    ll_begin_iter(ll2);
    while((iter = ll_get_iter_data(ll2)) != NULL){
	employee_print(iter);
	assert(expected_id == iter->id);
	expected_id++;
    }
    ll_end_iter(ll2);

    assert(ll_get_length(ll2) == split_no);

    /* dump ll1 */
    ll_begin_iter(ll1);
    while((iter = ll_get_iter_data(ll1)) != NULL){
	employee_print(iter);
	assert(expected_id == iter->id);
	expected_id++;
    }
    ll_end_iter(ll1);

    ll_destroy(ll1);
    ll_destroy(ll2);
}

static void
test_merge_lists_bug(){
    linked_list *ll1, *ll2, *merged;
    employee *emp,
	e1 = { 1, "foo" };

    ll1 = ll_init(employee_key_access, employee_key_match,
		  employee_free);
    ll2 = ll_init(employee_key_access, employee_key_match,
		  employee_free);

    ll_asc_insert(ll2, (void *) &e1);

    merged = ll_merge(ll1, ll2);
    assert(ll_get_length(merged) == 1);
    emp = (employee *) merged->head->data;
    assert(emp->id == 1);
    assert(strncmp(emp->name, "foo", strlen("foo")) == 0);

    ll_destroy(ll1);
    ll_destroy(ll2);
    ll_destroy(merged);
}

static void
test_index_fetch(void){
    linked_list *ll;
    employee *iter,
	e0 = { 0, "abc" },
	e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxxx" };
    int i;

    ll = ll_init(employee_key_access, employee_key_match,
		 employee_free);

    ll_asc_insert(ll, (void *) &e0);
    ll_asc_insert(ll, (void *) &e1);
    ll_asc_insert(ll, (void *) &e2);
    ll_asc_insert(ll, (void *) &e3);
    ll_asc_insert(ll, (void *) &e4);

    for (i = 0; i < ll_get_length(ll); i++){
	iter = (employee *) ll_ref_index_data(ll, i);
	employee_print(iter);
	assert(iter->id == i);
    }

    ll_destroy(ll);
}

/*
 * Enable user to store NULL in the linked list.
 *
 * Indicate iteration APIs stop by NULL, while
 * iteration by for loop with ll_ref_index_data
 * works OK.
 */
static void
test_null_node_iteration(void){
    linked_list *ll;
    employee *iter,
	e0 = { 0, "abc" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxxx" };
    int i;

    ll = ll_init(employee_key_access, employee_key_match,
		 employee_free);

    ll_tail_insert(ll, (void *) &e0);
    ll_tail_insert(ll, (void *) NULL);
    ll_tail_insert(ll, (void *) &e2);
    ll_tail_insert(ll, (void *) &e3);
    ll_tail_insert(ll, (void *) &e4);

    ll_begin_iter(ll);
    while((iter = (employee *) ll_get_iter_data(ll)) != NULL){
	employee_print(iter); /* print only first employee */
    }
    /* This will get the node after the NULL */
    iter = (employee *) ll_get_iter_data(ll);
    assert(iter != NULL);
    assert(iter->id == 2);
    ll_end_iter(ll);

    /*
     * Note:
     *
     * ll_iter_begin(), ll_end_iter() and ll_get_iter_data() works
     * with NULL. So, this is slower than the way using them to wrap
     * the iteration. But, checking the behavior of ll_ref_index_data()
     * in a for loop should be tested. Intentionally, keep this test
     * here.
     */
    for (i = 0; i < ll_get_length(ll); i++){
	if ((iter = (employee *) ll_ref_index_data(ll, i)) != NULL){
	    /* This won't stop and iterates all nodes safely */
	    employee_print(iter);
	}
    }
}

static void
test_index_insertion(void){
    linked_list *ll;
    employee *iter,
	e0 = { 0, "abc" },
	e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxxx" },
	e10 = { 10, "abc" },
	e11 = { 11, "efg" },
	e12 = { 12, "foo" },
	e13 = { 13, "aaa" },
	e14 = { 14, "bbb" },
	e15 = { 15, "ccc" };

    int ans_index = 0, expected_output[] =
	{ 10, 12, 13, 11, 0, 1, 2, 3, 4, 15, 14 };

    ll = ll_init(employee_key_access,
		 employee_key_match,
		 employee_free);

    ll_asc_insert(ll, (void *) &e0);
    ll_asc_insert(ll, (void *) &e1);
    ll_asc_insert(ll, (void *) &e2);
    ll_asc_insert(ll, (void *) &e3);
    ll_asc_insert(ll, (void *) &e4);

    ll_index_insert(ll, (void *) &e10, 0);
    ll_index_insert(ll, (void *) &e11, 1);
    ll_index_insert(ll, (void *) &e12, 1);
    ll_index_insert(ll, (void *) &e13, 2);
    ll_index_insert(ll, (void *) &e14, ll_get_length(ll));
    ll_index_insert(ll, (void *) &e15, ll_get_length(ll) - 1);

    ll_begin_iter(ll);
    while((iter = (employee *) ll_get_iter_data(ll)) != NULL){
	employee_print(iter);
	if (iter->id != expected_output[ans_index++]){
	    printf("the expected order is not the same as the list order\n");
	    exit(-1);
	}
    }
    ll_end_iter(ll);

    ll_destroy(ll);
}

static void
test_for_loop_iter(){
    linked_list *ll;
    employee *last, *iter,
	e0 = { 0, "abc" },
	e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxxx" };
    int i, non_null;

    ll = ll_init(employee_key_access,
		 employee_key_match,
		 employee_free);

    ll_insert(ll, NULL);
    ll_insert(ll, (void *) &e0);
    ll_insert(ll, (void *) &e1);
    ll_insert(ll, NULL);
    ll_insert(ll, (void *) &e2);
    ll_insert(ll, NULL);
    ll_insert(ll, NULL);
    ll_insert(ll, (void *) &e3);
    ll_insert(ll, (void *) &e4);
    ll_insert(ll, NULL);
    assert(ll_get_length(ll) == 10);

    non_null = 0;
    ll_begin_iter(ll);
    for (i = 0; i < ll_get_length(ll); i++){
	if ((iter = ll_get_iter_data(ll)) == NULL)
	    continue;
	last = iter;
	non_null++;
    }
    ll_end_iter(ll);

    assert(non_null == 5);
    assert(last->id == 0);
}

static void
test_tail_remove(void){
    linked_list *ll;
    employee *e,
	e0 = { 0, "abc" },
	e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxxx" };

    ll = ll_init(employee_key_access,
		 employee_key_match,
		 employee_free);

    /* ll_tail_remove() for no data returns NULL */
    assert(ll_tail_remove(ll) == NULL);

    ll_tail_insert(ll, (void *) &e0);
    e = (employee *) ll_tail_remove(ll);
    assert(e->id == 0);

    ll_tail_insert(ll, (void *) &e1);
    ll_tail_insert(ll, (void *) &e2);
    ll_tail_insert(ll, (void *) &e3);
    ll_tail_insert(ll, (void *) &e4);

    e = (employee *) ll_tail_remove(ll);
    assert(e->id == 4);
    e = (employee *) ll_tail_remove(ll);
    assert(e->id == 3);
    e = (employee *) ll_tail_remove(ll);
    assert(e->id == 2);
    e = (employee *) ll_tail_remove(ll);
    assert(e->id == 1);

    assert(ll_get_length(ll) == 0);
    e = (employee *) ll_tail_remove(ll);
    assert(e == NULL);

    ll_destroy(ll);
}

static void
test_key_replacement(void){
    linked_list *ll;
    employee *e,
	e0 = { 0, "abc" },
	e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxxx" },
	e10 = { 10, "foo" };
    int i;

    ll = ll_init(employee_key_access,
		 employee_key_match,
		 employee_free);

    ll_tail_insert(ll, (void *) &e0);
    ll_tail_insert(ll, (void *) &e1);
    ll_tail_insert(ll, (void *) &e2);
    ll_tail_insert(ll, (void *) &e10); /* target */
    ll_tail_insert(ll, (void *) &e4);

    /* failure */
    assert(ll_replace_by_key(ll, (void *) 100, (void *) &e3) == NULL);
    /* success */
    assert(ll_replace_by_key(ll, (void *) 10, (void *) &e3) != NULL);

    ll_begin_iter(ll);
    for (i = 0; i < ll_get_length(ll); i++){
	e = ll_get_iter_data(ll);

	employee_print(e);
	if (e->id != i){
	    fprintf(stderr,
		    "the result is not same as the expectation. %lu vs. %lu\n",
		    e->id, (uintptr_t) i);
	    exit(-1);
	}
    }
    ll_end_iter(ll);

    ll_destroy(ll);
}

static void
test_index_removal(void){
    linked_list *ll;
    employee *e,
	e0 = { 0, "abc" },
	e1 = { 1, "foo" },
	e2 = { 2, "bar" },
	e3 = { 3, "bazz" },
	e4 = { 4, "xxxx" },
	e5 = { 5, "aaaa" },
	e6 = { 6, "bbbb" },
	e7 = { 7, "cccc" };
    uintptr_t answers[] = { 1, 3, 4, 5, 6 };
    int i;

    ll = ll_init(employee_key_access,
		 employee_key_match,
		 employee_free);

    ll_tail_insert(ll, (void *) &e0); /* will be removed */
    ll_tail_insert(ll, (void *) &e1);
    ll_tail_insert(ll, (void *) &e2); /* will be removed */
    ll_tail_insert(ll, (void *) &e3);
    ll_tail_insert(ll, (void *) &e4);
    ll_tail_insert(ll, (void *) &e5);
    ll_tail_insert(ll, (void *) &e6);
    ll_tail_insert(ll, (void *) &e7); /* will be removed */
    assert(ll_get_length(ll) == 8);

    /* failure */
    assert(ll_index_remove(ll, -1) == NULL);
    assert(ll_index_remove(ll, 10) == NULL);

    /* success */
    e = (employee *) ll_index_remove(ll, 0);
    assert(e->id == 0);
    assert(ll_get_length(ll) == 7);

    e = (employee *) ll_index_remove(ll, ll_get_length(ll) - 1);
    assert(e->id == 7);
    assert(ll_get_length(ll) == 6);

    e = (employee *) ll_index_remove(ll, 1);
    assert(e->id == 2);
    assert(ll_get_length(ll) == 5);

    ll_begin_iter(ll);
    for (i = 0; i < ll_get_length(ll); i++){
	e = (employee *) ll_get_iter_data(ll);
	employee_print(e);

	if (e->id != answers[i]){
	    fprintf(stderr,
		    "the result is not same as the expectation. %lu vs. %lu\n",
		    e->id, answers[i]);
	    exit(-1);
	}
    }
    ll_end_iter(ll);

    ll_destroy(ll);
}

static void
test_key_existence(void){
    linked_list *ll;
    employee e0 = { 0, "abc" },
	e1 = { 1, "foo" },
	e2 = { 2, "bar" };

    ll = ll_init(employee_key_access,
		 employee_key_match,
		 employee_free);
    ll_tail_insert(ll, (void *) &e0);
    ll_tail_insert(ll, (void *) &e1);
    ll_tail_insert(ll, (void *) &e2);

    assert(ll_has_key(ll, (void *) -1) == false);
    assert(ll_has_key(ll, (void *) 0) == true);
    assert(ll_has_key(ll, (void *) 3) == false);

    ll_destroy(ll);
}

static void
run_bundled_tests(void){
    printf("<test basic operations>\n");
    test_basic_operations();

    printf("<test operation to get first node>\n");
    test_get_first_operation();

    printf("<test clean-up operation>\n");
    test_clean_up_operation();

    printf("<test iteration operation>\n");
    test_iteration_operation();

    printf("<test tail insert>\n");
    test_tail_insert();

    printf("<test ascending order insert>\n");
    test_asc_order_insert();

    printf("<test ascending order insert v2>\n");
    test_asc_order_insert_v2();

    printf("<test merge>\n");
    test_merge_lists();

    /* <bug fix>
     *
     * When either list is empty, merge failed
     * and some junk data was inserted to the
     * result list. The junk value was set
     * at initization and not null, then
     * it was inserted by ll_tail_insert()
     * just after the break. Fixed.
     */
    printf("<reproduce bug>\n");
    test_merge_lists_bug();

    printf("<test split>\n");
    test_split_list();

    printf("<test index fetch>\n");
    test_index_fetch();

    printf("<test null node iteration>\n");
    test_null_node_iteration();

    printf("<test index insertion>\n");
    test_index_insertion();

    printf("<test combination of iteration API and for loop>\n");
    test_for_loop_iter();

    printf("<test tail removal>\n");
    test_tail_remove();

    printf("<test key replacement>\n");
    test_key_replacement();

    printf("<test index removal>\n");
    test_index_removal();

    printf("<test key existence>\n");
    test_key_existence();
}

int
main(void){

    run_bundled_tests();

    printf("All tests are done gracefully\n");

    return 0;
}
