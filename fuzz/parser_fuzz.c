#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../parser.h"

#define CHUNK_SIZE 64

char* read_from_stdin() {
    // Allocate an initial buffer
    size_t buffer_size = CHUNK_SIZE;
    char* buffer = (char*)malloc(buffer_size);

    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Initialize buffer position
    size_t position = 0;

    // Read from the standard input (stdin)
    int ch;
    while ((ch = getchar()) != EOF) {
        // Check if buffer is full, reallocate if necessary
        if (position == buffer_size - 1) {
            buffer_size += CHUNK_SIZE;
            char* temp = realloc(buffer, buffer_size);
            if (temp == NULL) {
                fprintf(stderr, "Memory reallocation failed\n");
                free(buffer);
                return NULL;
            }
            buffer = temp;
        }

        // Store the character in the buffer
        buffer[position++] = ch;
    }

    // Null-terminate the string
    buffer[position] = '\0';

    return buffer;
}

int main() {
    char* src = read_from_stdin();
    printf("%s", src);
    Lexer* lexer = lexer_new(src);
    Grammar* grammar = grammar_new();
    SLRParser* parser = slr_parser_init(grammar, &SLR_TABLE);
    ParserState state;
    do {
        Token* tok = lexer_next_token(lexer);
        state = slr_parser_step(parser, tok);
    } while (state == PARSER_IDLE);
    assert(state == PARSER_ACCEPT);
    destroy_slr_parser(parser);
    destroy_grammar(grammar);
    destroy_lexer(lexer);
    free(src);
}
