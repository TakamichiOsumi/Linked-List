# Linked-List

Submodule utility for my other repositories.

## `liblinked_list` library functions

Two main features of this library are key comparison and iteration.

| Function | Description |
| ---- | ---- |
| ll_init | Create a new linked_list * object |
| ll_asc_insert | Insert one key value to linked_list * object in ascending order |
| ll_split | Split linked_list * object into two according to specified number |
| ll_merge | Merge two linked_list * objects in ascending order |
| ll_begin_iter | Declare an iteration of linked_list * begins |
| ll_get_iter_node | Fetch a data from linked_list * object during iteration |
| ll_end_iter | Declare iteration opened by ll_begin_iter ends |

See more explicit and other function prototypes in linked_list.h

## Notes

Expect the caller of this linked list is only one and not referenced from multiple entities (such as process or threads).
