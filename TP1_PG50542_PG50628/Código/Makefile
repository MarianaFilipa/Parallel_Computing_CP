CC = gcc-7.2.0
BIN = bin/
SRC = src/
INCLUDES = include/
EXEC = k_means

CFLAGS = -O2 -ftree-vectorize -msse4

.DEFAULT_GOAL = k_means

k_means: $(SRC)k_means.c
	$(CC) $(CFLAGS) $(SRC)k_means.c -o $(BIN)$(EXEC) 

clean:
	rm -r bin/*

run:
	./$(BIN)$(EXEC)
