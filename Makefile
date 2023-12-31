CC            ?= clang
CFLAGS        := $(CFLAGS) -Wall -Wextra
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

ifneq (,$(findstring clang,$(CC)))
    GCOV_TOOL = --gcov-tool llvm-cov --gcov-tool gcov
else
    GCOV_TOOL = --gcov-tool gcov
endif


all: test run_parser_fuzz

test: dfa_test lexer_test symbol_table_test slr_test parser_test func_test

func: build/func.o build/lexer.o build/symbol_table.o build/log.o build/parser.o build/parse_tree.o
	$(CC) $(CFLAGS) -o build/func build/func.o build/lexer.o build/symbol_table.o build/log.o build/parser.o build/parse_tree.o

func_test: func
	$(RUNTIME_FLAGS) find snapshots -type f -exec ./build/func {} \;

build:
	mkdir build

dfa_test: build/dfa_test.o build/lexer.o build/log.o
	$(CC) $(CFLAGS) -o build/dfa_test build/dfa_test.o build/lexer.o build/log.o
	$(RUNTIME_FLAGS) ./build/dfa_test

lexer_test: build build/lexer_test.o build/lexer.o build/log.o
	$(CC) $(CFLAGS) -o build/lexer_test build/lexer_test.o build/lexer.o build/log.o
	$(RUNTIME_FLAGS) ./build/lexer_test

symbol_table_test: build/symbol_table.o build/symbol_table_test.o
	$(CC) $(CFLAGS) -o build/symbol_table_test build/symbol_table.o build/symbol_table_test.o
	$(RUNTIME_FLAGS) ./build/symbol_table_test

slr_test: build build/slr_test.o build/lexer.o build/log.o build/parser.o build/parse_tree.o
	$(CC) $(CFLAGS) -o build/slr_test build/slr_test.o build/lexer.o build/log.o build/parser.o build/parse_tree.o
	$(RUNTIME_FLAGS) ./build/slr_test

parser_test: build build/parser_test.o build/lexer.o build/log.o build/parser.o build/parse_tree.o
	$(CC) $(CFLAGS) -o build/parser_test build/parser_test.o build/lexer.o build/log.o build/parser.o build/parse_tree.o
	$(RUNTIME_FLAGS) ./build/parser_test $$(find snapshots/parser -type f)

build/parser_fuzz: build build/parser_fuzz.o build/lexer.o build/log.o build/parser.o build/parse_tree.o
	$(CC) $(CFLAGS) -o build/parser_fuzz build/parser_fuzz.o build/lexer.o build/log.o build/parser.o build/parse_tree.o

run_parser_fuzz: build/parser_fuzz
	bnfgen table/grammar.bnf | build/parser_fuzz

build/log.o:
	$(CC) $(CFLAGS) -c log.c -o build/log.o

build/lexer_test.o: build tests/lexer_test.c
	$(CC) $(CFLAGS) -c tests/lexer_test.c -o build/lexer_test.o

build/dfa_test.o: build tests/dfa_test.c
	$(CC) $(CFLAGS) -c tests/dfa_test.c -o build/dfa_test.o

build/symbol_table_test.o: build tests/symbol_table_test.c
	$(CC) $(CFLAGS) -c tests/symbol_table_test.c -o build/symbol_table_test.o

build/parser_fuzz.o: build fuzz/parser_fuzz.c
	$(CC) $(CFLAGS) -c fuzz/parser_fuzz.c -o build/parser_fuzz.o

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

build/parser_test.o: build tests/parser_test.c
	$(CC) $(CFLAGS) -c tests/parser_test.c -o build/parser_test.o

build/parser.o: build parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c -o build/parser.o

build/parse_tree.o: build parse_tree.c parse_tree.h
	$(CC) $(CFLAGS) -c parse_tree.c -o build/parse_tree.o

format:
	clang-format -i *.c
	clang-format -i *.h
	clang-format -i tests/*.c
	clang-format -i fuzz/*.c

codecov: build func test
	lcov --capture --directory build $(GCOV_TOOL) --output-file coverage.lcov

clean: build
	@echo "Cleaning ..."
	rm -r ./build/*

