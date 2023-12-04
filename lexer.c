#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef LOG
#include "log.h"
#else
// If log is not enable, just ignore the macro call
#define log_trace(...)
#define log_debug(...)
#define log_info(...)
#define log_warn(...)
#define log_error(...)
#define log_fatal(...)
#endif

typedef enum LexerState {
    LEXER_STATE_COMMENT,
    LEXER_STATE_IDENTIFIER,
    LEXER_STATE_NUMBER,
    LEXER_STATE_PENDING,
} LexerState;

void dfa_reset(DFA *dfa) { dfa->state = dfa->start; };

void dfa_next(DFA *dfa, char c) {
    dfa->state = dfa->table[(uint8_t)c][dfa->state];
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
    token->lexeme = (char *)malloc(sizeof(char));
    token->lexeme[0] = '\0';
    token->span = NULL;
    token->ty = Eof;
    return token;
}

int debug_token(Token *token, char *buf, size_t bufsz) {
    switch (token->ty) {
        case Literal: /* Boolean Literal */
            return snprintf(buf, bufsz, "Literal(%s)", token->lexeme);

        case BoolDecl: /* The variable type Boolean */
            return snprintf(buf, bufsz, "BoolDecl");

        case NatDecl: /* The variable type Natural */
            return snprintf(buf, bufsz, "NatDecl");

        case FuncDecl: /* The variable type Function */
            return snprintf(buf, bufsz, "FuncDecl");

        case QuestionMark: /* ? */
            return snprintf(buf, bufsz, "Token('?')");

        case Colon: /* : */
            return snprintf(buf, bufsz, "Token(':')");

        case Semicolon: /* ; */
            return snprintf(buf, bufsz, "Token(';')");

        case LeftParen: /* ( */
            return snprintf(buf, bufsz, "Token('(')");

        case RightParen: /* ) */
            return snprintf(buf, bufsz, "Token(')')");

        case Plus: /* + */
            return snprintf(buf, bufsz, "Token('+')");

        case Ampersand: /* & */
            return snprintf(buf, bufsz, "Token('&')");

        case Arrow: /* -> */
            return snprintf(buf, bufsz, "Token('->')");

        case Less: /* < */
            return snprintf(buf, bufsz, "Token('<')");

        case Equal: /* = */
            return snprintf(buf, bufsz, "Token('=')");

        case Comment: /* [] */
            return snprintf(buf, bufsz, "Comment(%s)", token->lexeme);

        case Identifier: /* Identifier */
            return snprintf(buf, bufsz, "Ident(%s)", token->lexeme);

        case Eof: /* End of file */
            return snprintf(buf, bufsz, "EOF\n");

        case Invalid: /* Invalid token */
            return snprintf(buf, bufsz, "Invalid(%s)", token->lexeme);
    }
}

Span *span_new(unsigned start, unsigned end, unsigned line, unsigned column) {
    Span *span = (Span *)malloc(sizeof(Span));
    span->start = start;
    span->end = end;
    span->line = line;
    span->column = column;
    return span;
}

void destroy_span(Span *span) {
    if (span == NULL) {
        return;
    }
    free(span);
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

    if (strcmp(lexeme, "fun") == 0) {
        return FuncDecl;
    }

    if (strcmp(lexeme, "T") == 0 || strcmp(lexeme, "F") == 0) {
        return Literal;
    }

    if (strcmp(lexeme, "bool") == 0) {
        return BoolDecl;
    }

    if (strcmp(lexeme, "nat") == 0) {
        return NatDecl;
    }

    if (isdigit(lexeme[0])) {
        return Literal;
    } else {
        return Identifier;
    }
}

bool need_to_check_dfa(char cur, char peek, LexerState state) {
    // never check dfa if not reach the end of comment
    if (cur != ']' && state == LEXER_STATE_COMMENT) {
        return false;
    }

    if (isalpha(cur)) {
        return !isalpha(peek) && !isdigit(peek);
    }
    if (isdigit(cur)) {
        switch (state) {
            case LEXER_STATE_IDENTIFIER:
                return !isalpha(peek) && !isdigit(peek);
            case LEXER_STATE_NUMBER:
                return !isdigit(peek);
            default:
                log_fatal("unexpected state reached");
                break;
        }
    }

    if (cur == '-' && peek == '>') {
        return false;
    }
    return true;
}

