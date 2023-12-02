#ifndef MINI_COMPILER_PARSER_H
#define MINI_COMPILER_PARSER_H

#include <string.h>
#define MAX_PRODUCTIONS 20

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

typedef struct Grammar {
    unsigned n_prods;
    Production prods[MAX_PRODUCTIONS];
} Grammar;

// S' -> S
static const Production P0 = {.n_rhs = 1,
                              .lhs = {.value = {'S'}, .ty = TERM_NON_TERMINAL},
                              .rhs = {
                                  {.value = {'S'}, .ty = TERM_NON_TERMINAL},
                              }};

// S -> F ; S
static const Production P1 = {.n_rhs = 3,
                              .lhs = {.value = {'S'}, .ty = TERM_NON_TERMINAL},
                              .rhs = {
                                  {.value = {'F'}, .ty = TERM_NON_TERMINAL},
                                  {.value = {Semicolon}, .ty = TERM_TERMINAL},
                                  {.value = {'S'}, .ty = TERM_NON_TERMINAL},
                              }};

// S  -> C
static const Production P2 = {.n_rhs = 1,
                              .lhs = {.value = {'S'}, .ty = TERM_NON_TERMINAL},
                              .rhs = {
                                  {.value = {'C'}, .ty = TERM_NON_TERMINAL},
                              }};

// F  -> fun id A -> C
static const Production P3 = {.n_rhs = 5,
                              .lhs = {.value = {'F'}, .ty = TERM_NON_TERMINAL},
                              .rhs = {
                                  {.value = {FuncDecl}, .ty = TERM_TERMINAL},
                                  {.value = {Identifier}, .ty = TERM_TERMINAL},
                                  {.value = {'A'}, .ty = TERM_NON_TERMINAL},
                                  {.value = {Arrow}, .ty = TERM_TERMINAL},
                                  {.value = {'C'}, .ty = TERM_NON_TERMINAL},
                              }};

// A  -> T id A
static const Production P4 = {.n_rhs = 3,
                              .lhs = {.value = {'A'}, .ty = TERM_NON_TERMINAL},
                              .rhs = {
                                  {.value = {'T'}, .ty = TERM_NON_TERMINAL},
                                  {.value = {Identifier}, .ty = TERM_TERMINAL},
                                  {.value = {'A'}, .ty = TERM_NON_TERMINAL},
                              }};

// A  -> T id
static const Production P5 = {.n_rhs = 2,
                              .lhs = {.value = {'A'}, .ty = TERM_NON_TERMINAL},
                              .rhs = {
                                  {.value = {'T'}, .ty = TERM_NON_TERMINAL},
                                  {.value = {Identifier}, .ty = TERM_TERMINAL},
                              }};

// T  -> nat
static const Production P6 = {.n_rhs = 1,
                              .lhs = {.value = {'T'}, .ty = TERM_NON_TERMINAL},
                              .rhs = {
                                  {.value = {NatDecl}, .ty = TERM_TERMINAL},
                              }};

// T -> bool
static const Production P7 = {.n_rhs = 1,
                              .lhs = {.value = {'T'}, .ty = TERM_NON_TERMINAL},
                              .rhs = {
                                  {.value = {BoolDecl}, .ty = TERM_TERMINAL},
                              }};

// C  -> B ? E : C
static const Production P8 = {
    .n_rhs = 5,
    .lhs = {.value = {'C'}, .ty = TERM_NON_TERMINAL},
    .rhs = {
        {.value = {'B'}, .ty = TERM_NON_TERMINAL},
        {.value = {QuestionMark}, .ty = TERM_TERMINAL},
        {.value = {'E'}, .ty = TERM_NON_TERMINAL},
        {.value = {Colon}, .ty = TERM_TERMINAL},
        {.value = {'C'}, .ty = TERM_NON_TERMINAL},
    }};

// C -> E
static const Production P9 = {.n_rhs = 1,
                              .lhs = {.value = {'C'}, .ty = TERM_NON_TERMINAL},
                              .rhs = {
                                  {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                              }};

// E -> ( id I )
static const Production P10 = {.n_rhs = 4,
                               .lhs = {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                               .rhs = {
                                   {.value = {LeftParen}, .ty = TERM_TERMINAL},
                                   {.value = {Identifier}, .ty = TERM_TERMINAL},
                                   {.value = {'I'}, .ty = TERM_NON_TERMINAL},
                                   {.value = {RightParen}, .ty = TERM_TERMINAL},
                               }};

// I -> E I
static const Production P11 = {.n_rhs = 2,
                               .lhs = {.value = {'I'}, .ty = TERM_NON_TERMINAL},
                               .rhs = {
                                   {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                                   {.value = {'I'}, .ty = TERM_NON_TERMINAL},
                               }};

// I -> E
static const Production P12 = {.n_rhs = 1,
                               .lhs = {.value = {'I'}, .ty = TERM_NON_TERMINAL},
                               .rhs = {
                                   {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                               }};

// E -> lit + E
static const Production P13 = {.n_rhs = 3,
                               .lhs = {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                               .rhs = {
                                   {.value = {NatLit}, .ty = TERM_TERMINAL},
                                   {.value = {Plus}, .ty = TERM_TERMINAL},
                                   {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                               }};

// E -> lit
static const Production P14 = {.n_rhs = 1,
                               .lhs = {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                               .rhs = {
                                   {.value = {NatLit}, .ty = TERM_TERMINAL},
                               }};

// B -> E R E & B
static const Production P15 = {.n_rhs = 5,
                               .lhs = {.value = {'B'}, .ty = TERM_NON_TERMINAL},
                               .rhs = {
                                   {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                                   {.value = {'R'}, .ty = TERM_NON_TERMINAL},
                                   {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                                   {.value = {Ampersand}, .ty = TERM_TERMINAL},
                                   {.value = {'B'}, .ty = TERM_NON_TERMINAL},
                               }};

// B -> E R E
static const Production P16 = {.n_rhs = 3,
                               .lhs = {.value = {'B'}, .ty = TERM_NON_TERMINAL},
                               .rhs = {
                                   {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                                   {.value = {'R'}, .ty = TERM_NON_TERMINAL},
                                   {.value = {'E'}, .ty = TERM_NON_TERMINAL},
                               }};

// R -> <
static const Production P17 = {.n_rhs = 1,
                               .lhs = {.value = {'R'}, .ty = TERM_NON_TERMINAL},
                               .rhs = {
                                   {.value = {Less}, .ty = TERM_TERMINAL},
                               }};

// R -> =
static const Production P18 = {.n_rhs = 1,
                               .lhs = {.value = {'R'}, .ty = TERM_NON_TERMINAL},
                               .rhs = {
                                   {.value = {Equal}, .ty = TERM_TERMINAL},
                               }};

Grammar *grammar_new();
void destroy_grammar(Grammar *g);

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

static const struct SLRop GOTO_TABLE[38][9] = {{{SLR_EMPTY, 0}}};
static const struct SLRop SHIFT_REDUCE_TABLE[38][16] = {{{SLR_EMPTY, 0}}};

static const struct SLRTable SLR_TABLE = {
    GOTO_TABLE,
    SHIFT_REDUCE_TABLE,
};

SLRItem *slr_item_init(SLRSymbol symbol, SLRSymbolTy ty, unsigned value);

SLRParser *slr_parser_init(Grammar *grammar, const SLRTable *table);

#endif  // MINI_COMPILER_PARSER_H
