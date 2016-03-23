TARGET=libsamplechain_test

CC=gcc

CFLAGS= -Wall -Wno-unused-value -Wno-unused-function

CFLAGS += -DSC_DEBUG

EXE_OBJ= \
	testcases/test_bsp_varichain.o \
	testcases/main.o

LIB_OBJ= \
	algorithms/bsp_varichain/bsp_varichain.o \
	algorithm.o

OBJ= \
	$(LIB_OBJ) \
	$(EXE_OBJ)


$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

.c.o:
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -f $(OBJ) $(TARGET)

real_clean: clean
	rm -f `find . -name \*~`
