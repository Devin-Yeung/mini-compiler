#include "../parser.h"

int main() {
    Grammar* grammar = grammar_init();
    SLRParser* parser = slr_parser_init(grammar, &SLR_TABLE);
}