#include "parser.h"

SLRItem *slr_item_token(Token *tok, SLRSymbolTy ty, unsigned value) {
    SLRItem *item = malloc(sizeof(SLRItem));

    SLRSymbol *sym = malloc(sizeof(SLRSymbol));
    sym->token = tok;

    item->symbol = sym;
    item->ty = ty;
    item->value = value;
    return item;
}

SLRParser *slr_parser_init(Grammar *grammar, SLRTable *table) {
    SLRParser *parser = malloc(sizeof(SLRParser));

    parser->grammar = grammar;
    parser->table = table;
    cc_deque_new(&parser->stack);
    cc_deque_add(parser->stack, slr_item_token(NULL, SLR_SYMBOL_VOID, 0));

    return parser;
}
