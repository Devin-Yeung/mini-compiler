#ifndef MINI_COMPILER_PARSER_H
#define MINI_COMPILER_PARSER_H

#include <string.h>

#include "deque.h"
#include "lexer.h"

typedef enum TermTy {
    TERM_NON_TERMINAL,
    TERM_TERMINAL,
} TermTy;

typedef struct Term {
    union {
        char nt;   /* Non-Terminal */
        TokenTy t; /* Terminal */
    } value;
    TermTy ty;
} Term;

typedef struct Production {
    unsigned n_rhs; /* number of rhs terms */
    Term lhs;
    Term rhs[];
} Production;

#define MAX_PRODUCTIONS 20

typedef struct Grammar {
    unsigned n_prods;
    Production prods[MAX_PRODUCTIONS];
} Grammar;

// S' -> S
const Production P0 = {.n_rhs = 1,
                       .lhs = {.value = {'S'}, .ty = TERM_NON_TERMINAL},
                       .rhs = {
                           {.value = {'S'}, .ty = TERM_NON_TERMINAL},
                       }};

// S -> F ; S
const Production P1 = {.n_rhs = 3,
                       .lhs = {.value = {'S'}, .ty = TERM_NON_TERMINAL},
                       .rhs = {
                           {.value = {'F'}, .ty = TERM_NON_TERMINAL},
                           {.value = {Semicolon}, .ty = TERM_TERMINAL},
                           {.value = {'S'}, .ty = TERM_NON_TERMINAL},
                       }};

Grammar GRAMMAR = {
    .n_prods = MAX_PRODUCTIONS,
};

Grammar *grammar_init() {
    GRAMMAR.prods[0] = P0;
    GRAMMAR.prods[1] = P1;
    return &GRAMMAR;
}

typedef enum SLRopTy {
    SLR_SHIFT,
    SLR_REDUCE,
    SLR_GOTO,
    SLR_EMPTY,
} SLRopTy;

typedef struct SLRop {
    SLRopTy ty;
    unsigned value;
} SLRop;

typedef struct SLRTable {
    const SLRop (*goto_table)[9];           // 9 non-terminals
    const SLRop (*shift_reduce_table)[16];  // 16 tokens
} SLRTable;

typedef char NonTerminal;

typedef union {
    NonTerminal nt;
    Token *token;  // Take the **ownership** of the token
} SLRSymbol;

typedef enum SLRSymbolTy {
    SLR_SYMBOL_NON_TERMINAL,
    SLR_SYMBOL_TOKEN,
    SLR_SYMBOL_VOID,  // Reserve for the bottom of the stack
} SLRSymbolTy;

typedef struct SLRItem {
    SLRSymbol symbol;
    SLRSymbolTy ty;
    unsigned value;
} SLRItem;

typedef struct SLRParser {
    // TODO: /* stack of SLR items */
    const SLRTable *table;
    Grammar **grammar;
    CC_Deque *stack;
} SLRParser;

const struct SLRop GOTO_TABLE[38][9] = {{{SLR_EMPTY, 0}}};
const struct SLRop SHIFT_REDUCE_TABLE[38][16] = {{{SLR_EMPTY, 0}}};

const struct SLRTable SLR_TABLE = {
    GOTO_TABLE,
    SHIFT_REDUCE_TABLE,
};

#endif  // MINI_COMPILER_PARSER_H
