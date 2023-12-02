#include <assert.h>
#include <stdio.h>

#include "../parser.h"

void smoke_test() {
    char* src = "fun f1 nat a -> 1; 2";
    Lexer* lexer = lexer_new(src);
    Grammar* grammar = grammar_new();
    SLRParser* parser = slr_parser_init(grammar, &SLR_TABLE);
    ParserState state;
    do {
        char buf[256];
        Token* tok = lexer_next_token(lexer);
        debug_token(tok, buf, 256);
        printf("%s\n", buf);
        state = slr_parser_step(parser, tok);
    } while (state == PARSER_IDLE);
    assert(state = PARSER_ACCEPT);
    destroy_slr_parser(parser);
    destroy_grammar(grammar);
    destroy_lexer(lexer);
}

void api_test() {
    Grammar* grammar = grammar_new();
    SLRParser* parser = slr_parser_init(grammar, &SLR_TABLE);
    SLRop op = goto_table_get(SLR_TABLE.goto_table, 0, 'S');
    assert(op.value == 1);
    assert(op.ty == SLR_GOTO);
    op = shift_reduce_table_get(SHIFT_REDUCE_TABLE, 0, Identifier);
    assert(op.value == 0);
    assert(op.ty == SLR_EMPTY);
    op = shift_reduce_table_get(SHIFT_REDUCE_TABLE, 8, Semicolon);
    assert(op.value == 14);
    assert(op.ty == SLR_REDUCE);
    op = shift_reduce_table_get(SHIFT_REDUCE_TABLE, 22, Colon);
    assert(op.value == 29);
    assert(op.ty == SLR_SHIFT);
    destroy_slr_parser(parser);
    destroy_grammar(grammar);
}

int main() {
    smoke_test();
    api_test();
}