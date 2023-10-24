#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dfa_reset(DFA *dfa) { dfa->state = dfa->start; };

void dfa_next(DFA *dfa, char c) {
    dfa->state = dfa->table[(int)c][dfa->state];
};

bool dfa_is_accept(const DFA *dfa) {
    bool accept = false;
    for (unsigned i = 0; i < dfa->n_accepts; i++) {
        accept |= (dfa->state == dfa->accepts[i]);
    }
    return accept;
};

bool dfa_matches(DFA *dfa, const char *s) {
    unsigned cursor = 0;
    while (s[cursor] != '\0') {
        dfa_next(dfa, s[cursor++]);
    }

    bool accept = dfa_is_accept(dfa);
    dfa_reset(dfa);
    return accept;
};

Token *eof_token() {
    Token *token = (Token *)malloc(sizeof(Token));
    token->lexeme = "";  // TODO: should prevent this from being freed
    token->span = NULL;
    token->ty = Eof;
    return token;
}

Span *span_new(unsigned start, unsigned end) {
    Span *span = (Span *)malloc(sizeof(Span));
    span->start = start;
    span->end = end;
    return span;
}

TokenTy get_token_type(char *lexeme) {
    switch (lexeme[0]) {
        case '?':
            return QuestionMark;
        case ':':
            return Colon;
        case ';':
            return Semicolon;
        case '(':
            return LeftParen;
        case ')':
            return RightParen;
        case '+':
            return Plus;
        case '&':
            return Ampersand;
        case '-': /* -> is the only valid token start with - sign */
            return Arrow;
        case '<':
            return Less;
        case '=':
            return Equal;
        case '[':
            return Comment;
    }

    if (strcmp(lexeme, "func") == 0) {
        return FuncDecl;
    }

    if (strcmp(lexeme, "T") == 0 || strcmp(lexeme, "F") == 0) {
        return BoolLit;
    }

    if (strcmp(lexeme, "bool") == 0) {
        return BoolDecl;
    }

    if (strcmp(lexeme, "nat") == 0) {
        return NatDecl;
    }

    if (isdigit(lexeme[0])) {
        return NatLit;
    }
    else {
        return Identifier;
    }
}

Lexer *lexer_new(char *src) {
    Lexer *lexer = malloc(sizeof(Lexer));

    lexer->dfa = &LEXER_DFA;
    lexer->start = 0;
    lexer->end = 0;
    lexer->src = src;
    lexer->len = strlen(src);

    return lexer;
}

Token *lexer_next_token(Lexer *lexer) {
    printf("Getting next token, start from %u\n", lexer->start);

    if (lexer->start > lexer->len) {
        printf("EOF reached\n");
        return eof_token();
    }

    unsigned cursor = lexer->start;
    while (cursor <= lexer->len) {
        printf("Cursor at src[%u] = [%c]\n", cursor, lexer->src[cursor]);
        char peek = cursor + 1 < lexer->len ? lexer->src[cursor + 1] : '\0';
        printf("Peek   at src[%u] = [%c]\n", cursor + 1, peek);
        if (isspace(lexer->src[cursor]) || lexer->src[cursor] == '\0') {
            printf("Whitespace detected, Skip\n");
            cursor += 1;
            // reset span
            lexer->start = cursor;
            lexer->end = cursor;
        } else {
            // feed the current char to dfa
            dfa_next(lexer->dfa, lexer->src[cursor]);
            // if next char is whitespace or '\0'
            // which indicates current token is done
            if (isspace(peek) || peek == '\0') {
                printf("Whitespace peeked, check whether to accept\n");
                if (dfa_is_accept(lexer->dfa)) {
                    // trick, set whitespace to '\0' to slice the string
                    lexer->src[cursor + 1] = '\0';
                    printf("DFA accepted, set src[%u] to \\0\n", cursor + 1);
                    printf("accept:「%s」 => span(%u, %u)\n",
                           lexer->src + lexer->start, lexer->start, lexer->end);
                    dfa_reset(lexer->dfa);

                    Token *token = (Token *)malloc(sizeof(Token));
                    token->lexeme = lexer->src + lexer->start;
                    token->span = span_new(lexer->start, lexer->end);
                    token->ty = get_token_type(token->lexeme);

                    lexer->start = lexer->end + 1;
                    lexer->end = lexer->start;

                    return token;
                }
            } else {
                lexer->end += 1;
            }

            cursor += 1;
        }
    }

    return eof_token();
}
