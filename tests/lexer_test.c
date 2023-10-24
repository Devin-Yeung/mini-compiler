#include "../lexer.h"
#include "../slog.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

char *mutable_str(char* s) {
    char* mutable_s = (char*)malloc(strlen(s) + 1);
    strcpy(mutable_s, s);
    return mutable_s;
}

void parse(char* s) {
    char* mut = mutable_str(s);
    Lexer *lexer = lexer_new(mut);
    while (true) {
        Token* next = lexer_next_token(lexer);
        if (next->ty == Eof) {
            printf("EOF!\n");
            destroy_token(next);
            break;
        } else {
            char* info = debug_token(next);
            printf("Receive: %s\n", info);
            free(info);
        }
        destroy_token(next);
    }
    destroy_lexer(lexer);
    free(mut);
}

int main() {
    slog_init(NULL, SLOG_FLAGS_ALL, 1);
    parse("abc   def   func  1234   ");
    slog_destroy();
}