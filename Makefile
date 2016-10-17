CC=gcc
BIN=bin/
SRC=src/
INC=inc/
COMMON_OPTS=-I$(INC) -o $@ $(DEBUG)
BIN_OPTS=$(COMMON_OPTS) -c $^
PROG_OPTS=$(COMMON_OPTS) $^ -lm
HW1_DEPENDS=$(BIN)hw1_main.o $(BIN)graphics.o $(BIN)bezier.o $(BIN)polyline.o $(BIN)point3d.o $(BIN)point3d_vec.o $(BIN)awh44_math.o
HW2_DEPENDS=$(BIN)hw2_main.o $(BIN)graphics.o $(BIN)catmullrom.o $(BIN)bezier.o $(BIN)polyline.o $(BIN)point3d.o $(BIN)point3d_vec.o $(BIN)awh44_math.o

CG_hw2: $(HW2_DEPENDS)
	$(CC) $(PROG_OPTS)

CG_hw1: $(HW1_DEPENDS)
	$(CC) $(PROG_OPTS)

$(BIN)hw2_main.o: $(SRC)hw2_main.c
	$(CC) $(BIN_OPTS)

$(BIN)hw1_main.o: $(SRC)hw1_main.c
	$(CC) $(BIN_OPTS)

$(BIN)graphics.o: $(SRC)graphics.c
	$(CC) $(BIN_OPTS)

$(BIN)catmullrom.o: $(SRC)catmullrom.c
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
