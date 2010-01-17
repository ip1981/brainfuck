BF = brainfuck

CC     = gcc
CFLAGS = -ansi -pedantic -Wall -Wextra -Werror

all: $(BF) README

$(BF): $(BF).c
	$(CC) $(CFLAGS) -o $(BF) $(BF).c

README: $(BF)
	./$(BF) -h > $@

