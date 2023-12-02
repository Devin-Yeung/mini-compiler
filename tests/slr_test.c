#include "../parser.h"

int main() {
    Grammar* grammar = grammar_new();
    SLRParser* parser = slr_parser_init(grammar, &SLR_TABLE);
    destroy_slr_parser(parser);
    destroy_grammar(grammar);
}