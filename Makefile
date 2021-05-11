CC      = gcc
DEBUG   = -ggdb
CFLAGS  = -W -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wstrict-prototypes -Werror \
	-I.. -pthread -g -std=gnu11
SRC_DIR = .
OBJ_DIR = .
LFLAGS  = -L./$(OBJ_DIR) -lm
LINK    = $(CC)


TEST_TARGET = parser
EXT = c

.PHONY: all clean debug init memtest tests
.SUFFIXES: .o .c .d


clean:
	rm -f $(shell find $(OBJ_DIR)/ -type f -name '*.o') 2> /dev/null
	rm -f $(shell find $(OBJ_DIR)/ -type f -name '*.d') 2> /dev/null
	rm -f $(TEST_TARGET) 2> /dev/null


tests:
	$(CC) $(CFLAGS) $(DEBUG) -o parser.o -MMD -c parser.c
	$(CC) $(CFLAGS) $(DEBUG) -o test.o -MMD -c test.c
	$(LINK) -o $(TEST_TARGET) parser.o test.o $(LFLAGS)