char *pretty_ascii(char ascii, char *buf, size_t bufsz) {
    if (ascii >= '!') {
        snprintf(buf, bufsz, "%c", ascii);
    } else if (isspace(ascii) || ascii == '\0') {
        switch (ascii) {
            case 0x0:
                snprintf(buf, bufsz, "\\0");  // NUL char
                break;
            case 0x20:
                snprintf(buf, bufsz, "space");  // Space
                break;
            case 0x0c:
                snprintf(buf, bufsz, "\\f");  // Form feed
                break;
            case 0x0a:
                snprintf(buf, bufsz, "\\n");  // Line feed
                break;
            case 0x0d:
                snprintf(buf, bufsz, "\\r");  // Carriage return
                break;
            case 0x09:
                snprintf(buf, bufsz, "\\t");  // Horizontal tab
                break;
            case 0x0b:
                snprintf(buf, bufsz, "\\v");  // Vertical tab
                break;
        }
    } else {
        snprintf(buf, bufsz, "%u", (uint8_t)ascii);
    }
    return buf;
}

Lexer *lexer_new(const char *src) {
    Lexer *lexer = malloc(sizeof(Lexer));

    lexer->dfa = &LEXER_DFA;
    lexer->start = 0;
    lexer->end = 0;
    lexer->src = src;
    lexer->len = strlen(src);
    lexer->line = 1;
    lexer->column = 1;

    return lexer;
}

Token *lexer_next_token(Lexer *lexer) {
    log_debug("Getting next token, start from %u", lexer->start);

    if (lexer->start > lexer->len) {
        log_info("EOF reached");
        return eof_token();
    }

    unsigned cursor = lexer->start;
    LexerState state = LEXER_STATE_PENDING;
    while (cursor <= lexer->len) {
        if (state == LEXER_STATE_PENDING) {
            if (lexer->src[cursor] == '[') {
                state = LEXER_STATE_COMMENT;
            }

            if (isdigit(lexer->src[cursor])) {
                state = LEXER_STATE_NUMBER;
            }

            if (isalpha(lexer->src[cursor])) {
                state = LEXER_STATE_IDENTIFIER;
            }
        }
        char buf[8];
        log_debug("Cursor at src[%u] = [%s]", cursor,
                  pretty_ascii(lexer->src[cursor], buf, 8));
        char peek = cursor + 1 < lexer->len ? lexer->src[cursor + 1] : '\0';
        log_debug("Peek   at src[%u] = [%s]", cursor + 1,
                  pretty_ascii(peek, buf, 8));
        // Don't skip whitespace in comment section
        if (state != LEXER_STATE_COMMENT &&
            (isspace(lexer->src[cursor]) || lexer->src[cursor] == '\0')) {
            // Bump the lineno
            if (lexer->src[cursor] == '\n') {
                lexer->line += 1;
                lexer->column = 1;
                log_debug("Column Reset to 1");
            } else {
                lexer->column += 1;
                log_debug("Column Bumped");
            }
            log_debug("Whitespace detected at %u, Skip", cursor);
            cursor += 1;
            // reset span
            lexer->start = cursor;
            lexer->end = cursor;
        } else {
            // feed the current char to dfa
            dfa_next(lexer->dfa, lexer->src[cursor]);
            // if next char is whitespace or '\0'
            // which indicates current token is done
            if (need_to_check_dfa(lexer->src[cursor], peek, state)) {
                log_debug(
                    "DFA checking condition satisfied, check DFA to decide "
                    "whether to accept");

                Token *token = (Token *)malloc(sizeof(Token));
                unsigned token_len = lexer->end - lexer->start + 1;
                char *lexeme = (char *)malloc(sizeof(char) * (token_len + 1));
                token->lexeme =
                    strncpy(lexeme, lexer->src + lexer->start, token_len);
                token->lexeme[token_len] = '\0';
                token->span =
                    span_new(lexer->start, lexer->end, lexer->line,
                             lexer->column - (lexer->end - lexer->start));

                if (dfa_is_accept(lexer->dfa)) {
                    token->ty = get_token_type(token->lexeme);
                    log_debug("accept:「%s」 => span(%u, %u)", token->lexeme,
                              lexer->start, lexer->end);
                } else {
                    token->ty = Invalid;
                    log_warn("The lexeme is rejected by the DFA");
                }
                dfa_reset(lexer->dfa);
                lexer->start = lexer->end + 1;
                lexer->end = lexer->start;
                lexer->column += 1;
                log_debug("Column Bumped");
                return token;
            } else {
                lexer->end += 1;
                lexer->column += 1;
                log_debug("Column Bumped");
            }

            cursor += 1;
        }
    }

    return eof_token();
}

void destroy_token(Token *token) {
    destroy_span(token->span);
    free(token->lexeme);
    free(token);
}

void destroy_lexer(Lexer *lexer) { free(lexer); }
