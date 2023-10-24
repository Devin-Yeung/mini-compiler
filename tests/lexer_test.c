#include "../lexer.h"
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
            break;
        } else {
            printf("Receive token: %s\n", next->lexeme);
        }
    }
}

int main() {
    parse("abc   def   func  1234   ");
}