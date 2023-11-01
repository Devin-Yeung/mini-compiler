#include "../lexer.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../log.h"

char* mutable_str(char* s) {
    char* mutable_s = (char*)malloc(strlen(s) + 1);
    strcpy(mutable_s, s);
    return mutable_s;
}

void parse(char* s) {
    char* mut = mutable_str(s);
    Lexer* lexer = lexer_new(mut);
    while (true) {
        Token* next = lexer_next_token(lexer);
        if (next->ty == Eof) {
            printf("EOF!\n");
            destroy_token(next);
            break;
        } else {
            char buf[256];
            debug_token(next, buf, sizeof(buf));
            printf("Receive: %s\n", buf);
        }
        destroy_token(next);
    }
    destroy_lexer(lexer);
    free(mut);
}

int main() { parse("abc   def   func  1234   a"); }