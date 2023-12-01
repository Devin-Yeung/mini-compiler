CC            ?= clang
CFLAGS        := -Wall -Wextra
RUNTIME_FLAGS :=

SANITIZER ?= 0

ASAN_COMPILE_TIME_FLAGS  := -fsanitize=address -fsanitize-address-use-after-scope
ASAN_RUNTIME_FLAGS       := ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=1

ifeq ($(SANITIZER),1)
	CFLAGS        := $(CFLAGS) $(ASAN_COMPILE_TIME_FLAGS)
	RUNTIME_FLAGS := $(RUNTIME_FLAGS) $(ASAN_RUNTIME_FLAGS)
endif

DEBUG ?= 0

ifeq ($(DEBUG),1)
	CFLAGS := $(CFLAGS) -g -DLOG_USE_COLOR -DLOG
endif

CODECOV ?= 0

ifeq ($(CODECOV),1)
	CFLAGS := $(CFLAGS) -fprofile-arcs -ftest-coverage
endif

ifeq ($(CC), clang)
    GCOV_TOOL = --gcov-tool llvm-cov --gcov-tool gcov
else
    GCOV_TOOL = --gcov-tool gcov
endif


all: test main

test: dfa_test lexer_test symbol_table_test slr_test
	$(RUNTIME_FLAGS) ./build/dfa_test
	$(RUNTIME_FLAGS) ./build/lexer_test
	$(RUNTIME_FLAGS) ./build/slr_test

main: build/func.o build/lexer.o build/symbol_table.o build/log.o build/parser.o build/deque.o
	$(CC) $(CFLAGS) -o build/func build/func.o build/lexer.o build/symbol_table.o build/log.o build/deque.o build/parser.o

build:
	mkdir build

dfa_test: build/dfa_test.o build/lexer.o build/log.o
	$(CC) $(CFLAGS) -o build/dfa_test build/dfa_test.o build/lexer.o build/log.o

lexer_test: build build/lexer_test.o build/lexer.o build/log.o
	$(CC) $(CFLAGS) -o build/lexer_test build/lexer_test.o build/lexer.o build/log.o

symbol_table_test: build/symbol_table.o build/symbol_table_test.o
	$(CC) $(CFLAGS) -o build/symbol_table_test build/symbol_table.o build/symbol_table_test.o
	$(RUNTIME_FLAGS) ./build/symbol_table_test

slr_test: build build/slr_test.o build/lexer.o build/log.o build/deque.o build/parser.o
	$(CC) $(CFLAGS) -o build/slr_test build/slr_test.o build/lexer.o build/log.o build/deque.o build/parser.o
	$(RUNTIME_FLAGS) ./build/slr_test

build/log.o:
	$(CC) $(CFLAGS) -c log.c -o build/log.o

build/lexer_test.o: build tests/lexer_test.c
	$(CC) $(CFLAGS) -c tests/lexer_test.c -o build/lexer_test.o

build/dfa_test.o: build tests/dfa_test.c
	$(CC) $(CFLAGS) -c tests/dfa_test.c -o build/dfa_test.o

build/symbol_table_test.o: build tests/symbol_table_test.c
	$(CC) $(CFLAGS) -c tests/symbol_table_test.c -o build/symbol_table_test.o

lexer: build/lexer.o build/func.o
	$(CC) $(CFLAGS) -o build/lexer build/lexer.o build/func.o

build/func.o: build func.c
	$(CC) $(CFLAGS) -c func.c -o build/func.o

build/lexer.o: build lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c -o build/lexer.o

build/symbol_table.o: build symbol_table.c symbol_table.h
	$(CC) $(CFLAGS) -c symbol_table.c -o build/symbol_table.o

build/slr_test.o: build tests/slr_test.c
	$(CC) $(CFLAGS) -c tests/slr_test.c -o build/slr_test.o

build/parser.o: build parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c -o build/parser.o

build/deque.o: build deque.c deque.h
	$(CC) $(CFLAGS) -c deque.c -o build/deque.o

format:
	clang-format -i *.c
	clang-format -i *.h
	clang-format -i tests/*.c

codecov: build main test
	./build/dfa_test
	./build/lexer_test
	LOG=1  find snapshots -type f -exec ./build/func {} \;
	lcov --capture --directory build $(GCOV_TOOL) --output-file coverage.lcov

clean: build
	@echo "Cleaning ..."
	rm -r ./build/*

