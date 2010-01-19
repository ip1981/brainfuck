BF = brainfuck

CC     = gcc
CFLAGS = -ansi -pedantic -Wall -Wextra -Werror

INDENT_OPT = -bap -bad -bbb -bl -bli0 -bls \
			 -c2 -cli2 -cdb -sc -ncs -npcs \
			 -nhnl -i4 -l80 -saf -sai -saw \
			 -nprs -sob -nut -nbc -ppi2 -nbbo

all: $(BF) README

$(BF): $(BF).c
	$(CC) $(CFLAGS) -o $(BF) $(BF).c

README: $(BF)
	./$(BF) -h > $@

indent: $(BF).c
	indent $(INDENT_OPT) $?

.PHONY: indent
