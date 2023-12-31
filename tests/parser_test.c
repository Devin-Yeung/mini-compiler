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
bool check_src(char* src, Grammar* g, bool expected_pass) {
    Lexer* lexer = lexer_new(src);
    SLRParser* parser = slr_parser_init(g, &SLR_TABLE);
    ParserState state;
    do {
        Token* tok = lexer_next_token(lexer);
        if (tok->ty == Comment) {
            destroy_token(tok);
            continue;
        }
        if (tok->ty == Invalid) {
            destroy_token(tok);
            state = PARSER_REJECT;
            break;
        }
        state = slr_parser_step(parser, tok);
    } while (state == PARSER_IDLE);
    ParseTree* tree = slr_parser_parse_tree(parser);
    bool tree_check = (tree->root->children != NULL &&
                       cc_deque_size(tree->root->children) == 1);
    destroy_parse_tree(tree);
    destroy_slr_parser(parser);
    destroy_lexer(lexer);
    switch (expected_pass) {
        case true:
            return state == PARSER_ACCEPT && tree_check;
        case false:
            return state != PARSER_ACCEPT;
        default:
            __builtin_unreachable();
    }
}

int main(int argc, char* argv[]) {
    Grammar* g = grammar_new();
    int ret_code = 0;
    for (int i = 1; i < argc; i++) {
        char* path = argv[i];
        bool expected_pass;
        printf("Testing %s => ", path);
        fflush(stdout);
        char* src = read_file(argv[i]);
        // pass
        if (strstr(path, "pass") != NULL) {
            expected_pass = true;
        }
        // fail
        else if (strstr(path, "fail") != NULL) {
            expected_pass = false;
        } else {
            printf(
                "Invalid File Name, Must Suffix with \"pass\" or \"fail\"\n");
            ret_code |= 1;
            break;
        }

        if (check_src(src, g, expected_pass)) {
            printf("[pass]\n");
            ret_code |= 0;
        } else {
            printf("[fail]\n");
            ret_code |= 1;
        }
        free(src);
    }
    destroy_grammar(g);
    return ret_code;
}