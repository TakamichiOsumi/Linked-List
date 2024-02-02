CC	= gcc
CFLAGS	= -O0 -Wall
PROGRAM	= ll_test
OUTPUT_LIB = liblinked_list.a

all: $(PROGRAM) $(OUTPUT_LIB)

$(PROGRAM): linked_list.o
	$(CC) $(CFLAGS) test_linked_list.c $^ -o $@

linked_list.o:
	$(CC) $(CFLAGS) linked_list.c -c

$(OUTPUT_LIB): linked_list.o
	ar rs $@ $<

.PHONY: clean

clean:
	rm -rf $(PROGRAM) $(OUTPUT_LIB) linked_list.o
