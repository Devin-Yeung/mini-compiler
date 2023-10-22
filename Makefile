CC            := clang
CFLAGS        := -Wall -Werror -Wextra
RUNTIME_FLAGS :=

SANITIZER ?= 0

ASAN_COMPILE_TIME_FLAGS  := -fsanitize=address -fsanitize-address-use-after-scope
ASAN_RUNTIME_FLAGS       := ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=1

ifeq ($(SANITIZER),1)
	CFLAGS        := $(CFLAGS) $(ASAN_COMPILE_TIME_FLAGS)
	RUNTIME_FLAGS := $(RUNTIME_FLAGS) $(ASAN_RUNTIME_FLAGS)
endif


all: test main

test: dfa_test
	$(RUNTIME_FLAGS) ./build/dfa_test

main: build/func.o
	$(CC) $(CFLAGS) -o build/func build/func.o

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

format:
	clang-format -i *.c
	clang-format -i *.h

clean: build
	@echo "Cleaning ..."
	rm -r ./build/*

