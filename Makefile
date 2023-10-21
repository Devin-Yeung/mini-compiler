CC=clang
CFLAGS=-Wall -Werror -Wextra

all: test

test: dfa_test
	./build/dfa_test

build:
	mkdir build

dfa_test: build/dfa_test.o build/lexer.o
	$(CC) $(CFLAGS) -o build/dfa_test build/dfa_test.o build/lexer.o

build/dfa_test.o: tests/dfa_test.c
	$(CC) $(CFLAGS) -c tests/dfa_test.c -o build/dfa_test.o

lexer: build/lexer.o build/func.o
	$(CC) $(CFLAGS) -o build/lexer build/lexer.o build/func.o

build/func.o: func.c
	$(CC) $(CFLAGS) -c func.c -o build/func.o

build/lexer.o: build lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c -o build/lexer.o

clean: build
	echo "Cleaning ..."
	rm -r ./build/*

