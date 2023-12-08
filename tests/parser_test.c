#include "../parser.h"

#include <stdio.h>

char* read_file(char* path) {
    FILE* file = fopen(path, "r");

    if (file == NULL) {
        printf("Fail to open %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);

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

// check whether the given src satisfied the expectation
bool check_src(char* src, bool expected_pass) {
    Lexer* lexer = lexer_new(src);
    SLRParser* parser = slr_parser_init(grammar_new(), &SLR_TABLE);
    ParserState state;
    do {
        Token* tok = lexer_next_token(lexer);
        if (tok->ty == Invalid) {
            state = PARSER_REJECT;
            break;
        }
        state = slr_parser_step(parser, tok);
    } while (state == PARSER_IDLE);
    destroy_slr_parser(parser);
    destroy_lexer(lexer);
    switch (expected_pass) {
        case true:
            return state == PARSER_ACCEPT;
        case false:
            return state != PARSER_ACCEPT;
        default:
            __builtin_unreachable();
    }
}

int main(int argc, char* argv[]) {
    bool expected_pass;
    char* path = argv[1];
    char* src = read_file(argv[1]);
    // pass
    if (strstr(path, "pass") != NULL) {
        expected_pass = true;
    }
    // fail
    else if (strstr(path, "fail") != NULL) {
        expected_pass = false;
    } else {
        printf("Invalid File Name, Must Suffix with \"pass\" or \"fail\"\n");
        return 1;
    }

    if (check_src(src, expected_pass)) {
        printf("%s [pass]\n", path);
        return 0;
    } else {
        printf("%s [fail]\n", path);
        return 1;
    }
}