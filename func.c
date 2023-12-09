#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "symbol_table.h"
#ifdef LOG
#include "log.h"
#endif

char *read_to_string(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Fail to open %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *)malloc(file_size + 1);

    if (buffer == NULL) {
        fclose(file);
        printf("Memory not enough\n");
        return NULL;
    }

    fread(buffer, file_size, 1, file);
    buffer[file_size] = '\0';

    fclose(file);

    return buffer;
}

void parse(char *src, Grammar *g) {
    Lexer *lexer = lexer_new(src);
    SLRParser *parser = slr_parser_init(g, &SLR_TABLE);
    ParserState state;

    unsigned line;
    unsigned col;

    do {
        Token *tok = lexer_next_token(lexer);
        line = tok->span->line;
        col = tok->span->column;
        if (tok->ty == Comment) {
            destroy_token(tok);
            continue;
        }
        if (tok->ty == Invalid) {
            printf("Invalid token at Line %d Column %d\n", line, col);
            destroy_token(tok);
            break;
        }
        state = slr_parser_step(parser, tok);
    } while (state == PARSER_IDLE);
    if (state == PARSER_ACCEPT) {
        // success
        slr_parser_display_trace(parser);
        printf("Parsing Accomplished! No syntax error!\n");
    } else {
        // fail
        slr_parser_display_trace(parser);
        printf("Syntax error at Line %d Column %d\n", line, col);
    }
    destroy_slr_parser(parser);
    destroy_lexer(lexer);
}

int main(int argc, char *argv[]) {
#ifdef LOG
    bool enable_log =
        getenv("LOG") != NULL && (strcmp(getenv("LOG"), "1") == 0);

    if (!enable_log) {
        log_set_quiet(true);
    }
#endif
    const char *filename = argv[1];
    char *src = read_to_string(filename);
    Grammar *grammar = grammar_new();

    if (src != NULL) {
        parse(src, grammar);
        free(src);
    }

    destroy_grammar(grammar);
    return 0;
}