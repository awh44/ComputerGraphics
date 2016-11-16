CC=gcc
BIN=bin/
SRC=src/
INC=inc/
OUT=outputs/
COMMON_OPTS=-I$(INC) -Wall -o $@ $(DEBUG)
BIN_OPTS=$(COMMON_OPTS) -c $^
PROG_OPTS=$(COMMON_OPTS) $^ -lm
HW1_DEPENDS=$(BIN)hw1_main.o $(BIN)graphics.o $(BIN)bezier.o $(BIN)polyline.o $(BIN)point3d.o $(BIN)point3d_vec.o $(BIN)awh44_math.o
HW2_DEPENDS=$(BIN)hw2_main.o $(BIN)graphics.o $(BIN)catmullrom.o $(BIN)bezier.o $(BIN)polyline.o $(BIN)point3d.o $(BIN)point3d_vec.o $(BIN)awh44_math.o
HW3_DEPENDS=$(BIN)hw3_main.o $(BIN)graphics.o $(BIN)bezier_surface.o $(BIN)mesh.o $(BIN)mesh_face_vec.o $(BIN)point3d.o $(BIN)point3d_vec.o $(BIN)awh44_math.o
HW4_DEPENDS=$(BIN)hw4_main.o $(BIN)sellipsoid.o $(BIN)mesh.o $(BIN)mesh_face_vec.o $(BIN)point3d.o $(BIN)point3d_vec.o $(BIN)awh44_math.o

CG_hw4: $(HW4_DEPENDS)
	$(CC) $(PROG_OPTS)

CG_hw3: $(HW3_DEPENDS)
	$(CC) $(PROG_OPTS)

CG_hw2: $(HW2_DEPENDS)
	$(CC) $(PROG_OPTS)

CG_hw1: $(HW1_DEPENDS)
	$(CC) $(PROG_OPTS)

$(BIN)hw4_main.o: $(SRC)hw4_main.c
	$(CC) $(BIN_OPTS)

$(BIN)hw3_main.o: $(SRC)hw3_main.c
	$(CC) $(BIN_OPTS)

$(BIN)hw2_main.o: $(SRC)hw2_main.c
	$(CC) $(BIN_OPTS)

$(BIN)hw1_main.o: $(SRC)hw1_main.c
	$(CC) $(BIN_OPTS)

$(BIN)graphics.o: $(SRC)graphics.c
	$(CC) $(BIN_OPTS)

$(BIN)sellipsoid.o: $(SRC)sellipsoid.c
	$(CC) $(BIN_OPTS)

$(BIN)bezier_surface.o: $(SRC)bezier_surface.c
	$(CC) $(BIN_OPTS)

$(BIN)mesh.o: $(SRC)mesh.c
	$(CC) $(BIN_OPTS)

$(BIN)mesh_face_vec.o: $(SRC)mesh_face_vec.c
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

SELLIPSOID0=-A 1 -B 1 -C 1 -r 1 -t 1 -u 21 -v 11
$(OUT)sellipsoid0_flat.iv: CG_hw4
	./CG_hw4 $(SELLIPSOID0) -F > $@
$(OUT)sellipsoid0_smooth.iv: CG_hw4
	./CG_hw4 $(SELLIPSOID0) -S > $@

SELLIPSOID1=-A 1 -B 1 -C 1 -r 0.1 -t 0.23 -u 19 -v 9
$(OUT)sellipsoid1_flat.iv: CG_hw4
	./CG_hw4 $(SELLIPSOID1) -F > $@
$(OUT)sellipsoid1_smooth.iv: CG_hw4
	./CG_hw4 $(SELLIPSOID1) -S > $@

SELLIPSOID2=-A 1.2 -B 1.1 -C 2.3 -r 0.9 -t 1.0 -u 25 -v 13
$(OUT)sellipsoid2_flat.iv: CG_hw4
	./CG_hw4 $(SELLIPSOID2) -F > $@
$(OUT)sellipsoid2_smooth.iv: CG_hw4
	./CG_hw4 $(SELLIPSOID2) -S > $@

SELLIPSOID3=-A 1.9 -B 2.1 -C 0.5 -r 1.9 -t 1.6 -u 23 -v 12
$(OUT)sellipsoid3_flat.iv: CG_hw4
	./CG_hw4 $(SELLIPSOID3) -F > $@
$(OUT)sellipsoid3_smooth.iv: CG_hw4
	./CG_hw4 $(SELLIPSOID3) -S > $@

SELLIPSOID4=-A 2.5 -B 1.3 -C 3.3 -r 0.5 -t 1.35 -u 29 -v 15
$(OUT)sellipsoid4_flat.iv: CG_hw4
	./CG_hw4 $(SELLIPSOID4) -F > $@
$(OUT)sellipsoid4_smooth.iv: CG_hw4
	./CG_hw4 $(SELLIPSOID4) -S > $@

.PHONY: clean
clean:
	rm -f bin/* CG_hw*
