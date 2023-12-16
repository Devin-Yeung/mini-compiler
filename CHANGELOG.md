# Changelog

All notable changes to this project will be documented in this file.

## [0.2.3] - 2023-12-17

### Bug Fixes

- Remove deps `math.h`
- Fix heap buffer overflow in path concat
- Fix the heap buffer overflow in `add_suffix`
- Deal with the 'fake' suffix

### Miscellaneous Tasks

- Remove underlines in `0` and `$`
- Comment out the `assert` to avoid panic

## [0.2.2] - 2023-12-16

### Features

- Prettier print the slr trace

## [0.2.1] - 2023-12-16

### Bug Fixes

- Fix `func` build error
- Fix the parse tree construction
- Fix `parse_tree_init`
- Fix a bunch of mem pitfall
- Destroy the deque when destroy tree
- Try to fix mem leak in SLR sym

### Features

- Draft API of parse tree
- Try building the parse tree
- A simple test on parse tree
- Declare the node ty

### Miscellaneous Tasks

- Init the parse_tree module

### Refactor

- Merge deque into parse tree

## [0.2.0] - 2023-12-09

### Bug Fixes

- Change `=>` to `->` in SLR
- Set the production len in grammar
- Fix memory leak in slr test
- Destroy slr parser after use to prevent mem leak
- Change `accept` to `r0` in SLR table
- Use `==` instead of `=` in test
- Fix the mem layout of `Production` struct
- Fix some bugs in SLR parser core
- Try to fix mem leak in `slr_parser_step`
- Try to fix mem leak of SLR Item
- Fix mem leaks caused by the EOF token
- Fix fuzz CI script
- Enable clang in CI env
- Avoid eating more than one EOF
- Abandon the deque iterator since it's buggy
- Reduce should be displayed after the lhs is pushed into the stack
- Make parser test return correct error code
- Fix mem leaks when free the SLRItem
- Fix the double free in slr parser
- Fix the leaking string
- Fix the leaking token in SLR parsing
- Destroy the tok when it is invalid to prevent mem leak
- Deal with comment correctly
- Give eof token span info to prevent segfault
- Fix the clean-up script

### Features

- Fuzzy test for lexer
- Stick to the requirements, ignore the comments and early stop
- Draft SLR parser API
- Deque data structure
- Design SLR Stack (draft)
- Add SLR.csv
- Proof of concept of grammar rule
- Impl some of the initializer
- Finish the grammar rules
- SLR minimal working example
- SLR_parser_step API
- Convert the SLR table to C code
- Get item from SLR goto table
- Get item from SLR shift-reduce table
- A draft impl of SLR core parse logic
- Add a simple test case for SLR parser
- Raw version of the bnf grammar
- Generate more realistic test cases
- Tend to generate more functions in fuzz
- Impl a naive parser fuzzer
- Run fuzz in CI
- Remove all the `printf` in fuzz to improve speed
- Add line and col info to `Span`
- Auto rerun the job when bnfgen generate large string
- A naive stack trace impl
- Improve the log info
- Print the parser stack
- Track the SLRops
- Track all the stack and ops
- Display the trace in a pretty way
- Impl the parser_test
- Fix the memory leaks of grammar
- Add more test cases for parser
- Add some fuzz tests
- Add some should fail tests
- Rework the main function
- Add the initial state of stack to trace
- Add accept and reject state to SLR trace
- Persist the SLR trace

### Miscellaneous Tasks

- Init the source file
- Fix typo
- Allow reading `CFLAGS` from env
- Reduce the number of fuzz run
- Reorder the place where to show the SLR stack
- Only run fuzz on dispatch
- Make test in CI faster
- Make codecov faster
- Update zipignore

### Refactor

- Rework on the parser
- Merge `BoolLit` and `NatLit` to `Literal`
- Merge `BoolLit` and `NatLit` in Rust bindings
- Rework on the test structure
- Refactor the func test

## [0.1.2] - 2023-11-01

### Bug Fixes

- Revert comma support, comma is not a valid token
- Parse `1fun` correctly
- Prevent buffer overflow in DFA
- Fix the broken tests caused by the API changes

### Features

- Pre-commit hook support
- Improve cargo build script
- Display line and column instead of span

### Refactor

- Migrate rust binding into testsuite
- Refactor the `debug_token`

## [0.1.1] - 2023-10-28

### Bug Fixes

- Fix mem leak cause by duplicate symbol insertion
- Fix pretty print of NUL char

### Features

- AVL tree data structure
- Impl symbol table
- More test cases to test symbol table
- Impl symbol table walk
- Test insert duplicate symbols into symboltable
- Only compile `log` in debug mode
- Display symbol table after lexical analysis

### Miscellaneous Tasks

- Update gitignore
- Update zipignore
- Clean the unused Makefile recipe

## [0.1.0] - 2023-10-27

### Bug Fixes

- Fix the deps in Makefile
- Fix mem leak
- Downgrade llvm to 15
- Sometimes the last token is ignored
- Change the condition to check DFA
- Fix heap overflow
- Fix incorrect parse in example-1-compact
- Support comma token
- Update dfa check condition
- Compatibility
- Parse comment correctly

### Features

- DFA
- Support ASAN
- CI
- Draft impl of lexer `next_token`
- Return EOF if end of file
- Identify the token type
- Token debugger
- Support slog
- Run ci on more OS
- Commandline interface
- Snapshots examples
- Enable log by env var
- Support invalid token
- Prettier ascii debug print
- Codecov support
- Rust bindings
- Snapshot testsuite via rust bindings
- Snapshots for all the lexer examples
- Git cliff support

### Miscellaneous Tasks

- Fmt
- Make clion easy to analyze
- Also format the `.h` file
- Update fmt
- Temporarily skip ci on macOS
- Fmt
- Enable debug mode in sanitizer check
- Update log info
- Update ignore files
- Allow colorful log in DEBUG mode
- Enable snapshots test

### Refactor

- Make DFA static
- Remove the deprecated `sprintf`
- Use allocated string in token lexeme
- Migrate to a new log library

<!-- generated by git-cliff -->
