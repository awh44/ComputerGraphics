CC=gcc
BIN=bin/
SRC=src/
COMMON_OPTS=-Iinc/ -o $@ $(DEBUG)
BIN_OPTS=$(COMMON_OPTS) -c $^
PROG_OPTS=$(COMMON_OPTS) $^ -lm
ALL_DEPENDS=$(BIN)main.o $(BIN)bezier.o $(BIN)polyline.o $(BIN)point3d.o $(BIN)point3d_vec.o $(BIN)awh44_math.o

CG_hw1: $(ALL_DEPENDS)
	$(CC) $(PROG_OPTS)

$(BIN)main.o: $(SRC)main.c
	$(CC) $(BIN_OPTS)

$(BIN)bezier.o: $(SRC)bezier.c
	$(CC) $(BIN_OPTS)

$(BIN)polyline.o: $(SRC)polyline.c
	$(CC) $(BIN_OPTS)

$(BIN)point3d.o: $(SRC)point3d.c
	$(CC) $(BIN_OPTS)

$(BIN)point3d_vec.o: $(SRC)point3d_vec.c
	$(CC) $(BIN_OPTS)

$(BIN)awh44_math.o: $(SRC)awh44_math.c
	$(CC) $(BIN_OPTS)

.PHONY: clean
clean:
	rm -f bin/* CG_hw1 outputs/*
