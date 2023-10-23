#include "../lexer.h"
#include <string.h>
#include <stdlib.h>

char *mutable_str(char* s) {
    char* mutable_s = (char*)malloc(strlen(s) + 1);
    strcpy(mutable_s, s);
    return mutable_s;
}

void parse(char* s) {
    char* mut = mutable_str(s);
    Lexer *lexer = lexer_new(mut);
    lexer_next_token(lexer);
}

int main() {
    parse("abc   def   func  1234");
}