CC	= gcc
CFLAGS	= -O0 -Wall
PROGRAM	= ll_test

$(PROGRAM):
	gcc test_linked_list.c linked_list.c -o $@

.PHONY: clean

clean:
	rm -rf $(PROGRAM)
