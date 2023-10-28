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
    token->lexeme = (char *)malloc(sizeof(char));
    token->lexeme[0] = '\0';
    token->span = NULL;
    token->ty = Eof;
    return token;
}

char *debug_token(Token *token) {
    size_t bufsz = 256;
    char *buf = (char *)malloc(sizeof(char) * bufsz);
    switch (token->ty) {
        case BoolLit: /* Boolean Literal */
            snprintf(buf, bufsz, "BoolLit(%s) at Span (%u, %u)", token->lexeme,
                     token->span->start, token->span->end);
            break;
        case NatLit: /* Natural Number Literal */
            snprintf(buf, bufsz, "NatLit(%s)  at Span (%u, %u)", token->lexeme,
                     token->span->start, token->span->end);
            break;
        case BoolDecl: /* The variable type Boolean */
            snprintf(buf, bufsz, "BoolDecl    at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case NatDecl: /* The variable type Natural */
            snprintf(buf, bufsz, "NatDecl     at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case FuncDecl: /* The variable type Function */
            snprintf(buf, bufsz, "FuncDecl    at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case Comma: /* , */
            snprintf(buf, bufsz, "Token(',')  at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case QuestionMark: /* ? */
            snprintf(buf, bufsz, "Token('?')  at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case Colon: /* : */
            snprintf(buf, bufsz, "Token(':')  at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case Semicolon: /* ; */
            snprintf(buf, bufsz, "Token(';')  at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case LeftParen: /* ( */
            snprintf(buf, bufsz, "Token('(')  at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case RightParen: /* ) */
            snprintf(buf, bufsz, "Token(')')  at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case Plus: /* + */
            snprintf(buf, bufsz, "Token('+')  at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case Ampersand: /* & */
            snprintf(buf, bufsz, "Token('&')  at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case Arrow: /* -> */
            snprintf(buf, bufsz, "Token('->') at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case Less: /* < */
            snprintf(buf, bufsz, "Token('<')  at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case Equal: /* = */
            snprintf(buf, bufsz, "Token('=')  at Span (%u, %u)",
                     token->span->start, token->span->end);
            break;
        case Comment: /* [] */
            snprintf(buf, bufsz, "Comment(%s) at Span (%u, %u)", token->lexeme,
                     token->span->start, token->span->end);
            break;
        case Identifier: /* Identifier */
            snprintf(buf, bufsz, "Ident(%s)   at Span (%u, %u)", token->lexeme,
                     token->span->start, token->span->end);
            break;
        case Eof: /* End of file */
            snprintf(buf, bufsz, "EOF\n");
            break;
        case Invalid: /* Invalid token */
            snprintf(buf, bufsz, "Invalid(%s) at Span (%u, %u)", token->lexeme,
                     token->span->start, token->span->end);
            break;
    }
    return buf;
}

Span *span_new(unsigned start, unsigned end) {
    Span *span = (Span *)malloc(sizeof(Span));
    span->start = start;
    span->end = end;
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
        case ',':
            return Comma;
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
    } else {
        return Identifier;
    }
}

bool need_to_check_dfa(char cur, char peek, bool comment) {
    // never check dfa if not reach the end of comment
    if (cur != ']' && comment) {
        return false;
    }

    if (isalpha(cur)) {
        return !isalpha(peek) && !isdigit(peek);
    }
    if (isdigit(cur)) {
        return !isalpha(peek) && !isdigit(peek);
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

    return lexer;
}

Token *lexer_next_token(Lexer *lexer) {
    log_debug("Getting next token, start from %u", lexer->start);

    if (lexer->start > lexer->len) {
        log_info("EOF reached");
        return eof_token();
    }

    unsigned cursor = lexer->start;
    bool comment = false;
    while (cursor <= lexer->len) {
        switch (lexer->src[cursor]) {
            case '[':
                comment = true;
                break;
            case ']':
                comment = false;
                log_debug("Comment End");
                break;
        }
        char buf[8];
        log_debug("Cursor at src[%u] = [%s]", cursor,
                  pretty_ascii(lexer->src[cursor], buf, 8));
        char peek = cursor + 1 < lexer->len ? lexer->src[cursor + 1] : '\0';
        log_debug("Peek   at src[%u] = [%s]", cursor + 1,
                  pretty_ascii(peek, buf, 8));
        // Don't skip whitespace in comment section
        if (!comment &&
            (isspace(lexer->src[cursor]) || lexer->src[cursor] == '\0')) {
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
            if (need_to_check_dfa(lexer->src[cursor], peek, comment)) {
                log_debug(
                    "DFA checking condition satisfied, check DFA to decide "
                    "whether to accept");

                Token *token = (Token *)malloc(sizeof(Token));
                unsigned token_len = lexer->end - lexer->start + 1;
                char *lexeme = (char *)malloc(sizeof(char) * (token_len + 1));
                token->lexeme =
                    strncpy(lexeme, lexer->src + lexer->start, token_len);
                token->lexeme[token_len] = '\0';
                token->span = span_new(lexer->start, lexer->end);

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
                return token;
            } else {
                lexer->end += 1;
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
