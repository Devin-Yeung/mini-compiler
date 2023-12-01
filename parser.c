#include "parser.h"

Grammar *grammar_init() {
    Grammar *g = malloc(sizeof(Grammar));
    g->prods = malloc(sizeof(Production) * 19);

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
    cc_deque_add(parser->stack, slr_item_token(NULL, SLR_SYMBOL_VOID, 0));

    return parser;
}
