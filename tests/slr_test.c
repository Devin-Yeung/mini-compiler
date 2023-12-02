#include <assert.h>

#include "../parser.h"

int main() {
    Grammar* grammar = grammar_new();
    SLRParser* parser = slr_parser_init(grammar, &SLR_TABLE);
    SLRop op = goto_table_get(SLR_TABLE.goto_table, 0, 'S');
    assert(op.value = 1);
    assert(op.ty = SLR_GOTO);
    destroy_slr_parser(parser);
    destroy_grammar(grammar);
}