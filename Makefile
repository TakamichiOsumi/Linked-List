CC	= gcc
CFLAGS	= -O0 -Wall -g
PROGRAM	= ll_test
OUTPUT_LIB = liblinked_list.a

all: $(PROGRAM) $(OUTPUT_LIB)

$(PROGRAM): linked_list.o
	$(CC) $(CFLAGS) test_linked_list.c $^ -o $@

linked_list.o:
	$(CC) $(CFLAGS) linked_list.c -c

$(OUTPUT_LIB): linked_list.o
	ar rs $@ $<

.PHONY: clean test

clean:
	@rm -rf $(PROGRAM) $(PROGRAM).dSYM $(OUTPUT_LIB) linked_list.o

test: $(PROGRAM)
	@./$(PROGRAM) &> /dev/null && echo "Success when value is zero >>> $$?"
