#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "symbol_table.h"
#ifdef LOG
#include "log.h"
#endif

char *strcat_copy(const char *str1, const char *str2) {
    unsigned str1_len, str2_len;
    char *new_str;

    /* null check */
    str1_len = strlen(str1);
    str2_len = strlen(str2);

    new_str = malloc(str1_len + str2_len + 1);

    /* null check */
    memcpy(new_str, str1, str1_len);
    memcpy(new_str + str1_len, str2, str2_len + 1);

    return new_str;
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

char *add_suffix(const char *filename, const char *suffix) {
    // Check if the input strings are not NULL
    if (filename == NULL || suffix == NULL) {
        return NULL;
    }

    // Find the position of the last dot in the filename
    const char *dot_pos = strrchr(filename, '.');
    // Check if a dot was found
    if (dot_pos == NULL || *(dot_pos + 1) == '/' || *(dot_pos + 1) == '\\') {
        // If no dot was found, simply append the suffix
        return strcat_copy(filename, suffix);
    } else {
        // If a dot was found, insert the suffix before the dot
        size_t length = dot_pos - filename;
        char *result = (char *)malloc((strlen(filename) + strlen(suffix) + 1) *
                                      sizeof(char));

        if (result == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        strcpy(result, filename);
        strcpy(result + length, suffix);
        strcpy(result + length + strlen(suffix), dot_pos);

        return result;
    }
}

void parse(char *src, Grammar *g, FILE *fp) {
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
        slr_parser_display_trace(parser, fp);
        printf("Parsing Accomplished! No syntax error!\n");
    } else {
        // fail
        slr_parser_display_trace(parser, fp);
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
    char *output_file = add_suffix(filename, "_out");
    char *src = read_to_string(filename);
    FILE *fp = fopen(output_file, "w");
    Grammar *grammar = grammar_new();

    if (src != NULL) {
        parse(src, grammar, fp);
        free(src);
    }

    fclose(fp);
    destroy_grammar(grammar);
    free(output_file);
    return 0;
}
