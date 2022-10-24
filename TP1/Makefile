CC = gcc
BIN = bin/
SRC = src/
INCLUDES = include/
EXEC = k_means

CFLAGS = -O2
CFLAGS_MAT = -lm

.DEFAULT_GOAL = k_means

k_means: $(SRC)k_means.c
	$(CC) $(CFLAGS) $(SRC)k_means.c -o $(BIN)$(EXEC) $(CFLAGS_MAT)

clean:
	rm -r bin/*

run:
	./$(BIN)$(EXEC)