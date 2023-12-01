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

// S  -> C
const Production P2 = {.n_rhs = 1,
                       .lhs = {.value = {'S'}, .ty = TERM_NON_TERMINAL},
                       .rhs = {
                           {.value = {'C'}, .ty = TERM_NON_TERMINAL},
                       }};

// F  -> fun id A -> C
const Production P3 = {.n_rhs = 5,
                       .lhs = {.value = {'F'}, .ty = TERM_NON_TERMINAL},
                       .rhs = {
                           {.value = {FuncDecl}, .ty = TERM_TERMINAL},
                           {.value = {Identifier}, .ty = TERM_TERMINAL},
                           {.value = {'A'}, .ty = TERM_NON_TERMINAL},
                           {.value = {Arrow}, .ty = TERM_TERMINAL},
                           {.value = {'C'}, .ty = TERM_NON_TERMINAL},
                       }};

// A  -> T id A
const Production P4 = {.n_rhs = 3,
                       .lhs = {.value = {'A'}, .ty = TERM_NON_TERMINAL},
                       .rhs = {
                           {.value = {'T'}, .ty = TERM_NON_TERMINAL},
                           {.value = {Identifier}, .ty = TERM_TERMINAL},
                           {.value = {'A'}, .ty = TERM_NON_TERMINAL},
                       }};

// A  -> T id
const Production P5 = {.n_rhs = 2,
                       .lhs = {.value = {'A'}, .ty = TERM_NON_TERMINAL},
                       .rhs = {
                           {.value = {'T'}, .ty = TERM_NON_TERMINAL},
                           {.value = {Identifier}, .ty = TERM_TERMINAL},
                       }};

// T  -> nat
const Production P6 = {.n_rhs = 1,
                       .lhs = {.value = {'T'}, .ty = TERM_NON_TERMINAL},
                       .rhs = {
                           {.value = {NatDecl}, .ty = TERM_TERMINAL},
                       }};

// T -> bool
const Production P7 = {.n_rhs = 1,
                       .lhs = {.value = {'T'}, .ty = TERM_NON_TERMINAL},
                       .rhs = {
                           {.value = {BoolDecl}, .ty = TERM_TERMINAL},
                       }};

// C  -> B ? E : C
const Production P8 = {.n_rhs = 5,
                       .lhs = {.value = {'C'}, .ty = TERM_NON_TERMINAL},
                       .rhs = {
                           {.value = {'B'}, .ty = TERM_NON_TERMINAL},
                           {.value = {QuestionMark}, .ty = TERM_TERMINAL},
                           {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                           {.value = {Colon}, .ty = TERM_TERMINAL},
                           {.value = {'C'}, .ty = TERM_NON_TERMINAL},
                       }};

// C -> E
const Production P9 = {.n_rhs = 1,
                       .lhs = {.value = {'C'}, .ty = TERM_NON_TERMINAL},
                       .rhs = {
                           {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                       }};

// E -> ( id I )
const Production P10 = {.n_rhs = 4,
                        .lhs = {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                        .rhs = {
                            {.value = {LeftParen}, .ty = TERM_TERMINAL},
                            {.value = {Identifier}, .ty = TERM_TERMINAL},
                            {.value = {'I'}, .ty = TERM_NON_TERMINAL},
                            {.value = {RightParen}, .ty = TERM_TERMINAL},
                        }};

// I -> E I
const Production P11 = {.n_rhs = 2,
                        .lhs = {.value = {'I'}, .ty = TERM_NON_TERMINAL},
                        .rhs = {
                            {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                            {.value = {'I'}, .ty = TERM_NON_TERMINAL},
                        }};

// I -> E
const Production P12 = {.n_rhs = 1,
                        .lhs = {.value = {'I'}, .ty = TERM_NON_TERMINAL},
                        .rhs = {
                            {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                        }};

// E -> lit + E
const Production P13 = {.n_rhs = 3,
                        .lhs = {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                        .rhs = {
                            {.value = {NatLit}, .ty = TERM_TERMINAL},
                            {.value = {Plus}, .ty = TERM_TERMINAL},
                            {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                        }};

// E -> lit
const Production P14 = {.n_rhs = 1,
                        .lhs = {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                        .rhs = {
                            {.value = {NatLit}, .ty = TERM_TERMINAL},
                        }};

// B -> E R E & B
const Production P15 = {.n_rhs = 5,
                        .lhs = {.value = {'B'}, .ty = TERM_NON_TERMINAL},
                        .rhs = {
                            {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                            {.value = {'R'}, .ty = TERM_NON_TERMINAL},
                            {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                            {.value = {Ampersand}, .ty = TERM_TERMINAL},
                            {.value = {'B'}, .ty = TERM_NON_TERMINAL},
                        }};

// B -> E R E
const Production P16 = {.n_rhs = 3,
                        .lhs = {.value = {'B'}, .ty = TERM_NON_TERMINAL},
                        .rhs = {
                            {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                            {.value = {'R'}, .ty = TERM_NON_TERMINAL},
                            {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                        }};

// R -> <
const Production P17 = {.n_rhs = 1,
                        .lhs = {.value = {'R'}, .ty = TERM_NON_TERMINAL},
                        .rhs = {
                            {.value = {Less}, .ty = TERM_TERMINAL},
                        }};

// R -> =
const Production P18 = {.n_rhs = 1,
                        .lhs = {.value = {'R'}, .ty = TERM_NON_TERMINAL},
                        .rhs = {
                            {.value = {Equal}, .ty = TERM_TERMINAL},
                        }};

Grammar GRAMMAR = {
    .n_prods = MAX_PRODUCTIONS,
};

Grammar *grammar_init() {
    GRAMMAR.prods[0] = P0;
    GRAMMAR.prods[1] = P1;
    GRAMMAR.prods[2] = P2;
    GRAMMAR.prods[3] = P3;
    GRAMMAR.prods[4] = P4;
    GRAMMAR.prods[5] = P5;
    GRAMMAR.prods[6] = P6;
    GRAMMAR.prods[7] = P7;
    GRAMMAR.prods[8] = P8;
    GRAMMAR.prods[9] = P9;
    GRAMMAR.prods[10] = P10;
    GRAMMAR.prods[11] = P11;
    GRAMMAR.prods[12] = P12;
    GRAMMAR.prods[13] = P13;
    GRAMMAR.prods[14] = P14;
    GRAMMAR.prods[15] = P15;
    GRAMMAR.prods[16] = P16;
    GRAMMAR.prods[17] = P17;
    GRAMMAR.prods[18] = P18;

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
    SLRSymbol *symbol;  // Non-terminal or Token
    SLRSymbolTy ty;     // the type of the symbol
    unsigned value;
} SLRItem;

typedef struct SLRParser {
    // TODO: /* stack of SLR items */
    const SLRTable *table;
    Grammar *grammar;
    CC_Deque *stack;
} SLRParser;

const struct SLRop GOTO_TABLE[38][9] = {{{SLR_EMPTY, 0}}};
const struct SLRop SHIFT_REDUCE_TABLE[38][16] = {{{SLR_EMPTY, 0}}};

const struct SLRTable SLR_TABLE = {
    GOTO_TABLE,
    SHIFT_REDUCE_TABLE,
};

SLRItem *slr_item_init(SLRSymbol symbol, SLRSymbolTy ty, unsigned value);
SLRParser *slr_parser_init(Grammar *grammar, SLRTable *table);

#endif  // MINI_COMPILER_PARSER_H
