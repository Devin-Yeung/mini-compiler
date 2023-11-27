#ifndef MINI_COMPILER_PARSER_H
#define MINI_COMPILER_PARSER_H

#include <string.h>

typedef struct Grammar {
    // TODO
} Grammar;

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

typedef struct SLRParser {
    // TODO: /* stack of SLR items */
    SLRTable *table;
    Grammar **grammar;
} SLRParser;

const struct SLRop GOTO_TABLE[38][9] = {{{SLR_EMPTY, 0}}};
const struct SLRop SHIFT_REDUCE_TABLE[38][16] = {{{SLR_EMPTY, 0}}};

const struct SLRTable SLR_TABLE = {
    GOTO_TABLE,
    SHIFT_REDUCE_TABLE,
};

#endif  // MINI_COMPILER_PARSER_H
