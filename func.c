#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "symbol_table.h"
#ifdef LOG
#include "log.h"
#endif

typedef struct Position {
    unsigned line;
    unsigned col;
} Position;

/**
 * Convert Span representation to Position representation
 * SAFETY: immutable borrow to the span
 * @param span span information
 * @param src source code
 * @return
 */
Position *span_to_position(const Span *span, const char *src) {
    Position *pos = (Position *)malloc(sizeof(Position));
    pos->line = 1;
    pos->col = 1;
    for (unsigned i = 0; i < span->start; i++) {
        if (src[i] == '\n') {
            pos->line++;
            pos->col = 1;
        } else {
            pos->col++;
        }
    }
    return pos;
}

int debug_position(const Position *pos, char *buf, size_t bufsz) {
    return snprintf(buf, bufsz, "line: %u, column %u", pos->line, pos->col);
}

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

void lexical_parse(char *s) {
    Lexer *lexer = lexer_new(s);
    SymbolTable *tab = symbol_table_new();
    while (true) {
        Token *next = lexer_next_token(lexer);
        if (next->ty == Eof) {
            printf("EOF Reached, Lexical Analysis Finished.\n");
            destroy_token(next);
            break;
        } else {
            if (next->ty == Identifier) {
                symbol_table_insert(
                    tab, next->lexeme,
                    FuncTy);  // TODO: symbol ty is known in syntactic analysis
            }
            char buf[256];
            char *ptr = buf;
            ptr += debug_token(next, ptr, sizeof(buf));
            ptr += snprintf(ptr, sizeof(buf), " at ");
            ptr += debug_position(span_to_position(next->span, s), ptr,
                                  sizeof(buf));
            printf("==> %s\n", buf);
        }
        destroy_token(next);
    }
    printf("\n=== Symbol table: ===\n");
    symbol_table_walk(tab);
    symbol_table_destroy(tab);
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

    if (src != NULL) {
        lexical_parse(src);
        free(src);
    }

    return 0;
}