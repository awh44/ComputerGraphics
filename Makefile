CC=gcc
BIN=bin/
SRC=src/
COMMON_OPTS=-Iinc/ -o $@ $(DEBUG)
BIN_OPTS=$(COMMON_OPTS) -c $^
PROG_OPTS=$(COMMON_OPTS) $^
ALL_DEPENDS=$(BIN)main.o

CG_hw1: $(ALL_DEPENDS)
	$(CC) $(PROG_OPTS)

$(BIN)main.o: $(SRC)main.c
	$(CC) $(BIN_OPTS)
