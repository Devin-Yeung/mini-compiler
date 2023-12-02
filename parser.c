#include "parser.h"

#include <stdio.h>

#include "deque.h"

Grammar *grammar_new() {
    Grammar *g = malloc(sizeof(Grammar));

    g->prods[0] = P0;
    g->prods[1] = P1;
    g->prods[2] = P2;
    g->prods[3] = P3;
    g->prods[4] = P4;
    g->prods[5] = P5;
    g->prods[6] = P6;
    g->prods[7] = P7;
    g->prods[8] = P8;
    g->prods[9] = P9;
    g->prods[10] = P10;
    g->prods[11] = P11;
    g->prods[12] = P12;
    g->prods[13] = P13;
    g->prods[14] = P14;
    g->prods[15] = P15;
    g->prods[16] = P16;
    g->prods[17] = P17;
    g->prods[18] = P18;

    return g;
}

void destroy_grammar(Grammar *g) { free(g); }

SLRItem *slr_item_token(Token *tok, SLRSymbolTy ty, unsigned value) {
    SLRItem *item = malloc(sizeof(SLRItem));

    SLRSymbol *sym = malloc(sizeof(SLRSymbol));
    sym->token = tok;

    item->symbol = sym;
    item->ty = ty;
    item->value = value;
    return item;
}

SLRParser *slr_parser_init(Grammar *grammar, const SLRTable *table) {
    SLRParser *parser = malloc(sizeof(SLRParser));

    parser->grammar = grammar;
    parser->table = table;
    cc_deque_new(&parser->stack);
    SLRItem *item = slr_item_token(NULL, SLR_SYMBOL_VOID, 0);
    cc_deque_add(parser->stack, item);

    return parser;
}

void destroy_slr_item(SLRItem *item) {
    free(item->symbol);
    free(item);
}

void destroy_slr_item_cb(void *item) { destroy_slr_item((SLRItem *)item); }

void destroy_slr_parser(SLRParser *parser) {
    cc_deque_destroy_cb(parser->stack, destroy_slr_item_cb);
    free(parser);
}

void slr_parser_step(SLRParser *parser, Token *tok) {
    SLRItem *last = NULL;
    cc_deque_get_last(parser->stack, (void *)&last);
    SLRop next = parser->table->shift_reduce_table[last->value][tok->ty];
    // TODO: need slr table
}

SLRop goto_table_get(const SLRop (*goto_table)[9], unsigned state_id,
                     NonTerminal nt) {
    // mapping a non-terminal to table column index
    unsigned symbol_idx = 0;
    switch (nt) {
        case 'S':
            symbol_idx = 0;
            break;
        case 'F':
            symbol_idx = 1;
            break;
        case 'A':
            symbol_idx = 2;
            break;
        case 'T':
            symbol_idx = 3;
            break;
        case 'C':
            symbol_idx = 4;
            break;
        case 'E':
            symbol_idx = 5;
            break;
        case 'I':
            symbol_idx = 6;
            break;
        case 'B':
            symbol_idx = 7;
            break;
        case 'R':
            symbol_idx = 8;
            break;
        default:
            __builtin_unreachable();
    }
    return goto_table[state_id][symbol_idx];
}